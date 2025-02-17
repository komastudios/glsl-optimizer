set(glslopt_sources
    glslopt/Main.cpp
)

if (GLSL_OPTIMIZER_TOOLS)
    add_executable(glslopt ${glslopt_sources})
    target_link_libraries(glslopt glsl_optimizer)
endif ()
