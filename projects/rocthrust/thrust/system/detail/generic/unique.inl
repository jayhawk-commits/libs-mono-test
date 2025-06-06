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
#include <thrust/system/detail/generic/unique.h>
#include <thrust/iterator/iterator_traits.h>
#include <thrust/transform.h>
#include <thrust/unique.h>
#include <thrust/detail/temporary_array.h>
#include <thrust/detail/internal_functional.h>
#include <thrust/detail/copy_if.h>
#include <thrust/detail/count.h>
#include <thrust/distance.h>
#include <thrust/functional.h>
#include <thrust/detail/range/head_flags.h>

THRUST_NAMESPACE_BEGIN
namespace system
{
namespace detail
{
namespace generic
{


template<typename DerivedPolicy,
         typename ForwardIterator>
THRUST_HOST_DEVICE
  ForwardIterator unique(thrust::execution_policy<DerivedPolicy> &exec,
                         ForwardIterator first,
                         ForwardIterator last)
{
  using InputType = typename thrust::iterator_traits<ForwardIterator>::value_type;

  return thrust::unique(exec, first, last, thrust::equal_to<InputType>());
} // end unique()


template<typename DerivedPolicy,
         typename ForwardIterator,
         typename BinaryPredicate>
THRUST_HOST_DEVICE
  ForwardIterator unique(thrust::execution_policy<DerivedPolicy> &exec,
                         ForwardIterator first,
                         ForwardIterator last,
                         BinaryPredicate binary_pred)
{
  using InputType = typename thrust::iterator_traits<ForwardIterator>::value_type;

  thrust::detail::temporary_array<InputType,DerivedPolicy> input(exec, first, last);

  return thrust::unique_copy(exec, input.begin(), input.end(), first, binary_pred);
} // end unique()


template<typename DerivedPolicy,
         typename InputIterator,
         typename OutputIterator>
THRUST_HOST_DEVICE
  OutputIterator unique_copy(thrust::execution_policy<DerivedPolicy> &exec,
                             InputIterator first,
                             InputIterator last,
                             OutputIterator output)
{
  using value_type = typename thrust::iterator_value<InputIterator>::type;
  return thrust::unique_copy(exec, first,last,output,thrust::equal_to<value_type>());
} // end unique_copy()


template<typename DerivedPolicy,
         typename InputIterator,
         typename OutputIterator,
         typename BinaryPredicate>
THRUST_HOST_DEVICE
  OutputIterator unique_copy(thrust::execution_policy<DerivedPolicy> &exec,
                             InputIterator first,
                             InputIterator last,
                             OutputIterator output,
                             BinaryPredicate binary_pred)
{
  thrust::detail::head_flags<InputIterator, BinaryPredicate> stencil(first, last, binary_pred);

  using namespace thrust::placeholders;

  return thrust::copy_if(exec, first, last, stencil.begin(), output, _1);
} // end unique_copy()


template<typename DerivedPolicy,
         typename ForwardIterator,
         typename BinaryPredicate>
THRUST_HOST_DEVICE
  typename thrust::iterator_traits<ForwardIterator>::difference_type
    unique_count(thrust::execution_policy<DerivedPolicy> &exec,
                 ForwardIterator first,
                 ForwardIterator last,
                 BinaryPredicate binary_pred)
{
  thrust::detail::head_flags<ForwardIterator, BinaryPredicate> stencil(first, last, binary_pred);
  
  using namespace thrust::placeholders;
  
  return thrust::count_if(exec, stencil.begin(), stencil.end(), _1);
} // end unique_copy()


template<typename DerivedPolicy,
         typename ForwardIterator>
THRUST_HOST_DEVICE
  typename thrust::iterator_traits<ForwardIterator>::difference_type
    unique_count(thrust::execution_policy<DerivedPolicy> &exec,
                 ForwardIterator first,
                 ForwardIterator last)
{
  using value_type = typename thrust::iterator_value<ForwardIterator>::type;
  return thrust::unique_count(exec, first, last, thrust::equal_to<value_type>());
} // end unique_copy()


} // end namespace generic
} // end namespace detail
} // end namespace system
THRUST_NAMESPACE_END

