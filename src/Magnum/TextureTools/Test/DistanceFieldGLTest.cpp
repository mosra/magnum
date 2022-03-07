/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/StringStl.h> /** @todo remove when AbstractImporter is <string>-free */
#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/Utility/Path.h>

#ifdef CORRADE_TARGET_APPLE
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/System.h> /* isSandboxed() */
#endif

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/DebugTools/TextureImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/TextureTools/DistanceField.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
#endif

#include "configure.h"

#ifdef MAGNUM_TARGET_GLES
#include <Corrade/Containers/StringView.h> /* for SwiftShader detection */
#endif

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct DistanceFieldGLTest: GL::OpenGLTester {
    explicit DistanceFieldGLTest();

    void test();
    #ifndef MAGNUM_TARGET_WEBGL
    void benchmark();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        Containers::String _testDir;
};

#ifdef MAGNUM_TARGET_GLES
using namespace Containers::Literals; /* for SwiftShader detection */
#endif

DistanceFieldGLTest::DistanceFieldGLTest() {
    addTests({&DistanceFieldGLTest::test});

    #ifndef MAGNUM_TARGET_WEBGL
    addBenchmarks({&DistanceFieldGLTest::benchmark}, 5, BenchmarkType::GpuTime);
    #endif

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
        _testDir = Utility::Path::join(Utility::Path::split(*Utility::Path::executableLocation()).first(), "DistanceFieldGLTestFiles");
    } else
    #endif
    {
        _testDir = DISTANCEFIELDGLTEST_FILES_DIR;
    }
}

void DistanceFieldGLTest::test() {
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
    #elif !defined(MAGNUM_TARGET_WEBGL)
    GL::TextureFormat inputFormat;
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        CORRADE_INFO("Using" << GL::Extensions::EXT::texture_rg::string());
        inputFormat = GL::TextureFormat::R8;
    } else {
        inputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
    }
    #else
    const GL::TextureFormat inputFormat = GL::TextureFormat::Luminance;
    #endif

    GL::Texture2D input;
    input.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, inputFormat, inputImage->size());

    #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    input.setSubImage(0, {}, *inputImage);
    #else
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        input.setSubImage(0, {}, ImageView2D{inputImage->storage(), GL::PixelFormat::Red, GL::PixelType::UnsignedByte, inputImage->size(), inputImage->data()});
    else
        input.setSubImage(0, {}, *inputImage);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    const GL::TextureFormat outputFormat = GL::TextureFormat::R8;
    #elif !defined(MAGNUM_TARGET_WEBGL)
    GL::TextureFormat outputFormat;
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        outputFormat = GL::TextureFormat::R8;
    else
        outputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
    #else
    const GL::TextureFormat outputFormat = GL::TextureFormat::RGBA;
    #endif

    GL::Texture2D output;
    output.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, outputFormat, Vector2i{64});

    TextureTools::DistanceField distanceField{32};
    CORRADE_COMPARE(distanceField.radius(), 32);

    MAGNUM_VERIFY_NO_GL_ERROR();

    distanceField(input, output, {{}, Vector2i{64}}
        #ifdef MAGNUM_TARGET_GLES
        , inputImage->size()
        #endif
        );

    #ifdef MAGNUM_TARGET_GLES
    {
        /* Probably due to the luminance target pixel format? Works with 4.1,
           didn't find any commit in between that would clearly affect
           this. */
        CORRADE_EXPECT_FAIL_IF(GL::Context::current().versionString().contains("SwiftShader 4.0.0"_s),
            "SwiftShader 4.0.0 has a bug where the framebuffer is considered incomplete.");
        MAGNUM_VERIFY_NO_GL_ERROR();
        if(GL::Context::current().versionString().contains("SwiftShader 4.0.0"_s))
            CORRADE_SKIP("Skipping the rest of the test.");
    }
    #endif

    Containers::Optional<Image2D> actualOutputImage;
    #ifndef MAGNUM_TARGET_GLES2
    actualOutputImage = Image2D{PixelFormat::R8Unorm};
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        actualOutputImage = Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte};
    else
        actualOutputImage = Image2D{PixelFormat::R8Unorm};
    #else
    actualOutputImage = Image2D{PixelFormat::RGBA8Unorm};
    #endif

    DebugTools::textureSubImage(output, 0, {{}, Vector2i{64}}, *actualOutputImage);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /** @todo Do this via some TextureTools::pixelFormatTransform() */
    if(actualOutputImage->format() == PixelFormat::RGBA8Unorm) {
        /* Shrink the data */
        Containers::ArrayView<const Color4ub> in = Containers::arrayCast<const Color4ub>(actualOutputImage->data());
        Containers::ArrayView<UnsignedByte> out = Containers::arrayCast<UnsignedByte>(actualOutputImage->data());
        for(std::size_t i = 0; i != in.size(); ++i) {
            out[i] = in[i].r();
        }

        actualOutputImage = Image2D{PixelFormat::R8Unorm, actualOutputImage->size(), actualOutputImage->release()};
    }

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* In some cases actualOutputImage might have GL-specific format,
       reinterpret as R8Unorm for the comparison to work */
    if(actualOutputImage->format() == pixelFormatWrap(GL::PixelFormat::Red)) {
        const Vector2i imageSize = actualOutputImage->size();
        actualOutputImage = Image2D{actualOutputImage->storage(), PixelFormat::R8Unorm, imageSize, actualOutputImage->release()};
    }
    #endif

    CORRADE_COMPARE_WITH(*actualOutputImage,
        Utility::Path::join(_testDir, "output.tga"),
        /* Some mobile GPUs have slight (off-by-one) rounding errors compared
           to the ground truth, but it's just a very small amount of pixels
           (20-50 out of the total 4k pixels, iOS/WebGL has slightly more).
           That's okay. It's also possible that the ground truth itself has
           rounding errors ;) */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.178f}));
}

#ifndef MAGNUM_TARGET_WEBGL
void DistanceFieldGLTest::benchmark() {
    #ifdef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::disjoint_timer_query>())
        CORRADE_SKIP(GL::Extensions::EXT::disjoint_timer_query::string() << "is not supported, can't benchmark");
    #endif

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
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        inputFormat = GL::TextureFormat::R8;
    else
        inputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
    #endif

    GL::Texture2D input;
    input.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, inputFormat, inputImage->size());

    #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    input.setSubImage(0, {}, *inputImage);
    #else
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        input.setSubImage(0, {}, ImageView2D{inputImage->storage(), GL::PixelFormat::Red, GL::PixelType::UnsignedByte, inputImage->size(), inputImage->data()});
    else
        input.setSubImage(0, {}, *inputImage);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    const GL::TextureFormat outputFormat = GL::TextureFormat::R8;
    #else
    GL::TextureFormat outputFormat;
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        outputFormat = GL::TextureFormat::R8;
    else
        outputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
    #endif

    GL::Texture2D output;
    output.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, outputFormat, Vector2i{64});

    MAGNUM_VERIFY_NO_GL_ERROR();

    TextureTools::DistanceField distanceField{32};

    /* So it doesn't spam too much */
    GL::DebugOutput::setCallback(nullptr);

    CORRADE_BENCHMARK(5) {
        /* This is creating the shader from scratch every time, so no wonder
           it's so freaking slow */
        distanceField(input, output, {{}, Vector2i{64}}
            #ifdef MAGNUM_TARGET_GLES
            , inputImage->size()
            #endif
            );

        MAGNUM_VERIFY_NO_GL_ERROR();
    }

    GL::DebugOutput::setDefaultCallback();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::DistanceFieldGLTest)
