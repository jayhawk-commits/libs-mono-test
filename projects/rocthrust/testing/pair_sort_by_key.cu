/*
 *  Copyright 2008-2013 NVIDIA Corporation
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

#include <unittest/unittest.h>
#include <thrust/pair.h>
#include <thrust/sort.h>

struct make_pair_functor
{
  template <typename T1, typename T2>
  THRUST_HOST_DEVICE thrust::pair<T1, T2> operator()(const T1& x, const T2& y)
  {
    return thrust::make_pair(x,y);
  } // end operator()()
}; // end make_pair_functor

template <typename T>
  struct TestPairStableSort
{
  void operator()(const size_t n)
  {
    using P = thrust::pair<T, T>;

    thrust::host_vector<T>   h_p1 = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_p2 = unittest::random_integers<T>(n);
    thrust::host_vector<P>   h_pairs(n);

    // zip up pairs on the host
    thrust::transform(h_p1.begin(), h_p1.end(), h_p2.begin(), h_pairs.begin(), make_pair_functor());

    thrust::device_vector<P> d_pairs = h_pairs;

    // sort on the host
    thrust::stable_sort(h_pairs.begin(), h_pairs.end());

    // sort on the device
    thrust::stable_sort(d_pairs.begin(), d_pairs.end());

    ASSERT_EQUAL_QUIET(h_pairs, d_pairs);
  }
};
VariableUnitTest<TestPairStableSort, unittest::type_list<unittest::int8_t,unittest::int16_t,unittest::int32_t> > TestPairStableSortInstance;

