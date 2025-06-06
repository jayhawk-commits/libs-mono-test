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

#include <thrust/detail/config.h>

#if THRUST_CPP_DIALECT >= 2014

#include <async/test_policy_overloads.h>

#include <async/inclusive_scan/mixin.h>

#include <unittest/special_types.h>

// This test is an adaptation of TestScanWithLargeTypes from scan.cu.

// Need special initialization for the FixedVector type:
template <typename value_type>
struct device_vector_fill
{
  using input_type = thrust::device_vector<value_type>;

  static input_type generate_input(std::size_t num_values)
  {
    input_type input(num_values);
    thrust::fill(input.begin(), input.end(), value_type{2});
    return input;
  }
};

template <typename value_type, typename alternate_binary_op = thrust::maximum<>>
struct invoker
    : device_vector_fill<value_type>
    , testing::async::mixin::output::device_vector<value_type>
    , testing::async::inclusive_scan::mixin::postfix_args::
        all_overloads<alternate_binary_op>
    , testing::async::inclusive_scan::mixin::invoke_reference::host_synchronous<
        value_type>
    , testing::async::inclusive_scan::mixin::invoke_async::simple
    , testing::async::mixin::compare_outputs::assert_almost_equal_if_fp_quiet
{
  static std::string description()
  {
    return "scan with large value types.";
  }
};

struct test_large_types
{
  void operator()(std::size_t num_values) const
  {
    using testing::async::test_policy_overloads;

    test_policy_overloads<invoker<FixedVector<int, 1>>>::run(num_values);
    test_policy_overloads<invoker<FixedVector<int, 8>>>::run(num_values);
    test_policy_overloads<invoker<FixedVector<int, 32>>>::run(num_values);
    test_policy_overloads<invoker<FixedVector<int, 64>>>::run(num_values);
  }
};
DECLARE_UNITTEST(test_large_types);

#endif // C++14
