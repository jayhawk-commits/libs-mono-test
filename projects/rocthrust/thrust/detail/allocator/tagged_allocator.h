/*
 *  Copyright 2008-2013 NVIDIA Corporation
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

#pragma once

#include <thrust/detail/config.h>
#include <thrust/detail/type_traits/pointer_traits.h>
#include <thrust/iterator/iterator_traits.h>

THRUST_NAMESPACE_BEGIN
namespace detail
{

template<typename T, typename Tag, typename Pointer> class tagged_allocator;

template<typename Tag, typename Pointer>
  class tagged_allocator<void, Tag, Pointer>
{
  public:
    using value_type      = void;
    using pointer         = typename thrust::detail::pointer_traits<Pointer>::template rebind<void>::other;
    using const_pointer   = typename thrust::detail::pointer_traits<Pointer>::template rebind<const void>::other;
    using size_type       = std::size_t;
    using difference_type = typename thrust::detail::pointer_traits<Pointer>::difference_type;
    using system_type     = Tag;

    template<typename U>
      struct rebind
    {
      using other = tagged_allocator<U,Tag,Pointer>;
    }; // end rebind
};

template<typename T, typename Tag, typename Pointer>
  class tagged_allocator
{
  public:
    using value_type      = T;
    using pointer         = typename thrust::detail::pointer_traits<Pointer>::template rebind<T>::other;
    using const_pointer   = typename thrust::detail::pointer_traits<Pointer>::template rebind<const T>::other;
    using reference       = typename thrust::iterator_reference<pointer>::type;
    using const_reference = typename thrust::iterator_reference<const_pointer>::type;
    using size_type       = std::size_t;
    using difference_type = typename thrust::detail::pointer_traits<pointer>::difference_type;
    using system_type     = Tag;

    template<typename U>
      struct rebind
    {
      using other = tagged_allocator<U,Tag,Pointer>;
    }; // end rebind

    THRUST_HOST_DEVICE
    inline tagged_allocator();

    THRUST_HOST_DEVICE
    inline tagged_allocator(const tagged_allocator &);

    template<typename U, typename OtherPointer>
    THRUST_HOST_DEVICE
    inline tagged_allocator(const tagged_allocator<U, Tag, OtherPointer> &);

    THRUST_HOST_DEVICE
    inline ~tagged_allocator();

    THRUST_HOST_DEVICE
    pointer address(reference x) const;

    THRUST_HOST_DEVICE
    const_pointer address(const_reference x) const;

    size_type max_size() const;
};

template<typename T1, typename Pointer1, typename T2, typename Pointer2, typename Tag>
THRUST_HOST_DEVICE
bool operator==(const tagged_allocator<T1,Pointer1,Tag> &, const tagged_allocator<T2,Pointer2,Tag> &);

template<typename T1, typename Pointer1, typename T2, typename Pointer2, typename Tag>
THRUST_HOST_DEVICE
bool operator!=(const tagged_allocator<T1,Pointer1,Tag> &, const tagged_allocator<T2,Pointer2,Tag> &);

} // end detail
THRUST_NAMESPACE_END

#include <thrust/detail/allocator/tagged_allocator.inl>
