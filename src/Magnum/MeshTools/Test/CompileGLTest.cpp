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
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
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
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#endif

#include "configure.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

enum class Flag {
    NonIndexed = 1 << 0,
    Tangents = 1 << 1,
    Bitangents = 1 << 2,
    BitangentsFromTangents = 1 << 3,
    Normals = 1 << 4,
    GeneratedFlatNormals = 1 << 5,
    GeneratedSmoothNormals = 1 << 6,
    TextureCoordinates2D = 1 << 7,
    Colors = 1 << 8,
    ObjectId = 1 << 9
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

        void renderSetup();
        void renderTeardown();

        /** @todo remove the template once MeshDataXD is gone */
        template<class T> void twoDimensions();
        template<class T> void threeDimensions();

        void multipleAttributes();
        void packedAttributes();

        void customAttribute();
        void unsupportedAttribute();
        void implementationSpecificAttributeFormat();

        void generateNormalsNoPosition();
        void generateNormals2DPosition();
        void generateNormalsNoFloats();

        void externalBuffers();
        void externalBuffersInvalid();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};

        Shaders::Flat2D _flat2D;
        Shaders::Flat2D _flatTextured2D{Shaders::Flat2D::Flag::Textured};
        #ifndef MAGNUM_TARGET_GLES2
        Shaders::Flat2D _flatObjectId2D{NoCreate};
        #endif
        Shaders::Flat3D _flat3D;
        Shaders::Flat3D _flatTextured3D{Shaders::Flat3D::Flag::Textured};
        #ifndef MAGNUM_TARGET_GLES2
        Shaders::Flat3D _flatObjectId3D{NoCreate};
        #endif
        Shaders::VertexColor2D _color2D;
        Shaders::VertexColor3D _color3D;
        Shaders::Phong _phong;
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        Shaders::MeshVisualizer3D _meshVisualizer3D{NoCreate};
        Shaders::MeshVisualizer3D _meshVisualizerBitangentsFromTangents3D{NoCreate};
        #endif

        GL::Renderbuffer _color;
        #ifndef MAGNUM_TARGET_GLES2
        GL::Renderbuffer _objectId;
        #endif
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
    {"positions + texture coordinates + colors", Flag::TextureCoordinates2D|Flag::Colors},
    {"positions, object id, nonindexed", Flag::ObjectId|Flag::NonIndexed}
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
    {"positions, tangents, bitangents, normals", Flag::Tangents|Flag::Bitangents|Flag::Normals},
    {"positions, tangents, bitangents from tangents, normals", Flag::Tangents|Flag::BitangentsFromTangents|Flag::Normals},
    {"positions, object id, nonindexed", Flag::ObjectId|Flag::NonIndexed}
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

constexpr struct {
    const char* name;
    CompileFlags flags;
} CustomAttributeWarningData[] {
    {"", {}},
    {"no warning", CompileFlag::NoWarnOnCustomAttributes}
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
        &CompileGLTest::twoDimensions<Trade::MeshData>},
        Containers::arraySize(Data2D),
        &CompileGLTest::renderSetup,
        &CompileGLTest::renderTeardown);

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    addInstancedTests<CompileGLTest>({
        &CompileGLTest::twoDimensions<Trade::MeshData2D>},
        Containers::arraySize(Data2D),
        &CompileGLTest::renderSetup,
        &CompileGLTest::renderTeardown);
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    addInstancedTests<CompileGLTest>({
        &CompileGLTest::threeDimensions<Trade::MeshData>},
        Containers::arraySize(Data3D),
        &CompileGLTest::renderSetup,
        &CompileGLTest::renderTeardown);

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    addInstancedTests<CompileGLTest>({
        &CompileGLTest::threeDimensions<Trade::MeshData3D>},
        Containers::arraySize(Data3D),
        &CompileGLTest::renderSetup,
        &CompileGLTest::renderTeardown);
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    addTests({&CompileGLTest::multipleAttributes,
              &CompileGLTest::packedAttributes},
        &CompileGLTest::renderSetup,
        &CompileGLTest::renderTeardown);

    addInstancedTests({&CompileGLTest::customAttribute,
                       &CompileGLTest::unsupportedAttribute,
                       &CompileGLTest::implementationSpecificAttributeFormat},
        Containers::arraySize(CustomAttributeWarningData));

    addTests({&CompileGLTest::generateNormalsNoPosition,
              &CompileGLTest::generateNormals2DPosition,
              &CompileGLTest::generateNormalsNoFloats});

    addInstancedTests({&CompileGLTest::externalBuffers},
        Containers::arraySize(DataExternal),
        &CompileGLTest::renderSetup,
        &CompileGLTest::renderTeardown);

    addTests({&CompileGLTest::externalBuffersInvalid});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Set up the rendering */
    _color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        {32, 32});
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
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
    /* Use a point light instead of a directional light to better highlight the
       difference in normals; disable specular as that only causes unnecessary
       rounding errors across GPUs */
    _phong.setLightPositions({{0.0f, 0.0f, -0.0f, 1.0f}})
        .setLightColors({0xffffff_rgbf*5.0f})
        .setSpecularColor(0x00000000_rgbaf);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _flatObjectId2D = Shaders::Flat2D{Shaders::Flat2D::Flag::InstancedObjectId};
        _flatObjectId3D = Shaders::Flat3D{Shaders::Flat3D::Flag::InstancedObjectId};
        _objectId.setStorage(GL::RenderbufferFormat::R32UI, {32, 32});
        _framebuffer
            .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
            .mapForDraw({
                {Shaders::Generic3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                {Shaders::Generic3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
            });
    }
    #endif

    /* Mesh visualizer shaders only if we have a GS */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
    #else
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
    #endif
    {
        _meshVisualizer3D = Shaders::MeshVisualizer3D{
            Shaders::MeshVisualizer3D::Flag::TangentDirection|
            Shaders::MeshVisualizer3D::Flag::BitangentDirection|
            Shaders::MeshVisualizer3D::Flag::NormalDirection};
        _meshVisualizerBitangentsFromTangents3D = Shaders::MeshVisualizer3D{
            Shaders::MeshVisualizer3D::Flag::TangentDirection|
            Shaders::MeshVisualizer3D::Flag::BitangentFromTangentDirection|
            Shaders::MeshVisualizer3D::Flag::NormalDirection};
    }
    #endif
}

template<class T> struct MeshTypeName;
template<> struct MeshTypeName<Trade::MeshData> {
    static const char* name() { return "Trade::MeshData"; }
};
#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
template<> struct MeshTypeName<Trade::MeshData2D> {
    static const char* name() { return "Trade::MeshData2D"; }
};
template<> struct MeshTypeName<Trade::MeshData3D> {
    static const char* name() { return "Trade::MeshData3D"; }
};
CORRADE_IGNORE_DEPRECATED_POP
#endif

void CompileGLTest::renderSetup() {
    #ifndef MAGNUM_TARGET_GLES2
    /* To avoid reading from the integer object ID attachment */
    /** @todo ugh this needs to be a global thing and managed through
        scoped RendererState */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});
    #endif
}

void CompileGLTest::renderTeardown() {}

template<class T> void CompileGLTest::twoDimensions() {
    setTestCaseTemplateName(MeshTypeName<T>::name());
    auto&& data = Data2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags & Flag::ObjectId && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH /** @todo remove once MeshDataXD is gone */
    if(std::is_same<T, Trade::MeshData2D>::value && data.flags & Flag::ObjectId)
        CORRADE_SKIP("Not possible with MeshData2D.");
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /*
        Object ID initially set to the same value, bottom half changed to 13562
        after the mesh gets deindexed.

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
        UnsignedInt objectId;
    } vertexData[]{
        {{-0.75f, -0.75f}, {0.0f, 0.0f}, 0x00ff00_rgbf, 26234},
        {{ 0.00f, -0.75f}, {0.5f, 0.0f}, 0x808000_rgbf, 26234},
        {{ 0.75f, -0.75f}, {1.0f, 0.0f}, 0xff0000_rgbf, 26234},

        {{-0.75f,  0.00f}, {0.0f, 0.5f}, 0x00ff80_rgbf, 26234},
        {{ 0.00f,  0.00f}, {0.5f, 0.5f}, 0x808080_rgbf, 26234},
        {{ 0.75f,  0.00f}, {1.0f, 0.5f}, 0xff0080_rgbf, 26234},

        {{-0.75f,  0.75f}, {0.0f, 1.0f}, 0x00ffff_rgbf, 26234},
        {{ 0.0f,   0.75f}, {0.5f, 1.0f}, 0x8080ff_rgbf, 26234},
        {{ 0.75f,  0.75f}, {1.0f, 1.0f}, 0xff00ff_rgbf, 26234}
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
    if(data.flags & Flag::ObjectId)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::ObjectId,
            Containers::stridedArrayView(vertexData, &vertexData[0].objectId,
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
    if(data.flags & Flag::NonIndexed) {
        meshData = duplicate(meshData);

        /* Update object IDs in the bottom half */
        if(data.flags & Flag::ObjectId) {
            auto objectIds = meshData.mutableAttribute<UnsignedInt>(Trade::MeshAttribute::ObjectId);
            for(std::size_t i = 0; i != meshData.vertexCount()/2; ++i)
                objectIds[i] = 13562;
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH /** @todo remove once MeshDataXD is gone */
    #endif
    GL::Mesh mesh = compile(T{std::move(meshData)});
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Check with the flat shader, it should always work */
    {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _flat2D
            .setColor(0xff3366_rgbf)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "flat2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /* Check with the colored shader, if we have colors */
    if(data.flags & Flag::Colors) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _color2D.draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "color2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /* Check with the textured shader, if we have texture coords */
    if(data.flags & Flag::TextureCoordinates2D) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _flatTextured2D
            .bindTexture(_texture)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "textured2D.tga"),
            /* SwiftShader has some minor off-by-one precision differences,
               llvmpipe as well */
            (DebugTools::CompareImageToFile{_manager, 1.75f, 0.22f}));
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* Check object ID rendering, if we have per-vertex object ID */
    if(data.flags & Flag::ObjectId) {
        _framebuffer.clearColor(1, Vector4ui{27});
        _flatObjectId2D.draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        /* Object ID -- no need to verify the whole image, just check that
           pixels on known places have expected values. SwiftShader insists
           that the read format has to be 32bit, so the renderbuffer format is
           that too to make it the same (ES3 Mesa complains if these don't
           match). */
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        /* Outside of the object, cleared to 27 */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[2][2], 27);
        /* Inside of the object, bottom and top half should be different */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[11][18], 13562);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[19][15], 26234);
    }
    #endif
}

template<class T> void CompileGLTest::threeDimensions() {
    setTestCaseTemplateName(MeshTypeName<T>::name());
    auto&& data = Data3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags & Flag::ObjectId && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH /** @todo remove once MeshDataXD is gone */
    if(std::is_same<T, Trade::MeshData3D>::value && data.flags & (Flag::Tangents|Flag::Bitangents|Flag::BitangentsFromTangents|Flag::ObjectId))
        CORRADE_SKIP("Not possible with MeshData3D.");
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /*
        Object ID initially set to the same value, bottom half changed to 13562
        after the mesh gets deindexed.

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
    struct Vertex {
        Vector3 position;
        Vector4 tangent;
        Vector3 bitangent;
        Vector3 normal;
        Vector2 textureCoordinates;
        Color4 color;
        UnsignedInt objectId;
    } vertexData[]{
        {{-0.75f, -0.75f, -0.35f},
         Vector4{Vector3{1.0f, 0.5f, 0.5f}.normalized(), -1.0f}, {},
         Vector3{-0.5f, -0.5f, 1.0f}.normalized(),
         {0.0f, 0.0f}, 0x00ff00_rgbf, 26234},
        {{ 0.00f, -0.75f, -0.25f},
         Vector4{Vector3{1.0f, 0.0f, 0.5f}.normalized(), 1.0f}, {},
         Vector3{ 0.0f, -0.5f, 1.0f}.normalized(),
         {0.5f, 0.0f}, 0x808000_rgbf, 26234},
        {{ 0.75f, -0.75f, -0.35f},
         Vector4{Vector3{1.0f, -0.5f, 0.5f}.normalized(), 1.0f}, {},
         Vector3{ 0.5f, -0.5f, 1.0f}.normalized(),
         {1.0f, 0.0f}, 0xff0000_rgbf, 26234},

        {{-0.75f,  0.00f, -0.25f},
         Vector4{Vector3{1.0f, 0.5f, 0.0f}.normalized(), -1.0f}, {},
         Vector3{-0.5f,  0.0f, 1.0f}.normalized(),
         {0.0f, 0.5f}, 0x00ff80_rgbf, 26234},
        {{ 0.00f,  0.00f,  0.00f},
         Vector4{Vector3{1.0f, 0.0f, 0.0f}.normalized(), 1.0f}, {},
         Vector3{ 0.0f,  0.0f, 1.0f}.normalized(),
         {0.5f, 0.5f}, 0x808080_rgbf, 26234},
        {{ 0.75f,  0.00f, -0.25f},
         Vector4{Vector3{1.0f, -0.5f, 0.0f}.normalized(), 1.0f}, {},
         Vector3{ 0.5f,  0.0f, 1.0f}.normalized(),
         {1.0f, 0.5f}, 0xff0080_rgbf, 26234},

        {{-0.75f,  0.75f, -0.35f},
         Vector4{Vector3{1.0f, -0.5f, 0.0f}.normalized(), -1.0f}, {},
         Vector3{-0.5f,  0.5f, 1.0f}.normalized(),
         {0.0f, 1.0f}, 0x00ffff_rgbf, 26234},
        {{ 0.0f,   0.75f, -0.25f},
         Vector4{Vector3{1.0f, -0.5f, 0.0f}.normalized(), -1.0f}, {},
         Vector3{ 0.0f,  0.5f, 1.0f}.normalized(),
         {0.5f, 1.0f}, 0x8080ff_rgbf, 26234},
        {{ 0.75f,  0.75f, -0.35f},
         Vector4{Vector3{1.0f, -0.5f, 0.0f}.normalized(), -1.0f}, {},
         Vector3{ 0.5f,  0.5f, 1.0f}.normalized(),
         {1.0f, 1.0f}, 0xff00ff_rgbf, 26234}
    };

    /* Calculate bitangents from normal+tangent */
    for(Vertex& i: vertexData)
        i.bitangent = Math::cross(i.normal, i.tangent.xyz())*i.tangent.w();

    Containers::Array<Trade::MeshAttributeData> attributeData;
    arrayAppend(attributeData, Trade::MeshAttributeData{
        Trade::MeshAttribute::Position,
        Containers::stridedArrayView(vertexData, &vertexData[0].position,
            Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::Tangents || data.flags & Flag::BitangentsFromTangents)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::Tangent,
            Containers::stridedArrayView(vertexData, &vertexData[0].tangent,
                Containers::arraySize(vertexData), sizeof(Vertex))});
    if(data.flags & Flag::Bitangents)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::Bitangent,
            Containers::stridedArrayView(vertexData, &vertexData[0].bitangent,
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
    if(data.flags & Flag::ObjectId)
        arrayAppend(attributeData, Trade::MeshAttributeData{
            Trade::MeshAttribute::ObjectId,
            Containers::stridedArrayView(vertexData, &vertexData[0].objectId,
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
    if(data.flags & Flag::NonIndexed) {
        meshData = duplicate(meshData);

        /* Update object IDs in the bottom half */
        if(data.flags & Flag::ObjectId) {
            auto objectIds = meshData.mutableAttribute<UnsignedInt>(Trade::MeshAttribute::ObjectId);
            for(std::size_t i = 0; i != meshData.vertexCount()/2; ++i)
                objectIds[i] = 13562;
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompileFlags flags;
    if(data.flags & Flag::GeneratedFlatNormals)
        flags |= CompileFlag::GenerateFlatNormals;
    if(data.flags & Flag::GeneratedSmoothNormals)
        flags |= CompileFlag::GenerateSmoothNormals;
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH /** @todo remove once MeshDataXD is gone */
    #endif
    GL::Mesh mesh = compile(T{std::move(meshData)}, flags);
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

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
            .setColor(0x6633ff_rgbf)
            .draw(mesh);

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
            .setProjectionMatrix(projection)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.5f, 0.013f}));
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
            .setProjectionMatrix(projection)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong-flat.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.5f, 0.012f}));
    } else if(data.flags & Flag::GeneratedSmoothNormals) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _phong
            .setDiffuseColor(0x33ff66_rgbf)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setProjectionMatrix(projection)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong-smooth.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 0.5f, 0.0088f}));
    }

    /* Check with the colored shader, if we have colors */
    if(data.flags & Flag::Colors) {
        _framebuffer.clear(GL::FramebufferClear::Color);
        _color3D
            .setTransformationProjectionMatrix(projection*transformation)
            .draw(mesh);

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
            .bindTexture(_texture)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "textured3D.tga"),
            /* SwiftShader has some minor off-by-one precision differences,
               llvmpipe as well */
            (DebugTools::CompareImageToFile{_manager, 2.0f, 0.256f}));
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* Check object ID rendering, if we have per-vertex object ID */
    if(data.flags & Flag::ObjectId) {
        _framebuffer.clearColor(1, Vector4ui{27});
        _flatObjectId3D
            .setTransformationProjectionMatrix(projection*transformation)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        /* Object ID -- no need to verify the whole image, just check that
           pixels on known places have expected values. SwiftShader insists
           that the read format has to be 32bit, so the renderbuffer format is
           that too to make it the same (ES3 Mesa complains if these don't
           match). */
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        /* Outside of the object, cleared to 27 */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[2][2], 27);
        /* Inside of the object, bottom and top half should be different */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[11][18], 13562);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[19][15], 26234);
    }
    #endif

    /* Check with the mesh visualizer shader for TBN direction. This has to be
       last, as it gets skipped on WebGL / ES2. */
    if(data.flags >= (Flag::Tangents|Flag::Bitangents|Flag::Normals) ||
       data.flags >= (Flag::Tangents|Flag::BitangentsFromTangents|Flag::Normals)) {
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
        #endif

        _framebuffer.clear(GL::FramebufferClear::Color);

        if(data.flags >= (Flag::Tangents|Flag::Bitangents|Flag::Normals))
            _meshVisualizer3D
                .setTransformationMatrix(transformation)
                .setProjectionMatrix(projection)
                .setViewportSize({32, 32})
                .setSmoothness(0.0f) /* To avoid perspective artifacts */
                .draw(mesh);
        else if(data.flags >= (Flag::Tangents|Flag::BitangentsFromTangents|Flag::Normals))
            _meshVisualizerBitangentsFromTangents3D
                .setTransformationMatrix(transformation)
                .setProjectionMatrix(projection)
                .setViewportSize({32, 32})
                .setSmoothness(0.0f) /* To avoid perspective artifacts */
                .draw(mesh);
        else CORRADE_VERIFY(false);

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE_WITH(
            _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
            Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "tbn.tga"),
            /* SwiftShader has some minor off-by-one precision differences */
            (DebugTools::CompareImageToFile{_manager, 1.0f, 0.0948f}));
        #else
        CORRADE_SKIP("Geometry shaders not available on ES2 or WebGL.");
        #endif
    }
}

void CompileGLTest::multipleAttributes() {
    struct Vertex {
        Vector2 position;
        Vector2 textureCoordinates1, textureCoordinates2;
    } vertexData[]{
        {{-0.75f, -0.75f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.00f, -0.75f}, {0.5f, 0.0f}, {5.0f, 0.0f}},
        {{ 0.75f, -0.75f}, {1.0f, 0.0f}, {10.0f, 0.0f}},

        {{-0.75f,  0.00f}, {0.0f, 0.5f}, {0.0f, 5.0f}},
        {{ 0.00f,  0.00f}, {0.5f, 0.5f}, {5.0f, 5.0f}},
        {{ 0.75f,  0.00f}, {1.0f, 0.5f}, {10.0f, 5.0f}},

        {{-0.75f,  0.75f}, {0.0f, 1.0f}, {0.0f, 10.0f}},
        {{ 0.0f,   0.75f}, {0.5f, 1.0f}, {5.0f, 10.0f}},
        {{ 0.75f,  0.75f}, {1.0f, 1.0f}, {10.0f, 10.0f}}
    };

    const UnsignedInt indexData[]{
        0, 1, 4, 0, 4, 3,
        1, 2, 5, 1, 5, 4,
        3, 4, 7, 3, 7, 6,
        4, 5, 8, 4, 8, 7
    };

    Trade::MeshData meshData{MeshPrimitive::Triangles,
        {}, indexData, Trade::MeshIndexData{indexData},
        {}, vertexData, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::stridedArrayView(vertexData,
                &vertexData[0].position, Containers::arraySize(vertexData), sizeof(Vertex))},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::stridedArrayView(vertexData,
                &vertexData[0].textureCoordinates1, Containers::arraySize(vertexData), sizeof(Vertex))},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::stridedArrayView(vertexData,
                &vertexData[0].textureCoordinates2, Containers::arraySize(vertexData), sizeof(Vertex))},
        }};

    GL::Mesh mesh = compile(meshData);
    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    _framebuffer.clear(GL::FramebufferClear::Color);
    _flatTextured2D
        .bindTexture(_texture)
        .draw(mesh);

    /* The output should be the same as in the textured case of twoDimensions()
       -- i.e., the second texture coordinate set not affecting anything */
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "textured2D.tga"),
        /* SwiftShader has some minor off-by-one precision differences,
            llvmpipe as well */
        (DebugTools::CompareImageToFile{_manager, 1.75f, 0.22f}));
}

/* Can't be inline because MSVC 2015 doesn't like anonymous bitfields in local
   structs */
struct PackedVertex {
    Vector3s position;
    Vector3s normal;
    Vector2us textureCoordinates;
    Color4ub color;
    UnsignedShort objectId;
    UnsignedShort:16;
};

void CompileGLTest::packedAttributes() {

    /*
        Same as above, except that the middle row of indices is duplicated to
        make it possible to have different object IDs for the bottom and top
        row while still be able to test non-default index type as well.

        9----10----11
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        6-----7-----8
        3-----4-----5
        |    /|    /|
        |  /  |  /  |
        |/    |/    |
        0-----1-----2
    */

    const PackedVertex vertexData[]{
        {Math::pack<Vector3s>(Vector3{-0.75f, -0.75f, -0.35f}),
         Math::pack<Vector3s>(Vector3{-0.5f, -0.5f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.0f, 0.0f}), 0x00ff00_rgb, 13562},
        {Math::pack<Vector3s>(Vector3{ 0.00f, -0.75f, -0.25f}),
         Math::pack<Vector3s>(Vector3{ 0.0f, -0.5f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.5f, 0.0f}), 0x808000_rgb, 13562},
        {Math::pack<Vector3s>(Vector3{ 0.75f, -0.75f, -0.35f}),
         Math::pack<Vector3s>(Vector3{ 0.5f, -0.5f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{1.0f, 0.0f}), 0xff0000_rgb, 13562},

        {Math::pack<Vector3s>(Vector3{-0.75f,  0.00f, -0.25f}),
         Math::pack<Vector3s>(Vector3{-0.5f,  0.0f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.0f, 0.5f}), 0x00ff80_rgb, 13562},
        {Math::pack<Vector3s>(Vector3{ 0.00f,  0.00f,  0.00f}),
         Math::pack<Vector3s>(Vector3{ 0.0f,  0.0f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.5f, 0.5f}), 0x808080_rgb, 13562},
        {Math::pack<Vector3s>(Vector3{ 0.75f,  0.00f, -0.25f}),
         Math::pack<Vector3s>(Vector3{ 0.5f,  0.0f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{1.0f, 0.5f}), 0xff0080_rgb, 13562},

        {Math::pack<Vector3s>(Vector3{-0.75f,  0.00f, -0.25f}),
         Math::pack<Vector3s>(Vector3{-0.5f,  0.0f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.0f, 0.5f}), 0x00ff80_rgb, 26234},
        {Math::pack<Vector3s>(Vector3{ 0.00f,  0.00f,  0.00f}),
         Math::pack<Vector3s>(Vector3{ 0.0f,  0.0f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.5f, 0.5f}), 0x808080_rgb, 26234},
        {Math::pack<Vector3s>(Vector3{ 0.75f,  0.00f, -0.25f}),
         Math::pack<Vector3s>(Vector3{ 0.5f,  0.0f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{1.0f, 0.5f}), 0xff0080_rgb, 26234},

        {Math::pack<Vector3s>(Vector3{-0.75f,  0.75f, -0.35f}),
         Math::pack<Vector3s>(Vector3{-0.5f,  0.5f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.0f, 1.0f}), 0x00ffff_rgb, 26234},
        {Math::pack<Vector3s>(Vector3{ 0.0f,   0.75f, -0.25f}),
         Math::pack<Vector3s>(Vector3{ 0.0f,  0.5f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{0.5f, 1.0f}), 0x8080ff_rgb, 26234},
        {Math::pack<Vector3s>(Vector3{ 0.75f,  0.75f, -0.35f}),
         Math::pack<Vector3s>(Vector3{ 0.5f,  0.5f, 1.0f}.normalized()),
         Math::pack<Vector2us>(Vector2{1.0f, 1.0f}), 0xff00ff_rgb, 26234}
    };
    static_assert(sizeof(PackedVertex) % 4 == 0,
        "the vertex is not 4-byte aligned and that's bad");

    const UnsignedByte indexData[]{
        0, 1, 4, 0, 4, 3,
        1, 2, 5, 1, 5, 4,
        6, 7, 10, 6, 10, 9,
        7, 8, 11, 7, 11, 10
    };

    Trade::MeshData meshData{MeshPrimitive::Triangles, {}, indexData,
        Trade::MeshIndexData{indexData}, {}, vertexData, {
            Trade::MeshAttributeData{
                Trade::MeshAttribute::Position,
                VertexFormat::Vector3sNormalized,
                Containers::stridedArrayView(vertexData, &vertexData[0].position,
                    Containers::arraySize(vertexData), sizeof(PackedVertex))},
            Trade::MeshAttributeData{
                Trade::MeshAttribute::Normal,
                VertexFormat::Vector3sNormalized,
                Containers::stridedArrayView(vertexData, &vertexData[0].normal,
                    Containers::arraySize(vertexData), sizeof(PackedVertex))},
            Trade::MeshAttributeData{
                Trade::MeshAttribute::TextureCoordinates,
                VertexFormat::Vector2usNormalized,
                Containers::stridedArrayView(vertexData, &vertexData[0].textureCoordinates,
                    Containers::arraySize(vertexData), sizeof(PackedVertex))},
            Trade::MeshAttributeData{
                Trade::MeshAttribute::Color,
                /* It should figure out the type itself here */
                Containers::stridedArrayView(vertexData, &vertexData[0].color,
                    Containers::arraySize(vertexData), sizeof(PackedVertex))},
            #ifndef MAGNUM_TARGET_GLES2
            Trade::MeshAttributeData{
                Trade::MeshAttribute::ObjectId,
                Containers::stridedArrayView(vertexData, &vertexData[0].objectId,
                    Containers::arraySize(vertexData), sizeof(PackedVertex))}
            #endif
    }};

    GL::Mesh mesh = compile(meshData);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Matrix4 projection = Matrix4::perspectiveProjection(45.0_degf, 1.0f, 0.1f, 10.0f);
    Matrix4 transformation = Matrix4::translation(Vector3::zAxis(-2.0f));

    /* In all checks below, the rendering should be practically 1:1 as above
       with full-blown attribute types */

    /* Check positions and normals */
    _framebuffer.clear(GL::FramebufferClear::Color);
    _phong
        .setDiffuseColor(0x33ff66_rgbf)
        .setTransformationMatrix(transformation)
        .setNormalMatrix(transformation.normalMatrix())
        .setProjectionMatrix(projection)
        .draw(mesh);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "phong.tga"),
        /* SwiftShader has some minor off-by-one precision differences */
        (DebugTools::CompareImageToFile{_manager, 0.5f, 0.013f}));

    /* Check colors */
    _framebuffer.clear(GL::FramebufferClear::Color);
    _color3D
        .setTransformationProjectionMatrix(projection*transformation)
        .draw(mesh);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "color3D.tga"),
        /* SwiftShader has some minor off-by-one precision differences */
        (DebugTools::CompareImageToFile{_manager, 0.5f, 0.0162f}));

    /* Check texture coordinates */
    _framebuffer.clear(GL::FramebufferClear::Color);
    _flatTextured3D
        .setTransformationProjectionMatrix(projection*transformation)
        .bindTexture(_texture)
        .draw(mesh);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "textured3D.tga"),
        /* SwiftShader has some minor off-by-one precision differences,
           llvmpipe more */
        (DebugTools::CompareImageToFile{_manager, 2.0f, 0.259f}));

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _framebuffer.clearColor(1, Vector4ui{27});
        _flatObjectId3D
            .setTransformationProjectionMatrix(projection*transformation)
            .draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        /* Object ID -- no need to verify the whole image, just check that
           pixels on known places have expected values. SwiftShader insists
           that the read format has to be 32bit, so the renderbuffer format is
           that too to make it the same (ES3 Mesa complains if these don't
           match). */
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        /* Outside of the object, cleared to 27 */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[2][2], 27);
        /* Inside of the object, bottom and top half should be different */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[11][18], 13562);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[19][15], 26234);
    }
    #endif
}

void CompileGLTest::customAttribute() {
    auto&& instanceData = CustomAttributeWarningData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {Trade::MeshAttributeData{Trade::meshAttributeCustom(115),
            VertexFormat::Short, nullptr}}};

    std::ostringstream out;
    Warning redirectError{&out};
    if(instanceData.flags)
        MeshTools::compile(data, instanceData.flags);
    else
        MeshTools::compile(data);
    CORRADE_COMPARE(out.str(), instanceData.flags ? "" :
        "MeshTools::compile(): ignoring unknown/unsupported attribute Trade::MeshAttribute::Custom(115)\n");
}

void CompileGLTest::unsupportedAttribute() {
    auto&& instanceData = CustomAttributeWarningData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("All attributes are supported on ES3+.");
    #else
    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
            VertexFormat::UnsignedByte, nullptr}}};

    std::ostringstream out;
    Warning redirectError{&out};
    if(instanceData.flags)
        MeshTools::compile(data, instanceData.flags);
    else
        MeshTools::compile(data);
    /* Warns always, regardless of the flag */
    CORRADE_COMPARE(out.str(), "MeshTools::compile(): ignoring unknown/unsupported attribute Trade::MeshAttribute::ObjectId\n");
    #endif
}

void CompileGLTest::implementationSpecificAttributeFormat() {
    auto&& instanceData = CustomAttributeWarningData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            vertexFormatWrap(0xdead), nullptr}}};

    std::ostringstream out;
    Warning redirectError{&out};
    if(instanceData.flags)
        MeshTools::compile(data, instanceData.flags);
    else
        MeshTools::compile(data);
    CORRADE_COMPARE(out.str(), instanceData.flags ? "" :
        "MeshTools::compile(): ignoring attribute Trade::MeshAttribute::Position with an implementation-specific format 0xdead\n");
}

void CompileGLTest::generateNormalsNoPosition() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData data{MeshPrimitive::Triangles, 1};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compile(data, CompileFlag::GenerateFlatNormals);
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): the mesh has no positions, can't generate normals\n");
}

void CompileGLTest::generateNormals2DPosition() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector2, nullptr}}};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compile(data, CompileFlag::GenerateFlatNormals);
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): can't generate normals for VertexFormat::Vector2 positions\n");
}

void CompileGLTest::generateNormalsNoFloats() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData data{MeshPrimitive::Triangles,
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
                VertexFormat::Vector3h, nullptr},
        }};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compile(data, CompileFlag::GenerateFlatNormals);
    CORRADE_COMPARE(out.str(),
        "MeshTools::compile(): can't generate normals into VertexFormat::Vector3h\n");
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
    _flat2D
        .setColor(0xff3366_rgbf)
        .draw(mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(COMPILEGLTEST_TEST_DIR, "flat2D.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

void CompileGLTest::externalBuffersInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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
