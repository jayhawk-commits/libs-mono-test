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
#include <thrust/uninitialized_fill.h>
#include <thrust/device_malloc_allocator.h>
#include <thrust/iterator/retag.h>

#include <thrust/detail/nv_target.h>

template<typename ForwardIterator, typename T>
void uninitialized_fill(my_system &system,
                        ForwardIterator,
                        ForwardIterator,
                        const T &)
{
    system.validate_dispatch();
}

void TestUninitializedFillDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::uninitialized_fill(sys, vec.begin(), vec.begin(), 0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestUninitializedFillDispatchExplicit);


template<typename ForwardIterator, typename T>
void uninitialized_fill(my_tag,
                        ForwardIterator first,
                        ForwardIterator,
                        const T &)
{
    *first = 13;
}

void TestUninitializedFillDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    thrust::uninitialized_fill(thrust::retag<my_tag>(vec.begin()),
                               thrust::retag<my_tag>(vec.begin()),
                               0);

    ASSERT_EQUAL(13, vec.front());
}
DECLARE_UNITTEST(TestUninitializedFillDispatchImplicit);


template<typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n(my_system &system,
                                     ForwardIterator first,
                                     Size,
                                     const T &)
{
    system.validate_dispatch();
    return first;
}

void TestUninitializedFillNDispatchExplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::uninitialized_fill_n(sys, vec.begin(), vec.size(), 0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestUninitializedFillNDispatchExplicit);


template<typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n(my_tag,
                                     ForwardIterator first,
                                     Size,
                                     const T &)
{
    *first = 13;
    return first;
}

void TestUninitializedFillNDispatchImplicit()
{
    thrust::device_vector<int> vec(1);

    my_system sys(0);
    thrust::uninitialized_fill_n(sys,
                                 vec.begin(),
                                 vec.size(),
                                 0);

    ASSERT_EQUAL(true, sys.is_valid());
}
DECLARE_UNITTEST(TestUninitializedFillNDispatchImplicit);


template <class Vector>
void TestUninitializedFillPOD(void)
{
    using T = typename Vector::value_type;

    Vector v(5);
    v[0] = 0; v[1] = 1; v[2] = 2; v[3] = 3; v[4] = 4;

    T exemplar(7);

    thrust::uninitialized_fill(v.begin() + 1, v.begin() + 4, exemplar);

    ASSERT_EQUAL(v[0], 0);
    ASSERT_EQUAL(v[1], exemplar);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], exemplar);
    ASSERT_EQUAL(v[4], 4);

    exemplar = 8;
    
    thrust::uninitialized_fill(v.begin() + 0, v.begin() + 3, exemplar);
    
    ASSERT_EQUAL(v[0], exemplar);
    ASSERT_EQUAL(v[1], exemplar);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], 7);
    ASSERT_EQUAL(v[4], 4);

    exemplar = 9;
    
    thrust::uninitialized_fill(v.begin() + 2, v.end(), exemplar);
    
    ASSERT_EQUAL(v[0], 8);
    ASSERT_EQUAL(v[1], 8);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], exemplar);
    ASSERT_EQUAL(v[4], 9);

    exemplar = 1;

    thrust::uninitialized_fill(v.begin(), v.end(), exemplar);
    
    ASSERT_EQUAL(v[0], exemplar);
    ASSERT_EQUAL(v[1], exemplar);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], exemplar);
    ASSERT_EQUAL(v[4], exemplar);
}
DECLARE_VECTOR_UNITTEST(TestUninitializedFillPOD);


struct CopyConstructTest
{
  THRUST_HOST_DEVICE
  CopyConstructTest(void)
    :copy_constructed_on_host(false),
     copy_constructed_on_device(false)
  {}

  THRUST_HOST_DEVICE
  CopyConstructTest(const CopyConstructTest &)
  {
    NV_IF_TARGET(NV_IS_DEVICE, (
      copy_constructed_on_device = true;
      copy_constructed_on_host   = false;
    ), (
      copy_constructed_on_device = false;
      copy_constructed_on_host   = true;
    ));
  }

  THRUST_HOST_DEVICE
  CopyConstructTest &operator=(const CopyConstructTest &x)
  {
    copy_constructed_on_host   = x.copy_constructed_on_host;
    copy_constructed_on_device = x.copy_constructed_on_device;
    return *this;
  }

  bool copy_constructed_on_host;
  bool copy_constructed_on_device;
};


struct TestUninitializedFillNonPOD
{
  void operator()(const size_t)
  {
    using T                 = CopyConstructTest;
    thrust::device_ptr<T> v = thrust::device_malloc<T>(5);

    T exemplar;
    ASSERT_EQUAL(false, exemplar.copy_constructed_on_device);
    ASSERT_EQUAL(false, exemplar.copy_constructed_on_host);

    T host_copy_of_exemplar(exemplar);
    ASSERT_EQUAL(false, exemplar.copy_constructed_on_device);
    ASSERT_EQUAL(true,  exemplar.copy_constructed_on_host);

    // copy construct v from the exemplar
    thrust::uninitialized_fill(v, v + 1, exemplar);

    T x;
    ASSERT_EQUAL(false,  x.copy_constructed_on_device);
    ASSERT_EQUAL(false,  x.copy_constructed_on_host);

    x = v[0];
    ASSERT_EQUAL(true,   x.copy_constructed_on_device);
    ASSERT_EQUAL(false,  x.copy_constructed_on_host);

    thrust::device_free(v);
  }
};
DECLARE_UNITTEST(TestUninitializedFillNonPOD);

template <class Vector>
void TestUninitializedFillNPOD(void)
{
    using T = typename Vector::value_type;

    Vector v(5);
    v[0] = 0; v[1] = 1; v[2] = 2; v[3] = 3; v[4] = 4;

    T exemplar(7);

    typename Vector::iterator iter = thrust::uninitialized_fill_n(v.begin() + 1, 3, exemplar);

    ASSERT_EQUAL(v[0], 0);
    ASSERT_EQUAL(v[1], exemplar);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], exemplar);
    ASSERT_EQUAL(v[4], 4);
    ASSERT_EQUAL_QUIET(v.begin() + 4, iter);

    exemplar = 8;
    
    iter = thrust::uninitialized_fill_n(v.begin() + 0, 3, exemplar);
    
    ASSERT_EQUAL(v[0], exemplar);
    ASSERT_EQUAL(v[1], exemplar);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], 7);
    ASSERT_EQUAL(v[4], 4);
    ASSERT_EQUAL_QUIET(v.begin() + 3, iter);

    exemplar = 9;
    
    iter = thrust::uninitialized_fill_n(v.begin() + 2, 3, exemplar);
    
    ASSERT_EQUAL(v[0], 8);
    ASSERT_EQUAL(v[1], 8);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], exemplar);
    ASSERT_EQUAL(v[4], 9);
    ASSERT_EQUAL_QUIET(v.end(), iter);

    exemplar = 1;

    iter = thrust::uninitialized_fill_n(v.begin(), v.size(), exemplar);
    
    ASSERT_EQUAL(v[0], exemplar);
    ASSERT_EQUAL(v[1], exemplar);
    ASSERT_EQUAL(v[2], exemplar);
    ASSERT_EQUAL(v[3], exemplar);
    ASSERT_EQUAL(v[4], exemplar);
    ASSERT_EQUAL_QUIET(v.end(), iter);
}
DECLARE_VECTOR_UNITTEST(TestUninitializedFillNPOD);


struct TestUninitializedFillNNonPOD
{
  void operator()(const size_t)
  {
    using T                 = CopyConstructTest;
    thrust::device_ptr<T> v = thrust::device_malloc<T>(5);

    T exemplar;
    ASSERT_EQUAL(false, exemplar.copy_constructed_on_device);
    ASSERT_EQUAL(false, exemplar.copy_constructed_on_host);

    T host_copy_of_exemplar(exemplar);
    ASSERT_EQUAL(false, exemplar.copy_constructed_on_device);
    ASSERT_EQUAL(true,  exemplar.copy_constructed_on_host);

    // copy construct v from the exemplar
    thrust::uninitialized_fill_n(v, 1, exemplar);

    T x;
    ASSERT_EQUAL(false,  x.copy_constructed_on_device);
    ASSERT_EQUAL(false,  x.copy_constructed_on_host);

    x = v[0];
    ASSERT_EQUAL(true,   x.copy_constructed_on_device);
    ASSERT_EQUAL(false,  x.copy_constructed_on_host);

    thrust::device_free(v);
  }
};
DECLARE_UNITTEST(TestUninitializedFillNNonPOD);

