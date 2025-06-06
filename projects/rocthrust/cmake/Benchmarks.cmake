# ########################################################################
# Copyright 2024-2025 Advanced Micro Devices, Inc.
# ########################################################################

# ###########################
# rocThrust benchmarks
# ###########################

# Common functionality for configuring rocThrust's benchmarks

# Registers a .cu as C++ rocThrust benchmark
function(add_thrust_benchmark BENCHMARK_NAME BENCHMARK_SOURCE NOT_INTERNAL)
    set(BENCHMARK_TARGET "benchmark_thrust_${BENCHMARK_NAME}")
    if(USE_HIPCXX)
        set_source_files_properties(${BENCHMARK_SOURCE}
            PROPERTIES
                LANGUAGE HIP
        )
    else()
        set_source_files_properties(${BENCHMARK_SOURCE}
            PROPERTIES
                LANGUAGE CXX
        )
    endif()
    add_executable(${BENCHMARK_TARGET} ${BENCHMARK_SOURCE})

    target_compile_options(${BENCHMARK_TARGET} PRIVATE ${COMPILE_OPTIONS})
    target_link_libraries(${BENCHMARK_TARGET}
        PRIVATE
            rocthrust
            roc::rocprim_hip
    )
    # Internal benchmark does not use Google Benchmark nor rocRAND.
    # This can be omited when that benchmark is removed.
    if(NOT_INTERNAL)
        target_link_libraries(${BENCHMARK_TARGET}
            PRIVATE
                roc::rocrand
                benchmark::benchmark
    )
    endif()
    foreach(gpu_target ${GPU_TARGETS})
        target_link_libraries(${BENCHMARK_TARGET}
            INTERFACE
                --cuda-gpu-arch=${gpu_target}
        )
    endforeach()

    # Separate normal from internal benchmarks
    if(NOT_INTERNAL)
        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/benchmarks/")
    else()
        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/benchmarks/internal/")
    endif()

    set_target_properties(${BENCHMARK_TARGET}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    )
    rocm_install(TARGETS ${BENCHMARK_TARGET} COMPONENT benchmarks)
endfunction()
