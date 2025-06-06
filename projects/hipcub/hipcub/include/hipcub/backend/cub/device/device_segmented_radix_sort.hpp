/******************************************************************************
 * Copyright (c) 2010-2011, Duane Merrill.  All rights reserved.
 * Copyright (c) 2011-2018, NVIDIA CORPORATION.  All rights reserved.
 * Modifications Copyright (c) 2017-2024, Advanced Micro Devices, Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#ifndef HIPCUB_CUB_DEVICE_DEVICE_SEGMENTED_RADIX_SORT_HPP_
#define HIPCUB_CUB_DEVICE_DEVICE_SEGMENTED_RADIX_SORT_HPP_

#include "../../../config.hpp"
#include "../../../util_deprecated.hpp"
#include "hipcub/util_deprecated.hpp"

#include <cub/device/device_segmented_radix_sort.cuh>

BEGIN_HIPCUB_NAMESPACE

struct DeviceSegmentedRadixSort
{
    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortPairs(void*           d_temp_storage,
                                                        size_t&         temp_storage_bytes,
                                                        const KeyT*     d_keys_in,
                                                        KeyT*           d_keys_out,
                                                        const ValueT*   d_values_in,
                                                        ValueT*         d_values_out,
                                                        int             num_items,
                                                        int             num_segments,
                                                        OffsetIteratorT d_begin_offsets,
                                                        OffsetIteratorT d_end_offsets,
                                                        int             begin_bit = 0,
                                                        int             end_bit = sizeof(KeyT) * 8,
                                                        hipStream_t     stream  = 0)
    {
        return hipCUDAErrorTohipError(::cub::DeviceSegmentedRadixSort::SortPairs(d_temp_storage,
                                                                                 temp_storage_bytes,
                                                                                 d_keys_in,
                                                                                 d_keys_out,
                                                                                 d_values_in,
                                                                                 d_values_out,
                                                                                 num_items,
                                                                                 num_segments,
                                                                                 d_begin_offsets,
                                                                                 d_end_offsets,
                                                                                 begin_bit,
                                                                                 end_bit,
                                                                                 stream));
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortPairs(void*           d_temp_storage,
                  size_t&         temp_storage_bytes,
                  const KeyT*     d_keys_in,
                  KeyT*           d_keys_out,
                  const ValueT*   d_values_in,
                  ValueT*         d_values_out,
                  int             num_items,
                  int             num_segments,
                  OffsetIteratorT d_begin_offsets,
                  OffsetIteratorT d_end_offsets,
                  int             begin_bit,
                  int             end_bit,
                  hipStream_t     stream,
                  bool            debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortPairs(d_temp_storage,
                         temp_storage_bytes,
                         d_keys_in,
                         d_keys_out,
                         d_values_in,
                         d_values_out,
                         num_items,
                         num_segments,
                         d_begin_offsets,
                         d_end_offsets,
                         begin_bit,
                         end_bit,
                         stream);
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortPairs(void*                 d_temp_storage,
                                                        size_t&               temp_storage_bytes,
                                                        DoubleBuffer<KeyT>&   d_keys,
                                                        DoubleBuffer<ValueT>& d_values,
                                                        int                   num_items,
                                                        int                   num_segments,
                                                        OffsetIteratorT       d_begin_offsets,
                                                        OffsetIteratorT       d_end_offsets,
                                                        int                   begin_bit = 0,
                                                        int         end_bit = sizeof(KeyT) * 8,
                                                        hipStream_t stream  = 0)
    {
        return hipCUDAErrorTohipError(::cub::DeviceSegmentedRadixSort::SortPairs(d_temp_storage,
                                                                                 temp_storage_bytes,
                                                                                 d_keys,
                                                                                 d_values,
                                                                                 num_items,
                                                                                 num_segments,
                                                                                 d_begin_offsets,
                                                                                 d_end_offsets,
                                                                                 begin_bit,
                                                                                 end_bit,
                                                                                 stream));
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortPairs(void*                 d_temp_storage,
                  size_t&               temp_storage_bytes,
                  DoubleBuffer<KeyT>&   d_keys,
                  DoubleBuffer<ValueT>& d_values,
                  int                   num_items,
                  int                   num_segments,
                  OffsetIteratorT       d_begin_offsets,
                  OffsetIteratorT       d_end_offsets,
                  int                   begin_bit,
                  int                   end_bit,
                  hipStream_t           stream,
                  bool                  debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortPairs(d_temp_storage,
                         temp_storage_bytes,
                         d_keys,
                         d_values,
                         num_items,
                         num_segments,
                         d_begin_offsets,
                         d_end_offsets,
                         begin_bit,
                         end_bit,
                         stream);
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortPairsDescending(void*         d_temp_storage,
                                                                  size_t&       temp_storage_bytes,
                                                                  const KeyT*   d_keys_in,
                                                                  KeyT*         d_keys_out,
                                                                  const ValueT* d_values_in,
                                                                  ValueT*       d_values_out,
                                                                  int           num_items,
                                                                  int           num_segments,
                                                                  OffsetIteratorT d_begin_offsets,
                                                                  OffsetIteratorT d_end_offsets,
                                                                  int             begin_bit = 0,
                                                                  int end_bit = sizeof(KeyT) * 8,
                                                                  hipStream_t stream = 0)
    {
        return hipCUDAErrorTohipError(
            ::cub::DeviceSegmentedRadixSort::SortPairsDescending(d_temp_storage,
                                                                 temp_storage_bytes,
                                                                 d_keys_in,
                                                                 d_keys_out,
                                                                 d_values_in,
                                                                 d_values_out,
                                                                 num_items,
                                                                 num_segments,
                                                                 d_begin_offsets,
                                                                 d_end_offsets,
                                                                 begin_bit,
                                                                 end_bit,
                                                                 stream));
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortPairsDescending(void*           d_temp_storage,
                            size_t&         temp_storage_bytes,
                            const KeyT*     d_keys_in,
                            KeyT*           d_keys_out,
                            const ValueT*   d_values_in,
                            ValueT*         d_values_out,
                            int             num_items,
                            int             num_segments,
                            OffsetIteratorT d_begin_offsets,
                            OffsetIteratorT d_end_offsets,
                            int             begin_bit,
                            int             end_bit,
                            hipStream_t     stream,
                            bool            debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortPairsDescending(d_temp_storage,
                                   temp_storage_bytes,
                                   d_keys_in,
                                   d_keys_out,
                                   d_values_in,
                                   d_values_out,
                                   num_items,
                                   num_segments,
                                   d_begin_offsets,
                                   d_end_offsets,
                                   begin_bit,
                                   end_bit,
                                   stream);
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortPairsDescending(void*   d_temp_storage,
                                                                  size_t& temp_storage_bytes,
                                                                  DoubleBuffer<KeyT>&   d_keys,
                                                                  DoubleBuffer<ValueT>& d_values,
                                                                  int                   num_items,
                                                                  int             num_segments,
                                                                  OffsetIteratorT d_begin_offsets,
                                                                  OffsetIteratorT d_end_offsets,
                                                                  int             begin_bit,
                                                                  int             end_bit,
                                                                  hipStream_t     stream)
    {
        return hipCUDAErrorTohipError(
            ::cub::DeviceSegmentedRadixSort::SortPairsDescending(d_temp_storage,
                                                                 temp_storage_bytes,
                                                                 d_keys,
                                                                 d_values,
                                                                 num_items,
                                                                 num_segments,
                                                                 d_begin_offsets,
                                                                 d_end_offsets,
                                                                 begin_bit,
                                                                 end_bit,
                                                                 stream));
    }

    template<typename KeyT, typename ValueT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortPairsDescending(void*                 d_temp_storage,
                            size_t&               temp_storage_bytes,
                            DoubleBuffer<KeyT>&   d_keys,
                            DoubleBuffer<ValueT>& d_values,
                            int                   num_items,
                            int                   num_segments,
                            OffsetIteratorT       d_begin_offsets,
                            OffsetIteratorT       d_end_offsets,
                            int                   begin_bit,
                            int                   end_bit,
                            hipStream_t           stream,
                            bool                  debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortPairsDescending(d_temp_storage,
                                   temp_storage_bytes,
                                   d_keys,
                                   d_values,
                                   num_items,
                                   num_segments,
                                   d_begin_offsets,
                                   d_end_offsets,
                                   begin_bit,
                                   end_bit,
                                   stream);
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortKeys(void*           d_temp_storage,
                                                       size_t&         temp_storage_bytes,
                                                       const KeyT*     d_keys_in,
                                                       KeyT*           d_keys_out,
                                                       int             num_items,
                                                       int             num_segments,
                                                       OffsetIteratorT d_begin_offsets,
                                                       OffsetIteratorT d_end_offsets,
                                                       int             begin_bit = 0,
                                                       int             end_bit   = sizeof(KeyT) * 8,
                                                       hipStream_t     stream    = 0)
    {
        return hipCUDAErrorTohipError(::cub::DeviceSegmentedRadixSort::SortKeys(d_temp_storage,
                                                                                temp_storage_bytes,
                                                                                d_keys_in,
                                                                                d_keys_out,
                                                                                num_items,
                                                                                num_segments,
                                                                                d_begin_offsets,
                                                                                d_end_offsets,
                                                                                begin_bit,
                                                                                end_bit,
                                                                                stream));
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortKeys(void*           d_temp_storage,
                 size_t&         temp_storage_bytes,
                 const KeyT*     d_keys_in,
                 KeyT*           d_keys_out,
                 int             num_items,
                 int             num_segments,
                 OffsetIteratorT d_begin_offsets,
                 OffsetIteratorT d_end_offsets,
                 int             begin_bit,
                 int             end_bit,
                 hipStream_t     stream,
                 bool            debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortKeys(d_temp_storage,
                        temp_storage_bytes,
                        d_keys_in,
                        d_keys_out,
                        num_items,
                        num_segments,
                        d_begin_offsets,
                        d_end_offsets,
                        begin_bit,
                        end_bit,
                        stream);
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortKeys(void*               d_temp_storage,
                                                       size_t&             temp_storage_bytes,
                                                       DoubleBuffer<KeyT>& d_keys,
                                                       int                 num_items,
                                                       int                 num_segments,
                                                       OffsetIteratorT     d_begin_offsets,
                                                       OffsetIteratorT     d_end_offsets,
                                                       int                 begin_bit = 0,
                                                       int         end_bit = sizeof(KeyT) * 8,
                                                       hipStream_t stream  = 0)
    {
        return hipCUDAErrorTohipError(::cub::DeviceSegmentedRadixSort::SortKeys(d_temp_storage,
                                                                                temp_storage_bytes,
                                                                                d_keys,
                                                                                num_items,
                                                                                num_segments,
                                                                                d_begin_offsets,
                                                                                d_end_offsets,
                                                                                begin_bit,
                                                                                end_bit,
                                                                                stream));
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortKeys(void*               d_temp_storage,
                 size_t&             temp_storage_bytes,
                 DoubleBuffer<KeyT>& d_keys,
                 int                 num_items,
                 int                 num_segments,
                 OffsetIteratorT     d_begin_offsets,
                 OffsetIteratorT     d_end_offsets,
                 int                 begin_bit,
                 int                 end_bit,
                 hipStream_t         stream,
                 bool                debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortKeys(d_temp_storage,
                        temp_storage_bytes,
                        d_keys,
                        num_items,
                        num_segments,
                        d_begin_offsets,
                        d_end_offsets,
                        begin_bit,
                        end_bit,
                        stream);
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortKeysDescending(void*           d_temp_storage,
                                                                 size_t&         temp_storage_bytes,
                                                                 const KeyT*     d_keys_in,
                                                                 KeyT*           d_keys_out,
                                                                 int             num_items,
                                                                 int             num_segments,
                                                                 OffsetIteratorT d_begin_offsets,
                                                                 OffsetIteratorT d_end_offsets,
                                                                 int             begin_bit = 0,
                                                                 int end_bit = sizeof(KeyT) * 8,
                                                                 hipStream_t stream = 0)
    {
        return hipCUDAErrorTohipError(
            ::cub::DeviceSegmentedRadixSort::SortKeysDescending(d_temp_storage,
                                                                temp_storage_bytes,
                                                                d_keys_in,
                                                                d_keys_out,
                                                                num_items,
                                                                num_segments,
                                                                d_begin_offsets,
                                                                d_end_offsets,
                                                                begin_bit,
                                                                end_bit,
                                                                stream));
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortKeysDescending(void*           d_temp_storage,
                           size_t&         temp_storage_bytes,
                           const KeyT*     d_keys_in,
                           KeyT*           d_keys_out,
                           int             num_items,
                           int             num_segments,
                           OffsetIteratorT d_begin_offsets,
                           OffsetIteratorT d_end_offsets,
                           int             begin_bit,
                           int             end_bit,
                           hipStream_t     stream,
                           bool            debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortKeysDescending(d_temp_storage,
                                  temp_storage_bytes,
                                  d_keys_in,
                                  d_keys_out,
                                  num_items,
                                  num_segments,
                                  d_begin_offsets,
                                  d_end_offsets,
                                  begin_bit,
                                  end_bit,
                                  stream);
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_RUNTIME_FUNCTION static hipError_t SortKeysDescending(void*   d_temp_storage,
                                                                 size_t& temp_storage_bytes,
                                                                 DoubleBuffer<KeyT>& d_keys,
                                                                 int                 num_items,
                                                                 int                 num_segments,
                                                                 OffsetIteratorT d_begin_offsets,
                                                                 OffsetIteratorT d_end_offsets,
                                                                 int             begin_bit = 0,
                                                                 int end_bit = sizeof(KeyT) * 8,
                                                                 hipStream_t stream = 0)
    {
        return hipCUDAErrorTohipError(
            ::cub::DeviceSegmentedRadixSort::SortKeysDescending(d_temp_storage,
                                                                temp_storage_bytes,
                                                                d_keys,
                                                                num_items,
                                                                num_segments,
                                                                d_begin_offsets,
                                                                d_end_offsets,
                                                                begin_bit,
                                                                end_bit,
                                                                stream));
    }

    template<typename KeyT, typename OffsetIteratorT>
    HIPCUB_DETAIL_DEPRECATED_DEBUG_SYNCHRONOUS HIPCUB_RUNTIME_FUNCTION static hipError_t
        SortKeysDescending(void*               d_temp_storage,
                           size_t&             temp_storage_bytes,
                           DoubleBuffer<KeyT>& d_keys,
                           int                 num_items,
                           int                 num_segments,
                           OffsetIteratorT     d_begin_offsets,
                           OffsetIteratorT     d_end_offsets,
                           int                 begin_bit,
                           int                 end_bit,
                           hipStream_t         stream,
                           bool                debug_synchronous)
    {
        HIPCUB_DETAIL_RUNTIME_LOG_DEBUG_SYNCHRONOUS();
        return SortKeysDescending(d_temp_storage,
                                  temp_storage_bytes,
                                  d_keys,
                                  num_items,
                                  num_segments,
                                  d_begin_offsets,
                                  d_end_offsets,
                                  begin_bit,
                                  end_bit,
                                  stream);
    }
};

END_HIPCUB_NAMESPACE

#endif // HIPCUB_CUB_DEVICE_DEVICE_SEGMENTED_RADIX_SORT_HPP_
