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
#include <thrust/transform.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/discard_iterator.h>
#include <thrust/tuple.h>
#include <thrust/pair.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/iterator/retag.h>

// There is a unfortunate miscompilation of the gcc-12 vectorizer leading to OOB writes
// Adding this attribute suffices that this miscompilation does not appear anymore
#if (THRUST_HOST_COMPILER == THRUST_HOST_COMPILER_GCC) && __GNUC__ >= 12
#define THRUST_DISABLE_BROKEN_GCC_VECTORIZER __attribute__((optimize("no-tree-vectorize")))
#else
#define THRUST_DISABLE_BROKEN_GCC_VECTORIZER
#endif

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformUnarySimple(void)
{
     using T = typename Vector::value_type;

    typename Vector::iterator iter;

    Vector input(3);
    Vector output(3);
    Vector result(3);
    input[0]  =  1; input[1]  = -2; input[2]  =  3;
    result[0] = -1; result[1] =  2; result[2] = -3;

    iter = thrust::transform(input.begin(), input.end(), output.begin(), thrust::negate<T>());

    ASSERT_EQUAL(std::size_t(iter - output.begin()), input.size());
    ASSERT_EQUAL(output, result);
}
DECLARE_VECTOR_UNITTEST(TestTransformUnarySimple);


template<typename InputIterator,
         typename OutputIterator,
         typename UnaryFunction>
THRUST_HOST_DEVICE
OutputIterator transform(my_system &system, InputIterator, InputIterator, OutputIterator result, UnaryFunction)
{
    system.validate_dispatch();
    return result;
}

void TestTransformUnaryDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::transform(sys,
                      vec.begin(),
                      vec.begin(),
                      vec.begin(),
                      0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestTransformUnaryDispatchExplicit);


template<typename InputIterator,
         typename OutputIterator,
         typename UnaryFunction>
THRUST_HOST_DEVICE
OutputIterator transform(my_tag, InputIterator, InputIterator, OutputIterator result, UnaryFunction)
{
    *result = 13;
    return result;
}

void TestTransformUnaryDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    thrust::transform(thrust::retag<my_tag>(vec.begin()),
                      thrust::retag<my_tag>(vec.begin()),
                      thrust::retag<my_tag>(vec.begin()),
                      0);

    ASSERT_EQUAL(13, vec.front());
}
DECLARE_UNITTEST(TestTransformUnaryDispatchImplicit);


template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfUnaryNoStencilSimple(void)
{
    using T = typename Vector::value_type;

    typename Vector::iterator iter;

    Vector input(3);
    Vector output(3);
    Vector result(3);

    input[0]   =  0; input[1]   = -2; input[2]   =  0;
    output[0]  = -1; output[1]  = -2; output[2]  = -3;
    result[0]  = -1; result[1]  =  2; result[2]  = -3;

    iter = thrust::transform_if(input.begin(), input.end(),
                                output.begin(),
                                thrust::negate<T>(),
                                thrust::identity<T>());

    ASSERT_EQUAL(std::size_t(iter - output.begin()), input.size());
    ASSERT_EQUAL(output, result);
}
DECLARE_VECTOR_UNITTEST(TestTransformIfUnaryNoStencilSimple);


template<typename InputIterator,
         typename ForwardIterator,
         typename UnaryFunction,
         typename Predicate>
THRUST_HOST_DEVICE
ForwardIterator transform_if(my_system &system,
                             InputIterator,
                             InputIterator,
                             ForwardIterator result,
                             UnaryFunction,
                             Predicate)
{
    system.validate_dispatch();
    return result;
}

void TestTransformIfUnaryNoStencilDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::transform_if(sys,
                         vec.begin(),
                         vec.begin(),
                         vec.begin(),
                         vec.begin(),
                         0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestTransformIfUnaryNoStencilDispatchExplicit);


template<typename InputIterator,
         typename ForwardIterator,
         typename UnaryFunction,
         typename Predicate>
THRUST_HOST_DEVICE
ForwardIterator transform_if(my_tag,
                             InputIterator,
                             InputIterator,
                             ForwardIterator result,
                             UnaryFunction,
                             Predicate)
{
    *result = 13;
    return result;
}

void TestTransformIfUnaryNoStencilDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    thrust::transform_if(thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         0);

    ASSERT_EQUAL(13, vec.front());
}
DECLARE_UNITTEST(TestTransformIfUnaryNoStencilDispatchImplicit);


template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfUnarySimple(void)
{
    using T = typename Vector::value_type;

    typename Vector::iterator iter;

    Vector input(3);
    Vector stencil(3);
    Vector output(3);
    Vector result(3);

    input[0]   =  1; input[1]   = -2; input[2]   =  3;
    output[0]  =  1; output[1]  =  2; output[2]  =  3;
    stencil[0] =  1; stencil[1] =  0; stencil[2] =  1;
    result[0]  = -1; result[1]  =  2; result[2]  = -3;

    iter = thrust::transform_if(input.begin(), input.end(),
                                stencil.begin(),
                                output.begin(),
                                thrust::negate<T>(),
                                thrust::identity<T>());

    ASSERT_EQUAL(std::size_t(iter - output.begin()), input.size());
    ASSERT_EQUAL(output, result);
}
DECLARE_VECTOR_UNITTEST(TestTransformIfUnarySimple);


template<typename InputIterator1,
         typename InputIterator2,
         typename ForwardIterator,
         typename UnaryFunction,
         typename Predicate>
THRUST_HOST_DEVICE
ForwardIterator transform_if(my_system &system,
                             InputIterator1,
                             InputIterator1,
                             ForwardIterator result,
                             UnaryFunction,
                             Predicate)
{
    system.validate_dispatch();
    return result;
}

void TestTransformIfUnaryDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::transform_if(sys,
                         vec.begin(),
                         vec.begin(),
                         vec.begin(),
                         0,
                         0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestTransformIfUnaryDispatchExplicit);


template<typename InputIterator1,
         typename InputIterator2,
         typename ForwardIterator,
         typename UnaryFunction,
         typename Predicate>
THRUST_HOST_DEVICE
ForwardIterator transform_if(my_tag,
                             InputIterator1,
                             InputIterator1,
                             ForwardIterator result,
                             UnaryFunction,
                             Predicate)
{
    *result = 13;
    return result;
}

void TestTransformIfUnaryDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    thrust::transform_if(thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         0,
                         0);

    ASSERT_EQUAL(13, vec.front());
}
DECLARE_UNITTEST(TestTransformIfUnaryDispatchImplicit);


template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformBinarySimple(void)
{
    using T = typename Vector::value_type;

    typename Vector::iterator iter;

    // There is a strange gcc bug here where it belives we would write out of bounds.
    // It seems to go away if we add one more element that we leave untouched. Luckily 0 - 0 = 0 so all is fine.
    // Note that we still write the element, so it does not hide a functional thrust bug
    Vector input1(4);
    Vector input2(4);
    Vector output(4);
    Vector result(4);
    input1[0] =  1; input1[1] = -2; input1[2] =  3;
    input2[0] = -4; input2[1] =  5; input2[2] =  6;
    result[0] =  5; result[1] = -7; result[2] = -3;

    iter = thrust::transform(input1.begin(), input1.end(), input2.begin(), output.begin(), thrust::minus<T>());

    ASSERT_EQUAL(std::size_t(iter - output.begin()), input1.size());
    ASSERT_EQUAL(output, result);
}
DECLARE_VECTOR_UNITTEST(TestTransformBinarySimple);


template<typename InputIterator1,
         typename InputIterator2,
         typename OutputIterator,
         typename UnaryFunction>
THRUST_HOST_DEVICE
OutputIterator transform(my_system &system, InputIterator1, InputIterator1, InputIterator2, OutputIterator result, UnaryFunction)
{
    system.validate_dispatch();
    return result;
}

void TestTransformBinaryDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::transform(sys,
                      vec.begin(),
                      vec.begin(),
                      vec.begin(),
                      vec.begin(),
                      0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestTransformBinaryDispatchExplicit);


template<typename InputIterator1,
         typename InputIterator2,
         typename OutputIterator,
         typename UnaryFunction>
THRUST_HOST_DEVICE
OutputIterator transform(my_tag, InputIterator1, InputIterator1, InputIterator2, OutputIterator result, UnaryFunction)
{
    *result = 13;
    return result;
}

void TestTransformBinaryDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    thrust::transform(thrust::retag<my_tag>(vec.begin()),
                      thrust::retag<my_tag>(vec.begin()),
                      thrust::retag<my_tag>(vec.begin()),
                      thrust::retag<my_tag>(vec.begin()),
                      0);

    ASSERT_EQUAL(13, vec.front());
}
DECLARE_UNITTEST(TestTransformBinaryDispatchImplicit);




template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfBinarySimple(void)
{
    using T = typename Vector::value_type;

    typename Vector::iterator iter;

    Vector input1(3);
    Vector input2(3);
    Vector stencil(3);
    Vector output(3);
    Vector result(3);

    input1[0]  =  1; input1[1]  = -2; input1[2]  =  3;
    input2[0]  = -4; input2[1]  =  5; input2[2]  =  6;
    stencil[0] =  0; stencil[1] =  1; stencil[2] =  0;
    output[0]  =  1; output[1]  =  2; output[2]  =  3;
    result[0]  =  5; result[1]  =  2; result[2]  = -3;

    thrust::identity<T> identity;

    iter = thrust::transform_if(
        input1.begin(),
        input1.end(),
        input2.begin(),
        stencil.begin(),
        output.begin(),
        thrust::minus<T>(),
        thrust::not_fn(identity));

    ASSERT_EQUAL(std::size_t(iter - output.begin()), input1.size());
    ASSERT_EQUAL(output, result);
}
DECLARE_VECTOR_UNITTEST(TestTransformIfBinarySimple);


template<typename InputIterator1,
         typename InputIterator2,
         typename InputIterator3,
         typename ForwardIterator,
         typename BinaryFunction,
         typename Predicate>
THRUST_HOST_DEVICE
ForwardIterator transform_if(my_system &system,
                             InputIterator1,
                             InputIterator1,
                             InputIterator2,
                             InputIterator3,
                             ForwardIterator result,
                             BinaryFunction,
                             Predicate)
{
    system.validate_dispatch();
    return result;
}

void TestTransformIfBinaryDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::transform_if(sys,
                         vec.begin(),
                         vec.begin(),
                         vec.begin(),
                         vec.begin(),
                         vec.begin(),
                         0,
                         0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestTransformIfBinaryDispatchExplicit);


template<typename InputIterator1,
         typename InputIterator2,
         typename InputIterator3,
         typename ForwardIterator,
         typename BinaryFunction,
         typename Predicate>
THRUST_HOST_DEVICE
ForwardIterator transform_if(my_tag,
                             InputIterator1,
                             InputIterator1,
                             InputIterator2,
                             InputIterator3,
                             ForwardIterator result,
                             BinaryFunction,
                             Predicate)
{
    *result = 13;
    return result;
}

void TestTransformIfBinaryDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    thrust::transform_if(thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         thrust::retag<my_tag>(vec.begin()),
                         0,
                         0);

    ASSERT_EQUAL(13, vec.front());
}
DECLARE_UNITTEST(TestTransformIfBinaryDispatchImplicit);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformUnary(const size_t n)
{
    thrust::host_vector<T>   h_input = unittest::random_integers<T>(n);
    thrust::device_vector<T> d_input = h_input;

    thrust::host_vector<T>   h_output(n);
    thrust::device_vector<T> d_output(n);

    thrust::transform(h_input.begin(), h_input.end(), h_output.begin(), thrust::negate<T>());
    thrust::transform(d_input.begin(), d_input.end(), d_output.begin(), thrust::negate<T>());

    ASSERT_EQUAL(h_output, d_output);
}
DECLARE_VARIABLE_UNITTEST(TestTransformUnary);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformUnaryToDiscardIterator(const size_t n)
{
    thrust::host_vector<T>   h_input = unittest::random_integers<T>(n);
    thrust::device_vector<T> d_input = h_input;

    thrust::discard_iterator<> h_result =
      thrust::transform(h_input.begin(), h_input.end(), thrust::make_discard_iterator(), thrust::negate<T>());

    thrust::discard_iterator<> d_result =
      thrust::transform(d_input.begin(), d_input.end(), thrust::make_discard_iterator(), thrust::negate<T>());

    thrust::discard_iterator<> reference(n);

    ASSERT_EQUAL_QUIET(reference, h_result);
    ASSERT_EQUAL_QUIET(reference, d_result);
}
DECLARE_VARIABLE_UNITTEST(TestTransformUnaryToDiscardIterator);


struct repeat2
{
  template<typename T>
  THRUST_HOST_DEVICE
  thrust::pair<T,T> operator()(T x)
  {
    return thrust::make_pair(x,x);
  }
};


template<typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformUnaryToDiscardIteratorZipped(const size_t n)
{
    thrust::host_vector<T>   h_input = unittest::random_integers<T>(n);
    thrust::device_vector<T> d_input = h_input;

    thrust::host_vector<T>   h_output(n);
    thrust::device_vector<T> d_output(n);

    using Iterator1 = typename thrust::host_vector<T>::iterator;
    using Iterator2 = typename thrust::device_vector<T>::iterator;

    using Tuple1 = thrust::tuple<Iterator1, thrust::discard_iterator<>>;
    using Tuple2 = thrust::tuple<Iterator2, thrust::discard_iterator<>>;

    using ZipIterator1 = thrust::zip_iterator<Tuple1>;
    using ZipIterator2 = thrust::zip_iterator<Tuple2>;

    ZipIterator1 z1(thrust::make_tuple(h_output.begin(), thrust::make_discard_iterator()));
    ZipIterator2 z2(thrust::make_tuple(d_output.begin(), thrust::make_discard_iterator()));

    ZipIterator1 h_result =
      thrust::transform(h_input.begin(), h_input.end(), z1, repeat2());

    ZipIterator2 d_result =
      thrust::transform(d_input.begin(), d_input.end(), z2, repeat2());

    thrust::discard_iterator<> reference(n);

    ASSERT_EQUAL(h_output, d_output);

    ASSERT_EQUAL_QUIET(reference, thrust::get<1>(h_result.get_iterator_tuple()));
    ASSERT_EQUAL_QUIET(reference, thrust::get<1>(d_result.get_iterator_tuple()));
}
DECLARE_VARIABLE_UNITTEST(TestTransformUnaryToDiscardIteratorZipped);

struct is_positive
{
  template<typename T>
  THRUST_HOST_DEVICE
  bool operator()(T &x)
  {
    return x > 0;
  } // end operator()()
}; // end is_positive


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfUnaryNoStencil(const size_t n)
{
    thrust::host_vector<T>   h_input   = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_output  = unittest::random_integers<T>(n);

    thrust::device_vector<T> d_input   = h_input;
    thrust::device_vector<T> d_output  = h_output;

    thrust::transform_if(h_input.begin(), h_input.end(),
                         h_output.begin(),
                         thrust::negate<T>(), is_positive());

    thrust::transform_if(d_input.begin(), d_input.end(),
                         d_output.begin(),
                         thrust::negate<T>(), is_positive());

    ASSERT_EQUAL(h_output, d_output);
}
DECLARE_VARIABLE_UNITTEST(TestTransformIfUnaryNoStencil);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfUnary(const size_t n)
{
    thrust::host_vector<T>   h_input   = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_stencil = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_output  = unittest::random_integers<T>(n);

    thrust::device_vector<T> d_input   = h_input;
    thrust::device_vector<T> d_stencil = h_stencil;
    thrust::device_vector<T> d_output  = h_output;

    thrust::transform_if(h_input.begin(), h_input.end(),
                          h_stencil.begin(),
                          h_output.begin(),
                          thrust::negate<T>(), is_positive());

    thrust::transform_if(d_input.begin(), d_input.end(),
                          d_stencil.begin(),
                          d_output.begin(),
                          thrust::negate<T>(), is_positive());

    ASSERT_EQUAL(h_output, d_output);
}
DECLARE_VARIABLE_UNITTEST(TestTransformIfUnary);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfUnaryToDiscardIterator(const size_t n)
{
    thrust::host_vector<T>   h_input   = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_stencil = unittest::random_integers<T>(n);

    thrust::device_vector<T> d_input   = h_input;
    thrust::device_vector<T> d_stencil = h_stencil;

    thrust::discard_iterator<> h_result =
      thrust::transform_if(h_input.begin(), h_input.end(),
                           h_stencil.begin(),
                           thrust::make_discard_iterator(),
                           thrust::negate<T>(), is_positive());

    thrust::discard_iterator<> d_result =
      thrust::transform_if(d_input.begin(), d_input.end(),
                           d_stencil.begin(),
                           thrust::make_discard_iterator(),
                           thrust::negate<T>(), is_positive());

    thrust::discard_iterator<> reference(n);

    ASSERT_EQUAL_QUIET(reference, h_result);
    ASSERT_EQUAL_QUIET(reference, d_result);
}
DECLARE_VARIABLE_UNITTEST(TestTransformIfUnaryToDiscardIterator);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformBinary(const size_t n)
{
    thrust::host_vector<T>   h_input1 = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_input2 = unittest::random_integers<T>(n);
    thrust::device_vector<T> d_input1 = h_input1;
    thrust::device_vector<T> d_input2 = h_input2;

    thrust::host_vector<T>   h_output(n);
    thrust::device_vector<T> d_output(n);

    thrust::transform(h_input1.begin(), h_input1.end(), h_input2.begin(), h_output.begin(), thrust::minus<T>());
    thrust::transform(d_input1.begin(), d_input1.end(), d_input2.begin(), d_output.begin(), thrust::minus<T>());

    ASSERT_EQUAL(h_output, d_output);

    thrust::transform(h_input1.begin(), h_input1.end(), h_input2.begin(), h_output.begin(), thrust::multiplies<T>());
    thrust::transform(d_input1.begin(), d_input1.end(), d_input2.begin(), d_output.begin(), thrust::multiplies<T>());

    ASSERT_EQUAL(h_output, d_output);
}
DECLARE_VARIABLE_UNITTEST(TestTransformBinary);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformBinaryToDiscardIterator(const size_t n)
{
    thrust::host_vector<T>   h_input1 = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_input2 = unittest::random_integers<T>(n);
    thrust::device_vector<T> d_input1 = h_input1;
    thrust::device_vector<T> d_input2 = h_input2;

    thrust::discard_iterator<> h_result =
      thrust::transform(h_input1.begin(), h_input1.end(), h_input2.begin(), thrust::make_discard_iterator(), thrust::minus<T>());
    thrust::discard_iterator<> d_result =
      thrust::transform(d_input1.begin(), d_input1.end(), d_input2.begin(), thrust::make_discard_iterator(), thrust::minus<T>());

    thrust::discard_iterator<> reference(n);

    ASSERT_EQUAL_QUIET(reference, h_result);
    ASSERT_EQUAL_QUIET(reference, d_result);
}
DECLARE_VARIABLE_UNITTEST(TestTransformBinaryToDiscardIterator);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfBinary(const size_t n)
{
    thrust::host_vector<T>   h_input1  = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_input2  = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_stencil = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_output  = unittest::random_integers<T>(n);

    thrust::device_vector<T> d_input1  = h_input1;
    thrust::device_vector<T> d_input2  = h_input2;
    thrust::device_vector<T> d_stencil = h_stencil;
    thrust::device_vector<T> d_output  = h_output;

    thrust::transform_if(h_input1.begin(), h_input1.end(),
                         h_input2.begin(),
                         h_stencil.begin(),
                         h_output.begin(),
                         thrust::minus<T>(), is_positive());

    thrust::transform_if(d_input1.begin(), d_input1.end(),
                         d_input2.begin(),
                         d_stencil.begin(),
                         d_output.begin(),
                         thrust::minus<T>(), is_positive());

    ASSERT_EQUAL(h_output, d_output);

    h_stencil = unittest::random_integers<T>(n);
    d_stencil = h_stencil;

    thrust::transform_if(h_input1.begin(), h_input1.end(),
                         h_input2.begin(),
                         h_stencil.begin(),
                         h_output.begin(),
                         thrust::multiplies<T>(), is_positive());

    thrust::transform_if(d_input1.begin(), d_input1.end(),
                         d_input2.begin(),
                         d_stencil.begin(),
                         d_output.begin(),
                         thrust::multiplies<T>(), is_positive());

    ASSERT_EQUAL(h_output, d_output);
}
DECLARE_VARIABLE_UNITTEST(TestTransformIfBinary);


template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformIfBinaryToDiscardIterator(const size_t n)
{
    thrust::host_vector<T>   h_input1  = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_input2  = unittest::random_integers<T>(n);
    thrust::host_vector<T>   h_stencil = unittest::random_integers<T>(n);

    thrust::device_vector<T> d_input1  = h_input1;
    thrust::device_vector<T> d_input2  = h_input2;
    thrust::device_vector<T> d_stencil = h_stencil;

    thrust::discard_iterator<> h_result =
      thrust::transform_if(h_input1.begin(), h_input1.end(),
                           h_input2.begin(),
                           h_stencil.begin(),
                           thrust::make_discard_iterator(),
                           thrust::minus<T>(), is_positive());

    thrust::discard_iterator<> d_result =
      thrust::transform_if(d_input1.begin(), d_input1.end(),
                           d_input2.begin(),
                           d_stencil.begin(),
                           thrust::make_discard_iterator(),
                           thrust::minus<T>(), is_positive());

    thrust::discard_iterator<> reference(n);

    ASSERT_EQUAL_QUIET(reference, h_result);
    ASSERT_EQUAL_QUIET(reference, d_result);
}
DECLARE_VARIABLE_UNITTEST(TestTransformIfBinaryToDiscardIterator);


#if ((__GNUC__ * 10000 + __GNUC_MINOR__ * 100) == 40400) || defined(__INTEL_COMPILER)
template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformUnaryCountingIterator()
{
    // G++ 4.4.x has a known failure with auto-vectorization (due to -O3 or
    // -ftree-vectorize) of this test.
    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=43251

    // ICPC has a known failure with auto-vectorization (due to -O2 or
    // higher) of this test.
    // See nvbug 200326708.
    KNOWN_FAILURE;
}
#else
template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformUnaryCountingIterator()
{
    size_t const n = 15 * sizeof(T);

    ASSERT_LEQUAL(T(n), unittest::truncate_to_max_representable<T>(n));

    thrust::counting_iterator<T, thrust::host_system_tag>   h_first = thrust::make_counting_iterator<T>(0);
    thrust::counting_iterator<T, thrust::device_system_tag> d_first = thrust::make_counting_iterator<T>(0);

    thrust::host_vector<T>   h_result(n);
    thrust::device_vector<T> d_result(n);

    thrust::transform(h_first, h_first + n, h_result.begin(), thrust::identity<T>());
    thrust::transform(d_first, d_first + n, d_result.begin(), thrust::identity<T>());

    ASSERT_EQUAL(h_result, d_result);
}
#endif
DECLARE_GENERIC_UNITTEST(TestTransformUnaryCountingIterator);

#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100) == 40400
template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformBinaryCountingIterator()
{
    // GCC 4.4.x has a known failure with auto-vectorization (due to -O3 or -ftree-vectorize) of this test
    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=43251

    KNOWN_FAILURE;
}
#else
template <typename T>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformBinaryCountingIterator()
{
    size_t const n = 15 * sizeof(T);

    ASSERT_LEQUAL(T(n), unittest::truncate_to_max_representable<T>(n));

    thrust::counting_iterator<T, thrust::host_system_tag>   h_first = thrust::make_counting_iterator<T>(0);
    thrust::counting_iterator<T, thrust::device_system_tag> d_first = thrust::make_counting_iterator<T>(0);

    thrust::host_vector<T>   h_result(n);
    thrust::device_vector<T> d_result(n);

    thrust::transform(h_first, h_first + n, h_first, h_result.begin(), thrust::plus<T>());
    thrust::transform(d_first, d_first + n, d_first, d_result.begin(), thrust::plus<T>());

    ASSERT_EQUAL(h_result, d_result);
}
#endif
DECLARE_GENERIC_UNITTEST(TestTransformBinaryCountingIterator);


template <typename T>
struct plus_mod3
{
    T * table;

    plus_mod3(T * table) : table(table) {}

    THRUST_HOST_DEVICE
    T operator()(T a, T b)
    {
        return table[(int) (a + b)];
    }
};

template <typename Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestTransformWithIndirection(void)
{
    // add numbers modulo 3 with external lookup table
    using T = typename Vector::value_type;

    Vector input1(7);
    Vector input2(7);
    Vector output(7, 0);
    input1[0] = 0;  input2[0] = 2;
    input1[1] = 1;  input2[1] = 2;
    input1[2] = 2;  input2[2] = 2;
    input1[3] = 1;  input2[3] = 0;
    input1[4] = 2;  input2[4] = 2;
    input1[5] = 0;  input2[5] = 1;
    input1[6] = 1;  input2[6] = 0;

    Vector table(6);
    table[0] = 0;
    table[1] = 1;
    table[2] = 2;
    table[3] = 0;
    table[4] = 1;
    table[5] = 2;

    thrust::transform(input1.begin(), input1.end(),
                      input2.begin(),
                      output.begin(),
                      plus_mod3<T>(thrust::raw_pointer_cast(&table[0])));

    ASSERT_EQUAL(output[0], T(2));
    ASSERT_EQUAL(output[1], T(0));
    ASSERT_EQUAL(output[2], T(1));
    ASSERT_EQUAL(output[3], T(1));
    ASSERT_EQUAL(output[4], T(1));
    ASSERT_EQUAL(output[5], T(1));
    ASSERT_EQUAL(output[6], T(1));
}
DECLARE_INTEGRAL_VECTOR_UNITTEST(TestTransformWithIndirection);

