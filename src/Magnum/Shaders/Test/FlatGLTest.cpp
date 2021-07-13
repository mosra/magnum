/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
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
#include "Magnum/Shaders/FlatGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/TextureArray.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Cone.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Flat.h"
#endif

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct FlatGLTest: GL::OpenGLTester {
    explicit FlatGLTest();

    template<UnsignedInt dimensions> void construct();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffers();
    #endif

    template<UnsignedInt dimensions> void constructMove();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructMoveUniformBuffers();
    #endif

    template<UnsignedInt dimensions> void constructInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffersInvalid();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setUniformUniformBuffersEnabled();
    template<UnsignedInt dimensions> void bindBufferUniformBuffersNotEnabled();
    #endif
    template<UnsignedInt dimensions> void bindTextureInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void bindTextureArrayInvalid();
    #endif
    template<UnsignedInt dimensions> void setAlphaMaskNotEnabled();
    template<UnsignedInt dimensions> void setTextureMatrixNotEnabled();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setTextureLayerNotArray();
    template<UnsignedInt dimensions> void bindTextureTransformBufferNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setObjectIdNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setWrongDrawOffset();
    #endif

    void renderSetup();
    void renderTeardown();

    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderDefaults2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderDefaults3D();
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderColored2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderColored3D();
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderSinglePixelTextured2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderSinglePixelTextured3D();
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderTextured2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderTextured3D();

    template<class T, FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderVertexColor2D();
    template<class T, FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderVertexColor3D();

    void renderAlphaSetup();
    void renderAlphaTeardown();

    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderAlpha2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderAlpha3D();

    #ifndef MAGNUM_TARGET_GLES2
    void renderObjectIdSetup();
    void renderObjectIdTeardown();

    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderObjectId2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderObjectId3D();
    #endif

    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderInstanced2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderInstanced3D();

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti2D();
    void renderMulti3D();
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
    Rendering tests done:

    [B] base
    [A] alpha mask
    [D] object ID
    [I] instancing
    [O] UBOs + draw offset
    [M] multidraw
    [L] texture arrays

    Mesa Intel                      BADIOML
               ES2                      xxx
               ES3                  BAD Ox
    Mesa AMD                        BAD
    Mesa llvmpipe                   BAD
    SwiftShader ES2                 BAD xxx
                ES3                 BAD
    ANGLE ES2                           xxx
          ES3                       BAD OM
    ARM Mali (Huawei P10) ES2       BAD xxx
                          ES3       BAD Ox
    WebGL (on Mesa Intel) 1.0       BAD xxx
                          2.0       BAD OM
    NVidia                          BAD
    Intel Windows                   BAD
    AMD macOS                       BAD
    Intel macOS                     BAD Ox
    iPhone 6 w/ iOS 12.4 ES3        BAD  x
*/

using namespace Math::Literals;

constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
} ConstructData[]{
    {"", {}},
    {"textured", FlatGL2D::Flag::Textured},
    {"textured + texture transformation", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays},
    {"texture arrays + texture transformation", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::TextureTransformation},
    #endif
    {"alpha mask", FlatGL2D::Flag::AlphaMask},
    {"alpha mask + textured", FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::Textured},
    {"vertex colors", FlatGL2D::Flag::VertexColor},
    {"vertex colors + textured", FlatGL2D::Flag::VertexColor|FlatGL2D::Flag::Textured},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", FlatGL2D::Flag::ObjectId},
    {"instanced object ID", FlatGL2D::Flag::InstancedObjectId},
    {"object ID + alpha mask + textured", FlatGL2D::Flag::ObjectId|FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::Textured},
    #endif
    {"instanced transformation", FlatGL2D::Flag::InstancedTransformation},
    {"instanced texture offset", FlatGL2D::Flag::Textured|FlatGL2D::Flag::InstancedTextureOffset},
    #ifndef MAGNUM_TARGET_GLES2
    {"instanced texture array offset + layer", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::InstancedTextureOffset},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {}, 1, 1},
    {"", FlatGL2D::Flag::UniformBuffers, 1, 1},
    /* SwiftShader has 256 uniform vectors at most, per-draw is 4+1 in 3D case
       and 3+1 in 2D, per-material 2 */
    {"multiple materials, draws", FlatGL2D::Flag::UniformBuffers, 8, 48},
    {"textured", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured, 1, 1},
    {"textured + texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation, 1, 1},
    {"texture arrays + texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation, 1, 1},
    {"alpha mask", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::AlphaMask, 1, 1},
    {"object ID", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectId, 1, 1},
    {"instanced texture array offset + layer", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::InstancedTextureOffset, 1, 1},
    {"multidraw with all the things", FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::ObjectId|FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::InstancedObjectId, 8, 48}
};
#endif

constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    const char* message;
} ConstructInvalidData[]{
    {"texture transformation but not textured", FlatGL2D::Flag::TextureTransformation,
        "texture transformation enabled but the shader is not textured"},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays but not textured", FlatGL2D::Flag::TextureArrays,
        "texture arrays enabled but the shader is not textured"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    {"zero draws", FlatGL2D::Flag::UniformBuffers, 1, 0,
        "draw count can't be zero"},
    {"zero materials", FlatGL2D::Flag::UniformBuffers, 0, 1,
        "material count can't be zero"},
    {"texture arrays but no transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays, 1, 1,
        "texture arrays require texture transformation enabled as well if uniform buffers are used"}
};
#endif

constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    const char* message;
} BindTextureInvalidData[]{
    {"not textured", {},
        "the shader was not created with texturing enabled"},
    #ifndef MAGNUM_TARGET_GLES2
    {"array", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        "the shader was created with texture arrays enabled, use a Texture2DArray instead"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    const char* message;
} BindTextureArrayInvalidData[]{
    {"not textured", {},
        "the shader was not created with texturing enabled"},
    {"not array", FlatGL2D::Flag::Textured,
        "the shader was not created with texture arrays enabled, use a Texture2D instead"}
};
#endif

const struct {
    const char* name;
    FlatGL2D::Flags flags;
    Int layer;
} RenderSinglePixelTexturedData[]{
    {"", {}, 0},
    #ifndef MAGNUM_TARGET_GLES2
    {"array, first layer", FlatGL2D::Flag::TextureArrays, 0},
    {"array, arbitrary layer", FlatGL2D::Flag::TextureArrays, 6},
    #endif
};

const struct {
    const char* name;
    FlatGL2D::Flags flags;
    Matrix3 textureTransformation;
    Int layer;
    bool flip;
} RenderTexturedData[]{
    {"",
        FlatGL2D::Flag::Textured,
        {}, 0, false},
    {"texture transformation",
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        0, true},
    #ifndef MAGNUM_TARGET_GLES2
    {"array, first layer",
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        {}, 0, false},
    {"array, arbitrary layer",
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        {}, 6, false},
    {"array, texture transformation, arbitrary layer",
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        6, true},
    #endif
};

const struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    bool blending;
    FlatGL2D::Flags flags;
    Float threshold;
} RenderAlphaData[] {
    /* All those deliberately have a non-white diffuse in order to match the
       expected data from textured() */
    {"none", "FlatTestFiles/textured2D.tga", "FlatTestFiles/textured3D.tga", false,
        FlatGL2D::Flag::Textured, 0.0f},
    {"blending", "FlatTestFiles/textured2D-alpha.tga", "FlatTestFiles/textured3D-alpha.tga", true,
        FlatGL2D::Flag::Textured, 0.0f},
    {"masking 0.0", "FlatTestFiles/textured2D.tga", "FlatTestFiles/textured3D.tga", false,
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::AlphaMask, 0.0f},
    {"masking 0.5", "FlatTestFiles/textured2D-alpha-mask0.5.tga", "FlatTestFiles/textured3D-alpha-mask0.5.tga", false,
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::AlphaMask, 0.5f},
    {"masking 1.0", "TestFiles/alpha-mask1.0.tga", "TestFiles/alpha-mask1.0.tga", false,
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::AlphaMask, 1.0f}
    /* texture arrays are orthogonal to this, no need to be tested here */
};

constexpr struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    FlatGL2D::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData[] {
    {"colored", "instanced2D.tga", "instanced3D.tga",
        {},
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    {"textured", "instanced-textured2D.tga", "instanced-textured3D.tga",
        FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::Textured,
        /* Minor differences on SwiftShader */
        192.67f, 0.140f},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture array", "instanced-textured2D.tga", "instanced-textured3D.tga",
        FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around); minor differences on
           SwiftShader */
        192.67f, 0.398f},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData[] {
    {"bind with offset, colored", "multidraw2D.tga", "multidraw3D.tga",
        {}, 1, 1, 16, 0.0f, 0.0f},
    {"bind with offset, textured", "multidraw-textured2D.tga", "multidraw-textured3D.tga",
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured,
        1, 1, 16,
        /* Minor differences on ARM Mali */
        2.34f, 0.01f},
    {"bind with offset, texture array", "multidraw-textured2D.tga", "multidraw-textured3D.tga",
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        1, 1, 16,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    {"draw offset, colored", "multidraw2D.tga", "multidraw3D.tga",
        {},
        2, 3, 1, 0.0f, 0.0f},
    {"draw offset, textured", "multidraw-textured2D.tga", "multidraw-textured3D.tga",
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured,
        2, 3, 1,
        /* Minor differences on ARM Mali */
        2.34f, 0.01f},
    {"draw offset, texture array", "multidraw-textured2D.tga", "multidraw-textured3D.tga",
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        2, 3, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    {"multidraw, colored", "multidraw2D.tga", "multidraw3D.tga",
        FlatGL2D::Flag::MultiDraw, 2, 3, 1, 0.0f, 0.0f},
    {"multidraw, textured", "multidraw-textured2D.tga", "multidraw-textured3D.tga",
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured,
        2, 3, 1,
        /* Minor differences on ARM Mali */
        2.34f, 0.01f},
    {"multidraw, texture array", "multidraw-textured2D.tga", "multidraw-textured3D.tga",
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        2, 3, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f}
};
#endif

FlatGLTest::FlatGLTest() {
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::construct<2>,
        &FlatGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::constructUniformBuffers<2>,
        &FlatGLTest::constructUniformBuffers<3>},
        Containers::arraySize(ConstructUniformBuffersData));
    #endif

    addTests<FlatGLTest>({
        &FlatGLTest::constructMove<2>,
        &FlatGLTest::constructMove<3>,

        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::constructMoveUniformBuffers<2>,
        &FlatGLTest::constructMoveUniformBuffers<3>,
        #endif
        });

    addInstancedTests<FlatGLTest>({
        &FlatGLTest::constructInvalid<2>,
        &FlatGLTest::constructInvalid<3>},
        Containers::arraySize(ConstructInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::constructUniformBuffersInvalid<2>,
        &FlatGLTest::constructUniformBuffersInvalid<3>},
        Containers::arraySize(ConstructUniformBuffersInvalidData));
    #endif

    addTests<FlatGLTest>({
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::setUniformUniformBuffersEnabled<2>,
        &FlatGLTest::setUniformUniformBuffersEnabled<3>,
        &FlatGLTest::bindBufferUniformBuffersNotEnabled<2>,
        &FlatGLTest::bindBufferUniformBuffersNotEnabled<3>,
        #endif
    });

    addInstancedTests<FlatGLTest>({
        &FlatGLTest::bindTextureInvalid<2>,
        &FlatGLTest::bindTextureInvalid<3>},
        Containers::arraySize(BindTextureInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::bindTextureArrayInvalid<2>,
        &FlatGLTest::bindTextureArrayInvalid<3>},
        Containers::arraySize(BindTextureArrayInvalidData));
    #endif

    addTests<FlatGLTest>({
        &FlatGLTest::setAlphaMaskNotEnabled<2>,
        &FlatGLTest::setAlphaMaskNotEnabled<3>,
        &FlatGLTest::setTextureMatrixNotEnabled<2>,
        &FlatGLTest::setTextureMatrixNotEnabled<3>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::setTextureLayerNotArray<2>,
        &FlatGLTest::setTextureLayerNotArray<3>,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::bindTextureTransformBufferNotEnabled<2>,
        &FlatGLTest::bindTextureTransformBufferNotEnabled<3>,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::setObjectIdNotEnabled<2>,
        &FlatGLTest::setObjectIdNotEnabled<3>,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::setWrongDrawOffset<2>,
        &FlatGLTest::setWrongDrawOffset<3>,
        #endif
    });

    /* MSVC needs explicit type due to default template args */
    addTests<FlatGLTest>({
        &FlatGLTest::renderDefaults2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderDefaults2D<FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderDefaults3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderDefaults3D<FlatGL3D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderColored2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderColored2D<FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderColored3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderColored3D<FlatGL3D::Flag::UniformBuffers>,
        #endif
        },
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderSinglePixelTextured2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderSinglePixelTextured2D<FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderSinglePixelTextured3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderSinglePixelTextured3D<FlatGL2D::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderSinglePixelTexturedData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderTextured2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderTextured2D<FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderTextured3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderTextured3D<FlatGL3D::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderTexturedData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addTests<FlatGLTest>({
        &FlatGLTest::renderVertexColor2D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor2D<Color3, FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderVertexColor2D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor2D<Color4, FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderVertexColor3D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor3D<Color3, FlatGL3D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderVertexColor3D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor3D<Color4, FlatGL3D::Flag::UniformBuffers>
        #endif
        },
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderAlpha2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderAlpha2D<FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderAlpha3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderAlpha3D<FlatGL3D::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderAlphaData),
        &FlatGLTest::renderAlphaSetup,
        &FlatGLTest::renderAlphaTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<FlatGLTest>({
        &FlatGLTest::renderObjectId2D,
        &FlatGLTest::renderObjectId2D<FlatGL2D::Flag::UniformBuffers>,
        &FlatGLTest::renderObjectId3D,
        &FlatGLTest::renderObjectId3D<FlatGL3D::Flag::UniformBuffers>},
        &FlatGLTest::renderObjectIdSetup,
        &FlatGLTest::renderObjectIdTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderInstanced2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderInstanced2D<FlatGL2D::Flag::UniformBuffers>,
        #endif
        &FlatGLTest::renderInstanced3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderInstanced3D<FlatGL3D::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderInstancedData),
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderObjectIdSetup,
        &FlatGLTest::renderObjectIdTeardown
        #else
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown
        #endif
        );

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&FlatGLTest::renderMulti2D,
                       &FlatGLTest::renderMulti3D},
        Containers::arraySize(RenderMultiData),
        &FlatGLTest::renderObjectIdSetup,
        &FlatGLTest::renderObjectIdTeardown);
    #endif

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

template<UnsignedInt dimensions> void FlatGLTest::construct() {
    setTestCaseTemplateName(std::to_string(dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    FlatGL<dimensions> shader{data.flags};
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

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructUniformBuffers() {
    setTestCaseTemplateName(std::to_string(dimensions));

    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    if(data.flags >= FlatGL2D::Flag::MultiDraw) {
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

    FlatGL<dimensions> shader{data.flags, data.materialCount, data.drawCount};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.materialCount(), data.materialCount);
    CORRADE_COMPARE(shader.drawCount(), data.drawCount);
    CORRADE_VERIFY(shader.id());
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::constructMove() {
    setTestCaseTemplateName(std::to_string(dimensions));

    FlatGL<dimensions> a{FlatGL<dimensions>::Flag::Textured};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    FlatGL<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), FlatGL<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!a.id());

    FlatGL<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), FlatGL<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructMoveUniformBuffers() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    FlatGL<dimensions> a{FlatGL<dimensions>::Flag::UniformBuffers, 2, 5};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    FlatGL<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), FlatGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_VERIFY(!a.id());

    FlatGL<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), FlatGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_VERIFY(!b.id());
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::constructInvalid() {
    auto&& data = ConstructInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(std::to_string(dimensions));
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    FlatGL<dimensions>{data.flags};
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::FlatGL: {}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructUniformBuffersInvalid() {
    auto&& data = ConstructUniformBuffersInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(std::to_string(dimensions));
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    FlatGL<dimensions>{data.flags, data.materialCount, data.drawCount};
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::FlatGL: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setUniformUniformBuffersEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader{FlatGL<dimensions>::Flag::UniformBuffers};
    shader.setTransformationProjectionMatrix({})
        .setTextureMatrix({})
        .setTextureLayer({})
        .setColor({})
        .setAlphaMask({})
        .setObjectId({});
    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setTextureMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setTextureLayer(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setAlphaMask(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setObjectId(): the shader was created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void FlatGLTest::bindBufferUniformBuffersNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Buffer buffer;
    FlatGL<dimensions> shader;
    shader.bindTransformationProjectionBuffer(buffer)
          .bindTransformationProjectionBuffer(buffer, 0, 16)
          .bindDrawBuffer(buffer)
          .bindDrawBuffer(buffer, 0, 16)
          .bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16)
          .bindMaterialBuffer(buffer)
          .bindMaterialBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::bindTextureInvalid() {
    auto&& data = BindTextureInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(std::to_string(dimensions));
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL<dimensions>::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader{data.flags};
    GL::Texture2D texture;
    shader.bindTexture(texture);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::FlatGL::bindTexture(): {}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::bindTextureArrayInvalid() {
    auto&& data = BindTextureArrayInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(std::to_string(dimensions));
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader{data.flags};
    GL::Texture2DArray textureArray;
    shader.bindTexture(textureArray);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::FlatGL::bindTexture(): {}\n", data.message));
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::setAlphaMaskNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader;
    shader.setAlphaMask(0.75f);

    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

template<UnsignedInt dimensions> void FlatGLTest::setTextureMatrixNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader;
    shader.setTextureMatrix({});

    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setTextureLayerNotArray() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader;
    shader.setTextureLayer(37);

    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::bindTextureTransformBufferNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Buffer buffer{GL::Buffer::TargetHint::Uniform};
    FlatGL<dimensions> shader{FlatGL<dimensions>::Flag::UniformBuffers};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setObjectIdNotEnabled() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    FlatGL<dimensions> shader;
    shader.setObjectId(33376);

    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setWrongDrawOffset() {
    setTestCaseTemplateName(std::to_string(dimensions));

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    FlatGL<dimensions>{FlatGL<dimensions>::Flag::UniformBuffers, 2, 5}
        .setDrawOffset(5);
    CORRADE_COMPARE(out.str(),
        "Shaders::FlatGL::setDrawOffset(): draw offset 5 is out of bounds for 5 draws\n");
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

template<FlatGL2D::Flag flag> void FlatGLTest::renderDefaults2D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    FlatGL2D shader{flag};

    if(flag == FlatGL2D::Flag{}) {
        shader.draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/defaults.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderDefaults3D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    FlatGL3D shader{flag};

    if(flag == FlatGL3D::Flag{}) {
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL3D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/defaults.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderColored2D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    FlatGL2D shader{flag};

    if(flag == FlatGL2D::Flag{}) {
        shader
            .setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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

template<FlatGL3D::Flag flag> void FlatGLTest::renderColored3D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    FlatGL3D shader{flag};

    if(flag == FlatGL3D::Flag{}) {
        shader
            .setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL3D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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

template<FlatGL2D::Flag flag> void FlatGLTest::renderSinglePixelTextured2D() {
    auto&& data = RenderSinglePixelTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates));

    FlatGL2D::Flags flags = FlatGL2D::Flag::Textured|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL2D::Flag::TextureArrays) && !(data.flags & FlatGL2D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL2D::Flag::TextureTransformation;
    }
    #endif
    FlatGL2D shader{flags};

    const Color4ub imageData[]{ 0x9999ff_rgb };
    ImageView2D image{PixelFormat::RGBA8Unorm, Vector2i{1}, imageData};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::TextureArrays) {
        textureArray = GL::Texture2DArray{};
        textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector3i{1, 1, data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, image);
        shader.bindTexture(textureArray);
        if(flag != FlatGL2D::Flag::UniformBuffers && data.layer != 0)
            shader.setTextureLayer(data.layer); /* to verify the default */
    } else
    #endif
    {
        texture = GL::Texture2D{};
        texture.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector2i{1})
            .setSubImage(0, {}, image);
        shader.bindTexture(texture);
    }

    if(flag == FlatGL2D::Flag{}) {
        shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setLayer(data.layer)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & FlatGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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

template<FlatGL3D::Flag flag> void FlatGLTest::renderSinglePixelTextured3D() {
    auto&& data = RenderSinglePixelTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    FlatGL3D::Flags flags = FlatGL2D::Flag::Textured|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers && (data.flags & FlatGL3D::Flag::TextureArrays) && !(data.flags & FlatGL3D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL3D::Flag::TextureTransformation;
    }
    #endif
    FlatGL3D shader{flags};

    const Color4ub imageData[]{ 0x9999ff_rgb };
    ImageView2D image{PixelFormat::RGBA8Unorm, Vector2i{1}, imageData};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::TextureArrays) {
        textureArray = GL::Texture2DArray{};
        textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector3i{1, 1, data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, image);
        shader.bindTexture(textureArray);
        if(flag != FlatGL2D::Flag::UniformBuffers && data.layer != 0)
            shader.setTextureLayer(data.layer); /* to verify the default */
    } else
    #endif
    {
        texture = GL::Texture2D{};
        texture.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector2i{1})
            .setSubImage(0, {}, image);
        shader.bindTexture(texture);
    }

    if(flag == FlatGL3D::Flag{}) {
        shader.setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)
            )
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL3D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setLayer(data.layer)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & FlatGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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

template<FlatGL2D::Flag flag> void FlatGLTest::renderTextured2D() {
    auto&& data = RenderTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

    FlatGL2D::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL2D::Flag::TextureArrays) && !(data.flags & FlatGL2D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL2D::Flag::TextureTransformation;
    }
    #endif
    FlatGL2D shader{flags};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::TextureArrays) {
        textureArray = GL::Texture2DArray{};
        textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, {image->size(), data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, ImageView2D{*image});
        shader.bindTexture(textureArray);
        if(flag != FlatGL2D::Flag::UniformBuffers && data.layer != 0)
            shader.setTextureLayer(data.layer); /* to verify the default */
    } else
    #endif
    {
        texture = GL::Texture2D{};
        texture.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, image->size())
            .setSubImage(0, {}, *image);
        shader.bindTexture(texture);
    }

    if(flag == FlatGL2D::Flag{}) {
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured2D() */
            .setColor(0x9999ff_rgbf)
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layer)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & FlatGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D rendered = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
    /* Dropping the alpha channel, as it's always 1.0 */
    Containers::StridedArrayView2D<Color3ub> pixels =
        Containers::arrayCast<Color3ub>(rendered.pixels<Color4ub>());
    if(data.flip) pixels = pixels.flipped<0>().flipped<1>();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has minor rounding errors, Apple A8 & llvmpipe a bit more */
    const Float maxThreshold = 2.334f, meanThreshold = 0.032f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.667f, meanThreshold = 3.254f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Directory::join(_testDir, "FlatTestFiles/textured2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderTextured3D() {
    auto&& data = RenderTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

    FlatGL3D::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL3D::Flag::TextureArrays) && !(data.flags & FlatGL3D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL3D::Flag::TextureTransformation;
    }
    #endif
    FlatGL3D shader{flags};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::TextureArrays) {
        textureArray = GL::Texture2DArray{};
        textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, {image->size(), data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, ImageView2D{*image});
        shader.bindTexture(textureArray);
        if(flag != FlatGL3D::Flag::UniformBuffers && data.layer != 0)
            shader.setTextureLayer(data.layer); /* to verify the default */
    } else
    #endif
    {
        texture = GL::Texture2D{};
        texture.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, image->size())
            .setSubImage(0, {}, *image);
        shader.bindTexture(texture);
    }

    if(flag == FlatGL3D::Flag{}) {
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader
            .setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(data.flip ? 15.0_degf : -15.0_degf)*
                Matrix4::rotationX(data.flip ? -15.0_degf : 15.0_degf))
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured3D() */
            .setColor(0x9999ff_rgbf)
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL3D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(data.flip ? 15.0_degf : -15.0_degf)*
                    Matrix4::rotationX(data.flip ? -15.0_degf : 15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layer)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays */
        if(flags & FlatGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D rendered = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
    /* Dropping the alpha channel, as it's always 1.0 */
    Containers::StridedArrayView2D<Color3ub> pixels =
        Containers::arrayCast<Color3ub>(rendered.pixels<Color4ub>());
    if(data.flip) pixels = pixels.flipped<0>().flipped<1>();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 139.0f, meanThreshold = 0.087f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 139.0f, meanThreshold = 2.896f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Directory::join(_testDir, "FlatTestFiles/textured3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, FlatGL2D::Flag flag> void FlatGLTest::renderVertexColor2D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else
    #endif
    {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Trade::MeshData circleData = Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates);

    /* Highlight a quarter */
    Containers::Array<T> colorData{DirectInit, circleData.vertexCount(), 0x999999_rgbf};
    for(std::size_t i = 8; i != 16; ++i)
        colorData[i + 1] = 0xffff99_rgbf*1.5f;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh circle = MeshTools::compile(circleData);
    circle.addVertexBuffer(colors, 0, GL::Attribute<Shaders::FlatGL2D::Color3::Location, T>{});

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

    FlatGL2D shader{FlatGL2D::Flag::Textured|FlatGL2D::Flag::VertexColor|flag};
    shader.bindTexture(texture);

    if(flag == FlatGL2D::Flag{}) {
        shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .setColor(0x9999ff_rgbf)
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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

template<class T, FlatGL2D::Flag flag> void FlatGLTest::renderVertexColor3D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else
    #endif
    {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);

    /* Highlight the middle rings */
    Containers::Array<T> colorData{DirectInit, sphereData.vertexCount(), 0x999999_rgbf};
    for(std::size_t i = 6*33; i != 9*33; ++i)
        colorData[i + 1] = 0xffff99_rgbf*1.5f;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<Shaders::FlatGL3D::Color4::Location, T>{});

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

    FlatGL3D shader{FlatGL3D::Flag::Textured|FlatGL3D::Flag::VertexColor|flag};
    shader.bindTexture(texture);

    if(flag == FlatGL2D::Flag{}) {
        shader.setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setColor(0x9999ff_rgbf)
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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

template<FlatGL2D::Flag flag> void FlatGLTest::renderAlpha2D() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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
        Primitives::Circle2DFlag::TextureCoordinates));

    FlatGL2D shader{data.flags|flag};
    shader.bindTexture(texture);

    if(flag == FlatGL2D::Flag{}) {
        /* Test that the default is correct by not setting the threshold if
           it's equal to the default */
        if(data.flags & FlatGL2D::Flag::AlphaMask && data.threshold != 0.5f)
            shader.setAlphaMask(data.threshold);
        shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .setColor(0x9999ff_rgbf)
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
                .setAlphaMask(data.threshold)
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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

template<FlatGL3D::Flag flag> void FlatGLTest::renderAlpha3D() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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
        Primitives::UVSphereFlag::TextureCoordinates));

    FlatGL3D shader{data.flags|flag};
    shader.bindTexture(texture);

    if(flag == FlatGL2D::Flag{}) {
        shader.setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setColor(0x9999ff_rgbf);

        /* Test that the default is correct by not setting the threshold if
           it's equal to the default */
        if(data.flags & FlatGL3D::Flag::AlphaMask && data.threshold != 0.5f)
            shader.setAlphaMask(data.threshold);

        /* For proper Z order draw back faces first and then front faces */
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
        shader.draw(sphere);
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL3D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
                .setAlphaMask(data.threshold)
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);

        /* For proper Z order draw back faces first and then front faces */
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
        shader.draw(sphere);
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
        shader.draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* Minor differences between opaque and diffuse, not sure why. SwiftShader
       has 5 different pixels on the edges, llvmpipe some off-by-one errors */
    const Float maxThreshold = 139.0f, meanThreshold = 0.421f;
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
    _framebuffer = GL::Framebuffer{{{}, RenderSize}};
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        /* Pick a color that's directly representable on RGBA4 as well to
           reduce artifacts (well, and this needs to be consistent with other
           tests that *need* to run on WebGL 1) */
        .clearColor(0, 0x111111_rgbf)
        .bind();

    /* If we don't have EXT_gpu_shader4, we likely don't have integer
       framebuffers either (Mesa's Zink), so skip setting up integer
       attachments to avoid GL errors */
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _objectId = GL::Renderbuffer{};
        _objectId.setStorage(GL::RenderbufferFormat::R32UI, RenderSize);
        _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
            .mapForDraw({
                {FlatGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                {FlatGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
            })
            .clearColor(1, Vector4ui{27});
    }
}

void FlatGLTest::renderObjectIdTeardown() {
    _color = GL::Renderbuffer{NoCreate};
    _objectId = GL::Renderbuffer{NoCreate};
    _framebuffer = GL::Framebuffer{NoCreate};
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderObjectId2D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    FlatGL2D shader{FlatGL2D::Flag::ObjectId|flag};

    if(flag == FlatGL2D::Flag{}) {
        shader.setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .setObjectId(48526)
            .draw(circle);
    } else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setObjectId(48526)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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
    /* Inside of the object */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[40][46], 48526);
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderObjectId3D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    FlatGL3D shader{FlatGL3D::Flag::ObjectId|flag};

    if(flag == FlatGL3D::Flag{}) {
        shader.setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setObjectId(48526)
            .draw(sphere);
    } else if(flag == FlatGL3D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setObjectId(48526)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(0x9999ff_rgbf)
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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
    /* Inside of the object */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[40][46], 48526);
}
#endif

template<FlatGL2D::Flag flag> void FlatGLTest::renderInstanced2D() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(GL::Extensions::ARB::instanced_arrays::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::instanced_arrays>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("GL_{ANGLE,EXT,NV}_instanced_arrays is not supported");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(GL::Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates));

    /* Three circles, each in a different location */
    struct {
        Matrix3 transformation;
        Color3 color;
        Vector3 textureOffsetLayer;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix3::translation({-1.25f, -1.25f}),
            data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf,
            {0.0f, 0.0f, 0.0f}, 211},
        {Matrix3::translation({ 1.25f, -1.25f}),
            data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0x00ffff_rgbf,
            {1.0f, 0.0f, 1.0f}, 4627},
        {Matrix3::translation({ 0.00f,  1.25f}),
            data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xff00ff_rgbf,
            #ifndef MAGNUM_TARGET_GLES2
            data.flags & FlatGL2D::Flag::TextureArrays ? Vector3{0.0f, 0.0f, 2.0f} :
            #endif
            Vector3{0.5f, 1.0f, 2.0f}, 35363},
    };

    circle
        .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
            FlatGL2D::TransformationMatrix{},
            FlatGL2D::Color3{},
            #ifndef MAGNUM_TARGET_GLES2
            FlatGL2D::TextureOffsetLayer{},
            #else
            FlatGL2D::TextureOffset{},
            4,
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            FlatGL2D::ObjectId{}
            #else
            4
            #endif
        )
        .setInstanceCount(3);

    /* Enable also Object ID, if supported */
    FlatGL2D::Flags flags = FlatGL2D::Flag::VertexColor|
        FlatGL2D::Flag::InstancedTransformation|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        flags |= FlatGL2D::Flag::InstancedObjectId;
    }
    #endif
    FlatGL2D shader{flags};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    #endif
    if(data.flags & FlatGL3D::Flag::Textured) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

        #ifndef MAGNUM_TARGET_GLES2
        /* For arrays we upload three slices of the original image to half-high
           slices */
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            /** @todo implement image slicing, ffs */
            const ImageView2D first{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({0, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D second{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/2, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D third{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/4, image->size().y()/2, 0}),
                image->format(), image->size()/2, image->data()};

            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                /* Three slices with 2 extra as a base offset, each slice has
                   half the height */
                .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 2 + 3})
                .setSubImage(0, {0, 0, 2}, first)
                /* Put the second image on the right half to test that the
                   per-instance offset is used together with the layer */
                .setSubImage(0, {image->size().x()/2, 0, 3}, second)
                .setSubImage(0, {0, 0, 4}, third);
            shader.bindTexture(textureArray);
            if(flag != FlatGL2D::Flag::UniformBuffers)
                shader.setTextureLayer(2); /* base offset */

        } else
        #endif
        {
            texture = GL::Texture2D{};
            texture.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindTexture(texture);
        }
    }

    if(flag == FlatGL2D::Flag{}) {
        shader
            .setColor(data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf)
            .setTransformationProjectionMatrix(
                Matrix3::projection({2.1f, 2.1f})*
                Matrix3::scaling(Vector2{0.4f}));

        if(data.flags & FlatGL3D::Flag::Textured)
            shader.setTextureMatrix(Matrix3::scaling(
                #ifndef MAGNUM_TARGET_GLES2
                /* Slices of the texture array have half the height */
                data.flags & FlatGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                #endif
                Vector2{0.5f}
            ));

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        #endif
        {
            shader.setObjectId(1000); /* gets added to the per-instance ID */
        }
        #endif

        shader.draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(
                    Matrix3::projection({2.1f, 2.1f})*
                    Matrix3::scaling(Vector2{0.4f})
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setObjectId(1000) /* gets added to the per-instance ID */
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(Matrix3::scaling(
                    #ifndef MAGNUM_TARGET_GLES2
                    /* Slices of the texture array have half the height */
                    data.flags & FlatGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                    #endif
                    Vector2{0.5f}))
                .setLayer(2) /* base offset */
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf)
        }};
        if(data.flags & FlatGL3D::Flag::Textured)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        Colored case:

        -   First should be lower left, yellow with a yellow base color, so
            yellow
        -   Second lower right, cyan with a yellow base color, so green
        -   Third up center, magenta with a yellow base color, so red

        Textured case:

        -   Lower left has bottom left numbers, so light 7881
        -   Lower light has bottom right, 1223
        -   Up center has 6778
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "FlatTestFiles", data.expected2D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    #ifndef MAGNUM_TARGET_GLES2
    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], 1211);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], 5627);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], 36363);
    }
    #endif
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderInstanced3D() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(GL::Extensions::ARB::instanced_arrays::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::instanced_arrays>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("GL_{ANGLE,EXT,NV}_instanced_arrays is not supported");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(GL::Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    /* Three spheres, each in a different location */
    struct {
        Matrix4 transformation;
        Color3 color;
        Vector3 textureOffsetLayer;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix4::translation({-1.25f, -1.25f, 0.0f})*
            /* To be consistent with Phong's output where it tests that the
               normal matrix is applied properly */
            Matrix4::rotationX(90.0_degf),
            data.flags & FlatGL3D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf,
            {0.0f, 0.0f, 0.0f}, 211},
        {Matrix4::translation({ 1.25f, -1.25f, 0.0f}),
            data.flags & FlatGL3D::Flag::Textured ? 0xffffff_rgbf : 0x00ffff_rgbf,
            {1.0f, 0.0f, 1.0f}, 4627},
        {Matrix4::translation({  0.0f,  1.0f, 1.0f}),
            data.flags & FlatGL3D::Flag::Textured ? 0xffffff_rgbf : 0xff00ff_rgbf,
            #ifndef MAGNUM_TARGET_GLES2
            data.flags & FlatGL2D::Flag::TextureArrays ? Vector3{0.0f, 0.0f, 2.0f} :
            #endif
            Vector3{0.5f, 1.0f, 2.0f}, 35363}
    };

    sphere
        .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
            FlatGL3D::TransformationMatrix{},
            FlatGL3D::Color3{},
            #ifndef MAGNUM_TARGET_GLES2
            FlatGL2D::TextureOffsetLayer{},
            #else
            FlatGL2D::TextureOffset{},
            4,
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            FlatGL2D::ObjectId{}
            #else
            4
            #endif
        )
        .setInstanceCount(3);

    /* Enable also Object ID, if supported */
    FlatGL3D::Flags flags = FlatGL3D::Flag::VertexColor|
        FlatGL3D::Flag::InstancedTransformation|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        flags |= FlatGL2D::Flag::InstancedObjectId;
    }
    #endif
    FlatGL3D shader{flags};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    #endif
    if(data.flags & FlatGL2D::Flag::Textured) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

        #ifndef MAGNUM_TARGET_GLES2
        /* For arrays we upload three slices of the original image to half-high
           slices */
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            /** @todo implement image slicing, ffs */
            const ImageView2D first{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({0, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D second{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/2, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D third{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/4, image->size().y()/2, 0}),
                image->format(), image->size()/2, image->data()};

            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                /* Three slices with 2 extra as a base offset, each slice has
                   half the height */
                .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 2 + 3})
                .setSubImage(0, {0, 0, 2}, first)
                /* Put the second image on the right half to test that the
                   per-instance offset is used together with the layer */
                .setSubImage(0, {image->size().x()/2, 0, 3}, second)
                .setSubImage(0, {0, 0, 4}, third);
            shader.bindTexture(textureArray);
            if(flag != FlatGL2D::Flag::UniformBuffers)
                shader.setTextureLayer(2); /* base offset */

        } else
        #endif
        {
            texture = GL::Texture2D{};
            texture.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindTexture(texture);
        }
    }

    if(flag == FlatGL3D::Flag{}) {
        shader
            .setColor(data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf)
            .setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::scaling(Vector3{0.4f}));

        if(data.flags & FlatGL3D::Flag::Textured)
            shader.setTextureMatrix(Matrix3::scaling(
                #ifndef MAGNUM_TARGET_GLES2
                /* Slices of the texture array have half the height */
                data.flags & FlatGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                #endif
                Vector2{0.5f}
            ));

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        #endif
        {
            shader.setObjectId(1000); /* gets added to the per-instance ID */
        }
        #endif

        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::scaling(Vector3{0.4f})
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setObjectId(1000) /* gets added to the per-instance ID */
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(Matrix3::scaling(
                    #ifndef MAGNUM_TARGET_GLES2
                    /* Slices of the texture array have half the height */
                    data.flags & FlatGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                    #endif
                    Vector2{0.5f}))
                .setLayer(2) /* base offset */
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
                .setColor(data.flags & FlatGL3D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf)
        }};
        if(data.flags & FlatGL3D::Flag::Textured)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        Colored case:

        -   First should be lower left, yellow with a yellow base color, so
            yellow
        -   Second lower right, cyan with a yellow base color, so green
        -   Third up center, magenta with a yellow base color, so red

        Textured case:

        -   Lower left has bottom left numbers, so light 7881, rotated (78
            visible)
        -   Lower light has bottom right, 1223
        -   Up center has 6778
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "FlatTestFiles", data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    #ifndef MAGNUM_TARGET_GLES2
    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], 1211);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], 5627);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], 36363);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void FlatGLTest::renderMulti2D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    if(data.flags >= FlatGL2D::Flag::MultiDraw) {
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

    FlatGL2D shader{FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectId|data.flags, data.materialCount, data.drawCount};

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    #endif
    if(data.flags & FlatGL3D::Flag::Textured) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

        #ifndef MAGNUM_TARGET_GLES2
        /* For arrays we upload three slices of the original image to half-high
           slices */
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            /** @todo implement image slicing, ffs */
            const ImageView2D first{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({0, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D second{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/2, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D third{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/4, image->size().y()/2, 0}),
                image->format(), image->size()/2, image->data()};

            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                /* Each slice has half the height */
                .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 3})
                .setSubImage(0, {0, 0, 0}, first)
                /* Put the second image on the right half to test that the
                   per-instance offset is used together with the layer */
                .setSubImage(0, {image->size().x()/2, 0, 1}, second)
                .setSubImage(0, {0, 0, 2}, third);
            shader.bindTexture(textureArray);

        } else
        #endif
        {
            texture = GL::Texture2D{};
            texture.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindTexture(texture);
        }
    }

    /* Circle is a fan, plane is a strip, make it indexed first */
    Trade::MeshData circleData = MeshTools::generateIndices(Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates));
    Trade::MeshData squareData = MeshTools::generateIndices(Primitives::squareSolid(
        Primitives::SquareFlag::TextureCoordinates));
    Trade::MeshData triangleData = MeshTools::generateIndices(Primitives::circle2DSolid(3,
        Primitives::Circle2DFlag::TextureCoordinates));
    GL::Mesh mesh = MeshTools::compile(MeshTools::concatenate({circleData, squareData, triangleData}));
    GL::MeshView circle{mesh};
    circle.setCount(circleData.indexCount());
    GL::MeshView square{mesh};
    square.setCount(squareData.indexCount())
        .setIndexRange(circleData.indexCount());
    GL::MeshView triangle{mesh};
    triangle.setCount(triangleData.indexCount())
        .setIndexRange(circleData.indexCount() + squareData.indexCount());

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<FlatMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(data.flags & FlatGL2D::Flag::Textured ?
            0xffffff_rgbf : 0x0000ff_rgbf);
    materialData[1*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(data.flags & FlatGL2D::Flag::Textured ?
            0xffffff_rgbf : 0xff0000_rgbf);
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

    Containers::Array<TextureTransformationUniform> textureTransformationData{2*data.uniformIncrement + 1};
    textureTransformationData[0*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & FlatGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.0f, 0.0f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & FlatGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({1.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({1.0f, 0.0f}))
        .setLayer(1); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & FlatGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.5f, 1.0f}))
        .setLayer(2); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<FlatDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 1)
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 0)
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 1)
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Just one draw, rebinding UBOs each time */
    if(data.drawCount == 1) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        if(data.flags & FlatGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(circle);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        if(data.flags & FlatGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        if(data.flags & FlatGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(triangle);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
        if(data.flags & FlatGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);

        if(data.flags >= FlatGL2D::Flag::MultiDraw)
            shader.draw({circle, square, triangle});
        else {
            shader.setDrawOffset(0)
                .draw(circle);
            shader.setDrawOffset(1)
                .draw(square);
            shader.setDrawOffset(2)
                .draw(triangle);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        Colored case:

        -   Circle should be lower left, red
        -   Square lower right, blue
        -   Triangle up center, red

        Textured case:

        -   Circle should have bottom left numbers, so light 7881
        -   Square bottom right, 1223
        -   Triangle 6778
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "FlatTestFiles", data.expected2D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], 1211); /* Circle */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], 5627); /* Square */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], 36363); /* Triangle */
    }
}

void FlatGLTest::renderMulti3D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    if(data.flags >= FlatGL3D::Flag::MultiDraw) {
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

    FlatGL3D shader{FlatGL3D::Flag::UniformBuffers|FlatGL3D::Flag::ObjectId|data.flags, data.materialCount, data.drawCount};

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::Textured) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

        /* For arrays we upload three slices of the original image to half-high
           slices */
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            /** @todo implement image slicing, ffs */
            const ImageView2D first{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({0, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D second{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/2, 0, 0}),
                image->format(), image->size()/2, image->data()};
            const ImageView2D third{
                image->storage().setRowLength(image->size().x())
                    .setImageHeight(image->size().y())
                    .setSkip({image->size().x()/4, image->size().y()/2, 0}),
                image->format(), image->size()/2, image->data()};

            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                /* Each slice has half the height */
                .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 3})
                .setSubImage(0, {0, 0, 0}, first)
                /* Put the second image on the right half to test that the
                   per-instance offset is used together with the layer */
                .setSubImage(0, {image->size().x()/2, 0, 1}, second)
                .setSubImage(0, {0, 0, 2}, third);
            shader.bindTexture(textureArray);

        } else {
            texture = GL::Texture2D{};
            texture.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindTexture(texture);
        }
    }

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);
    /* Plane is a strip, make it indexed first */
    Trade::MeshData planeData = MeshTools::generateIndices(Primitives::planeSolid(
        Primitives::PlaneFlag::TextureCoordinates));
    Trade::MeshData coneData = Primitives::coneSolid(1, 32, 1.0f,
        Primitives::ConeFlag::TextureCoordinates);
    GL::Mesh mesh = MeshTools::compile(MeshTools::concatenate({sphereData, planeData, coneData}));
    GL::MeshView sphere{mesh};
    sphere.setCount(sphereData.indexCount());
    GL::MeshView plane{mesh};
    plane.setCount(planeData.indexCount())
        .setIndexRange(sphereData.indexCount());
    GL::MeshView cone{mesh};
    cone.setCount(coneData.indexCount())
        .setIndexRange(sphereData.indexCount() + planeData.indexCount());

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<FlatMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(data.flags & FlatGL2D::Flag::Textured ?
            0xffffff_rgbf : 0x0000ff_rgbf);
    materialData[1*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(data.flags & FlatGL2D::Flag::Textured ?
            0xffffff_rgbf : 0xff0000_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationProjectionUniform3D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({-1.25f, -1.25f, 0.0f})*
            /* To be consistent with Phong's output where it tests that the
               normal matrix is applied properly */
            Matrix4::rotationX(90.0_degf)
        );
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({ 1.25f, -1.25f, 0.0f})
        );
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({  0.0f,  1.0f, 1.0f})
        );
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    Containers::Array<TextureTransformationUniform> textureTransformationData{2*data.uniformIncrement + 1};
    textureTransformationData[0*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & FlatGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.0f, 0.0f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & FlatGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({1.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({1.0f, 0.0f}))
        .setLayer(1); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & FlatGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.5f, 1.0f}))
        .setLayer(2); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<FlatDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are done with
       UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 1)
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 0)
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 1)
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Just one draw, rebinding UBOs each time */
    if(data.drawCount == 1) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        if(data.flags & FlatGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(sphere);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        if(data.flags & FlatGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(plane);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        if(data.flags & FlatGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(cone);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
        if(data.flags & FlatGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);

        if(data.flags >= FlatGL3D::Flag::MultiDraw)
            shader.draw({sphere, plane, cone});
        else {
            shader.setDrawOffset(0)
                .draw(sphere);
            shader.setDrawOffset(1)
                .draw(plane);
            shader.setDrawOffset(2)
                .draw(cone);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        Colored case:

        -   Sphere should be lower left, red
        -   Plane lower right, blue
        -   Cone up center, red

        Textured case:

        -   Sphere should have bottom left numbers, so light 7881, rotated (78
            visible)
        -   Plane bottom right, 1223
        -   Cone 6778
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "FlatTestFiles", data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], 1211); /* Sphere */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], 5627); /* Plane */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], 36363); /* Circle */
    }
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatGLTest)
