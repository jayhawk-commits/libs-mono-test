/******************************************************************************
 * Copyright (c) 2011, Duane Merrill.  All rights reserved.
 * Copyright (c) 2011-2018, NVIDIA CORPORATION.  All rights reserved.
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

/******************************************************************************
 * Simple example of DeviceReduce::Sum().
 *
 * Sums an array of int keys.
 *
 * To compile using the command line:
 *   nvcc -arch=sm_XX example_device_reduce.cu -I../.. -lcudart -O3
 *
 ******************************************************************************/

// Ensure printing of CUDA runtime errors to console
#define CUB_STDERR

#include <stdio.h>

#include <hipcub/device/device_reduce.hpp>

#include "../example_utils.hpp"

using namespace hipcub;

//---------------------------------------------------------------------
// Globals, constants and typedefs
//---------------------------------------------------------------------

bool                            g_verbose = false;  // Whether to display input/output to console
hipcub::CachingDeviceAllocator  g_allocator;  // Caching allocator for device memory


//---------------------------------------------------------------------
// Test generation
//---------------------------------------------------------------------

/**
 * Initialize problem
 */
void Initialize(
    int   *h_in,
    int     num_items)
{
    for (int i = 0; i < num_items; ++i)
        h_in[i] = i;

    if (g_verbose)
    {
        printf("Input:\n");
        DisplayResults(h_in, num_items);
        printf("\n\n");
    }
}


/**
 * Compute solution
 */
void Solve(
    int           *h_in,
    int           &h_reference,
    int             num_items)
{
    for (int i = 0; i < num_items; ++i)
    {
        if (i == 0)
            h_reference = h_in[0];
        else
            h_reference += h_in[i];
    }
}


//---------------------------------------------------------------------
// Main
//---------------------------------------------------------------------

/**
 * Main
 */
int main(int argc, char** argv)
{
    int num_items = 150;

    // Initialize command line
    CommandLineArgs args(argc, argv);
    g_verbose = args.CheckCmdLineFlag("v");
    args.GetCmdLineArgument("n", num_items);

    // Print usage
    if (args.CheckCmdLineFlag("help"))
    {
        printf("%s "
            "[--n=<input items> "
            "[--device=<device-id>] "
            "[--v] "
            "\n", argv[0]);
        exit(0);
    }

    // Initialize device
    HIP_CHECK(args.DeviceInit());

    printf("hipcub::DeviceReduce::Sum() %d items (%d-byte elements)\n",
        num_items, (int) sizeof(int));
    fflush(stdout);

    // Allocate host arrays
    int* h_in = new int[num_items];
    int  h_reference;

    // Initialize problem and solution
    Initialize(h_in, num_items);
    Solve(h_in, h_reference, num_items);

    // Allocate problem device arrays
    int* d_in = nullptr;
    HIP_CHECK(g_allocator.DeviceAllocate((void**)&d_in, sizeof(int) * num_items));

    // Initialize device input
    HIP_CHECK(hipMemcpy(d_in, h_in, sizeof(int) * num_items, hipMemcpyHostToDevice));

    // Allocate device output array
    int* d_out = nullptr;
    HIP_CHECK(g_allocator.DeviceAllocate((void**)&d_out, sizeof(int) * 1));

    // Request and allocate temporary storage
    void*           d_temp_storage     = nullptr;
    size_t          temp_storage_bytes = 0;
    HIP_CHECK(
        hipcub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, d_in, d_out, num_items));
    HIP_CHECK(g_allocator.DeviceAllocate(&d_temp_storage, temp_storage_bytes));

    // Run
    HIP_CHECK(
        hipcub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, d_in, d_out, num_items));

    // Check for correctness (and display results, if specified)
    int compare = CompareDeviceResults(&h_reference, d_out, 1, g_verbose, g_verbose);
    printf("\t%s", compare ? "FAIL" : "PASS");
    AssertEquals(0, compare);

    // Cleanup
    if(h_in)
        delete[] h_in;
    if(d_in)
        HIP_CHECK(g_allocator.DeviceFree(d_in));
    if(d_out)
        HIP_CHECK(g_allocator.DeviceFree(d_out));
    if(d_temp_storage)
        HIP_CHECK(g_allocator.DeviceFree(d_temp_storage));

    printf("\n\n");

    return 0;
}
