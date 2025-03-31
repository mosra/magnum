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
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/System.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Complex.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Shaders/PhongGL.h" /* for testing correct depth in 3D */
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Line.h"
#include "Magnum/Shaders/LineGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct LineGLTest: GL::OpenGLTester {
    explicit LineGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructAsync();
    template<UnsignedInt dimensions> void constructUniformBuffers();
    template<UnsignedInt dimensions> void constructUniformBuffersAsync();

    template<UnsignedInt dimensions> void constructMove();
    template<UnsignedInt dimensions> void constructMoveUniformBuffers();

    /* No constructInvalid() as there isn't any assertion in the constructor
       that wouldn't be related to UBOs */
    template<UnsignedInt dimensions> void constructUniformBuffersInvalid();

    template<UnsignedInt dimensions> void setUniformUniformBuffersEnabled();
    template<UnsignedInt dimensions> void bindBufferUniformBuffersNotEnabled();
    template<UnsignedInt dimensions> void setMiterLengthLimitInvalid();
    template<UnsignedInt dimensions> void setMiterAngleLimitInvalid();
    template<UnsignedInt dimensions> void setObjectIdNotEnabled();
    template<UnsignedInt dimensions> void setWrongDrawOffset();

    void renderSetupLarge();
    void renderSetupSmall();
    void renderTeardown();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderDefaults2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderDefaults3D();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderLineCapsJoins2D();
    /* These test the shader algorithms, which are independent of UBOs */
    void renderLineCapsJoins2DReversed();
    void renderLineCapsJoins2DTransformed();

    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderCube3D();
    /* This test the shader algorithms, which are independent of UBOs */
    void renderPerspective3D();

    template<class T, LineGL2D::Flag flag = LineGL2D::Flag{}> void renderVertexColor2D();
    template<class T, LineGL3D::Flag flag = LineGL3D::Flag{}> void renderVertexColor3D();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderObjectId2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderObjectId3D();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderInstanced2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderInstanced3D();

    void renderMulti2D();
    void renderMulti3D();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};

        GL::Renderbuffer _color{NoCreate};
        GL::Renderbuffer _depth{NoCreate};
        GL::Renderbuffer _objectId{NoCreate};
        GL::Framebuffer _framebuffer{NoCreate};
};

using namespace Containers::Literals;
using namespace Math::Literals;

const struct {
    const char* name;
    LineGL2D::Flags flags;
    Containers::Optional<LineCapStyle> capStyle;
    Containers::Optional<LineJoinStyle> joinStyle;
} ConstructData[]{
    {"", {}, {}, {}},
    {"square caps, bevel joins", {}, LineCapStyle::Square, LineJoinStyle::Bevel},
    {"round caps, miter joins", {}, LineCapStyle::Round, LineJoinStyle::Miter},
    /** @todo use JoinStyle::MiterClip once it exists */
    {"butt caps, miter joins", {}, LineCapStyle::Butt, LineJoinStyle::Miter},
    /** @todo use JoinStyle::Round once it exists */
    {"triangle caps, miter joins", {}, LineCapStyle::Triangle, LineJoinStyle::Miter},
    {"vertex colors", LineGL2D::Flag::VertexColor, {}, {}},
    {"object ID", LineGL2D::Flag::ObjectId, {}, {}},
    {"instanced object ID", LineGL2D::Flag::InstancedObjectId, {}, {}},
    {"instanced transformation", LineGL2D::Flag::InstancedTransformation, {}, {}},
};

const struct {
    const char* name;
    LineGL2D::Flags flags;
    Containers::Optional<LineCapStyle> capStyle;
    Containers::Optional<LineJoinStyle> joinStyle;
    UnsignedInt materialCount, drawCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {}, {}, {}, 1, 1},
    {"", LineGL2D::Flag::UniformBuffers, {}, {}, 1, 1},
    /* Just to verify that access to the miter limits is properly guarded,
       no need to check all variants */
    {"round caps, miter joins", LineGL2D::Flag::UniformBuffers, LineCapStyle::Round, LineJoinStyle::Miter, 1, 1},
    {"butt caps, bevel joins", LineGL2D::Flag::UniformBuffers, LineCapStyle::Butt, LineJoinStyle::Bevel, 1, 1},
    /* SwiftShader has 256 uniform vectors at most, per-draw is 4+1 in 3D case
       and 3+1 in 2D, per-material 3 */
    {"multiple materials, draws", LineGL2D::Flag::UniformBuffers, {}, {}, 16, 41},
    {"object ID", LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::ObjectId, {}, {}, 1, 1},
    {"instanced object ID", LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::InstancedObjectId, {}, {}, 1, 1},
    {"multidraw with all the things", LineGL2D::Flag::MultiDraw|LineGL2D::Flag::ObjectId|LineGL2D::Flag::InstancedTransformation|LineGL2D::Flag::InstancedObjectId, {}, {}, 16, 41},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with all the things", LineGL2D::Flag::ShaderStorageBuffers|LineGL2D::Flag::MultiDraw|LineGL2D::Flag::ObjectId|LineGL2D::Flag::InstancedTransformation|LineGL2D::Flag::InstancedObjectId, {}, {}, 0, 0},
    #endif
};

const struct {
    const char* name;
    LineGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    /* These two fail for UBOs but not SSBOs */
    {"zero draws", LineGL2D::Flag::UniformBuffers, 1, 0,
        "draw count can't be zero"},
    {"zero materials", LineGL2D::Flag::UniformBuffers, 0, 1,
        "material count can't be zero"}
};

const struct {
    const char* name;
    LineJoinStyle joinStyle;
    Float limit;
    const char* message;
} SetMiterLengthLimitInvalidData[]{
    {"wrong join style", LineJoinStyle::Bevel, 1.0f,
        "the shader was created with Shaders::LineJoinStyle::Bevel"},
    {"too short", LineJoinStyle::Miter, 0.9997f,
        "expected a finite value greater than or equal to 1, got 0.9997"},
    {"too long", LineJoinStyle::Miter, Constants::inf(),
        "expected a finite value greater than or equal to 1, got inf"},
};

const struct {
    const char* name;
    LineJoinStyle joinStyle;
    Rad limit;
    const char* message;
} SetMiterAngleLimitInvalidData[]{
    {"wrong join style", LineJoinStyle::Bevel, 90.0_degf,
        "the shader was created with Shaders::LineJoinStyle::Bevel"},
    {"too small", LineJoinStyle::Miter, 0.0_degf,
        "expected a value greater than 0° and less than or equal to 180°, got 0°"},
    {"too large", LineJoinStyle::Miter, 180.1_degf,
        "expected a value greater than 0° and less than or equal to 180°, got 180.1°"}
};

const struct {
    const char* name;
    Float width;
    Float smoothness;
    Containers::Optional<Float> miterLengthLimit;
    Containers::Optional<Deg> miterAngleLimit;
    Containers::Optional<LineCapStyle> capStyle;
    Containers::Optional<LineJoinStyle> joinStyle;
    const char* expected;
} RenderLineCapsJoins2DData[]{
    {"caps & joints default, flat",
        16.0f, 0.0f, {}, {}, {}, {},
        "caps-square-joins-miter-flat.tga"},
    {"caps butt, joins default, flat",
        16.0f, 0.0f, {}, {},
        LineCapStyle::Butt, {},
        "caps-butt-joins-miter-flat.tga"},
    {"caps butt, joins bevel", /** @todo bevel smoothing is off for 30° */
        16.0f, 1.0f, {}, {},
        LineCapStyle::Butt, LineJoinStyle::Bevel,
        "caps-butt-joins-bevel.tga"},
    {"caps square, joins miter",
        16.0f, 1.0f, {}, {},
        LineCapStyle::Square, LineJoinStyle::Miter,
        "caps-square-joins-miter.tga"},
    {"caps square, joins bevel", /** @todo bevel smoothing is off for 30° */
        16.0f, 1.0f, {}, {},
        LineCapStyle::Square, LineJoinStyle::Bevel,
        "caps-square-joins-bevel.tga"},
    {"caps square, joins miter, limit 3.95",
        16.0f, 1.0f, 3.95f, {},
        LineCapStyle::Square, LineJoinStyle::Miter,
        /* Same as default */
        "caps-square-joins-miter.tga"},
    {"caps square, joins miter, limit 3.6",
        16.0f, 1.0f, 3.6f, {}, /** @todo 3.85 should work but it doesn't */
        LineCapStyle::Square, LineJoinStyle::Miter,
        /* The 30° join should get a bevel here */
        "caps-square-joins-miter-limit-36.tga"},
    {"caps square, joins miter, limit 59°",
        16.0f, 1.0f, {}, 59.0_degf,
        LineCapStyle::Square, LineJoinStyle::Miter,
        /* Same as limit 3.6, the 30° join gets a bevel */
        "caps-square-joins-miter-limit-36.tga"},
    {"caps square, joins miter, limit 70°",
        16.0f, 1.0f, {}, 70.0_degf, /** @todo 61° should work but it doesn't */
        LineCapStyle::Square, LineJoinStyle::Miter,
        /* The 30° and 60° join should get a bevel here, 90° and 120° should
           stay */
        "caps-square-joins-miter-limit-70deg.tga"},
    {"caps square, joins miter, limit 89°",
        16.0f, 1.0f, {}, 89.0_degf,
        LineCapStyle::Square, LineJoinStyle::Miter,
        /* Same as limit 61°, the 30° and 60° joins get a bevel, 90° and 120°
           not */
        "caps-square-joins-miter-limit-70deg.tga"},
    {"caps square, joins miter, limit 91°",
        16.0f, 1.0f, {}, 91.0_degf,
        LineCapStyle::Square, LineJoinStyle::Miter,
        /* The 30°, 60° and 90° join should get a bevel here, 120° should
           stay */
        "caps-square-joins-miter-limit-91deg.tga"},
    {"caps round, joins miter",
        /** @todo use round joins instead once implemented */
        16.0f, 1.0f, {}, {},
        LineCapStyle::Round, LineJoinStyle::Miter,
        "caps-round-joins-miter.tga"},
    {"caps triangle, joins bevel",
        16.0f, 1.0f, {}, {},
        LineCapStyle::Triangle, LineJoinStyle::Bevel,
        "caps-triangle-joins-bevel.tga"},
};

const struct {
    const char* name;
    Float width;
    Float smoothness;
    Containers::Optional<Float> miterLengthLimit;
    Containers::Optional<LineCapStyle> capStyle;
    Containers::Optional<LineJoinStyle> joinStyle;
    bool renderSolidCube;
    const char* expected;
} RenderCube3DData[]{
    {"caps & joins default, flat, single-pixel",
        1.0f, 0.0f, 8.0f, {}, {}, false,
        "cube3D-flat-single-pixel.tga"},
    {"caps square, joins miter",
        10.0f, 1.0f, 8.0f, {}, {}, false,
        "cube3D-caps-square-joins-miter.tga"},
    {"caps butt, joins bevel", /** @todo bevel smoothing is off */
        10.0f, 1.0f, {}, LineCapStyle::Butt, LineJoinStyle::Bevel, false,
        "cube3D-caps-butt-joins-bevel.tga"},
    {"depth", /** @todo depth is imprecise for wide lines */
        /* Not smooth, as the cut-off pieces are jaggy anyway */
        10.0f, 0.0f, 8.0f, {}, {}, true,
        "cube3D-depth.tga"},
};

const struct {
    const char* name;
    Float width;
    Float smoothness;
    LineCapStyle capStyle;
} RenderObjectIdData[]{
    {"flat, square caps", 10.0f, 0.0f, LineCapStyle::Square},
    {"flat, round caps", 10.0f, 0.0f, LineCapStyle::Round},
    {"smooth, square caps", 4.0f, 3.0f, LineCapStyle::Square},
};

const struct {
    const char* name;
    UnsignedInt expectedId[3];
    LineGL2D::Flags flags;
} RenderInstancedData[]{
    {"",
        {}, {}},
    {"object ID",
        {1000, 1000, 1000}, LineGL2D::Flag::ObjectId},
    {"instanced object ID",
        {1211, 5627, 36363}, LineGL2D::Flag::InstancedObjectId},
};

const struct {
    const char* name;
    UnsignedInt expectedId[3];
    LineGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
} RenderMultiData[]{
    {"bind with offset",
        {},
        {},
        1, 1, true, 16},
    {"bind with offset, object ID",
        {1211, 5627, 36363},
        LineGL2D::Flag::ObjectId,
        1, 1, true, 16},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, object ID, shader storage",
        {1211, 5627, 36363},
        LineGL2D::Flag::ShaderStorageBuffers|LineGL2D::Flag::ObjectId,
        0, 0, true, 16},
    #endif
    {"draw offset",
        {},
        {},
        2, 3, false, 1},
    {"draw offset, object ID",
        {1211, 5627, 36363},
        LineGL2D::Flag::ObjectId,
        2, 3, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, object ID, shader storage",
        {1211, 5627, 36363},
        LineGL2D::Flag::ShaderStorageBuffers|LineGL2D::Flag::ObjectId,
        0, 0, false, 1},
    #endif
    {"multidraw",
        {},
        LineGL2D::Flag::MultiDraw,
        2, 3, false, 1},
    {"multidraw, object ID",
        {1211, 5627, 36363},
        LineGL2D::Flag::MultiDraw|LineGL2D::Flag::ObjectId,
        2, 3, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, object ID, shader storage",
        {1211, 5627, 36363},
        LineGL2D::Flag::ShaderStorageBuffers|LineGL2D::Flag::MultiDraw|LineGL2D::Flag::ObjectId,
        0, 0, false, 1},
    #endif
};

LineGLTest::LineGLTest() {
    addInstancedTests<LineGLTest>({
        &LineGLTest::construct<2>,
        &LineGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    addTests<LineGLTest>({
        &LineGLTest::constructAsync<2>,
        &LineGLTest::constructAsync<3>});

    addInstancedTests<LineGLTest>({
        &LineGLTest::constructUniformBuffers<2>,
        &LineGLTest::constructUniformBuffers<3>},
        Containers::arraySize(ConstructUniformBuffersData));

    addTests<LineGLTest>({
        &LineGLTest::constructUniformBuffersAsync<2>,
        &LineGLTest::constructUniformBuffersAsync<3>,

        &LineGLTest::constructMove<2>,
        &LineGLTest::constructMove<3>,

        &LineGLTest::constructMoveUniformBuffers<2>,
        &LineGLTest::constructMoveUniformBuffers<3>});

    addInstancedTests<LineGLTest>({
        &LineGLTest::constructUniformBuffersInvalid<2>,
        &LineGLTest::constructUniformBuffersInvalid<3>},
        Containers::arraySize(ConstructUniformBuffersInvalidData));

    addTests<LineGLTest>({
        &LineGLTest::setUniformUniformBuffersEnabled<2>,
        &LineGLTest::setUniformUniformBuffersEnabled<3>,
        &LineGLTest::bindBufferUniformBuffersNotEnabled<2>,
        &LineGLTest::bindBufferUniformBuffersNotEnabled<3>});

    addInstancedTests<LineGLTest>({
        &LineGLTest::setMiterLengthLimitInvalid<2>,
        &LineGLTest::setMiterLengthLimitInvalid<3>},
        Containers::arraySize(SetMiterLengthLimitInvalidData));

    addInstancedTests<LineGLTest>({
        &LineGLTest::setMiterAngleLimitInvalid<2>,
        &LineGLTest::setMiterAngleLimitInvalid<3>},
        Containers::arraySize(SetMiterAngleLimitInvalidData));

    addTests<LineGLTest>({
        &LineGLTest::setObjectIdNotEnabled<2>,
        &LineGLTest::setObjectIdNotEnabled<3>,
        &LineGLTest::setWrongDrawOffset<2>,
        &LineGLTest::setWrongDrawOffset<3>});

    /* MSVC needs explicit type due to default template args */
    addTests<LineGLTest>({
        &LineGLTest::renderDefaults2D,
        &LineGLTest::renderDefaults2D<LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderDefaults2D<LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderDefaults3D,
        &LineGLTest::renderDefaults3D<LineGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderDefaults3D<LineGL3D::Flag::ShaderStorageBuffers>
        #endif
        },
        &LineGLTest::renderSetupSmall,
        &LineGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<LineGLTest>({
        &LineGLTest::renderLineCapsJoins2D,
        &LineGLTest::renderLineCapsJoins2D<LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderLineCapsJoins2D<LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderLineCapsJoins2DReversed,
        &LineGLTest::renderLineCapsJoins2DTransformed},
        Containers::arraySize(RenderLineCapsJoins2DData),
        &LineGLTest::renderSetupLarge,
        &LineGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<LineGLTest>({
        &LineGLTest::renderCube3D,
        &LineGLTest::renderCube3D<LineGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderCube3D<LineGL3D::Flag::ShaderStorageBuffers>
        #endif
        },
        Containers::arraySize(RenderCube3DData),
        &LineGLTest::renderSetupLarge,
        &LineGLTest::renderTeardown);

    addTests({&LineGLTest::renderPerspective3D},
        &LineGLTest::renderSetupSmall,
        &LineGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addTests<LineGLTest>({
        &LineGLTest::renderVertexColor2D<Color3>,
        &LineGLTest::renderVertexColor2D<Color3, LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderVertexColor2D<Color3, LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderVertexColor2D<Color4>,
        &LineGLTest::renderVertexColor2D<Color4, LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderVertexColor2D<Color4, LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderVertexColor3D<Color3>,
        &LineGLTest::renderVertexColor3D<Color3, LineGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderVertexColor3D<Color3, LineGL3D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderVertexColor3D<Color4>,
        &LineGLTest::renderVertexColor3D<Color4, LineGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderVertexColor3D<Color4, LineGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        &LineGLTest::renderSetupSmall,
        &LineGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<LineGLTest>({
        &LineGLTest::renderObjectId2D,
        &LineGLTest::renderObjectId2D<LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderObjectId2D<LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderObjectId3D,
        &LineGLTest::renderObjectId3D<LineGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderObjectId3D<LineGL3D::Flag::ShaderStorageBuffers>
        #endif
        },
        Containers::arraySize(RenderObjectIdData),
        &LineGLTest::renderSetupSmall,
        &LineGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<LineGLTest>({
        &LineGLTest::renderInstanced2D,
        &LineGLTest::renderInstanced2D<LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderInstanced2D<LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &LineGLTest::renderInstanced3D,
        &LineGLTest::renderInstanced3D<LineGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &LineGLTest::renderInstanced3D<LineGL2D::Flag::ShaderStorageBuffers>,
        #endif
        },
        Containers::arraySize(RenderInstancedData),
        &LineGLTest::renderSetupSmall,
        &LineGLTest::renderTeardown);

    addInstancedTests({&LineGLTest::renderMulti2D,
                       &LineGLTest::renderMulti3D},
        Containers::arraySize(RenderMultiData),
        &LineGLTest::renderSetupSmall,
        &LineGLTest::renderTeardown);

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

template<UnsignedInt dimensions> void LineGLTest::construct() {
    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    typename LineGL<dimensions>::Configuration configuration;
    configuration.setFlags(data.flags);
    if(data.capStyle) configuration.setCapStyle(*data.capStyle);
    if(data.joinStyle) configuration.setJoinStyle(*data.joinStyle);
    LineGL<dimensions> shader{configuration};
    CORRADE_COMPARE(shader.flags(), data.flags);
    if(data.capStyle) CORRADE_COMPARE(shader.capStyle(), data.capStyle);
    if(data.joinStyle) CORRADE_COMPARE(shader.joinStyle(), data.joinStyle);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void LineGLTest::constructAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    typename LineGL<dimensions>::CompileState state = LineGL<dimensions>::compile(typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL2D::Flag::VertexColor)
        .setCapStyle(LineCapStyle::Butt)
        .setJoinStyle(LineJoinStyle::Bevel));
    CORRADE_COMPARE(state.flags(),  LineGL2D::Flag::VertexColor);
    CORRADE_COMPARE(state.capStyle(), LineCapStyle::Butt);
    CORRADE_COMPARE(state.joinStyle(), LineJoinStyle::Bevel);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    LineGL<dimensions> shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), LineGL2D::Flag::VertexColor);
    CORRADE_COMPARE(shader.capStyle(), LineCapStyle::Butt);
    CORRADE_COMPARE(shader.joinStyle(), LineJoinStyle::Bevel);

    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void LineGLTest::constructUniformBuffers() {
    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & LineGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= LineGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
    #endif

    if(data.flags >= LineGL2D::Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    typename LineGL<dimensions>::Configuration configuration;
    configuration
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount);
    if(data.capStyle) configuration.setCapStyle(*data.capStyle);
    if(data.joinStyle) configuration.setJoinStyle(*data.joinStyle);
    LineGL<dimensions> shader{configuration};
    CORRADE_COMPARE(shader.flags(), data.flags);
    if(data.capStyle) CORRADE_COMPARE(shader.capStyle(), data.capStyle);
    if(data.joinStyle) CORRADE_COMPARE(shader.joinStyle(), data.joinStyle);
    CORRADE_COMPARE(shader.materialCount(), data.materialCount);
    CORRADE_COMPARE(shader.drawCount(), data.drawCount);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void LineGLTest::constructUniformBuffersAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    typename LineGL<dimensions>::CompileState state = LineGL<dimensions>::compile(typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::VertexColor)
        .setCapStyle(LineCapStyle::Butt)
        .setJoinStyle(LineJoinStyle::Bevel)
        .setMaterialCount(16)
        .setDrawCount(41));
    CORRADE_COMPARE(state.flags(), LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::VertexColor);
    CORRADE_COMPARE(state.capStyle(), LineCapStyle::Butt);
    CORRADE_COMPARE(state.joinStyle(), LineJoinStyle::Bevel);
    CORRADE_COMPARE(state.materialCount(), 16);
    CORRADE_COMPARE(state.drawCount(), 41);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    LineGL<dimensions> shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::VertexColor);
    CORRADE_COMPARE(shader.capStyle(), LineCapStyle::Butt);
    CORRADE_COMPARE(shader.joinStyle(), LineJoinStyle::Bevel);
    CORRADE_COMPARE(shader.materialCount(), 16);
    CORRADE_COMPARE(shader.drawCount(), 41);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void LineGLTest::constructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    LineGL<dimensions> a{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::VertexColor)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    LineGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_VERIFY(!a.id());

    LineGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_VERIFY(!b.id());
}

template<UnsignedInt dimensions> void LineGLTest::constructMoveUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    LineGL<dimensions> a{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    LineGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), LineGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_VERIFY(!a.id());

    LineGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), LineGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_VERIFY(!b.id());
}

template<UnsignedInt dimensions> void LineGLTest::constructUniformBuffersInvalid() {
    auto&& data = ConstructUniformBuffersInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::format("{}", dimensions));
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    Containers::String out;
    Error redirectError{&out};
    LineGL<dimensions>{typename LineGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::LineGL: {}\n", data.message));
}

template<UnsignedInt dimensions> void LineGLTest::setUniformUniformBuffersEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::UniformBuffers)};

    /* This should work fine */
    shader.setViewportSize({});

    Containers::String out;
    Error redirectError{&out};
    shader.setTransformationProjectionMatrix({})
        .setBackgroundColor({})
        .setColor({})
        .setWidth({})
        .setSmoothness({})
        .setMiterLengthLimit({})
        .setMiterAngleLimit({})
        .setObjectId({});
    CORRADE_COMPARE(out,
        "Shaders::LineGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setBackgroundColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setWidth(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setSmoothness(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setMiterLengthLimit(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setMiterAngleLimit(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setObjectId(): the shader was created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void LineGLTest::bindBufferUniformBuffersNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Buffer buffer;
    LineGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.bindTransformationProjectionBuffer(buffer)
          .bindTransformationProjectionBuffer(buffer, 0, 16)
          .bindDrawBuffer(buffer)
          .bindDrawBuffer(buffer, 0, 16)
          .bindMaterialBuffer(buffer)
          .bindMaterialBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out,
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void LineGLTest::setMiterLengthLimitInvalid() {
    auto&& data = SetMiterLengthLimitInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Buffer buffer;
    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setJoinStyle(data.joinStyle)
    };

    Containers::String out;
    Error redirectError{&out};
    shader.setMiterLengthLimit(data.limit);
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::LineGL::setMiterLengthLimit(): {}\n", data.message));
}

template<UnsignedInt dimensions> void LineGLTest::setMiterAngleLimitInvalid() {
    auto&& data = SetMiterAngleLimitInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Buffer buffer;
    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setJoinStyle(data.joinStyle)
    };

    Containers::String out;
    Error redirectError{&out};
    shader.setMiterAngleLimit(data.limit);
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::LineGL::setMiterAngleLimit(): {}\n", data.message));
}

template<UnsignedInt dimensions> void LineGLTest::setObjectIdNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    LineGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setObjectId(33376);
    CORRADE_COMPARE(out,
        "Shaders::LineGL::setObjectId(): the shader was not created with object ID enabled\n");
}

template<UnsignedInt dimensions> void LineGLTest::setWrongDrawOffset() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::LineGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
}

constexpr Vector2i RenderSizeLarge{128, 128};

void LineGLTest::renderSetupLarge() {
    /* The geometry should be generated in CCW order, enable face culling to
       verify that */
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    /* Depth test enabled only in certain cases */

    _color = GL::Renderbuffer{};
    _color.setStorage(GL::RenderbufferFormat::RGBA8, RenderSizeLarge);
    _objectId = GL::Renderbuffer{};
    _objectId.setStorage(GL::RenderbufferFormat::R32UI, RenderSizeLarge);
    _depth = GL::Renderbuffer{};
    _depth.setStorage(GL::RenderbufferFormat::DepthComponent24, RenderSizeLarge);
    _framebuffer = GL::Framebuffer{{{}, RenderSizeLarge}};
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
        .mapForDraw({
            {LineGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}}
            /* ObjectIdOutput mapped in test cases that actually draw to it,
               otherwise it causes an error on WebGL due to the shader not
               rendering to all outputs */
        })
        .clearDepth(1.0f)
        .clearColor(0, 0x111111_rgbf)
        .clearColor(1, Vector4ui{27})
        .bind();
}

constexpr Vector2i RenderSizeSmall{80, 80};

void LineGLTest::renderSetupSmall() {
    /* The geometry should be generated in CCW order, enable face culling to
       verify that */
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    /* Depth test enabled only in certain cases */

    _color = GL::Renderbuffer{};
    _color.setStorage(GL::RenderbufferFormat::RGBA8, RenderSizeSmall);
    _objectId = GL::Renderbuffer{};
    _objectId.setStorage(GL::RenderbufferFormat::R32UI, RenderSizeSmall);
    _depth = GL::Renderbuffer{};
    _depth.setStorage(GL::RenderbufferFormat::DepthComponent24, RenderSizeSmall);
    _framebuffer = GL::Framebuffer{{{}, RenderSizeSmall}};
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
        .mapForDraw({
            {LineGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}}
            /* ObjectIdOutput is mapped (and cleared) in test cases that
               actually draw to it, otherwise it causes an error on WebGL due
               to the shader not rendering to all outputs */
        })
        .clearDepth(1.0f)
        .clearColor(0, 0x111111_rgbf)
        .bind();
}

void LineGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
    _objectId = GL::Renderbuffer{NoCreate};
    _depth = GL::Renderbuffer{NoCreate};
}

/* A barebones utility for generating a line mesh. Embedded directly in the
   test (as opposed to using MeshTools::compileLines()) to have it easier to
   modify, debug and iterate on. */
template<UnsignedInt dimensions> struct Vertex {
    VectorTypeFor<dimensions, Float> previousPosition;
    VectorTypeFor<dimensions, Float> position;
    VectorTypeFor<dimensions, Float> nextPosition;
    LineVertexAnnotations annotation;
};

template<UnsignedInt dimensions> Containers::Array<Vertex<dimensions>> generateLineMeshVertices(Containers::StridedArrayView1D<const VectorTypeFor<dimensions, Float>> lineSegments) {
    CORRADE_INTERNAL_ASSERT(lineSegments.size() % 2 == 0);

    Containers::Array<Vertex<dimensions>> vertices{ValueInit, lineSegments.size()*2};
    for(std::size_t i = 0; i != lineSegments.size(); ++i) {
        vertices[i*2 + 0].position =
            vertices[i*2 + 1].position =
                lineSegments[i];
        if(i % 2 == 0)
            vertices[i*2 + 0].annotation =
                vertices[i*2 + 1].annotation = LineVertexAnnotation::Begin;
        vertices[i*2 + 0].annotation |= LineVertexAnnotation::Up;
    }

    /* Mark joins if the segment endpoints are the same */
    for(std::size_t i = 4; i < vertices.size(); i += 4) {
        if(vertices[i - 2].position != vertices[i].position)
            continue;
        for(std::size_t j: {i - 2, i - 1, i + 0, i + 1})
            vertices[j].annotation |= LineVertexAnnotation::Join;
    }

    /* Prev positions for segment last vertices -- the other segment point */
    for(std::size_t i = 2; i < Containers::arraySize(vertices); i += 4) {
        vertices[i + 0].previousPosition =
            vertices[i + 1].previousPosition =
                vertices[i - 2].position;
    }
    /* Prev positions for segment first vertices -- a neighbor segment, if it's
       a join */
    for(std::size_t i = 4; i < Containers::arraySize(vertices); i += 4) {
        if(!(vertices[i].annotation & LineVertexAnnotation::Join))
            continue;
        vertices[i + 0].previousPosition =
            vertices[i + 1].previousPosition =
                vertices[i - 4].position;
    }
    /* Next positions for segment first vertices -- the other segment point */
    for(std::size_t i = 0; i < Containers::arraySize(vertices) - 2; i += 4) {
        vertices[i + 0].nextPosition =
            vertices[i + 1].nextPosition =
                vertices[i + 2].position;
    }
    /* Next positions for last vertices -- a neighbor segment, if any */
    for(std::size_t i = 2; i < Containers::arraySize(vertices) - 4; i += 4) {
        if(!(vertices[i].annotation & LineVertexAnnotation::Join))
            continue;
        vertices[i + 0].nextPosition =
            vertices[i + 1].nextPosition =
                vertices[i + 4].position;
    }

    return vertices;
}

/* Order consistent with what MeshTools::compileLines() does to avoid pointless
   differences */
Containers::Array<UnsignedInt> generateLineMeshIndices(Containers::StridedArrayView1D<const LineVertexAnnotations> vertexAnnotations) {
    Containers::Array<UnsignedInt> indices;
    for(UnsignedInt i = 0; i != vertexAnnotations.size()/4; ++i) {
        /* 0---2 2
           |  / /|
           | / / |
           |/ /  |
           1 1---3 */
        arrayAppend(indices, {
            i*4 + 2,
            i*4 + 0,
            i*4 + 1,
            i*4 + 1,
            i*4 + 3,
            i*4 + 2
        });

        /* Add also indices for the bevel in both orientations (one will always
           degenerate)

           2 2   2---4 4   4--
            /|   |  / /|   |
           / |   | / / |   | /
             |   |/ /  |   |/
           --3   3 3---5   5 5 */
        if(vertexAnnotations[i*4 + 3] & LineVertexAnnotation::Join) {
            arrayAppend(indices, {
                i*4 + 2,
                i*4 + 3,
                i*4 + 4,
                i*4 + 4,
                i*4 + 3,
                i*4 + 5
            });
        }
    }

    return indices;
}

template<UnsignedInt dimensions> GL::Mesh generateLineMesh(Containers::StridedArrayView1D<const VectorTypeFor<dimensions, Float>> lineSegments) {
    Containers::Array<Vertex<dimensions>> vertices = generateLineMeshVertices<dimensions>(lineSegments);
    Containers::Array<UnsignedInt> indices = generateLineMeshIndices(stridedArrayView(vertices).slice(&Vertex<dimensions>::annotation));

    GL::Mesh mesh;
    mesh.addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, vertices}, 0,
            typename LineGL<dimensions>::PreviousPosition{},
            typename LineGL<dimensions>::Position{},
            typename LineGL<dimensions>::NextPosition{},
            typename LineGL<dimensions>::Annotation{})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, indices}, 0, GL::MeshIndexType::UnsignedInt)
        .setCount(indices.size());

    return mesh;
}

GL::Mesh generateLineMesh(std::initializer_list<Vector2> lineSegments) {
    return generateLineMesh<2>(Containers::arrayView(lineSegments));
}

GL::Mesh generateLineMesh(std::initializer_list<Vector3> lineSegments) {
    return generateLineMesh<3>(Containers::arrayView(lineSegments));
}

template<LineGL2D::Flag flag> void LineGLTest::renderDefaults2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Mesh lines = generateLineMesh({
        /* A / line from the top to bottom */
        {-0.0f, 0.5f}, {-0.5f, -0.5f},
        /* A / line from the bottom to top */
        {-0.5f, -0.5f}, {0.5f, -0.25f},
        /* A | line from the bottom to top */
        {-0.75f, -0.25f}, {-0.75f, 0.75f},
        /* A _ line from the left to right */
        {-0.25f, -0.75f}, {0.75f, -0.75f},
        /* A zero-size line that should be visible as a point */
        {0.5f, 0.5f}, {0.5f, 0.5f}
    });

    LineGL2D shader{LineGL2D::Configuration{}
        .setFlags(flag)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    if(flag == LineGL2D::Flag{}) {
        shader.draw(lines);
    } else if(flag == LineGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

template<LineGL3D::Flag flag> void LineGLTest::renderDefaults3D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* Same as in the 2D case, just with a varying Z coordinate added. As the
       implicit projection is orthographic, this should result in the exact
       same output as 2D. */
    GL::Mesh lines = generateLineMesh({
        /* A / line from the top to bottom, front to back */
        {-0.0f, 0.5f, 1.0f}, {-0.5f, -0.5f, -1.0f},
        /* A / line from the bottom to top, back to front */
        {-0.5f, -0.5f, -1.0f}, {0.5f, -0.25f, 1.0f},
        /* A | line from the bottom to top, on the back */
        {-0.75f, -0.25f, -1.0f}, {-0.75f, 0.75f, -1.0f},
        /* A _ line from the left to right, on the front */
        {-0.25f, -0.75f, 1.0f}, {0.75f, -0.75f, 1.0f},
        /* A zero-size line that should be visible as a point, in the middle */
        {0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}
    });

    LineGL3D shader{LineGL3D::Configuration{}
        .setFlags(flag)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    if(flag == LineGL3D::Flag{}) {
        shader.draw(lines);
    } else if(flag == LineGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

const Vector2 RenderLineCapsJoins2DLineData[]{
    /* A single point (a zero-length line) */
    {0.2f, 0.8f}, {0.2f, 0.8f},
    /* A rotated point (i.e., a line of a very small length). Should ideally be
       symmetric. */
    /** @todo it isn't, why? */
    {-0.4f, 0.15f}, {-0.4f + Math::TypeTraits<Float>::epsilon(), 0.15f + Math::TypeTraits<Float>::epsilon()},
    /* A 90° join with a large length ratio. Caps should look the same on both
       ends, independently on the length */
    {-0.8f, 0.7f}, {-0.8f, -0.25f},
    {-0.8f, -0.25f}, {-0.6f, -0.25f},
    /* A four-segment line with a 60°, 120° and a 30° join. All should be
       miters in the default setup. */
    Vector2{0.25f, 0.3f} + Complex::rotation(-60.0_degf).transformVector(Vector2::yAxis(0.6f)) + Vector2::yAxis(-0.3f),
        Vector2{0.25f, 0.3f} + Complex::rotation(-60.0_degf).transformVector(Vector2::yAxis(0.6f)),
    Vector2{0.25f, 0.3f} + Complex::rotation(-60.0_degf).transformVector(Vector2::yAxis(0.6f)),
        {0.2f, 0.35f},
    {0.2f, 0.35f},
        Vector2{0.25f, 0.3f} + Complex::rotation(60.0_degf).transformVector(Vector2::yAxis(0.6f)),
    Vector2{0.25f, 0.3f} + Complex::rotation(60.0_degf).transformVector(Vector2::yAxis(0.6f)),
        Vector2{0.25f, 0.3f} + Complex::rotation(60.0_degf).transformVector(Vector2::yAxis(0.6f)) +
        Complex::rotation(30.0_degf).transformVector(Vector2::yAxis(-0.5f)),
    /* A completely ordinary line segment, to test the case when everything
       goes wrong */
    {0.4f, -0.05f}, {0.8f, -0.05f},
    /* A 180° join, with one part shorter. Should be always beveled, should not
       overlap and should not disappear. */
    /** @todo the other end disappears, fix */
    {0.8f, -0.4f}, {0.0f, -0.4f},
    {0.0f, -0.4f}, {0.8f, -0.40001f}, /** @todo otherwise disappears, fix */
    /* A join where the other line touches the edge (either slightly above for
       square/round caps, or slightly below for butt caps). The caps should not
       get distorted in any way. */
    /** @todo they do, fix */
    {-0.45f, -0.8f}, {-0.7f, -0.8f},
    {-0.7f, -0.8f},
        Vector2{-0.7f, -0.8f} + Complex::rotation(60.0_degf).transformVector(Vector2::xAxis(0.2f)),
    /* A join where the other line endpoint is inside the line. The caps should
       not get distorted and it shouldn't overlap. */
    /** @todo it's distorted heavily, fix */
    {0.25f, -0.8f}, {0.0f, -0.8f},
    {0.0f, -0.8f},
        Vector2{0.0f, -0.8f} + Complex::rotation(60.0_degf).transformVector(Vector2::xAxis(0.01f)),
    /* Like above, but with the first line short as well */
    /** @todo here's both a heavy distortion and an overlap, fix */
    {0.725f, -0.8f}, {0.7f, -0.8f},
    {0.7f, -0.8f},
        Vector2{0.7f, -0.8f} + Complex::rotation(60.0_degf).transformVector(Vector2::xAxis(0.01f))
};

template<LineGL2D::Flag flag> void LineGLTest::renderLineCapsJoins2D() {
    auto&& data = RenderLineCapsJoins2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Mesh lines = generateLineMesh<2>(RenderLineCapsJoins2DLineData);

    LineGL2D::Configuration configuration;
    configuration.setFlags(flag);
    if(data.capStyle) configuration.setCapStyle(*data.capStyle);
    if(data.joinStyle) configuration.setJoinStyle(*data.joinStyle);
    LineGL2D shader{configuration};
    shader.setViewportSize(Vector2{RenderSizeLarge});

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    if(flag == LineGL2D::Flag{}) {
        shader
            .setWidth(data.width)
            .setSmoothness(data.smoothness)
            .setColor(0x80808080_rgbaf);
        if(data.miterLengthLimit)
            shader.setMiterLengthLimit(*data.miterLengthLimit);
        if(data.miterAngleLimit)
            shader.setMiterAngleLimit(*data.miterAngleLimit);
        shader.draw(lines);
    } else if(flag == LineGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};

        LineMaterialUniform materialUniformData[1];
        (*materialUniformData)
            .setWidth(data.width)
            .setSmoothness(data.smoothness)
            .setColor(0x80808080_rgbaf);
        if(data.miterLengthLimit)
            materialUniformData->setMiterLengthLimit(*data.miterLengthLimit);
        if(data.miterAngleLimit)
            materialUniformData->setMiterAngleLimit(*data.miterAngleLimit);
        GL::Buffer materialUniform{materialUniformData};

        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        image.pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({SHADERS_TEST_DIR, "LineTestFiles", data.expected}),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 119.0f, 0.111f}));

    {
        /** @todo drop this when fixed */
        CORRADE_EXPECT_FAIL("Rendered with overlapping geometry at the moment.");
        CORRADE_COMPARE(Math::max(image.pixels<Color4ub>().asContiguous()), 0x888888ff_rgba);
    }
}

void LineGLTest::renderLineCapsJoins2DReversed() {
    auto&& data = RenderLineCapsJoins2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* As this verifies mainly the algorithm, there's no variant with UBOs --
       those are sufficiently tested elsewhere */

    GL::Mesh lines = generateLineMesh<2>(Containers::stridedArrayView(RenderLineCapsJoins2DLineData).flipped<0>());

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    LineGL2D::Configuration configuration;
    if(data.capStyle) configuration.setCapStyle(*data.capStyle);
    if(data.joinStyle) configuration.setJoinStyle(*data.joinStyle);
    LineGL2D shader{configuration};
    shader
        .setViewportSize(Vector2{RenderSizeLarge})
        .setWidth(data.width)
        .setSmoothness(data.smoothness)
        .setColor(0x80808080_rgbaf);
    if(data.miterLengthLimit) shader.setMiterLengthLimit(*data.miterLengthLimit);
    if(data.miterAngleLimit) shader.setMiterAngleLimit(*data.miterAngleLimit);

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    shader.draw(lines);

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({SHADERS_TEST_DIR, "LineTestFiles", data.expected}),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        /** @todo sync this with render2D() once the overlaps are fixed */
        (DebugTools::CompareImageToFile{_manager, 119.0f, 0.111f}));
}

void LineGLTest::renderLineCapsJoins2DTransformed() {
    auto&& data = RenderLineCapsJoins2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* As this verifies mainly the algorithm, there's no variant with UBOs --
       those are sufficiently tested elsewhere */

    const Matrix3 transformation = Matrix3::scaling({100.0f, 2.0f})*Matrix3::rotation(45.0_degf);

    Containers::Array<Vector2> transformedLineSegments{NoInit, Containers::arraySize(RenderLineCapsJoins2DLineData)};
    for(std::size_t i = 0; i != transformedLineSegments.size(); ++i)
        transformedLineSegments[i] = transformation.transformPoint(RenderLineCapsJoins2DLineData[i]);

    GL::Mesh lines = generateLineMesh<2>(transformedLineSegments);

    LineGL2D::Configuration configuration;
    if(data.capStyle) configuration.setCapStyle(*data.capStyle);
    if(data.joinStyle) configuration.setJoinStyle(*data.joinStyle);
    LineGL2D shader{configuration};
    shader
        .setViewportSize(Vector2{RenderSizeLarge})
        .setWidth(data.width)
        .setSmoothness(data.smoothness)
        .setTransformationProjectionMatrix(transformation.inverted())
        .setColor(0x80808080_rgbaf);

    if(data.miterLengthLimit) shader.setMiterLengthLimit(*data.miterLengthLimit);
    if(data.miterAngleLimit) shader.setMiterAngleLimit(*data.miterAngleLimit);

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    shader.draw(lines);

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({SHADERS_TEST_DIR, "LineTestFiles", data.expected}),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 119.0f, 0.112f}));
}

template<LineGL3D::Flag flag> void LineGLTest::renderCube3D() {
    auto&& data = RenderCube3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* A cube, with the top and bottom quad being a loop and the sides being
       disconnected segments */
    Containers::Array<Vertex<3>> vertices = generateLineMeshVertices<3>(Containers::stridedArrayView<Vector3>({
        {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},     // 0 to 3, loops to 12/13
        {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f},     // 4 to 7
        {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},   // 8 to 11
        {-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 1.0f},   // 12 to 15, loops to 2/3

        {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},   // 16 to 19, loops to 28/29
        {1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f},   // 20 to 23
        {1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, // 24 to 27
        {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f}, // 28 to 31, loops to 18/19

        {-1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f},   // 32 to 35
        {1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},     // 36 to 39
        {1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},   // 40 to 43
        {-1.0f, 1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, // 43 to 47
    }));
    CORRADE_COMPARE(vertices.size(), 48);

    /* Check prerequisites */
    for(std::size_t i: {0, 1, 16, 17}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(vertices[i].previousPosition, Vector3{});
    }
    for(std::size_t i: {14, 15, 30, 31}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(vertices[i].nextPosition, Vector3{});
    }

    /* Manually loop the top & bottom */
    vertices[0].previousPosition = vertices[12].position;
    vertices[1].previousPosition = vertices[13].position;
    vertices[14].nextPosition = vertices[2].position;
    vertices[15].nextPosition = vertices[3].position;

    vertices[16].previousPosition = vertices[28].position;
    vertices[17].previousPosition = vertices[29].position;
    vertices[30].nextPosition = vertices[18].position;
    vertices[31].nextPosition = vertices[19].position;

    Containers::Array<UnsignedInt> indices = generateLineMeshIndices(stridedArrayView(vertices).slice(&Vertex<3>::annotation));

    /* Add line join annotation to the looped parts. Has to be done *after*
       generating indices because otherwise it'd assume the next point of the
       join is right after which it isn't. */
    for(std::size_t i: {0, 1, 14, 15, 16, 17, 30, 31}) {
        CORRADE_ITERATION(i);
        CORRADE_VERIFY(!(vertices[i].annotation & LineVertexAnnotation::Join));
        vertices[i].annotation |= LineVertexAnnotation::Join;
    }

    /* Add indices for the two newly created joins */
    arrayAppend(indices, {
        14u, 15u, 0u,
        0u, 15u, 1u,

        30u, 31u, 16u,
        16u, 31u, 17u
    });

    GL::Mesh lines;
    lines.addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, vertices}, 0,
            LineGL3D::PreviousPosition{},
            LineGL3D::Position{},
            LineGL3D::NextPosition{},
            LineGL3D::Annotation{})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, indices}, 0, GL::MeshIndexType::UnsignedInt)
        .setCount(indices.size());

    const Matrix4 projection = Matrix4::perspectiveProjection(50.0_degf, 1.0f, 0.1f, 10.0f);
    const Matrix4 transformation =
        Matrix4::translation({-0.125f, 0.25f, -5.0f})*
        Matrix4::rotationX(25.0_degf)*
        Matrix4::rotationY(30.0_degf);

    if(data.renderSolidCube) {
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);

        PhongGL shader;
        shader.setLightPositions({{-1.0f, 2.0f, 3.0f, 0.0f}})
            .setProjectionMatrix(projection)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setDiffuseColor(0xff0000_rgbf)
            .draw(MeshTools::compile(Primitives::cubeSolid()));

        MAGNUM_VERIFY_NO_GL_ERROR();

        GL::Renderer::setDepthFunction(GL::Renderer::DepthFunction::LessOrEqual);
        GL::Renderer::setDepthMask(false);
    }

    LineGL3D::Configuration configuration;
    configuration.setFlags(flag);
    if(data.capStyle) configuration.setCapStyle(*data.capStyle);
    if(data.joinStyle) configuration.setJoinStyle(*data.joinStyle);
    LineGL3D shader{configuration};
    shader.setViewportSize(Vector2{RenderSizeLarge});

    /* Enabling blending and a half-transparent color -- there should be no
       overlaps */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    if(flag == LineGL3D::Flag{}) {
        shader
            .setTransformationProjectionMatrix(projection*transformation)
            .setWidth(data.width)
            .setSmoothness(data.smoothness)
            .setColor(0x80808080_rgbaf);
        if(data.miterLengthLimit)
            shader.setMiterLengthLimit(*data.miterLengthLimit);
        shader.draw(lines);
    } else if(flag == LineGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL3D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(projection*transformation)
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};

        LineMaterialUniform materialUniformData[1];
        (*materialUniformData)
            .setWidth(data.width)
            .setSmoothness(data.smoothness)
            .setColor(0x80808080_rgbaf);
        if(data.miterLengthLimit)
            materialUniformData->setMiterLengthLimit(*data.miterLengthLimit);
        GL::Buffer materialUniform{materialUniformData};

        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    if(data.renderSolidCube) {
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::setDepthFunction(GL::Renderer::DepthFunction::Less);
        GL::Renderer::setDepthMask(true);
    }

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({SHADERS_TEST_DIR, "LineTestFiles", data.expected}),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 119.0f, 0.102f}));
}

void LineGLTest::renderPerspective3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* Verify that perspective-correct interpolation isn't used (which would
       cause significant artifacts) */
    GL::Mesh lines = generateLineMesh({
        {0.0f, -1.0f, 10.0f}, {0.0f, 7.5f, -10.0f}
    });

    LineGL3D shader;
    shader.setViewportSize(Vector2{RenderSizeSmall})
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(50.0_degf, 1.0f, 0.1f, 50.0f)*
            Matrix4::translation({0.0f, 0.0f, -13.0f}))
        .setWidth(10.0f)
        .setSmoothness(1.0f)
        .draw(lines);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/perspective3D.tga"),
        /* Minor differences on SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 6.0f, 0.025f}));
}

template<class T, LineGL2D::Flag flag> void LineGLTest::renderVertexColor2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Mesh lines = generateLineMesh({
        {-0.8f, 0.5f}, {-0.5f, -0.5f},
        {-0.5f, -0.5f}, {0.0f, 0.0f},
        {0.0f, 0.0f}, {0.5f, -0.5f},
        {0.5f, -0.5f}, {0.8f, 0.5f}
    });

    /* Each line segment from above is four points */
    T colors[]{
        0xff0000_rgbf, 0xff0000_rgbf, 0xffff00_rgbf, 0xffff00_rgbf,
        0xffff00_rgbf, 0xffff00_rgbf, 0x00ffff_rgbf, 0x00ffff_rgbf,
        0x00ffff_rgbf, 0x00ffff_rgbf, 0x00ff00_rgbf, 0x00ff00_rgbf,
        0x00ff00_rgbf, 0x00ff00_rgbf, 0x0000ff_rgbf, 0x0000ff_rgbf
    };
    if(std::is_same<T, Color3>::value)
        lines.addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, colors}, 0, LineGL2D::Color3{});
    else
        lines.addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, colors}, 0, LineGL2D::Color4{});

    LineGL2D shader{LineGL2D::Configuration{}
        .setFlags(LineGL2D::Flag::VertexColor|flag)
        .setCapStyle(LineCapStyle::Triangle)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Set background to blue as well so we don't have too much aliasing */
    GL::Renderer::setClearColor(0x000080_rgbf);
    _framebuffer.clear(GL::FramebufferClear::Color);

    if(flag == LineGL2D::Flag{}) {
        shader
            /* Background should stay blue, foreground should have no blue */
            .setBackgroundColor(0x000080_rgbf)
            .setColor(0x999900_rgbf)
            .setWidth(4.0f)
            .setSmoothness(1.0f)
            .draw(lines);
    } else if(flag == LineGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
                /* Background should stay blue, foreground should have no blue */
                .setBackgroundColor(0x000080_rgbf)
                .setColor(0x999900_rgbf)
                .setWidth(4.0f)
                .setSmoothness(1.0f)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/vertex-color.tga"),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 1.34f, 0.028f}));
}

template<class T, LineGL3D::Flag flag> void LineGLTest::renderVertexColor3D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* Same as renderVertexColor2D(), except that the positions are 3D with
       varying Z. But the (default) projection is orthographic so the output is
       the same -- nothing 3D-specific to test here. */
    GL::Mesh lines = generateLineMesh({
        {-0.8f, 0.5f, 1.0f}, {-0.5f, -0.5f, -1.0f},
        {-0.5f, -0.5f, -1.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, 0.5f}, {0.8f, 0.5f, -1.0f}
    });

    /* Each line segment from above is four points */
    T colors[]{
        0xff0000_rgbf, 0xff0000_rgbf, 0xffff00_rgbf, 0xffff00_rgbf,
        0xffff00_rgbf, 0xffff00_rgbf, 0x00ffff_rgbf, 0x00ffff_rgbf,
        0x00ffff_rgbf, 0x00ffff_rgbf, 0x00ff00_rgbf, 0x00ff00_rgbf,
        0x00ff00_rgbf, 0x00ff00_rgbf, 0x0000ff_rgbf, 0x0000ff_rgbf
    };
    if(std::is_same<T, Color3>::value)
        lines.addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, colors}, 0, LineGL3D::Color3{});
    else
        lines.addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, colors}, 0, LineGL3D::Color4{});

    LineGL3D shader{LineGL3D::Configuration{}
        .setFlags(LineGL3D::Flag::VertexColor|flag)
        .setCapStyle(LineCapStyle::Triangle)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Set background to blue as well so we don't have too much aliasing */
    GL::Renderer::setClearColor(0x000080_rgbf);
    _framebuffer.clear(GL::FramebufferClear::Color);

    if(flag == LineGL3D::Flag{}) {
        shader
            /* Background should stay blue, foreground should have no blue */
            .setBackgroundColor(0x000080_rgbf)
            .setColor(0x999900_rgbf)
            .setWidth(4.0f)
            .setSmoothness(1.0f)
            .draw(lines);
    } else if(flag == LineGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL3D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
                /* Background should stay blue, foreground should have no blue */
                .setBackgroundColor(0x000080_rgbf)
                .setColor(0x999900_rgbf)
                .setWidth(4.0f)
                .setSmoothness(1.0f)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/vertex-color.tga"),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 1.34f, 0.028f}));
}

template<LineGL2D::Flag flag> void LineGLTest::renderObjectId2D() {
    auto&& data = RenderObjectIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    GL::Mesh lines = generateLineMesh({
        {-0.6f, 0.0f}, {0.6f, 0.0f}
    });

    LineGL2D shader{LineGL2D::Configuration{}
        .setFlags(LineGL2D::Flag::ObjectId|flag)
        .setCapStyle(data.capStyle)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    _framebuffer
        .mapForDraw({
            {LineGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {LineGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == LineGL2D::Flag{}) {
        shader
            .setWidth(data.width)
            .setSmoothness(data.smoothness)
            .setObjectId(47365)
            .draw(lines);
    } else if(flag == LineGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
                .setObjectId(47365)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
                .setWidth(data.width)
                .setSmoothness(data.smoothness)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* No need to verify the whole image, just check that pixels at known
       places have expected values. SwiftShader insists that the read format
       has to be 32bit, so the renderbuffer format is that too to make it the
       same (ES3 Mesa complains if these don't match). */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Center of the line, should be set */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[39][39], 47365);
    /* Corner of the line, should be set as well, independently of the cap
       style or smoothness */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[35][11], 47365);
    /* Outside of the object */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[34][11], 27);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[35][10], 27);
}

template<LineGL3D::Flag flag> void LineGLTest::renderObjectId3D() {
    auto&& data = RenderObjectIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* Same as renderObjectId2D(), just with a varying Z coordinate (which
       should have no effect as the projection is orthographic) */
    GL::Mesh lines = generateLineMesh({
        {-0.6f, 0.0f, 1.0f}, {0.6f, 0.0f, -1.0f}
    });

    LineGL3D shader{LineGL3D::Configuration{}
        .setFlags(LineGL3D::Flag::ObjectId|flag)
        .setCapStyle(data.capStyle)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    _framebuffer
        .mapForDraw({
            {LineGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {LineGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == LineGL3D::Flag{}) {
        shader
            .setWidth(data.width)
            .setSmoothness(data.smoothness)
            .setObjectId(47365)
            .draw(lines);
    } else if(flag == LineGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL3D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
                .setObjectId(47365)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
                .setWidth(data.width)
                .setSmoothness(data.smoothness)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* No need to verify the whole image, just check that pixels at known
       places have expected values. SwiftShader insists that the read format
       has to be 32bit, so the renderbuffer format is that too to make it the
       same (ES3 Mesa complains if these don't match). */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Center of the line, should be set */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[39][39], 47365);
    /* Corner of the line, should be set as well, independently of the cap
       style or smoothness */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[35][11], 47365);
    /* Outside of the object */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[34][11], 27);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[35][10], 27);
}

template<LineGL2D::Flag flag> void LineGLTest::renderInstanced2D() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* A wave, important property is that it passes through origin to make
       object ID verification easier */
    GL::Mesh lines = generateLineMesh({
        {-0.8f, -0.8f}, {-0.5f, 0.5f},
        {-0.5f, 0.5f}, {0.5f, -0.5f},
        {0.5f, -0.5f}, {0.8f, 0.8f}
    });

    /* Three circles, each in a different location */
    struct {
        Matrix3 transformation;
        Color3 color;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix3::translation({-1.25f, -1.25f}), 0xffff00_rgbf, 211},
        {Matrix3::translation({ 1.25f, -1.25f}), 0x00ffff_rgbf, 4627},
        {Matrix3::translation({ 0.00f,  1.25f}), 0xff00ff_rgbf, 35363},
    };

    lines
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceData}, 1, 0,
            LineGL2D::TransformationMatrix{},
            LineGL2D::Color3{},
            LineGL2D::ObjectId{})
        .setInstanceCount(3);

    LineGL2D shader{LineGL2D::Configuration{}
        .setFlags(LineGL2D::Flag::InstancedTransformation|LineGL2D::Flag::VertexColor|data.flags|flag)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & LineGL2D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {LineGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {LineGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == LineGL2D::Flag{}) {
        shader
            .setTransformationProjectionMatrix(
                Matrix3::projection({2.1f, 2.1f})*
                Matrix3::scaling(Vector2{0.4f}))
            .setColor(0xffff00_rgbf)
            .setWidth(5.0f)
            .setSmoothness(1.0f);
        if(data.flags & LineGL2D::Flag::ObjectId)
            /* Gets added to the per-instance ID, if that's enabled as well */
            shader.setObjectId(1000);
        shader.draw(lines);
    } else if(flag == LineGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(
                    Matrix3::projection({2.1f, 2.1f})*
                    Matrix3::scaling(Vector2{0.4f})
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
                /* Gets added to the per-instance ID, if that's enabled as
                   well */
                .setObjectId(1000)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
            .setColor(0xffff00_rgbf)
            .setWidth(5.0f)
            .setSmoothness(1.0f)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* - First should be lower left, yellow with a yellow base color, so yellow
       - Second lower right, cyan with a yellow base color, so green
       - Third up center, magenta with a yellow base color, so red */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/instanced.tga"),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 17.0f, 0.107f}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & LineGL2D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});

        MAGNUM_VERIFY_NO_GL_ERROR();

        /* If instanced object IDs are enabled, the per-instance ID gets added
           to the output as well */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][20], data.expectedId[0]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][60], data.expectedId[1]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[60][40], data.expectedId[2]);
    }
}

template<LineGL3D::Flag flag> void LineGLTest::renderInstanced3D() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == LineGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == LineGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* Same as in renderInstanced2D() except for a varying Z coordinate. Which
       shouldn't affect the output as the projection is orthographic. */
    GL::Mesh lines = generateLineMesh({
        {-0.8f, -0.8f, 1.0f}, {-0.5f, 0.5f, -1.0f},
        {-0.5f, 0.5f, -1.0f}, {0.5f, -0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f}, {0.8f, 0.8f, 1.0f}
    });

    /* Three circles, each in a different location */
    struct {
        Matrix4 transformation;
        Color3 color;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix4::translation({-1.25f, -1.25f, 0.5f}), 0xffff00_rgbf, 211},
        {Matrix4::translation({ 1.25f, -1.25f, -0.5f}), 0x00ffff_rgbf, 4627},
        {Matrix4::translation({ 0.00f,  1.25f, 0.0f}), 0xff00ff_rgbf, 35363},
    };

    lines
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceData}, 1, 0,
            LineGL3D::TransformationMatrix{},
            LineGL3D::Color3{},
            LineGL3D::ObjectId{})
        .setInstanceCount(3);

    LineGL3D shader{LineGL3D::Configuration{}
        .setFlags(LineGL3D::Flag::InstancedTransformation|LineGL3D::Flag::VertexColor|data.flags|flag)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & LineGL3D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {LineGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {LineGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == LineGL3D::Flag{}) {
        shader
            .setTransformationProjectionMatrix(
                Matrix4::orthographicProjection({2.1f, 2.1f}, -1.0f, 1.0f)*
                Matrix4::scaling(Vector3{0.4f}))
            .setColor(0xffff00_rgbf)
            .setWidth(5.0f)
            .setSmoothness(1.0f);
        if(data.flags & LineGL2D::Flag::ObjectId)
            /* Gets added to the per-instance ID, if that's enabled as well */
            shader.setObjectId(1000);
        shader.draw(lines);
    } else if(flag == LineGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == LineGL3D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::orthographicProjection({2.1f, 2.1f}, -1.0f, 1.0f)*
                    Matrix4::scaling(Vector3{0.4f})
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
                /* Gets added to the per-instance ID, if that's enabled as
                   well */
                .setObjectId(1000)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
            .setColor(0xffff00_rgbf)
            .setWidth(5.0f)
            .setSmoothness(1.0f)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* - First should be lower left, yellow with a yellow base color, so yellow
       - Second lower right, cyan with a yellow base color, so green
       - Third up center, magenta with a yellow base color, so red */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/instanced.tga"),
        /* Minor differences on NVidia vs Mesa Intel vs SwiftShader */
        (DebugTools::CompareImageToFile{_manager, 17.0f, 0.107f}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & LineGL2D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});

        MAGNUM_VERIFY_NO_GL_ERROR();

        /* If instanced object IDs are enabled, the per-instance ID gets added
           to the output as well */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][20], data.expectedId[0]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][60], data.expectedId[1]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[60][40], data.expectedId[2]);
    }
}

void LineGLTest::renderMulti2D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= LineGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= LineGL2D::Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
        CORRADE_SKIP("UBOs with dynamically indexed arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    /* All parts pass through origin to make object ID verification easier */
    GL::Mesh lines = generateLineMesh({
        /* A wave, same as in renderInstanced2D() */
        {-0.8f, -0.8f}, {-0.5f, 0.5f},  // 0 to 5 + 6 to 12 for the join
        {-0.5f, 0.5f}, {0.5f, -0.5f},   // 12 to 17 + 18 to 23 for the join
        {0.5f, -0.5f}, {0.8f, 0.8f},    // 24 to 29

        /* A cross */
        {-0.8f, -0.8f}, {0.8f, 0.8f},   // 30 to 35
        {0.8f, -0.8f}, {-0.8f, 0.8f},   // 36 to 41

        /* A single point */
        {0.0f, 0.0f}, {0.0f, 0.0f}      // 42 to 47
    });
    CORRADE_COMPARE(lines.count(), 48);

    GL::MeshView wave{lines};
    wave.setCount(30);
    GL::MeshView cross{lines};
    cross.setCount(12)
        .setIndexOffset(30);
    GL::MeshView point{lines};
    point.setCount(6)
        .setIndexOffset(42);

    LineGL2D shader{LineGL2D::Configuration{}
        .setFlags(LineGL2D::Flag::UniformBuffers|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & LineGL2D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {LineGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {LineGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<LineMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = LineMaterialUniform{}
        .setColor(0x0000ff_rgbf)
        .setWidth(3.0f)
        .setSmoothness(3.0f);
    materialData[1*data.uniformIncrement] = LineMaterialUniform{}
        .setColor(0xff0000_rgbf)
        .setWidth(5.0f)
        .setSmoothness(1.0f);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationProjectionUniform2D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::scaling(Vector2{0.4f})*
            Matrix3::translation({-1.25f, -1.25f})
        );
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::scaling(Vector2{0.4f})*
            Matrix3::translation({ 1.25f, -1.25f})
        );
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::scaling(Vector2{0.4f})*
            Matrix3::translation({ 0.00f,  1.25f})
        );
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    Containers::Array<LineDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = LineDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = LineDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = LineDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Enabling blending so the overlap in X is rendered alright */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(LineMaterialUniform),
            sizeof(LineMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(LineDrawUniform),
            sizeof(LineDrawUniform));
        shader.draw(wave);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(LineMaterialUniform),
            sizeof(LineMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(LineDrawUniform),
            sizeof(LineDrawUniform));
        shader.draw(cross);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(LineMaterialUniform),
            sizeof(LineMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(LineDrawUniform),
            sizeof(LineDrawUniform));
        shader.draw(point);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);

        if(data.flags >= LineGL2D::Flag::MultiDraw)
            shader.draw({wave, cross, point});
        else shader
            .setDrawOffset(0)
            .draw(wave)
            .setDrawOffset(1)
            .draw(cross)
            .setDrawOffset(2)
            .draw(point);
    }

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* - Wave should be lower left, red
       - Cross lower right, blue
       - Point up center, red */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/multidraw.tga"),
        /* Minor differences on NVidia vs Mesa Intel, also on ARM Mali */
        (DebugTools::CompareImageToFile{_manager, 0.67f, 0.011f}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & LineGL2D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][20], data.expectedId[0]); /* Wave */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][60], data.expectedId[1]); /* Cross */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[60][40], data.expectedId[2]); /* Point */
    }
}

void LineGLTest::renderMulti3D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= LineGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 3)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= LineGL2D::Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
        CORRADE_SKIP("UBOs with dynamically indexed arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    /* Same as in renderMulti2D() except for an additional varying Z
       coordinate. Which won't affect the output because the projection is
       orthographic. */
    GL::Mesh lines = generateLineMesh({
        {-0.8f, -0.8f, 1.0f}, {-0.5f, 0.5f, -1.0f},
        {-0.5f, 0.5f, -1.0f}, {0.5f, -0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f}, {0.8f, 0.8f, 1.0f},

        {-0.8f, -0.8f, 1.0f}, {0.8f, 0.8f, 1.0f},
        {0.8f, -0.8f, -1.0f}, {-0.8f, 0.8f, -1.0f},

        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}
    });
    CORRADE_COMPARE(lines.count(), 48);

    GL::MeshView wave{lines};
    wave.setCount(30);
    GL::MeshView cross{lines};
    cross.setCount(12)
        .setIndexOffset(30);
    GL::MeshView point{lines};
    point.setCount(6)
        .setIndexOffset(42);

    LineGL3D shader{LineGL3D::Configuration{}
        .setFlags(LineGL3D::Flag::UniformBuffers|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    shader.setViewportSize(Vector2{RenderSizeSmall});

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & LineGL3D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {LineGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {LineGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<LineMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = LineMaterialUniform{}
        .setColor(0x0000ff_rgbf)
        .setWidth(3.0f)
        .setSmoothness(3.0f);
    materialData[1*data.uniformIncrement] = LineMaterialUniform{}
        .setColor(0xff0000_rgbf)
        .setWidth(5.0f)
        .setSmoothness(1.0f);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationProjectionUniform3D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::orthographicProjection({2.1f, 2.1f}, -1.0f, 1.0f)*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({-1.25f, -1.25f, 0.5f})
        );
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::orthographicProjection({2.1f, 2.1f}, -1.0f, 1.0f)*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({ 1.25f, -1.25f, -0.5f})
        );
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::orthographicProjection({2.1f, 2.1f}, -1.0f, 1.0f)*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({ 0.00f,  1.25f, 0.0f})
        );
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    Containers::Array<LineDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = LineDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = LineDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = LineDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Enabling blending so the overlap in X is rendered alright */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::One,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(LineMaterialUniform),
            sizeof(LineMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(LineDrawUniform),
            sizeof(LineDrawUniform));
        shader.draw(wave);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(LineMaterialUniform),
            sizeof(LineMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(LineDrawUniform),
            sizeof(LineDrawUniform));
        shader.draw(cross);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(LineMaterialUniform),
            sizeof(LineMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(LineDrawUniform),
            sizeof(LineDrawUniform));
        shader.draw(point);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);

        if(data.flags >= LineGL3D::Flag::MultiDraw)
            shader.draw({wave, cross, point});
        else shader
            .setDrawOffset(0)
            .draw(wave)
            .setDrawOffset(1)
            .draw(cross)
            .setDrawOffset(2)
            .draw(point);
    }

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* - Wave should be lower left, red
       - Cross lower right, blue
       - Point up center, red */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(SHADERS_TEST_DIR, "LineTestFiles/multidraw.tga"),
        /* Minor differences on NVidia vs Mesa Intel, also on ARM Mali */
        (DebugTools::CompareImageToFile{_manager, 0.67f, 0.011f}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & LineGL3D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][20], data.expectedId[0]); /* Wave */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][60], data.expectedId[1]); /* Cross */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[60][40], data.expectedId[2]); /* Point */
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::LineGLTest)
