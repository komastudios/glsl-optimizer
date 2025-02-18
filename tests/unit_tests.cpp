#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <utility>
#include <stdexcept>
#include <string_view>
#include <future>
#include <optional>
#include <mutex>

#include "glsl_optimizer.h"

#define VERTEX_SHADER kGlslOptShaderVertex
#define FRAGMENT_SHADER kGlslOptShaderFragment

#define TEST_COMPILE_SHADER(type, src, expected) \
    auto [success, output] = compileShader(type, src); \
    EXPECT_TRUE(success) << "failed to compile shader: " << output; \
    EXPECT_EQ(TrimStr(expected), TrimStr(output))

using namespace ::testing;

namespace {

constexpr std::string_view kWhitespaceCharacters { " \n\r\t" };

constexpr std::string_view TrimLeft(std::string_view source)
{
    const auto idx = source.find_first_not_of(kWhitespaceCharacters);
    return idx != std::string_view::npos ? source.substr(idx) : std::string_view{};
}

constexpr std::string_view TrimRight(std::string_view source)
{
    const auto idx = source.find_last_not_of(kWhitespaceCharacters);
    return idx != std::string_view::npos ? source.substr(0, idx+1) : std::string_view{};
}

constexpr std::string_view TrimStr(std::string_view source)
{
    return TrimRight(TrimLeft(source));
}

class OptimizerTest : public ::testing::TestWithParam<std::tuple<size_t, bool>>
{
public:
    glslopt_target shaderTargetLang { kGlslTargetOpenGLES20 };

    [[nodiscard]] std::pair<bool, std::string> compileShader(glslopt_shader_type type, const std::string& shaderSrc) const;
};

// NOLINTNEXTLINE
TEST_P(OptimizerTest, VertexShader)
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
TEST_P(OptimizerTest, FragmentShader)
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
uniform sampler2D mainTex;
varying vec4 color;
varying vec2 uv;
void main ()
{
  lowp vec4 tmpvar_1;
  tmpvar_1 = texture2D (mainTex, uv);
  gl_FragColor = (tmpvar_1 * color);
})GLSL");
}

// NOLINTNEXTLINE
TEST_P(OptimizerTest, FragmentShaderHighPrecision)
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
uniform sampler2D mainTex;
varying vec4 color;
varying highp vec2 uv;
void main ()
{
  lowp vec4 tmpvar_1;
  tmpvar_1 = texture2D (mainTex, uv);
  gl_FragColor = (tmpvar_1 * color);
})GLSL");
}

// NOLINTNEXTLINE
TEST_P(OptimizerTest, FragmentShaderShadowSampler)
{
    TEST_COMPILE_SHADER(FRAGMENT_SHADER, R"GLSL(
#extension GL_EXT_shadow_samplers : require

precision mediump float;

uniform sampler2D mainTex;
uniform sampler2DShadow depthTex;

varying vec4 color;
varying vec2 uv;
varying highp vec4 projShadow;

void main()
{
    float val = shadow2DProjEXT(depthTex, projShadow);
	gl_FragColor = vec4(texture2D(mainTex, uv).rgb * color.rgb * val, color.a);
}
    )GLSL", R"GLSL(#extension GL_EXT_shadow_samplers : enable
precision mediump float;
uniform sampler2D mainTex;
uniform lowp sampler2DShadow depthTex;
varying vec4 color;
varying vec2 uv;
varying highp vec4 projShadow;
void main ()
{
  lowp vec4 tmpvar_1;
  tmpvar_1.xyz = ((texture2D (mainTex, uv).xyz * color.xyz) * shadow2DProjEXT (depthTex, projShadow));
  tmpvar_1.w = color.w;
  gl_FragColor = tmpvar_1;
})GLSL");
}

using CompilerResult = std::pair<bool, std::string>;

CompilerResult CompileShader(glslopt_target targetLang, glslopt_shader_type type, const char* shaderSrc)
{
    auto* ctx = glslopt_initialize(targetLang);
    if (!ctx) {
        throw std::runtime_error { "failed to initialize glslopt context" };
    }
    auto* shader = glslopt_optimize (ctx, type, shaderSrc, 0);
    bool success = shader ? glslopt_get_status (shader) : false;

    const char* outp = shader ? (success ? glslopt_get_output(shader) : glslopt_get_log(shader)) : nullptr;
    std::string output = outp ? std::string(outp) : std::string{};

    glslopt_shader_delete(shader);
    glslopt_cleanup(ctx); // TODO: implement global context cleanup (thread-safe)

    if (!outp) {
        throw std::runtime_error { "unexpected null pointer" };
    }

    return std::make_pair(success, output);
}

CompilerResult OptimizerTest::compileShader(glslopt_shader_type type, const std::string& shaderSrc) const
{
    size_t asyncTasks = std::get<0>(GetParam());
    bool synchronized = std::get<1>(GetParam());
    if (asyncTasks == 0) {
        return CompileShader(shaderTargetLang, type, shaderSrc.c_str());
    }

    std::vector<std::future<CompilerResult>> futures;
    futures.reserve(asyncTasks);

    std::mutex mutex;
    for (size_t i=0;i<asyncTasks;++i) {
        if (synchronized) {
            futures.push_back(std::async(std::launch::async, [&mutex](glslopt_target targetLang, glslopt_shader_type type, const char* shaderSrc)
            {
                std::unique_lock lk(mutex);
                return CompileShader(targetLang, type, shaderSrc);
            }, shaderTargetLang, type, shaderSrc.c_str()));
        }
        else
        {
            futures.push_back(std::async(std::launch::async, &CompileShader, shaderTargetLang, type, shaderSrc.c_str()));
        }
    }

    std::optional<CompilerResult> result;
    std::vector<std::exception_ptr> errors;
    for (size_t i=0;i<asyncTasks;++i) {
        try
        {
            auto taskResult = futures[i].get();
            if (!result.has_value()) {
                result = std::make_optional(taskResult);
            }
            else {
                if (result->first != taskResult.first || result->second != taskResult.second) {
                    throw std::runtime_error { "async error: inconsistent results" };
                }
            }
        }
        catch (...)
        {
            errors.push_back(std::current_exception());
        }
    }

    return result.has_value() ? *result : std::make_pair(false, "async error: no valid result");
}

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(AsyncJobsSynchronized,
                         OptimizerTest,
                         testing::Combine(
                             testing::Values(1, 2, 4),
                             testing::Values(true)
                         ));

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(AsyncJobs,
                         OptimizerTest,
                         testing::Combine(
                             testing::Values(0, 1, 4, 8, 16, 32, 64, 256, 512, 1024),
                             testing::Values(false)
                         ));

} // namespace
