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
#include <thrust/transform.h>

#include <functional>
#include <algorithm>

THRUST_DISABLE_MSVC_POSSIBLE_LOSS_OF_DATA_WARNING_BEGIN

// There is a unfortunate miscompilation of the gcc-12 vectorizer leading to OOB writes
// Adding this attribute suffices that this miscompilation does not appear anymore
#if (THRUST_HOST_COMPILER == THRUST_HOST_COMPILER_GCC) && __GNUC__ >= 12
#define THRUST_DISABLE_BROKEN_GCC_VECTORIZER __attribute__((optimize("no-tree-vectorize")))
#else
#define THRUST_DISABLE_BROKEN_GCC_VECTORIZER
#endif

const size_t NUM_SAMPLES = 10000;

template <class InputVector, class OutputVector, class Operator, class ReferenceOperator>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestUnaryFunctional(void)
{
    using InputType  = typename InputVector::value_type;
    using OutputType = typename OutputVector::value_type;

    thrust::host_vector<InputType>  std_input = unittest::random_samples<InputType>(NUM_SAMPLES);
    thrust::host_vector<OutputType> std_output(NUM_SAMPLES);

    InputVector  input = std_input;
    OutputVector output(NUM_SAMPLES);

    thrust::transform(    input.begin(),     input.end(),     output.begin(),          Operator());
    thrust::transform(std_input.begin(), std_input.end(), std_output.begin(), ReferenceOperator());

    ASSERT_EQUAL(output, std_output);
}

template <class InputVector, class OutputVector, class Operator, class ReferenceOperator>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestBinaryFunctional(void)
{
    using InputType  = typename InputVector::value_type;
    using OutputType = typename OutputVector::value_type;

    thrust::host_vector<InputType>  std_input1 = unittest::random_samples<InputType>(NUM_SAMPLES);
    thrust::host_vector<InputType>  std_input2 = unittest::random_samples<InputType>(NUM_SAMPLES);
    thrust::host_vector<OutputType> std_output(NUM_SAMPLES);

    // Replace zeros to avoid divide by zero exceptions
    std::replace(std_input2.begin(), std_input2.end(), (InputType) 0, (InputType) 1);

    InputVector input1 = std_input1;
    InputVector input2 = std_input2;
    OutputVector output(NUM_SAMPLES);

    thrust::transform(    input1.begin(),     input1.end(),      input2.begin(),     output.begin(),          Operator());
    thrust::transform(std_input1.begin(), std_input1.end(),  std_input2.begin(), std_output.begin(), ReferenceOperator());

    // Note: FP division is not bit-equal, even when nvcc is invoked with --prec-div
    ASSERT_ALMOST_EQUAL(output, std_output);
}

// XXX add bool to list
// Instantiate a macro for all integer-like data types
#define INSTANTIATE_INTEGER_TYPES(Macro, vector_type, operator_name)   \
Macro(vector_type, operator_name, unittest::int8_t  )                  \
Macro(vector_type, operator_name, unittest::uint8_t )                  \
Macro(vector_type, operator_name, unittest::int16_t )                  \
Macro(vector_type, operator_name, unittest::uint16_t)                  \
Macro(vector_type, operator_name, unittest::int32_t )                  \
Macro(vector_type, operator_name, unittest::uint32_t)                  \
Macro(vector_type, operator_name, unittest::int64_t )                  \
Macro(vector_type, operator_name, unittest::uint64_t)

// Instantiate a macro for all integer and floating point data types
#define INSTANTIATE_ALL_TYPES(Macro, vector_type, operator_name)       \
INSTANTIATE_INTEGER_TYPES(Macro, vector_type, operator_name)           \
Macro(vector_type, operator_name, float)


// op(T) -> T
#define INSTANTIATE_UNARY_ARITHMETIC_FUNCTIONAL_TEST(vector_type, operator_name, data_type) \
    TestUnaryFunctional< thrust::vector_type<data_type>,                                   \
                         thrust::vector_type<data_type>,                                   \
                         thrust::operator_name<data_type>,                                 \
                         std::operator_name<data_type> >();
// XXX revert OutputVector<T> back to bool
// op(T) -> bool
#define INSTANTIATE_UNARY_LOGICAL_FUNCTIONAL_TEST(vector_type, operator_name, data_type) \
    TestUnaryFunctional< thrust::vector_type<data_type>,                                \
                         thrust::vector_type<data_type>,                                \
                         thrust::operator_name<data_type>,                              \
                         std::operator_name<data_type> >();
// op(T,T) -> T
#define INSTANTIATE_BINARY_ARITHMETIC_FUNCTIONAL_TEST(vector_type, operator_name, data_type) \
    TestBinaryFunctional< thrust::vector_type<data_type>,                                   \
                          thrust::vector_type<data_type>,                                   \
                          thrust::operator_name<data_type>,                                 \
                          std::operator_name<data_type> >();
// XXX revert OutputVector<T> back to bool
// op(T,T) -> bool
#define INSTANTIATE_BINARY_LOGICAL_FUNCTIONAL_TEST(vector_type, operator_name, data_type) \
    TestBinaryFunctional< thrust::vector_type<data_type>,                                \
                          thrust::vector_type<data_type>,                                \
                          thrust::operator_name<data_type>,                              \
                          std::operator_name<data_type> >();




// op(T) -> T
#define DECLARE_UNARY_ARITHMETIC_FUNCTIONAL_UNITTEST(operator_name, OperatorName)                          \
void Test##OperatorName##FunctionalHost(void)                                                              \
{                                                                                                          \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_UNARY_ARITHMETIC_FUNCTIONAL_TEST, host_vector,   operator_name);    \
}                                                                                                          \
DECLARE_UNITTEST(Test##OperatorName##FunctionalHost);                                                      \
void Test##OperatorName##FunctionalDevice(void)                                                            \
{                                                                                                          \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_UNARY_ARITHMETIC_FUNCTIONAL_TEST, device_vector, operator_name);    \
}                                                                                                          \
DECLARE_UNITTEST(Test##OperatorName##FunctionalDevice);

// op(T) -> bool
#define DECLARE_UNARY_LOGICAL_FUNCTIONAL_UNITTEST(operator_name, OperatorName)                             \
void Test##OperatorName##FunctionalHost(void)                                                              \
{                                                                                                          \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_UNARY_LOGICAL_FUNCTIONAL_TEST, host_vector,   operator_name);       \
}                                                                                                          \
DECLARE_UNITTEST(Test##OperatorName##FunctionalHost);                                                      \
void Test##OperatorName##FunctionalDevice(void)                                                            \
{                                                                                                          \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_UNARY_LOGICAL_FUNCTIONAL_TEST, device_vector, operator_name);       \
}                                                                                                          \
DECLARE_UNITTEST(Test##OperatorName##FunctionalDevice);

// op(T,T) -> T
#define DECLARE_BINARY_ARITHMETIC_FUNCTIONAL_UNITTEST(operator_name, OperatorName)                         \
void Test##OperatorName##FunctionalHost(void)                                                              \
{                                                                                                          \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_BINARY_ARITHMETIC_FUNCTIONAL_TEST, host_vector,   operator_name);   \
}                                                                                                          \
DECLARE_UNITTEST(Test##OperatorName##FunctionalHost);                                                      \
void Test##OperatorName##FunctionalDevice(void)                                                            \
{                                                                                                          \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_BINARY_ARITHMETIC_FUNCTIONAL_TEST, device_vector, operator_name);   \
}                                                                                                          \
DECLARE_UNITTEST(Test##OperatorName##FunctionalDevice);

// op(T,T) -> T (for integer T only)
#define DECLARE_BINARY_INTEGER_ARITHMETIC_FUNCTIONAL_UNITTEST(operator_name, OperatorName)                     \
void Test##OperatorName##FunctionalHost(void)                                                                  \
{                                                                                                              \
    INSTANTIATE_INTEGER_TYPES( INSTANTIATE_BINARY_ARITHMETIC_FUNCTIONAL_TEST, host_vector,   operator_name);   \
}                                                                                                              \
DECLARE_UNITTEST(Test##OperatorName##FunctionalHost);                                                          \
void Test##OperatorName##FunctionalDevice(void)                                                                \
{                                                                                                              \
    INSTANTIATE_INTEGER_TYPES( INSTANTIATE_BINARY_ARITHMETIC_FUNCTIONAL_TEST, device_vector, operator_name);   \
}                                                                                                              \
DECLARE_UNITTEST(Test##OperatorName##FunctionalDevice);

// op(T,T) -> bool
#define DECLARE_BINARY_LOGICAL_FUNCTIONAL_UNITTEST(operator_name, OperatorName)                         \
void Test##OperatorName##FunctionalHost(void)                                                           \
{                                                                                                       \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_BINARY_LOGICAL_FUNCTIONAL_TEST, host_vector,   operator_name);   \
}                                                                                                       \
DECLARE_UNITTEST(Test##OperatorName##FunctionalHost);                                                   \
void Test##OperatorName##FunctionalDevice(void)                                                         \
{                                                                                                       \
    INSTANTIATE_ALL_TYPES( INSTANTIATE_BINARY_LOGICAL_FUNCTIONAL_TEST, device_vector, operator_name);   \
}                                                                                                       \
DECLARE_UNITTEST(Test##OperatorName##FunctionalDevice);




// Create the unit tests
DECLARE_UNARY_ARITHMETIC_FUNCTIONAL_UNITTEST(negate, Negate);
DECLARE_UNARY_LOGICAL_FUNCTIONAL_UNITTEST(logical_not, LogicalNot);

// Ad-hoc testing for other functionals
template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestIdentityFunctional(void)
{
    using T = typename Vector::value_type;

    Vector input(4);
    input[0] = 0; input[1] = 1; input[2] = 2; input[3] = 3;

    Vector output(4);

    thrust::transform(input.begin(), input.end(), output.begin(), thrust::identity<T>());

    ASSERT_EQUAL(input, output);
}
DECLARE_VECTOR_UNITTEST(TestIdentityFunctional);

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestProject1stFunctional(void)
{
    using T = typename Vector::value_type;

    Vector lhs(4);
    Vector rhs(4);
    lhs[0] = 0;  rhs[0] = 3;
    lhs[1] = 1;  rhs[1] = 4;
    lhs[2] = 2;  rhs[2] = 5;
    lhs[2] = 3;  rhs[2] = 6;

    Vector output(4);

    thrust::transform(lhs.begin(), lhs.end(), rhs.begin(), output.begin(), thrust::project1st<T,T>());

    ASSERT_EQUAL(output, lhs);
}
DECLARE_VECTOR_UNITTEST(TestProject1stFunctional);

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestProject2ndFunctional(void)
{
    using T = typename Vector::value_type;

    Vector lhs(4);
    Vector rhs(4);
    lhs[0] = 0;  rhs[0] = 3;
    lhs[1] = 1;  rhs[1] = 4;
    lhs[2] = 2;  rhs[2] = 5;
    lhs[2] = 3;  rhs[2] = 6;

    Vector output(4);

    thrust::transform(lhs.begin(), lhs.end(), rhs.begin(), output.begin(), thrust::project2nd<T,T>());

    ASSERT_EQUAL(output, rhs);
}
DECLARE_VECTOR_UNITTEST(TestProject2ndFunctional);

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestMaximumFunctional(void)
{
    using T = typename Vector::value_type;

    Vector input1(4);
    Vector input2(4);
    input1[0] = 8; input1[1] = 3; input1[2] = 7; input1[3] = 7;
    input2[0] = 5; input2[1] = 6; input2[2] = 9; input2[3] = 3;

    Vector output(4);

    thrust::transform(input1.begin(), input1.end(),
                      input2.begin(),
                      output.begin(),
                      thrust::maximum<T>());

    ASSERT_EQUAL(output[0], 8);
    ASSERT_EQUAL(output[1], 6);
    ASSERT_EQUAL(output[2], 9);
    ASSERT_EQUAL(output[3], 7);
}
DECLARE_VECTOR_UNITTEST(TestMaximumFunctional);

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestMinimumFunctional(void)
{
    using T = typename Vector::value_type;

    Vector input1(4);
    Vector input2(4);
    input1[0] = 8; input1[1] = 3; input1[2] = 7; input1[3] = 7;
    input2[0] = 5; input2[1] = 6; input2[2] = 9; input2[3] = 3;

    Vector output(4);

    thrust::transform(input1.begin(), input1.end(),
                      input2.begin(),
                      output.begin(),
                      thrust::minimum<T>());

    ASSERT_EQUAL(output[0], 5);
    ASSERT_EQUAL(output[1], 3);
    ASSERT_EQUAL(output[2], 7);
    ASSERT_EQUAL(output[3], 3);
}
DECLARE_VECTOR_UNITTEST(TestMinimumFunctional);

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestNot1(void)
{
    using T = typename Vector::value_type;

    Vector input(5);
    input[0] = 1; input[1] = 0; input[2] = 1; input[3] = 1; input[4] = 0;

    Vector output(5);

    thrust::transform(input.begin(),
                      input.end(),
                      output.begin(),
                      thrust::not_fn(thrust::identity<T>()));

    ASSERT_EQUAL(output[0], 0);
    ASSERT_EQUAL(output[1], 1);
    ASSERT_EQUAL(output[2], 0);
    ASSERT_EQUAL(output[3], 0);
    ASSERT_EQUAL(output[4], 1);
}
DECLARE_INTEGRAL_VECTOR_UNITTEST(TestNot1);


// GCC 11 fails to build this test case with a spurious error in a
// very specific scenario:
// - GCC 11
// - CPP system for both host and device
// - C++11 dialect
#if !(defined(THRUST_GCC_VERSION) &&				\
      THRUST_GCC_VERSION >= 110000 &&				\
      THRUST_GCC_VERSION < 120000 &&				\
      THRUST_HOST_SYSTEM == THRUST_HOST_SYSTEM_CPP &&		\
      THRUST_DEVICE_SYSTEM == THRUST_DEVICE_SYSTEM_CPP &&	\
      THRUST_CPP_DIALECT == 2011)

template <class Vector>
THRUST_DISABLE_BROKEN_GCC_VECTORIZER
void TestNot2(void)
{
  using T = typename Vector::value_type;

  Vector input1(5);
  Vector input2(5);
  input1[0] = 1;
  input1[1] = 0;
  input1[2] = 1;
  input1[3] = 1;
  input1[4] = 0;
  input2[0] = 1;
  input2[1] = 1;
  input2[2] = 0;
  input2[3] = 1;
  input2[4] = 1;

  Vector output(5);

  thrust::transform(input1.begin(), input1.end(),
                    input2.begin(),
                    output.begin(),
                    thrust::not_fn(thrust::equal_to<T>()));

  ASSERT_EQUAL(output[0], 0);
  ASSERT_EQUAL(output[1], 1);
  ASSERT_EQUAL(output[2], 1);
  ASSERT_EQUAL(output[3], 0);
  ASSERT_EQUAL(output[4], 1);
}
DECLARE_VECTOR_UNITTEST(TestNot2);

#endif // Weird GCC11 failure case

THRUST_DISABLE_MSVC_POSSIBLE_LOSS_OF_DATA_WARNING_END
