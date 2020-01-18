/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct FlatGLTest: GL::OpenGLTester {
    explicit FlatGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructMove();

    template<UnsignedInt dimensions> void bindTextureNotEnabled();
    template<UnsignedInt dimensions> void setAlphaMaskNotEnabled();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setObjectIdNotEnabled();
    #endif

    void renderSetup();
    void renderTeardown();

    void renderDefaults2D();
    void renderDefaults3D();
    void renderColored2D();
    void renderColored3D();
    void renderSinglePixelTextured2D();
    void renderSinglePixelTextured3D();
    void renderTextured2D();
    void renderTextured3D();

    template<class T> void renderVertexColor2D();
    template<class T> void renderVertexColor3D();

    void renderAlphaSetup();
    void renderAlphaTeardown();

    void renderAlpha2D();
    void renderAlpha3D();

    #ifndef MAGNUM_TARGET_GLES2
    void renderObjectIdSetup();
    void renderObjectIdTeardown();

    void renderObjectId2D();
    void renderObjectId3D();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        std::string _testDir;

        GL::Renderbuffer _color{NoCreate};
        #ifndef MAGNUM_TARGET_GLES2
        GL::Renderbuffer _objectId{NoCreate};
        #endif
        GL::Framebuffer _framebuffer{NoCreate};
};

/*
    Rendering tests done on:

    -   Mesa Intel
    -   Mesa AMD
    -   SwiftShader ES2/ES3
    -   ARM Mali (Huawei P10) ES2/ES3
    -   WebGL 1 / 2 (on Mesa Intel)
    -   NVidia Windows
    -   Intel Windows
    -   AMD on macOS
    -   iPhone 6 w/ iOS 12.4
*/

using namespace Math::Literals;

constexpr struct {
    const char* name;
    Flat2D::Flags flags;
} ConstructData[]{
    {"", {}},
    {"textured", Flat2D::Flag::Textured},
    {"alpha mask", Flat2D::Flag::AlphaMask},
    {"alpha mask + textured", Flat2D::Flag::AlphaMask|Flat2D::Flag::Textured},
    {"vertex colors", Flat2D::Flag::VertexColor},
    {"vertex colors + textured", Flat2D::Flag::VertexColor|Flat2D::Flag::Textured},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", Flat2D::Flag::ObjectId},
    {"object ID + alpha mask + textured", Flat2D::Flag::ObjectId|Flat2D::Flag::AlphaMask|Flat2D::Flag::Textured}
    #endif
};

const struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    bool blending;
    Flat2D::Flags flags;
    Float threshold;
} RenderAlphaData[] {
    /* All those deliberately have a non-white diffuse in order to match the
       expected data from textured() */
    {"none", "FlatTestFiles/textured2D.tga", "FlatTestFiles/textured3D.tga", false,
        Flat2D::Flag::Textured, 0.0f},
    {"blending", "FlatTestFiles/textured2D-alpha.tga", "FlatTestFiles/textured3D-alpha.tga", true,
        Flat2D::Flag::Textured, 0.0f},
    {"masking 0.0", "FlatTestFiles/textured2D.tga", "FlatTestFiles/textured3D.tga", false,
        Flat2D::Flag::Textured, 0.0f},
    {"masking 0.5", "FlatTestFiles/textured2D-alpha-mask0.5.tga", "FlatTestFiles/textured3D-alpha-mask0.5.tga", false,
        Flat2D::Flag::Textured|Flat2D::Flag::AlphaMask, 0.5f},
    {"masking 1.0", "TestFiles/alpha-mask1.0.tga", "TestFiles/alpha-mask1.0.tga", false,
        Flat2D::Flag::Textured|Flat2D::Flag::AlphaMask, 1.0f}
};

FlatGLTest::FlatGLTest() {
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::construct<2>,
        &FlatGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    addTests<FlatGLTest>({
        &FlatGLTest::constructMove<2>,
        &FlatGLTest::constructMove<3>,

        &FlatGLTest::bindTextureNotEnabled<2>,
        &FlatGLTest::bindTextureNotEnabled<3>,
        &FlatGLTest::setAlphaMaskNotEnabled<2>,
        &FlatGLTest::setAlphaMaskNotEnabled<3>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::setObjectIdNotEnabled<2>,
        &FlatGLTest::setObjectIdNotEnabled<3>
        #endif
        });

    addTests({&FlatGLTest::renderDefaults2D,
              &FlatGLTest::renderDefaults3D,
              &FlatGLTest::renderColored2D,
              &FlatGLTest::renderColored3D,
              &FlatGLTest::renderSinglePixelTextured2D,
              &FlatGLTest::renderSinglePixelTextured3D,
              &FlatGLTest::renderTextured2D,
              &FlatGLTest::renderTextured3D,
              &FlatGLTest::renderVertexColor2D<Color3>,
              &FlatGLTest::renderVertexColor2D<Color4>,
              &FlatGLTest::renderVertexColor3D<Color3>,
              &FlatGLTest::renderVertexColor3D<Color4>},
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    addInstancedTests({&FlatGLTest::renderAlpha2D,
                       &FlatGLTest::renderAlpha3D},
        Containers::arraySize(RenderAlphaData),
        &FlatGLTest::renderAlphaSetup,
        &FlatGLTest::renderAlphaTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    addTests({&FlatGLTest::renderObjectId2D,
              &FlatGLTest::renderObjectId3D},
        &FlatGLTest::renderObjectIdSetup,
        &FlatGLTest::renderObjectIdTeardown);
    #endif

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    #ifdef CORRADE_TARGET_APPLE
    if(Utility::Directory::isSandboxed()
        #if defined(CORRADE_TARGET_IOS) && defined(CORRADE_TESTSUITE_TARGET_XCTEST)
        /** @todo Fix this once I persuade CMake to run XCTest tests properly */
        && std::getenv("SIMULATOR_UDID")
        #endif
    ) {
        _testDir = Utility::Directory::path(Utility::Directory::executableLocation());
    } else
    #endif
    {
        _testDir = SHADERS_TEST_DIR;
    }
}

template<UnsignedInt dimensions> void FlatGLTest::construct() {
    setTestCaseTemplateName(std::to_string(dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Flat<dimensions> shader{data.flags};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_VERIFY(shader.id());
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void FlatGLTest::constructMove() {
    setTestCaseTemplateName(std::to_string(dimensions));

    Flat<dimensions> a{Flat<dimensions>::Flag::Textured};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Flat<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), Flat<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!a.id());

    Flat<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), Flat<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!b.id());
}

template<UnsignedInt dimensions> void FlatGLTest::bindTextureNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    Flat<dimensions> shader;
    shader.bindTexture(texture);

    CORRADE_COMPARE(out.str(), "Shaders::Flat::bindTexture(): the shader was not created with texturing enabled\n");
}

template<UnsignedInt dimensions> void FlatGLTest::setAlphaMaskNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    std::ostringstream out;
    Error redirectError{&out};

    Flat<dimensions> shader;
    shader.setAlphaMask(0.75f);

    CORRADE_COMPARE(out.str(),
        "Shaders::Flat::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setObjectIdNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    std::ostringstream out;
    Error redirectError{&out};

    Flat<dimensions> shader;
    shader.setObjectId(33376);

    CORRADE_COMPARE(out.str(),
        "Shaders::Flat::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

constexpr Vector2i RenderSize{80, 80};

void FlatGLTest::renderSetup() {
    /* Pick a color that's directly representable on RGBA4 as well to reduce
       artifacts */
    GL::Renderer::setClearColor(0x111111_rgbf);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    _color = GL::Renderbuffer{};
    _color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        RenderSize);
    _framebuffer = GL::Framebuffer{{{}, RenderSize}};
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .clear(GL::FramebufferClear::Color)
        .bind();
}

void FlatGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

void FlatGLTest::renderDefaults2D() {
    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    Flat2D shader;
    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/defaults.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

void FlatGLTest::renderDefaults3D() {
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    Flat3D shader;
    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/defaults.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

void FlatGLTest::renderColored2D() {
    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    Flat2D shader;
    shader.setColor(0x9999ff_rgbf)
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}));

    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    const Float maxThreshold = 0.0f, meanThreshold = 0.0f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 11.34f, meanThreshold = 0.51f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void FlatGLTest::renderColored3D() {
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    Flat3D shader;
    shader.setColor(0x9999ff_rgbf)
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf));

    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 170.0f, meanThreshold = 0.133f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.456f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

constexpr GL::TextureFormat TextureFormatRGB =
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    GL::TextureFormat::RGB8
    #else
    GL::TextureFormat::RGB
    #endif
    ;
constexpr GL::TextureFormat TextureFormatRGBA =
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    GL::TextureFormat::RGBA8
    #else
    GL::TextureFormat::RGBA
    #endif
    ;

void FlatGLTest::renderSinglePixelTextured2D() {
    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::CircleTextureCoords::Generate));

    const Color4ub diffuseData[]{ 0x9999ff_rgb };
    ImageView2D diffuseImage{PixelFormat::RGBA8Unorm, Vector2i{1}, diffuseData};
    GL::Texture2D texture;
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, Vector2i{1})
        .setSubImage(0, {}, diffuseImage);

    Flat2D shader{Flat3D::Flag::Textured};
    shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .bindTexture(texture);
    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 170.0f, meanThreshold = 0.133f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 11.34f, meanThreshold = 0.51f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void FlatGLTest::renderSinglePixelTextured3D() {
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereTextureCoords::Generate));

    const Color4ub diffuseData[]{ 0x9999ff_rgb };
    ImageView2D diffuseImage{PixelFormat::RGBA8Unorm, Vector2i{1}, diffuseData};
    GL::Texture2D texture;
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, Vector2i{1})
        .setSubImage(0, {}, diffuseImage);

    Flat3D shader{Flat3D::Flag::Textured};
    shader.setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .bindTexture(texture);
    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 170.0f, meanThreshold = 0.133f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.456f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void FlatGLTest::renderTextured2D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::CircleTextureCoords::Generate));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    Flat2D shader{Flat2D::Flag::Textured};
    shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        /* Colorized. Case without a color (where it should be white) is tested
           in renderSinglePixelTextured() */
        .setColor(0x9999ff_rgbf)
        .bindTexture(texture);
    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has minor rounding errors, Apple A8 slightly more */
    const Float maxThreshold = 2.334f, meanThreshold = 0.023f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.667f, meanThreshold = 3.254f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/textured2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void FlatGLTest::renderTextured3D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereTextureCoords::Generate));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    Flat3D shader{Flat3D::Flag::Textured};
    shader.setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        /* Colorized. Case without a color (where it should be white) is tested
           in renderSinglePixelTextured() */
        .setColor(0x9999ff_rgbf)
        .bindTexture(texture);
    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 139.0f, meanThreshold = 0.087f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 139.0f, meanThreshold = 2.896f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/textured3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T> void FlatGLTest::renderVertexColor2D() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    Trade::MeshData circleData = Primitives::circle2DSolid(32,
        Primitives::CircleTextureCoords::Generate);

    /* Highlight a quarter */
    Containers::Array<T> colorData{Containers::DirectInit, circleData.vertexCount(), 0x999999_rgbf};
    for(std::size_t i = 8; i != 16; ++i)
        colorData[i + 1] = 0xffff99_rgbf*1.5f;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh circle = MeshTools::compile(circleData);
    circle.addVertexBuffer(colors, 0, GL::Attribute<Shaders::Flat2D::Color3::Location, T>{});

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    Flat2D shader{Flat2D::Flag::Textured|Flat2D::Flag::VertexColor};
    shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .setColor(0x9999ff_rgbf)
        .bindTexture(texture);
    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has minor rounding errors. ARM Mali / Apple A8 a bit more */
    const Float maxThreshold = 3.334f, meanThreshold = 0.064f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's worse */
    const Float maxThreshold = 15.334f, meanThreshold = 4.355f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/vertexColor2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T> void FlatGLTest::renderVertexColor3D() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereTextureCoords::Generate);

    /* Highlight the middle rings */
    Containers::Array<T> colorData{Containers::DirectInit, sphereData.vertexCount(), 0x999999_rgbf};
    for(std::size_t i = 6*33; i != 9*33; ++i)
        colorData[i + 1] = 0xffff99_rgbf*1.5f;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<Shaders::Flat3D::Color4::Location, T>{});

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    Flat3D shader{Flat3D::Flag::Textured|Flat3D::Flag::VertexColor};
    shader.setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .setColor(0x9999ff_rgbf)
        .bindTexture(texture);
    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has some minor differences on the edges, Apple A8 more */
    const Float maxThreshold = 76.67f, meanThreshold = 0.138f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's worse */
    const Float maxThreshold = 76.67f, meanThreshold = 3.908f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/vertexColor3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void FlatGLTest::renderAlphaSetup() {
    renderSetup();
    if(RenderAlphaData[testCaseInstanceId()].blending)
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add);
}

void FlatGLTest::renderAlphaTeardown() {
    if(RenderAlphaData[testCaseInstanceId()].blending)
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    renderTeardown();
}

void FlatGLTest::renderAlpha2D() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    Containers::Optional<Trade::ImageData2D> image;
    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join({_testDir, "TestFiles", "diffuse-alpha-texture.tga"})) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, image->size())
        .setSubImage(0, {}, *image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::CircleTextureCoords::Generate));

    Flat2D shader{data.flags};
    shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .setColor(0x9999ff_rgbf)
        .bindTexture(texture);

    if(data.flags & Flat3D::Flag::AlphaMask)
        shader.setAlphaMask(data.threshold);

    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* Minor differences between opaque and diffuse, not sure why */
    const Float maxThreshold = 24.34f, meanThreshold = 0.305f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 31.34f, meanThreshold = 3.945f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, data.expected2D),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void FlatGLTest::renderAlpha3D() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    Containers::Optional<Trade::ImageData2D> image;
    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join({_testDir, "TestFiles", "diffuse-alpha-texture.tga"})) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, image->size())
        .setSubImage(0, {}, *image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereTextureCoords::Generate));

    Flat3D shader{data.flags};
    shader.setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .setColor(0x9999ff_rgbf)
        .bindTexture(texture);

    if(data.flags & Flat3D::Flag::AlphaMask)
        shader.setAlphaMask(data.threshold);

    /* For proper Z order draw back faces first and then front faces */
    GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
    sphere.draw(shader);
    GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* Minor differences between opaque and diffuse, not sure why. SwiftShader
       has 5 different pixels on the edges. */
    const Float maxThreshold = 139.0f, meanThreshold = 0.280f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 139.0f, meanThreshold = 4.587f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
void FlatGLTest::renderObjectIdSetup() {
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    _color = GL::Renderbuffer{};
    _color.setStorage(GL::RenderbufferFormat::RGBA8, RenderSize);
    _objectId = GL::Renderbuffer{};
    _objectId.setStorage(GL::RenderbufferFormat::R32UI, RenderSize);
    _framebuffer = GL::Framebuffer{{{}, RenderSize}};
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
        .mapForDraw({
            {Flat3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {Flat3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        /* Pick a color that's directly representable on RGBA4 as well to
           reduce artifacts (well, and this needs to be consistent with other
           tests that *need* to run on WebGL 1) */
        .clearColor(0, 0x111111_rgbf)
        .clearColor(1, Vector4ui{27})
        .bind();
}

void FlatGLTest::renderObjectIdTeardown() {
    _color = GL::Renderbuffer{NoCreate};
    _objectId = GL::Renderbuffer{NoCreate};
    _framebuffer = GL::Framebuffer{NoCreate};
}

void FlatGLTest::renderObjectId2D() {
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    Flat2D shader{Flat3D::Flag::ObjectId};
    shader.setColor(0x9999ff_rgbf)
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .setObjectId(47523);

    circle.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    /* Color output should have no difference -- same as in colored2D() */
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    const Float maxThreshold = 0.0f, meanThreshold = 0.0f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 11.34f, meanThreshold = 0.51f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Outside of the object, cleared to 27 */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[10][10], 27);
    /* Inside of the object. Verify that it can hold 16 bits at least. */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[40][46], 47523);
}

void FlatGLTest::renderObjectId3D() {
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    Flat3D shader{Flat3D::Flag::ObjectId};
    shader.setColor(0x9999ff_rgbf)
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .setObjectId(48526);

    sphere.draw(shader);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    /* Color output should have no difference -- same as in colored3D() */
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 170.0f, meanThreshold = 0.133f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.456f;
    #endif
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Outside of the object, cleared to 27 */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[10][10], 27);
    /* Inside of the object. Verify that it can hold 16 bits at least. */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[40][46], 48526);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatGLTest)
