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
#include <Corrade/TestSuite/Compare/Numeric.h>
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
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/MeshTools/FlipNormals.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/PhongGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/TextureArray.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Cone.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Phong.h"
#endif

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct PhongGLTest: GL::OpenGLTester {
    explicit PhongGLTest();

    void construct();
    #ifndef MAGNUM_TARGET_GLES2
    void constructSkinning();
    #endif
    void constructAsync();
    #ifndef MAGNUM_TARGET_GLES2
    void constructUniformBuffers();
    void constructUniformBuffersAsync();
    #endif

    void constructMove();
    #ifndef MAGNUM_TARGET_GLES2
    void constructMoveUniformBuffers();
    #endif

    void constructInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    void constructUniformBuffersInvalid();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void setPerVertexJointCountInvalid();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setUniformUniformBuffersEnabled();
    void bindBufferUniformBuffersNotEnabled();
    #endif
    void bindTexturesInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    void bindTextureArraysInvalid();
    #endif
    void setAlphaMaskNotEnabled();
    void setSpecularDisabled();
    void setTextureMatrixNotEnabled();
    void setNormalTextureScaleNotEnabled();
    #ifndef MAGNUM_TARGET_GLES2
    void setTextureLayerNotArray();
    void bindTextureTransformBufferNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setObjectIdNotEnabled();
    #endif
    void setWrongLightCountOrId();
    #ifndef MAGNUM_TARGET_GLES2
    void setWrongJointCountOrId();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setWrongDrawOffset();
    #endif

    void renderSetup();
    void renderTeardown();

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderDefaults();
    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderColored();
    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderSinglePixelTextured();

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderTextured();
    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderTexturedNormal();

    template<class T, PhongGL::Flag flag = PhongGL::Flag{}> void renderVertexColor();

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderShininess();

    void renderAlphaSetup();
    void renderAlphaTeardown();

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderAlpha();

    #ifndef MAGNUM_TARGET_GLES2
    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderObjectId();
    #endif

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderLights();

    /* This tests something that's irrelevant to UBOs */
    void renderLightsSetOneByOne();
    /* This tests just the algorithm, not affected by UBOs */
    void renderLowLightAngle();
    #ifndef MAGNUM_TARGET_GLES2
    void renderLightCulling();
    #endif

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderZeroLights();

    /* This tests something that's irrelevant to UBOs */
    void renderDoubleSided();

    #ifndef MAGNUM_TARGET_GLES2
    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderSkinning();
    #endif

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderInstanced();
    #ifndef MAGNUM_TARGET_GLES2
    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderInstancedSkinning();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti();
    void renderMultiSkinning();
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
    [L] point lights
    [I] instancing
    [O] UBOs + draw offset
    [M] multidraw
    [L] texture arrays

    Mesa Intel                      BADLIOML
               ES2                       xxx
               ES3                  BADL Ox
    Mesa AMD                        BAD
    Mesa llvmpipe                   BAD
    SwiftShader ES2                 BADL xxx
                ES3                 BADL
    ANGLE ES2                            xxx
          ES3                       BADL OM
    ARM Mali (Huawei P10) ES2       BAD  xxx
                          ES3       BADL Ox
    WebGL (on Mesa Intel) 1.0       BAD  xxx
                          2.0       BADL OM
    NVidia                          BAD
    Intel Windows                   BAD
    AMD macOS                       BAD
    Intel macOS                     BADL Ox
    iPhone 6 w/ iOS 12.4 ES3        BAD   x
*/

constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, perDrawLightCount;
} ConstructData[]{
    {"", {}, 1, 1},
    {"ambient texture", PhongGL::Flag::AmbientTexture, 1, 1},
    {"diffuse texture", PhongGL::Flag::DiffuseTexture, 1, 1},
    {"diffuse texture + texture transform", PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureTransformation, 1, 1},
    {"specular texture", PhongGL::Flag::SpecularTexture, 1, 1},
    {"normal texture", PhongGL::Flag::NormalTexture, 1, 1},
    {"normal texture + separate bitangents", PhongGL::Flag::NormalTexture|PhongGL::Flag::Bitangent, 1, 1},
    {"separate bitangents alone", PhongGL::Flag::Bitangent, 1, 1},
    {"ambient + diffuse texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture, 1, 1},
    {"ambient + specular texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture, 1, 1},
    {"diffuse + specular texture", PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture, 1, 1},
    {"ambient + diffuse + specular texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture, 1, 1},
    {"ambient + diffuse + specular + normal texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"ambient + diffuse + specular + normal texture arrays", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays, 1, 1},
    {"ambient + diffuse + specular + normal texture arrays + texture transformation", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation, 1, 1},
    #endif
    {"alpha mask", PhongGL::Flag::AlphaMask, 1, 1},
    {"alpha mask + diffuse texture", PhongGL::Flag::AlphaMask|PhongGL::Flag::DiffuseTexture, 1, 1},
    {"vertex colors", PhongGL::Flag::VertexColor, 1, 1},
    {"vertex colors + diffuse texture", PhongGL::Flag::VertexColor|PhongGL::Flag::DiffuseTexture, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", PhongGL::Flag::ObjectId, 1, 1},
    /* This is fine, InstancedObjectId isn't (check in ConstructInvalidData) */
    {"object ID + separate bitangent", PhongGL::Flag::ObjectId|PhongGL::Flag::Bitangent, 1, 1},
    {"instanced object ID", PhongGL::Flag::InstancedObjectId, 1, 1},
    {"object ID + alpha mask + specular texture", PhongGL::Flag::ObjectId|PhongGL::Flag::AlphaMask|PhongGL::Flag::SpecularTexture, 1, 1},
    {"object ID texture", PhongGL::Flag::ObjectIdTexture, 1, 1},
    {"object ID texture array", PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays, 1, 1},
    {"object ID texture + instanced texture transformation", PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedTextureOffset, 1, 1},
    {"object ID texture array + instanced texture transformation", PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::InstancedTextureOffset, 1, 1},
    {"instanced object ID texture array + texture transformation", PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedObjectId|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation, 1, 1},
    {"object ID texture + diffuse texture", PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::DiffuseTexture, 1, 1},
    {"object ID texture, zero lights", PhongGL::Flag::ObjectIdTexture, 0, 0},
    #endif
    {"no specular", PhongGL::Flag::NoSpecular, 1, 1},
    {"five lights", {}, 5, 5},
    {"fifteen lights, five used", {}, 15, 5},
    {"zero lights", {}, 0, 0},
    {"instanced transformation", PhongGL::Flag::InstancedTransformation, 3, 3},
    {"instanced transformation, zero lights", PhongGL::Flag::InstancedTransformation, 0, 0},
    {"instanced specular texture offset", PhongGL::Flag::SpecularTexture|PhongGL::Flag::InstancedTextureOffset, 3, 3},
    {"instanced normal texture offset", PhongGL::Flag::NormalTexture|PhongGL::Flag::InstancedTextureOffset, 3, 3},
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId|Bitangent is disallowed (checked in
       ConstructInvalidData), but this should work */
    {"object ID + normal texture with bitangent from tangent", PhongGL::Flag::InstancedObjectId|PhongGL::Flag::NormalTexture, 1, 1}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructSkinningData[]{
    {"no skinning", {},
        1, 0, 0, 0},
    {"one set", {},
        1, 16, 4, 0},
    {"two partial sets", {},
        1, 32, 2, 3},
    {"secondary set only", {},
        1, 12, 0, 4},
    {"dynamic per-vertex sets", PhongGL::Flag::DynamicPerVertexJointCount,
        1, 16, 4, 3},
    {"zero lights, one set", {},
        0, 15, 4, 0},
    {"multiple lights, one set", {},
        3, 15, 4, 0},
    {"multiple lights, two sets, dynamic per-vertex sets",
        PhongGL::Flag::DynamicPerVertexJointCount,
        5, 10, 4, 4}
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, perDrawLightCount, materialCount, drawCount;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {},
        1, 1, 1, 1, 0, 0, 0},
    {"", PhongGL::Flag::UniformBuffers,
        1, 1, 1, 1, 0, 0, 0},
    /* SwiftShader has 256 uniform vectors at most, per-3D-draw is 4+4,
       per-material 4, per-light 4 plus 4 for projection */
    {"multiple lights, materials, draws", PhongGL::Flag::UniformBuffers,
        8, 8, 8, 24, 0, 0, 0},
    {"multiple lights, materials, draws + light culling", PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling,
        8, 4, 8, 24, 0, 0, 0},
    {"zero lights", PhongGL::Flag::UniformBuffers,
        0, 0, 16, 24, 0, 0, 0},
    {"ambient + diffuse + specular texture", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture,
        1, 1, 1, 1, 0, 0, 0},
    {"ambient + diffuse + specular texture + texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureTransformation,
        1, 1, 1, 1, 0, 0, 0},
    {"ambient + diffuse + specular texture array + texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation,
        1, 1, 1, 1, 0, 0, 0},
    {"normal texture", PhongGL::Flag::UniformBuffers|PhongGL::Flag::NormalTexture,
        1, 1, 1, 1, 0, 0, 0},
    {"normal texture + separate bitangents", PhongGL::Flag::UniformBuffers|PhongGL::Flag::NormalTexture|PhongGL::Flag::Bitangent,
        1, 1, 1, 1, 0, 0, 0},
    {"alpha mask", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AlphaMask,
        1, 1, 1, 1, 0, 0, 0},
    {"object ID", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectId,
        1, 1, 1, 1, 0, 0, 0},
    {"object ID texture", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectIdTexture,
        1, 1, 1, 1, 0, 0, 0},
    {"object ID texture array", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation,
        1, 1, 1, 1, 0, 0, 0},
    {"object ID texture + instanced texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedTextureOffset,
        1, 1, 1, 1, 0, 0, 0},
    {"object ID texture array + instanced texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::InstancedTextureOffset,
        1, 1, 1, 1, 0, 0, 0},
    {"instanced object ID texture array + texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedObjectId|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation,
        1, 1, 1, 1, 0, 0, 0},
    {"object ID texture + diffuse texture", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::DiffuseTexture,
        1, 1, 1, 1, 0, 0, 0},
    {"no specular", PhongGL::Flag::UniformBuffers|PhongGL::Flag::NoSpecular,
        1, 1, 1, 1, 0, 0, 0},
    {"skinning", PhongGL::Flag::UniformBuffers,
        1, 1, 1, 1, 32, 3, 2},
    {"skinning, dynamic per-vertex sets", PhongGL::Flag::UniformBuffers|PhongGL::Flag::DynamicPerVertexJointCount,
        1, 1, 1, 1, 32, 3, 4},
    {"multidraw with all the things except secondary per-vertex sets", PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::AlphaMask|PhongGL::Flag::ObjectId|PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::InstancedTransformation|PhongGL::Flag::InstancedObjectId|PhongGL::Flag::LightCulling|PhongGL::Flag::DynamicPerVertexJointCount,
        8, 4, 16, 24, 16, 4, 0},
    {"multidraw with all the things except instancing", PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::AlphaMask|PhongGL::Flag::ObjectId|PhongGL::Flag::LightCulling|PhongGL::Flag::DynamicPerVertexJointCount,
        8, 4, 16, 24, 16, 3, 4},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with all the things except secondary per-vertex sets", PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::AlphaMask|PhongGL::Flag::ObjectId|PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::InstancedTransformation|PhongGL::Flag::InstancedObjectId|PhongGL::Flag::LightCulling|PhongGL::Flag::DynamicPerVertexJointCount,
        0, 4, 0, 0, 0, 4, 0},
    {"shader storage + multidraw with all the things except instancing", PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::AlphaMask|PhongGL::Flag::ObjectId|PhongGL::Flag::LightCulling|PhongGL::Flag::DynamicPerVertexJointCount,
        0, 4, 0, 0, 0, 3, 4},
    #endif
};
#endif

constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, perDrawLightCount, jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    const char* message;
} ConstructInvalidData[] {
    {"per-draw light count larger than total count", {}, 10, 11, 0, 0, 0,
        "per-draw light count expected to not be larger than total count of 10, got 11"},
    {"texture transformation but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        PhongGL::Flag::TextureTransformation
            #ifndef MAGNUM_TARGET_GLES2
            |PhongGL::Flag::ObjectId
            #endif
            ,
        1, 1, 0, 0, 0,
        "texture transformation enabled but the shader is not textured"},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        PhongGL::Flag::TextureArrays|PhongGL::Flag::ObjectId,
        1, 1, 0, 0, 0,
        "texture arrays enabled but the shader is not textured"},
    {"conflicting bitangent and instanced object id attribute",
        PhongGL::Flag::Bitangent|PhongGL::Flag::InstancedObjectId,
        1, 1, 0, 0, 0,
        "Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead"},
    #endif
    {"specular texture but no specular",
        PhongGL::Flag::SpecularTexture|PhongGL::Flag::NoSpecular,
        1, 1, 0, 0, 0,
        "specular texture requires the shader to not have specular disabled"},
    #ifndef MAGNUM_TARGET_GLES2
    {"dynamic per-vertex joint count but no static per-vertex joint count",
        PhongGL::Flag::DynamicPerVertexJointCount,
        1, 1, 0, 0, 0,
        "dynamic per-vertex joint count enabled for zero joints"},
    {"instancing together with secondary per-vertex sets",
        PhongGL::Flag::InstancedTransformation,
        1, 1, 10, 4, 1,
        "TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, perDrawLightCount, jointCount, perVertexJointCount, secondaryPerVertexJointCount, materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    /* These three fail for UBOs but not SSBOs */
    {"per-draw light count larger than total count",
        PhongGL::Flag::UniformBuffers,
        10, 11, 0, 0, 0, 1, 1,
        "per-draw light count expected to not be larger than total count of 10, got 11"},
    {"zero draws",
        PhongGL::Flag::UniformBuffers,
        1, 1, 0, 0, 0, 1, 0,
        "draw count can't be zero"},
    {"zero materials",
        PhongGL::Flag::UniformBuffers,
        1, 1, 0, 0, 0, 0, 1,
        "material count can't be zero"},
    {"texture arrays but no transformation",
        PhongGL::Flag::UniformBuffers|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        1, 1, 0, 0, 0, 1, 1,
        "texture arrays require texture transformation enabled as well if uniform buffers are used"},
    {"light culling but no UBOs",
        PhongGL::Flag::LightCulling,
        1, 1, 0, 0, 0, 1, 1,
        "light culling requires uniform buffers to be enabled"},
    /* These two fail for UBOs but not SSBOs */
    {"per-vertex joint count but no joint count",
        PhongGL::Flag::UniformBuffers,
        1, 1, 0, 2, 0, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
    {"secondary per-vertex joint count but no joint count",
        PhongGL::Flag::UniformBuffers,
        1, 1, 0, 0, 3, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
};
#endif

constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    const char* message;
} BindTexturesInvalidData[]{
    {"not textured",
        PhongGL::Flags{}
            #ifndef MAGNUM_TARGET_GLES2
            /* ObjectId shares bits with ObjectIdTexture but should still
               trigger the assert */
            |PhongGL::Flag::ObjectId
            #endif
            ,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled\n"
        #ifndef MAGNUM_TARGET_GLES2
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled\n"
        #endif
        "Shaders::PhongGL::bindTextures(): the shader was not created with any textures enabled\n"},
    #ifndef MAGNUM_TARGET_GLES2
    {"array", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindTextures(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    const char* message;
} BindTextureArraysInvalidData[]{
    {"not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        PhongGL::Flag::ObjectId,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled\n"
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled\n"},
    {"not array",
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::ObjectIdTexture,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"}
};
#endif

using namespace Containers::Literals;
using namespace Math::Literals;

const struct {
    const char* name;
    UnsignedInt lightCount, perDrawLightCount;
    Deg rotation;
    Color3 lightColor1, lightColor2;
    Float lightPosition1, lightPosition2;
} RenderColoredData[]{
    {"", 2, 2, {}, 0x993366_rgbf, 0x669933_rgbf, -3.0f, 3.0f},
    {"per-draw light count less than total", 4, 2, {}, 0x993366_rgbf, 0x669933_rgbf, -3.0f, 3.0f},
    {"flip lights", 2, 2, {}, 0x669933_rgbf, 0x993366_rgbf, 3.0f, -3.0f},
    {"rotated", 2, 2, 45.0_degf, 0x993366_rgbf, 0x669933_rgbf, -3.0f, 3.0f}
};

constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    Int layer;
    bool multiBind;
} RenderSinglePixelTexturedData[]{
    {"", {}, 0, false},
    {"multi bind", {}, 0, true},
    #ifndef MAGNUM_TARGET_GLES2
    {"array, first layer", PhongGL::Flag::TextureArrays, 0, false},
    {"array, arbitrary layer", PhongGL::Flag::TextureArrays, 6, false},
    #endif
};

const struct {
    const char* name;
    const char* expected;
    PhongGL::Flags flags;
    Matrix3 textureTransformation;
    Int layer;
} RenderTexturedData[]{
    {"all", "textured.tga",
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture,
        {}, 0},
    {"ambient", "textured-ambient.tga", PhongGL::Flag::AmbientTexture,
        {}, 0},
    {"diffuse", "textured-diffuse.tga", PhongGL::Flag::DiffuseTexture,
        {}, 0},
    {"diffuse transformed", "textured-diffuse-transformed.tga",
        PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), 0},
    {"specular", "textured-specular.tga", PhongGL::Flag::SpecularTexture,
        {}, 0},
    #ifndef MAGNUM_TARGET_GLES2
    {"all, array, first layer", "textured.tga",
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureArrays,
        {}, 0},
    {"all, array, arbitrary layer", "textured.tga",
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureArrays,
        {}, 6},
    {"diffuse, array, texture transformation, arbitrary layer", "textured-diffuse-transformed.tga",
        PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), 6},
    #endif
};

/* MSVC 2015 doesn't like constexpr here due to the angles */
const struct {
    const char* name;
    const char* expected;
    bool multiBind;
    Deg rotation;
    Float scale;
    Vector4 tangent;
    Vector3 bitangent;
    PhongGL::Tangent4::Components tangentComponents;
    bool flipNormalY;
    PhongGL::Flags flags;
    Int layer;
} RenderTexturedNormalData[]{
    {"", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    {"multi bind", "textured-normal.tga", true, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays, first layer", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false,
        PhongGL::Flag::TextureArrays, 0},
    {"texture arrays, arbitrary layer", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false,
        PhongGL::Flag::TextureArrays, 6},
    #endif
    {"rotated 90°", "textured-normal.tga", false, 90.0_degf, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    {"rotated -90°", "textured-normal.tga", false, -90.0_degf, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    {"0.5 scale", "textured-normal0.5.tga", false, {}, 0.5f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    {"0.0 scale", "textured-normal0.0.tga", false, {}, 0.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    /* The fourth component, if missing, gets automatically filled up to 1,
       so this should work */
    {"implicit bitangent direction", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 0.0f}, {},
        PhongGL::Tangent4::Components::Three, false, {}, 0},
    {"separate bitangents", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
        PhongGL::Tangent4::Components::Three, false,
        PhongGL::Flag::Bitangent, 0},
    {"right-handed, flipped Y", "textured-normal-left.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        PhongGL::Tangent4::Components::Four, true, {}, 0},
    {"left-handed", "textured-normal-left.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, -1.0f}, {},
        PhongGL::Tangent4::Components::Four, false, {}, 0},
    {"left-handed, separate bitangents", "textured-normal-left.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
        PhongGL::Tangent4::Components::Three, false,
        PhongGL::Flag::Bitangent, 0},
    {"left-handed, flipped Y", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, -1.0f}, {},
        PhongGL::Tangent4::Components::Four, true, {}, 0},
};

const struct {
    const char* name;
    const char* expected;
    PhongGL::Flags flags;
    Float shininess;
    Color4 specular;
} RenderShininessData[] {
    {"80", "shininess80.tga",
        {}, 80.0f, 0xffffff_rgbf},
    {"10", "shininess10.tga",
        {}, 10.0f, 0xffffff_rgbf},
    {"0", "shininess0.tga",
        {}, 0.0f, 0xffffff_rgbf},
    {"0.001", "shininess0.tga",
        {}, 0.001f, 0xffffff_rgbf},
    {"black specular", "shininess-no-specular.tga",
        {}, 80.0f, 0x000000_rgbf},
    {"no specular", "shininess-no-specular.tga",
        PhongGL::Flag::NoSpecular, 80.0f, 0xffffff_rgbf}
};

const struct {
    const char* name;
    const char* expected;
    bool blending;
    PhongGL::Flags flags;
    Float threshold;
    const char* ambientTexture;
    const char* diffuseTexture;
    Color4 ambientColor;
    Color4 diffuseColor;
} RenderAlphaData[] {
    /* All those deliberately have a non-white diffuse in order to match the
       expected data from textured() */
    {"none, separate", "PhongTestFiles/textured-diffuse.tga", false,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture, 0.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"none, combined", "PhongTestFiles/textured-diffuse.tga", false,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture, 0.0f,
        "diffuse-alpha-texture.tga", "diffuse-alpha-texture.tga",
        0x000000ff_rgbaf, 0x9999ff00_rgbaf},
    {"blending, separate", "PhongTestFiles/textured-diffuse-alpha.tga", true,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture, 0.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"blending, combined", "PhongTestFiles/textured-diffuse-alpha.tga", true,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture, 0.0f,
        "diffuse-alpha-texture.tga", "diffuse-alpha-texture.tga",
        0x000000ff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 0.0, separate", "PhongTestFiles/textured-diffuse.tga", false,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AlphaMask, 0.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 0.5, separate", "PhongTestFiles/textured-diffuse-alpha-mask0.5.tga", false,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AlphaMask, 0.5f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 0.5, combined", "PhongTestFiles/textured-diffuse-alpha-mask0.5.tga", false,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AlphaMask, 0.5f,
        "diffuse-alpha-texture.tga", "diffuse-alpha-texture.tga",
        0x000000ff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 1.0, separate", "TestFiles/alpha-mask1.0.tga", false,
        PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AlphaMask, 1.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf}
    /* texture arrays are orthogonal to this, no need to be tested here */
};

const struct {
    const char* name;
    const char* file;
    Vector4 position;
    Color3 specularColor, lightSpecularColor;
    Float intensity;
    Containers::Optional<Float> range; /* Constants::inf() if not set */
    Containers::Array<Containers::Pair<Vector2i, Color3ub>> picks;
} RenderLightsData[] {
    {"directional", "light-directional.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(),
        {InPlaceInit, {
            /* Ambient isn't affected by light direction, otherwise it's a
               dot product of a normalized direction */
            {{40, 40}, 0x222222_rgb + 0xff8080_rgb*dot(Vector3{1.0f, -1.5f, 0.5f}.normalized(), Vector3::zAxis())},
            /* and it's the same across the whole surface */
            {{70, 70}, 0x222222_rgb + 0xff8080_rgb*dot(Vector3{1.0f, -1.5f, 0.5f}.normalized(), Vector3::zAxis())},
        }}},
    /* These two should produce the same output as the *normalized* dot product
       is the same */
    {"directional, from the other side", "light-directional.tga",
        {-1.0f, 1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(), {}},
    {"directional, scaled direction", "light-directional.tga",
        {10.0f, -15.0f, 5.0f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(), {}},
    /* Range should have no effect either, especially zero range should not
       cause any NaNs. Default or explicit infinity shouldn't either. */
    {"directional, range left at (infinity) default", "light-directional.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, {}, {}},
    {"directional, range=inf", "light-directional.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(), {}},
    {"directional, range=0.1", "light-directional.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 1.0f, {}},
    {"directional, range=0", "light-directional.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 1.0f, {}},
    /* Light from the other side doesn't contribute anything */
    {"directional, from back", "light-none.tga",
        {-1.0f, 1.5f, -0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(),
        {InPlaceInit, {
            /* Only ambient color left */
            {{40, 40}, 0x222222_rgb}
        }}},
    /* This is the same as above, except that twice the intensity causes it to
       be 2x brighter */
    {"directional, intensity=2", "light-directional-intensity2.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        2.0f, 1.0f,
        {InPlaceInit, {
            {{40, 40}, 0x222222_rgb + 0xff8080_rgb*dot(Vector3{1.0f, -1.5f, 0.5f}.normalized(), Vector3::zAxis())*2.0f}
        }}},
    {"point", "light-point.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(),
        {InPlaceInit, {
            /* The range is inf, so it doesn't get fully ambient even at the
               edge */
            {{8, 71}, 0x2c2727_rgb},
            /* Closest to the light */
            {{63, 16}, 0x222222_rgb + 0xff8080_rgb/(1 + 0.75f*0.75f)},
            /* Specular highlight */
            {{60, 19}, 0xc47575_rgb}
        }}},
    {"point, specular material color", "light-point-specular-color.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, 0x80ff80_rgbf, Color3{1.0f},
        1.0f, Constants::inf(),
        {InPlaceInit, {
            /* Colored specular highlight */
            {{60, 19}, 0xc27573_rgb}
        }}},
    {"point, specular light color", "light-point-specular-color.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, 0x80ff80_rgbf,
        1.0f, Constants::inf(),
        {InPlaceInit, {
            /* Colored specular highlight */
            {{60, 19}, 0xc27573_rgb}
        }}},
    {"point, attenuated specular", "light-point-attenuated-specular.tga",
        {1.0f, -1.0f, -0.25f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 2.5f,
        {InPlaceInit, {
            /* Specular highlight shouldn't be brighter than the attenuated
               intensity */
            {{57, 22}, 0xa68787_rgb}
        }}},
    {"point, range=1.5, specular color", "light-point-range1.5.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, 0x80ff80_rgbf,
        1.0f, 1.5f,
        {InPlaceInit, {
            /* Color goes back to ambient at distance = 1.5 */
            {{59, 60}, 0x222222_rgb},
            {{29, 50}, 0x222222_rgb},
            {{19, 14}, 0x222222_rgb},
            /* But the center and specular stays ~ the same */
            {{63, 16}, 0xb16a6a_rgb},
            {{60, 19}, 0xad6a69_rgb}
        }}},
    {"point, intensity=10, range=1.0", "light-point-intensity10-range1.0.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        10.0f, 1.0f, {}},
    /* These two should produce the same result */
    {"point, range left at (infinity) default", "light-point.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, {}, {}},
    {"point, range=inf", "light-point.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(), {}},
    /* Range ends right at the surface, so no contribution */
    {"point, range=0.75", "light-none.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 0.75f, {}},
    /* Zero range should not cause any NaNs, so the ambient contribution is
       still there */
    {"point, range=0.0", "light-none.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 0.0f, {}},
    /* Distance is 0, which means the direction is always perpendicular and
       thus contributes nothing */
    {"point, distance=0", "light-none.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 0.0f, {}}
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt count, perDrawCount;
} RenderLightCullingData[]{
    {"same count and per-draw count", {}, 64, 64},
    {"per-draw count lower", {}, 64, 2},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage buffers, per-draw count only", PhongGL::Flag::ShaderStorageBuffers, 0, 2},
    #endif
};
#endif

const struct {
    const char* name;
    PhongGL::Flags flags;
    bool flipNormals;
} RenderDoubleSidedData[]{
    {"normals flipped", {}, true},
    {"double-sided rendering", PhongGL::Flag::DoubleSided, false}
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    UnsignedInt expected[4];
    PhongGL::Flags flags;
    Matrix3 textureTransformation;
    Int layer;
} RenderObjectIdData[]{
    {"",
        {40006, 40006, 40006, 40006},
        {}, {}, 0},
    {"textured",
        {40106, 40206, 40306, 40406},
        PhongGL::Flag::ObjectIdTexture, {}, 0},
    {"textured, texture transformation",
        {40406, 40306, 40206, 40106},
        PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), 0},
    {"texture array, first layer",
        {40106, 40206, 40306, 40406},
        PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays,
        {}, 0},
    {"texture array, arbitrary layer",
        {40106, 40206, 40306, 40406},
        PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays,
        {}, 6},
    {"texture array, texture transformation, arbitrary layer",
        {40406, 40306, 40206, 40106},
        PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureTransformation|PhongGL::Flag::TextureArrays,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), 6},
};
#endif

#ifndef MAGNUM_TARGET_GLES2
/* Same as in FlatGL and MeshVisualizerGL tests */
const struct {
    const char* name;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    UnsignedInt dynamicPerVertexJointCount, dynamicSecondaryPerVertexJointCount;
    PhongGL::Flags flags;
    Containers::Array<Containers::Pair<UnsignedInt, GL::DynamicAttribute>> attributes;
    bool setDynamicPerVertexJointCount, setJointMatrices, setJointMatricesOneByOne;
    const char* expected;
} RenderSkinningData[]{
    {"no skinning", 0, 0, 0, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, false, false, false,
        "skinning-default.tga"},
    {"default joint matrices", 5, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, false, false, false,
        "skinning-default.tga"},
    {"single set", 5, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, upload just a prefix of joint matrices", 15, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, upload joint matrices one by one", 5, 3, 0, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, false, true, true,
        "skinning.tga"},
    {"single set, dynamic, left at defaults", 5, 3, 0, 0, 0, PhongGL::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, dynamic", 5, 4, 0, 3, 0, PhongGL::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::Three}},
        }}, true, true, false,
        "skinning.tga"},
    {"two sets", 5, 1, 2, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::One}},
            {4, PhongGL::SecondaryJointIds{PhongGL::SecondaryJointIds::Components::Two}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::One}},
            {4*4, PhongGL::SecondaryWeights{PhongGL::SecondaryWeights::Components::Two}},
        }}, false, true, false,
        "skinning.tga"},
    {"two sets, dynamic, left at defaults", 5, 1, 2, 0, 0, PhongGL::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::One}},
            {4, PhongGL::SecondaryJointIds{PhongGL::SecondaryJointIds::Components::Two}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::One}},
            {4*4, PhongGL::SecondaryWeights{PhongGL::SecondaryWeights::Components::Two}},
        }}, false, true, false,
        "skinning.tga"},
    {"two sets, dynamic", 5, 4, 4, 1, 2, PhongGL::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::One}},
            {4, PhongGL::SecondaryJointIds{PhongGL::SecondaryJointIds::Components::Two}},
            {3*4, PhongGL::Weights{PhongGL::Weights::Components::One}},
            {4*4, PhongGL::SecondaryWeights{PhongGL::SecondaryWeights::Components::Two}},
        }}, true, true, false,
        "skinning.tga"},
    {"only secondary set", 5, 0, 3, 0, 0, {}, {InPlaceInit, {
            {0, PhongGL::SecondaryJointIds{PhongGL::SecondaryJointIds::Components::Three}},
            {3*4, PhongGL::SecondaryWeights{PhongGL::SecondaryWeights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"only secondary set, dynamic", 5, 4, 4, 0, 3, PhongGL::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            #ifdef MAGNUM_TARGET_WEBGL
            /* On WebGL the primary joint vertex attribute has to be bound to
               something even if not (dynamically) used in the end, otherwise
               it causes an error. So just alias it with the secondary one. */
            {0, PhongGL::JointIds{PhongGL::JointIds::Components::Three}},
            #endif
            {0, PhongGL::SecondaryJointIds{PhongGL::SecondaryJointIds::Components::Three}},
            {3*4, PhongGL::SecondaryWeights{PhongGL::SecondaryWeights::Components::Three}},
        }}, true, true, false,
        "skinning.tga"},
};
#endif

constexpr struct {
    const char* name;
    const char* expected;
    UnsignedInt expectedId[3];
    PhongGL::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData[] {
    {"diffuse color",
        "instanced.tga", {},
        {},
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    #ifndef MAGNUM_TARGET_GLES2
    {"diffuse color + object ID",
        "instanced.tga", {1000, 1000, 1000},
        PhongGL::Flag::ObjectId,
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    {"diffuse color + instanced object ID",
        "instanced.tga", {1211, 5627, 36363},
        PhongGL::Flag::InstancedObjectId,
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    {"diffuse color + textured object ID",
        "instanced.tga", {3000, 4000, 5000},
        PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedTextureOffset,
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    {"diffuse color + instanced textured object ID",
        "instanced.tga", {3211, 8627, 40363},
        PhongGL::Flag::InstancedObjectId|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedTextureOffset,
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    {"diffuse color + instanced texture array object ID",
        "instanced.tga", {3211, 8627, 40363},
        PhongGL::Flag::InstancedObjectId|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::TextureArrays,
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    #endif
    {"diffuse texture",
        "instanced-textured.tga", {},
        PhongGL::Flag::DiffuseTexture|PhongGL::Flag::InstancedTextureOffset,
        /* Minor differences on SwiftShader */
        112.0f, 0.09f},
    /** @todo test normal when there's usable texture */
    #ifndef MAGNUM_TARGET_GLES2
    {"diffuse texture array",
        "instanced-textured.tga", {},
        PhongGL::Flag::DiffuseTexture|PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::TextureArrays,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around); minor differences on
           SwiftShader */
        112.0f, 0.099f}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    const char* expected;
    UnsignedInt expectedId[3];
    PhongGL::Flags flags;
    UnsignedInt lightCount, perDrawLightCount, materialCount, drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData[] {
    {"bind with offset, colored",
        "multidraw.tga", {},
        {},
        2, 2, 1, 1, true, 16,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"bind with offset, colored + object ID",
        "multidraw.tga", {1211, 5627, 36363},
        PhongGL::Flag::ObjectId,
        2, 2, 1, 1, true, 16,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"bind with offset, colored + textured object ID",
        "multidraw.tga", {3211, 8627, 40363},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::ObjectIdTexture,
        2, 2, 1, 1, true, 16,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"bind with offset, colored + textured array object ID",
        "multidraw.tga", {3211, 8627, 40363},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays,
        2, 2, 1, 1, true, 16,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"bind with offset, textured",
        "multidraw-textured.tga", {},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture,
        2, 2, 1, 1, true, 16,
        /* Minor differences on ARM Mali, on NVidia */
        7.0f, 0.02f},
    {"bind with offset, texture array",
        "multidraw-textured.tga", {},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        2, 2, 1, 1, true, 16,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.146f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, texture array, shader storage",
        "multidraw-textured.tga", {},
        PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        0, 2, 0, 0, true, 16,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.146f},
    #endif
    {"draw offset, colored",
        "multidraw.tga", {},
        {},
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"draw offset, colore, less per-draw lights",
        "multidraw.tga", {},
        {},
        4, 2, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"draw offset, colored + object ID",
        "multidraw.tga", {1211, 5627, 36363},
        PhongGL::Flag::ObjectId,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"draw offset, colored + textured object ID",
        "multidraw.tga", {3211, 8627, 40363},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::ObjectIdTexture,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"draw offset, colored + textured array object ID",
        "multidraw.tga", {3211, 8627, 40363},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"draw offset, textured",
        "multidraw-textured.tga", {},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali, on NVidia */
        7.0f, 0.02f},
    {"draw offset, texture array",
        "multidraw-textured.tga", {},
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        4, 4, 2, 3, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.146f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, texture array, shader storage",
        "multidraw-textured.tga", {},
        PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        0, 2, 0, 0, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.146f},
    #endif
    {"multidraw, colored",
        "multidraw.tga", {},
        PhongGL::Flag::MultiDraw,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"multidraw, colored, less per-draw lights",
        "multidraw.tga", {},
        PhongGL::Flag::MultiDraw,
        4, 2, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"multidraw, colored + object ID",
        "multidraw.tga", {1211, 5627, 36363},
        PhongGL::Flag::MultiDraw|PhongGL::Flag::ObjectId,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"multidraw, colored + textured object ID",
        "multidraw.tga", {3211, 8627, 40363},
        PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::ObjectIdTexture,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"multidraw, colored + textured array object ID",
        "multidraw.tga", {3211, 8627, 40363},
        PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::ObjectIdTexture|PhongGL::Flag::TextureArrays,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"multidraw, textured",
        "multidraw-textured.tga", {},
        PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture,
        4, 4, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        4.67f, 0.02f},
    {"multidraw, texture array",
        "multidraw-textured.tga", {},
        PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        4, 4, 2, 3, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.141f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, texture array, shader storage",
        "multidraw-textured.tga", {},
        PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        0, 4, 0, 0, false, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.141f},
    #endif
    /** @todo test normal and per-draw scaling when there's usable texture */
};

/* Same as in FlatGL and MeshVisualizerGL tests */
const struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt materialCount, drawCount, jointCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
} RenderMultiSkinningData[]{
    {"bind with offset",
        {},
        1, 1, 4, true, 16},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, shader storage",
        PhongGL::Flag::ShaderStorageBuffers,
        0, 0, 0, true, 16},
    #endif
    {"draw offset",
        {},
        2, 3, 9, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, shader storage",
        PhongGL::Flag::ShaderStorageBuffers,
        0, 0, 0, false, 1},
    #endif
    {"multidraw",
        PhongGL::Flag::MultiDraw,
        2, 3, 9, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, shader storage",
        PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::MultiDraw,
        0, 0, 0, false, 1},
    #endif
};
#endif

PhongGLTest::PhongGLTest() {
    addInstancedTests({&PhongGLTest::construct},
        Containers::arraySize(ConstructData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::constructSkinning},
        Containers::arraySize(ConstructSkinningData));
    #endif

    addTests({&PhongGLTest::constructAsync});

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::constructUniformBuffers},
        Containers::arraySize(ConstructUniformBuffersData));

    addTests({&PhongGLTest::constructUniformBuffersAsync});
    #endif

    addTests({
        &PhongGLTest::constructMove,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::constructMoveUniformBuffers,
        #endif
    });

    addInstancedTests({&PhongGLTest::constructInvalid},
        Containers::arraySize(ConstructInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({
        &PhongGLTest::constructUniformBuffersInvalid},
        Containers::arraySize(ConstructUniformBuffersInvalidData));
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addTests({&PhongGLTest::setPerVertexJointCountInvalid,
              &PhongGLTest::setUniformUniformBuffersEnabled,
              &PhongGLTest::bindBufferUniformBuffersNotEnabled});
    #endif

    addInstancedTests({&PhongGLTest::bindTexturesInvalid},
        Containers::arraySize(BindTexturesInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::bindTextureArraysInvalid},
        Containers::arraySize(BindTextureArraysInvalidData));
    #endif

    addTests({
        &PhongGLTest::setAlphaMaskNotEnabled,
        &PhongGLTest::setSpecularDisabled,
        &PhongGLTest::setTextureMatrixNotEnabled,
        &PhongGLTest::setNormalTextureScaleNotEnabled,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::setTextureLayerNotArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::bindTextureTransformBufferNotEnabled,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::setObjectIdNotEnabled,
        #endif
        &PhongGLTest::setWrongLightCountOrId,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::setWrongJointCountOrId,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::setWrongDrawOffset
        #endif
    });

    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderDefaults,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderDefaults<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderDefaults<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderColored,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderColored<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderColored<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderColoredData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderSinglePixelTextured,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderSinglePixelTextured<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderSinglePixelTextured<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderSinglePixelTexturedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderTextured,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderTextured<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderTextured<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderTexturedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderTexturedNormal,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderTexturedNormal<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderTexturedNormal<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderTexturedNormalData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderVertexColor<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderVertexColor<Color3, PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderVertexColor<Color3, PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &PhongGLTest::renderVertexColor<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderVertexColor<Color4, PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderVertexColor<Color4, PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderShininess,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderShininess<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderShininess<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderShininessData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderAlpha,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderAlpha<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderAlpha<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderAlphaData),
        &PhongGLTest::renderAlphaSetup,
        &PhongGLTest::renderAlphaTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderObjectId,
        &PhongGLTest::renderObjectId<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderObjectId<PhongGL::Flag::ShaderStorageBuffers>
        #endif
        },
        Containers::arraySize(RenderObjectIdData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderLights,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderLights<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderLights<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderLightsData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addTests({&PhongGLTest::renderLightsSetOneByOne,
              &PhongGLTest::renderLowLightAngle},
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::renderLightCulling},
        Containers::arraySize(RenderLightCullingData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderZeroLights,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderZeroLights<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderZeroLights<PhongGL::Flag::ShaderStorageBuffers>
        #endif
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests<PhongGLTest>({&PhongGLTest::renderDoubleSided},
        Containers::arraySize(RenderDoubleSidedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderSkinning,
        &PhongGLTest::renderSkinning<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderSkinning<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        },
        Containers::arraySize(RenderSkinningData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderInstanced,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderInstanced<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderInstanced<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderInstancedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderInstancedSkinning,
        &PhongGLTest::renderInstancedSkinning<PhongGL::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &PhongGLTest::renderInstancedSkinning<PhongGL::Flag::ShaderStorageBuffers>,
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::renderMulti},
        Containers::arraySize(RenderMultiData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderMultiSkinning},
        Containers::arraySize(RenderMultiSkinningData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);
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

void PhongGLTest::construct() {
    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags)
        .setLightCount(data.lightCount, data.perDrawLightCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.lightCount(), data.lightCount);
    CORRADE_COMPARE(shader.perDrawLightCount(), data.perDrawLightCount);
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
void PhongGLTest::constructSkinning() {
    auto&& data = ConstructSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags)
        .setLightCount(data.lightCount)
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

void PhongGLTest::constructAsync() {
    PhongGL::CompileState state = PhongGL::compile(PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::SpecularTexture|PhongGL::Flag::InstancedTextureOffset)
        .setLightCount(3, 2)
        /* Skinning properties tested in constructUniformBuffersAsync(), as
           there we don't need to bother with ES2 */
    );
    CORRADE_COMPARE(state.flags(), PhongGL::Flag::SpecularTexture|PhongGL::Flag::InstancedTextureOffset);
    CORRADE_COMPARE(state.lightCount(), 3);
    CORRADE_COMPARE(state.perDrawLightCount(), 2);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    PhongGL shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), PhongGL::Flag::SpecularTexture|PhongGL::Flag::InstancedTextureOffset);
    CORRADE_COMPARE(shader.lightCount(), 3);
    CORRADE_COMPARE(shader.perDrawLightCount(), 2);
    CORRADE_VERIFY(shader.isLinkFinished());
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
void PhongGLTest::constructUniformBuffers() {
    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::ObjectId || data.jointCount) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= PhongGL::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
    #endif

    if(data.flags >= PhongGL::Flag::MultiDraw) {
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

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags)
        .setLightCount(data.lightCount, data.perDrawLightCount)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.lightCount(), data.lightCount);
    CORRADE_COMPARE(shader.perDrawLightCount(), data.perDrawLightCount);
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

void PhongGLTest::constructUniformBuffersAsync() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    PhongGL::CompileState state = PhongGL::compile(PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling)
        /* SwiftShader has 256 uniform vectors at most, per-3D-draw is 4+4,
           per-material 4, per-light 4, per joint 4 plus 4 for projection */
        .setLightCount(2, 1)
        .setMaterialCount(5)
        .setDrawCount(24)
        .setJointCount(7, 3, 4));
    CORRADE_COMPARE(state.flags(), PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling);
    CORRADE_COMPARE(state.lightCount(), 2);
    CORRADE_COMPARE(state.perDrawLightCount(), 1);
    CORRADE_COMPARE(state.materialCount(), 5);
    CORRADE_COMPARE(state.drawCount(), 24);
    CORRADE_COMPARE(state.jointCount(), 7);
    CORRADE_COMPARE(state.perVertexJointCount(), 3);
    CORRADE_COMPARE(state.secondaryPerVertexJointCount(), 4);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    PhongGL shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling);
    CORRADE_COMPARE(shader.lightCount(), 2);
    CORRADE_COMPARE(shader.perDrawLightCount(), 1);
    CORRADE_COMPARE(shader.materialCount(), 5);
    CORRADE_COMPARE(shader.drawCount(), 24);
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

void PhongGLTest::constructMove() {
    PhongGL a{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::AlphaMask)
        .setLightCount(3, 2)
        /* Skinning properties tested in constructMoveUniformBuffers(), as
           there we don't need to bother with ES2 */
    };
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    PhongGL b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), PhongGL::Flag::AlphaMask);
    CORRADE_COMPARE(b.lightCount(), 3);
    CORRADE_COMPARE(b.perDrawLightCount(), 2);
    CORRADE_VERIFY(!a.id());

    PhongGL c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), PhongGL::Flag::AlphaMask);
    CORRADE_COMPARE(c.lightCount(), 3);
    CORRADE_COMPARE(c.perDrawLightCount(), 2);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::constructMoveUniformBuffers() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    PhongGL a{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers)
        .setLightCount(5, 3)
        .setMaterialCount(2)
        .setDrawCount(5)
        .setJointCount(16, 4, 3)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    PhongGL b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), PhongGL::Flag::UniformBuffers);
    CORRADE_COMPARE(b.lightCount(), 5);
    CORRADE_COMPARE(b.perDrawLightCount(), 3);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_COMPARE(b.jointCount(), 16);
    CORRADE_COMPARE(b.perVertexJointCount(), 4);
    CORRADE_COMPARE(b.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!a.id());

    PhongGL c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), PhongGL::Flag::UniformBuffers);
    CORRADE_COMPARE(c.lightCount(), 5);
    CORRADE_COMPARE(c.perDrawLightCount(), 3);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_COMPARE(c.jointCount(), 16);
    CORRADE_COMPARE(c.perVertexJointCount(), 4);
    CORRADE_COMPARE(c.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!b.id());
}
#endif

void PhongGLTest::constructInvalid() {
    auto&& data = ConstructInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    PhongGL{PhongGL::Configuration{}
        .setFlags(data.flags)
        .setLightCount(data.lightCount, data.perDrawLightCount)
        #ifndef MAGNUM_TARGET_GLES2
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        #endif
    };
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::PhongGL: {}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::constructUniformBuffersInvalid() {
    auto&& data = ConstructUniformBuffersInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    Containers::String out;
    Error redirectError{&out};
    PhongGL{PhongGL::Configuration{}
        .setFlags(data.flags)
        .setLightCount(data.lightCount, data.perDrawLightCount)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::PhongGL: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setPerVertexJointCountInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    PhongGL a{PhongGL::Configuration{}};
    PhongGL b{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::DynamicPerVertexJointCount)
        .setJointCount(16, 3, 2)};

    Containers::String out;
    Error redirectError{&out};
    a.setPerVertexJointCount(3, 2);
    b.setPerVertexJointCount(4);
    b.setPerVertexJointCount(3, 3);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled\n"
        "Shaders::PhongGL::setPerVertexJointCount(): expected at most 3 per-vertex joints, got 4\n"
        "Shaders::PhongGL::setPerVertexJointCount(): expected at most 2 secondary per-vertex joints, got 3\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setUniformUniformBuffersEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader
        /* setPerVertexJointCount() works on both UBOs and classic */
        .setAmbientColor({})
        .setDiffuseColor({})
        .setNormalTextureScale({})
        .setSpecularColor({})
        .setShininess({})
        .setAlphaMask({})
        .setObjectId({})
        .setTransformationMatrix({})
        .setNormalMatrix({})
        .setProjectionMatrix({})
        .setTextureMatrix({})
        .setTextureLayer({})
        .setLightPositions(std::initializer_list<Vector4>{})
        .setLightPosition(0, Vector4{})
        .setLightColors(std::initializer_list<Color3>{})
        .setLightColor(0, Color3{})
        .setLightSpecularColors({})
        .setLightSpecularColor(0, {})
        .setLightRanges({})
        .setLightRange(0, {})
        .setJointMatrices({})
        .setJointMatrix(0, {})
        .setPerInstanceJointCount(0);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setAmbientColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setDiffuseColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setNormalTextureScale(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setSpecularColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setShininess(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setAlphaMask(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setObjectId(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setTransformationMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setNormalMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setTextureMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setTextureLayer(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightPositions(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightPosition(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightColors(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightSpecularColors(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightSpecularColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightRanges(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setLightRange(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setJointMatrices(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setJointMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::PhongGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled\n");
}

void PhongGLTest::bindBufferUniformBuffersNotEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Buffer buffer;
    PhongGL shader;

    Containers::String out;
    Error redirectError{&out};
    shader.bindProjectionBuffer(buffer)
          .bindProjectionBuffer(buffer, 0, 16)
          .bindTransformationBuffer(buffer)
          .bindTransformationBuffer(buffer, 0, 16)
          .bindDrawBuffer(buffer)
          .bindDrawBuffer(buffer, 0, 16)
          .bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16)
          .bindMaterialBuffer(buffer)
          .bindMaterialBuffer(buffer, 0, 16)
          .bindLightBuffer(buffer)
          .bindLightBuffer(buffer, 0, 16)
          .bindJointBuffer(buffer)
          .bindJointBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::PhongGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}
#endif

void PhongGLTest::bindTexturesInvalid() {
    auto&& data = BindTexturesInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindAmbientTexture(texture)
          .bindDiffuseTexture(texture)
          .bindSpecularTexture(texture)
          .bindNormalTexture(texture)
          #ifndef MAGNUM_TARGET_GLES2
          .bindObjectIdTexture(texture)
          #endif
          .bindTextures(&texture, &texture, &texture, &texture);

    CORRADE_COMPARE(out, data.message);
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::bindTextureArraysInvalid() {
    auto&& data = BindTextureArraysInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2DArray textureArray;
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindAmbientTexture(textureArray)
          .bindDiffuseTexture(textureArray)
          .bindSpecularTexture(textureArray)
          .bindNormalTexture(textureArray)
          .bindObjectIdTexture(textureArray);
    CORRADE_COMPARE(out, data.message);
}
#endif

void PhongGLTest::setAlphaMaskNotEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setAlphaMask(0.75f);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

void PhongGLTest::setSpecularDisabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Texture2D texture;
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::NoSpecular)};

    Containers::String out;
    Error redirectError{&out};
    shader.setSpecularColor({})
        .setShininess({})
        /* {{}} makes GCC 4.8 warn about zero as null pointer constant */
        .setLightSpecularColors({Color3{}})
        .setLightSpecularColor(0, {});
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setSpecularColor(): the shader was created with specular disabled\n"
        "Shaders::PhongGL::setShininess(): the shader was created with specular disabled\n"
        "Shaders::PhongGL::setLightSpecularColors(): the shader was created with specular disabled\n"
        "Shaders::PhongGL::setLightSpecularColor(): the shader was created with specular disabled\n");
}

void PhongGLTest::setTextureMatrixNotEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureMatrix({});
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

void PhongGLTest::setNormalTextureScaleNotEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setNormalTextureScale({});
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setNormalTextureScale(): the shader was not created with normal texture enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setTextureLayerNotArray() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureLayer(37);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::bindTextureTransformBufferNotEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    GL::Buffer buffer{GL::Buffer::TargetHint::Uniform};
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setObjectIdNotEnabled() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setObjectId(33376);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

void PhongGLTest::setWrongLightCountOrId() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader{PhongGL::Configuration{}
        .setLightCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader
        .setLightColors({Color3{}})
        .setLightPositions({Vector4{}})
        .setLightRanges({0.0f})
        .setLightColor(5, Color3{})
        .setLightPosition(5, Vector4{})
        .setLightRange(5, 0.0f);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setLightColors(): expected 5 items but got 1\n"
        "Shaders::PhongGL::setLightPositions(): expected 5 items but got 1\n"
        "Shaders::PhongGL::setLightRanges(): expected 5 items but got 1\n"
        "Shaders::PhongGL::setLightColor(): light ID 5 is out of range for 5 lights\n"
        "Shaders::PhongGL::setLightPosition(): light ID 5 is out of range for 5 lights\n"
        "Shaders::PhongGL::setLightRange(): light ID 5 is out of range for 5 lights\n");
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setWrongJointCountOrId() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL shader{PhongGL::Configuration{}
        .setJointCount(5, 1)};

    Containers::String out;
    Error redirectError{&out};
    /* Calling setJointMatrices() with less items is fine, tested in
       renderSkinning() */
    shader.setJointMatrices({{}, {}, {}, {}, {}, {}})
        .setJointMatrix(5, Matrix4{});
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setJointMatrices(): expected at most 5 items but got 6\n"
        "Shaders::PhongGL::setJointMatrix(): joint ID 5 is out of range for 5 joints\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setWrongDrawOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers)
        .setLightCount(1)
        .setMaterialCount(2)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::PhongGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
}
#endif

constexpr Vector2i RenderSize{80, 80};

void PhongGLTest::renderSetup() {
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
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
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
                {PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}}
                /* ObjectIdOutput is mapped (and cleared) in test cases that
                   actually draw to it, otherwise it causes an error on WebGL
                   due to the shader not rendering to all outputs */
            });
    }
    #endif
}

void PhongGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    _objectId = GL::Renderbuffer{NoCreate};
    #endif
}

template<PhongGL::Flag flag> void PhongGLTest::renderDefaults() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flag)};

    if(flag == PhongGL::Flag{}) {
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has 6 different pixels on the edges and a bunch of small
       rounding errors */
    const Float maxThreshold = 31.0f, meanThreshold = 0.122f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 31.0f, meanThreshold = 4.142f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderColored() {
    auto&& data = RenderColoredData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flag)
        .setLightCount(data.lightCount, data.perDrawLightCount)};

    if(flag == PhongGL::Flag{}) {
        Color3 lightColors[]{
            data.lightColor1,
            data.lightColor2,
            {},
            {}
        };
        Vector4 lightPositions[]{
            {data.lightPosition1, -3.0f, 2.0f, 0.0f},
            {data.lightPosition2, -3.0f, 2.0f, 0.0f},
            {},
            {}
        };

        shader
            .setLightColors(Containers::arrayView(lightColors)
                .prefix(data.lightCount))
            .setLightPositions(Containers::arrayView(lightPositions)
                .prefix(data.lightCount))
            .setAmbientColor(0x330033_rgbf)
            .setDiffuseColor(0xccffcc_rgbf)
            .setSpecularColor(0x6666ff_rgbf)
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f))*
                                     Matrix4::rotationY(data.rotation))
            .setNormalMatrix(Matrix4::rotationY(data.rotation).normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .draw(sphere);

    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
                .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f))*
                                         Matrix4::rotationY(data.rotation))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
                .setNormalMatrix(Matrix4::rotationY(data.rotation).normalMatrix())
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0x330033_rgbf)
                .setDiffuseColor(0xccffcc_rgbf)
                .setSpecularColor(0x6666ff_rgbf)
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
                .setPosition({data.lightPosition1, -3.0f, 2.0f, 0.0f})
                .setColor(data.lightColor1),
            PhongLightUniform{}
                .setPosition({data.lightPosition2, -3.0f, 2.0f, 0.0f})
                .setColor(data.lightColor2),
            PhongLightUniform{},
            PhongLightUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has some minor rounding differences (max = 1). ARM Mali G71
       and Apple A8 has bigger rounding differences. NVidia as well, more on
       ES2. */
    const Float maxThreshold = 12.67f, meanThreshold = 0.121f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/colored.tga"),
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

template<PhongGL::Flag flag> void PhongGLTest::renderSinglePixelTextured() {
    auto&& data = RenderSinglePixelTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & PhongGL::Flag::TextureTransformation ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    PhongGL::Flags flags = PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    #endif
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flags)
        /* Different count and per-draw count tested in renderColored() */
        .setLightCount(2)};

    const Color4ub ambientData[]{ 0x330033_rgb };
    ImageView2D ambientImage{PixelFormat::RGBA8Unorm, Vector2i{1}, ambientData};

    const Color4ub diffuseData[]{ 0xccffcc_rgb };
    ImageView2D diffuseImage{PixelFormat::RGBA8Unorm, Vector2i{1}, diffuseData};

    const Color4ub specularData[]{ 0x6666ff_rgb };
    ImageView2D specularImage{PixelFormat::RGBA8Unorm, Vector2i{1}, specularData};

    GL::Texture2D ambient;
    GL::Texture2D diffuse;
    GL::Texture2D specular;
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray ambientArray{NoCreate};
    GL::Texture2DArray diffuseArray{NoCreate};
    GL::Texture2DArray specularArray{NoCreate};
    if(data.flags & PhongGL::Flag::TextureArrays) {
        ambientArray = GL::Texture2DArray{};
        ambientArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector3i{1, 1, data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, ambientImage);
        diffuseArray = GL::Texture2DArray{};
        diffuseArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector3i{1, 1, data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, diffuseImage);
        specularArray = GL::Texture2DArray{};
        specularArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector3i{1, 1, data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, specularImage);
        shader
            .bindAmbientTexture(ambientArray)
            .bindDiffuseTexture(diffuseArray)
            .bindSpecularTexture(specularArray);
        if(!(flag & PhongGL::Flag::UniformBuffers) && data.layer != 0)
            shader.setTextureLayer(data.layer); /* to verify the default */
    } else
    #endif
    {
        ambient = GL::Texture2D{};
        ambient.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector2i{1})
            .setSubImage(0, {}, ambientImage);
        diffuse = GL::Texture2D{};
        diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector2i{1})
            .setSubImage(0, {}, diffuseImage);
        specular = GL::Texture2D{};
        specular.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGBA, Vector2i{1})
            .setSubImage(0, {}, specularImage);
        if(data.multiBind)
            shader.bindTextures(&ambient, &diffuse, &specular, nullptr);
        else shader
            .bindAmbientTexture(ambient)
            .bindDiffuseTexture(diffuse)
            .bindSpecularTexture(specular);
    }

    if(flag == PhongGL::Flag{}) {
        shader.setLightColors({0x993366_rgbf, 0x669933_rgbf})
            .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                                { 3.0f, -3.0f, 2.0f, 0.0f}})
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
                .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setLayer(data.layer)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                /* Has to be set because the default is black regardless of
                   whether the texture is present or not (it has no way to
                   know) */
                .setAmbientColor(0xffffff_rgbf)
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
                .setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x993366_rgbf),
            PhongLightUniform{}
                .setPosition({ 3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x669933_rgbf)
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has some minor rounding differences (max = 1). ARM Mali G71
       and Apple A8 has bigger rounding differences. NVidia as well, more on
       ES2. */
    const Float maxThreshold = 12.67f, meanThreshold = 0.125f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/colored.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderTextured() {
    auto&& data = RenderTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & PhongGL::Flag::TextureTransformation ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    PhongGL::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    #endif
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flags)
        /* Different count and per-draw count tested in renderColored() */
        .setLightCount(2)};

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D ambient{NoCreate};
    GL::Texture2D diffuse{NoCreate};
    GL::Texture2D specular{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray ambientArray{NoCreate};
    GL::Texture2DArray diffuseArray{NoCreate};
    GL::Texture2DArray specularArray{NoCreate};
    #endif
    if(data.flags & PhongGL::Flag::AmbientTexture) {
        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/ambient-texture.tga")) && (image = importer->image2D(0)));

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & PhongGL::Flag::TextureArrays) {
            ambientArray = GL::Texture2DArray{};
            ambientArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, {image->size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, ImageView2D{*image});
            shader.bindAmbientTexture(ambientArray);
        } else
        #endif
        {
            ambient = GL::Texture2D{};
            ambient.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindAmbientTexture(ambient);
        }
    }

    /* If no diffuse texture is present, dial down the default diffuse color
       so ambient/specular is visible */
    if(data.flags & PhongGL::Flag::DiffuseTexture) {
        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & PhongGL::Flag::TextureArrays) {
            diffuseArray = GL::Texture2DArray{};
            diffuseArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, {image->size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, ImageView2D{*image});
            shader.bindDiffuseTexture(diffuseArray);
        } else
        #endif
        {
            diffuse = GL::Texture2D{};
            diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindDiffuseTexture(diffuse);
        }
    }

    if(data.flags & PhongGL::Flag::SpecularTexture) {
        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/specular-texture.tga")) && (image = importer->image2D(0)));

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & PhongGL::Flag::TextureArrays) {
            specularArray = GL::Texture2DArray{};
            specularArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, {image->size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, ImageView2D{*image});
            shader.bindSpecularTexture(specularArray);
        } else
        #endif
        {
            specular = GL::Texture2D{};
            specular.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindSpecularTexture(specular);
        }
    }

    if(flag == PhongGL::Flag{}) {
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        if(data.flags & PhongGL::Flag::AmbientTexture)
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured() */
            shader.setAmbientColor(0xff9999_rgbf);
        if(data.flags & PhongGL::Flag::DiffuseTexture)
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured() */
            shader.setDiffuseColor(0x9999ff_rgbf);
        else shader.setDiffuseColor(0x333333_rgbf);
        if(data.flags & PhongGL::Flag::SpecularTexture)
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured() */
            shader.setSpecularColor(0x99ff99_rgbf);
        #ifndef MAGNUM_TARGET_GLES2
        if(data.layer != 0) /* to verify the default */
            shader.setTextureLayer(data.layer);
        #endif

        /* Using default (white) light colors to have the texture data visible
           better */
        shader.setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                                  { 3.0f, -3.0f, 2.0f, 0.0f}})
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setNormalMatrix((Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)).normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}.setNormalMatrix(
                (Matrix4::rotationY(-15.0_degf)*
                 Matrix4::rotationX(15.0_degf)).normalMatrix()
            )
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layer)
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}.setPosition({-3.0f, -3.0f, 2.0f, 0.0f}),
            PhongLightUniform{}.setPosition({3.0f, -3.0f, 2.0f, 0.0f})
        }};

        PhongMaterialUniform materialUniformData[1];
        if(data.flags & PhongGL::Flag::AmbientTexture)
            materialUniformData->setAmbientColor(0xff9999_rgbf);
        if(data.flags & PhongGL::Flag::DiffuseTexture)
            materialUniformData->setDiffuseColor(0x9999ff_rgbf);
        else
            materialUniformData->setDiffuseColor(0x333333_rgbf);
        if(data.flags & PhongGL::Flag::SpecularTexture)
            materialUniformData->setSpecularColor(0x99ff99_rgbf);
        GL::Buffer materialUniform{materialUniformData};

        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has few rounding errors at the edges (giving a large max
       error), but that's basically it. Apple A8 has more. */
    const Float maxThreshold = 227.0f, meanThreshold = 0.202f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's a bit worse */
    const Float maxThreshold = 227.0f, meanThreshold = 3.434f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "PhongTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderTexturedNormal() {
    auto&& data = RenderTexturedNormalData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & PhongGL::Flag::TextureTransformation ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    /* Normal texture. Flip normal Y, if requested */
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/normal-texture.tga")) && (image = importer->image2D(0)));
    if(data.flipNormalY) for(auto row: image->mutablePixels<Color3ub>())
        for(Color3ub& pixel: row)
            pixel.y() = 255 - pixel.y();

    PhongGL::Flags flags = PhongGL::Flag::NormalTexture|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    #endif
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flags)
        /* Different count and per-draw count tested in renderColored() */
        .setLightCount(2)};

    GL::Texture2D normal{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray normalArray{NoCreate};
    if(data.flags & PhongGL::Flag::TextureArrays) {
        normalArray = GL::Texture2DArray{};
        normalArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, {image->size(), data.layer + 1})
            .setSubImage(0, {0, 0, data.layer}, ImageView2D{*image});
        shader.bindNormalTexture(normalArray);
    } else
    #endif
    {
        normal = GL::Texture2D{};
        normal.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, image->size())
            .setSubImage(0, {}, *image);
        if(data.multiBind)
            shader.bindTextures(nullptr, nullptr, nullptr, &normal);
        else
            shader.bindNormalTexture(normal);
    }

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid( Primitives::PlaneFlag::TextureCoordinates));

    /* Add tangents / bitangents of desired component count. Unused components
       are set to zero to ensure the shader doesn't use them. */
    const struct TangentBitangent {
        Vector4 tangent;
        Vector3 bitangent;
    } tangentBitangent{data.tangent, data.bitangent};
    GL::Buffer tangents;
    tangents.setData(Containers::Array<TangentBitangent>{DirectInit, 4, tangentBitangent});
    plane.addVertexBuffer(tangents, 0, sizeof(TangentBitangent),
        GL::DynamicAttribute{Shaders::PhongGL::Tangent4{data.tangentComponents}});
    plane.addVertexBuffer(Utility::move(tangents), sizeof(Vector4),
        sizeof(TangentBitangent),
        GL::DynamicAttribute{Shaders::PhongGL::Bitangent{}});

    /* Rotating the view a few times (together with light positions). If the
       tangent transformation in the shader is correct, it should result in
       exactly the same images. */
    if(flag == PhongGL::Flag{}) {
        /* Verify the defaults are working properly */
        if(data.scale != 1.0f)
            shader.setNormalTextureScale(data.scale);
        #ifndef MAGNUM_TARGET_GLES2
        if(data.layer != 0)
            shader.setTextureLayer(data.layer);
        #endif

        shader.setLightPositions({
                Matrix4::rotationZ(data.rotation)*Vector4{-3.0f, -3.0f, 2.0f, 0.0f},
                Matrix4::rotationZ(data.rotation)*Vector4{ 3.0f, -3.0f, 2.0f, 0.0f}})
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.35f))*
                Matrix4::rotationZ(data.rotation)*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setNormalMatrix((Matrix4::rotationZ(data.rotation)*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)).normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setDiffuseColor(0x999999_rgbf)
            .draw(plane);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.35f))*
                Matrix4::rotationZ(data.rotation)*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}.setNormalMatrix(
                (Matrix4::rotationZ(data.rotation)*
                 Matrix4::rotationY(-15.0_degf)*
                 Matrix4::rotationX(15.0_degf)).normalMatrix()
            )
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setDiffuseColor(0x999999_rgbf)
                .setNormalTextureScale(data.scale)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setLayer(data.layer)
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}.setPosition(Matrix4::rotationZ(data.rotation)*Vector4{-3.0f, -3.0f, 2.0f, 0.0f}),
            PhongLightUniform{}.setPosition(Matrix4::rotationZ(data.rotation)*Vector4{3.0f, -3.0f, 2.0f, 0.0f})
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(plane);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D actual = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});
    Containers::StridedArrayView2D<Color3ub> pixels =
        /* Dropping the alpha channel, as it's always 1.0 */
        actual.pixels<Color4ub>().slice(&Color4ub::rgb);

    /* Rotate pixels back to upright position so we can compare with the 0°
       file and ensure the tangent calculation is transformation invariant */
    if(data.rotation == -90.0_degf)
        pixels = pixels.flipped<0>().transposed<0, 1>();
    else if(data.rotation == 90.0_degf)
        pixels = pixels.flipped<1>().transposed<0, 1>();
    else CORRADE_COMPARE(data.rotation, 0.0_degf);

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* One pixel in the center didn't survive the transformation. But that's
       okay. Due to the density of the normal map, SwiftShader has an overally
       consistent off-by-a-bit error. AMD macOS drivers have one pixel off
       due to a rounding error on the edge. Apple A8 has a slightly larger
       overall difference; llvmpipe is off also. */
    const Float maxThreshold = 191.0f, meanThreshold = 0.918f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 191.0f, meanThreshold = 3.017f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Path::join({_testDir, "PhongTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, PhongGL::Flag flag> void PhongGLTest::renderVertexColor() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
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

    /* Highlight the pole vertices and the middle rings */
    Containers::Array<T> colorData{DirectInit, sphereData.vertexCount(), 0x999999_rgbf};
    for(std::size_t i = 0; i != 3*33 + 1; ++i)
        colorData[sphereData.vertexCount()  - i - 1] = 0xff0000_rgbf*5.0f;
    for(std::size_t i = 6*33; i != 9*33; ++i)
        colorData[i + 1] = 0xffff99_rgbf*1.5f;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<Shaders::PhongGL::Color3::Location, T>{});

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D diffuse;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::DiffuseTexture|PhongGL::Flag::VertexColor|flag)
        /* Different count and per-draw count tested in renderColored() */
        .setLightCount(2)};
    shader.bindDiffuseTexture(diffuse);

    if(flag == PhongGL::Flag{}) {
        shader
            .setLightPositions({{-3.0f, -3.0f, 0.0f, 0.0f},
                                { 3.0f, -3.0f, 0.0f, 0.0f}})
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setNormalMatrix((Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)).normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setAmbientColor(0x111111_rgbf)
            .setDiffuseColor(0x9999ff_rgbf)
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}.setNormalMatrix(
                (Matrix4::rotationY(-15.0_degf)*
                 Matrix4::rotationX(15.0_degf)).normalMatrix()
            )
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}.setPosition({-3.0f, -3.0f, 0.0f, 0.0f}),
            PhongLightUniform{}.setPosition({ 3.0f, -3.0f, 0.0f, 0.0f})
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0x111111_rgbf)
                .setDiffuseColor(0x9999ff_rgbf)
        }};
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has some minor differences on the edges, Apple A8 a bit
       more */
    const Float maxThreshold = 115.4f, meanThreshold = 0.167f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's worse */
    const Float maxThreshold = 115.4f, meanThreshold = 3.254f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/vertexColor.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderShininess() {
    auto&& data = RenderShininessData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flag|data.flags)};
    if(flag == PhongGL::Flag{}) {
        if(!(data.flags & PhongGL::Flag::NoSpecular)) shader
            .setSpecularColor(data.specular)
            .setShininess(data.shininess);
        shader
            .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f}})
            .setDiffuseColor(0xff3333_rgbf)
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}.setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setDiffuseColor(0xff3333_rgbf)
                .setSpecularColor(data.specular) /* ignored if NoSpecular */
                .setShininess(data.shininess) /* ignored if NoSpecular */
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        #ifdef CORRADE_TARGET_IOS
        /* Apple A8 has a large single-pixel difference in the shininess ~= 0
           case, but it's not nearly as bad as in the "huge ring" case on Mesa
           etc. */
        const Float maxThreshold = 211.0f, meanThreshold = 0.052f;
        #elif !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has some minor rounding differences (max = 1.67). ARM
           Mali G71 has bigger rounding differences. */
        const Float maxThreshold = 221.0f, meanThreshold = 0.106f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 16.667f, meanThreshold = 2.583f;
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        CORRADE_EXPECT_FAIL_IF(data.shininess <= 0.0011f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader),
            "SwiftShader has a much larger ring for the overflown shininess.");
        #endif
        #if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES2)
        CORRADE_EXPECT_FAIL_IF(data.shininess == 0.0f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::ArmMali),
            "ARM Mali has a much larger ring for the overflown shininess when it's exactly 0.");
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        CORRADE_EXPECT_FAIL_IF(data.shininess == 0.0f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Mesa) && GL::Context::current().rendererString().contains("AMD"),
            "AMD Mesa drivers have a much larger ring for the overflown shininess when it's exactly 0.");
        CORRADE_EXPECT_FAIL_IF(data.shininess <= 0.0011f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Mesa) && GL::Context::current().rendererString().contains("llvmpipe"),
            "Mesa llvmpipe drivers have a much larger ring for the overflown shininess.");
        #endif
        #if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
        CORRADE_EXPECT_FAIL_IF(data.shininess == 0.0f && GL::Context::current().rendererString().contains("AMD"),
            "AMD on macOS has a much larger ring for the overflown shininess when it's exactly 0.");
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join({_testDir, "PhongTestFiles", data.expected}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test the special overflow results as well */
    if(false
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        || (data.shininess <= 0.0011f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader))
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        || (data.shininess == 0.0f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Mesa) && GL::Context::current().rendererString().contains("AMD"))
        #endif
        #if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
        || (data.shininess == 0.0f && GL::Context::current().rendererString().contains("AMD"))
        #endif
        #if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES2)
        || (data.shininess == 0.0f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::ArmMali))
        #endif
    ) {
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join({_testDir, "PhongTestFiles", "shininess0-overflow.tga"}),
            /* The threshold = 0.001 case has a slight reddish tone on
               SwiftShader; ARM Mali has one pixel off */
            (DebugTools::CompareImageToFile{_manager, 255.0f, 23.1f}));
    }
}

void PhongGLTest::renderAlphaSetup() {
    renderSetup();
    if(RenderAlphaData[testCaseInstanceId()].blending)
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add);
}

void PhongGLTest::renderAlphaTeardown() {
    if(RenderAlphaData[testCaseInstanceId()].blending)
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    renderTeardown();
}

template<PhongGL::Flag flag> void PhongGLTest::renderAlpha() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::Optional<Trade::ImageData2D> image;
    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D ambient;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join({_testDir, "TestFiles", data.ambientTexture})) && (image = importer->image2D(0)));
    ambient.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, image->size())
        .setSubImage(0, {}, *image);

    GL::Texture2D diffuse;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join({_testDir, "TestFiles", data.diffuseTexture})) && (image = importer->image2D(0)));
    diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge);

    /* In some instances the diffuse texture is just three-component, handle
       that properly */
    if(image->format() == PixelFormat::RGBA8Unorm)
        diffuse.setStorage(1, TextureFormatRGBA, image->size());
    else {
        CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
        diffuse.setStorage(1, TextureFormatRGB, image->size());
    }
    diffuse.setSubImage(0, {}, *image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags|flag)
        /* Different count and per-draw count tested in renderColored() */
        .setLightCount(2)};
    shader.bindTextures(&ambient, &diffuse, nullptr, nullptr);

    if(flag == PhongGL::Flag{}) {
        shader.setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                                  { 3.0f, -3.0f, 2.0f, 0.0f}})
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setNormalMatrix((Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)).normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setAmbientColor(data.ambientColor)
            .setDiffuseColor(data.diffuseColor)
            .setSpecularColor(0xffffff00_rgbaf);

        /* Test that the default is correct by not setting the threshold if
           it's equal to the default */
        if(data.flags & PhongGL::Flag::AlphaMask && data.threshold != 0.5f)
            shader.setAlphaMask(data.threshold);

        /* For proper Z order draw back faces first and then front faces */
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
        shader.draw(sphere);
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}.setNormalMatrix(
                (Matrix4::rotationY(-15.0_degf)*
                 Matrix4::rotationX(15.0_degf)).normalMatrix()
            )
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}.setPosition({-3.0f, -3.0f, 2.0f, 0.0f}),
            PhongLightUniform{}.setPosition({3.0f, -3.0f, 2.0f, 0.0f})
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(data.ambientColor)
                .setDiffuseColor(data.diffuseColor)
                .setSpecularColor(0xffffff00_rgbaf)
                .setAlphaMask(data.threshold)
        }};
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform);

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
    /* In some cases (separate vs combined alpha) there are off-by-one errors.
       That's okay, as we have only 8bit texture precision. SwiftShader has
       additionally a few minor rounding errors at the edges, Apple A8 a bit
       more. */
    const Float maxThreshold = 189.4f, meanThreshold = 0.385f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 189.4f, meanThreshold = 4.736f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, data.expected),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
template<PhongGL::Flag flag> void PhongGLTest::renderObjectId() {
    auto&& data = RenderObjectIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & PhongGL::Flag::TextureTransformation ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    Primitives::UVSphereFlags sphereFlags;
    if(data.flags & PhongGL::Flag::ObjectIdTexture)
        sphereFlags |= Primitives::UVSphereFlag::TextureCoordinates;
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32, sphereFlags));

    PhongGL::Flags flags = data.flags|flag;
    if(flag & PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::ObjectId|flags)
        /* Different count and per-draw count tested in renderColored() */
        .setLightCount(2)};

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags >= PhongGL::Flag::ObjectIdTexture) {
        const UnsignedShort imageData[]{
            100, 200, 300, 400
        };
        ImageView2D image{PixelFormat::R16UI, {2, 2}, imageData};

        if(data.flags & PhongGL::Flag::TextureArrays) {
            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, {image.size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, image);
            shader.bindObjectIdTexture(textureArray);
            if(!(flag & PhongGL::Flag::UniformBuffers) && data.layer != 0)
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
            {PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {PhongGL::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    if(flag == PhongGL::Flag{}) {
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader
            .setLightColors({0x993366_rgbf, 0x669933_rgbf})
            .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                                { 3.0f, -3.0f, 2.0f, 0.0f}})
            .setAmbientColor(0x330033_rgbf)
            .setDiffuseColor(0xccffcc_rgbf)
            .setSpecularColor(0x6666ff_rgbf)
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setObjectId(40006)
            .draw(sphere);
    } else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
                .setObjectId(40006)
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
                .setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x993366_rgbf),
            PhongLightUniform{}
                .setPosition({3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x669933_rgbf)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layer)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0x330033_rgbf)
                .setDiffuseColor(0xccffcc_rgbf)
                .setSpecularColor(0x6666ff_rgbf)
        }};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Color output should have no difference -- same as in colored() */
    /* SwiftShader has some minor rounding differences (max = 1). ARM Mali G71
       and Apple A8 has bigger rounding differences. NVidia as well. */
    const Float maxThreshold = 12.67f, meanThreshold = 0.113f;
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/colored.tga"),
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

template<PhongGL::Flag flag> void PhongGLTest::renderLights() {
    auto&& data = RenderLightsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    Matrix4 transformation =
        Matrix4::translation({0.0f, 0.0f, -1.5f});

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flag)
        /* Different count and per-draw count tested in renderColored(), here
           it's testing mainly the calculation */
        .setLightCount(1)};
    if(flag == PhongGL::Flag{}) {
        shader
            /* Set non-black ambient to catch accidental NaNs -- the render
               should never be fully black */
            .setAmbientColor(0x222222_rgbf)
            .setSpecularColor(data.specularColor)
            .setLightPositions({data.position})
            .setLightColors({0xff8080_rgbf*data.intensity})
            .setLightSpecularColors({data.lightSpecularColor})
            .setShininess(60.0f)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(80.0_degf, 1.0f, 0.1f, 20.0f));
        /* Also testing a case where it's left at the default infinity value
           embedded in the shader code or passed directly during construction
           --- it should not cause any difference compared to passing
           Constants::inf(). */
        if(data.range)
            shader.setLightRanges({*data.range});
        shader.draw(plane);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(80.0_degf, 1.0f, 0.1f, 20.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(transformation)
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}.setNormalMatrix(transformation.normalMatrix())
        }};
        PhongLightUniform lightUniformData;
        lightUniformData
            .setPosition({data.position})
            .setColor(0xff8080_rgbf*data.intensity)
            .setSpecularColor(data.lightSpecularColor);
        if(data.range)
            lightUniformData.setRange(*data.range);
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            lightUniformData
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0x222222_rgbf)
                .setSpecularColor(data.specularColor)
                .setShininess(60.0f)
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(plane);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});

    /* Analytical output check. Comment this out when image comparison fails
       for easier debugging. */
    for(const auto& pick: data.picks) {
        CORRADE_ITERATION(pick.first());
        CORRADE_COMPARE_WITH(
            image.pixels<Color4ub>()[pick.first().y()][pick.first().x()].xyz(),
            pick.second(), TestSuite::Compare::around(0x010101_rgb));
    }

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        image.pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "PhongTestFiles", data.file}),
        /* Minor differences on ES2 and on NVidia */
        (DebugTools::CompareImageToFile{_manager, 3.0f, 0.27f}));
}

void PhongGLTest::renderLightsSetOneByOne() {
    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    Matrix4 transformation =
        Matrix4::translation({0.0f, 0.0f, -1.5f});

    PhongGL shader{PhongGL::Configuration{}
        .setLightCount(2)};
    shader
        /* Set non-black ambient to catch accidental NaNs -- the render should
           never be fully black */
        .setAmbientColor(0x222222_rgbf)
        /* First light is directional, from back, so it shouldn't affect the
           output at all -- we only want to test that the ID is used properly */
        .setLightPosition(0, {-1.0f, 1.5f, -0.5f, 0.0f})
        .setLightPosition(1, {0.75f, -0.75f, -0.75f, 1.0f})
        .setLightColor(0, 0x00ffff_rgbf)
        .setLightColor(1, 0xff8080_rgbf)
        .setLightSpecularColor(0, 0x0000ff_rgbf)
        .setLightSpecularColor(1, 0x80ff80_rgbf)
        .setLightRange(0, Constants::inf())
        .setLightRange(1, 1.5f)
        .setShininess(60.0f)
        .setTransformationMatrix(transformation)
        .setNormalMatrix(transformation.normalMatrix())
        .setProjectionMatrix(Matrix4::perspectiveProjection(80.0_degf, 1.0f, 0.1f, 20.0f))
        .draw(plane);

    #ifdef MAGNUM_TARGET_GLES
    {
        /* The setLightPosition(1) is the first call that causes the error.
           Works with 4.1, didn't find any commit in between that would clearly
           affect this. */
        CORRADE_EXPECT_FAIL_IF(GL::Context::current().versionString().contains("SwiftShader 4.0.0"_s),
            "SwiftShader 4.0.0 has a bug where setting array uniform elements other than 0 causes GL_INVALID_OPERATION.");
        MAGNUM_VERIFY_NO_GL_ERROR();
        if(GL::Context::current().versionString().contains("SwiftShader 4.0.0"_s))
            CORRADE_SKIP("Skipping the rest of the test.");
    }
    #endif

    Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    const Float maxThreshold = 3.0f, meanThreshold = 0.02f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 3.0f, meanThreshold = 0.02f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        image.pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "PhongTestFiles/light-point-range1.5.tga"}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void PhongGLTest::renderLowLightAngle() {
    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    Matrix4 transformation =
        Matrix4::translation({0.0f, 0.0f, -2.0f})*
        Matrix4::rotationX(-75.0_degf)*
        Matrix4::scaling(Vector3::yScale(10.0f));

    /* The light position is at the camera location, so the most light should
       be there and not at some other place. This is a repro case for a bug
       where lightDirection = normalize(lightPosition - transformedPosition)
       in the vertex shader, where the incorrect normalization caused the
       fragment-interpolated light direction being incorrect, most visible with
       long polygons and low light angles. */
    PhongGL shader{PhongGL::Configuration{}
        .setLightCount(1)};
    shader
        .setLightPositions({{0.0f, 0.1f, 0.0f, 1.0f}})
        .setShininess(200)
        .setTransformationMatrix(transformation)
        .setNormalMatrix(transformation.normalMatrix())
        .setProjectionMatrix(Matrix4::perspectiveProjection(80.0_degf, 1.0f, 0.1f, 20.0f))
        .draw(plane);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    const Float maxThreshold = 63.0f, meanThreshold = 0.36f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 63.0f, meanThreshold = 0.36f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/low-light-angle.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::renderLightCulling() {
    auto&& data = RenderLightCullingData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= PhongGL::Flag::ShaderStorageBuffers) {
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

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
        CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
        ProjectionUniform3D{}
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
    }};
    GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
        TransformationUniform3D{}
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
    }};
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
        PhongDrawUniform{}
            .setLightOffsetCount(57, 2)
    }};
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
        PhongMaterialUniform{}
            .setAmbientColor(0x330033_rgbf)
            .setDiffuseColor(0xccffcc_rgbf)
            .setSpecularColor(0x6666ff_rgbf)
    }};
    PhongLightUniform lights[64];
    lights[57] = PhongLightUniform{}
        .setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
        .setColor(0x993366_rgbf);
    lights[58] = PhongLightUniform{}
        .setPosition({3.0f, -3.0f, 2.0f, 0.0f})
        .setColor(0x669933_rgbf);
    GL::Buffer lightUniform{lights};

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling|data.flags)
        .setLightCount(data.count, data.perDrawCount)};
    shader
        .bindProjectionBuffer(projectionUniform)
        .bindTransformationBuffer(transformationUniform)
        .bindDrawBuffer(drawUniform)
        .bindMaterialBuffer(materialUniform)
        .bindLightBuffer(lightUniform)
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has some minor rounding differences (max = 1). ARM Mali G71
       and Apple A8 has bigger rounding differences. NVidia as well. */
    const Float maxThreshold = 12.67f, meanThreshold = 0.113f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/colored.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}
#endif

template<PhongGL::Flag flag> void PhongGLTest::renderZeroLights() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag >= PhongGL::Flag::UniformBuffers) {
        if(flag == PhongGL::Flag::UniformBuffers)
            setTestCaseTemplateName("Flag::UniformBuffers");
        else
            setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    /* Enable also Object ID, if supported */
    PhongGL::Flags flags = PhongGL::Flag::AmbientTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::AlphaMask;
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        flags |= PhongGL::Flag::ObjectId;
    }
    #endif
    PhongGL shader{PhongGL::Configuration{}
        .setFlags(flags|flag)
        .setLightCount(0)};

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D ambient;
    Containers::Optional<Trade::ImageData2D> ambientImage;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-alpha-texture.tga")) && (ambientImage = importer->image2D(0)));
    ambient.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, ambientImage->size())
        .setSubImage(0, {}, *ambientImage);

    shader.bindAmbientTexture(ambient);

    #ifndef MAGNUM_TARGET_GLES2
    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    _framebuffer
        .mapForDraw({
            {PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {PhongGL::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});
    #endif

    if(flag == PhongGL::Flag{}) {
        shader
            .setAmbientColor(0x9999ff_rgbf)
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            /* Keep alpha mask at the default 0.5 to test the default */
            /* Passing a zero-sized light position / color array, shouldn't
               assert */
            .setLightPositions(Containers::ArrayView<const Vector4>{})
            .setLightColors(Containers::ArrayView<const Color3>{})
            /* Using a bogus normal matrix -- it's not used so it should be
               okay. Same for all other unused values, they should get
               ignored. */
            .setNormalMatrix(Matrix3x3{Math::ZeroInit})
            .setDiffuseColor(0xfa9922_rgbf)
            .setSpecularColor(0xfa9922_rgbf)
            .setShininess(0.2f)
            .setNormalTextureScale(-0.3f);

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        #endif
        {
            shader.setObjectId(65534);
        }
        #endif

        /* For proper Z order draw back faces first and then front faces */
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
        shader.draw(sphere);
        GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf)
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
                /* Using a bogus normal matrix -- it's not used so it should be
                   okay. */
                .setNormalMatrix(Matrix3x3{Math::ZeroInit})
                .setObjectId(65534)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0x9999ff_rgbf)
                /* Same for all other unused values, they should get ignored */
                .setDiffuseColor(0xfa9922_rgbf)
                .setSpecularColor(0xfa9922_rgbf)
                .setShininess(0.2f)
                .setNormalTextureScale(-0.3f)
        }};
        /* Not binding any light buffer as it's not needed */
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
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
    /* Compared to FlatGLTest::renderAlpha3D(0.5), there's a bit more different
       pixels on the edges, caused by matrix multiplication being done in the
       shader and not on the CPU side. Apple A8 sprinkles a bunch of tiny
       differences here and there. */
    const Float maxThreshold = 139.0f, meanThreshold = 0.421f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 139.0f, meanThreshold = 2.896f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        /* Should be equivalent to masked Flat3D */
        Utility::Path::join(_testDir, "FlatTestFiles/textured3D-alpha-mask0.5.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));

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
        /* Outside of the object, cleared to 27 */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[10][10], 27);
        /* Inside of the object. Verify that it can hold 16 bits at least. */
        CORRADE_COMPARE(image.pixels<UnsignedInt>()[40][46], 65534);
    }
    #endif
}

void PhongGLTest::renderDoubleSided() {
    auto&& data = RenderDoubleSidedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData sphere = Primitives::uvSphereSolid(16, 32);

    Trade::MeshData sphereFlippedWinding = Primitives::uvSphereSolid(16, 32);
    MeshTools::flipFaceWindingInPlace(sphereFlippedWinding.mutableIndices());

    Trade::MeshData sphereFlippedNormalsWinding = Primitives::uvSphereSolid(16, 32);
    MeshTools::flipNormalsInPlace(
        sphereFlippedNormalsWinding.mutableIndices(),
        sphereFlippedNormalsWinding.mutableAttribute<Vector3>(Trade::MeshAttribute::Normal));

    /* Double-sided sphere, renders from both sides if DoubleSided is
       enabled and face culling disabled, otherwise only one depending on the
       normal direction */
    Trade::MeshData sphereDoubleSided = Primitives::uvSphereSolid(16, 32);
    if(data.flipNormals)
        MeshTools::flipNormalsInPlace(sphereDoubleSided.mutableAttribute<Vector3>(Trade::MeshAttribute::Normal));

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags)
        .setLightCount(1)};
    shader
        .setLightPositions({{-3.0f, 3.0f, 3.0f, 0.0f}})
        .setAmbientColor(0x111111_rgbf)
        .setDiffuseColor(0xff3333_rgbf)
        .setSpecularColor(0x00000000_rgbaf);

    /* Top left is a sphere from the outside, with CCW triangles, with the back
       cut off by the far plane */
    shader
        .setProjectionMatrix(Matrix4::orthographicProjection(Vector2{4.5f}, -1.0f, 0.0f))
        .setTransformationMatrix(Matrix4::translation({-1.05f, 1.05f, 0.0f}))
        .draw(MeshTools::compile(sphere));

    /* Bottom left is a sphere from the inside, with CCW triangles, with the
       front cut off by the near plane. Normals pointing outside so only top
       left should be slightly lighted. */
    shader
        .setProjectionMatrix(Matrix4::orthographicProjection(Vector2{4.5f}, 0.0f, 1.0f))
        .setTransformationMatrix(Matrix4::translation({-1.05f, -1.05f, 0.0f}))
        .draw(MeshTools::compile(sphereFlippedWinding));

    /* Top right is a sphere from the inside, with CCW triangles, with face
       winding and normals flipped */
    shader
        .setProjectionMatrix(Matrix4::orthographicProjection(Vector2{4.5f}, 0.0f, 1.0f))
        .setTransformationMatrix(Matrix4::translation({+1.05f, 1.05f, 0.0f}))
        .draw(MeshTools::compile(sphereFlippedNormalsWinding));

    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);

    /* Bottom right is a sphere from the inside, with CW triangles and face
       culling disabled. Should render like bottom right.
        - If DoubleSided isn't enabled on the shader, the code above flipped
          normals to point inside. If DoubleSided is accidentally active
          always, it will flip them back outside, resulting in the same result
          as on the bottom left.
        - If DoubleSided is enabled on the shader, the normals weren't flipped
          by the code above and the shader should do that instead. If it
          doesn't, it will again wrongly render as on the bottom left. */
    shader
        .setProjectionMatrix(Matrix4::orthographicProjection(Vector2{4.5f}, 0.0f, 1.0f))
        .setTransformationMatrix(Matrix4::translation({+1.05f, -1.05f, 0.0f}))
        .draw(MeshTools::compile(sphereDoubleSided));

    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "PhongTestFiles/double-sided.tga"),
        (DebugTools::CompareImageToFile{_manager, 1.34f, 0.04f}));
}

#ifndef MAGNUM_TARGET_GLES2
template<PhongGL::Flag flag> void PhongGLTest::renderSkinning() {
    auto&& data = RenderSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 4)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == PhongGL::Flag::UniformBuffers) {
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

    /* Tests just 2D movement, no lights, no normals, as that should be pretty
       independent of the skinning process. That also makes it easy to reuse
       for Flat2D/3D and MeshVisualizer shaders. */
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
    if(flag == PhongGL::Flag::UniformBuffers && data.jointCount > Containers::arraySize(jointMatrices))
        CORRADE_SKIP("Uploading an uniform buffer smaller than the size hardcoded in the shader is an error in WebGL.");
    #endif

    GL::Buffer buffer{vertices};

    GL::Mesh mesh{MeshPrimitive::TriangleStrip};
    mesh.setCount(4);
    mesh.addVertexBuffer(buffer, 0, sizeof(Vertex), GL::DynamicAttribute{PhongGL::Position{}});
    for(auto&& attribute: data.attributes)
        mesh.addVertexBuffer(buffer, 3*4 + attribute.first(), sizeof(Vertex), attribute.second());

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(data.flags|flag)
        .setLightCount(0)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    if(data.setDynamicPerVertexJointCount)
        shader.setPerVertexJointCount(data.dynamicPerVertexJointCount, data.dynamicSecondaryPerVertexJointCount);

    if(flag == PhongGL::Flag{}) {
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
            .setAmbientColor(0xffffff_rgbf)
            .setTransformationMatrix(Matrix4::scaling(Vector3{0.5f}))
            .draw(mesh);
    } else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(Matrix4::scaling(Vector3{0.5f}))
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
            PhongDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0xffffff_rgbf)
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
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

template<PhongGL::Flag flag> void PhongGLTest::renderInstanced() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
            CORRADE_SKIP("UBOs with dynamically indexed (light) arrays are a crashy dumpster fire on SwiftShader, can't test.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
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
        Primitives::UVSphereFlag::TextureCoordinates|
        Primitives::UVSphereFlag::Tangents));

    /* Three spheres, each in a different location. To test normal matrix
       concatenation, everything is rotated 90° on Y, thus X is now -Z and Z is
       now X. */
    struct {
        Matrix4 transformation;
        Matrix3x3 normal;
        Color3 color;
        Vector3 textureOffsetLayer;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix4::translation(Math::gather<'z', 'y', 'x'>(Vector3{-1.25f, -1.25f, 0.0f}))*Matrix4::rotationY(-90.0_degf)*Matrix4::rotationX(90.0_degf),
            /* to test also per-instance normal matrix is applied properly --
               the texture should look the same as in the case of Flat 3D
               instanced textured */
            (Matrix4::rotationY(-90.0_degf)*Matrix4::rotationX(90.0_degf)).normalMatrix(),
            data.flags & PhongGL::Flag::DiffuseTexture ? 0xffffff_rgbf : 0xffff00_rgbf,
            {0.0f, 0.0f, 0.0f}, 211},
        {Matrix4::translation(Math::gather<'z', 'y', 'x'>(Vector3{ 1.25f, -1.25f, 0.0f})),
            {},
            data.flags & PhongGL::Flag::DiffuseTexture ? 0xffffff_rgbf : 0x00ffff_rgbf,
            {1.0f, 0.0f, 1.0f}, 4627},
        {Matrix4::translation(Math::gather<'z', 'y', 'x'>(Vector3{  0.0f,  1.0f, -1.0f})),
            {},
            data.flags & PhongGL::Flag::DiffuseTexture ? 0xffffff_rgbf : 0xff00ff_rgbf,
            #ifndef MAGNUM_TARGET_GLES2
            data.flags & PhongGL::Flag::TextureArrays ? Vector3{0.0f, 0.0f, 2.0f} :
            #endif
            Vector3{0.5f, 1.0f, 2.0f}, 35363}
    };

    sphere
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceData}, 1, 0,
            PhongGL::TransformationMatrix{},
            PhongGL::NormalMatrix{},
            PhongGL::Color3{},
            #ifndef MAGNUM_TARGET_GLES2
            PhongGL::TextureOffsetLayer{},
            #else
            PhongGL::TextureOffset{},
            4,
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            PhongGL::ObjectId{}
            #else
            4
            #endif
        )
        .setInstanceCount(3);

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::VertexColor|PhongGL::Flag::InstancedTransformation|data.flags|flag)
        .setLightCount(2)};

    GL::Texture2D diffuse{NoCreate};
    GL::Texture2D normal{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray diffuseArray{NoCreate};
    GL::Texture2DArray normalArray{NoCreate};
    #endif
    if(data.flags & (PhongGL::Flag::DiffuseTexture|PhongGL::Flag::NormalTexture)) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
          !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        if(data.flags & PhongGL::Flag::DiffuseTexture) {
            Containers::Optional<Trade::ImageData2D> image;
            CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

            #ifndef MAGNUM_TARGET_GLES2
            if(data.flags & PhongGL::Flag::TextureArrays) {
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

                diffuseArray = GL::Texture2DArray{};
                diffuseArray.setMinificationFilter(GL::SamplerFilter::Linear)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setWrapping(GL::SamplerWrapping::ClampToEdge)
                    /* Three slices with 2 extra as a base offset, each slice
                       has half the height */
                    .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 2 + 3})
                    .setSubImage(0, {0, 0, 2}, first)
                    /* Put the second image on the right half to test that the
                       per-instance offset is used together with the layer */
                    .setSubImage(0, {image->size().x()/2, 0, 3}, second)
                    .setSubImage(0, {0, 0, 4}, third);
                shader.bindDiffuseTexture(diffuseArray);

            } else
            #endif
            {
                diffuse = GL::Texture2D{};
                diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setWrapping(GL::SamplerWrapping::ClampToEdge)
                    .setStorage(1, TextureFormatRGB, image->size())
                    .setSubImage(0, {}, *image);
                shader.bindDiffuseTexture(diffuse);
            }
        }

        if(data.flags & PhongGL::Flag::NormalTexture) {
            Containers::Optional<Trade::ImageData2D> image;
            CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/normal-texture.tga")) && (image = importer->image2D(0)));

            #ifndef MAGNUM_TARGET_GLES2
            if(data.flags & PhongGL::Flag::TextureArrays) {
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

                normalArray = GL::Texture2DArray{};
                normalArray.setMinificationFilter(GL::SamplerFilter::Linear)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setWrapping(GL::SamplerWrapping::ClampToEdge)
                    /* Three slices with 2 extra as a base offset, each slice
                       has half the height */
                    .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 2 + 3})
                    .setSubImage(0, {0, 0, 2}, first)
                    /* Put the second image on the right half to test that the
                       per-instance offset is used together with the layer */
                    .setSubImage(0, {image->size().x()/2, 0, 3}, second)
                    .setSubImage(0, {0, 0, 4}, third);
                shader.bindNormalTexture(normalArray);

            } else
            #endif
            {
                normal = GL::Texture2D{};
                normal.setMinificationFilter(GL::SamplerFilter::Linear)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setWrapping(GL::SamplerWrapping::ClampToEdge)
                    .setStorage(1, TextureFormatRGB, image->size())
                    .setSubImage(0, {}, *image);
                shader.bindNormalTexture(normal);
            }

            normal.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindNormalTexture(normal);
        }
    }

    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= PhongGL::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & PhongGL::Flag::TextureArrays) {
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
    if(data.flags & PhongGL::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {PhongGL::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});
    #endif

    if(flag == PhongGL::Flag{}) {
        shader
            .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                                { 3.0f, -3.0f, 2.0f, 0.0f}})
            .setLightColors({0x999999_rgbf, 0x999999_rgbf})
            .setLightSpecularColors({0x0000ff_rgbf, 0x00ff00_rgbf})
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(90.0_degf)*
                Matrix4::scaling(Vector3{0.4f}))
            .setNormalMatrix(Matrix4::rotationY(90.0_degf).normalMatrix())
            .setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setDiffuseColor(data.flags & PhongGL::Flag::DiffuseTexture ?
            0xffffff_rgbf : 0xffff00_rgbf);

        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.setTextureMatrix(Matrix3::scaling(
                #ifndef MAGNUM_TARGET_GLES2
                /* Slices of the texture array have half the height */
                data.flags & PhongGL::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                #endif
                Vector2{0.5f}
            ));
        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & PhongGL::Flag::TextureArrays)
            shader.setTextureLayer(2); /* base offset */
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & PhongGL::Flag::ObjectId) {
            /* Gets added to the per-instance ID, if that's enabled as well */
            shader.setObjectId(1000);
        }
        #endif

        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}.setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
            )
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}.setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(90.0_degf)*
                Matrix4::scaling(Vector3{0.4f})
            )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
                .setNormalMatrix(Matrix4::rotationY(90.0_degf).normalMatrix())
                /* Gets added to the per-instance ID, if that's enabled as
                   well */
                .setObjectId(1000)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setDiffuseColor(data.flags & PhongGL::Flag::DiffuseTexture ?
            0xffffff_rgbf : 0xffff00_rgbf)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(Matrix3::scaling(
                    #ifndef MAGNUM_TARGET_GLES2
                    /* Slices of the texture array have half the height */
                    data.flags & PhongGL::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                    #endif
                    Vector2{0.5f}))
                .setLayer(2) /* base offset */
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
                .setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x999999_rgbf)
                .setSpecularColor(0x0000ff_rgbf),
            PhongLightUniform{}
                .setPosition({3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x999999_rgbf)
                .setSpecularColor(0x00ff00_rgbf)
        }};
        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    /*
        Colored case:

        -   First should be lower left, yellow with a blue and green highlight
            on bottom left and right part
        -   Second lower right, cyan with a yellow light, so green, the same
            highlight at the same position
        -   Third up center, magenta with a yellow light, so red, the same
            highlight at the same position

        Textured case:

        -   Lower left has bottom left numbers, so light 7881, rotated (78
            visible, should look the same as the multidraw case or as Flat)
        -   Lower light has bottom right, 1223, rotated (23 visible, looking at
            the left side of the sphere in the equivalent Flat test)
        -   Up center has 6778, rotated (78 visible, looking at the left side
            of the sphere in the equivalent Flat test)
    */
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "PhongTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    #ifndef MAGNUM_TARGET_GLES2
    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & PhongGL::Flag::ObjectId) {
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
template<PhongGL::Flag flag> void PhongGLTest::renderInstancedSkinning() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == PhongGL::Flag::ShaderStorageBuffers) {
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
    if(flag == PhongGL::Flag::UniformBuffers) {
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

    /* Similarly to renderSkinning() tests just 2D movement, differently and
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
            PhongGL::Position{},
            PhongGL::JointIds{PhongGL::JointIds::Components::Three},
            PhongGL::Weights{PhongGL::Weights::Components::Three})
        .addVertexBufferInstanced(GL::Buffer{GL::Buffer::TargetHint::Array, instanceTransformations}, 1, 0,
            PhongGL::TransformationMatrix{})
        .setInstanceCount(3);

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::InstancedTransformation|flag)
        .setLightCount(0)
        .setJointCount(15, 3, 0)};

    if(flag == PhongGL::Flag{}) {
        shader
            .setJointMatrices(jointMatrices)
            .setPerInstanceJointCount(5)
            .setAmbientColor(0xffffff_rgbf)
            .setTransformationMatrix(Matrix4::scaling(Vector3{0.3f}))
            .draw(mesh);
    } else if(flag == PhongGL::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == PhongGL::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(Matrix4::scaling(Vector3{0.3f}))
        }};
        TransformationUniform3D jointMatricesUniformData[Containers::arraySize(jointMatrices)];
        Utility::copy(jointMatrices, Containers::stridedArrayView(jointMatricesUniformData).slice(&TransformationUniform3D::transformationMatrix));
        GL::Buffer jointMatricesUniform{GL::Buffer::TargetHint::Uniform,
            jointMatricesUniformData
        };
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            PhongDrawUniform{}
                .setPerInstanceJointCount(5)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0xffffff_rgbf)
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
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
void PhongGLTest::renderMulti() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= PhongGL::Flag::ShaderStorageBuffers) {
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

    if(data.flags >= PhongGL::Flag::MultiDraw) {
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

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling|data.flags)
        .setLightCount(data.lightCount, data.perDrawLightCount)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};

    GL::Texture2D diffuse{NoCreate};
    GL::Texture2DArray diffuseArray{NoCreate};
    if(data.flags & PhongGL::Flag::DiffuseTexture) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
           !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

        /* For arrays we upload three slices of the original image to half-high
           slices */
        if(data.flags & PhongGL::Flag::TextureArrays) {
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

            Vector3i size{image->size().x(), image->size().y()/2, 3};

            diffuseArray = GL::Texture2DArray{};
            diffuseArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                /* Each slice has half the height */
                .setStorage(1, TextureFormatRGB, size)
                /* Clear to all zeros for reproducible output */
                .setSubImage(0, {}, Image3D{PixelFormat::RGB8Unorm, size, Containers::Array<char>{ValueInit, std::size_t(size.product()*3)}})
                .setSubImage(0, {0, 0, 0}, first)
                /* Put the second image on the right half to test that the
                   per-instance offset is used together with the layer */
                .setSubImage(0, {image->size().x()/2, 0, 1}, second)
                .setSubImage(0, {0, 0, 2}, third);
            shader.bindDiffuseTexture(diffuseArray);

        } else {
            diffuse = GL::Texture2D{};
            diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, TextureFormatRGB, image->size())
                .setSubImage(0, {}, *image);
            shader.bindDiffuseTexture(diffuse);
        }
    }

    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= PhongGL::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & PhongGL::Flag::TextureArrays) {
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
        Primitives::UVSphereFlag::TextureCoordinates|
        Primitives::UVSphereFlag::Tangents);
    /* Plane is a strip, make it indexed first */
    Trade::MeshData planeData = MeshTools::generateIndices(Primitives::planeSolid(
        Primitives::PlaneFlag::TextureCoordinates|
        Primitives::PlaneFlag::Tangents));
    Trade::MeshData coneData = Primitives::coneSolid(1, 32, 1.0f,
        Primitives::ConeFlag::TextureCoordinates|
        Primitives::ConeFlag::Tangents);
    GL::Mesh mesh = MeshTools::compile(MeshTools::concatenate({sphereData, planeData, coneData}));
    GL::MeshView sphere{mesh};
    sphere.setCount(sphereData.indexCount());
    GL::MeshView plane{mesh};
    plane.setCount(planeData.indexCount())
        .setIndexOffset(sphereData.indexCount());
    GL::MeshView cone{mesh};
    cone.setCount(coneData.indexCount())
        .setIndexOffset(sphereData.indexCount() + planeData.indexCount());

    GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
        ProjectionUniform3D{}.setProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
        )
    }};

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<PhongMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = PhongMaterialUniform{}
        .setDiffuseColor(data.flags & PhongGL::Flag::DiffuseTexture ?
            0xffffff_rgbf : 0x00ffff_rgbf);
    materialData[1*data.uniformIncrement] = PhongMaterialUniform{}
        .setDiffuseColor(data.flags & PhongGL::Flag::DiffuseTexture ?
            0xffffff_rgbf : 0xffff00_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    /* The shader has two lights hardcoded, so make sure the buffer can fit
       2 items enough even though the last draw needs just one light. Not a
       problem on desktop, but WebGL complains. */
    Containers::Array<PhongLightUniform> lightData{2*data.uniformIncrement + 2};
    lightData[0*data.uniformIncrement] = PhongLightUniform{}
        .setPosition(Vector4{0.0f, 0.0f, 1.0f, 0.0f})
        .setColor(data.flags & PhongGL::Flag::DiffuseTexture ?
            0xffffff_rgbf : 0x00ffff_rgbf);
    lightData[1*data.uniformIncrement + 0] = PhongLightUniform{}
        .setPosition(Vector4{-3.0f, -3.0f, 2.0f, 0.0f})
        .setColor(0x999999_rgbf)
        .setSpecularColor(0xff0000_rgbf);
    lightData[1*data.uniformIncrement + 1] = PhongLightUniform{}
        .setPosition(Vector4{3.0f, -3.0f, 2.0f, 0.0f})
        .setColor(0x999999_rgbf)
        .setSpecularColor(0x00ff00_rgbf);
    /* This will put the light to position 4 in case data.uniformIncrement is 1
       and to an offset aligned to 256 if it's higher */
    lightData[2*data.uniformIncrement + 1/data.uniformIncrement] = PhongLightUniform{}
        .setPosition(Vector4{0.0f, 0.0f, 1.0f, 0.0f})
        .setColor(data.flags & PhongGL::Flag::DiffuseTexture ?
            0xffffff_rgbf : 0xff00ff_rgbf);
    GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, lightData};

    Containers::Array<TransformationUniform3D> transformationData{2*data.uniformIncrement + 1};
    transformationData[0*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({-1.25f, -1.25f, 0.0f})*
            /* to test the normal matrix is applied properly */
            Matrix4::rotationX(90.0_degf)
        );
    transformationData[1*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({ 1.25f, -1.25f, 0.0f})
        );
    transformationData[2*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({  0.0f,  1.0f, 1.0f})
        );
    GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, transformationData};

    Containers::Array<TextureTransformationUniform> textureTransformationData{2*data.uniformIncrement + 1};
    textureTransformationData[0*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & PhongGL::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.0f, 0.0f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & PhongGL::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({1.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({1.0f, 0.0f}))
        .setLayer(1); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & PhongGL::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.5f, 1.0f}))
        .setLayer(2); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<PhongDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material / light offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setLightOffsetCount(data.bindWithOffset ? 0 : 1, 2)
        .setNormalMatrix(transformationData[0*data.uniformIncrement].transformationMatrix.normalMatrix())
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setLightOffsetCount(data.bindWithOffset ? 0 : 3, 1)
        .setNormalMatrix(transformationData[1*data.uniformIncrement].transformationMatrix.normalMatrix())
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setLightOffsetCount(data.bindWithOffset ? 0 : 0, 1)
        .setNormalMatrix(transformationData[2*data.uniformIncrement].transformationMatrix.normalMatrix())
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    shader.bindProjectionBuffer(projectionUniform);

    /* Map ObjectIdOutput so we can draw to it. Mapping it always causes an
       error on WebGL when the shader does not render to it; however if not
       bound we can't even clear it on WebGL, so it has to be cleared after. */
    if(data.flags & PhongGL::Flag::ObjectId) _framebuffer
        .mapForDraw({
            {PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {PhongGL::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
        })
        .clearColor(1, Vector4ui{27});

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(PhongMaterialUniform),
            sizeof(PhongMaterialUniform));
        shader.bindLightBuffer(lightUniform,
            1*data.uniformIncrement*sizeof(PhongLightUniform),
            2*sizeof(PhongLightUniform));
        shader.bindTransformationBuffer(transformationUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(PhongDrawUniform),
            sizeof(PhongDrawUniform));
        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(sphere);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(PhongMaterialUniform),
            sizeof(PhongMaterialUniform));
        shader.bindLightBuffer(lightUniform,
            2*data.uniformIncrement*sizeof(PhongLightUniform),
            2*sizeof(PhongLightUniform));
        shader.bindTransformationBuffer(transformationUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(PhongDrawUniform),
            sizeof(PhongDrawUniform));
        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(plane);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(PhongMaterialUniform),
            sizeof(PhongMaterialUniform));
        shader.bindLightBuffer(lightUniform,
            0*data.uniformIncrement*sizeof(PhongLightUniform),
            2*sizeof(PhongLightUniform));
        shader.bindTransformationBuffer(transformationUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(PhongDrawUniform),
            sizeof(PhongDrawUniform));
        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(cone);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform);
        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);

        if(data.flags >= PhongGL::Flag::MultiDraw)
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

    /*
        Colored case:

        -   Sphere should be lower left, yellow with a white light with red and
            green highlight on bottom left and right part
        -   Plane lower right, cyan with a magenta light so blue
        -   Cone up center, yellow with a cyan light so green

        Textured case:

        -   Sphere should have bottom left numbers, so light 7881, rotated (78
            visible)
        -   Plane bottom right, 1223
        -   Cone 6778
    */
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "PhongTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));

    /* Object ID -- no need to verify the whole image, just check that pixels
       on known places have expected values. SwiftShader insists that the read
       format has to be 32bit, so the renderbuffer format is that too to make
       it the same (ES3 Mesa complains if these don't match). */
    if(data.flags & PhongGL::Flag::ObjectId) {
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

void PhongGLTest::renderMultiSkinning() {
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
    if(data.flags >= PhongGL::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 4)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= PhongGL::Flag::MultiDraw) {
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

    PhongGL shader{PhongGL::Configuration{}
        .setFlags(PhongGL::Flag::UniformBuffers|data.flags)
        .setLightCount(0)
        .setDrawCount(data.drawCount)
        .setMaterialCount(data.materialCount)
        .setJointCount(data.jointCount, 2, 0)};

    /* Similarly to renderSkinning() tests just 2D movement, differently and
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
            PhongGL::Position{},
            PhongGL::JointIds{PhongGL::JointIds::Components::Two},
            PhongGL::Weights{PhongGL::Weights::Components::Two})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, indices}, 0, MeshIndexType::UnsignedInt);
    GL::MeshView square{mesh};
    square.setCount(6);
    GL::MeshView triangle1{mesh};
    triangle1.setCount(3)
        .setIndexOffset(6);
    GL::MeshView triangle2{mesh};
    triangle2.setCount(3)
        .setIndexOffset(9);

    GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
        ProjectionUniform3D{}
    }};;

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<PhongMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = PhongMaterialUniform{}
        .setAmbientColor(0x33ffff_rgbf);
    materialData[1*data.uniformIncrement] = PhongMaterialUniform{}
        .setAmbientColor(0xffff33_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationUniform3D> transformationData{2*data.uniformIncrement + 1};
    transformationData[0*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::scaling(Vector3{0.3f})*
                                 Matrix4::translation({ 0.0f, -1.5f, 0.0f}));
    transformationData[1*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::scaling(Vector3{0.3f})*
                                 Matrix4::translation({ 1.5f,  1.5f, 0.0f}));
    transformationData[2*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(Matrix4::scaling(Vector3{0.3f})*
                                 Matrix4::translation({-1.5f,  1.5f, 0.0f}));
    GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, transformationData};

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

    Containers::Array<PhongDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material / joint offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead */
    drawData[0*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 0);
    drawData[1*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        /* Overlaps with the first joint set with two matrices, unless the
           padding in the single-draw case prevents that */
        .setJointOffset(data.bindWithOffset ? 0 : 2);
    drawData[2*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 6);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    shader.bindProjectionBuffer(projectionUniform);

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(PhongMaterialUniform),
            sizeof(PhongMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindJointBuffer(jointUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(PhongDrawUniform),
            sizeof(PhongDrawUniform));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(PhongMaterialUniform),
            sizeof(PhongMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindJointBuffer(jointUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(PhongDrawUniform),
            sizeof(PhongDrawUniform));
        shader.draw(triangle1);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(PhongMaterialUniform),
            sizeof(PhongMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindJointBuffer(jointUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(PhongDrawUniform),
            sizeof(PhongDrawUniform));
        shader.draw(triangle2);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindJointBuffer(jointUniform)
            .bindDrawBuffer(drawUniform);

        if(data.flags >= PhongGL::Flag::MultiDraw)
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

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGLTest)
