/*
 *  Copyright 2008-2021 NVIDIA Corporation
 *  Modifications Copyright© 2019-2025 Advanced Micro Devices, Inc. All rights reserved.
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

#include <thrust/detail/pointer.h>
#include <thrust/detail/type_traits.h>

THRUST_NAMESPACE_BEGIN

template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  pointer<Element,Tag,Reference,Derived>
    ::pointer()
      : super_t(static_cast<Element*>(nullptr))
{} // end pointer::pointer


template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  pointer<Element,Tag,Reference,Derived>
    ::pointer(std::nullptr_t)
      : super_t(static_cast<Element*>(nullptr))
{} // end pointer::pointer


template<typename Element, typename Tag, typename Reference, typename Derived>
  template<typename OtherElement>
    THRUST_HOST_DEVICE
    pointer<Element,Tag,Reference,Derived>
      ::pointer(OtherElement *other)
        : super_t(other)
{} // end pointer::pointer

// Fixes hipcc linkage error
template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  pointer<Element,Tag,Reference,Derived>
    ::pointer(Element* ptr)
      : super_t(ptr)
{} // end pointer::pointer

template<typename Element, typename Tag, typename Reference, typename Derived>
  template<typename OtherPointer>
    THRUST_HOST_DEVICE
    pointer<Element,Tag,Reference,Derived>
      ::pointer(const OtherPointer &other,
                typename thrust::detail::enable_if_pointer_is_convertible<
                  OtherPointer,
                  pointer<Element,Tag,Reference,Derived>
                 >::type *)
        : super_t(thrust::detail::pointer_traits<OtherPointer>::get(other))
{} // end pointer::pointer


template<typename Element, typename Tag, typename Reference, typename Derived>
  template<typename OtherPointer>
    THRUST_HOST_DEVICE
    pointer<Element,Tag,Reference,Derived>
      ::pointer(const OtherPointer &other,
                typename thrust::detail::enable_if_void_pointer_is_system_convertible<
                  OtherPointer,
                  pointer<Element,Tag,Reference,Derived>
                 >::type *)
        : super_t(static_cast<Element *>(thrust::detail::pointer_traits<OtherPointer>::get(other)))
{} // end pointer::pointer


template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  typename pointer<Element,Tag,Reference,Derived>::derived_type &
    pointer<Element,Tag,Reference,Derived>
      ::operator=(decltype(nullptr))
{
  super_t::base_reference() = nullptr;
  return static_cast<derived_type&>(*this);
} // end pointer::operator=


template<typename Element, typename Tag, typename Reference, typename Derived>
  template<typename OtherPointer>
    THRUST_HOST_DEVICE
    typename thrust::detail::enable_if_pointer_is_convertible<
      OtherPointer,
      pointer<Element,Tag,Reference,Derived>,
      typename pointer<Element,Tag,Reference,Derived>::derived_type &
    >::type
      pointer<Element,Tag,Reference,Derived>
        ::operator=(const OtherPointer &other)
{
  super_t::base_reference() = thrust::detail::pointer_traits<OtherPointer>::get(other);
  return static_cast<derived_type&>(*this);
} // end pointer::operator=

namespace detail
{

// Implementation for dereference() when Reference is Element&,
// e.g. cuda's managed_memory_pointer
template <typename Reference, typename Derived>
THRUST_HOST_DEVICE
Reference pointer_dereference_impl(const Derived& ptr,
                                   thrust::detail::true_type /* is_cpp_ref */)
{
  return *ptr.get();
}

// Implementation for pointers with proxy references:
template <typename Reference, typename Derived>
THRUST_HOST_DEVICE
Reference pointer_dereference_impl(const Derived& ptr,
                                   thrust::detail::false_type /* is_cpp_ref */)
{
  return Reference(ptr);
}

} // namespace detail


template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  typename pointer<Element,Tag,Reference,Derived>::super_t::reference
  pointer<Element,Tag,Reference,Derived>
    ::dereference() const
{
  // Need to handle cpp refs and fancy refs differently:
  using RefT     = typename super_t::reference;
  using IsCppRef = typename thrust::detail::is_reference<RefT>::type;

  const derived_type& derivedPtr = static_cast<const derived_type&>(*this);

  return detail::pointer_dereference_impl<RefT>(derivedPtr, IsCppRef());
} // end pointer::dereference



template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  Element *pointer<Element,Tag,Reference,Derived>
    ::get() const
{
  return super_t::base();
} // end pointer::get


template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  Element *pointer<Element,Tag,Reference,Derived>
    ::operator->() const
{
  return super_t::base();
} // end pointer::operator->


template<typename Element, typename Tag, typename Reference, typename Derived>
  THRUST_HOST_DEVICE
  pointer<Element,Tag,Reference,Derived>
    ::operator bool() const
{
  return bool(get());
} // end pointer::operator bool


template<typename Element, typename Tag, typename Reference, typename Derived,
         typename charT, typename traits>
THRUST_HOST
std::basic_ostream<charT, traits> &
operator<<(std::basic_ostream<charT, traits> &os,
           const pointer<Element, Tag, Reference, Derived> &p) {
  return os << p.get();
}

// NOTE: These are needed so that Thrust smart pointers work with
// `std::unique_ptr`.
template <typename Element, typename Tag, typename Reference, typename Derived>
THRUST_HOST_DEVICE
bool operator==(std::nullptr_t, pointer<Element, Tag, Reference, Derived> p)
{
  return nullptr == p.get();
}

template <typename Element, typename Tag, typename Reference, typename Derived>
THRUST_HOST_DEVICE
bool operator==(pointer<Element, Tag, Reference, Derived> p, std::nullptr_t)
{
  return nullptr == p.get();
}

template <typename Element, typename Tag, typename Reference, typename Derived>
THRUST_HOST_DEVICE
bool operator!=(std::nullptr_t, pointer<Element, Tag, Reference, Derived> p)
{
  return !(nullptr == p);
}

template <typename Element, typename Tag, typename Reference, typename Derived>
THRUST_HOST_DEVICE
bool operator!=(pointer<Element, Tag, Reference, Derived> p, std::nullptr_t)
{
  return !(nullptr == p);
}

THRUST_NAMESPACE_END
