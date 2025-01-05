/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/System.h>

#include "Magnum/DebugTools/FrameProfiler.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Shaders/FlatGL.h"
#include "Magnum/Trade/MeshData.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Primitives/Plane.h"
#endif

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct FrameProfilerGLTest: GL::OpenGLTester {
    explicit FrameProfilerGLTest();

    void test();
    #ifndef MAGNUM_TARGET_GLES
    void vertexFetchRatioDivisionByZero();
    void primitiveClipRatioDivisionByZero();
    void primitiveClipRatioNegative();
    #endif
};

using namespace Math::Literals;

struct {
    const char* name;
    FrameProfilerGL::Values values;
} Data[]{
    {"gpu duration", FrameProfilerGL::Value::GpuDuration},
    {"cpu duration + gpu duration", FrameProfilerGL::Value::CpuDuration|FrameProfilerGL::Value::GpuDuration},
    {"frame time + gpu duration", FrameProfilerGL::Value::FrameTime|FrameProfilerGL::Value::GpuDuration},
    #ifndef MAGNUM_TARGET_GLES
    {"gpu duration + vertex fetch ratio", FrameProfilerGL::Value::GpuDuration|FrameProfilerGL::Value::VertexFetchRatio},
    {"vertex fetch ratio + primitive clip ratio", FrameProfilerGL::Value::VertexFetchRatio|FrameProfilerGL::Value::PrimitiveClipRatio}
    #endif
};

FrameProfilerGLTest::FrameProfilerGLTest() {
    addInstancedTests({&FrameProfilerGLTest::test},
        Containers::arraySize(Data));

    #ifndef MAGNUM_TARGET_GLES
    addTests({&FrameProfilerGLTest::vertexFetchRatioDivisionByZero,
              &FrameProfilerGLTest::primitiveClipRatioDivisionByZero,
              &FrameProfilerGLTest::primitiveClipRatioNegative});
    #endif
}

void FrameProfilerGLTest::test() {
    auto&& data = Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(data.values & FrameProfilerGL::Value::GpuDuration) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::timer_query>())
            CORRADE_SKIP(GL::Extensions::ARB::timer_query::string() << "is not supported.");
        #elif defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::disjoint_timer_query_webgl2>())
            CORRADE_SKIP(GL::Extensions::EXT::disjoint_timer_query_webgl2::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::disjoint_timer_query>())
            CORRADE_SKIP(GL::Extensions::EXT::disjoint_timer_query::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if((data.values & FrameProfilerGL::Value::VertexFetchRatio) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::pipeline_statistics_query>())
        CORRADE_SKIP(GL::Extensions::ARB::pipeline_statistics_query::string() << "is not supported.");
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    GL::Renderbuffer color;
    color.setStorage(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        Vector2i{32});
    GL::Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color)
      .bind();

    GL::Mesh mesh = MeshTools::compile(Primitives::cubeSolid());
    Shaders::FlatGL3D shader;

    FrameProfilerGL profiler{data.values, 4};
    CORRADE_COMPARE(profiler.maxFrameCount(), 4);

    /* MSVC 2015 needs the {} */
    UnsignedInt i = 0;
    for(auto value: {FrameProfilerGL::Value::CpuDuration,
                     FrameProfilerGL::Value::GpuDuration,
                     #ifndef MAGNUM_TARGET_GLES
                     FrameProfilerGL::Value::VertexFetchRatio,
                     FrameProfilerGL::Value::PrimitiveClipRatio
                     #endif
                     }) {
        if(!(data.values & value)) continue;

        CORRADE_VERIFY(!profiler.isMeasurementAvailable(value));
        /* The names should not be allocated */
        CORRADE_COMPARE(profiler.measurementName(i++).flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
    }

    profiler.beginFrame();
    shader.draw(mesh);
    Utility::System::sleep(1);
    profiler.endFrame();

    profiler.beginFrame();
    shader.draw(mesh);
    profiler.endFrame();

    Utility::System::sleep(10);

    profiler.beginFrame();
    shader.draw(mesh);
    Utility::System::sleep(1);
    profiler.endFrame();

    profiler.beginFrame();
    shader.draw(mesh);
    Utility::System::sleep(1);
    profiler.endFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The GPU time should not be a total zero. Can't test upper bound because
       (especially on overloaded CIs) it all takes a magnitude more than
       expected. */
    if(data.values & FrameProfilerGL::Value::GpuDuration) {
        CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::GpuDuration));
        CORRADE_COMPARE_AS(profiler.gpuDurationMean(), 100,
            TestSuite::Compare::Greater);
    }

    /* 3/4 frames took 1 ms, the ideal average is 0.75 ms. Can't test upper
       bound because (especially on overloaded CIs) it all takes a magnitude
       more than expected. */
    if(data.values & FrameProfilerGL::Value::CpuDuration) {
        CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::CpuDuration));
        CORRADE_COMPARE_AS(profiler.cpuDurationMean(), 0.70*1000*1000,
            TestSuite::Compare::GreaterOrEqual);
    }

    #ifndef MAGNUM_TARGET_GLES
    /* 24 unique vertices in 12 triangles, ideal ratio is 24/36 */
    if(data.values & FrameProfilerGL::Value::VertexFetchRatio) {
        CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::VertexFetchRatio));
        CORRADE_COMPARE_WITH(profiler.vertexFetchRatioMean()/1000, 0.6667,
            TestSuite::Compare::around(0.1));
    }

    /* We use a default transformation, which means the whole cube should be
       visible, nothing clipped */
    if(data.values & FrameProfilerGL::Value::PrimitiveClipRatio) {
        CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::PrimitiveClipRatio));
        CORRADE_COMPARE(profiler.primitiveClipRatioMean()/1000, 0.0);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void FrameProfilerGLTest::vertexFetchRatioDivisionByZero() {
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::pipeline_statistics_query>())
        CORRADE_SKIP(GL::Extensions::ARB::pipeline_statistics_query::string() << "is not supported.");

    FrameProfilerGL profiler{FrameProfilerGL::Value::VertexFetchRatio, 4};

    profiler.beginFrame();
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* No draws happened, so the ratio should be 0 (and not crashing with a
       division by zero) */
    CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::VertexFetchRatio));
    CORRADE_COMPARE(profiler.vertexFetchRatioMean(), 0.0);
}

void FrameProfilerGLTest::primitiveClipRatioDivisionByZero() {
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::pipeline_statistics_query>())
        CORRADE_SKIP(GL::Extensions::ARB::pipeline_statistics_query::string() << "is not supported.");

    FrameProfilerGL profiler{FrameProfilerGL::Value::PrimitiveClipRatio, 4};

    profiler.beginFrame();
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* No draws happened, so the ratio should be 0 (and not crashing with a
       division by zero) */
    CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::PrimitiveClipRatio));
    CORRADE_COMPARE(profiler.primitiveClipRatioMean(), 0.0);
}

void FrameProfilerGLTest::primitiveClipRatioNegative() {
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::pipeline_statistics_query>())
        CORRADE_SKIP(GL::Extensions::ARB::pipeline_statistics_query::string() << "is not supported.");

    /* Bind some FB to avoid errors on contexts w/o default FB */
    GL::Renderbuffer color;
    color.setStorage(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        Vector2i{32});
    GL::Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color)
      .bind();

    /* Rotate a plane so it cuts through the near/far clipping plane. That
       forces the primitive clipping pipeline to cut the two input primitives
       into four, causing the number of output primitives being larger than the
       number of input primitives, which would then result in an underflow of
       the unsigned counter and tripping up various assertions if not accounted
       for.

       Implementations are allowed to split up primitives in other cases as
       well, but this is guaranteed to happen everywhere. */
    GL::Mesh mesh = MeshTools::compile(Primitives::planeSolid());
    Shaders::FlatGL3D shader;
    shader.setTransformationProjectionMatrix(
        Matrix4::rotationX(45.0_degf)*
        Matrix4::scaling(Vector3{2.0f}));

    FrameProfilerGL profiler{FrameProfilerGL::Value::PrimitiveClipRatio, 4};

    profiler.beginFrame();
    shader.draw(mesh);
    profiler.endFrame();

    profiler.beginFrame();
    shader.draw(mesh);
    profiler.endFrame();

    profiler.beginFrame();
    shader.draw(mesh);
    profiler.endFrame();

    profiler.beginFrame();
    shader.draw(mesh);
    profiler.endFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* No draws happened, so the ratio should be 0 (and not crashing with a
       division by zero) */
    CORRADE_VERIFY(profiler.isMeasurementAvailable(FrameProfilerGL::Value::PrimitiveClipRatio));
    CORRADE_COMPARE(profiler.primitiveClipRatioMean(), 0.0);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::FrameProfilerGLTest)
