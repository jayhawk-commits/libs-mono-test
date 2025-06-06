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
#include <thrust/iterator/retag.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/random.h>
#include <thrust/scan.h>

#include "test_header.hpp"

TESTS_DEFINE(ScanByKeyTests, FullTestsParams);

TESTS_DEFINE(ScanByKeyVariablesTests, NumericalTestsParams);

TESTS_DEFINE(ScanByKeyVectorTests, VectorSignedIntegerTestsParams);

TYPED_TEST(ScanByKeyVectorTests, TestInclusiveScanByKeySimple)
{
    using Vector   = typename TestFixture::input_type;
    using T        = typename Vector::value_type;
    using Iterator = typename Vector::iterator;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    Vector keys(7);
    Vector vals(7);

    Vector output(7, 0);

    keys[0] = 0;
    vals[0] = 1;
    keys[1] = 1;
    vals[1] = 2;
    keys[2] = 1;
    vals[2] = 3;
    keys[3] = 1;
    vals[3] = 4;
    keys[4] = 2;
    vals[4] = 5;
    keys[5] = 3;
    vals[5] = 6;
    keys[6] = 3;
    vals[6] = 7;

    Iterator iter
        = thrust::inclusive_scan_by_key(keys.begin(), keys.end(), vals.begin(), output.begin());

    ASSERT_EQ_QUIET(iter, output.end());

    ASSERT_EQ(output[0], 1);
    ASSERT_EQ(output[1], 2);
    ASSERT_EQ(output[2], 5);
    ASSERT_EQ(output[3], 9);
    ASSERT_EQ(output[4], 5);
    ASSERT_EQ(output[5], 6);
    ASSERT_EQ(output[6], 13);

    thrust::inclusive_scan_by_key(keys.begin(),
                                  keys.end(),
                                  vals.begin(),
                                  output.begin(),
                                  thrust::equal_to<T>(),
                                  thrust::multiplies<T>());

    ASSERT_EQ(output[0], 1);
    ASSERT_EQ(output[1], 2);
    ASSERT_EQ(output[2], 6);
    ASSERT_EQ(output[3], 24);
    ASSERT_EQ(output[4], 5);
    ASSERT_EQ(output[5], 6);
    ASSERT_EQ(output[6], 42);

    thrust::inclusive_scan_by_key(
        keys.begin(), keys.end(), vals.begin(), output.begin(), thrust::equal_to<T>());

    ASSERT_EQ(output[0], 1);
    ASSERT_EQ(output[1], 2);
    ASSERT_EQ(output[2], 5);
    ASSERT_EQ(output[3], 9);
    ASSERT_EQ(output[4], 5);
    ASSERT_EQ(output[5], 6);
    ASSERT_EQ(output[6], 13);
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator inclusive_scan_by_key(
    my_system& system, InputIterator1, InputIterator1, InputIterator2, OutputIterator result)
{
    system.validate_dispatch();
    return result;
}

TEST(ScanByKeyTests, TestInclusiveScanByKeyDispatchExplicit)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::inclusive_scan_by_key(sys, vec.begin(), vec.begin(), vec.begin(), vec.begin());

    ASSERT_EQ(true, sys.is_valid());
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator
inclusive_scan_by_key(my_tag, InputIterator1, InputIterator1, InputIterator2, OutputIterator result)
{
    *result = 13;
    return result;
}

TEST(ScanByKeyTests, TestInclusiveScanByKeyDispatchImplicit)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    thrust::device_vector<int> vec(1);

    thrust::inclusive_scan_by_key(thrust::retag<my_tag>(vec.begin()),
                                  thrust::retag<my_tag>(vec.begin()),
                                  thrust::retag<my_tag>(vec.begin()),
                                  thrust::retag<my_tag>(vec.begin()));

    ASSERT_EQ(13, vec.front());
}

TYPED_TEST(ScanByKeyVectorTests, TestExclusiveScanByKeySimple)
{
    using Vector   = typename TestFixture::input_type;
    using T        = typename Vector::value_type;
    using Iterator = typename Vector::iterator;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    Vector keys(7);
    Vector vals(7);

    Vector output(7, 0);

    keys[0] = 0;
    vals[0] = 1;
    keys[1] = 1;
    vals[1] = 2;
    keys[2] = 1;
    vals[2] = 3;
    keys[3] = 1;
    vals[3] = 4;
    keys[4] = 2;
    vals[4] = 5;
    keys[5] = 3;
    vals[5] = 6;
    keys[6] = 3;
    vals[6] = 7;

    Iterator iter
        = thrust::exclusive_scan_by_key(keys.begin(), keys.end(), vals.begin(), output.begin());

    ASSERT_EQ_QUIET(iter, output.end());

    ASSERT_EQ(output[0], 0);
    ASSERT_EQ(output[1], 0);
    ASSERT_EQ(output[2], 2);
    ASSERT_EQ(output[3], 5);
    ASSERT_EQ(output[4], 0);
    ASSERT_EQ(output[5], 0);
    ASSERT_EQ(output[6], 6);

    thrust::exclusive_scan_by_key(keys.begin(), keys.end(), vals.begin(), output.begin(), T(10));

    ASSERT_EQ(output[0], 10);
    ASSERT_EQ(output[1], 10);
    ASSERT_EQ(output[2], 12);
    ASSERT_EQ(output[3], 15);
    ASSERT_EQ(output[4], 10);
    ASSERT_EQ(output[5], 10);
    ASSERT_EQ(output[6], 16);

    thrust::exclusive_scan_by_key(keys.begin(),
                                  keys.end(),
                                  vals.begin(),
                                  output.begin(),
                                  T(10),
                                  thrust::equal_to<T>(),
                                  thrust::multiplies<T>());

    ASSERT_EQ(output[0], 10);
    ASSERT_EQ(output[1], 10);
    ASSERT_EQ(output[2], 20);
    ASSERT_EQ(output[3], 60);
    ASSERT_EQ(output[4], 10);
    ASSERT_EQ(output[5], 10);
    ASSERT_EQ(output[6], 60);

    thrust::exclusive_scan_by_key(
        keys.begin(), keys.end(), vals.begin(), output.begin(), T(10), thrust::equal_to<T>());

    ASSERT_EQ(output[0], 10);
    ASSERT_EQ(output[1], 10);
    ASSERT_EQ(output[2], 12);
    ASSERT_EQ(output[3], 15);
    ASSERT_EQ(output[4], 10);
    ASSERT_EQ(output[5], 10);
    ASSERT_EQ(output[6], 16);
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator exclusive_scan_by_key(
    my_system& system, InputIterator1, InputIterator1, InputIterator2, OutputIterator result)
{
    system.validate_dispatch();
    return result;
}

TEST(ScanByKeyTests, TestExclusiveScanByKeyDispatchExplicit)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::exclusive_scan_by_key(sys, vec.begin(), vec.begin(), vec.begin(), vec.begin());

    ASSERT_EQ(true, sys.is_valid());
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator
exclusive_scan_by_key(my_tag, InputIterator1, InputIterator1, InputIterator2, OutputIterator result)
{
    *result = 13;
    return result;
}

TEST(ScanByKeyTests, TestExclusiveScanByKeyDispatchImplicit)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    thrust::device_vector<int> vec(1);

    thrust::exclusive_scan_by_key(thrust::retag<my_tag>(vec.begin()),
                                  thrust::retag<my_tag>(vec.begin()),
                                  thrust::retag<my_tag>(vec.begin()),
                                  thrust::retag<my_tag>(vec.begin()));

    ASSERT_EQ(13, vec.front());
}

struct head_flag_predicate
{
    template <typename T>
    __host__ __device__ bool operator()(const T&, const T& b) const
    {
        return b ? false : true;
    }
};

TYPED_TEST(ScanByKeyVectorTests, TestScanByKeyHeadFlags)
{
    using Vector = typename TestFixture::input_type;
    using T      = typename Vector::value_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    Vector keys(7);
    Vector vals(7);

    Vector output(7, 0);

    keys[0] = 0;
    vals[0] = 1;
    keys[1] = 1;
    vals[1] = 2;
    keys[2] = 0;
    vals[2] = 3;
    keys[3] = 0;
    vals[3] = 4;
    keys[4] = 1;
    vals[4] = 5;
    keys[5] = 1;
    vals[5] = 6;
    keys[6] = 0;
    vals[6] = 7;

    thrust::inclusive_scan_by_key(keys.begin(),
                                  keys.end(),
                                  vals.begin(),
                                  output.begin(),
                                  head_flag_predicate(),
                                  thrust::plus<T>());

    ASSERT_EQ(output[0], 1);
    ASSERT_EQ(output[1], 2);
    ASSERT_EQ(output[2], 5);
    ASSERT_EQ(output[3], 9);
    ASSERT_EQ(output[4], 5);
    ASSERT_EQ(output[5], 6);
    ASSERT_EQ(output[6], 13);

    thrust::exclusive_scan_by_key(keys.begin(),
                                  keys.end(),
                                  vals.begin(),
                                  output.begin(),
                                  T(10),
                                  head_flag_predicate(),
                                  thrust::plus<T>());

    ASSERT_EQ(output[0], 10);
    ASSERT_EQ(output[1], 10);
    ASSERT_EQ(output[2], 12);
    ASSERT_EQ(output[3], 15);
    ASSERT_EQ(output[4], 10);
    ASSERT_EQ(output[5], 10);
    ASSERT_EQ(output[6], 16);
}

TYPED_TEST(ScanByKeyVectorTests, TestInclusiveScanByKeyTransformIterator)
{
    using Vector = typename TestFixture::input_type;
    using T      = typename Vector::value_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    Vector keys(7);
    Vector vals(7);

    Vector output(7, 0);

    keys[0] = 0;
    vals[0] = 1;
    keys[1] = 1;
    vals[1] = 2;
    keys[2] = 1;
    vals[2] = 3;
    keys[3] = 1;
    vals[3] = 4;
    keys[4] = 2;
    vals[4] = 5;
    keys[5] = 3;
    vals[5] = 6;
    keys[6] = 3;
    vals[6] = 7;

    thrust::inclusive_scan_by_key(
        keys.begin(),
        keys.end(),
        thrust::make_transform_iterator(vals.begin(), thrust::negate<T>()),
        output.begin());

    ASSERT_EQ(output[0], -1);
    ASSERT_EQ(output[1], -2);
    ASSERT_EQ(output[2], -5);
    ASSERT_EQ(output[3], -9);
    ASSERT_EQ(output[4], -5);
    ASSERT_EQ(output[5], -6);
    ASSERT_EQ(output[6], -13);
}

TYPED_TEST(ScanByKeyVectorTests, TestScanByKeyReusedKeys)
{
    using Vector = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());
    Vector keys(7);
    Vector vals(7);

    Vector output(7, 0);

    keys[0] = 0;
    vals[0] = 1;
    keys[1] = 1;
    vals[1] = 2;
    keys[2] = 1;
    vals[2] = 3;
    keys[3] = 1;
    vals[3] = 4;
    keys[4] = 0;
    vals[4] = 5;
    keys[5] = 1;
    vals[5] = 6;
    keys[6] = 1;
    vals[6] = 7;

    thrust::inclusive_scan_by_key(keys.begin(), keys.end(), vals.begin(), output.begin());

    ASSERT_EQ(output[0], 1);
    ASSERT_EQ(output[1], 2);
    ASSERT_EQ(output[2], 5);
    ASSERT_EQ(output[3], 9);
    ASSERT_EQ(output[4], 5);
    ASSERT_EQ(output[5], 6);
    ASSERT_EQ(output[6], 13);

    thrust::exclusive_scan_by_key(keys.begin(), keys.end(), vals.begin(), output.begin(), 10);

    ASSERT_EQ(output[0], 10);
    ASSERT_EQ(output[1], 10);
    ASSERT_EQ(output[2], 12);
    ASSERT_EQ(output[3], 15);
    ASSERT_EQ(output[4], 10);
    ASSERT_EQ(output[5], 10);
    ASSERT_EQ(output[6], 16);
}

TYPED_TEST(ScanByKeyVariablesTests, TestInclusiveScanByKey)
{
    using T = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    // XXX WAR nvbug 1541533
#if THRUST_HOST_COMPILER == THRUST_HOST_COMPILER_MSVC
    if(typeid(T) == typeid(char) || typeid(T) == typeid(unsigned char))
    {
        KNOWN_FAILURE;
    }
#endif

    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        thrust::host_vector<int>      h_keys(size);
        thrust::default_random_engine rng;
        for(size_t i = 0, k = 0; i < size; i++)
        {
            h_keys[i] = k;
            if(rng() % 10 == 0)
                k++;
        }
        thrust::device_vector<int> d_keys = h_keys;

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<int> h_vals = get_random_data<int>(
                size, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), seed);
            for(size_t i = 0; i < size; i++)
                h_vals[i] = i % 10;
            thrust::device_vector<T> d_vals = h_vals;

            thrust::host_vector<T>   h_output(size);
            thrust::device_vector<T> d_output(size);

            thrust::inclusive_scan_by_key(
                h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin());
            thrust::inclusive_scan_by_key(
                d_keys.begin(), d_keys.end(), d_vals.begin(), d_output.begin());
            ASSERT_EQ(d_output, h_output);
        }
    }
}

TYPED_TEST(ScanByKeyVariablesTests, TestExclusiveScanByKey)
{
    using T = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        thrust::host_vector<int>      h_keys(size);
        thrust::default_random_engine rng;
        for(size_t i = 0, k = 0; i < size; i++)
        {
            h_keys[i] = k;
            if(rng() % 10 == 0)
                k++;
        }
        thrust::device_vector<int> d_keys = h_keys;

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<int> h_vals = get_random_data<int>(
                size, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), seed);
            for(size_t i = 0; i < size; i++)
                h_vals[i] = i % 10;
            thrust::device_vector<T> d_vals = h_vals;

            thrust::host_vector<T>   h_output(size);
            thrust::device_vector<T> d_output(size);

            // without init
            thrust::exclusive_scan_by_key(
                h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin());
            thrust::exclusive_scan_by_key(
                d_keys.begin(), d_keys.end(), d_vals.begin(), d_output.begin());
            ASSERT_EQ(d_output, h_output);

            // with init
            thrust::exclusive_scan_by_key(
                h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin(), (T)11);
            thrust::exclusive_scan_by_key(
                d_keys.begin(), d_keys.end(), d_vals.begin(), d_output.begin(), (T)11);
            ASSERT_EQ(d_output, h_output);
        }
    }
}

TYPED_TEST(ScanByKeyVariablesTests, TestInclusiveScanByKeyInPlace)
{
    using T = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    // XXX WAR nvbug 1541533
#if THRUST_HOST_COMPILER == THRUST_HOST_COMPILER_MSVC
    if(typeid(T) == typeid(char) || typeid(T) == typeid(unsigned char))
    {
        KNOWN_FAILURE;
    }
#endif

    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        thrust::host_vector<int>      h_keys(size);
        thrust::default_random_engine rng;
        for(size_t i = 0, k = 0; i < size; i++)
        {
            h_keys[i] = k;
            if(rng() % 10 == 0)
                k++;
        }
        thrust::device_vector<int> d_keys = h_keys;

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<T> h_vals = get_random_data<T>(
                size, get_default_limits<T>::min(), get_default_limits<T>::max(), seed);
            for(size_t i = 0; i < size; i++)
                h_vals[i] = i % 10;
            thrust::device_vector<T> d_vals = h_vals;

            thrust::host_vector<T>   h_output(size);
            thrust::device_vector<T> d_output(size);

            // in-place scans
            h_output = h_vals;
            d_output = d_vals;
            thrust::inclusive_scan_by_key(
                h_keys.begin(), h_keys.end(), h_output.begin(), h_output.begin());
            thrust::inclusive_scan_by_key(
                d_keys.begin(), d_keys.end(), d_output.begin(), d_output.begin());
            test_equality(h_output, d_output);
        }
    }
}

TYPED_TEST(ScanByKeyVariablesTests, TestExclusiveScanByKeyInPlace)
{
    using T = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        thrust::host_vector<int>      h_keys(size);
        thrust::default_random_engine rng;
        for(size_t i = 0, k = 0; i < size; i++)
        {
            h_keys[i] = k;
            if(rng() % 10 == 0)
                k++;
        }
        thrust::device_vector<int> d_keys = h_keys;

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<T> h_vals = get_random_data<T>(
                size, get_default_limits<T>::min(), get_default_limits<T>::max(), seed);
            for(size_t i = 0; i < size; i++)
                h_vals[i] = i % 10;
            thrust::device_vector<T> d_vals = h_vals;

            thrust::host_vector<T>   h_output = h_vals;
            thrust::device_vector<T> d_output = d_vals;
            thrust::exclusive_scan_by_key(
                h_keys.begin(), h_keys.end(), h_output.begin(), h_output.begin(), (T)11);
            thrust::exclusive_scan_by_key(
                d_keys.begin(), d_keys.end(), d_output.begin(), d_output.begin(), (T)11);\
            test_equality(h_output, d_output);
        }
    }
}

TEST(ScanByKeyTests, TestScanByKeyMixedTypes)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    const unsigned int size = 113;

    thrust::host_vector<int>      h_keys(size);
    thrust::default_random_engine rng;
    for(size_t i = 0, k = 0; i < size; i++)
    {
        h_keys[i] = k;
        if(rng() % 10 == 0)
            k++;
    }
    thrust::device_vector<int> d_keys = h_keys;

    for(auto seed : get_seeds())
    {
        SCOPED_TRACE(testing::Message() << "with seed= " << seed);

        thrust::host_vector<unsigned int> h_vals = get_random_data<unsigned int>(
            size, std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max(), seed);
        for(size_t i = 0; i < size; i++)
            h_vals[i] %= 10;
        thrust::device_vector<unsigned int> d_vals = h_vals;

        thrust::host_vector<float>   h_float_output(size);
        thrust::device_vector<float> d_float_output(size);
        thrust::host_vector<int>     h_int_output(size);
        thrust::device_vector<int>   d_int_output(size);

        //mixed vals/output types
        thrust::inclusive_scan_by_key(
            h_keys.begin(), h_keys.end(), h_vals.begin(), h_float_output.begin());
        thrust::inclusive_scan_by_key(
            d_keys.begin(), d_keys.end(), d_vals.begin(), d_float_output.begin());
        ASSERT_EQ(d_float_output, h_float_output);

        thrust::exclusive_scan_by_key(
            h_keys.begin(), h_keys.end(), h_vals.begin(), h_float_output.begin(), (float)3.5);
        thrust::exclusive_scan_by_key(
            d_keys.begin(), d_keys.end(), d_vals.begin(), d_float_output.begin(), (float)3.5);
        ASSERT_EQ(d_float_output, h_float_output);

        thrust::exclusive_scan_by_key(
            h_keys.begin(), h_keys.end(), h_vals.begin(), h_float_output.begin(), (int)3);
        thrust::exclusive_scan_by_key(
            d_keys.begin(), d_keys.end(), d_vals.begin(), d_float_output.begin(), (int)3);
        ASSERT_EQ(d_float_output, h_float_output);

        thrust::exclusive_scan_by_key(
            h_keys.begin(), h_keys.end(), h_vals.begin(), h_int_output.begin(), (int)3);
        thrust::exclusive_scan_by_key(
            d_keys.begin(), d_keys.end(), d_vals.begin(), d_int_output.begin(), (int)3);
        ASSERT_EQ(d_int_output, h_int_output);

        thrust::exclusive_scan_by_key(
            h_keys.begin(), h_keys.end(), h_vals.begin(), h_int_output.begin(), (float)3.5);
        thrust::exclusive_scan_by_key(
            d_keys.begin(), d_keys.end(), d_vals.begin(), d_int_output.begin(), (float)3.5);
        ASSERT_EQ(d_int_output, h_int_output);
    }
}

TEST(ScanByKeyTests, TestScanByKeyLargeInput)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    const unsigned int N = 1 << 20;

    for(auto seed : get_seeds())
    {
        SCOPED_TRACE(testing::Message() << "with seed= " << seed);

        thrust::host_vector<int> vals_sizes = get_random_data<int>(
            10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), seed);

        thrust::host_vector<int> h_vals = get_random_data<int>(
            N,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            seed + seed_value_addition
        );
        thrust::device_vector<unsigned int> d_vals = h_vals;

        thrust::host_vector<unsigned int>   h_output(N, 0);
        thrust::device_vector<unsigned int> d_output(N, 0);

        for(unsigned int i = 0; i < vals_sizes.size(); i++)
        {
            const unsigned int n = vals_sizes[i] % N;

            // define segments
            thrust::host_vector<unsigned int> h_keys(n);
            thrust::default_random_engine     rng;
            for(size_t i = 0, k = 0; i < n; i++)
            {
                h_keys[i] = k;
                if(rng() % 100 == 0)
                    k++;
            }
            thrust::device_vector<unsigned int> d_keys = h_keys;

            thrust::inclusive_scan_by_key(
                h_keys.begin(), h_keys.begin() + n, h_vals.begin(), h_output.begin());
            thrust::inclusive_scan_by_key(
                d_keys.begin(), d_keys.begin() + n, d_vals.begin(), d_output.begin());
            ASSERT_EQ(d_output, h_output);

            thrust::inclusive_scan_by_key(
                h_keys.begin(), h_keys.begin() + n, h_vals.begin(), h_output.begin());
            thrust::inclusive_scan_by_key(
                d_keys.begin(), d_keys.begin() + n, d_vals.begin(), d_output.begin());
            ASSERT_EQ(d_output, h_output);
        }
    }
}

template <typename T, unsigned int N>
void _TestScanByKeyWithLargeTypes(void)
{
    size_t n = (64 * 1024) / sizeof(FixedVector<T, N>);

    thrust::host_vector<unsigned int>      h_keys(n);
    thrust::host_vector<FixedVector<T, N>> h_vals(n);
    thrust::host_vector<FixedVector<T, N>> h_output(n);

    thrust::default_random_engine rng;
    for(size_t i = 0, k = 0; i < h_vals.size(); i++)
    {
        h_vals[i] = FixedVector<T, N>(i);
        h_keys[i] = k;
        if(rng() % 5 == 0)
            k++;
    }

    thrust::device_vector<unsigned int>      d_keys = h_keys;
    thrust::device_vector<FixedVector<T, N>> d_vals = h_vals;
    thrust::device_vector<FixedVector<T, N>> d_output(n);

    thrust::inclusive_scan_by_key(h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin());
    thrust::inclusive_scan_by_key(d_keys.begin(), d_keys.end(), d_vals.begin(), d_output.begin());

    ASSERT_EQ_QUIET(h_output, d_output);

    thrust::exclusive_scan_by_key(
        h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin(), FixedVector<T, N>(0));
    thrust::exclusive_scan_by_key(
        d_keys.begin(), d_keys.end(), d_vals.begin(), d_output.begin(), FixedVector<T, N>(0));

    ASSERT_EQ_QUIET(h_output, d_output);
}

TEST(ScanByKeyTests, TestScanByKeyWithLargeTypes)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    _TestScanByKeyWithLargeTypes<int, 1>();
    _TestScanByKeyWithLargeTypes<int, 2>();
    _TestScanByKeyWithLargeTypes<int, 4>();
    _TestScanByKeyWithLargeTypes<int, 8>();
    //_TestScanByKeyWithLargeTypes<int,   16>();  // too many resources requested for launch
    //_TestScanByKeyWithLargeTypes<int,   32>();
    //_TestScanByKeyWithLargeTypes<int,   64>();  // too large to pass as argument
    //_TestScanByKeyWithLargeTypes<int,  128>();
    //_TestScanByKeyWithLargeTypes<int,  256>();
    //_TestScanByKeyWithLargeTypes<int,  512>();
    //_TestScanByKeyWithLargeTypes<int, 1024>();
}

__global__
THRUST_HIP_LAUNCH_BOUNDS_DEFAULT
void InclusiveScanByKeyKernel(int const N, int* in_array, int* keys_array, int *out_array)
{
    if(threadIdx.x == 0)
    {
        thrust::device_ptr<int> in_begin(in_array);
        thrust::device_ptr<int> in_end(in_array + N);
        thrust::device_ptr<int> keys_begin(keys_array);
        thrust::device_ptr<int> keys_end(keys_array+N);
        thrust::device_ptr<int> out_begin(out_array);

        thrust::inclusive_scan_by_key(thrust::hip::par, keys_begin, keys_end, in_begin, out_begin);
    }
}

TEST(ScanByKeyTests, TestInclusiveScanByKeyDevice)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        thrust::host_vector<int>      h_keys(size);
        thrust::default_random_engine rng;
        for(size_t i = 0, k = 0; i < size; i++)
        {
            h_keys[i] = k;
            if(rng() % 10 == 0)
                k++;
        }
        thrust::device_vector<int> d_keys = h_keys;

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<int> h_vals = get_random_data<int>(size, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), seed);
            for(size_t i = 0; i < size; i++)
            {
              h_vals[i] = i % 10;
            }
            thrust::device_vector<int> d_vals = h_vals;

            thrust::host_vector<int>   h_output(size);
            thrust::device_vector<int> d_output(size);

            thrust::inclusive_scan_by_key(h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin());

            hipLaunchKernelGGL(InclusiveScanByKeyKernel,
                               dim3(1, 1, 1),
                               dim3(128, 1, 1),
                               0,
                               0,
                               size,
                               thrust::raw_pointer_cast(&d_vals[0]),
                               thrust::raw_pointer_cast(&d_keys[0]),
                               thrust::raw_pointer_cast(&d_output[0]));

            ASSERT_EQ(d_output, h_output);
        }
    }
}


__global__
THRUST_HIP_LAUNCH_BOUNDS_DEFAULT
void ExclusiveScanByKeyKernel(int const N, int* in_array, int* keys_array, int *out_array)
{
    if(threadIdx.x == 0)
    {
        thrust::device_ptr<int> in_begin(in_array);
        thrust::device_ptr<int> in_end(in_array + N);
        thrust::device_ptr<int> keys_begin(keys_array);
        thrust::device_ptr<int> keys_end(keys_array+N);
        thrust::device_ptr<int> out_begin(out_array);

        thrust::exclusive_scan_by_key(thrust::hip::par, keys_begin, keys_end, in_begin, out_begin);
    }
}

TEST(ScanByKeyTests, TestExclusiveScanByKeyDevice)
{
    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        thrust::host_vector<int>      h_keys(size);
        thrust::default_random_engine rng;
        for(size_t i = 0, k = 0; i < size; i++)
        {
            h_keys[i] = k;
            if(rng() % 10 == 0)
                k++;
        }
        thrust::device_vector<int> d_keys = h_keys;

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<int> h_vals = get_random_data<int>(size, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), seed);
            for(size_t i = 0; i < size; i++)
            {
              h_vals[i] = i % 10;
            }
            thrust::device_vector<int> d_vals = h_vals;

            thrust::host_vector<int>   h_output(size);
            thrust::device_vector<int> d_output(size);

            thrust::exclusive_scan_by_key(h_keys.begin(), h_keys.end(), h_vals.begin(), h_output.begin());

            hipLaunchKernelGGL(ExclusiveScanByKeyKernel,
                               dim3(1, 1, 1),
                               dim3(128, 1, 1),
                               0,
                               0,
                               size,
                               thrust::raw_pointer_cast(&d_vals[0]),
                               thrust::raw_pointer_cast(&d_keys[0]),
                               thrust::raw_pointer_cast(&d_output[0]));

            ASSERT_EQ(d_output, h_output);
        }
    }
}
