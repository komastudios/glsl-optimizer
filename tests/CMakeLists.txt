if (GLSL_OPTIMIZER_REGRESSION_TESTS)
    add_executable(glsl_test glsl_optimizer_tests.cpp)
    target_link_libraries(glsl_test glsl_optimizer)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        # OSX-specific build requirements
        find_library(OpenGL_LIBRARY OpenGL)
        target_link_libraries(glsl_test ${OpenGL_LIBRARY})
    endif()
endif()

if (BUILD_TESTING)
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

    add_executable(glsl_optimizer_unit_tests unit_tests.cpp)
    set_property(TARGET glsl_optimizer_unit_tests PROPERTY CXX_STANDARD 17)
    set_property(TARGET glsl_optimizer_unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(glsl_optimizer_unit_tests glsl_optimizer gtest gmock gtest_main)

    add_test(NAME GlslOptimizerUnitTests
            COMMAND glsl_optimizer_unit_tests
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif ()
