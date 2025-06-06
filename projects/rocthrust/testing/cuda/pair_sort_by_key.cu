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
#include <thrust/transform.h>
#include <thrust/sequence.h>
#include <thrust/execution_policy.h>


#ifdef THRUST_TEST_DEVICE_SIDE
template<typename ExecutionPolicy, typename Iterator1, typename Iterator2>
__global__
void stable_sort_by_key_kernel(ExecutionPolicy exec, Iterator1 keys_first, Iterator1 keys_last, Iterator2 values_first)
{
  thrust::stable_sort_by_key(exec, keys_first, keys_last, values_first);
}


struct make_pair_functor
{
  template <typename T1, typename T2>
  THRUST_HOST_DEVICE thrust::pair<T1, T2> operator()(const T1& x, const T2& y)
  {
    return thrust::make_pair(x,y);
  } // end operator()()
}; // end make_pair_functor


template<typename ExecutionPolicy>
void TestPairStableSortByKeyDevice(ExecutionPolicy exec)
{
  size_t n = 10000;
  using P  = thrust::pair<int, int>;

  // host arrays
  thrust::host_vector<int>   h_p1 = unittest::random_integers<int>(n);
  thrust::host_vector<int>   h_p2 = unittest::random_integers<int>(n);
  thrust::host_vector<P>   h_pairs(n);

  thrust::host_vector<int> h_values(n);
  thrust::sequence(h_values.begin(), h_values.end());

  // zip up pairs on the host
  thrust::transform(h_p1.begin(), h_p1.end(), h_p2.begin(), h_pairs.begin(), make_pair_functor());

  // device arrays
  thrust::device_vector<P>   d_pairs = h_pairs;
  thrust::device_vector<int> d_values = h_values;

  // sort on the device
  stable_sort_by_key_kernel<<<1,1>>>(exec, d_pairs.begin(), d_pairs.end(), d_values.begin());
  cudaError_t const err = cudaDeviceSynchronize();
  ASSERT_EQUAL(cudaSuccess, err);

  // sort on the host
  thrust::stable_sort_by_key(h_pairs.begin(), h_pairs.end(), h_values.begin());

  ASSERT_EQUAL_QUIET(h_pairs,  d_pairs);
  ASSERT_EQUAL(h_values, d_values);
};


void TestPairStableSortByKeyDeviceSeq()
{
  TestPairStableSortByKeyDevice(thrust::seq);
}
DECLARE_UNITTEST(TestPairStableSortByKeyDeviceSeq);


void TestPairStableSortByKeyDeviceDevice()
{
  TestPairStableSortByKeyDevice(thrust::device);
}
DECLARE_UNITTEST(TestPairStableSortByKeyDeviceDevice);
#endif

