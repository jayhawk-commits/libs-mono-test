/******************************************************************************
 * Copyright (c) 2018, NVIDIA CORPORATION.  All rights reserved.
 * Modifications Copyright© 2020-2025 Advanced Micro Devices, Inc. All rights reserved.
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

// TODO: Move into system::hip

#pragma once

#include <thrust/detail/config.h>

#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HIP

#include <thrust/system/hip/config.h>

#include <thrust/system/hip/detail/async/customization.h>
#include <thrust/system/hip/detail/async/copy.h>
#include <thrust/system/hip/detail/sort.h>
#include <thrust/detail/alignment.h>
#include <thrust/system/hip/future.h>
#include <thrust/type_traits/is_trivially_relocatable.h>
#include <thrust/type_traits/is_contiguous_iterator.h>
#include <thrust/type_traits/is_operator_less_or_greater_function_object.h>
#include <thrust/type_traits/logical_metafunctions.h>
#include <thrust/iterator/iterator_traits.h>
#include <thrust/detail/static_assert.h>
#include <thrust/distance.h>

#include <type_traits>

// rocprim include
#include <rocprim/rocprim.hpp>

THRUST_NAMESPACE_BEGIN

namespace system { namespace hip { namespace detail
{

// Non-ContiguousIterator input and output iterators
template <
  typename DerivedPolicy
, typename ForwardIt, typename Size, typename StrictWeakOrdering
>
auto async_stable_sort_n(
  execution_policy<DerivedPolicy>& policy,
  ForwardIt                        first,
  Size                             n,
  StrictWeakOrdering               comp
) ->
  typename std::enable_if<
    negation<is_contiguous_iterator<ForwardIt>>::value
  , unique_eager_event
  >::type
{
  using T = typename iterator_traits<ForwardIt>::value_type;

  auto const device_alloc = get_async_device_allocator(policy);

  // Create device-side buffer.

  // FIXME: Combine this temporary allocation with the main one for CUB.
  auto device_buffer = uninitialized_allocate_unique_n<T>(device_alloc, n);

  auto const device_buffer_ptr = device_buffer.get();

  // Synthesize a suitable new execution policy, because we don't want to
  // try and extract twice from the one we were passed.
  typename remove_cvref_t<decltype(policy)>::tag_type tag_policy{};

  // Copy from the input into the buffer.

  auto new_policy0 = thrust::detail::derived_cast(policy).rebind_after(
    std::move(device_buffer)
  );

  THRUST_STATIC_ASSERT((
    std::tuple_size<decltype(
      extract_dependencies(policy)
    )>::value + 1
    <=
    std::tuple_size<decltype(
      extract_dependencies(new_policy0)
    )>::value
  ));

  auto f0 = async_copy_n(
    new_policy0
  , tag_policy
  , first
  , n
  , device_buffer_ptr
  );

  // Sort the buffer.

  auto new_policy1 = thrust::detail::derived_cast(policy).rebind_after(
    std::move(f0)
  );

  THRUST_STATIC_ASSERT((
    std::tuple_size<decltype(
      extract_dependencies(policy)
    )>::value + 1
    <=
    std::tuple_size<decltype(
      extract_dependencies(new_policy1)
    )>::value
  ));

  auto f1 = async_sort_n(
    new_policy1
  , tag_policy
  , device_buffer_ptr
  , n
  , comp
  );

  // Copy from the buffer into the input.
  // FIXME: Combine this with the potential memcpy at the end of the main sort
  // routine.

  auto new_policy2 = thrust::detail::derived_cast(policy).rebind_after(
    std::move(f1)
  );

  THRUST_STATIC_ASSERT((
    std::tuple_size<decltype(
      extract_dependencies(policy)
    )>::value + 1
    <=
    std::tuple_size<decltype(
      extract_dependencies(new_policy2)
    )>::value
  ));

  return async_copy_n(
    new_policy2
  , tag_policy
  , device_buffer_ptr
  , n
  , first
  );
}

// ContiguousIterator iterators
// Non-Scalar value type or user-defined StrictWeakOrdering
template <
  typename DerivedPolicy
, typename ForwardIt, typename Size, typename StrictWeakOrdering
>
auto async_stable_sort_n(
  execution_policy<DerivedPolicy>& policy,
  ForwardIt                        first,
  Size                             n,
  StrictWeakOrdering               comp
) ->
  typename std::enable_if<
    conjunction<
      is_contiguous_iterator<ForwardIt>
    , disjunction<
        negation<
          std::is_scalar<
            typename iterator_traits<ForwardIt>::value_type
          >
        >
      , negation<
          is_operator_less_or_greater_function_object<StrictWeakOrdering>
        >
      >
    >::value
  , unique_eager_event
  >::type
{
  auto const device_alloc = get_async_device_allocator(policy);
  unique_eager_event e;

  // Determine temporary device storage requirements.

  size_t tmp_size = 0;
  thrust::hip_rocprim::throw_on_error(
    thrust::hip_rocprim::__merge_sort::dispatch<thrust::detail::false_type>::doit(
      nullptr
    , tmp_size
    , first
    , static_cast<std::uint8_t*>(nullptr) // Items.
    , n
    , comp
    , nullptr // Null stream, just for sizing.
    , THRUST_HIP_DEBUG_SYNC_FLAG
    )
  , "after merge sort sizing"
  );

  // Allocate temporary storage.

  auto content = uninitialized_allocate_unique_n<std::uint8_t>(
    device_alloc, tmp_size
  );

  // The array was dynamically allocated, so we assume that it's suitably
  // aligned for any type of data. `malloc`/`hipMalloc`/`new`/`std::allocator`
  // make this guarantee.
  auto const content_ptr = content.get();

  void* const tmp_ptr = static_cast<void*>(
    thrust::raw_pointer_cast(content_ptr)
  );

  // Set up stream with dependencies.

  hipStream_t const user_raw_stream = thrust::hip_rocprim::stream(policy);

  if (thrust::hip_rocprim::default_stream() != user_raw_stream)
  {
      e = make_dependent_event(
          std::tuple_cat(
            std::make_tuple(
              std::move(content)
            , unique_stream(nonowning, user_raw_stream)
            )
          , extract_dependencies(
              std::move(thrust::detail::derived_cast(policy))
            )
          )
      );
  }
  else
  {
      e = make_dependent_event(
          std::tuple_cat(
            std::make_tuple(
              std::move(content)
            )
          , extract_dependencies(
              std::move(thrust::detail::derived_cast(policy))
            )
          )
      );
  }

  if( n == 0)
  {
      e.ready();
      return e;
  }

  // Run merge sort.

  thrust::hip_rocprim::throw_on_error(
    thrust::hip_rocprim::__merge_sort::dispatch<thrust::detail::false_type>::doit(
      tmp_ptr
    , tmp_size
    , first
    , static_cast<std::uint8_t*>(nullptr) // Items.
    , n
    , comp
    , e.stream().native_handle()
    , THRUST_HIP_DEBUG_SYNC_FLAG
    )
  , "after merge sort sizing"
  );

  return e;
}

template <typename T, typename Size, typename StrictWeakOrdering>
typename std::enable_if<
  is_operator_less_function_object<StrictWeakOrdering>::value
, hipError_t
>::type
invoke_radix_sort(
  hipStream_t  stream
, void*        tmp_ptr
, std::size_t& tmp_size
, T*           keys_in
, T*           keys_out
, Size&        n
, StrictWeakOrdering
)
{
  return rocprim::radix_sort_keys(
    tmp_ptr
  , tmp_size
  , keys_in
  , keys_out
  , n
  , 0
  , sizeof(T) * 8
  , stream
  , THRUST_HIP_DEBUG_SYNC_FLAG
  );
}

template <typename T, typename Size, typename StrictWeakOrdering>
typename std::enable_if<
  is_operator_greater_function_object<StrictWeakOrdering>::value
, hipError_t
>::type
invoke_radix_sort(
  hipStream_t  stream
, void*        tmp_ptr
, std::size_t& tmp_size
, T*           keys_in
, T*           keys_out
, Size&        n
, StrictWeakOrdering
)
{
  return rocprim::radix_sort_keys_desc(
    tmp_ptr
  , tmp_size
  , keys_in
  , keys_out
  , n
  , 0
  , sizeof(T) * 8
  , stream
  , THRUST_HIP_DEBUG_SYNC_FLAG
  );
}

// ContiguousIterator iterators
// Scalar value type
// operator< or operator>
template <
  typename DerivedPolicy
, typename ForwardIt, typename Size, typename StrictWeakOrdering
>
auto async_stable_sort_n(
  execution_policy<DerivedPolicy>& policy
, ForwardIt                        first
, Size                             n
, StrictWeakOrdering               comp
) ->
  typename std::enable_if<
    conjunction<
      is_contiguous_iterator<ForwardIt>
    , std::is_scalar<
        typename iterator_traits<ForwardIt>::value_type
      >
    , is_operator_less_or_greater_function_object<StrictWeakOrdering>
    >::value
  , unique_eager_event
  >::type
{
  using T = typename iterator_traits<ForwardIt>::value_type;

  auto const device_alloc = get_async_device_allocator(policy);

  unique_eager_event e;

  // Determine temporary device storage requirements.

  size_t tmp_size = 0;
  T* first_ptr = raw_pointer_cast(&*first);
  thrust::hip_rocprim::throw_on_error(
    invoke_radix_sort(
      nullptr
    , nullptr
    , tmp_size
    , first_ptr
    , static_cast<T*>(nullptr)
    , n
    , comp
    )
  , "after radix sort sizing"
  );

  // Allocate temporary storage.

  size_t keys_temp_storage = thrust::detail::aligned_storage_size(
    sizeof(T) * n, 128
  );

  auto content = uninitialized_allocate_unique_n<std::uint8_t>(
    device_alloc, keys_temp_storage + tmp_size
  );

  // The array was dynamically allocated, so we assume that it's suitably
  // aligned for any type of data. `malloc`/`hipMalloc`/`new`/`std::allocator`
  // make this guarantee.
  auto const content_ptr = content.get();

  T* keys_pointer = thrust::detail::aligned_reinterpret_cast<T*>(
    thrust::raw_pointer_cast(content_ptr)
  );

  void* const tmp_ptr = static_cast<void*>(
    thrust::raw_pointer_cast(content_ptr + keys_temp_storage)
  );

  // Set up stream with dependencies.

  hipStream_t const user_raw_stream = thrust::hip_rocprim::stream(policy);

  if (thrust::hip_rocprim::default_stream() != user_raw_stream)
  {
    e = make_dependent_event(
      std::tuple_cat(
        std::make_tuple(
          std::move(content)
        , unique_stream(nonowning, user_raw_stream)
        )
      , extract_dependencies(
          std::move(thrust::detail::derived_cast(policy))
        )
      )
    );
  }
  else
  {
    e = make_dependent_event(
      std::tuple_cat(
        std::make_tuple(
          std::move(content)
        )
      , extract_dependencies(
          std::move(thrust::detail::derived_cast(policy))
        )
      )
    );
  }

  if( n == 0)
  {
      e.ready();
      return e;
  }

  // Run radix sort.
  thrust::hip_rocprim::throw_on_error(
    invoke_radix_sort(
      e.stream().native_handle()
    , tmp_ptr
    , tmp_size
    , first_ptr
    , keys_pointer
    , n
    , comp
    )
  , "after radix sort launch"
  );

  auto new_policy0 = thrust::detail::derived_cast(policy).rebind_after(
    std::move(e)
  );

  THRUST_STATIC_ASSERT((
    std::tuple_size<decltype(
      extract_dependencies(policy)
    )>::value + 1
    <=
    std::tuple_size<decltype(
      extract_dependencies(new_policy0)
    )>::value
  ));

  // Synthesize a suitable new execution policy, because we don't want to
  // try and extract twice from the one we were passed.
  typename remove_cvref_t<decltype(policy)>::tag_type tag_policy{};

  using return_future = decltype(e);
  return return_future(async_copy_n(
    new_policy0
  , tag_policy
  , reinterpret_cast<T*>(keys_pointer)
  , n
  , reinterpret_cast<T*>(first_ptr)
  ));

  return e;
}

}}} // namespace system::hip::detail

namespace hip_rocprim
{

// ADL entry point.
template <
  typename DerivedPolicy
, typename ForwardIt, typename Sentinel, typename StrictWeakOrdering
>
auto async_stable_sort(
  execution_policy<DerivedPolicy>& policy,
  ForwardIt                        first,
  Sentinel                         last,
  StrictWeakOrdering               comp
)
// A GCC 5 bug requires an explicit trailing return type here, so stick with
// THRUST_DECLTYPE_RETURNS for now.
THRUST_DECLTYPE_RETURNS(
  thrust::system::hip::detail::async_stable_sort_n(
    policy, first, thrust::distance(first, last), comp
  )
);

} // hip_rocprim

THRUST_NAMESPACE_END

#endif // THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HIP
