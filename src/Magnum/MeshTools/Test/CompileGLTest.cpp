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
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Shaders/VertexColor.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

#include "configure.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

enum class Flag {
    NonIndexed = 1 << 0,
    Normals = 1 << 1,
    GeneratedFlatNormals = 1 << 2,
    GeneratedSmoothNormals = 1 << 3,
    TextureCoordinates2D = 1 << 4,
    Colors = 1 << 5
};

typedef Containers::EnumSet<Flag> Flags;

#ifdef CORRADE_TARGET_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
CORRADE_ENUMSET_OPERATORS(Flags)
#ifdef CORRADE_TARGET_CLANG
#pragma clang diagnostic pop
#endif

struct CompileGLTest: GL::OpenGLTester {
    public:
        explicit CompileGLTest();

        template<class T> void twoDimensions();
        template<class T> void threeDimensions();
        void unknownAttribute();
        void generateNormalsNoPosition();
        void generateNormals2DPosition();

        void externalBuffers();
        void externalBuffersInvalid();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};

        Shaders::Flat2D _flat2D;
        Shaders::Flat2D _flatTextured2D{Shaders::Flat2D::Flag::Textured};
        Shaders::Flat3D _flat3D;
        Shaders::Flat3D _flatTextured3D{Shaders::Flat3D::Flag::Textured};
        Shaders::VertexColor2D _color2D;
        Shaders::VertexColor3D _color3D;
        Shaders::Phong _phong;

        GL::Renderbuffer _color;
        GL::Framebuffer _framebuffer{{{}, {32, 32}}};
        GL::Texture2D _texture;
};

constexpr struct {
    const char* name;
    Flags flags;
} Data2D[] {
    {"positions", {}},
    {"positions, nonindexed", Flag::NonIndexed},
    {"positions + colors", Flag::Colors},
    {"positions + texture coordinates", Flag::TextureCoordinates2D},
    {"positions + texture coordinates + colors", Flag::TextureCoordinates2D|Flag::Colors}
};

constexpr struct {
    const char* name;
    Flags flags;
} Data3D[] {
    {"positions", {}},
    {"positions, nonindexed", Flag::NonIndexed},
    {"positions + colors", Flag::Colors},
    {"positions + texcoords", Flag::TextureCoordinates2D},
    {"positions + texcoords + colors", Flag::TextureCoordinates2D|Flag::Colors},
    {"positions + normals", Flag::Normals},
    {"positions + normals + colors", Flag::Normals|Flag::Colors},
    {"positions + normals + texcoords", Flag::Normals|Flag::TextureCoordinates2D},
    {"positions + normals + texcoords + colors", Flag::Normals|Flag::TextureCoordinates2D|Flag::Colors},
    {"positions + gen flat normals", Flag::GeneratedFlatNormals},
    {"positions + gen both smooth and flat normals", Flag::GeneratedSmoothNormals|Flag::GeneratedFlatNormals},
    {"positions + normals, gen flat normals", Flag::Normals|Flag::GeneratedFlatNormals},
    {"positions + gen flat normals + colors", Flag::GeneratedFlatNormals|Flag::Colors},
    {"positions + gen flat normals + texcoords", Flag::GeneratedFlatNormals|Flag::TextureCoordinates2D},
    {"positions + gen flat normals + texcoords + colors", Flag::NonIndexed|Flag::GeneratedFlatNormals|Flag::TextureCoordinates2D|Flag::Colors},
    {"positions, nonindexed + gen flat normals", Flag::NonIndexed|Flag::GeneratedFlatNormals},
    {"positions, nonindexed + gen flat normals + colors", Flag::NonIndexed|Flag::GeneratedFlatNormals|Flag::Colors},
    {"positions, nonindexed + gen flat normals + texcoords", Flag::NonIndexed|Flag::GeneratedFlatNormals|Flag::TextureCoordinates2D},
    {"positions, nonindexed + gen flat normals + texcoords + colors", Flag::NonIndexed|Flag::GeneratedFlatNormals|Flag::TextureCoordinates2D|Flag::Colors},
    {"positions, gen smooth normals", Flag::GeneratedSmoothNormals},
    {"positions, gen smooth normals + colors", Flag::GeneratedSmoothNormals|Flag::Colors},
    {"positions, gen smooth normals + texcoords", Flag::GeneratedSmoothNormals|Flag::TextureCoordinates2D},
    {"positions, gen smooth normals + texcoords + colors", Flag::GeneratedSmoothNormals|Flag::TextureCoordinates2D|Flag::Colors},
    {"positions, nonindexed + gen smooth normals", Flag::NonIndexed|Flag::GeneratedSmoothNormals},
};

constexpr struct {
    const char* name;
    bool indexed, moveIndices, moveVertices;
} DataExternal[] {
    {"indexed", true, false, false},
    {"", false, false, false},
    {"move indices", true, true, false},
    {"move vertices", false, false, true},
    {"move both", true, true, true}
};

using namespace Math::Literals;

constexpr Color4ub ImageData[] {
    0xff000000_rgba, 0x80000000_rgba, 0x00008000_rgba, 0x0000ff00_rgba,
    0x80000000_rgba, 0xffffffff_rgba, 0xffffffff_rgba, 0x00008000_rgba,
    0x00800000_rgba, 0xffffffff_rgba, 0xffffffff_rgba, 0x00008000_rgba,
    0x00ff0000_rgba, 0x00800000_rgba, 0x00000080_rgba, 0x000000ff_rgba
};

CompileGLTest::CompileGLTest() {
    addInstancedTests<CompileGLTest>({
        &CompileGLTest::twoDimensions<Trade::MeshData>,
        &CompileGLTest::twoDimensions<Trade::MeshData2D>},
        Containers::arraySize(Data2D));

    addInstancedTests<CompileGLTest>({
        &CompileGLTest::threeDimensions<Trade::MeshData>,
        &CompileGLTest::threeDimensions<Trade::MeshData3D>},
        Containers::arraySize(Data3D));

    addTests({&CompileGLTest::unknownAttribute,
              &CompileGLTest::generateNormalsNoPosition,
              &CompileGLTest::generateNormals2DPosition});

    addInstancedTests({&CompileGLTest::externalBuffers},
        Containers::arraySize(DataExternal));

    addTests({&CompileGLTest::externalBuffersInvalid});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Set up the rendering */
    _color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        {32, 32});
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .bind();
    _texture
        .setMinificationFilter(SamplerFilter::Linear)
        .setMagnificationFilter(SamplerFilter::Linear)
        .setWrapping(SamplerWrapping::ClampToEdge)
        .setStorage(1,
            #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
            GL::TextureFormat::RGBA8,
            #else
            GL::TextureFormat::RGBA,
            #endif
            {4, 4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA8Unorm, {4, 4}, ImageData});
}

template<class T> struct MeshTypeName;
template<> struct MeshTypeName<Trade::MeshData> {
    static const char* name() { return "Trade::MeshData"; }
};
template<> struct MeshTypeName<Trade::MeshData2D> {
    static const char* name() { return "Trade::MeshData2D"; }
};
template<> struct MeshTypeName<Trade::MeshData3D> {
    static const char* name() { return "Trade::MeshData3D"; }
};

template<class T> void CompileGLTest::twoDimensions() {
    setTestCaseTemplateName(MeshTypeName<T>::name());
    auto&& data = Data2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /*
        6-----7-----8
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        3-----4-----5
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        0-----1-----2
    */
    const struct Vertex {
        Vector2 position;
        Vector2 textureCoordinates;
        Color3 color;
    } vertexData[]{
        {{-0.75f, -0.75f}, {0.0f, 0.0f}, 0x00ff00_rgbf},
        {{ 0.00f, -0.75f}, {0.5f, 0.0f}, 0x808000_rgbf},
        {{ 0.75f, -0.75f}, {1.0f, 0.0f}, 0xff0000_rgbf},

        {{-0.75f,  0.00f}, {0.0f, 0.5f}, 0x00ff80_rgbf},
        {{ 0.00f,  0.00f}, {0.5f, 0.5f}, 0x808080_rgbf},
        {{ 0.75f,  0.00f}, {1.0f, 0.5f}, 0xff0080_rgbf},

        {{-0.75f,  0.75f}, {0.0f, 1.0f}, 0x00ffff_rgbf},
        {{ 0.0f,   0.75f}, {0.5f, 1.0f}, 0x8080ff_rgbf},
        {{ 0.75f,  0.75f}, {1.0f, 1.0f}, 0xff00ff_rgbf}
    };

    Containers::Array<Trade::MeshAttributeData> attributeData;
    arrayAppend(attributeData, Trade::MeshAttributeData{
        Trade::MeshAttribute::Position,
        Containers::stridedArrayView(vertexData, &vertexData[0].position,
            Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::TextureCoordinates2D)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexData, &vertexData[0].textureCoordinates,
                Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::Colors)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::Color,
            Containers::stridedArrayView(vertexData, &vertexData[0].color,
                Containers::arraySize(vertexData), sizeof(Vertex))});

    const UnsignedInt indexData[]{
        0, 1, 4, 0, 4, 3,
        1, 2, 5, 1, 5, 4,
        3, 4, 7, 3, 7, 6,
        4, 5, 8, 4, 8, 7
    };

    Trade::MeshData meshData{MeshPrimitive::Triangles,
        {}, indexData, Trade::MeshIndexData{indexData},
        {}, vertexData, std::move(attributeData)};

    /* Duplicate everything if data is non-indexed */
    if(data.flags & Flag::NonIndexed) meshData = duplicate(meshData);

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh mesh = compile(T{std::move(meshData)});

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Check with the flat shader, it should always work */
    {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _flat2D.setColor(0xff3366_rgbf);
        mesh.draw(_flat2D);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "flat2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /* Check with the colored shader, if we have colors */
    if(data.flags & Flag::Colors) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        mesh.draw(_color2D);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "color2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /* Check with the textured shader, if we have texture coords */
    if(data.flags & Flag::TextureCoordinates2D) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _flatTextured2D.bindTexture(_texture);
        mesh.draw(_flatTextured2D);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "textured2D.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.75f, 0.0906f}));
    }
}

template<class T> void CompileGLTest::threeDimensions() {
    setTestCaseTemplateName(MeshTypeName<T>::name());
    auto&& data = Data3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /*
        6-----7-----8
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        3-----4-----5
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        0-----1-----2
    */
    const struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 textureCoordinates;
        Color4 color;
    } vertexData[]{
        {{-0.75f, -0.75f, -0.35f}, Vector3{-0.5f, -0.5f, 1.0f}.normalized(),
         {0.0f, 0.0f}, 0x00ff00_rgbf},
        {{ 0.00f, -0.75f, -0.25f}, Vector3{ 0.0f, -0.5f, 1.0f}.normalized(),
         {0.5f, 0.0f}, 0x808000_rgbf},
        {{ 0.75f, -0.75f, -0.35f}, Vector3{ 0.5f, -0.5f, 1.0f}.normalized(),
         {1.0f, 0.0f}, 0xff0000_rgbf},

        {{-0.75f,  0.00f, -0.25f}, Vector3{-0.5f,  0.0f, 1.0f}.normalized(),
         {0.0f, 0.5f}, 0x00ff80_rgbf},
        {{ 0.00f,  0.00f,  0.00f}, Vector3{ 0.0f,  0.0f, 1.0f}.normalized(),
         {0.5f, 0.5f}, 0x808080_rgbf},
        {{ 0.75f,  0.00f, -0.25f}, Vector3{ 0.5f,  0.0f, 1.0f}.normalized(),
         {1.0f, 0.5f}, 0xff0080_rgbf},

        {{-0.75f,  0.75f, -0.35f}, Vector3{-0.5f,  0.5f, 1.0f}.normalized(),
         {0.0f, 1.0f}, 0x00ffff_rgbf},
        {{ 0.0f,   0.75f, -0.25f}, Vector3{ 0.0f,  0.5f, 1.0f}.normalized(),
         {0.5f, 1.0f}, 0x8080ff_rgbf},
        {{ 0.75f,  0.75f, -0.35f}, Vector3{ 0.5f,  0.5f, 1.0f}.normalized(),
         {1.0f, 1.0f}, 0xff00ff_rgbf}
    };

    Containers::Array<Trade::MeshAttributeData> attributeData;
    arrayAppend(attributeData, Trade::MeshAttributeData{
        Trade::MeshAttribute::Position,
        Containers::stridedArrayView(vertexData, &vertexData[0].position,
            Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::Normals)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::Normal,
            Containers::stridedArrayView(vertexData, &vertexData[0].normal,
                Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::TextureCoordinates2D)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexData, &vertexData[0].textureCoordinates,
                Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::Colors)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::Color,
            Containers::stridedArrayView(vertexData, &vertexData[0].color,
                Containers::arraySize(vertexData), sizeof(Vertex))});

    const UnsignedByte indexData[]{
        0, 1, 4, 0, 4, 3,
        1, 2, 5, 1, 5, 4,
        3, 4, 7, 3, 7, 6,
        4, 5, 8, 4, 8, 7
    };

    Trade::MeshData meshData{MeshPrimitive::Triangles,
        {}, indexData, Trade::MeshIndexData{indexData},
        {}, vertexData, std::move(attributeData)};

    /* Duplicate everything if data is non-indexed */
    if(data.flags & Flag::NonIndexed) meshData = duplicate(meshData);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompileFlags flags;
    if(data.flags & Flag::GeneratedFlatNormals)
        flags |= CompileFlag::GenerateFlatNormals;
    if(data.flags & Flag::GeneratedSmoothNormals)
        flags |= CompileFlag::GenerateSmoothNormals;
    GL::Mesh mesh = compile(T{std::move(meshData)}, flags);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Matrix4 projection = Matrix4::perspectiveProjection(45.0_degf, 1.0f, 0.1f, 10.0f);
    Matrix4 transformation = Matrix4::translation(Vector3::zAxis(-2.0f));

    /* Check with the flat shader, it should always work */
    {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _flat3D
            .setTransformationProjectionMatrix(projection*transformation)
            .setColor(0x6633ff_rgbf);
        mesh.draw(_flat3D);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "flat3D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /* Check with the phong shader, if we have normals (but not flat generated) */
    if(data.flags & Flag::Normals && !(data.flags & Flag::GeneratedFlatNormals)) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _phong
            .setDiffuseColor(0x33ff66_rgbf)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setProjectionMatrix(projection);
        mesh.draw(_phong);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.5f, 0.0113f}));
    }

    /* Check generated flat / smooth normals with the phong shader. If smooth
       normals are requested but the mesh is not indexed, it should behave the
       same as flat normals. */
    if(data.flags & Flag::GeneratedFlatNormals || (data.flags & Flag::GeneratedSmoothNormals && data.flags & Flag::NonIndexed)) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _phong
            .setDiffuseColor(0x33ff66_rgbf)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setProjectionMatrix(projection);
        mesh.draw(_phong);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong-flat.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.25f, 0.0079f}));
    } else if(data.flags & Flag::GeneratedSmoothNormals) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _phong
            .setDiffuseColor(0x33ff66_rgbf)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setProjectionMatrix(projection);
        mesh.draw(_phong);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong-smooth.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.25f, 0.0059f}));
    }

    /* Check with the colored shader, if we have colors */
    if(data.flags & Flag::Colors) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _color3D
            .setTransformationProjectionMatrix(projection*transformation);
        mesh.draw(_color3D);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "color3D.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.5f, 0.0162f}));
    }

    /* Check with the textured shader, if we have texture coords */
    if(data.flags & Flag::TextureCoordinates2D) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _flatTextured3D
            .setTransformationProjectionMatrix(projection*transformation)
            .bindTexture(_texture);
        mesh.draw(_flatTextured3D);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "textured3D.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 1.0f, 0.0948f}));
    }
}

void CompileGLTest::unknownAttribute() {
    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {Trade::MeshAttributeData{Trade::meshAttributeCustom(115),
            VertexFormat::Short, nullptr}}};

    std::ostringstream out;
    Warning redirectError{&out};
    MeshTools::compile(data);
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): ignoring unknown attribute Trade::MeshAttribute::Custom(115)\n");
}

void CompileGLTest::generateNormalsNoPosition() {
    Trade::MeshData data{MeshPrimitive::Triangles, 1};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compile(data, CompileFlag::GenerateFlatNormals);
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): the mesh has no positions, can't generate normals\n");
}

void CompileGLTest::generateNormals2DPosition() {
    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector2, nullptr}}};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compile(data, CompileFlag::GenerateFlatNormals);
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): can't generate normals for VertexFormat::Vector2 positions\n");
}

void CompileGLTest::externalBuffers() {
    auto&& data = DataExternal[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /*
        6-----7-----8
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        3-----4-----5
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        0-----1-----2
    */
    Vector2 positions[] {
        {-0.75f, -0.75f},
        { 0.00f, -0.75f},
        { 0.75f, -0.75f},

        {-0.75f,  0.00f},
        { 0.00f,  0.00f},
        { 0.75f,  0.00f},

        {-0.75f,  0.75f},
        { 0.0f,   0.75f},
        { 0.75f,  0.75f}
    };

    const UnsignedShort indexData[]{
        0, 1, 4, 0, 4, 3,
        1, 2, 5, 1, 5, 4,
        3, 4, 7, 3, 7, 6,
        4, 5, 8, 4, 8, 7
    };

    Trade::MeshData meshData{MeshPrimitive::Triangles,
        {}, indexData, Trade::MeshIndexData{indexData},
        {}, positions, {Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}}};

    /* Duplicate everything if data is non-indexed */
    if(!data.indexed) meshData = duplicate(meshData);

    GL::Buffer indices{NoCreate};
    if(meshData.isIndexed()) {
        indices = GL::Buffer{GL::Buffer::TargetHint::ElementArray};
        indices.setData(meshData.indexData());
    }

    GL::Buffer vertices{GL::Buffer::TargetHint::Array};
    vertices.setData(meshData.vertexData());

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh mesh{NoCreate};
    if(data.moveIndices && data.moveVertices)
        mesh = compile(meshData, std::move(indices), std::move(vertices));
    else if(data.moveIndices && !data.moveVertices)
        mesh = compile(meshData, std::move(indices), vertices);
    else if(!data.moveIndices && data.moveVertices)
        mesh = compile(meshData, indices, std::move(vertices));
    else
        mesh = compile(meshData, indices, vertices);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    _framebuffer.clear(GL::FramebufferClear::Color);
    _flat2D.setColor(0xff3366_rgbf);
    mesh.draw(_flat2D);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "flat2D.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

void CompileGLTest::externalBuffersInvalid() {
    Trade::MeshData data{MeshPrimitive::Triangles, 5};
    Trade::MeshData indexedData{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedInt, nullptr},
        {}};

    std::ostringstream out;
    Error redirectError{&out};
    compile(data, GL::Buffer{NoCreate}, GL::Buffer{}); /* this is okay */
    compile(data, GL::Buffer{NoCreate}, GL::Buffer{NoCreate});
    compile(indexedData, GL::Buffer{NoCreate}, GL::Buffer{});
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): invalid external buffer(s)\n"
        "MeshTools::compile(): invalid external buffer(s)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompileGLTest)
