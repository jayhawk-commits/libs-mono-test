// Copyright (c) 2018 NVIDIA Corporation
// Modifications Copyright© 2023-2025 Advanced Micro Devices, Inc. All rights reserved.
// Author: Bryce Adelstein Lelbach <brycelelbach@gmail.com>
//
// Distributed under the Boost Software License v1.0 (boost.org/LICENSE_1_0.txt)

// TODO: These need to be turned into proper Thrust algorithms (dispatch layer,
// backends, etc).

#pragma once

#include <thrust/detail/config.h>
#include <thrust/detail/type_traits.h>
#include <thrust/iterator/iterator_traits.h>
#include <thrust/detail/allocator/allocator_traits.h>
#include <thrust/detail/memory_wrapper.h>
#include <thrust/addressof.h>

#include <thrust/detail/nv_target.h>

#include <utility>
#include <new>


THRUST_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////

template <typename T>
THRUST_HOST_DEVICE
void destroy_at(T* location)
{
  location->~T();
}

template <typename Allocator, typename T>
THRUST_HOST_DEVICE
void destroy_at(Allocator const& alloc, T* location)
{
  using traits = typename detail::allocator_traits<
    typename detail::remove_cv<
      typename detail::remove_reference<Allocator>::type
    >::type
  >::template rebind_traits<T>::other;

  typename traits::allocator_type alloc_T(alloc);

  traits::destroy(alloc_T, location);
}

template <typename ForwardIt>
THRUST_HOST_DEVICE
ForwardIt destroy(ForwardIt first, ForwardIt last)
{
  for (; first != last; ++first)
    destroy_at(addressof(*first));

  return first;
}

template <typename Allocator, typename ForwardIt>
THRUST_HOST_DEVICE
ForwardIt destroy(Allocator const& alloc, ForwardIt first, ForwardIt last)
{
  using T = typename iterator_traits<ForwardIt>::value_type;
  using traits = typename detail::allocator_traits<
    typename detail::remove_cv<
      typename detail::remove_reference<Allocator>::type
    >::type
  >::template rebind_traits<T>::other;

  typename traits::allocator_type alloc_T(alloc);

  for (; first != last; ++first)
    destroy_at(alloc_T, addressof(*first));

  return first;
}

template <typename ForwardIt, typename Size>
THRUST_HOST_DEVICE
ForwardIt destroy_n(ForwardIt first, Size n)
{
  for (; n > 0; (void) ++first, --n)
    destroy_at(addressof(*first));

  return first;
}

template <typename Allocator, typename ForwardIt, typename Size>
THRUST_HOST_DEVICE
ForwardIt destroy_n(Allocator const& alloc, ForwardIt first, Size n)
{
  using T = typename iterator_traits<ForwardIt>::value_type;
  using traits = typename detail::allocator_traits<
    typename detail::remove_cv<
      typename detail::remove_reference<Allocator>::type
    >::type
  >::template rebind_traits<T>::other;

  typename traits::allocator_type alloc_T(alloc);

  for (; n > 0; (void) ++first, --n)
    destroy_at(alloc_T, addressof(*first));

  return first;
}

template <typename ForwardIt, typename... Args>
THRUST_HOST_DEVICE
void uninitialized_construct(
  ForwardIt first, ForwardIt last, Args const&... args
)
{
  using T = typename iterator_traits<ForwardIt>::value_type;

  ForwardIt current = first;
  // No exceptions in CUDA.
  NV_IF_TARGET(NV_IS_HOST, (
    try {
      for (; current != last; ++current)
      {
        ::new (static_cast<void*>(addressof(*current))) T(args...);
      }
    } catch (...) {
      destroy(first, current);
      throw;
    }
  ), (
    for (; current != last; ++current)
    {
      ::new (static_cast<void*>(addressof(*current))) T(args...);
    }
  ));
}

template <typename Allocator, typename ForwardIt, typename... Args>
void uninitialized_construct_with_allocator(
  Allocator const& alloc, ForwardIt first, ForwardIt last, Args const&... args
)
{
  using T = typename iterator_traits<ForwardIt>::value_type;
  using traits = typename detail::allocator_traits<
    typename std::remove_cv<
      typename std::remove_reference<Allocator>::type
    >::type
  >::template rebind_traits<T>;

  typename traits::allocator_type alloc_T(alloc);

  ForwardIt current = first;
  // No exceptions in CUDA.
  NV_IF_TARGET(NV_IS_HOST, (
    try {
      for (; current != last; ++current)
      {
        traits::construct(alloc_T, addressof(*current), args...);
      }
    } catch (...) {
      destroy(alloc_T, first, current);
      throw;
    }
  ), (
    for (; current != last; ++current)
    {
      traits::construct(alloc_T, addressof(*current), args...);
    }
  ));
}

template <typename ForwardIt, typename Size, typename... Args>
void uninitialized_construct_n(
  ForwardIt first, Size n, Args const&... args
)
{
  using T = typename iterator_traits<ForwardIt>::value_type;

  ForwardIt current = first;
  // No exceptions in CUDA.
  NV_IF_TARGET(NV_IS_HOST, (
    try {
      for (; n > 0; ++current, --n)
      {
        ::new (static_cast<void*>(addressof(*current))) T(args...);
      }
    } catch (...) {
      destroy(first, current);
      throw;
    }
  ), (
    for (; n > 0; ++current, --n)
    {
      ::new (static_cast<void*>(addressof(*current))) T(args...);
    }
  ));
}

template <typename Allocator, typename ForwardIt, typename Size, typename... Args>
void uninitialized_construct_n_with_allocator(
  Allocator const& alloc, ForwardIt first, Size n, Args const&... args
)
{
  using T = typename iterator_traits<ForwardIt>::value_type;
  using traits = typename detail::allocator_traits<
    typename std::remove_cv<
      typename std::remove_reference<Allocator>::type
    >::type
  >::template rebind_traits<T>;

  typename traits::allocator_type alloc_T(alloc);

  ForwardIt current = first;
  // No exceptions in CUDA.
  NV_IF_TARGET(NV_IS_HOST, (
    try {
      for (; n > 0; (void) ++current, --n)
      {
        traits::construct(alloc_T, addressof(*current), args...);
      }
    } catch (...) {
      destroy(alloc_T, first, current);
      throw;
    }
  ), (
    for (; n > 0; (void) ++current, --n)
    {
      traits::construct(alloc_T, addressof(*current), args...);
    }
  ));
}

///////////////////////////////////////////////////////////////////////////////

THRUST_NAMESPACE_END
