# CPU-only tests for the stencil reference implementations.
add_executable(stable_fluids_cpu_tests
    GridIndexTests.cpp
    CpuStencilReferenceTests.cpp
)
target_include_directories(stable_fluids_cpu_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(stable_fluids_cpu_tests PRIVATE)
stable_fluids_set_warnings(stable_fluids_cpu_tests)

add_test(NAME stable_fluids_cpu_tests COMMAND stable_fluids_cpu_tests)
