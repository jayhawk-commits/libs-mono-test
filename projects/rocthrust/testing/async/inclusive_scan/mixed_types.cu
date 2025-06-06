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

// Test using mixed int/float types for:
// - input_value_type       | (int, float)
// - output_value_type      | (int, float)
// - thrust::plus<T> T-type | (int, float, void>)
//
// The thrust::plus<T> types are covered by the
// scan_mixed_types_overloads component.
//
// The testing/scan.cu TestMixedTypes test spells out the expected behavior,
// which is defined by https://wg21.link/P0571.

namespace
{

template <typename value_type>
struct mixed_type_input_generator
{
  using input_type = thrust::device_vector<value_type>;

  static input_type generate_input(std::size_t num_values)
  {
    input_type input(num_values);
    thrust::sequence(input.begin(),
                     input.end(),
                     // fractional values are chosen deliberately to test
                     // casting orders and accumulator types:
                     static_cast<value_type>(1.5),
                     static_cast<value_type>(1));
    return input;
  }
};

// A fractional value is used to ensure that a different result is obtained when
// using float vs. int.
struct mixed_types_postfix_args
{
  using postfix_args_type = std::tuple<  // Overloads to test:
    std::tuple<>,                        // - no extra args
    std::tuple<thrust::plus<>>,          // - plus<>
    std::tuple<thrust::plus<int>>,       // - plus<int>
    std::tuple<thrust::plus<float>>      // - plus<float>
    >;

  static postfix_args_type generate_postfix_args()
  {
    return postfix_args_type{std::tuple<>{},
                             std::make_tuple(thrust::plus<>{}),
                             std::make_tuple(thrust::plus<int>{}),
                             std::make_tuple(thrust::plus<float>{})};
  }
};

template <typename input_value_type,
          typename output_value_type>
struct invoker
    : mixed_type_input_generator<input_value_type>
    , testing::async::mixin::output::device_vector<output_value_type>
    , mixed_types_postfix_args
    , testing::async::inclusive_scan::mixin::invoke_reference::
        host_synchronous<input_value_type, output_value_type>
    , testing::async::inclusive_scan::mixin::invoke_async::simple
    // Use almost_equal instead of almost_equal_if_fp because floating point
    // addition may be hidden in the scan_op (thrust::plus<float> is always
    // tested).
    , testing::async::mixin::compare_outputs::assert_almost_equal
{
  static std::string description()
  {
    return "mixed input/output/functor value_type tests";
  }
};

} // namespace

void test_scan_mixed_types(size_t num_values)
{
  // Since fp addition is non-associative, the results may be slightly off
  // from the reference.
  // This is primarily handled by using `compare_almost_equal` to do a fuzzy
  // comparison. But for large enough test sizes, eventually the scan results
  // will wrap for integral value_types. If a float accumulator is used, the
  // small errors from non-associative addition may cause the wrap to happen in
  // a different location, resulting in an error too large for almost_equal to
  // ignore.
  // This wrap seems to happen around 2^16 values, so skip when num_values is
  // close to that.
  if (num_values > ((1ll << 16) - 10))
  {
    return;
  }

  // invoker template params are input_value_type, output_vt:
  using testing::async::test_policy_overloads;
  test_policy_overloads<invoker<int, int>>::run(num_values);
  test_policy_overloads<invoker<int, float>>::run(num_values);
  test_policy_overloads<invoker<float, int>>::run(num_values);
  test_policy_overloads<invoker<float, float>>::run(num_values);
}
DECLARE_SIZED_UNITTEST(test_scan_mixed_types);

#endif // C++14
