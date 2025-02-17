set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wshadow -Wpedantic
            -Wconversion -Wno-gcc-compat
            -Wno-sign-conversion
            -Wno-unused-parameter
            -Wuninitialized -Wmissing-field-initializers
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-overlength-strings)
    add_compile_options(-Werror)
endif()


add_executable(glsl_optimizer_unit_tests glsl_optimizer_tests.cpp)
target_link_libraries(glsl_optimizer_unit_tests glsl_optimizer gtest gmock gtest_main)

add_test(NAME GlslOptimizerUnitTests
     COMMAND glsl_optimizer_unit_tests
     WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
