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

#include <thrust/functional.h>
#include <thrust/set_operations.h>
#include <thrust/sort.h>

#include "test_header.hpp"

TESTS_DEFINE(SetDifferenceByKeyDescendingTests, FullTestsParams);
TESTS_DEFINE(SetDifferenceByKeyDescendingPrimitiveTests, NumericalTestsParams);

TYPED_TEST(SetDifferenceByKeyDescendingTests, TestSetDifferenceByKeyDescendingSimple)
{
    using Vector   = typename TestFixture::input_type;
    using Policy   = typename TestFixture::execution_policy;
    using T        = typename Vector::value_type;
    using Iterator = typename Vector::iterator;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    Vector a_key(4), a_val(4);
    Vector b_key(5), b_val(5);

    a_key[0] = 5;
    a_key[1] = 4;
    a_key[2] = 2;
    a_key[3] = 0;
    a_val[0] = 0;
    a_val[1] = 0;
    a_val[2] = 0;
    a_val[3] = 0;

    b_key[0] = 6;
    b_key[1] = 4;
    b_key[2] = 3;
    b_key[3] = 3;
    b_key[4] = 0;
    b_val[0] = 1;
    b_val[1] = 1;
    b_val[2] = 1;
    b_val[3] = 1;
    b_val[4] = 1;

    Vector ref_key(2), ref_val(2);
    ref_key[0] = 5;
    ref_key[1] = 2;
    ref_val[0] = 0;
    ref_val[1] = 0;

    Vector result_key(2), result_val(2);

    thrust::pair<Iterator, Iterator> end = thrust::set_difference_by_key(Policy{},
                                                                         a_key.begin(),
                                                                         a_key.end(),
                                                                         b_key.begin(),
                                                                         b_key.end(),
                                                                         a_val.begin(),
                                                                         b_val.begin(),
                                                                         result_key.begin(),
                                                                         result_val.begin(),
                                                                         thrust::greater<T>());

    EXPECT_EQ(result_key.end(), end.first);
    EXPECT_EQ(result_val.end(), end.second);
    ASSERT_EQ(ref_key, result_key);
    ASSERT_EQ(ref_val, result_val);
}

TYPED_TEST(SetDifferenceByKeyDescendingPrimitiveTests, TestSetDifferenceByKeyDescending)
{
    using T = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    const std::vector<size_t> sizes = get_sizes();

    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<T> temp = get_random_data<T>(
                2 * size, get_default_limits<T>::min(), get_default_limits<T>::max(), seed);

            thrust::host_vector<T> h_a_key(temp.begin(), temp.begin() + size);
            thrust::host_vector<T> h_b_key(temp.begin() + size, temp.end());

            thrust::sort(h_a_key.begin(), h_a_key.end(), thrust::greater<T>());
            thrust::sort(h_b_key.begin(), h_b_key.end(), thrust::greater<T>());

            thrust::host_vector<T> h_a_val = get_random_data<T>(
                h_a_key.size(),
                get_default_limits<T>::min(),
                get_default_limits<T>::max(),
                seed + seed_value_addition
            );
            thrust::host_vector<T> h_b_val = get_random_data<T>(
                h_b_key.size(),
                get_default_limits<T>::min(),
                get_default_limits<T>::max(),
                seed + 2 * seed_value_addition
            );

            thrust::device_vector<T> d_a_key = h_a_key;
            thrust::device_vector<T> d_b_key = h_b_key;

            thrust::device_vector<T> d_a_val = h_a_val;
            thrust::device_vector<T> d_b_val = h_b_val;

            thrust::host_vector<T>   h_result_key(size), h_result_val(size);
            thrust::device_vector<T> d_result_key(size), d_result_val(size);

            thrust::pair<typename thrust::host_vector<T>::iterator,
                         typename thrust::host_vector<T>::iterator>
                h_end;

            thrust::pair<typename thrust::device_vector<T>::iterator,
                         typename thrust::device_vector<T>::iterator>
                d_end;

            h_end = thrust::set_difference_by_key(h_a_key.begin(),
                                                  h_a_key.end(),
                                                  h_b_key.begin(),
                                                  h_b_key.end(),
                                                  h_a_val.begin(),
                                                  h_b_val.begin(),
                                                  h_result_key.begin(),
                                                  h_result_val.begin(),
                                                  thrust::greater<T>());
            h_result_key.erase(h_end.first, h_result_key.end());
            h_result_val.erase(h_end.second, h_result_val.end());

            d_end = thrust::set_difference_by_key(d_a_key.begin(),
                                                  d_a_key.end(),
                                                  d_b_key.begin(),
                                                  d_b_key.end(),
                                                  d_a_val.begin(),
                                                  d_b_val.begin(),
                                                  d_result_key.begin(),
                                                  d_result_val.begin(),
                                                  thrust::greater<T>());
            d_result_key.erase(d_end.first, d_result_key.end());
            d_result_val.erase(d_end.second, d_result_val.end());

            ASSERT_EQ(h_result_key, d_result_key);
            ASSERT_EQ(h_result_val, d_result_val);
        }
    }
}
