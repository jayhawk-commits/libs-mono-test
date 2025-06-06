/******************************************************************************
 * Copyright (c) 2016, NVIDIA CORPORATION.  All rights reserved.
 * Modifications Copyright (c) 2019-2025, Advanced Micro Devices, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/
#pragma once

#include <thrust/detail/config.h>

#include <thrust/system/hip/detail/guarded_hip_runtime_api.h>
#include <thrust/system/cpp/detail/execution_policy.h>
#include <thrust/system/hip/detail/execution_policy.h>

THRUST_NAMESPACE_BEGIN
namespace hip_rocprim
{

template <class Sys1, class Sys2>
struct cross_system : execution_policy<cross_system<Sys1, Sys2> >
{
    using policy1 = thrust::execution_policy<Sys1>;
    using policy2 = thrust::execution_policy<Sys2>;

    policy1 &sys1;
    policy2 &sys2;

    inline THRUST_HOST_DEVICE
    cross_system(policy1 &sys1, policy2 &sys2) : sys1(sys1), sys2(sys2) {}

    inline THRUST_HOST_DEVICE
    cross_system<Sys2, Sys1> rotate() const
    {
        return cross_system<Sys2, Sys1>(sys2, sys1);
    }
};

  // Device to host.
  template <class Sys1, class Sys2>
  constexpr THRUST_HOST_DEVICE
  auto direction_of_copy(
    thrust::system::hip::execution_policy<Sys1> const&
  , thrust::cpp::execution_policy<Sys2> const&
  )
  THRUST_DECLTYPE_RETURNS(
    thrust::detail::integral_constant<
      hipMemcpyKind, hipMemcpyDeviceToHost
    >{}
  )

  // Host to device.
  template <class Sys1, class Sys2>
  constexpr THRUST_HOST_DEVICE
  auto direction_of_copy(
    thrust::cpp::execution_policy<Sys1> const&
  , thrust::system::hip::execution_policy<Sys2> const&
  )
  THRUST_DECLTYPE_RETURNS(
    thrust::detail::integral_constant<
      hipMemcpyKind, hipMemcpyHostToDevice
    >{}
  )

  // Device to device.
  template <class Sys1, class Sys2>
  constexpr THRUST_HOST_DEVICE
  auto direction_of_copy(
    thrust::system::hip::execution_policy<Sys1> const&
  , thrust::system::hip::execution_policy<Sys2> const&
  )
  THRUST_DECLTYPE_RETURNS(
    thrust::detail::integral_constant<
      hipMemcpyKind, hipMemcpyDeviceToDevice
    >{}
  )

  // Device to device.
  template <class DerivedPolicy>
  constexpr THRUST_HOST_DEVICE
  auto direction_of_copy(execution_policy<DerivedPolicy> const &)
  THRUST_DECLTYPE_RETURNS(
    thrust::detail::integral_constant<
      hipMemcpyKind, hipMemcpyDeviceToDevice
    >{}
  )

  template <class Sys1, class Sys2>
  constexpr THRUST_HOST_DEVICE
  auto direction_of_copy(
    execution_policy<cross_system<Sys1, Sys2>> const &systems
  )
  THRUST_DECLTYPE_RETURNS(
    direction_of_copy(
      derived_cast(derived_cast(systems).sys1)
    , derived_cast(derived_cast(systems).sys2)
    )
  )

  template <typename ExecutionPolicy0,
            typename ExecutionPolicy1,
            // MSVC2015 WAR: put decltype here instead of in trailing return type
            typename Direction =
              decltype(direction_of_copy(std::declval<ExecutionPolicy0>(),
                                         std::declval<ExecutionPolicy1>()))>
  constexpr THRUST_HOST_DEVICE
  auto is_device_to_host_copy(
    ExecutionPolicy0 const&
  , ExecutionPolicy1 const&
  )
    noexcept ->
      thrust::detail::integral_constant<
        bool, hipMemcpyDeviceToHost== Direction::value
      >
  {
    return {};
  }

  template <typename ExecutionPolicy,
            // MSVC2015 WAR: put decltype here instead of in trailing return type
            typename Direction =
              decltype(direction_of_copy(std::declval<ExecutionPolicy>()))>
  constexpr THRUST_HOST_DEVICE
  auto is_device_to_host_copy(ExecutionPolicy const& )
    noexcept ->
      thrust::detail::integral_constant<
        bool, hipMemcpyDeviceToHost == Direction::value
      >
  {
    return {};
  }

  template <typename ExecutionPolicy0,
            typename ExecutionPolicy1,
            // MSVC2015 WAR: put decltype here instead of in trailing return type
            typename Direction =
              decltype(direction_of_copy(std::declval<ExecutionPolicy0>(),
                                         std::declval<ExecutionPolicy1>()))>
  constexpr THRUST_HOST_DEVICE
  auto is_host_to_device_copy(
    ExecutionPolicy0 const&
  , ExecutionPolicy1 const&
  )
    noexcept ->
      thrust::detail::integral_constant<
        bool, hipMemcpyHostToDevice  == Direction::value
      >
  {
    return {};
  }

  template <typename ExecutionPolicy,
            // MSVC2015 WAR: put decltype here instead of in trailing return type
            typename Direction =
              decltype(direction_of_copy(std::declval<ExecutionPolicy>()))>
  constexpr THRUST_HOST_DEVICE
  auto is_host_to_device_copy(ExecutionPolicy const& )
    noexcept ->
      thrust::detail::integral_constant<
        bool, hipMemcpyHostToDevice == Direction::value
      >
  {
    return {};
  }

  template <typename ExecutionPolicy0,
            typename ExecutionPolicy1,
            // MSVC2015 WAR: put decltype here instead of in trailing return type
            typename Direction =
              decltype(direction_of_copy(std::declval<ExecutionPolicy0>(),
                                         std::declval<ExecutionPolicy1>()))>
  constexpr THRUST_HOST_DEVICE
  auto is_device_to_device_copy(
    ExecutionPolicy0 const&
  , ExecutionPolicy1 const&
  )
    noexcept ->
      thrust::detail::integral_constant<
        bool,    hipMemcpyDeviceToDevice == Direction::value
      >
  {
    return {};
  }

  template <typename ExecutionPolicy,
            // MSVC2015 WAR: put decltype here instead of in trailing return type
            typename Direction =
              decltype(direction_of_copy(std::declval<ExecutionPolicy>()))>
  constexpr THRUST_HOST_DEVICE
  auto is_device_to_device_copy(ExecutionPolicy const& )
    noexcept ->
      thrust::detail::integral_constant<
        bool,   hipMemcpyDeviceToDevice  == Direction::value
      >
  {
    return {};
  }

  /////////////////////////////////////////////////////////////////////////////

  // Device to host.
  template <class Sys1, class Sys2>
  THRUST_HOST_DEVICE auto select_device_system(thrust::hip::execution_policy<Sys1>& sys1,
                                                thrust::execution_policy<Sys2>&)
      THRUST_DECLTYPE_RETURNS(sys1)

      // Device to host.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_device_system(thrust::hip::execution_policy<Sys1> const& sys1,
                                                    thrust::execution_policy<Sys2> const&)
          THRUST_DECLTYPE_RETURNS(sys1)

      // Host to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_device_system(thrust::execution_policy<Sys1>&,
                                                    thrust::hip::execution_policy<Sys2>& sys2)
          THRUST_DECLTYPE_RETURNS(sys2)

      // Host to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_device_system(thrust::execution_policy<Sys1> const&,
                                                    thrust::hip::execution_policy<Sys2> const& sys2)
          THRUST_DECLTYPE_RETURNS(sys2)

      // Device to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_device_system(thrust::hip::execution_policy<Sys1>& sys1,
                                                    thrust::hip::execution_policy<Sys2>&)
          THRUST_DECLTYPE_RETURNS(sys1)

      // Device to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_device_system(thrust::hip::execution_policy<Sys1> const& sys1,
                                                    thrust::hip::execution_policy<Sys2> const&)
          THRUST_DECLTYPE_RETURNS(sys1)

      /////////////////////////////////////////////////////////////////////////////

      // Device to host.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_host_system(thrust::hip::execution_policy<Sys1>&,
                                                  thrust::execution_policy<Sys2>& sys2)
          THRUST_DECLTYPE_RETURNS(sys2)

      // Device to host.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_host_system(thrust::hip::execution_policy<Sys1> const&,
                                                  thrust::execution_policy<Sys2> const& sys2)
          THRUST_DECLTYPE_RETURNS(sys2)

      // Host to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_host_system(thrust::execution_policy<Sys1>& sys1,
                                                  thrust::hip::execution_policy<Sys2>&)
          THRUST_DECLTYPE_RETURNS(sys1)

      // Host to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_host_system(thrust::execution_policy<Sys1> const& sys1,
                                                  thrust::hip::execution_policy<Sys2> const&)
          THRUST_DECLTYPE_RETURNS(sys1)

      // Device to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE
      auto select_host_system(thrust::execution_policy<Sys1>& sys1, thrust::execution_policy<Sys2>&)
          THRUST_DECLTYPE_RETURNS(sys1)

      // Device to device.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE auto select_host_system(thrust::execution_policy<Sys1> const& sys1,
                                                  thrust::execution_policy<Sys2> const&)
          THRUST_DECLTYPE_RETURNS(sys1)

      // Device to host.
      template <class Sys1, class Sys2>
      THRUST_HOST_DEVICE cross_system<Sys1, Sys2> select_system(
          execution_policy<Sys1> const& sys1, thrust::cpp::execution_policy<Sys2> const& sys2)
  {
    thrust::execution_policy<Sys1> &     non_const_sys1 = const_cast<execution_policy<Sys1> &>(sys1);
    thrust::cpp::execution_policy<Sys2> &non_const_sys2 = const_cast<thrust::cpp::execution_policy<Sys2> &>(sys2);
    return cross_system<Sys1, Sys2>(non_const_sys1, non_const_sys2);
  }

  // Host to device.
  template <class Sys1, class Sys2>
  THRUST_HOST_DEVICE
  cross_system<Sys1, Sys2>
  select_system(thrust::cpp::execution_policy<Sys1> const &sys1,
                execution_policy<Sys2> const &             sys2)
  {
    thrust::cpp::execution_policy<Sys1> &non_const_sys1 = const_cast<thrust::cpp::execution_policy<Sys1> &>(sys1);
    thrust::execution_policy<Sys2> &     non_const_sys2 = const_cast<execution_policy<Sys2> &>(sys2);
    return cross_system<Sys1, Sys2>(non_const_sys1, non_const_sys2);
  }

} // namespace hip_rocprim
THRUST_NAMESPACE_END
