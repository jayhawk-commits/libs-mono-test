/*
 *  Copyright 2008-2013 NVIDIA Corporation
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

#pragma once

#include <thrust/detail/config.h>
#include <thrust/binary_search.h>
#include <thrust/iterator/iterator_traits.h>
#include <thrust/system/detail/generic/select_system.h>
#include <thrust/system/detail/generic/binary_search.h>
#include <thrust/system/detail/adl/binary_search.h>

THRUST_NAMESPACE_BEGIN

THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename LessThanComparable>
THRUST_HOST_DEVICE
ForwardIterator lower_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                            ForwardIterator first,
                            ForwardIterator last,
                            const LessThanComparable &value)
{
    using thrust::system::detail::generic::lower_bound;
    return lower_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value);
}


THRUST_EXEC_CHECK_DISABLE
template<typename DerivedPolicy, typename ForwardIterator, typename T, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
ForwardIterator lower_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                            ForwardIterator first,
                            ForwardIterator last,
                            const T &value,
                            StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::lower_bound;
    return lower_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value, comp);
}


THRUST_EXEC_CHECK_DISABLE
template<typename DerivedPolicy, typename ForwardIterator, typename LessThanComparable>
THRUST_HOST_DEVICE
ForwardIterator upper_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                            ForwardIterator first,
                            ForwardIterator last,
                            const LessThanComparable &value)
{
    using thrust::system::detail::generic::upper_bound;
    return upper_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value);
}


THRUST_EXEC_CHECK_DISABLE
template<typename DerivedPolicy, typename ForwardIterator, typename T, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
ForwardIterator upper_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                            ForwardIterator first,
                            ForwardIterator last,
                            const T &value,
                            StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::upper_bound;
    return upper_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value, comp);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename LessThanComparable>
THRUST_HOST_DEVICE
bool binary_search(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                   ForwardIterator first,
                   ForwardIterator last,
                   const LessThanComparable& value)
{
    using thrust::system::detail::generic::binary_search;
    return binary_search(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename T, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
bool binary_search(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                   ForwardIterator first,
                   ForwardIterator last,
                   const T& value,
                   StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::binary_search;
    return binary_search(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value, comp);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename T, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
thrust::pair<ForwardIterator, ForwardIterator>
equal_range(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
            ForwardIterator first,
            ForwardIterator last,
            const T& value,
            StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::equal_range;
    return equal_range(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value, comp);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename LessThanComparable>
THRUST_HOST_DEVICE
thrust::pair<ForwardIterator, ForwardIterator>
equal_range(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
            ForwardIterator first,
            ForwardIterator last,
            const LessThanComparable& value)
{
    using thrust::system::detail::generic::equal_range;
    return equal_range(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, value);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename InputIterator, typename OutputIterator>
THRUST_HOST_DEVICE
OutputIterator lower_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                           ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output)
{
    using thrust::system::detail::generic::lower_bound;
    return lower_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, values_first, values_last, output);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename InputIterator, typename OutputIterator, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
OutputIterator lower_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                           ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output,
                           StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::lower_bound;
    return lower_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, values_first, values_last, output, comp);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename InputIterator, typename OutputIterator>
THRUST_HOST_DEVICE
OutputIterator upper_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                           ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output)
{
    using thrust::system::detail::generic::upper_bound;
    return upper_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, values_first, values_last, output);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename InputIterator, typename OutputIterator, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
OutputIterator upper_bound(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                           ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output,
                           StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::upper_bound;
    return upper_bound(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, values_first, values_last, output, comp);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename InputIterator, typename OutputIterator>
THRUST_HOST_DEVICE
OutputIterator binary_search(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                             ForwardIterator first,
                             ForwardIterator last,
                             InputIterator values_first,
                             InputIterator values_last,
                             OutputIterator output)
{
    using thrust::system::detail::generic::binary_search;
    return binary_search(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, values_first, values_last, output);
}


THRUST_EXEC_CHECK_DISABLE
template <typename DerivedPolicy, typename ForwardIterator, typename InputIterator, typename OutputIterator, typename StrictWeakOrdering>
THRUST_HOST_DEVICE
OutputIterator binary_search(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
                             ForwardIterator first,
                             ForwardIterator last,
                             InputIterator values_first,
                             InputIterator values_last,
                             OutputIterator output,
                             StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::binary_search;
    return binary_search(thrust::detail::derived_cast(thrust::detail::strip_const(exec)), first, last, values_first, values_last, output, comp);
}


//////////////////////
// Scalar Functions //
//////////////////////

template <typename ForwardIterator, typename LessThanComparable>
ForwardIterator lower_bound(ForwardIterator first,
                            ForwardIterator last,
                            const LessThanComparable& value)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::lower_bound(select_system(system), first, last, value);
}

template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
ForwardIterator lower_bound(ForwardIterator first,
                            ForwardIterator last,
                            const T& value,
                            StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::lower_bound(select_system(system), first, last, value, comp);
}

template <typename ForwardIterator, typename LessThanComparable>
ForwardIterator upper_bound(ForwardIterator first,
                            ForwardIterator last,
                            const LessThanComparable& value)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::upper_bound(select_system(system), first, last, value);
}

template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
ForwardIterator upper_bound(ForwardIterator first,
                            ForwardIterator last,
                            const T& value,
                            StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::upper_bound(select_system(system), first, last, value, comp);
}

template <typename ForwardIterator, typename LessThanComparable>
bool binary_search(ForwardIterator first,
                   ForwardIterator last,
                   const LessThanComparable& value)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::binary_search(select_system(system), first, last, value);
}

template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
bool binary_search(ForwardIterator first,
                   ForwardIterator last,
                   const T& value,
                   StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::binary_search(select_system(system), first, last, value, comp);
}

template <typename ForwardIterator, typename LessThanComparable>
thrust::pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first,
            ForwardIterator last,
            const LessThanComparable& value)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::equal_range(select_system(system), first, last, value);
}

template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
thrust::pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first,
            ForwardIterator last,
            const T& value,
            StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System = typename thrust::iterator_system<ForwardIterator>::type;

    System system;

    return thrust::equal_range(select_system(system), first, last, value, comp);
}

//////////////////////
// Vector Functions //
//////////////////////

template <typename ForwardIterator, typename InputIterator, typename OutputIterator>
OutputIterator lower_bound(ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output)
{
    using thrust::system::detail::generic::select_system;

    using System1 = typename thrust::iterator_system<ForwardIterator>::type;
    using System2 = typename thrust::iterator_system<InputIterator>::type;
    using System3 = typename thrust::iterator_system<OutputIterator>::type;

    System1 system1;
    System2 system2;
    System3 system3;

    return thrust::lower_bound(select_system(system1,system2,system3), first, last, values_first, values_last, output);
}

template <typename ForwardIterator, typename InputIterator, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator lower_bound(ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output,
                           StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System1 = typename thrust::iterator_system<ForwardIterator>::type;
    using System2 = typename thrust::iterator_system<InputIterator>::type;
    using System3 = typename thrust::iterator_system<OutputIterator>::type;

    System1 system1;
    System2 system2;
    System3 system3;

    return thrust::lower_bound(select_system(system1,system2,system3), first, last, values_first, values_last, output, comp);
}

template <typename ForwardIterator, typename InputIterator, typename OutputIterator>
OutputIterator upper_bound(ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output)
{
    using thrust::system::detail::generic::select_system;

    using System1 = typename thrust::iterator_system<ForwardIterator>::type;
    using System2 = typename thrust::iterator_system<InputIterator>::type;
    using System3 = typename thrust::iterator_system<OutputIterator>::type;

    System1 system1;
    System2 system2;
    System3 system3;

    return thrust::upper_bound(select_system(system1,system2,system3), first, last, values_first, values_last, output);
}

template <typename ForwardIterator, typename InputIterator, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator upper_bound(ForwardIterator first,
                           ForwardIterator last,
                           InputIterator values_first,
                           InputIterator values_last,
                           OutputIterator output,
                           StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System1 = typename thrust::iterator_system<ForwardIterator>::type;
    using System2 = typename thrust::iterator_system<InputIterator>::type;
    using System3 = typename thrust::iterator_system<OutputIterator>::type;

    System1 system1;
    System2 system2;
    System3 system3;

    return thrust::upper_bound(select_system(system1,system2,system3), first, last, values_first, values_last, output, comp);
}

template <typename ForwardIterator, typename InputIterator, typename OutputIterator>
OutputIterator binary_search(ForwardIterator first,
                             ForwardIterator last,
                             InputIterator values_first,
                             InputIterator values_last,
                             OutputIterator output)
{
    using thrust::system::detail::generic::select_system;

    using System1 = typename thrust::iterator_system<ForwardIterator>::type;
    using System2 = typename thrust::iterator_system<InputIterator>::type;
    using System3 = typename thrust::iterator_system<OutputIterator>::type;

    System1 system1;
    System2 system2;
    System3 system3;

    return thrust::binary_search(select_system(system1,system2,system3), first, last, values_first, values_last, output);
}

template <typename ForwardIterator, typename InputIterator, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator binary_search(ForwardIterator first,
                             ForwardIterator last,
                             InputIterator values_first,
                             InputIterator values_last,
                             OutputIterator output,
                             StrictWeakOrdering comp)
{
    using thrust::system::detail::generic::select_system;

    using System1 = typename thrust::iterator_system<ForwardIterator>::type;
    using System2 = typename thrust::iterator_system<InputIterator>::type;
    using System3 = typename thrust::iterator_system<OutputIterator>::type;

    System1 system1;
    System2 system2;
    System3 system3;

    return thrust::binary_search(select_system(system1,system2,system3), first, last, values_first, values_last, output, comp);
}

THRUST_NAMESPACE_END
