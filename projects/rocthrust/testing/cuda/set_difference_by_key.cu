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
#include <thrust/set_operations.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>


#ifdef THRUST_TEST_DEVICE_SIDE
template<typename ExecutionPolicy, typename Iterator1, typename Iterator2, typename Iterator3, typename Iterator4, typename Iterator5, typename Iterator6, typename Iterator7>
__global__
void set_difference_by_key_kernel(ExecutionPolicy exec,
                                  Iterator1 keys_first1, Iterator1 keys_last1,
                                  Iterator2 keys_first2, Iterator2 keys_last2,
                                  Iterator3 values_first1,
                                  Iterator4 values_first2,
                                  Iterator5 keys_result,
                                  Iterator6 values_result,
                                  Iterator7 result)
{
  *result = thrust::set_difference_by_key(exec,
                                          keys_first1, keys_last1,
                                          keys_first2, keys_last2,
                                          values_first1,
                                          values_first2,
                                          keys_result,
                                          values_result);
}


template<typename ExecutionPolicy>
void TestSetDifferenceByKeyDevice(ExecutionPolicy exec)
{
  using Vector   = thrust::device_vector<int>;
  using Iterator = typename Vector::iterator;

  Vector a_key(4), b_key(5);
  Vector a_val(4), b_val(5);

  a_key[0] = 0; a_key[1] = 2; a_key[2] = 4; a_key[3] = 5;
  a_val[0] = 0; a_val[1] = 0; a_val[2] = 0; a_val[3] = 0;

  b_key[0] = 0; b_key[1] = 3; b_key[2] = 3; b_key[3] = 4; b_key[4] = 6;
  b_val[0] = 1; b_val[1] = 1; b_val[2] = 1; b_val[3] = 1; b_val[4] = 1;

  Vector ref_key(2), ref_val(2);
  ref_key[0] = 2; ref_key[1] = 5;
  ref_val[0] = 0; ref_val[1] = 0;

  Vector result_key(2), result_val(2);

  using iter_pair = thrust::pair<Iterator, Iterator>;

  thrust::device_vector<iter_pair> end_vec(1);

  set_difference_by_key_kernel<<<1,1>>>(exec,
                                        a_key.begin(), a_key.end(),
                                        b_key.begin(), b_key.end(),
                                        a_val.begin(),
                                        b_val.begin(),
                                        result_key.begin(),
                                        result_val.begin(),
                                        end_vec.begin());
  cudaError_t const err = cudaDeviceSynchronize();
  ASSERT_EQUAL(cudaSuccess, err);

  iter_pair end = end_vec.front();

  ASSERT_EQUAL_QUIET(result_key.end(), end.first);
  ASSERT_EQUAL_QUIET(result_val.end(), end.second);
  ASSERT_EQUAL(ref_key, result_key);
  ASSERT_EQUAL(ref_val, result_val);
}


void TestSetDifferenceByKeyDeviceSeq()
{
  TestSetDifferenceByKeyDevice(thrust::seq);
}
DECLARE_UNITTEST(TestSetDifferenceByKeyDeviceSeq);


void TestSetDifferenceByKeyDeviceDevice()
{
  TestSetDifferenceByKeyDevice(thrust::device);
}
DECLARE_UNITTEST(TestSetDifferenceByKeyDeviceDevice);
#endif


void TestSetDifferenceByKeyCudaStreams()
{
  using Vector   = thrust::device_vector<int>;
  using Iterator = Vector::iterator;

  Vector a_key(4), b_key(5);
  Vector a_val(4), b_val(5);

  a_key[0] = 0; a_key[1] = 2; a_key[2] = 4; a_key[3] = 5;
  a_val[0] = 0; a_val[1] = 0; a_val[2] = 0; a_val[3] = 0;

  b_key[0] = 0; b_key[1] = 3; b_key[2] = 3; b_key[3] = 4; b_key[4] = 6;
  b_val[0] = 1; b_val[1] = 1; b_val[2] = 1; b_val[3] = 1; b_val[4] = 1;

  Vector ref_key(2), ref_val(2);
  ref_key[0] = 2; ref_key[1] = 5;
  ref_val[0] = 0; ref_val[1] = 0;

  Vector result_key(2), result_val(2);

  cudaStream_t s;
  cudaStreamCreate(&s);

  thrust::pair<Iterator,Iterator> end =
    thrust::set_difference_by_key(thrust::cuda::par.on(s),
                                  a_key.begin(), a_key.end(),
                                  b_key.begin(), b_key.end(),
                                  a_val.begin(),
                                  b_val.begin(),
                                  result_key.begin(),
                                  result_val.begin());
  cudaStreamSynchronize(s);

  ASSERT_EQUAL_QUIET(result_key.end(), end.first);
  ASSERT_EQUAL_QUIET(result_val.end(), end.second);
  ASSERT_EQUAL(ref_key, result_key);
  ASSERT_EQUAL(ref_val, result_val);

  cudaStreamDestroy(s);
}
DECLARE_UNITTEST(TestSetDifferenceByKeyCudaStreams);

