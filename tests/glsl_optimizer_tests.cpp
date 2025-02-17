#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <utility>
#include <stdexcept>

#include "glsl_optimizer.h"

#define VERTEX_SHADER kGlslOptShaderVertex
#define FRAGMENT_SHADER kGlslOptShaderFragment

#define TEST_COMPILE_SHADER(type, src, expected) \
    auto [success, output] = compileShader(type, src); \
    EXPECT_TRUE(success) << "failed to compile shader: " << output; \
    EXPECT_EQ(expected, output)

using namespace ::testing;

namespace {

class OptimizerTest : public ::testing::Test
{
public:
    glslopt_target shaderTargetLang { kGlslTargetOpenGLES20 };

    [[nodiscard]] std::pair<bool, std::string> compileShader(glslopt_shader_type type, const std::string& shaderSrc) const;
};

// NOLINTNEXTLINE
TEST_F(OptimizerTest, VertexShader)
{
    TEST_COMPILE_SHADER(VERTEX_SHADER, R"GLSL(
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
    )GLSL", R"GLSL(attribute highp vec4 vPosition;
attribute highp vec4 vColor;
attribute highp vec2 vTexcoord;
varying highp vec4 color;
varying highp vec2 uv;
void main ()
{
  gl_Position = vPosition;
  color = vColor;
  uv = vTexcoord;
})GLSL");
}

// NOLINTNEXTLINE
TEST_F(OptimizerTest, FragmentShader)
{
    TEST_COMPILE_SHADER(FRAGMENT_SHADER, R"GLSL(
precision mediump float;

uniform sampler2D mainTex;

varying vec4 color;
varying vec2 uv;

void main()
{
	gl_FragColor = texture2D(mainTex, uv) * color;
}
    )GLSL", R"GLSL(precision mediump float;
uniform lowp sampler2D mainTex;
varying mediump vec4 color;
varying mediump vec2 uv;
void main ()
{
  gl_FragColor = (texture2D (mainTex, uv) * color);
})GLSL");
}

// NOLINTNEXTLINE
TEST_F(OptimizerTest, FragmentShaderHighPrecision)
{
    TEST_COMPILE_SHADER(FRAGMENT_SHADER, R"GLSL(
precision mediump float;

uniform sampler2D mainTex;

varying vec4 color;
varying highp vec2 uv;

void main()
{
	gl_FragColor = texture2D(mainTex, uv) * color;
}
    )GLSL", R"GLSL(precision mediump float;
uniform lowp sampler2D mainTex;
varying mediump vec4 color;
varying highp vec2 uv;
void main ()
{
  gl_FragColor = (texture2D (mainTex, uv) * color);
})GLSL");
}

std::pair<bool, std::string> OptimizerTest::compileShader(glslopt_shader_type type, const std::string& shaderSrc) const
{
    auto ctx = glslopt_initialize(shaderTargetLang);
    auto shader = glslopt_optimize (ctx, type, shaderSrc.c_str(), 0);
    bool success = glslopt_get_status (shader);

    const char* outp = success ? glslopt_get_output(shader) : glslopt_get_log(shader);
    std::string output = outp ? std::string(outp) : std::string{};

    glslopt_cleanup(ctx);

    if (!outp)
        throw std::runtime_error { "unexpected null pointer" };

    while (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    return std::make_pair(success, output);
}

}
