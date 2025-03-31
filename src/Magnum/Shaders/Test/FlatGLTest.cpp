/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/System.h>

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
#include "Magnum/GL/Shader.h"
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
    template<UnsignedInt dimensions> void constructSkinning();
    #endif
    template<UnsignedInt dimensions> void constructAsync();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffers();
    template<UnsignedInt dimensions> void constructUniformBuffersAsync();
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
    template<UnsignedInt dimensions> void setPerVertexJointCountInvalid();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setUniformUniformBuffersEnabled();
    template<UnsignedInt dimensions> void bindBufferUniformBuffersNotEnabled();
    #endif
    template<UnsignedInt dimensions> void bindTexturesInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void bindTextureArraysInvalid();
    #endif
    template<UnsignedInt dimensions> void setAlphaMaskNotEnabled();
    template<UnsignedInt dimensions> void setTextureMatrixNotEnabled();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setTextureLayerNotArray();
    template<UnsignedInt dimensions> void bindTextureTransformBufferNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setObjectIdNotEnabled();
    template<UnsignedInt dimensions> void setWrongJointCountOrId();
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
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderObjectId2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderObjectId3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderSkinning2D();
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderSkinning3D();
    #endif

    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderInstanced2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderInstanced3D();
    #ifndef MAGNUM_TARGET_GLES2
    template<FlatGL2D::Flag flag = FlatGL2D::Flag{}> void renderInstancedSkinning2D();
    template<FlatGL3D::Flag flag = FlatGL3D::Flag{}> void renderInstancedSkinning3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti2D();
    void renderMulti3D();
    void renderMultiSkinning2D();
    void renderMultiSkinning3D();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        Containers::String _testDir;

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
    {"object ID texture", FlatGL2D::Flag::ObjectIdTexture},
    {"object ID texture array", FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays},
    {"object ID texture + instanced texture transformation", FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedTextureOffset},
    {"object ID texture array + instanced texture transformation", FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::InstancedTextureOffset},
    {"instanced object ID texture array + texture transformation", FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedObjectId|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::TextureTransformation},
    {"object ID texture + textured", FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::Textured},
    #endif
    {"instanced transformation", FlatGL2D::Flag::InstancedTransformation},
    {"instanced texture offset", FlatGL2D::Flag::Textured|FlatGL2D::Flag::InstancedTextureOffset},
    #ifndef MAGNUM_TARGET_GLES2
    {"instanced texture array offset + layer", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::InstancedTextureOffset},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructSkinningData[]{
    {"no skinning", {},
        0, 0, 0},
    {"one set", {},
        16, 4, 0},
    {"two partial sets", {},
        32, 2, 3},
    {"secondary set only", {},
        12, 0, 4},
    {"dynamic per-vertex sets", FlatGL2D::Flag::DynamicPerVertexJointCount,
        16, 4, 3},
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {},
        1, 1, 0, 0, 0},
    {"", FlatGL2D::Flag::UniformBuffers,
        1, 1, 0, 0, 0},
    /* SwiftShader has 256 uniform vectors at most, per-draw is 4+1 in 3D case
       and 3+1 in 2D, per-material 2 */
    {"multiple materials, draws", FlatGL2D::Flag::UniformBuffers,
        8, 48, 0, 0, 0},
    {"textured", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured,
        1, 1, 0, 0, 0},
    {"textured + texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"texture arrays + texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"alpha mask", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::AlphaMask,
        1, 1, 0, 0, 0},
    {"object ID", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectId,
        1, 1, 0, 0, 0},
    {"object ID texture", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectIdTexture,
        1, 1, 0, 0, 0},
    {"object ID texture array", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"object ID texture + instanced texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"object ID texture array + instanced texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"instanced object ID texture array + texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedObjectId|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"object ID texture + textured", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::Textured,
        1, 1, 0, 0, 0},
    {"instanced texture array offset + layer", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"skinning", FlatGL2D::Flag::UniformBuffers,
        1, 1, 32, 3, 2},
    {"skinning, dynamic per-vertex sets", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::DynamicPerVertexJointCount,
        1, 1, 32, 3, 4},
    {"multidraw with all the things except secondary per-vertex sets", FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::ObjectId|FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::InstancedObjectId|FlatGL2D::Flag::DynamicPerVertexJointCount,
        8, 48, 16, 4, 0},
    {"multidraw with all the things except instancing", FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::ObjectId|FlatGL2D::Flag::DynamicPerVertexJointCount,
        8, 48, 16, 3, 4},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with all the things except secondary per-vertex sets", FlatGL2D::Flag::ShaderStorageBuffers|FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::ObjectId|FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::InstancedObjectId|FlatGL2D::Flag::DynamicPerVertexJointCount,
        0, 0, 0, 4, 0},
    {"shader storage + multidraw with all the things except instancing", FlatGL2D::Flag::ShaderStorageBuffers|FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::AlphaMask|FlatGL2D::Flag::ObjectId|FlatGL2D::Flag::DynamicPerVertexJointCount,
        0, 0, 0, 3, 4}
    #endif
};
#endif

constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    const char* message;
} ConstructInvalidData[]{
    {"texture transformation but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        FlatGL2D::Flag::TextureTransformation
            #ifndef MAGNUM_TARGET_GLES2
            |FlatGL2D::Flag::ObjectId
            #endif
            ,
        0, 0, 0,
        "texture transformation enabled but the shader is not textured"},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::ObjectId,
        0, 0, 0,
        "texture arrays enabled but the shader is not textured"},
    {"dynamic per-vertex joint count but no static per-vertex joint count",
        FlatGL2D::Flag::DynamicPerVertexJointCount,
        0, 0, 0,
        "dynamic per-vertex joint count enabled for zero joints"},
    {"instancing together with secondary per-vertex sets",
        FlatGL2D::Flag::InstancedTransformation,
        10, 4, 1,
        "TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount, materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    /* These two fail for UBOs but not SSBOs */
    {"zero draws",
        FlatGL2D::Flag::UniformBuffers,
        0, 0, 0, 1, 0,
        "draw count can't be zero"},
    {"zero materials",
        FlatGL2D::Flag::UniformBuffers,
        0, 0, 0, 0, 1,
        "material count can't be zero"},
    {"texture arrays but no transformation",
        FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        0, 0, 0, 1, 1,
        "texture arrays require texture transformation enabled as well if uniform buffers are used"},
    /* These two fail for UBOs but not SSBOs */
    {"per-vertex joint count but no joint count",
        FlatGL2D::Flag::UniformBuffers,
        0, 2, 0, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
    {"secondary per-vertex joint count but no joint count", FlatGL2D::Flag::UniformBuffers,
        0, 0, 3, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
};
#endif

constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    const char* message;
} BindTexturesInvalidData[]{
    {"not textured",
        FlatGL2D::Flags{}
            #ifndef MAGNUM_TARGET_GLES2
            /* ObjectId shares bits with ObjectIdTexture but should still
               trigger the assert */
            |FlatGL2D::Flag::ObjectId
            #endif
            ,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texturing enabled\n"
        #ifndef MAGNUM_TARGET_GLES2
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled\n"
        #endif
        },
    #ifndef MAGNUM_TARGET_GLES2
    {"array",
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays,
        "Shaders::FlatGL::bindTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    FlatGL2D::Flags flags;
    const char* message;
} BindTextureArraysInvalidData[]{
    {"not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        FlatGL2D::Flag::ObjectId,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texturing enabled\n"
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled\n"},
    {"not array",
        FlatGL2D::Flag::Textured|FlatGL2D::Flag::ObjectIdTexture,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"}
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

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    UnsignedInt expected[4];
    FlatGL2D::Flags flags;
    Matrix3 textureTransformation;
    Int layer;
} RenderObjectIdData[]{
    {"",
        {40006, 40006, 40006, 40006},
        {}, {}, 0},
    {"textured",
        {40106, 40206, 40306, 40406},
        FlatGL2D::Flag::ObjectIdTexture, {}, 0},
    {"textured, texture transformation",
        {40406, 40306, 40206, 40106},
        FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), 0},
    {"texture array, first layer",
        {40106, 40206, 40306, 40406},
        FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays,
        {}, 0},
    {"texture array, arbitrary layer",
        {40106, 40206, 40306, 40406},
        FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays,
        {}, 6},
    {"texture array, texture transformation, arbitrary layer",
        {40406, 40306, 40206, 40106},
        FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::TextureArrays,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), 6},
};
#endif

#ifndef MAGNUM_TARGET_GLES2
/* Same as in PhongGL and MeshVisualizerGL tests */
const struct {
    const char* name;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    UnsignedInt dynamicPerVertexJointCount, dynamicSecondaryPerVertexJointCount;
    FlatGL2D::Flags flags;
    Containers::Array<Containers::Pair<UnsignedInt, GL::DynamicAttribute>> attributes;
    bool setDynamicPerVertexJointCount, setJointMatrices, setJointMatricesOneByOne;
    const char* expected;
} RenderSkinningData[]{
    {"no skinning", 0, 0, 0, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, false, false, false,
        "skinning-default.tga"},
    {"default joint matrices", 5, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, false, false, false,
        "skinning-default.tga"},
    {"single set", 5, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, upload just a prefix of joint matrices", 15, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, upload joint matrices one by one", 5, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, false, true, true,
        "skinning.tga"},
    {"single set, dynamic, left at defaults", 5, 3, 0, 0, 0, FlatGL2D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, dynamic", 5, 4, 0, 3, 0, FlatGL2D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::Three}},
        }}, true, true, false,
        "skinning.tga"},
    {"two sets", 5, 1, 2, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::One}},
            {4, FlatGL2D::SecondaryJointIds{FlatGL2D::SecondaryJointIds::Components::Two}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::One}},
            {4*4, FlatGL2D::SecondaryWeights{FlatGL2D::SecondaryWeights::Components::Two}},
        }}, false, true, false,
        "skinning.tga"},
    {"two sets, dynamic, left at defaults", 5, 1, 2, 0, 0, FlatGL2D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::One}},
            {4, FlatGL2D::SecondaryJointIds{FlatGL2D::SecondaryJointIds::Components::Two}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::One}},
            {4*4, FlatGL2D::SecondaryWeights{FlatGL2D::SecondaryWeights::Components::Two}},
        }}, false, true, false,
        "skinning.tga"},
    {"two sets, dynamic", 5, 4, 4, 1, 2, FlatGL2D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::One}},
            {4, FlatGL2D::SecondaryJointIds{FlatGL2D::SecondaryJointIds::Components::Two}},
            {3*4, FlatGL2D::Weights{FlatGL2D::Weights::Components::One}},
            {4*4, FlatGL2D::SecondaryWeights{FlatGL2D::SecondaryWeights::Components::Two}},
        }}, true, true, false,
        "skinning.tga"},
    {"only secondary set", 5, 0, 3, 0, 0, {}, {InPlaceInit, {
            {0, FlatGL2D::SecondaryJointIds{FlatGL2D::SecondaryJointIds::Components::Three}},
            {3*4, FlatGL2D::SecondaryWeights{FlatGL2D::SecondaryWeights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"only secondary set, dynamic", 5, 4, 4, 0, 3, FlatGL2D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            #ifdef MAGNUM_TARGET_WEBGL
            /* On WebGL the primary joint vertex attribute has to be bound to
               something even if not (dynamically) used in the end, otherwise
               it causes an error. So just alias it with the secondary one. */
            {0, FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three}},
            #endif
            {0, FlatGL2D::SecondaryJointIds{FlatGL2D::SecondaryJointIds::Components::Three}},
            {3*4, FlatGL2D::SecondaryWeights{FlatGL2D::SecondaryWeights::Components::Three}},
        }}, true, true, false,
        "skinning.tga"},
};
#endif

constexpr struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    UnsignedInt expectedId[3];
    FlatGL2D::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData[] {
    {"colored",
        "instanced2D.tga", "instanced3D.tga", {},
        {},
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    #ifndef MAGNUM_TARGET_GLES2
    {"colored + object ID",
        "instanced2D.tga", "instanced3D.tga", {1000, 1000, 1000},
        FlatGL2D::Flag::ObjectId,
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    {"colored + instanced object ID",
        "instanced2D.tga", "instanced3D.tga", {1211, 5627, 36363},
        FlatGL2D::Flag::InstancedObjectId,
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    {"colored + textured object ID",
        "instanced2D.tga", "instanced3D.tga", {3000, 4000, 5000},
        FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedTextureOffset,
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    {"colored + instanced textured object ID",
        "instanced2D.tga", "instanced3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::InstancedObjectId|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedTextureOffset,
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    {"colored + instanced textured array object ID",
        "instanced2D.tga", "instanced3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::InstancedObjectId|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::TextureArrays,
        /* Minor differences on SwiftShader */
        164.4f, 0.094f},
    #endif
    {"textured",
        "instanced-textured2D.tga", "instanced-textured3D.tga", {},
        FlatGL2D::Flag::InstancedTextureOffset|FlatGL2D::Flag::Textured,
        /* Minor differences on SwiftShader */
        192.67f, 0.140f},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture array",
        "instanced-textured2D.tga", "instanced-textured3D.tga", {},
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
    UnsignedInt expectedId[3];
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData[] {
    {"bind with offset, colored",
        "multidraw2D.tga", "multidraw3D.tga", {},
        {}, 1, 1, true, 16, 0.0f, 0.0f},
    {"bind with offset, colored + object ID",
        "multidraw2D.tga", "multidraw3D.tga", {1211, 5627, 36363},
        FlatGL2D::Flag::ObjectId,
        1, 1, true, 16, 0.0f, 0.0f},
    {"bind with offset, colored + textured object ID",
        "multidraw2D.tga", "multidraw3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::ObjectIdTexture,
        1, 1, true, 16, 0.0f, 0.0f},
    {"bind with offset, colored + textured array object ID",
        "multidraw2D.tga", "multidraw3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays,
        1, 1, true, 16, 0.0f, 0.0f},
    {"bind with offset, textured",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        2.34f, 0.01f},
    {"bind with offset, texture array",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        1, 1, true, 16,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, texture array, shader storage",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::ShaderStorageBuffers|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        0, 0, true, 16,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    #endif
    {"draw offset, colored",
        "multidraw2D.tga", "multidraw3D.tga", {},
        {},
        2, 3, false, 1, 0.0f, 0.0f},
    {"draw offset, colored + object ID",
        "multidraw2D.tga", "multidraw3D.tga", {1211, 5627, 36363},
        FlatGL2D::Flag::ObjectId,
        2, 3, false, 1, 0.0f, 0.0f},
    {"draw offset, colored + textured object ID",
        "multidraw2D.tga", "multidraw3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::ObjectIdTexture,
        2, 3, false, 1, 0.0f, 0.0f},
    {"draw offset, colored + textured array object ID",
        "multidraw2D.tga", "multidraw3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays,
        2, 3, false, 1, 0.0f, 0.0f},
    {"draw offset, textured",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        2.34f, 0.01f},
    {"draw offset, texture array",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        2, 3, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, texture array, shader storage",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::ShaderStorageBuffers|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        0, 0, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    #endif
    {"multidraw, colored",
        "multidraw2D.tga", "multidraw3D.tga", {},
        FlatGL2D::Flag::MultiDraw,
        2, 3, false, 1, 0.0f, 0.0f},
    {"multidraw, colored + object ID",
        "multidraw2D.tga", "multidraw3D.tga", {1211, 5627, 36363},
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::ObjectId,
        2, 3, false, 1, 0.0f, 0.0f},
    {"multidraw, colored + textured object ID",
        "multidraw2D.tga", "multidraw3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::ObjectIdTexture,
        2, 3, false, 1, 0.0f, 0.0f},
    {"multidraw, colored + textured array object ID",
        "multidraw2D.tga", "multidraw3D.tga", {3211, 8627, 40363},
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::ObjectIdTexture|FlatGL2D::Flag::TextureArrays,
        2, 3, false, 1, 0.0f, 0.0f},
    {"multidraw, textured",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        2.34f, 0.01f},
    {"multidraw, texture array",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        2, 3, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, texture array, shader storage",
        "multidraw-textured2D.tga", "multidraw-textured3D.tga", {},
        FlatGL2D::Flag::ShaderStorageBuffers|FlatGL2D::Flag::MultiDraw|FlatGL2D::Flag::TextureTransformation|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays,
        0, 0, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        65.0f, 0.15f}
    #endif
};

/* Same as in PhongGL and MeshVisualizerGL tests */
const struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount, jointCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
} RenderMultiSkinningData[]{
    {"bind with offset",
        {},
        1, 1, 4, true, 16},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, shader storage",
        FlatGL2D::Flag::ShaderStorageBuffers,
        0, 0, 0, true, 16},
    #endif
    {"draw offset",
        {},
        2, 3, 9, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, shader storage",
        FlatGL2D::Flag::ShaderStorageBuffers,
        0, 0, 0, false, 1},
    #endif
    {"multidraw",
        FlatGL2D::Flag::MultiDraw,
        2, 3, 9, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, shader storage",
        FlatGL2D::Flag::ShaderStorageBuffers|FlatGL2D::Flag::MultiDraw,
        0, 0, 0, false, 1}
    #endif
};
#endif

FlatGLTest::FlatGLTest() {
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::construct<2>,
        &FlatGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::constructSkinning<2>,
        &FlatGLTest::constructSkinning<3>},
        Containers::arraySize(ConstructSkinningData));
    #endif

    addTests<FlatGLTest>({
        &FlatGLTest::constructAsync<2>,
        &FlatGLTest::constructAsync<3>});

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::constructUniformBuffers<2>,
        &FlatGLTest::constructUniformBuffers<3>},
        Containers::arraySize(ConstructUniformBuffersData));

    addTests<FlatGLTest>({
        &FlatGLTest::constructUniformBuffersAsync<2>,
        &FlatGLTest::constructUniformBuffersAsync<3>});
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
        &FlatGLTest::setPerVertexJointCountInvalid<2>,
        &FlatGLTest::setPerVertexJointCountInvalid<3>,
        &FlatGLTest::setUniformUniformBuffersEnabled<2>,
        &FlatGLTest::setUniformUniformBuffersEnabled<3>,
        &FlatGLTest::bindBufferUniformBuffersNotEnabled<2>,
        &FlatGLTest::bindBufferUniformBuffersNotEnabled<3>,
        #endif
    });

    addInstancedTests<FlatGLTest>({
        &FlatGLTest::bindTexturesInvalid<2>,
        &FlatGLTest::bindTexturesInvalid<3>},
        Containers::arraySize(BindTexturesInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::bindTextureArraysInvalid<2>,
        &FlatGLTest::bindTextureArraysInvalid<3>},
        Containers::arraySize(BindTextureArraysInvalidData));
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
        &FlatGLTest::setWrongJointCountOrId<2>,
        &FlatGLTest::setWrongJointCountOrId<3>,
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
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderDefaults2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderDefaults3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderDefaults3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderDefaults3D<FlatGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderColored2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderColored2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderColored2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderColored3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderColored3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderColored3D<FlatGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderSinglePixelTextured2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderSinglePixelTextured2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderSinglePixelTextured2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderSinglePixelTextured3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderSinglePixelTextured3D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderSinglePixelTextured3D<FlatGL2D::Flag::ShaderStorageBuffers>
        #endif
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
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderTextured2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderTextured3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderTextured3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderTextured3D<FlatGL3D::Flag::ShaderStorageBuffers>
        #endif
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
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderVertexColor2D<Color3, FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderVertexColor2D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor2D<Color4, FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderVertexColor2D<Color4, FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderVertexColor3D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor3D<Color3, FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderVertexColor3D<Color3, FlatGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderVertexColor3D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderVertexColor3D<Color4, FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderVertexColor3D<Color4, FlatGL3D::Flag::ShaderStorageBuffers>
        #endif
        #endif
        },
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderAlpha2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderAlpha2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderAlpha2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderAlpha3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderAlpha3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderAlpha3D<FlatGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderAlphaData),
        &FlatGLTest::renderAlphaSetup,
        &FlatGLTest::renderAlphaTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderObjectId2D,
        &FlatGLTest::renderObjectId2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderObjectId2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &FlatGLTest::renderObjectId3D,
        &FlatGLTest::renderObjectId3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderObjectId3D<FlatGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        Containers::arraySize(RenderObjectIdData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderSkinning2D,
        &FlatGLTest::renderSkinning2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderSkinning2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &FlatGLTest::renderSkinning3D,
        &FlatGLTest::renderSkinning3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderSkinning3D<FlatGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        Containers::arraySize(RenderSkinningData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::renderInstanced2D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderInstanced2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderInstanced2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &FlatGLTest::renderInstanced3D,
        #ifndef MAGNUM_TARGET_GLES2
        &FlatGLTest::renderInstanced3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderInstanced3D<FlatGL3D::Flag::ShaderStorageBuffers>
        #endif
        #endif
        },
        Containers::arraySize(RenderInstancedData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<FlatGLTest>({
        &FlatGLTest::renderInstancedSkinning2D,
        &FlatGLTest::renderInstancedSkinning2D<FlatGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderInstancedSkinning2D<FlatGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &FlatGLTest::renderInstancedSkinning3D,
        &FlatGLTest::renderInstancedSkinning3D<FlatGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &FlatGLTest::renderInstancedSkinning3D<FlatGL3D::Flag::ShaderStorageBuffers>
        #endif
        },
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&FlatGLTest::renderMulti2D,
                       &FlatGLTest::renderMulti3D},
        Containers::arraySize(RenderMultiData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);

    addInstancedTests({&FlatGLTest::renderMultiSkinning2D,
                       &FlatGLTest::renderMultiSkinning3D},
        Containers::arraySize(RenderMultiSkinningData),
        &FlatGLTest::renderSetup,
        &FlatGLTest::renderTeardown);
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
    if(Utility::System::isSandboxed()
        #if defined(CORRADE_TARGET_IOS) && defined(CORRADE_TESTSUITE_TARGET_XCTEST)
        /** @todo Fix this once I persuade CMake to run XCTest tests properly */
        && std::getenv("SIMULATOR_UDID")
        #endif
    ) {
        _testDir = Utility::Path::path(*Utility::Path::executableLocation());
    } else
    #endif
    {
        _testDir = SHADERS_TEST_DIR;
    }
}

template<UnsignedInt dimensions> void FlatGLTest::construct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructSkinning() {
    auto&& data = ConstructSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.jointCount(), data.jointCount);
    CORRADE_COMPARE(shader.perVertexJointCount(), data.perVertexJointCount);
    CORRADE_COMPARE(shader.secondaryPerVertexJointCount(), data.secondaryPerVertexJointCount);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::constructAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename FlatGL<dimensions>::CompileState state = FlatGL<dimensions>::compile(typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation)
        /* Skinning properties tested in constructUniformBuffersAsync(), as
           there we don't need to bother with ES2 */
    );
    CORRADE_COMPARE(state.flags(),  FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    FlatGL<dimensions> shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation);

    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::ObjectId || data.jointCount) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= FlatGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
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

    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.materialCount(), data.materialCount);
    CORRADE_COMPARE(shader.drawCount(), data.drawCount);
    CORRADE_COMPARE(shader.jointCount(), data.jointCount);
    CORRADE_COMPARE(shader.perVertexJointCount(), data.perVertexJointCount);
    CORRADE_COMPARE(shader.secondaryPerVertexJointCount(), data.secondaryPerVertexJointCount);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void FlatGLTest::constructUniformBuffersAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    typename FlatGL<dimensions>::CompileState state = FlatGL<dimensions>::compile(typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::AlphaMask)
        .setMaterialCount(5)
        .setDrawCount(36)
        .setJointCount(7, 3, 4));
    CORRADE_COMPARE(state.flags(), FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::AlphaMask);
    CORRADE_COMPARE(state.materialCount(), 5);
    CORRADE_COMPARE(state.drawCount(), 36);
    CORRADE_COMPARE(state.jointCount(), 7);
    CORRADE_COMPARE(state.perVertexJointCount(), 3);
    CORRADE_COMPARE(state.secondaryPerVertexJointCount(), 4);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    FlatGL<dimensions> shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::AlphaMask);
    CORRADE_COMPARE(shader.materialCount(), 5);
    CORRADE_COMPARE(shader.drawCount(), 36);
    CORRADE_COMPARE(shader.jointCount(), 7);
    CORRADE_COMPARE(shader.perVertexJointCount(), 3);
    CORRADE_COMPARE(shader.secondaryPerVertexJointCount(), 4);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#endif

template<UnsignedInt dimensions> void FlatGLTest::constructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    FlatGL<dimensions> a{typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL<dimensions>::Flag::Textured)
        /* Skinning properties tested in constructMoveUniformBuffers(), as
           there we don't need to bother with ES2 */
    };
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    FlatGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), FlatGL<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!a.id());

    FlatGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), FlatGL<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructMoveUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    FlatGL<dimensions> a{typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)
        .setJointCount(16, 4, 3)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    FlatGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), FlatGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_COMPARE(b.jointCount(), 16);
    CORRADE_COMPARE(b.perVertexJointCount(), 4);
    CORRADE_COMPARE(b.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!a.id());

    FlatGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), FlatGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_COMPARE(c.jointCount(), 16);
    CORRADE_COMPARE(c.perVertexJointCount(), 4);
    CORRADE_COMPARE(c.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!b.id());
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::constructInvalid() {
    auto&& data = ConstructInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::format("{}", dimensions));
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    FlatGL<dimensions>{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        #ifndef MAGNUM_TARGET_GLES2
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        #endif
    };
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::FlatGL: {}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::constructUniformBuffersInvalid() {
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
    FlatGL<dimensions>{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::FlatGL: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setPerVertexJointCountInvalid() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    FlatGL<dimensions> a{typename FlatGL<dimensions>::Configuration{}};
    FlatGL<dimensions> b{typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL<dimensions>::Flag::DynamicPerVertexJointCount)
        .setJointCount(16, 3, 2)};

    Containers::String out;
    Error redirectError{&out};
    a.setPerVertexJointCount(3, 2);
    b.setPerVertexJointCount(4);
    b.setPerVertexJointCount(3, 3);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled\n"
        "Shaders::FlatGL::setPerVertexJointCount(): expected at most 3 per-vertex joints, got 4\n"
        "Shaders::FlatGL::setPerVertexJointCount(): expected at most 2 secondary per-vertex joints, got 3\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setUniformUniformBuffersEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL<dimensions>::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader
        /* setPerVertexJointCount() works on both UBOs and classic */
        .setTransformationProjectionMatrix({})
        .setTextureMatrix({})
        .setTextureLayer({})
        .setColor({})
        .setAlphaMask({})
        .setObjectId({})
        .setJointMatrices({})
        .setJointMatrix(0, {})
        .setPerInstanceJointCount(0);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setTextureMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setTextureLayer(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setAlphaMask(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setObjectId(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setJointMatrices(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setJointMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::FlatGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void FlatGLTest::bindBufferUniformBuffersNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Buffer buffer;
    FlatGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.bindTransformationProjectionBuffer(buffer)
          .bindTransformationProjectionBuffer(buffer, 0, 16)
          .bindDrawBuffer(buffer)
          .bindDrawBuffer(buffer, 0, 16)
          .bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16)
          .bindMaterialBuffer(buffer)
          .bindMaterialBuffer(buffer, 0, 16)
          .bindJointBuffer(buffer)
          .bindJointBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::FlatGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::bindTexturesInvalid() {
    auto&& data = BindTexturesInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::format("{}", dimensions));
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL<dimensions>::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTexture(texture);
    #ifndef MAGNUM_TARGET_GLES2
    shader.bindObjectIdTexture(texture);
    #endif
    CORRADE_COMPARE(out, data.message);
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::bindTextureArraysInvalid() {
    auto&& data = BindTextureArraysInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::format("{}", dimensions));
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2DArray textureArray;
    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(data.flags)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTexture(textureArray);
    shader.bindObjectIdTexture(textureArray);
    CORRADE_COMPARE(out, data.message);
}
#endif

template<UnsignedInt dimensions> void FlatGLTest::setAlphaMaskNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    FlatGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setAlphaMask(0.75f);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

template<UnsignedInt dimensions> void FlatGLTest::setTextureMatrixNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    FlatGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureMatrix({});
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setTextureLayerNotArray() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    FlatGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureLayer(37);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::bindTextureTransformBufferNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    GL::Buffer buffer{GL::Buffer::TargetHint::Uniform};
    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL<dimensions>::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setObjectIdNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    FlatGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setObjectId(33376);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setWrongJointCountOrId() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setJointCount(5, 1)};

    Containers::String out;
    Error redirectError{&out};
    /* Calling setJointMatrices() with less items is fine, tested in
       renderSkinning*D() */
    shader.setJointMatrices({{}, {}, {}, {}, {}, {}})
        .setJointMatrix(5, MatrixTypeFor<dimensions, Float>{});
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setJointMatrices(): expected at most 5 items but got 6\n"
        "Shaders::FlatGL::setJointMatrix(): joint ID 5 is out of range for 5 joints\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void FlatGLTest::setWrongDrawOffset() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    FlatGL<dimensions> shader{typename FlatGL<dimensions>::Configuration{}
        .setFlags(FlatGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::FlatGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
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

    #ifndef MAGNUM_TARGET_GLES2
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
                {FlatGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}}
                /* ObjectIdOutput is mapped (and cleared) in test cases that
                   actually draw to it, otherwise it causes an error on WebGL
                   due to the shader not rendering to all outputs */
            });
    }
    #endif
}

void FlatGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    _objectId = GL::Renderbuffer{NoCreate};
    #endif
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderDefaults2D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(flag)};

    if(flag == FlatGL2D::Flag{}) {
        shader.draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/defaults.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderDefaults3D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(flag)};

    if(flag == FlatGL3D::Flag{}) {
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/defaults.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderColored2D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(flag)};

    if(flag == FlatGL2D::Flag{}) {
        shader
            .setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/colored2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderColored3D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(flag)};

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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/colored3D.tga"),
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
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & FlatGL2D::Flag::TextureTransformation ? 3 : 2))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    if(flag & FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL2D::Flag::TextureArrays) && !(data.flags & FlatGL2D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL2D::Flag::TextureTransformation;
    }
    #endif
    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(flags)};

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
        if(!(flag & FlatGL2D::Flag::UniformBuffers) && data.layer != 0)
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/colored2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderSinglePixelTextured3D() {
    auto&& data = RenderSinglePixelTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & FlatGL2D::Flag::TextureTransformation ? 3 : 2))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    if(flag & FlatGL3D::Flag::UniformBuffers && (data.flags & FlatGL3D::Flag::TextureArrays) && !(data.flags & FlatGL3D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL3D::Flag::TextureTransformation;
    }
    #endif
    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(flags)};

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
        if(!(flag & FlatGL2D::Flag::UniformBuffers) && data.layer != 0)
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/colored3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderTextured2D() {
    auto&& data = RenderTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & FlatGL2D::Flag::TextureTransformation ? 3 : 2))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

    FlatGL2D::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL2D::Flag::TextureArrays) && !(data.flags & FlatGL2D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL2D::Flag::TextureTransformation;
    }
    #endif
    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(flags)};

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
        if(!(flag & FlatGL2D::Flag::UniformBuffers) && data.layer != 0)
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        rendered.pixels<Color4ub>().slice(&Color4ub::rgb);
    if(data.flip) pixels = pixels.flipped<0>().flipped<1>();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has minor rounding errors, Apple A8 & llvmpipe a bit more */
    const Float maxThreshold = 2.334f, meanThreshold = 0.032f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.667f, meanThreshold = 3.254f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Path::join(_testDir, "FlatTestFiles/textured2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderTextured3D() {
    auto&& data = RenderTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & FlatGL2D::Flag::TextureTransformation ? 3 : 2))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

    FlatGL3D::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL3D::Flag::TextureArrays) && !(data.flags & FlatGL3D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL3D::Flag::TextureTransformation;
    }
    #endif
    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(flags)};

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
        if(!(flag & FlatGL3D::Flag::UniformBuffers) && data.layer != 0)
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        rendered.pixels<Color4ub>().slice(&Color4ub::rgb);
    if(data.flip) pixels = pixels.flipped<0>().flipped<1>();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 5 different pixels on the edges */
    const Float maxThreshold = 139.0f, meanThreshold = 0.087f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 139.0f, meanThreshold = 2.896f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Path::join(_testDir, "FlatTestFiles/textured3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, FlatGL2D::Flag flag> void FlatGLTest::renderVertexColor2D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(FlatGL2D::Flag::Textured|FlatGL2D::Flag::VertexColor|flag)};
    shader.bindTexture(texture);

    if(flag == FlatGL2D::Flag{}) {
        shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .setColor(0x9999ff_rgbf)
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/vertexColor2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, FlatGL2D::Flag flag> void FlatGLTest::renderVertexColor3D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(FlatGL3D::Flag::Textured|FlatGL3D::Flag::VertexColor|flag)};
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/vertexColor3D.tga"),
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
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    CORRADE_VERIFY(importer->openFile(Utility::Path::join({_testDir, "TestFiles", "diffuse-alpha-texture.tga"})) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, image->size())
        .setSubImage(0, {}, *image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates));

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(data.flags|flag)};
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, data.expected2D),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderAlpha3D() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
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
    CORRADE_VERIFY(importer->openFile(Utility::Path::join({_testDir, "TestFiles", "diffuse-alpha-texture.tga"})) && (image = importer->image2D(0)));
    texture.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, image->size())
        .setSubImage(0, {}, *image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(data.flags|flag)};
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
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
template<FlatGL2D::Flag flag> void FlatGLTest::renderObjectId2D() {
    auto&& data = RenderObjectIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL2D::Flag::UniformBuffers) {
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

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    Primitives::Circle2DFlags circleFlags;
    if(data.flags & FlatGL2D::Flag::ObjectIdTexture)
        circleFlags |= Primitives::Circle2DFlag::TextureCoordinates;
    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32, circleFlags));

    FlatGL2D::Flags flags = data.flags|flag;
    if(flag & FlatGL2D::Flag::UniformBuffers && (data.flags & FlatGL2D::Flag::TextureArrays) && !(data.flags & FlatGL2D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL2D::Flag::TextureTransformation;
    }
    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(FlatGL2D::Flag::ObjectId|flags)};

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags >= FlatGL2D::Flag::ObjectIdTexture) {
        const UnsignedShort imageData[]{
            100, 200, 300, 400
        };
        ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, {image.size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, image);
            shader.bindObjectIdTexture(textureArray);
            if(!(flag & FlatGL2D::Flag::UniformBuffers) && data.layer != 0)
                shader.setTextureLayer(data.layer); /* to verify the default */
        } else {
            texture = GL::Texture2D{};
            texture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(texture);
        }
    }

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    _framebuffer
        .mapForDraw({
            {FlatGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {FlatGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == FlatGL2D::Flag{}) {
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader.setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .setObjectId(40006)
            .draw(circle);
    } else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setObjectId(40006)
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
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Color output should have no difference -- same as in colored2D() */
    const Float maxThreshold = 0.0f, meanThreshold = 0.0f;
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/colored2D.tga"),
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
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[30][30], data.expected[0]);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[30][50], data.expected[1]);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[50][30], data.expected[2]);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[50][50], data.expected[3]);
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderObjectId3D() {
    auto&& data = RenderObjectIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL3D::Flag::UniformBuffers) {
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

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL3D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    Primitives::UVSphereFlags sphereFlags;
    if(data.flags & FlatGL3D::Flag::ObjectIdTexture)
        sphereFlags |= Primitives::UVSphereFlag::TextureCoordinates;
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32, sphereFlags));

    FlatGL3D::Flags flags = data.flags|flag;
    if(flag & FlatGL3D::Flag::UniformBuffers && (data.flags & FlatGL3D::Flag::TextureArrays) && !(data.flags & FlatGL3D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= FlatGL3D::Flag::TextureTransformation;
    }
    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(FlatGL3D::Flag::ObjectId|flags)};

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::ObjectIdTexture) {
        const UnsignedShort imageData[]{
            100, 200, 300, 400
        };
        ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, {image.size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, image);
            shader.bindObjectIdTexture(textureArray);
            if(!(flag & FlatGL2D::Flag::UniformBuffers) && data.layer != 0)
                shader.setTextureLayer(data.layer); /* to verify the default */
        } else {
            texture = GL::Texture2D{};
            texture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(texture);
        }
    }

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    _framebuffer
        .mapForDraw({
            {FlatGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {FlatGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == FlatGL3D::Flag{}) {
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader.setColor(0x9999ff_rgbf)
            .setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setObjectId(40006)
            .draw(sphere);
    } else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
                .setObjectId(40006)
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/colored3D.tga"),
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
    /* Inside of the object. It's a sphere and the seam is at the front,
       rotated to bottom left, meaning left is actually the right part of the
       texture and right is the left part of the texture. */
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][50], data.expected[0]);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[20][20], data.expected[1]);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[50][50], data.expected[2]);
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[50][20], data.expected[3]);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<FlatGL2D::Flag flag> void FlatGLTest::renderSkinning2D() {
    auto&& data = RenderSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(data.jointCount && GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (joint) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }

    /* Same as in PhongGLTest::renderSkinning(), except in 2D, and same as in
       MeshVisualizerGLTest::renderSkinning2D() */
    struct Vertex {
        Vector2 position;
        UnsignedInt jointIds[3];
        Float weights[3];
    } vertices[]{
        /* Top right corner gets moved to the right and up, top left just up,
           bottom right just right, bottom left corner gets slightly scaled.

           3--1
           | /|
           |/ |
           2--0 */
        {{ 1.0f, -1.0f}, {0, 2, 0}, {1.0f, 50.0f, 0.5f}},
        {{ 1.0f,  1.0f}, {1, 0, 0}, {0.5f, 0.5f, 0.0f}},
        {{-1.0f, -1.0f}, {3, 4, 4}, {0.5f, 0.25f, 0.25f}},
        {{-1.0f,  1.0f}, {1, 0, 4}, {1.0f, 0.0f, 0.0f}},
    };

    Matrix3 jointMatrices[]{
        Matrix3::translation(Vector2::xAxis(0.5f)),
        Matrix3::translation(Vector2::yAxis(0.5f)),
        Matrix3{Math::ZeroInit},
        Matrix3::scaling(Vector2{2.0f}),
        Matrix3{Math::IdentityInit},
    };

    #ifdef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::UniformBuffers && data.jointCount > Containers::arraySize(jointMatrices))
        CORRADE_SKIP("Uploading an uniform buffer smaller than the size hardcoded in the shader is an error in WebGL.");
    #endif

    GL::Buffer buffer{vertices};

    GL::Mesh mesh{MeshPrimitive::TriangleStrip};
    mesh.setCount(4);
    mesh.addVertexBuffer(buffer, 0, sizeof(Vertex), GL::DynamicAttribute{FlatGL2D::Position{}});
    for(auto&& attribute: data.attributes)
        mesh.addVertexBuffer(buffer, 2*4 + attribute.first(), sizeof(Vertex), attribute.second());

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(data.flags|flag)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    if(data.setDynamicPerVertexJointCount)
        shader.setPerVertexJointCount(data.dynamicPerVertexJointCount, data.dynamicSecondaryPerVertexJointCount);

    if(flag == FlatGL2D::Flag{}) {
        if(data.setJointMatricesOneByOne) {
            shader
                .setJointMatrix(0, jointMatrices[0])
                .setJointMatrix(1, jointMatrices[1])
                .setJointMatrix(2, jointMatrices[2])
                .setJointMatrix(3, jointMatrices[3])
                .setJointMatrix(4, jointMatrices[4]);
        } else if(data.setJointMatrices)
            shader.setJointMatrices(jointMatrices);
        shader
            .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{0.5f}))
            .draw(mesh);
    } else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{0.5f}))
        }};
        GL::Buffer jointMatricesUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform2D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[0] : Matrix3{}),
            TransformationUniform2D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[1] : Matrix3{}),
            TransformationUniform2D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[2] : Matrix3{}),
            TransformationUniform2D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[3] : Matrix3{}),
            TransformationUniform2D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[4] : Matrix3{}),
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
            .bindJointBuffer(jointMatricesUniform)
            .draw(mesh);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "TestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager}));
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderSkinning3D() {
    auto&& data = RenderSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(data.jointCount && GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (joint) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }

    /* Same as in PhongGLTest::renderSkinning() */
    struct Vertex {
        Vector3 position;
        UnsignedInt jointIds[3];
        Float weights[3];
    } vertices[]{
        /* Top right corner gets moved to the right and up, top left just up,
           bottom right just right, bottom left corner gets slightly scaled.

           3--1
           | /|
           |/ |
           2--0 */
        {{ 1.0f, -1.0f, 0.0f}, {0, 2, 0}, {1.0f, 50.0f, 0.5f}},
        {{ 1.0f,  1.0f, 0.0f}, {1, 0, 0}, {0.5f, 0.5f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {3, 4, 4}, {0.5f, 0.25f, 0.25f}},
        {{-1.0f,  1.0f, 0.0f}, {1, 0, 4}, {1.0f, 0.0f, 0.0f}},
    };

    Matrix4 jointMatrices[]{
        Matrix4::translation(Vector3::xAxis(0.5f)),
        Matrix4::translation(Vector3::yAxis(0.5f)),
        Matrix4{Math::ZeroInit},
        Matrix4::scaling(Vector3{2.0f}),
        Matrix4{Math::IdentityInit},
    };

    #ifdef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL3D::Flag::UniformBuffers && data.jointCount > Containers::arraySize(jointMatrices))
        CORRADE_SKIP("Uploading an uniform buffer smaller than the size hardcoded in the shader is an error in WebGL.");
    #endif

    GL::Buffer buffer{vertices};

    GL::Mesh mesh{MeshPrimitive::TriangleStrip};
    mesh.setCount(4);
    mesh.addVertexBuffer(buffer, 0, sizeof(Vertex), GL::DynamicAttribute{FlatGL3D::Position{}});
    for(auto&& attribute: data.attributes)
        mesh.addVertexBuffer(buffer, 3*4 + attribute.first(), sizeof(Vertex), attribute.second());

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(data.flags|flag)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    if(data.setDynamicPerVertexJointCount)
        shader.setPerVertexJointCount(data.dynamicPerVertexJointCount, data.dynamicSecondaryPerVertexJointCount);

    if(flag == FlatGL3D::Flag{}) {
        if(data.setJointMatricesOneByOne) {
            shader
                .setJointMatrix(0, jointMatrices[0])
                .setJointMatrix(1, jointMatrices[1])
                .setJointMatrix(2, jointMatrices[2])
                .setJointMatrix(3, jointMatrices[3])
                .setJointMatrix(4, jointMatrices[4]);
        } else if(data.setJointMatrices)
            shader.setJointMatrices(jointMatrices);
        shader
            .setTransformationProjectionMatrix(Matrix4::scaling(Vector3{0.5f}))
            .draw(mesh);
    } else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(Matrix4::scaling(Vector3{0.5f}))
        }};
        GL::Buffer jointMatricesUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[0] : Matrix4{}),
            TransformationUniform3D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[1] : Matrix4{}),
            TransformationUniform3D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[2] : Matrix4{}),
            TransformationUniform3D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[3] : Matrix4{}),
            TransformationUniform3D{}
                .setTransformationMatrix(data.setJointMatrices ?
                    jointMatrices[4] : Matrix4{}),
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
            .bindJointBuffer(jointMatricesUniform)
            .draw(mesh);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "TestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager}));
}
#endif

template<FlatGL2D::Flag flag> void FlatGLTest::renderInstanced2D() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
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
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceData}, 1, 0,
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

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(FlatGL2D::Flag::VertexColor|
        FlatGL2D::Flag::InstancedTransformation|data.flags|flag)};

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
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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

    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= FlatGL2D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            /* 2 extra slices as a base offset, each slice has half height,
               second slice has the data in the right half */
            const UnsignedShort imageData[]{
                0, 0,
                0, 0,
                2000, 0,
                0, 3000,
                4000, 0
            };
            ImageView3D image{PixelFormat::R16UI, {2, 1, 5}, imageData};

            objectIdTextureArray = GL::Texture2DArray{};
            objectIdTextureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTextureArray);
        } else {
            /* First is taken from bottom left, second from bottom right, third
               from top center (there I just duplicate the pixel on both
               sides) */
            const UnsignedShort imageData[]{
                2000, 3000,
                4000, 4000
            };
            ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

            objectIdTexture = GL::Texture2D{};
            objectIdTexture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTexture);
        }
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & FlatGL2D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {FlatGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {FlatGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});
    #endif

    if(flag == FlatGL2D::Flag{}) {
        shader
            .setColor(data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf)
            .setTransformationProjectionMatrix(
                Matrix3::projection({2.1f, 2.1f})*
                Matrix3::scaling(Vector2{0.4f}));

        if(data.flags & FlatGL2D::Flag::TextureTransformation)
            shader.setTextureMatrix(Matrix3::scaling(
                #ifndef MAGNUM_TARGET_GLES2
                /* Slices of the texture array have half the height */
                data.flags & FlatGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                #endif
                Vector2{0.5f}
            ));

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & FlatGL2D::Flag::TextureArrays)
            shader.setTextureLayer(2); /* base offset */
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & FlatGL2D::Flag::ObjectId) {
            /* Gets added to the per-instance ID, if that's enabled as well */
            shader.setObjectId(1000);
        }
        #endif

        shader.draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
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
            FlatDrawUniform{}
                /* Gets added to the per-instance ID, if that's enabled as
                   well */
                .setObjectId(1000)
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
        if(data.flags & FlatGL2D::Flag::TextureTransformation)
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "FlatTestFiles", data.expected2D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    #ifndef MAGNUM_TARGET_GLES2
    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & FlatGL2D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();

        /* If instanced object IDs are enabled, the per-instance ID gets added
           to the output as well */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], data.expectedId[0]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], data.expectedId[1]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], data.expectedId[2]);
    }
    #endif
}

template<FlatGL3D::Flag flag> void FlatGLTest::renderInstanced3D() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL3D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
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
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceData}, 1, 0,
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

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(FlatGL3D::Flag::VertexColor|
        FlatGL3D::Flag::InstancedTransformation|data.flags|flag)};

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
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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

    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= FlatGL3D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & FlatGL3D::Flag::TextureArrays) {
            /* 2 extra slices as a base offset, each slice has half height,
               second slice has the data in the right half */
            const UnsignedShort imageData[]{
                0, 0,
                0, 0,
                2000, 0,
                0, 3000,
                4000, 0
            };
            ImageView3D image{PixelFormat::R16UI, {2, 1, 5}, imageData};

            objectIdTextureArray = GL::Texture2DArray{};
            objectIdTextureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTextureArray);
        } else {
            /* First is taken from bottom left, second from bottom right, third
               from top center (there I just duplicate the pixel on both
               sides) */
            const UnsignedShort imageData[]{
                2000, 3000,
                4000, 4000
            };
            ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

            objectIdTexture = GL::Texture2D{};
            objectIdTexture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTexture);
        }
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & FlatGL3D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {FlatGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {FlatGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});
    #endif

    if(flag == FlatGL3D::Flag{}) {
        shader
            .setColor(data.flags & FlatGL2D::Flag::Textured ? 0xffffff_rgbf : 0xffff00_rgbf)
            .setTransformationProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::scaling(Vector3{0.4f}));

        if(data.flags & FlatGL3D::Flag::TextureTransformation)
            shader.setTextureMatrix(Matrix3::scaling(
                #ifndef MAGNUM_TARGET_GLES2
                /* Slices of the texture array have half the height */
                data.flags & FlatGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                #endif
                Vector2{0.5f}
            ));

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & FlatGL3D::Flag::TextureArrays)
            shader.setTextureLayer(2); /* base offset */
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & FlatGL3D::Flag::ObjectId) {
            /* Gets added to the per-instance ID, if that's enabled as well */
            shader.setObjectId(1000);
        }
        #endif

        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
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
                /* Gets added to the per-instance ID, if that's enabled as
                   well */
                .setObjectId(1000)
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
        if(data.flags & FlatGL3D::Flag::TextureTransformation)
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "FlatTestFiles", data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    #ifndef MAGNUM_TARGET_GLES2
    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & FlatGL3D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], data.expectedId[0]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], data.expectedId[1]);
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], data.expectedId[2]);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
template<FlatGL2D::Flag flag> void FlatGLTest::renderInstancedSkinning2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (joint) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }

    /* Similarly to renderSkinning2D() tests just 2D movement, differently and
       clearly distinguisable for each instance */
    struct Vertex {
        Vector2 position;
        UnsignedInt jointIds[3];
        Float weights[3];
    } vertices[]{
        /* Each corner affected by exactly one matrix, but at different item
           in the array

           3--1
           | /|
           |/ |
           2--0 */
        {{ 1.0f, -1.0f}, {0, 0, 0}, {1.0f, 0.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {0, 3, 0}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f}, {0, 0, 1}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f,  1.0f}, {4, 0, 0}, {1.0f, 0.0f, 0.0f}},
    };

    Matrix3 instanceTransformations[]{
        Matrix3::translation({-1.5f, -1.5f}),
        Matrix3::translation({ 1.5f, -1.5f}),
        Matrix3::translation({ 0.0f,  1.5f})
    };

    Matrix3 jointMatrices[]{
        /* First instance moves bottom left corner */
        {},
        Matrix3::translation({-0.5f, -0.5f}),
        {},
        {},
        {},

        /* Second instance moves bottom right corner */
        Matrix3::translation({0.5f, -0.5f}),
        {},
        {},
        {},
        {},

        /* Third instance moves both top corners */
        {},
        {},
        {},
        Matrix3::translation({0.5f, 0.5f}),
        Matrix3::translation({-0.5f, 0.5f}),
    };

    GL::Mesh mesh{MeshPrimitive::TriangleStrip};
    mesh.setCount(4)
        .addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, vertices}, 0,
            FlatGL2D::Position{},
            FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Three},
            FlatGL2D::Weights{FlatGL2D::Weights::Components::Three})
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceTransformations}, 1, 0,
            FlatGL2D::TransformationMatrix{})
        .setInstanceCount(3);

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(FlatGL2D::Flag::InstancedTransformation|flag)
        .setJointCount(15, 3, 0)};

    if(flag == FlatGL2D::Flag{}) {
        shader
            .setJointMatrices(jointMatrices)
            .setPerInstanceJointCount(5)
            .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{0.3f}))
            .draw(mesh);
    } else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{0.3f}))
        }};
        TransformationUniform2D jointMatricesUniformData[Containers::arraySize(jointMatrices)];
        Utility::copy( /* This API is so powerful it should be outlawed!! */
            Containers::arrayCast<2, const Vector3>(Containers::stridedArrayView(jointMatrices)), Containers::arrayCast<2, Vector4>(Containers::stridedArrayView(jointMatricesUniformData).slice(&TransformationUniform2D::transformationMatrix)).slice(&Vector4::xyz));
        GL::Buffer jointMatricesUniform{GL::Buffer::TargetHint::Uniform,
            jointMatricesUniformData
        };
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setPerInstanceJointCount(5)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindJointBuffer(jointMatricesUniform)
            .draw(mesh);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "TestFiles/skinning-instanced.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

template<FlatGL2D::Flag flag> void FlatGLTest::renderInstancedSkinning3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == FlatGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == FlatGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (joint) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }

    /* Similarly to renderSkinning3D() tests just 2D movement, differently and
       clearly distinguisable for each instance */
    struct Vertex {
        Vector3 position;
        UnsignedInt jointIds[3];
        Float weights[3];
    } vertices[]{
        /* Each corner affected by exactly one matrix, but at different item
           in the array

           3--1
           | /|
           |/ |
           2--0 */
        {{ 1.0f, -1.0f, 0.0f}, {0, 0, 0}, {1.0f, 0.0f, 0.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {0, 3, 0}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0, 0, 1}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f,  1.0f, 0.0f}, {4, 0, 0}, {1.0f, 0.0f, 0.0f}},
    };

    Matrix4 instanceTransformations[]{
        Matrix4::translation({-1.5f, -1.5f, 0.0f}),
        Matrix4::translation({ 1.5f, -1.5f, 0.0f}),
        Matrix4::translation({ 0.0f,  1.5f, 0.0f})
    };

    Matrix4 jointMatrices[]{
        /* First instance moves bottom left corner */
        {},
        Matrix4::translation({-0.5f, -0.5f, 0.0f}),
        {},
        {},
        {},

        /* Second instance moves bottom right corner */
        Matrix4::translation({0.5f, -0.5f, 0.0f}),
        {},
        {},
        {},
        {},

        /* Third instance moves both top corners */
        {},
        {},
        {},
        Matrix4::translation({0.5f, 0.5f, 0.0f}),
        Matrix4::translation({-0.5f, 0.5f, 0.0f}),
    };

    GL::Mesh mesh{MeshPrimitive::TriangleStrip};
    mesh.setCount(4)
        .addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, vertices}, 0,
            FlatGL3D::Position{},
            FlatGL3D::JointIds{FlatGL3D::JointIds::Components::Three},
            FlatGL3D::Weights{FlatGL3D::Weights::Components::Three})
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceTransformations}, 1, 0,
            FlatGL3D::TransformationMatrix{})
        .setInstanceCount(3);

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(FlatGL3D::Flag::InstancedTransformation|flag)
        .setJointCount(15, 3, 0)};

    if(flag == FlatGL3D::Flag{}) {
        shader
            .setJointMatrices(jointMatrices)
            .setPerInstanceJointCount(5)
            .setTransformationProjectionMatrix(Matrix4::scaling(Vector3{0.3f}))
            .draw(mesh);
    } else if(flag == FlatGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == FlatGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(Matrix4::scaling(Vector3{0.3f}))
        }};
        TransformationUniform3D jointMatricesUniformData[Containers::arraySize(jointMatrices)];
        Utility::copy(jointMatrices, Containers::stridedArrayView(jointMatricesUniformData).slice(&TransformationUniform3D::transformationMatrix));
        GL::Buffer jointMatricesUniform{GL::Buffer::TargetHint::Uniform,
            jointMatricesUniformData
        };
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            FlatDrawUniform{}
                .setPerInstanceJointCount(5)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            FlatMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindJointBuffer(jointMatricesUniform)
            .draw(mesh);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "TestFiles/skinning-instanced.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}
#endif

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

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= FlatGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
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

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(FlatGL2D::Flag::UniformBuffers|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::Textured) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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

    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= FlatGL2D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            /* Each slice has half height, second slice has the data in the
               right half */
            const UnsignedShort imageData[]{
                2000, 0,
                0, 3000,
                4000, 0
            };
            ImageView3D image{PixelFormat::R16UI, {2, 1, 3}, imageData};

            objectIdTextureArray = GL::Texture2DArray{};
            objectIdTextureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTextureArray);
        } else {
            /* First is taken from bottom left, second from bottom right, third
               from top center (there I just duplicate the pixel on both
               sides) */
            const UnsignedShort imageData[]{
                2000, 3000,
                4000, 4000
            };
            ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

            objectIdTexture = GL::Texture2D{};
            objectIdTexture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTexture);
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
        .setIndexOffset(circleData.indexCount());
    GL::MeshView triangle{mesh};
    triangle.setCount(triangleData.indexCount())
        .setIndexOffset(circleData.indexCount() + squareData.indexCount());

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
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & FlatGL2D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {FlatGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {FlatGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "FlatTestFiles", data.expected2D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & FlatGL2D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], data.expectedId[0]); /* Circle */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], data.expectedId[1]); /* Square */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], data.expectedId[2]); /* Triangle */
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

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL3D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= FlatGL3D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
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

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(FlatGL3D::Flag::UniformBuffers|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags & FlatGL3D::Flag::Textured) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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

    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= FlatGL3D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & FlatGL3D::Flag::TextureArrays) {
            /* Each slice has half height, second slice has the data in the
               right half */
            const UnsignedShort imageData[]{
                2000, 0,
                0, 3000,
                4000, 0
            };
            ImageView3D image{PixelFormat::R16UI, {2, 1, 3}, imageData};

            objectIdTextureArray = GL::Texture2DArray{};
            objectIdTextureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTextureArray);
        } else {
            /* First is taken from bottom left, second from bottom right, third
               from top center (there I just duplicate the pixel on both
               sides) */
            const UnsignedShort imageData[]{
                2000, 3000,
                4000, 4000
            };
            ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

            objectIdTexture = GL::Texture2D{};
            objectIdTexture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTexture);
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
        .setIndexOffset(sphereData.indexCount());
    GL::MeshView cone{mesh};
    cone.setCount(coneData.indexCount())
        .setIndexOffset(sphereData.indexCount() + planeData.indexCount());

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
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & FlatGL3D::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {FlatGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {FlatGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "FlatTestFiles", data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & FlatGL2D::Flag::ObjectId) {
        _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
        CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);
        Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::R32UI});
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[5][5], 27); /* Outside */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][24], data.expectedId[0]); /* Sphere */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[24][56], data.expectedId[1]); /* Plane */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[56][40], data.expectedId[2]); /* Circle */
    }
}

void FlatGLTest::renderMultiSkinning2D() {
    auto&& data = RenderMultiSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= FlatGL2D::Flag::ShaderStorageBuffers) {
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
        CORRADE_SKIP("UBOs with dynamically indexed (joint) arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    FlatGL2D shader{FlatGL2D::Configuration{}
        .setFlags(FlatGL2D::Flag::UniformBuffers|data.flags)
        .setDrawCount(data.drawCount)
        .setMaterialCount(data.materialCount)
        .setJointCount(data.jointCount, 2, 0)};

    /* Similarly to renderSkinning2D() tests just 2D movement, differently and
       clearly distinguisable for each draw */
    struct Vertex {
        Vector2 position;
        UnsignedInt jointIds[2];
        Float weights[2];
    } vertices[]{
        /* Each corner affected by exactly one matrix, but at different item
           in the array

           3--1    5 9--8
           | /|   /| | /
           |/ |  / | |/
           2--0 6--4 7 */
        {{ 1.0f, -1.0f}, {0, 0}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {0, 2}, {0.0f, 1.0f}},
        {{-1.0f, -1.0f}, {1, 2}, {1.0f, 0.0f}},
        {{-1.0f,  1.0f}, {0, 3}, {0.0f, 1.0f}},

        {{ 1.0f, -1.0f}, {0, 3}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f}, {2, 1}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f}, {0, 0}, {1.0f, 0.0f}},

        {{-1.0f, -1.0f}, {0, 1}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f}, {1, 0}, {1.0f, 0.0f}},
        {{-1.0f,  1.0f}, {2, 2}, {0.5f, 0.5f}}
    };

    UnsignedInt indices[]{
        0, 1, 2,
        2, 1, 3,

        4, 5, 6,

        7, 8, 9
    };

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(12)
        .addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, vertices}, 0,
            FlatGL2D::Position{},
            FlatGL2D::JointIds{FlatGL2D::JointIds::Components::Two},
            FlatGL2D::Weights{FlatGL2D::Weights::Components::Two})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, indices}, 0, MeshIndexType::UnsignedInt);
    GL::MeshView square{mesh};
    square.setCount(6);
    GL::MeshView triangle1{mesh};
    triangle1.setCount(3)
        .setIndexOffset(6);
    GL::MeshView triangle2{mesh};
    triangle2.setCount(3)
        .setIndexOffset(9);

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<FlatMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(0x33ffff_rgbf);
    materialData[1*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(0xffff33_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationProjectionUniform2D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::scaling(Vector2{0.3f})*
            Matrix3::translation({ 0.0f, -1.5f}));
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::scaling(Vector2{0.3f})*
            Matrix3::translation({ 1.5f,  1.5f}));
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::scaling(Vector2{0.3f})*
            Matrix3::translation({-1.5f,  1.5f}));
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    Containers::Array<TransformationUniform2D> jointData{Math::max(2*data.uniformIncrement + 4, 10u)};
    /* First draw moves both bottom corners */
    jointData[Math::max(0*data.uniformIncrement, 0u) + 0] = TransformationUniform2D{}
        .setTransformationMatrix(Matrix3::translation({ 0.5f, -0.5f}));
    jointData[Math::max(0*data.uniformIncrement, 0u) + 1] = TransformationUniform2D{}
        .setTransformationMatrix(Matrix3::translation({-0.5f, -0.5f}));
    jointData[Math::max(0*data.uniformIncrement, 0u) + 2] =  TransformationUniform2D{};
    jointData[Math::max(0*data.uniformIncrement, 0u) + 3] = TransformationUniform2D{};
    /* Second draw overlaps with the first with two identity matrices (unless
       the padding prevents that); moves top right corner */
    jointData[Math::max(1*data.uniformIncrement, 2u) + 0] = TransformationUniform2D{};
    jointData[Math::max(1*data.uniformIncrement, 2u) + 1] = TransformationUniform2D{};
    jointData[Math::max(1*data.uniformIncrement, 2u) + 2] = TransformationUniform2D{}
        .setTransformationMatrix(Matrix3::translation({ 0.5f, 0.5f}));
    jointData[Math::max(1*data.uniformIncrement, 2u) + 3] = TransformationUniform2D{};
    /* Third draw moves top left corner */
    jointData[Math::max(2*data.uniformIncrement, 6u) + 0] = TransformationUniform2D{};
    jointData[Math::max(2*data.uniformIncrement, 6u) + 1] = TransformationUniform2D{};
    jointData[Math::max(2*data.uniformIncrement, 6u) + 2] = TransformationUniform2D{}
        .setTransformationMatrix(Matrix3::translation({-0.5f, 0.5f}));
    /* This one is unused but has to be here in order to be able to bind the
       last three-component part while JOINT_COUNT is set to 4 */
    jointData[Math::max(2*data.uniformIncrement, 6u) + 3] = TransformationUniform2D{};
    GL::Buffer jointUniform{GL::Buffer::TargetHint::Uniform,
        jointData};

    Containers::Array<FlatDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material / joint offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead */
    drawData[0*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 0);
    drawData[1*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        /* Overlaps with the first joint set with two matrices, unless the
           padding in the single-draw case prevents that */
        .setJointOffset(data.bindWithOffset ? 0 : 2);
    drawData[2*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 6);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindJointBuffer(jointUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform2D),
            4*sizeof(TransformationUniform2D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindJointBuffer(jointUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform2D),
            4*sizeof(TransformationUniform2D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        shader.draw(triangle1);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindJointBuffer(jointUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform2D),
            4*sizeof(TransformationUniform2D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        shader.draw(triangle2);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindJointBuffer(jointUniform)
            .bindDrawBuffer(drawUniform);

        if(data.flags >= FlatGL2D::Flag::MultiDraw)
            shader.draw({square, triangle1, triangle2});
        else {
            shader.setDrawOffset(0)
                .draw(square);
            shader.setDrawOffset(1)
                .draw(triangle1);
            shader.setDrawOffset(2)
                .draw(triangle2);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "TestFiles/skinning-multi.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

void FlatGLTest::renderMultiSkinning3D() {
    auto&& data = RenderMultiSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= FlatGL3D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 2)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
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
        CORRADE_SKIP("UBOs with dynamically indexed (joint) arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    FlatGL3D shader{FlatGL3D::Configuration{}
        .setFlags(FlatGL3D::Flag::UniformBuffers|data.flags)
        .setDrawCount(data.drawCount)
        .setMaterialCount(data.materialCount)
        .setJointCount(data.jointCount, 2, 0)};

    /* Similarly to renderSkinning3D() tests just 2D movement, differently and
       clearly distinguisable for each draw */
    struct Vertex {
        Vector3 position;
        UnsignedInt jointIds[2];
        Float weights[2];
    } vertices[]{
        /* Each corner affected by exactly one matrix, but at different item
           in the array

           3--1    5 9--8
           | /|   /| | /
           |/ |  / | |/
           2--0 6--4 7 */
        {{ 1.0f, -1.0f, 0.0f}, {0, 0}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {0, 2}, {0.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {1, 2}, {1.0f, 0.0f}},
        {{-1.0f,  1.0f, 0.0f}, {0, 3}, {0.0f, 1.0f}},

        {{ 1.0f, -1.0f, 0.0f}, {0, 3}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {2, 1}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0, 0}, {1.0f, 0.0f}},

        {{-1.0f, -1.0f, 0.0f}, {0, 1}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {1, 0}, {1.0f, 0.0f}},
        {{-1.0f,  1.0f, 0.0f}, {2, 2}, {0.5f, 0.5f}}
    };

    UnsignedInt indices[]{
        0, 1, 2,
        2, 1, 3,

        4, 5, 6,

        7, 8, 9
    };

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(12)
        .addVertexBuffer(GL::Buffer{GL::Buffer::TargetHint::Array, vertices}, 0,
            FlatGL3D::Position{},
            FlatGL3D::JointIds{FlatGL3D::JointIds::Components::Two},
            FlatGL3D::Weights{FlatGL3D::Weights::Components::Two})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, indices}, 0, MeshIndexType::UnsignedInt);
    GL::MeshView square{mesh};
    square.setCount(6);
    GL::MeshView triangle1{mesh};
    triangle1.setCount(3)
        .setIndexOffset(6);
    GL::MeshView triangle2{mesh};
    triangle2.setCount(3)
        .setIndexOffset(9);

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<FlatMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(0x33ffff_rgbf);
    materialData[1*data.uniformIncrement] = FlatMaterialUniform{}
        .setColor(0xffff33_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationProjectionUniform3D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::scaling(Vector3{0.3f})*
            Matrix4::translation({ 0.0f, -1.5f, 0.0f}));
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::scaling(Vector3{0.3f})*
            Matrix4::translation({ 1.5f,  1.5f, 0.0f}));
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::scaling(Vector3{0.3f})*
            Matrix4::translation({-1.5f,  1.5f, 0.0f}));
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    Containers::Array<TransformationUniform3D> jointData{Math::max(2*data.uniformIncrement + 4, 10u)};
    /* First draw moves both bottom corners */
    jointData[Math::max(0*data.uniformIncrement, 0u) + 0] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::translation({ 0.5f, -0.5f, 0.0f}));
    jointData[Math::max(0*data.uniformIncrement, 0u) + 1] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::translation({-0.5f, -0.5f, 0.0f}));
    jointData[Math::max(0*data.uniformIncrement, 0u) + 2] =  TransformationUniform3D{};
    jointData[Math::max(0*data.uniformIncrement, 0u) + 3] = TransformationUniform3D{};
    /* Second draw overlaps with the first with two identity matrices (unless
       the padding prevents that); moves top right corner */
    jointData[Math::max(1*data.uniformIncrement, 2u) + 0] = TransformationUniform3D{};
    jointData[Math::max(1*data.uniformIncrement, 2u) + 1] = TransformationUniform3D{};
    jointData[Math::max(1*data.uniformIncrement, 2u) + 2] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::translation({ 0.5f, 0.5f, 0.0f}));
    jointData[Math::max(1*data.uniformIncrement, 2u) + 3] = TransformationUniform3D{};
    /* Third draw moves top left corner */
    jointData[Math::max(2*data.uniformIncrement, 6u) + 0] = TransformationUniform3D{};
    jointData[Math::max(2*data.uniformIncrement, 6u) + 1] = TransformationUniform3D{};
    jointData[Math::max(2*data.uniformIncrement, 6u) + 2] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::translation({-0.5f, 0.5f, 0.0f}));
    /* This one is unused but has to be here in order to be able to bind the
       last three-component part while JOINT_COUNT is set to 4 */
    jointData[Math::max(2*data.uniformIncrement, 6u) + 3] = TransformationUniform3D{};
    GL::Buffer jointUniform{GL::Buffer::TargetHint::Uniform,
        jointData};

    Containers::Array<FlatDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material / joint offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead */
    drawData[0*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 0);
    drawData[1*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        /* Overlaps with the first joint set with two matrices, unless the
           padding in the single-draw case prevents that */
        .setJointOffset(data.bindWithOffset ? 0 : 2);
    drawData[2*data.uniformIncrement] = FlatDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 6);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindJointBuffer(jointUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindJointBuffer(jointUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        shader.draw(triangle1);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(FlatMaterialUniform),
            sizeof(FlatMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindJointBuffer(jointUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(FlatDrawUniform),
            sizeof(FlatDrawUniform));
        shader.draw(triangle2);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindJointBuffer(jointUniform)
            .bindDrawBuffer(drawUniform);

        if(data.flags >= FlatGL3D::Flag::MultiDraw)
            shader.draw({square, triangle1, triangle2});
        else {
            shader.setDrawOffset(0)
                .draw(square);
            shader.setDrawOffset(1)
                .draw(triangle1);
            shader.setDrawOffset(2)
                .draw(triangle2);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "TestFiles/skinning-multi.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatGLTest)
