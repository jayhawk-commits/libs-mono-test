# ########################################################################
# Copyright 2019 - 2023 Advanced Micro Devices, Inc.
# ########################################################################

add_library(testing_common INTERFACE)
target_include_directories(testing_common
    SYSTEM
    BEFORE
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)
target_link_libraries(testing_common
    INTERFACE
        rocthrust
        roc::rocprim_hip
)
if (TARGET GTest::GTest)
    target_link_libraries(testing_common
            INTERFACE
            GTest::GTest
            GTest::Main)
else ()
    target_link_libraries(testing_common
            INTERFACE
            GTest::gtest
            GTest::gtest_main)
endif ()
foreach(gpu_target ${GPU_TARGETS})
    target_link_libraries(testing_common
        INTERFACE
          --cuda-gpu-arch=${gpu_target}
    )
endforeach()

if(USE_HIPCXX)
    set_source_files_properties(unittest/testframework.cu
        PROPERTIES
        LANGUAGE HIP
    )
else()
    set_source_files_properties(unittest/testframework.cu
        PROPERTIES
        LANGUAGE CXX
    )
endif()

if(NOT CMAKE_VERSION VERSION_LESS 3.13)
    add_library(testframework OBJECT unittest/testframework.cu)
else() # Workaround
    add_library(testframework STATIC unittest/testframework.cu)
endif()
target_compile_options(testframework PRIVATE ${COMPILE_OPTIONS})

if(HIP_COMPILER STREQUAL "nvcc")
    set_source_files_properties(unittest/cuda/testframework.cu
        PROPERTIES
            LANGUAGE CXX
    )
    target_sources(testframework PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/unittest/cuda/testframework.cu)
elseif(HIP_COMPILER STREQUAL "clang")
    if(USE_HIPCXX)
        set_source_files_properties(unittest/hip/testframework.cu
        PROPERTIES
            LANGUAGE HIP
        )
    else()
        set_source_files_properties(unittest/hip/testframework.cu
        PROPERTIES
            LANGUAGE CXX
        )
    endif()
    target_sources(testframework PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/unittest/hip/testframework.cu)
endif()

target_link_libraries(testframework
    PUBLIC
        testing_common
)

function(add_thrust_test TEST)
    set(TEST_SOURCE "${TEST}.cu")
    set(TEST_TARGET "test_thrust_${TEST}")
    # Unless this property isn't set, CMake silently discards .cu files when
    # CUDA language has not been enabled. If enabled, it will do the regular compiler
    # detection and search for a fully functioning CUDA compiler, which hipcc isn't (yet).
    # Note that removing this property builds will continue to pass, running the tests
    # will also succeed, however the tests will be empty and be near empty main functions.
    # This code path used to add "-x c++" to the compiler flags too, but that was
    # superfluous as "-x hip" was already present on the command-line due to linking to
    # hip::device transitively from roc::rocprim_hip.
    if(USE_HIPCXX)
        set_source_files_properties(${TEST_SOURCE}
            PROPERTIES
                LANGUAGE HIP
        )
    else()
        set_source_files_properties(${TEST_SOURCE}
            PROPERTIES
                LANGUAGE CXX
        )
    endif()
    if(NOT CMAKE_VERSION VERSION_LESS 3.13)
        add_executable(${TEST_TARGET} ${TEST_SOURCE} $<TARGET_OBJECTS:testframework>)
        target_link_libraries(${TEST_TARGET}
            PRIVATE
                testing_common
        )
    else() # Workaround
        add_executable(${TEST_TARGET} ${TEST_SOURCE} unittest/testframework.cu)
        target_link_libraries(${TEST_TARGET}
            PRIVATE
                testing_common
                testframework
        )
    endif()
    target_compile_options(${TEST_TARGET} PRIVATE ${COMPILE_OPTIONS})
    set_target_properties(${TEST_TARGET}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testing/"
    )
    if(AMDGPU_TEST_TARGETS)
    foreach(AMDGPU_TARGET IN LISTS AMDGPU_TEST_TARGETS)
        add_test("${AMDGPU_TARGET}-${TEST}" ${TEST_TARGET})
        set_tests_properties("${AMDGPU_TARGET}-${TEST}"
            PROPERTIES
                RESOURCE_GROUPS "1,${AMDGPU_TARGET}:1"
                LABELS "upstream;${AMDGPU_TARGET}"
        )
    endforeach()
    else()
        add_test(${TEST} ${TEST_TARGET})
        set_tests_properties(${TEST}
            PROPERTIES
                LABELS upstream
        )
    endif()
    rocm_install(TARGETS ${TEST_TARGET} COMPONENT tests)
endfunction()

# ****************************************************************************
# Tests
# ****************************************************************************

add_thrust_test("adjacent_difference")
add_thrust_test("advance")
add_thrust_test("alignment")
add_thrust_test("allocator")
add_thrust_test("allocator_aware_policies")
add_thrust_test("async_copy")
add_thrust_test("async_for_each")
add_thrust_test("async_reduce")
add_thrust_test("async_reduce_into")
add_thrust_test("async_sort")
add_thrust_test("async_transform")
add_thrust_test("binary_search")
add_thrust_test("binary_search_descending")
add_thrust_test("binary_search_vector")
add_thrust_test("binary_search_vector_descending")
add_thrust_test("caching_allocator")
add_thrust_test("complex")
add_thrust_test("complex_transform")
add_thrust_test("constant_iterator")
add_thrust_test("copy")
add_thrust_test("copy_n")
add_thrust_test("count")
add_thrust_test("counting_iterator")
add_thrust_test("decompose")
add_thrust_test("dependencies_aware_policies")
add_thrust_test("dereference")
add_thrust_test("device_delete")
add_thrust_test("device_ptr")
add_thrust_test("device_reference")
add_thrust_test("discard_iterator")
add_thrust_test("distance")
add_thrust_test("equal")
add_thrust_test("event")
add_thrust_test("fill")
# add_thrust_test("find") # Disabeld due to issue #516
# add_thrust_test("for_each") # Disabled due to issze #516
add_thrust_test("functional")
add_thrust_test("functional_arithmetic")
add_thrust_test("functional_bitwise")
add_thrust_test("functional_logical")
add_thrust_test("functional_placeholders_arithmetic")
add_thrust_test("functional_placeholders_bitwise")
add_thrust_test("functional_placeholders_compound_assignment")
add_thrust_test("functional_placeholders_logical")
add_thrust_test("functional_placeholders_miscellaneous")
add_thrust_test("functional_placeholders_relational")
add_thrust_test("future")
add_thrust_test("gather")
add_thrust_test("generate")
add_thrust_test("generate_const_iterators")
add_thrust_test("inner_product")
add_thrust_test("is_contiguous_iterator")
add_thrust_test("is_operator_function_object")
add_thrust_test("is_partitioned")
add_thrust_test("is_sorted")
add_thrust_test("is_sorted_until")
add_thrust_test("logical")
add_thrust_test("max_element")
add_thrust_test("memory")
add_thrust_test("merge")
add_thrust_test("merge_by_key")
add_thrust_test("merge_key_value")
add_thrust_test("metaprogamming")
add_thrust_test("minmax_element")
add_thrust_test("min_and_max")
add_thrust_test("min_element")
add_thrust_test("mismatch")
add_thrust_test("mr_disjoint_pool")
add_thrust_test("mr_new")
add_thrust_test("mr_pool")
add_thrust_test("mr_pool_options")
add_thrust_test("out_of_memory_recovery")
add_thrust_test("pair")
add_thrust_test("pair_reduce")
add_thrust_test("pair_scan")
add_thrust_test("pair_scan_by_key")
add_thrust_test("pair_sort")
add_thrust_test("pair_sort_by_key")
add_thrust_test("pair_transform")
add_thrust_test("partition")
add_thrust_test("partition_point")
add_thrust_test("permutation_iterator")
add_thrust_test("preprocessor")
add_thrust_test("random")
add_thrust_test("reduce")
add_thrust_test("reduce_by_key")
add_thrust_test("reduce_large")
add_thrust_test("remove")
add_thrust_test("replace")
add_thrust_test("reverse")
add_thrust_test("reverse_iterator")
add_thrust_test("scan")
add_thrust_test("scan_by_key.exclusive")
add_thrust_test("scan_by_key.inclusive")
add_thrust_test("scatter")
add_thrust_test("sequence")
add_thrust_test("set_difference")
add_thrust_test("set_difference_by_key")
add_thrust_test("set_difference_by_key_descending")
add_thrust_test("set_difference_descending")
add_thrust_test("set_difference_key_value")
add_thrust_test("set_intersection")
add_thrust_test("set_intersection_by_key")
add_thrust_test("set_intersection_by_key_descending")
add_thrust_test("set_intersection_descending")
add_thrust_test("set_intersection_key_value")
add_thrust_test("set_symmetric_difference")
add_thrust_test("set_symmetric_difference_by_key")
add_thrust_test("set_symmetric_difference_by_key_descending")
add_thrust_test("set_symmetric_difference_descending")
add_thrust_test("set_union")
add_thrust_test("set_union_by_key")
add_thrust_test("set_union_by_key_descending")
add_thrust_test("set_union_descending")
add_thrust_test("set_union_key_value")
add_thrust_test("shuffle")
add_thrust_test("sort")
add_thrust_test("sort_by_key")
add_thrust_test("sort_by_key_variable_bits")
add_thrust_test("sort_permutation_iterator")
add_thrust_test("sort_variable_bits")
add_thrust_test("stable_sort")
add_thrust_test("stable_sort_by_key")
add_thrust_test("stable_sort_by_key_large_keys")
add_thrust_test("stable_sort_by_key_large_keys_and_values")
add_thrust_test("stable_sort_by_key_large_values")
add_thrust_test("stable_sort_large")
add_thrust_test("swap_ranges")
add_thrust_test("tabulate")
add_thrust_test("transform")
add_thrust_test("transform_iterator")
add_thrust_test("transform_input_output_iterator")
add_thrust_test("transform_output_iterator_reduce_by_key")
add_thrust_test("transform_output_iterator")
add_thrust_test("transform_reduce")
add_thrust_test("transform_scan")
add_thrust_test("trivial_sequence")
add_thrust_test("tuple")
add_thrust_test("tuple_algorithms")
add_thrust_test("tuple_reduce")
add_thrust_test("tuple_scan")
add_thrust_test("tuple_sort")
add_thrust_test("tuple_transform")
add_thrust_test("type_traits")
add_thrust_test("uninitialized_copy")
add_thrust_test("uninitialized_fill")
add_thrust_test("unique")
add_thrust_test("unique_by_key")
add_thrust_test("universal_memory")
add_thrust_test("unittest_static_assert")
add_thrust_test("unittest_tester")
add_thrust_test("vector")
add_thrust_test("vector_allocators")
add_thrust_test("vector_insert")
add_thrust_test("vector_manipulation")
add_thrust_test("zip_function")
add_thrust_test("zip_iterator")
add_thrust_test("zip_iterator_reduce")
add_thrust_test("zip_iterator_reduce_by_key")
add_thrust_test("zip_iterator_scan")
add_thrust_test("zip_iterator_sort")
add_thrust_test("zip_iterator_sort_by_key")

# async test
add_subdirectory(async)
