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
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>
#include <thrust/iterator/constant_iterator.h>


template<typename ExecutionPolicy, typename Iterator, typename T, typename Iterator2>
__global__
void reduce_kernel(ExecutionPolicy exec, Iterator first, Iterator last, T init, Iterator2 result)
{
  *result = thrust::reduce(exec, first, last, init);
}


#ifdef THRUST_TEST_DEVICE_SIDE
template<typename T, typename ExecutionPolicy>
void TestReduceDevice(ExecutionPolicy exec, const size_t n)
{
  thrust::host_vector<T>   h_data = unittest::random_integers<T>(n);
  thrust::device_vector<T> d_data = h_data;
  
  thrust::device_vector<T> d_result(1);
  
  T init = 13;
  
  T h_result = thrust::reduce(h_data.begin(), h_data.end(), init);
  
  reduce_kernel<<<1,1>>>(exec, d_data.begin(), d_data.end(), init, d_result.begin());
  cudaError_t const err = cudaDeviceSynchronize();
  ASSERT_EQUAL(cudaSuccess, err);
  
  ASSERT_EQUAL(h_result, d_result[0]);
}


template<typename T>
struct TestReduceDeviceSeq
{
  void operator()(const size_t n)
  {
    TestReduceDevice<T>(thrust::seq, n);
  }
};
VariableUnitTest<TestReduceDeviceSeq, IntegralTypes> TestReduceDeviceSeqInstance;


template<typename T>
struct TestReduceDeviceDevice
{
  void operator()(const size_t n)
  {
    TestReduceDevice<T>(thrust::device, n);
  }
};
VariableUnitTest<TestReduceDeviceDevice, IntegralTypes> TestReduceDeviceDeviceInstance;


template<typename T>
struct TestReduceDeviceNoSync
{
  void operator()(const size_t n)
  {
    TestReduceDevice<T>(thrust::cuda::par_nosync, n);
  }
};
VariableUnitTest<TestReduceDeviceNoSync, IntegralTypes> TestReduceDeviceNoSyncInstance;
#endif


template<typename ExecutionPolicy>
void TestReduceCudaStreams(ExecutionPolicy policy)
{
  using Vector = thrust::device_vector<int>;

  Vector v(3);
  v[0] = 1; v[1] = -2; v[2] = 3;

  cudaStream_t s;
  cudaStreamCreate(&s);

  auto streampolicy = policy.on(s);

  // no initializer
  ASSERT_EQUAL(thrust::reduce(streampolicy, v.begin(), v.end()), 2);

  // with initializer
  ASSERT_EQUAL(thrust::reduce(streampolicy, v.begin(), v.end(), 10), 12);

  cudaStreamDestroy(s);
}

void TestReduceCudaStreamsSync()
{
  TestReduceCudaStreams(thrust::cuda::par);
}
DECLARE_UNITTEST(TestReduceCudaStreamsSync);


void TestReduceCudaStreamsNoSync()
{
  TestReduceCudaStreams(thrust::cuda::par_nosync);
}
DECLARE_UNITTEST(TestReduceCudaStreamsNoSync);

#if defined(THRUST_RDC_ENABLED)
void TestReduceLargeInput()
{
  using T = unsigned long long;
  using OffsetT = std::size_t;
  const OffsetT num_items = 1ull << 32;

  thrust::constant_iterator<T> d_data(T{1});
  thrust::device_vector<T> d_result(1);

  reduce_kernel<<<1,1>>>(thrust::device, d_data, d_data + num_items, T{}, d_result.begin());
  cudaError_t const err = cudaDeviceSynchronize();
  ASSERT_EQUAL(cudaSuccess, err);
  
  ASSERT_EQUAL(num_items, d_result[0]);
}
DECLARE_UNITTEST(TestReduceLargeInput);
#endif

