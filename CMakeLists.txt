cmake_minimum_required(VERSION 3.23)

project(glsl_optimizer 
    VERSION 0.1
    DESCRIPTION "GLSL Optimizer"
    LANGUAGES C CXX)

if(PROJECT_IS_TOP_LEVEL)
    include(Sanitizers.cmake)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TESTING "Build unit tests" ON)
option(GLSL_OPTIMIZER_TOOLS "Build tools" ON)
option(GLSL_OPTIMIZER_REGRESSION_TESTS "Build runtime tests" OFF)

if (BUILD_TESTING)
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
