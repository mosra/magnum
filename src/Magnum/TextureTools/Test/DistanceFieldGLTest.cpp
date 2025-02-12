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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Path.h>

#ifdef CORRADE_TARGET_APPLE
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
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/TextureTools/DistanceFieldGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct DistanceFieldGLTest: GL::OpenGLTester {
    explicit DistanceFieldGLTest();

    void construct();
    void constructCopy();
    void constructMove();

    void runTexture();
    void runFramebuffer();

    void formatNotDrawable();
    void sizeRatioNotMultipleOfTwo();

    #ifndef MAGNUM_TARGET_WEBGL
    void benchmark();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        Containers::String _testDir;
};

using namespace Math::Literals;

const struct {
    const char* name;
    Vector2i size;
    Vector2i offset;
    bool flipX, flipY;
} RunData[]{
    {"", {64, 64}, {}, false, false},
    {"flipped on X", {64, 64}, {}, true, false},
    {"flipped on Y", {64, 64}, {}, false, true},
    {"with offset", {128, 96}, {64, 32}, false, false},
};

DistanceFieldGLTest::DistanceFieldGLTest() {
    addTests({&DistanceFieldGLTest::construct,
              &DistanceFieldGLTest::constructCopy,
              &DistanceFieldGLTest::constructMove});

    addInstancedTests({&DistanceFieldGLTest::runTexture,
                       &DistanceFieldGLTest::runFramebuffer},
        Containers::arraySize(RunData));

    addTests({&DistanceFieldGLTest::formatNotDrawable,
              &DistanceFieldGLTest::sizeRatioNotMultipleOfTwo});

    #ifndef MAGNUM_TARGET_WEBGL
    addBenchmarks({&DistanceFieldGLTest::benchmark}, 10, BenchmarkType::GpuTime);
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
        _testDir = Utility::Path::join(Utility::Path::path(*Utility::Path::executableLocation()), "DistanceFieldGLTestFiles");
    } else
    #endif
    {
        _testDir = Utility::Path::join(TEXTURETOOLS_TEST_DIR, "DistanceFieldGLTestFiles");
    }
}

void DistanceFieldGLTest::construct() {
    DistanceFieldGL distanceField{32};
    CORRADE_COMPARE(distanceField.radius(), 32);
}

void DistanceFieldGLTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DistanceFieldGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DistanceFieldGL>{});
}

void DistanceFieldGLTest::constructMove() {
    DistanceFieldGL a{16};

    DistanceFieldGL b = Utility::move(a);
    CORRADE_COMPARE(b.radius(), 16);

    DistanceFieldGL c{8};
    c = Utility::move(b);
    CORRADE_COMPARE(c.radius(), 16);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DistanceFieldGL>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DistanceFieldGL>::value);
}

void DistanceFieldGLTest::runTexture() {
    auto&& data = RunData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _manager.loadAndInstantiate("TgaImporter")))
        CORRADE_SKIP("TgaImporter plugin not found.");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "input.tga")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    Containers::Optional<Trade::ImageData2D> inputImage = importer->image2D(0);
    CORRADE_VERIFY(inputImage);
    CORRADE_COMPARE(inputImage->format(), PixelFormat::R8Unorm);

    /* Flip the input if desired */
    if(data.flipX)
        Utility::flipInPlace<1>(inputImage->mutablePixels());
    if(data.flipY)
        Utility::flipInPlace<0>(inputImage->mutablePixels());

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
    const GL::TextureFormat outputTextureFormat = GL::TextureFormat::R8;
    const GL::PixelFormat outputPixelFormat = GL::PixelFormat::Red;
    #elif !defined(MAGNUM_TARGET_WEBGL)
    GL::TextureFormat outputTextureFormat;
    GL::PixelFormat outputPixelFormat;
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        outputTextureFormat = GL::TextureFormat::R8;
        outputPixelFormat = GL::PixelFormat::Red;
    } else {
        outputTextureFormat = GL::TextureFormat::RGBA;
        outputPixelFormat = GL::PixelFormat::RGBA;
    }
    #else
    const GL::TextureFormat outputTextureFormat = GL::TextureFormat::RGBA;
    const GL::PixelFormat outputPixelFormat = GL::PixelFormat::RGBA;
    #endif
    const GL::PixelType outputPixelType = GL::PixelType::UnsignedByte;

    GL::Texture2D output;
    output.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, outputTextureFormat, data.size);

    /* Fill the texture with some data to verify they don't affect the output
       and aren't accidentally overwritten when running on just a
       subrectangle */
    output.setSubImage(0, {}, ImageView2D{outputPixelFormat, outputPixelType, data.size,
    Containers::Array<char>{DirectInit, std::size_t(data.size.product()*GL::pixelFormatSize(outputPixelFormat, outputPixelType)), '\x66'}});

    DistanceFieldGL distanceField{32};
    CORRADE_COMPARE(distanceField.radius(), 32);

    MAGNUM_VERIFY_NO_GL_ERROR();

    distanceField(input, output, Range2Di::fromSize(data.offset, Vector2i{64})
        #ifdef MAGNUM_TARGET_GLES
        , inputImage->size()
        #endif
        );

    Containers::Optional<Image2D> actualOutputImage;
    #ifndef MAGNUM_TARGET_GLES2
    actualOutputImage = Image2D{PixelFormat::R8Unorm};
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        actualOutputImage = Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte};
    else
        actualOutputImage = Image2D{PixelFormat::RGBA8Unorm};
    #else
    actualOutputImage = Image2D{PixelFormat::RGBA8Unorm};
    #endif

    /* Verify that the other data weren't overwritten if processing just a
       subrange -- it should still have the original data kept */
    if(data.offset.product()) {
        DebugTools::textureSubImage(output, 0, Range2Di::fromSize({}, Vector2i{1}), *actualOutputImage);

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(actualOutputImage->data()[0], '\x66');
    }

    DebugTools::textureSubImage(output, 0, Range2Di::fromSize(data.offset, Vector2i{64}), *actualOutputImage);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Flip the output back */
    Containers::StridedArrayView3D<char> pixels3 = actualOutputImage->pixels();
    if(data.flipX)
        Utility::flipInPlace<1>(pixels3);
    if(data.flipY)
        Utility::flipInPlace<0>(pixels3);

    /* Use just the first channel if the format is RGBA */
    Containers::StridedArrayView2D<UnsignedByte> pixels;
    if(actualOutputImage->format() == PixelFormat::RGBA8Unorm)
        pixels = Containers::arrayCast<2, Color4ub>(pixels3).slice(&Color4ub::r);
    else
        pixels = Containers::arrayCast<2, UnsignedByte>(pixels3);

    CORRADE_COMPARE_WITH(
        pixels,
        Utility::Path::join(_testDir, "output.tga"),
        /* Some mobile GPUs have slight (off-by-one) rounding errors compared
           to the ground truth, but it's just a very small amount of pixels
           (20-50 out of the total 4k pixels, iOS/WebGL has slightly more).
           That's okay. It's also possible that the ground truth itself has
           rounding errors ;) */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.178f}));
}

void DistanceFieldGLTest::runFramebuffer() {
    auto&& data = RunData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like runTexture(), except that the it's using the framebuffer overload.
       It should give the same results even without having to explicitly set
       anything on the framebuffer. */

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _manager.loadAndInstantiate("TgaImporter")))
        CORRADE_SKIP("TgaImporter plugin not found.");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "input.tga")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    Containers::Optional<Trade::ImageData2D> inputImage = importer->image2D(0);
    CORRADE_VERIFY(inputImage);
    CORRADE_COMPARE(inputImage->format(), PixelFormat::R8Unorm);

    /* Flip the input if desired */
    if(data.flipX)
        Utility::flipInPlace<1>(inputImage->mutablePixels());
    if(data.flipY)
        Utility::flipInPlace<0>(inputImage->mutablePixels());

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
        outputFormat = GL::TextureFormat::RGBA;
    #else
    const GL::TextureFormat outputFormat = GL::TextureFormat::RGBA;
    #endif

    GL::Texture2D outputTexture;
    outputTexture.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, outputFormat, data.size);

    /* Deliberately making the viewport the whole framebuffer -- the tool
       should adjust it as appropriate and then revert back */
    GL::Framebuffer output{{{}, data.size}};
    output.attachTexture(GL::Framebuffer::ColorAttachment(0), outputTexture, 0);

    /* Clear the framebuffer to some data to verify it's not getting cleared
       again inside, stomping on existing data. Use the statless clear command
       if possible to avoid the clear color getting accidentally reused for a
       clear inside, making the test wrongly pass */
    #ifndef MAGNUM_TARGET_GLES2
    output.clearColor(0, 0x667788_rgbf);
    #else
    GL::Renderer::setClearColor(0x667788_rgbf);
    output.clear(GL::FramebufferClear::Color);
    /* Same as in GL::Renderer::initializeContextBasedFunctionality() */
    GL::Renderer::setClearColor(0x1f1f1f_rgbf);
    #endif

    DistanceFieldGL distanceField{32};
    CORRADE_COMPARE(distanceField.radius(), 32);

    MAGNUM_VERIFY_NO_GL_ERROR();

    distanceField(input, output, Range2Di::fromSize(data.offset, Vector2i{64})
        #ifdef MAGNUM_TARGET_GLES
        , inputImage->size()
        #endif
        );

    /* The viewport should stay as it was before */
    CORRADE_COMPARE(output.viewport(), (Range2Di{{}, data.size}));

    Containers::Optional<Image2D> actualOutputImage;
    #ifndef MAGNUM_TARGET_GLES2
    actualOutputImage = Image2D{PixelFormat::R8Unorm};
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        actualOutputImage = Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte};
    else
        actualOutputImage = Image2D{PixelFormat::RGBA8Unorm};
    #else
    actualOutputImage = Image2D{PixelFormat::RGBA8Unorm};
    #endif

    /* Verify that the other data weren't overwritten if processing just a
       subrange -- it should still have the original data kept */
    if(data.offset.product()) {
        DebugTools::textureSubImage(outputTexture, 0, Range2Di::fromSize({}, Vector2i{1}), *actualOutputImage);

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(actualOutputImage->data()[0], '\x66');
    }

    DebugTools::textureSubImage(outputTexture, 0, Range2Di::fromSize(data.offset, Vector2i{64}), *actualOutputImage);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Flip the output back */
    Containers::StridedArrayView3D<char> pixels3 = actualOutputImage->pixels();
    if(data.flipX)
        Utility::flipInPlace<1>(pixels3);
    if(data.flipY)
        Utility::flipInPlace<0>(pixels3);

    /* Use just the first channel if the format is RGBA */
    Containers::StridedArrayView2D<UnsignedByte> pixels;
    if(actualOutputImage->format() == PixelFormat::RGBA8Unorm)
        pixels = Containers::arrayCast<2, Color4ub>(pixels3).slice(&Color4ub::r);
    else
        pixels = Containers::arrayCast<2, UnsignedByte>(pixels3);

    CORRADE_COMPARE_WITH(
        pixels,
        Utility::Path::join(_testDir, "output.tga"),
        /* Some mobile GPUs have slight (off-by-one) rounding errors compared
           to the ground truth, but it's just a very small amount of pixels
           (20-50 out of the total 4k pixels, iOS/WebGL has slightly more).
           That's okay. It's also possible that the ground truth itself has
           rounding errors ;) */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.178f}));
}

void DistanceFieldGLTest::formatNotDrawable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_shared_exponent>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_shared_exponent::string() << "not supported, can't test");
    #endif

    /* Not using GL::textureFormat(PixelFormat::R8Unorm) as that could pass
       an unsized format to glTexStorage() on ES2, causing a GL error */
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
        .setStorage(1, inputFormat, {64, 64});

    /* Similarly in this case */
    GL::Texture2D output;
    #ifdef MAGNUM_TARGET_GLES2
    output.setImage(0, GL::TextureFormat::Luminance, ImageView2D{GL::PixelFormat::Luminance, GL::PixelType::UnsignedByte, Vector2i{4}});
    #else
    output.setImage(0, GL::TextureFormat::RGB9E5, ImageView2D{GL::PixelFormat::RGB, GL::PixelType::UnsignedInt5999Rev, Vector2i{4}});
    #endif

    DistanceFieldGL distanceField{4};

    Containers::String out;
    Error redirectError{&out};
    distanceField(input, output, {{}, Vector2i{4}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{64}
        #endif
        );
    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES
    /* NV drivers print the same error on both desktop and ES */
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::NVidia)
        CORRADE_COMPARE(out, "TextureTools::DistanceFieldGL: output texture format not framebuffer-drawable: GL::Framebuffer::Status::IncompleteAttachment\n");
    else
        CORRADE_COMPARE(out, "TextureTools::DistanceFieldGL: output texture format not framebuffer-drawable: GL::Framebuffer::Status::Unsupported\n");
    #else
    CORRADE_COMPARE(out, "TextureTools::DistanceFieldGL: output texture format not framebuffer-drawable: GL::Framebuffer::Status::IncompleteAttachment\n");
    #endif
}

void DistanceFieldGLTest::sizeRatioNotMultipleOfTwo() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Not using GL::textureFormat(PixelFormat::R8Unorm) as that could pass
       an unsized format to glTexStorage() on ES2, causing a GL error */
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
    input.setStorage(1, inputFormat, {23*14, 23*14});

    /* Similarly in this case */
    GL::Texture2D output;
    #ifdef MAGNUM_TARGET_GLES2
    output.setImage(0, GL::TextureFormat::RGBA, Image2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, {23, 23}, Containers::Array<char>{NoInit, 23*23*4}});
    #else
    output.setStorage(1, GL::textureFormat(PixelFormat::RGBA8Unorm), {23, 23});
    #endif

    DistanceFieldGL distanceField{4};

    /* This should be fine */
    distanceField(input, output, {{}, Vector2i{23}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{23*14}
        #endif
        );

    Containers::String out;
    Error redirectError{&out};
    distanceField(input, output, {{}, Vector2i{23*2}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{23*14}
        #endif
        );
    /* Verify also just one axis wrong */
    distanceField(input, output, {{}, {23*2, 23}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{23*14}
        #endif
        );
    distanceField(input, output, {{}, {23, 23*2}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{23*14}
        #endif
        );
    /* Almost correct except that it's not an integer multiply */
    distanceField(input, output, {{}, {22, 23}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{23*14}
        #endif
        );
    distanceField(input, output, {{}, {23, 22}}
        #ifdef MAGNUM_TARGET_GLES
        , Vector2i{23*14}
        #endif
        );
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(out,
        "TextureTools::DistanceFieldGL: expected input and output size ratio to be a multiple of 2, got {322, 322} and {46, 46}\n"
        "TextureTools::DistanceFieldGL: expected input and output size ratio to be a multiple of 2, got {322, 322} and {46, 23}\n"
        "TextureTools::DistanceFieldGL: expected input and output size ratio to be a multiple of 2, got {322, 322} and {23, 46}\n"
        "TextureTools::DistanceFieldGL: expected input and output size ratio to be a multiple of 2, got {322, 322} and {22, 23}\n"
        "TextureTools::DistanceFieldGL: expected input and output size ratio to be a multiple of 2, got {322, 322} and {23, 22}\n");
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
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::DistanceFieldGLTest)
