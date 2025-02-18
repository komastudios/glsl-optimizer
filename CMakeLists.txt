cmake_minimum_required(VERSION 3.14)

project(glsl_optimizer 
    VERSION 0.1
    DESCRIPTION "GLSL Optimizer"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fwrapv -fno-strict-aliasing)
endif()

option(GLSL_OPTIMIZER_TOOLS "Build tools" OFF)
option(GLSL_OPTIMIZER_UNIT_TESTS "Build unit tests" OFF)
option(GLSL_OPTIMIZER_RUNTIME_TESTS "Build runtime tests" OFF)

if (GLSL_OPTIMIZER_UNIT_TESTS)
    enable_testing()
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.16.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

add_subdirectory(src)
add_subdirectory(contrib)

add_subdirectory(tests)
