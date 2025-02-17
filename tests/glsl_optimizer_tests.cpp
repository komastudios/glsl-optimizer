#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>

#include "glsl_optimizer.h"

using namespace ::testing;

namespace {

// NOLINTNEXTLINE
TEST(GlslOptimTests, CanOptimizeSingleShader)
{
    auto ctx = glslopt_initialize(kGlslTargetOpenGLES20);

    const char* vShaderSrc = R""""(
attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec2 vTexcoord;

varying vec4 color;
varying vec2 uv;

void main() {
    gl_Position = vPosition;
    color = vColor;
    uv = vTexcoord;
}
    )"""";

    auto shader = glslopt_optimize (ctx, kGlslOptShaderVertex, vShaderSrc, 0);
    EXPECT_TRUE(glslopt_get_status (shader)) << "failed to compile shader: " << glslopt_get_log (shader);

    std::string output = glslopt_get_output(shader);
    EXPECT_EQ(output, R""""(attribute highp vec4 vPosition;
attribute highp vec4 vColor;
attribute highp vec2 vTexcoord;
varying highp vec4 color;
varying highp vec2 uv;
void main ()
{
  gl_Position = vPosition;
  color = vColor;
  uv = vTexcoord;
}

)"""");

    glslopt_cleanup(ctx);
}

}
