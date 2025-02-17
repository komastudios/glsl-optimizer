add_executable(glsl_optimizer_test test.cpp)
target_link_libraries(glsl_optimizer_test glsl_optimizer gtest gmock gtest_main)

enable_testing()
add_test(NAME GlslOptimizerTest
     COMMAND glsl_optimizer_test
     WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
