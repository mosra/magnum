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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/CompileLines.h"
#include "Magnum/MeshTools/GenerateLines.h"
#include "Magnum/Shaders/FlatGL.h"
#include "Magnum/Shaders/Line.h"
#include "Magnum/Shaders/LineGL.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CompileLinesGLTest: GL::OpenGLTester {
    explicit CompileLinesGLTest();

    void renderSetup();
    void renderTeardown();

    void twoDimensions();
    void threeDimensions();
    void linePrimitiveCompatibility();
    void conflictingAttributes();

    void emptyMesh();

    void notGeneratedLineMesh();

    PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};

    GL::Renderbuffer _color;
    GL::Framebuffer _framebuffer{{{}, {32, 32}}};
};

using namespace Math::Literals;

const struct {
    const char* name;
    bool colors;
    bool flip;
    Shaders::LineJoinStyle joinStyle;
    const char* expected;
} TwoDimensionsData[]{
    {"", false, false, Shaders::LineJoinStyle::Miter, "miter.tga"},
    {"bevel", false, false, Shaders::LineJoinStyle::Bevel, "bevel.tga"},
    {"bevel, flipped", false, true, Shaders::LineJoinStyle::Bevel, "bevel.tga"},
    {"vertex color", true, false, Shaders::LineJoinStyle::Miter, "vertex-color.tga"},
};

const struct {
    const char* name;
    Trade::MeshAttribute attribute;
    VertexFormat format;
    const char* expected;
} ConflictingAttributesData[]{
    {"texture coordinates",
        Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2,
        "MeshTools::compileLines(): Trade::MeshAttribute::TextureCoordinates conflicts with line annotation attribute, ignoring\n"},
    {"tangent",
        Trade::MeshAttribute::Tangent, VertexFormat::Vector3,
        "MeshTools::compileLines(): Trade::MeshAttribute::Tangent conflicts with line previous position attribute, ignoring\n"},
    {"normal",
        Trade::MeshAttribute::Normal, VertexFormat::Vector3,
        "MeshTools::compileLines(): Trade::MeshAttribute::Normal conflicts with line next position attribute, ignoring\n"},
};

CompileLinesGLTest::CompileLinesGLTest() {
    addInstancedTests({&CompileLinesGLTest::twoDimensions},
        Containers::arraySize(TwoDimensionsData),
        &CompileLinesGLTest::renderSetup,
        &CompileLinesGLTest::renderTeardown);

    addTests({&CompileLinesGLTest::threeDimensions,
              &CompileLinesGLTest::linePrimitiveCompatibility},
        &CompileLinesGLTest::renderSetup,
        &CompileLinesGLTest::renderTeardown);

    addInstancedTests({&CompileLinesGLTest::conflictingAttributes},
        Containers::arraySize(ConflictingAttributesData),
        &CompileLinesGLTest::renderSetup,
        &CompileLinesGLTest::renderTeardown);

    addTests({&CompileLinesGLTest::emptyMesh,

              &CompileLinesGLTest::notGeneratedLineMesh});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    _color.setStorage(GL::RenderbufferFormat::RGBA8, {32, 32});
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .bind();
}

void CompileLinesGLTest::renderSetup() {
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    _framebuffer.clear(GL::FramebufferClear::Color);
}

void CompileLinesGLTest::renderTeardown() {}

void CompileLinesGLTest::twoDimensions() {
    auto&& data = TwoDimensionsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Vertex {
        Vector2 position;
        Color3 color;
    } vertexData[]{
        {{-1.0f, -1.0f}, 0xdcdcdc_rgbf},
        {{+1.0f, -1.0f}, 0xdcdcdc_rgbf},
        {{+1.0f, +1.0f}, 0x2f83cc_rgbf},
        {{-1.0f, +1.0f}, 0x2f83cc_rgbf},
    };
    auto vertices = Containers::stridedArrayView(vertexData);

    Containers::Array<Trade::MeshAttributeData> attributes;
    arrayAppend(attributes, InPlaceInit, Trade::MeshAttribute::Position, vertices.slice(&Vertex::position));
    if(data.colors)
        arrayAppend(attributes, InPlaceInit, Trade::MeshAttribute::Color, vertices.slice(&Vertex::color));

    GL::Mesh mesh = compileLines(generateLines(Trade::MeshData{MeshPrimitive::LineLoop, {}, vertexData, Utility::move(attributes)}));

    Shaders::LineGL2D shader{Shaders::LineGL2D::Configuration{}
        .setFlags(data.colors ? Shaders::LineGL2D::Flag::VertexColor : Shaders::LineGL2D::Flags{})
        .setJoinStyle(data.joinStyle)};
    shader
        .setViewportSize({32, 32})
        .setWidth(9)
        .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{21.0f/32.0f}*Vector2::yScale(data.flip ? -1.0f : 1.0f)))
        .draw(mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Path::join({MESHTOOLS_TEST_DIR, "CompileLinesTestFiles", data.expected}),
        /* Minor differences in vertex color rendering on NVidia vs Mesa
           Intel */
        (DebugTools::CompareImageToFile{_manager, 0.25f, 0.007f}));
}

void CompileLinesGLTest::threeDimensions() {
    /* Same as the initial case in twoDimensions(), just in 3D and with a
       varying Z (which shouldn't have any effect as it's rendering in an
       orthographic projection) */

    Vector3 positions[]{
        {-1.0f, -1.0f, +0.5f},
        {+1.0f, -1.0f, +0.5f},
        {+1.0f, +1.0f, -0.5f},
        {-1.0f, +1.0f, -0.5f},
    };

    GL::Mesh mesh = compileLines(generateLines(Trade::MeshData{MeshPrimitive::LineLoop, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positions)}
    }}));

    Shaders::LineGL3D{}
        .setViewportSize({32, 32})
        .setWidth(9)
        .setTransformationProjectionMatrix(Matrix4::scaling({Vector2{21.0f/32.0f}, 1.0f}))
        .draw(mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Path::join(MESHTOOLS_TEST_DIR, "CompileLinesTestFiles/miter.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

void CompileLinesGLTest::linePrimitiveCompatibility() {
    struct Vertex {
        Vector2 position;
        Color3 color;
    } vertexData[]{
        {{-1.0f, -1.0f}, 0xdcdcdc_rgbf},
        {{+1.0f, -1.0f}, 0xdcdcdc_rgbf},
        {{+1.0f, +1.0f}, 0x2f83cc_rgbf},
        {{-1.0f, +1.0f}, 0x2f83cc_rgbf},
    };
    auto vertices = Containers::stridedArrayView(vertexData);

    /* Enabling blending and a half-transparent color to catch accidental
       overlaps where they shouldn't be */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    Trade::MeshData lineMeshData{MeshPrimitive::LineLoop, {}, vertexData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color, vertices.slice(&Vertex::color)},
    }};

    Shaders::FlatGL2D shader{Shaders::FlatGL2D::Configuration{}
        .setFlags(Shaders::FlatGL2D::Flag::VertexColor)};
    shader
        .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{21.0f/32.0f}))
        .setColor(0x80808080_rgbaf);

    /* Render the original */
    shader.draw(compile(lineMeshData));
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Path::join(MESHTOOLS_TEST_DIR, "CompileLinesTestFiles/line-primitive.tga"),
        /* Two/three pixel difference on NVidia vs. Mesa Intel vs Mesa
           llvmpipe, some more on SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 54.25f, 0.139f}));

    _framebuffer.clear(GL::FramebufferClear::Color);

    /* Render the line mesh with the primitive set back to lines. The index
       buffer layout should be compatible with it, and produce the same
       result. */
    shader.draw(compileLines(generateLines(lineMeshData)).setPrimitive(MeshPrimitive::Lines));
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Path::join(MESHTOOLS_TEST_DIR, "CompileLinesTestFiles/line-primitive.tga"),
        /* Two/three pixel difference on NVidia vs. Mesa Intel vs Mesa
           llvmpipe, some more on SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 54.25f, 0.139f}));

    GL::Renderer::disable(GL::Renderer::Feature::Blending);
}

void CompileLinesGLTest::conflictingAttributes() {
    auto&& data = ConflictingAttributesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Same as the initial case in twoDimensions(), should just warn but
       produce correct result */

    struct Vertex {
        Vector3 position;
        Vector3 extra;
    } vertexData[]{
        {{-1.0f, -1.0f, +0.5f}, {}},
        {{+1.0f, -1.0f, +0.5f}, {}},
        {{+1.0f, +1.0f, -0.5f}, {}},
        {{-1.0f, +1.0f, -0.5f}, {}},
    };
    auto vertices = Containers::stridedArrayView(vertexData);

    Trade::MeshData lineMesh = generateLines(Trade::MeshData{MeshPrimitive::LineLoop, {}, vertexData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
        Trade::MeshAttributeData{data.attribute, data.format, vertices.slice(&Vertex::extra)},
    }});

    Containers::String out;
    GL::Mesh mesh{NoCreate};
    {
        Warning redirectWarning{&out};
        mesh = compileLines(lineMesh);
    }

    Shaders::LineGL3D{}
        .setViewportSize({32, 32})
        .setWidth(9)
        .setTransformationProjectionMatrix(Matrix4::scaling({Vector2{21.0f/32.0f}, 1.0f}))
        .draw(mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        _framebuffer.read({{}, {32, 32}}, {PixelFormat::RGBA8Unorm}),
        Utility::Path::join(MESHTOOLS_TEST_DIR, "CompileLinesTestFiles/miter.tga"),
        (DebugTools::CompareImageToFile{_manager}));
    CORRADE_COMPARE(out, data.expected);
}

void CompileLinesGLTest::emptyMesh() {
    GL::Mesh mesh = compileLines(generateLines(Trade::MeshData{MeshPrimitive::LineLoop, {}, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr}
    }}));

    CORRADE_COMPARE(mesh.primitive(), GL::MeshPrimitive::Triangles);
    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.count(), 0);
}

void CompileLinesGLTest::notGeneratedLineMesh() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 positions[3]{};

    Containers::String out;
    Error redirectError{&out};
    compileLines(Trade::MeshData{MeshPrimitive::Lines, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positions)}
    }});
    compileLines(Trade::MeshData{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positions)}
    }});
    CORRADE_COMPARE(out,
        "MeshTools::compileLines(): the mesh wasn't produced with generateLines()\n"
        "MeshTools::compileLines(): the mesh wasn't produced with generateLines()\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompileLinesGLTest)
