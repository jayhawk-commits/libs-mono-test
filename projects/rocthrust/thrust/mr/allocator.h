/*
 *  Copyright 2018 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*! \file
 *  \brief Allocator types usable with \ref memory_resources.
 */

#pragma once

#include <limits>

#include <thrust/detail/config.h>
#include <thrust/detail/config/memory_resource.h>
#include <thrust/detail/type_traits/pointer_traits.h>

#include <thrust/mr/validator.h>
#include <thrust/mr/polymorphic_adaptor.h>

THRUST_NAMESPACE_BEGIN
namespace mr
{

/*! \addtogroup allocators Allocators
 *  \ingroup memory_management
 *  \{
 */

/*! An \p mr::allocator is a template that fulfills the C++ requirements for Allocators,
 *  allowing to use the NPA-based memory resources where an Allocator is required. Unlike
 *  memory resources, but like other allocators, \p mr::allocator is typed and bound to
 *  allocate object of a specific type, however it can be freely rebound to other types.
 *
 *  \tparam T the type that will be allocated by this allocator.
 *  \tparam MR the upstream memory resource to use for memory allocation. Must derive from
 *      \p thrust::mr::memory_resource and must be \p final (in C++11 and beyond).
 */
template<typename T, class MR>
class allocator : private validator<MR>
{
public:
    /*! The pointer to void type of this allocator. */
    using void_pointer = typename MR::pointer;

    /*! The value type allocated by this allocator. Equivalent to \p T. */
    using value_type = T;
    /*! The pointer type allocated by this allocator. Equivaled to the pointer type of \p MR rebound to \p T. */
    using pointer = typename thrust::detail::pointer_traits<void_pointer>::template rebind<T>::other;
    /*! The pointer to const type. Equivalent to a pointer type of \p MR rebound to <tt>const T</tt>. */
    using const_pointer = typename thrust::detail::pointer_traits<void_pointer>::template rebind<const T>::other;
    /*! The reference to the type allocated by this allocator. Supports smart references. */
    using reference = typename thrust::detail::pointer_traits<pointer>::reference;
    /*! The const reference to the type allocated by this allocator. Supports smart references. */
    using const_reference = typename thrust::detail::pointer_traits<const_pointer>::reference;
    /*! The size type of this allocator. Always \p std::size_t. */
    using size_type = std::size_t;
    /*! The difference type between pointers allocated by this allocator. */
    using difference_type = typename thrust::detail::pointer_traits<pointer>::difference_type;

    /*! Specifies that the allocator shall be propagated on container copy assignment. */
    using propagate_on_container_copy_assignment = detail::true_type;
    /*! Specifies that the allocator shall be propagated on container move assignment. */
    using propagate_on_container_move_assignment = detail::true_type;
    /*! Specifies that the allocator shall be propagated on container swap. */
    using propagate_on_container_swap = detail::true_type;

    /*! The \p rebind metafunction provides the type of an \p allocator instantiated with another type.
     *
     *  \tparam U the other type to use for instantiation.
     */
    template<typename U>
    struct rebind
    {
        /*! The typedef \p other gives the type of the rebound \p allocator.
         */
        using other = allocator<U, MR>;
    };

    /*! Calculates the maximum number of elements allocated by this allocator.
     *
     *  \return the maximum value of \p std::size_t, divided by the size of \p T.
     */
    THRUST_EXEC_CHECK_DISABLE
    THRUST_HOST_DEVICE
    size_type max_size() const
    {
        return (std::numeric_limits<size_type>::max)() / sizeof(T);
    }

    /*! Constructor.
     *
     *  \param resource the resource to be used to allocate raw memory.
     */
    THRUST_HOST_DEVICE
    allocator(MR * resource) : mem_res(resource)
    {
    }

    /*! Copy constructor. Copies the resource pointer. */
    template<typename U>
    THRUST_HOST_DEVICE
    allocator(const allocator<U, MR> & other) : mem_res(other.resource())
    {
    }

    /*! Allocates objects of type \p T.
     *
     *  \param n number of elements to allocate
     *  \return a pointer to the newly allocated storage.
     */
    THRUST_NODISCARD
    THRUST_HOST
    pointer allocate(size_type n)
    {
        return static_cast<pointer>(mem_res->do_allocate(n * sizeof(T), alignof(T)));
    }

    /*! Deallocates objects of type \p T.
     *
     *  \param p pointer returned by a previous call to \p allocate
     *  \param n number of elements, passed as an argument to the \p allocate call that produced \p p
     */
    THRUST_HOST
    void deallocate(pointer p, size_type n)
    {
        return mem_res->do_deallocate(p, n * sizeof(T), alignof(T));
    }

    /*! Extracts the memory resource used by this allocator.
     *
     *  \return the memory resource used by this allocator.
     */
    THRUST_HOST_DEVICE
    MR * resource() const
    {
        return mem_res;
    }

private:
    MR * mem_res;
};

/*! Compares the allocators for equality by comparing the underlying memory resources. */
template<typename T, typename MR>
THRUST_HOST_DEVICE
bool operator==(const allocator<T, MR> & lhs, const allocator<T, MR> & rhs) noexcept
{
    return *lhs.resource() == *rhs.resource();
}

/*! Compares the allocators for inequality by comparing the underlying memory resources. */
template<typename T, typename MR>
THRUST_HOST_DEVICE
bool operator!=(const allocator<T, MR> & lhs, const allocator<T, MR> & rhs) noexcept
{
    return !(lhs == rhs);
}


/*! An allocator whose memory resource we can dynamically configure at runtime.
 *
 * \tparam T the type that will be allocated by this allocator
 * \tparam Pointer the pointer type that will be used to create the memory resource
 */
template<typename T, typename Pointer>
using polymorphic_allocator = allocator<T, polymorphic_adaptor_resource<Pointer> >;



/*! A helper allocator class that uses global instances of a given upstream memory resource. Requires the memory resource
 *      to be default constructible.
 *
 *  \tparam T the type that will be allocated by this allocator.
 *  \tparam Upstream the upstream memory resource to use for memory allocation. Must derive from
 *      \p thrust::mr::memory_resource and must be \p final (in C++11 and beyond).
 */
template<typename T, typename Upstream>
class stateless_resource_allocator : public thrust::mr::allocator<T, Upstream>
{
    using base = thrust::mr::allocator<T, Upstream>;

public:
    /*! The \p rebind metafunction provides the type of an \p stateless_resource_allocator instantiated with another type.
     *
     *  \tparam U the other type to use for instantiation.
     */
    template<typename U>
    struct rebind
    {
        /*! The typedef \p other gives the type of the rebound \p stateless_resource_allocator.
         */
        using other = stateless_resource_allocator<U, Upstream>;
    };

    /*! Default constructor. Uses \p get_global_resource to get the global instance of \p Upstream and initializes the
     *      \p allocator base subobject with that resource.
     */
    THRUST_EXEC_CHECK_DISABLE
    THRUST_HOST_DEVICE
    stateless_resource_allocator() : base(get_global_resource<Upstream>())
    {
    }

    /*! Copy constructor. Copies the memory resource pointer. */
    THRUST_HOST_DEVICE
    stateless_resource_allocator(const stateless_resource_allocator & other)
        : base(other) {}

    /*! Conversion constructor from an allocator of a different type. Copies the memory resource pointer. */
    template<typename U>
    THRUST_HOST_DEVICE
    stateless_resource_allocator(const stateless_resource_allocator<U, Upstream> & other)
        : base(other) {}

    /*! Default copy assignment operator. */
    stateless_resource_allocator & operator=(const stateless_resource_allocator &) = default;

    /*! Destructor. */
    THRUST_HOST_DEVICE
    ~stateless_resource_allocator() {}
};

/*! \} // allocators
 */

} // end mr
THRUST_NAMESPACE_END
