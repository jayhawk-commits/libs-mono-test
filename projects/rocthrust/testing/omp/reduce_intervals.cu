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

#include <thrust/functional.h>
#include <thrust/system/detail/internal/decompose.h>
#include <thrust/system/omp/detail/reduce_intervals.h>

// CPP reference implementation 
template<typename InputIterator,
         typename OutputIterator,
         typename BinaryFunction,
         typename Decomposition>
void reduce_intervals(InputIterator input,
                      OutputIterator output,
                      BinaryFunction binary_op,
                      Decomposition decomp)
{
  using OutputType = typename thrust::iterator_value<OutputIterator>::type;
  using index_type = typename Decomposition::index_type;

  // wrap binary_op
  thrust::detail::wrapped_function<
    BinaryFunction,
    OutputType
  > wrapped_binary_op(binary_op);

  for(index_type i = 0; i < decomp.size(); ++i, ++output)
  {
    InputIterator begin = input + decomp[i].begin();
    InputIterator end   = input + decomp[i].end();

    if (begin != end)
    {
      OutputType sum = *begin;

      ++begin;

      while (begin != end)
      {
        sum = wrapped_binary_op(sum, *begin);
        ++begin;
      }

      *output = sum;
    }
  }
}


void TestOmpReduceIntervalsSimple(void)
{
  using T      = int;
  using Vector = thrust::device_vector<T>;

  using thrust::system::omp::detail::reduce_intervals;
  using thrust::system::detail::internal::uniform_decomposition;

  Vector input(10, 1);

  thrust::omp::tag omp_tag;
    
  {
    uniform_decomposition<int> decomp(10, 10, 1);
    Vector output(decomp.size());
    reduce_intervals(omp_tag, input.begin(), output.begin(), thrust::plus<T>(), decomp);

    ASSERT_EQUAL(output[0], 10);
  }
  
  {
    uniform_decomposition<int> decomp(10, 6, 2);
    Vector output(decomp.size());
    reduce_intervals(omp_tag, input.begin(), output.begin(), thrust::plus<T>(), decomp);

    ASSERT_EQUAL(output[0], 6);
    ASSERT_EQUAL(output[1], 4);
  }
}
DECLARE_UNITTEST(TestOmpReduceIntervalsSimple);


template<typename T>
struct TestOmpReduceIntervals
{
  void operator()(const size_t n)
  {
    using thrust::system::omp::detail::reduce_intervals;
    using thrust::system::detail::internal::uniform_decomposition;
    
    thrust::host_vector<T>   h_input = unittest::random_integers<T>(n);
    thrust::device_vector<T> d_input = h_input;

    uniform_decomposition<size_t> decomp(n, 7, 100);

    thrust::host_vector<T>   h_output(decomp.size());
    thrust::device_vector<T> d_output(decomp.size());
    
    ::reduce_intervals(h_input.begin(), h_output.begin(), thrust::plus<T>(), decomp);
    thrust::system::omp::tag omp_tag;
    reduce_intervals(omp_tag, d_input.begin(), d_output.begin(), thrust::plus<T>(), decomp);

    ASSERT_EQUAL(h_output, d_output);
  }
};
VariableUnitTest<TestOmpReduceIntervals, IntegralTypes> TestOmpReduceIntervalsInstance;

