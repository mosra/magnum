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

#include <Corrade/Containers/String.h>
#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/Utility/Path.h>

#ifdef CORRADE_TARGET_APPLE
#include <Corrade/Utility/System.h> /* isSandboxed() */
#endif

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#ifdef MAGNUM_TARGET_GLES
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#endif
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/OpenGLTester.h"
#ifdef MAGNUM_TARGET_GLES2
#include "Magnum/GL/PixelFormat.h"
#endif
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/TextureTools/DistanceFieldGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct DistanceFieldGLBenchmark: GL::OpenGLTester {
    explicit DistanceFieldGLBenchmark();

    void benchmark();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        Containers::String _testDir;
};

using namespace Math::Literals;

const struct {
    const char* name;
    GL::TextureFormat format;
} BenchmarkData[]{
    /* On ES2 the format selection is too annoying, just skip the variants
       altogether and pick *some* format in the function itself */
    #ifdef MAGNUM_TARGET_GLES2
    {"", GL::TextureFormat{}},
    #else
    {"R8 output", GL::TextureFormat::R8},
    {"RGB8 output", GL::TextureFormat::RGB8},
    {"RGBA8 output", GL::TextureFormat::RGBA8},
    {"R16 output", GL::TextureFormat::R16},
    #endif
};

DistanceFieldGLBenchmark::DistanceFieldGLBenchmark() {
    addInstancedBenchmarks({&DistanceFieldGLBenchmark::benchmark}, 10,
        Containers::arraySize(BenchmarkData),
        BenchmarkType::GpuTime);

    /* Load the plugin directly from the build tree. Otherwise it's either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    #ifdef CORRADE_TARGET_APPLE
    if(Utility::System::isSandboxed()
        #if defined(CORRADE_TARGET_IOS) && defined(CORRADE_TESTSUITE_TARGET_XCTEST)
        /** @todo Fix this once I persuade CMake to run XCTest tests properly */
        && std::getenv("SIMULATOR_UDID")
        #endif
    ) {
        _testDir = Utility::Path::join(Utility::Path::path(*Utility::Path::executableLocation()), "DistanceFieldGLTestFiles");
    } else
    #endif
    {
        _testDir = Utility::Path::join(TEXTURETOOLS_TEST_DIR, "DistanceFieldGLTestFiles");
    }
}

void DistanceFieldGLBenchmark::benchmark() {
    auto&& data = BenchmarkData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _manager.loadAndInstantiate("TgaImporter")))
        CORRADE_SKIP("TgaImporter plugin not found.");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "input.tga")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    Containers::Optional<Trade::ImageData2D> inputImage = importer->image2D(0);
    CORRADE_VERIFY(inputImage);
    CORRADE_COMPARE(inputImage->format(), PixelFormat::R8Unorm);

    #ifndef MAGNUM_TARGET_GLES2
    const GL::TextureFormat inputFormat = GL::TextureFormat::R8;
    #else
    GL::TextureFormat inputFormat;
    #ifndef MAGNUM_TARGET_WEBGL
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        inputFormat = GL::TextureFormat::R8;
    } else
    #endif
    {
        inputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
    }
    #endif

    GL::Texture2D input;
    input.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, inputFormat, inputImage->size());

    #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    input.setSubImage(0, {}, *inputImage);
    #else
    #ifndef MAGNUM_TARGET_WEBGL
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        input.setSubImage(0, {}, ImageView2D{inputImage->storage(), GL::PixelFormat::Red, GL::PixelType::UnsignedByte, inputImage->size(), inputImage->data()});
    } else
    #endif
    {
        input.setSubImage(0, {}, *inputImage);
    }
    #endif

    /* On ES2 the format selection is too annoying, just skip the variants
       altogether and pick *some* format in the function itself */
    #ifndef MAGNUM_TARGET_GLES2
    const GL::TextureFormat outputFormat = data.format;
    #else
    GL::TextureFormat outputFormat;
    #ifndef MAGNUM_TARGET_WEBGL
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        outputFormat = GL::TextureFormat::R8;
    } else
    #endif
    {
        outputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
    }
    #endif

    GL::Texture2D output;
    output.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, outputFormat, Vector2i{64});

    GL::Framebuffer framebuffer{{{}, Vector2i{64}}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment(0), output, 0);

    MAGNUM_VERIFY_NO_GL_ERROR();

    DistanceFieldGL distanceField{32};

    CORRADE_BENCHMARK(50) {
        distanceField(input, framebuffer, {{}, Vector2i{64}}
            #ifdef MAGNUM_TARGET_GLES
            , inputImage->size()
            #endif
            );
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::DistanceFieldGLBenchmark)
