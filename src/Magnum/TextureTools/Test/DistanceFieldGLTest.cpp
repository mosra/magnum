/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
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

#include "configure.h"

namespace Magnum { namespace TextureTools { namespace Test {

struct DistanceFieldGLTest: GL::OpenGLTester {
    explicit DistanceFieldGLTest();

    void test();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager;
};

DistanceFieldGLTest::DistanceFieldGLTest() {
    addTests({&DistanceFieldGLTest::test});

    /* Load the plugin directly from the build tree. Otherwise it's either
       static and already loaded or not present in the build tree */
    #if defined(ANYIMAGEIMPORTER_PLUGIN_FILENAME) && defined(TGAIMPORTER_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void DistanceFieldGLTest::test() {
    std::unique_ptr<Trade::AbstractImporter> importer;
    if(!(importer = _manager.loadAndInstantiate("TgaImporter")))
        CORRADE_SKIP("TgaImporter plugin not found.");

    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(DISTANCEFIELDGLTEST_FILES_DIR, "input.tga")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    Containers::Optional<Trade::ImageData2D> inputImage = importer->image2D(0);
    CORRADE_VERIFY(inputImage);
    CORRADE_COMPARE(inputImage->format(), PixelFormat::R8Unorm);

    #ifndef MAGNUM_TARGET_GLES2
    const GL::TextureFormat inputFormat = GL::TextureFormat::R8;
    #elif !defined(MAGNUM_TARGET_WEBGL)
    GL::TextureFormat inputFormat;
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        inputFormat = GL::TextureFormat::R8;
    else
        inputFormat = GL::TextureFormat::Luminance; /** @todo Luminance8 */
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
    const GL::TextureFormat outputFormat = GL::TextureFormat::Luminance;
    #endif

    /** @todo luminance is usually not renderable, but with RGBA i would need
        some TextureTools::swizzle() to get rid of the extra channels for an
        image compare ... so at the moment, the test just blows up on WebGL 1 */

    GL::Texture2D output;
    output.setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, outputFormat, Vector2i{64});

    TextureTools::distanceField(input, output, {{}, Vector2i{64}}, 32
        #ifdef MAGNUM_TARGET_GLES
        , inputImage->size()
        #endif
        );

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Optional<Image2D> actualOutputImage;
    #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    actualOutputImage = Image2D{PixelFormat::R8Unorm};
    #else
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        actualOutputImage = Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte};
    else
        actualOutputImage = Image2D{PixelFormat::R8Unorm};
    #endif

    DebugTools::textureSubImage(output, 0, {{}, Vector2i{64}}, *actualOutputImage);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(_manager.loadState("AnyImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin not found.");

    /* In some cases actualOutputImage might have GL-specific format,
       reinterpret as R8Unorm for the comparison to work */
    CORRADE_COMPARE_WITH((ImageView2D{actualOutputImage->storage(), PixelFormat::R8Unorm, actualOutputImage->size(), actualOutputImage->data()}), Utility::Directory::join(DISTANCEFIELDGLTEST_FILES_DIR, "output.tga"),
        DebugTools::CompareImageToFile{_manager});
}

}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::DistanceFieldGLTest)
