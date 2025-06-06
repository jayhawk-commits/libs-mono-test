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
#include <thrust/sort.h>
#include <thrust/execution_policy.h>
#include <thrust/functional.h>


template<typename T>
struct my_less
{
  THRUST_HOST_DEVICE
  bool operator()(const T& lhs, const T& rhs) const
  {
    return lhs < rhs;
  }
};


#ifdef THRUST_TEST_DEVICE_SIDE
template<typename ExecutionPolicy, typename Iterator1, typename Iterator2, typename Compare>
__global__
void sort_by_key_kernel(ExecutionPolicy exec, Iterator1 keys_first, Iterator1 keys_last, Iterator2 values_first, Compare comp)
{
  thrust::sort_by_key(exec, keys_first, keys_last, values_first, comp);
}


template<typename T, typename ExecutionPolicy, typename Compare>
void TestComparisonSortByKeyDevice(ExecutionPolicy exec, const size_t n, Compare comp)
{
  thrust::host_vector<T>   h_keys = unittest::random_integers<T>(n);
  thrust::device_vector<T> d_keys = h_keys;

  thrust::host_vector<T>   h_values = h_keys;
  thrust::device_vector<T> d_values = d_keys;

  sort_by_key_kernel<<<1,1>>>(exec, d_keys.begin(), d_keys.end(), d_values.begin(), comp);
  cudaError_t const err = cudaDeviceSynchronize();
  ASSERT_EQUAL(cudaSuccess, err);

  thrust::sort_by_key(h_keys.begin(), h_keys.end(), h_values.begin(), comp);

  ASSERT_EQUAL(h_keys, d_keys);
  ASSERT_EQUAL(h_values, d_values);
};


template<typename T>
  struct TestComparisonSortByKeyDeviceSeq
{
  void operator()(const size_t n)
  {
    TestComparisonSortByKeyDevice<T>(thrust::seq, n, my_less<T>());
  }
};
VariableUnitTest<
  TestComparisonSortByKeyDeviceSeq,
  unittest::type_list<unittest::int8_t,unittest::int32_t>
> TestComparisonSortByKeyDeviceSeqInstance;


template<typename T>
  struct TestComparisonSortByKeyDeviceDevice
{
  void operator()(const size_t n)
  {
    TestComparisonSortByKeyDevice<T>(thrust::device, n, my_less<T>());
  }
};
VariableUnitTest<
  TestComparisonSortByKeyDeviceDevice,
  unittest::type_list<unittest::int8_t,unittest::int32_t>
> TestComparisonSortByKeyDeviceDeviceDeviceInstance;


template<typename T, typename ExecutionPolicy>
void TestSortByKeyDevice(ExecutionPolicy exec, const size_t n)
{
  TestComparisonSortByKeyDevice<T>(exec, n, thrust::less<T>());
};


template<typename T>
  struct TestSortByKeyDeviceSeq
{
  void operator()(const size_t n)
  {
    TestSortByKeyDevice<T>(thrust::seq, n);
  }
};
VariableUnitTest<
  TestSortByKeyDeviceSeq,
  unittest::type_list<unittest::int8_t,unittest::int32_t>
> TestSortByKeyDeviceSeqInstance;


template<typename T>
  struct TestSortByKeyDeviceDevice
{
  void operator()(const size_t n)
  {
    TestSortByKeyDevice<T>(thrust::device, n);
  }
};
VariableUnitTest<
  TestSortByKeyDeviceDevice,
  unittest::type_list<unittest::int8_t,unittest::int32_t>
> TestSortByKeyDeviceDeviceInstance;
#endif


void TestComparisonSortByKeyCudaStreams()
{
  thrust::device_vector<int> keys(10);
  thrust::device_vector<int> vals(10);

  keys[0] = 9; vals[0] = 9;
  keys[1] = 3; vals[1] = 3;
  keys[2] = 2; vals[2] = 2;
  keys[3] = 0; vals[3] = 0;
  keys[4] = 4; vals[4] = 4;
  keys[5] = 7; vals[5] = 7;
  keys[6] = 8; vals[6] = 8;
  keys[7] = 1; vals[7] = 1;
  keys[8] = 5; vals[8] = 5;
  keys[9] = 6; vals[9] = 6;

  cudaStream_t s;
  cudaStreamCreate(&s);

  thrust::sort_by_key(thrust::cuda::par.on(s), keys.begin(), keys.end(), vals.begin(), my_less<int>());
  cudaStreamSynchronize(s);

  ASSERT_EQUAL(true, thrust::is_sorted(keys.begin(), keys.end()));
  ASSERT_EQUAL(true, thrust::is_sorted(vals.begin(), vals.end()));

  cudaStreamDestroy(s);
}
DECLARE_UNITTEST(TestComparisonSortByKeyCudaStreams);


void TestSortByKeyCudaStreams()
{
  thrust::device_vector<int> keys(10);
  thrust::device_vector<int> vals(10);

  keys[0] = 9; vals[0] = 9;
  keys[1] = 3; vals[1] = 3;
  keys[2] = 2; vals[2] = 2;
  keys[3] = 0; vals[3] = 0;
  keys[4] = 4; vals[4] = 4;
  keys[5] = 7; vals[5] = 7;
  keys[6] = 8; vals[6] = 8;
  keys[7] = 1; vals[7] = 1;
  keys[8] = 5; vals[8] = 5;
  keys[9] = 6; vals[9] = 6;

  cudaStream_t s;
  cudaStreamCreate(&s);

  thrust::sort_by_key(thrust::cuda::par.on(s), keys.begin(), keys.end(), vals.begin());
  cudaStreamSynchronize(s);

  ASSERT_EQUAL(true, thrust::is_sorted(keys.begin(), keys.end()));
  ASSERT_EQUAL(true, thrust::is_sorted(vals.begin(), vals.end()));

  cudaStreamDestroy(s);
}
DECLARE_UNITTEST(TestSortByKeyCudaStreams);

