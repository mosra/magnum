/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Shaders/Vector.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VectorGLTest: GL::OpenGLTester {
    explicit VectorGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructMove();

    template<UnsignedInt dimensions> void setTextureMatrixNotEnabled();

    void renderSetup();
    void renderTeardown();

    void renderDefaults2D();
    void renderDefaults3D();
    void render2D();
    void render3D();

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
    -   Mesa llvmpipe
    -   SwiftShader ES2/ES3
    -   ARM Mali (Huawei P10) ES2/ES3
    -   WebGL 1 / 2 (on Mesa Intel)
    -   iPhone 6 w/ iOS 12.4
*/

using namespace Math::Literals;

constexpr struct {
    const char* name;
    Vector2D::Flags flags;
} ConstructData[]{
    {"", {}},
    {"texture transformation", Vector2D::Flag::TextureTransformation}
};

const struct {
    const char* name;
    Vector2D::Flags flags;
    Matrix3 textureTransformation;
    Color4 backgroundColor, color;
    const char* file2D;
    const char* file3D;
    bool flip;
} RenderData[] {
    {"texture transformation", Vector2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        0x00000000_rgbaf, 0xffffff_rgbf,
        "defaults.tga", "defaults.tga", true},
    {"", {}, {}, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false}
};

VectorGLTest::VectorGLTest() {
    addInstancedTests<VectorGLTest>({
        &VectorGLTest::construct<2>,
        &VectorGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    addTests<VectorGLTest>({
        &VectorGLTest::constructMove<2>,
        &VectorGLTest::constructMove<3>,

        &VectorGLTest::setTextureMatrixNotEnabled<2>,
        &VectorGLTest::setTextureMatrixNotEnabled<3>});

    addTests({&VectorGLTest::renderDefaults2D,
              &VectorGLTest::renderDefaults3D},
        &VectorGLTest::renderSetup,
        &VectorGLTest::renderTeardown);

    addInstancedTests({&VectorGLTest::render2D,
                       &VectorGLTest::render3D},
        Containers::arraySize(RenderData),
        &VectorGLTest::renderSetup,
        &VectorGLTest::renderTeardown);

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
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

template<UnsignedInt dimensions> void VectorGLTest::construct() {
    setTestCaseTemplateName(std::to_string(dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Vector<dimensions> shader{data.flags};
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

template<UnsignedInt dimensions> void VectorGLTest::constructMove() {
    setTestCaseTemplateName(std::to_string(dimensions));

    Vector<dimensions> a{Vector<dimensions>::Flag::TextureTransformation};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Vector<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), Vector<dimensions>::Flag::TextureTransformation);
    CORRADE_VERIFY(!a.id());

    Vector<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), Vector<dimensions>::Flag::TextureTransformation);
    CORRADE_VERIFY(!b.id());
}

template<UnsignedInt dimensions> void VectorGLTest::setTextureMatrixNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Vector<dimensions> shader;
    shader.setTextureMatrix({});

    CORRADE_COMPARE(out.str(),
        "Shaders::Vector::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

constexpr Vector2i RenderSize{80, 80};

void VectorGLTest::renderSetup() {
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

void VectorGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

constexpr GL::TextureFormat TextureFormatR =
    #ifndef MAGNUM_TARGET_GLES2
    GL::TextureFormat::R8
    #else
    GL::TextureFormat::Luminance
    #endif
    ;

void VectorGLTest::renderDefaults2D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh square = MeshTools::compile(Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge);

    #ifdef MAGNUM_TARGET_GLES2
    /* Don't want to bother with the fiasco of single-channel formats and
       texture storage extensions on ES2 */
    texture.setImage(0, TextureFormatR, *image);
    #else
    texture.setStorage(1, TextureFormatR, image->size())
        .setSubImage(0, {}, *image);
    #endif

    Vector2D{}
        .bindVectorTexture(texture)
        .draw(square);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has off-by-one differences on edges, ARM Mali a bit more of
       them, llvmpipe is off-by-two */
    const Float maxThreshold = 2.0f, meanThreshold = 0.071f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 17.0f, meanThreshold = 0.359f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "VectorTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void VectorGLTest::renderDefaults3D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge);

    #ifdef MAGNUM_TARGET_GLES2
    /* Don't want to bother with the fiasco of single-channel formats and
       texture storage extensions on ES2 */
    texture.setImage(0, TextureFormatR, *image);
    #else
    texture.setStorage(1, TextureFormatR, image->size())
        .setSubImage(0, {}, *image);
    #endif

    Vector3D{}
        .bindVectorTexture(texture)
        .draw(plane);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has off-by-one differences on edges, ARM Mali a bit more of
       them; llvmpipe is off-by-two */
    const Float maxThreshold = 2.0f, meanThreshold = 0.071f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 17.0f, meanThreshold = 0.359f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "VectorTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void VectorGLTest::render2D() {
    auto&& data = RenderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh square = MeshTools::compile(Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge);

    #ifdef MAGNUM_TARGET_GLES2
    /* Don't want to bother with the fiasco of single-channel formats and
       texture storage extensions on ES2 */
    texture.setImage(0, TextureFormatR, *image);
    #else
    texture.setStorage(1, TextureFormatR, image->size())
        .setSubImage(0, {}, *image);
    #endif

    Vector2D shader{data.flags};
    shader.setBackgroundColor(data.backgroundColor)
        .setColor(data.color)
        .bindVectorTexture(texture);

    if(data.textureTransformation != Matrix3{})
        shader.setTextureMatrix(data.textureTransformation);
    else shader.setTransformationProjectionMatrix(
        Matrix3::projection({2.1f, 2.1f})*
        Matrix3::rotation(5.0_degf));

    shader.draw(square);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D rendered = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
    /* Dropping the alpha channel, as it's always 1.0 */
    Containers::StridedArrayView2D<Color3ub> pixels =
        Containers::arrayCast<Color3ub>(rendered.pixels<Color4ub>());
    if(data.flip) pixels = pixels.flipped<0>().flipped<1>();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on four pixels */
    const Float maxThreshold = 170.0f, meanThreshold = 0.146f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.962f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Directory::join({_testDir, "VectorTestFiles", data.file2D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void VectorGLTest::render3D() {
    auto&& data = RenderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge);

    #ifdef MAGNUM_TARGET_GLES2
    /* Don't want to bother with the fiasco of single-channel formats and
       texture storage extensions on ES2 */
    texture.setImage(0, TextureFormatR, *image);
    #else
    texture.setStorage(1, TextureFormatR, image->size())
        .setSubImage(0, {}, *image);
    #endif

    Vector3D shader{data.flags};
    shader.setBackgroundColor(data.backgroundColor)
        .setColor(data.color)
        .bindVectorTexture(texture);

    if(data.textureTransformation != Matrix3{})
        shader.setTextureMatrix(data.textureTransformation);
    else shader.setTransformationProjectionMatrix(
        Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
        Matrix4::translation(Vector3::zAxis(-2.15f))*
        Matrix4::rotationY(-15.0_degf)*
        Matrix4::rotationZ(15.0_degf));

    shader.draw(plane);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D rendered = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
    /* Dropping the alpha channel, as it's always 1.0 */
    Containers::StridedArrayView2D<Color3ub> pixels =
        Containers::arrayCast<Color3ub>(rendered.pixels<Color4ub>());
    if(data.flip) pixels = pixels.flipped<0>().flipped<1>();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on four pixels */
    const Float maxThreshold = 170.0f, meanThreshold = 0.171f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.660f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Directory::join({_testDir, "VectorTestFiles", data.file3D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorGLTest)
