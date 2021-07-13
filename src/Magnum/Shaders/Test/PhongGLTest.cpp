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
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

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
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Swizzle.h"
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
    void constructUniformBuffers();
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
    void setWrongLightCount();
    void setWrongLightId();
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
    void renderObjectIdSetup();
    void renderObjectIdTeardown();

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

    template<PhongGL::Flag flag = PhongGL::Flag{}> void renderInstanced();

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti();
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
    UnsignedInt lightCount;
} ConstructData[]{
    {"", {}, 1},
    {"ambient texture", PhongGL::Flag::AmbientTexture, 1},
    {"diffuse texture", PhongGL::Flag::DiffuseTexture, 1},
    {"diffuse texture + texture transform", PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureTransformation, 1},
    {"specular texture", PhongGL::Flag::SpecularTexture, 1},
    {"normal texture", PhongGL::Flag::NormalTexture, 1},
    {"normal texture + separate bitangents", PhongGL::Flag::NormalTexture|PhongGL::Flag::Bitangent, 1},
    {"separate bitangents alone", PhongGL::Flag::Bitangent, 1},
    {"ambient + diffuse texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture, 1},
    {"ambient + specular texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture, 1},
    {"diffuse + specular texture", PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture, 1},
    {"ambient + diffuse + specular texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture, 1},
    {"ambient + diffuse + specular + normal texture", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"ambient + diffuse + specular + normal texture arrays", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays, 1},
    {"ambient + diffuse + specular + normal texture arrays + texture transformation", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation, 1},
    #endif
    {"alpha mask", PhongGL::Flag::AlphaMask, 1},
    {"alpha mask + diffuse texture", PhongGL::Flag::AlphaMask|PhongGL::Flag::DiffuseTexture, 1},
    {"vertex colors", PhongGL::Flag::VertexColor, 1},
    {"vertex colors + diffuse texture", PhongGL::Flag::VertexColor|PhongGL::Flag::DiffuseTexture, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", PhongGL::Flag::ObjectId, 1},
    /* This is fine, InstancedObjectId isn't (check in ConstructInvalidData) */
    {"object ID + separate bitangent", PhongGL::Flag::ObjectId|PhongGL::Flag::Bitangent, 1},
    {"instanced object ID", PhongGL::Flag::InstancedObjectId, 1},
    {"object ID + alpha mask + specular texture", PhongGL::Flag::ObjectId|PhongGL::Flag::AlphaMask|PhongGL::Flag::SpecularTexture, 1},
    #endif
    {"no specular", PhongGL::Flag::NoSpecular, 1},
    {"five lights", {}, 5},
    {"zero lights", {}, 0},
    {"instanced transformation", PhongGL::Flag::InstancedTransformation, 3},
    {"instanced specular texture offset", PhongGL::Flag::SpecularTexture|PhongGL::Flag::InstancedTextureOffset, 3},
    {"instanced normal texture offset", PhongGL::Flag::NormalTexture|PhongGL::Flag::InstancedTextureOffset, 3},
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId|Bitangent is disallowed (checked in
       ConstructInvalidData), but this should work */
    {"object ID + normal texture with bitangent from tangent", PhongGL::Flag::InstancedObjectId|PhongGL::Flag::NormalTexture, 1}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, materialCount, drawCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {}, 1, 1, 1},
    {"", PhongGL::Flag::UniformBuffers, 1, 1, 1},
    /* SwiftShader has 256 uniform vectors at most, per-3D-draw is 4+4,
       per-material 4, per-light 4 plus 4 for projection */
    {"multiple lights, materials, draws", PhongGL::Flag::UniformBuffers, 8, 8, 24},
    {"multiple lights, materials, draws + light culling", PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling, 8, 8, 24},
    {"zero lights", PhongGL::Flag::UniformBuffers, 0, 16, 24},
    {"ambient + diffuse + specular texture", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture, 1, 1, 1},
    {"ambient + diffuse + specular texture + texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureTransformation, 1, 1, 1},
    {"ambient + diffuse + specular texture array + texture transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation, 1, 1, 1},
    {"normal texture", PhongGL::Flag::UniformBuffers|PhongGL::Flag::NormalTexture, 1, 1, 1},
    {"normal texture + separate bitangents", PhongGL::Flag::UniformBuffers|PhongGL::Flag::NormalTexture|PhongGL::Flag::Bitangent, 1, 1, 1},
    {"alpha mask", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AlphaMask, 1, 1, 1},
    {"object ID", PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectId, 1, 1, 1},
    {"no specular", PhongGL::Flag::UniformBuffers|PhongGL::Flag::NoSpecular, 1, 1, 1},
    {"multidraw with all the things", PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::AmbientTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::AlphaMask|PhongGL::Flag::ObjectId|PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::InstancedTransformation|PhongGL::Flag::InstancedObjectId|PhongGL::Flag::LightCulling, 8, 16, 24}
};
#endif

constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    const char* message;
} ConstructInvalidData[] {
    {"texture transformation but not textured",
        PhongGL::Flag::TextureTransformation,
        "texture transformation enabled but the shader is not textured"},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays but not textured", PhongGL::Flag::TextureArrays,
        "texture arrays enabled but the shader is not textured"},
    {"conflicting bitangent and instanced object id attribute",
        PhongGL::Flag::Bitangent|PhongGL::Flag::InstancedObjectId,
        "Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead"},
    #endif
    {"specular texture but no specular",
        PhongGL::Flag::SpecularTexture|PhongGL::Flag::NoSpecular,
        "specular texture requires the shader to not have specular disabled"}
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    {"zero draws", PhongGL::Flag::UniformBuffers, 1, 1, 0,
        "draw count can't be zero"},
    {"zero materials", PhongGL::Flag::UniformBuffers, 1, 0, 1,
        "material count can't be zero"},
    {"texture arrays but no transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays, 1, 1, 1,
        "texture arrays require texture transformation enabled as well if uniform buffers are used"},
    {"light culling but no UBOs", PhongGL::Flag::LightCulling, 1, 1, 1,
        "light culling requires uniform buffers to be enabled"}
};
#endif

constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    const char* message;
} BindTexturesInvalidData[]{
    {"not textured", {},
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled\n"
        "Shaders::PhongGL::bindTextures(): the shader was not created with any textures enabled\n"},
    #ifndef MAGNUM_TARGET_GLES2
    {"array", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture|PhongGL::Flag::TextureArrays,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"
        "Shaders::PhongGL::bindTextures(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    PhongGL::Flags flags;
    const char* message;
} BindTextureArraysInvalidData[]{
    {"not textured", {},
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled\n"},
    {"not array", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::NormalTexture,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"}
};
#endif

using namespace Math::Literals;

const struct {
    const char* name;
    Deg rotation;
    Color3 lightColor1, lightColor2;
    Float lightPosition1, lightPosition2;
} RenderColoredData[]{
    {"", {}, 0x993366_rgbf, 0x669933_rgbf, -3.0f, 3.0f},
    {"flip lights", {}, 0x669933_rgbf, 0x993366_rgbf, 3.0f, -3.0f},
    {"rotated", 45.0_degf, 0x993366_rgbf, 0x669933_rgbf, -3.0f, 3.0f}
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
    Float range;
    Containers::Array<std::pair<Vector2i, Color3ub>> picks;
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
       cause any NaNs */
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

constexpr struct {
    const char* name;
    const char* file;
    PhongGL::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData[] {
    {"diffuse color", "instanced.tga", {},
        /* Minor differences on SwiftShader */
        81.0f, 0.06f},
    {"diffuse texture", "instanced-textured.tga",
        PhongGL::Flag::DiffuseTexture|PhongGL::Flag::InstancedTextureOffset,
        /* Minor differences on SwiftShader */
        112.0f, 0.09f},
    /** @todo test normal when there's usable texture */
    #ifndef MAGNUM_TARGET_GLES2
    {"diffuse texture array", "instanced-textured.tga",
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
    PhongGL::Flags flags;
    UnsignedInt lightCount, materialCount, drawCount;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData[] {
    {"bind with offset, colored", "multidraw.tga",
        {},
        2, 1, 1, 16,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"bind with offset, textured", "multidraw-textured.tga",
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture,
        2, 1, 1, 16,
        /* Minor differences on ARM Mali */
        4.67f, 0.02f},
    #ifndef MAGNUM_TARGET_GLES2
    {"bind with offset, texture array", "multidraw-textured.tga",
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        2, 1, 1, 16,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.131f},
    #endif
    {"draw offset, colored", "multidraw.tga",
        {},
        4, 2, 3, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"draw offset, textured", "multidraw-textured.tga",
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture,
        4, 2, 3, 1,
        /* Minor differences on ARM Mali */
        4.67f, 0.02f},
    #ifndef MAGNUM_TARGET_GLES2
    {"draw offset, texture array", "multidraw-textured.tga",
        PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        4, 2, 3, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.131f},
    #endif
    {"multidraw, colored", "multidraw.tga",
        PhongGL::Flag::MultiDraw,
        4, 2, 3, 1,
        /* Minor differences on ARM Mali */
        3.34f, 0.01f},
    {"multidraw, textured", "multidraw-textured.tga",
        PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture,
        4, 2, 3, 1,
        /* Minor differences on ARM Mali */
        4.67f, 0.02f},
    #ifndef MAGNUM_TARGET_GLES2
    {"multidraw, texture array", "multidraw-textured.tga",
        PhongGL::Flag::MultiDraw|PhongGL::Flag::TextureTransformation|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::TextureArrays,
        4, 2, 3, 1,
        /* Some difference at the UV edge (texture is wrapping in the 2D case
           while the 2D array has a black area around) */
        50.34f, 0.131f},
    #endif
    /** @todo test normal and per-draw scaling when there's usable texture */
};
#endif

PhongGLTest::PhongGLTest() {
    addInstancedTests({&PhongGLTest::construct},
        Containers::arraySize(ConstructData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::constructUniformBuffers},
        Containers::arraySize(ConstructUniformBuffersData));
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
    addTests({&PhongGLTest::setUniformUniformBuffersEnabled,
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
        &PhongGLTest::setWrongLightCount,
        &PhongGLTest::setWrongLightId,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::setWrongDrawOffset
        #endif
    });

    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderDefaults,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderDefaults<PhongGL::Flag::UniformBuffers>
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderColored,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderColored<PhongGL::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderColoredData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderSinglePixelTextured,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderSinglePixelTextured<PhongGL::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderSinglePixelTexturedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderTextured,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderTextured<PhongGL::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderTexturedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderTexturedNormal,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderTexturedNormal<PhongGL::Flag::UniformBuffers>
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
        #endif
        &PhongGLTest::renderVertexColor<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderVertexColor<Color4, PhongGL::Flag::UniformBuffers>,
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderShininess,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderShininess<PhongGL::Flag::UniformBuffers>,
        #endif
        },
        Containers::arraySize(RenderShininessData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderAlpha,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderAlpha<PhongGL::Flag::UniformBuffers>
        #endif
        },
        Containers::arraySize(RenderAlphaData),
        &PhongGLTest::renderAlphaSetup,
        &PhongGLTest::renderAlphaTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderObjectId,
        &PhongGLTest::renderObjectId<PhongGL::Flag::UniformBuffers>},
        &PhongGLTest::renderObjectIdSetup,
        &PhongGLTest::renderObjectIdTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderLights,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderLights<PhongGL::Flag::UniformBuffers>,
        #endif
        },
        Containers::arraySize(RenderLightsData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addTests({
        &PhongGLTest::renderLightsSetOneByOne,
        &PhongGLTest::renderLowLightAngle,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderLightCulling
        #endif
        },
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addTests<PhongGLTest>({
        &PhongGLTest::renderZeroLights,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderZeroLights<PhongGL::Flag::UniformBuffers>
        #endif
        },
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderObjectIdSetup,
        &PhongGLTest::renderObjectIdTeardown
        #else
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown
        #endif
    );

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<PhongGLTest>({
        &PhongGLTest::renderInstanced,
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderInstanced<PhongGL::Flag::UniformBuffers>,
        #endif
        },
        Containers::arraySize(RenderInstancedData),
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderObjectIdSetup,
        &PhongGLTest::renderObjectIdTeardown
        #else
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown
        #endif
    );

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::renderMulti},
        Containers::arraySize(RenderMultiData),
        &PhongGLTest::renderObjectIdSetup,
        &PhongGLTest::renderObjectIdTeardown);
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

void PhongGLTest::construct() {
    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    PhongGL shader{data.flags, data.lightCount};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.lightCount(), data.lightCount);
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
void PhongGLTest::constructUniformBuffers() {
    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
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

    PhongGL shader{data.flags, data.lightCount, data.materialCount, data.drawCount};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.lightCount(), data.lightCount);
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

void PhongGLTest::constructMove() {
    PhongGL a{PhongGL::Flag::AlphaMask, 3};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    PhongGL b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), PhongGL::Flag::AlphaMask);
    CORRADE_COMPARE(b.lightCount(), 3);
    CORRADE_VERIFY(!a.id());

    PhongGL c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), PhongGL::Flag::AlphaMask);
    CORRADE_COMPARE(c.lightCount(), 3);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::constructMoveUniformBuffers() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    PhongGL a{PhongGL::Flag::UniformBuffers, 3, 2, 5};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    PhongGL b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), PhongGL::Flag::UniformBuffers);
    CORRADE_COMPARE(b.lightCount(), 3);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_VERIFY(!a.id());

    PhongGL c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), PhongGL::Flag::UniformBuffers);
    CORRADE_COMPARE(c.lightCount(), 3);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_VERIFY(!b.id());
}
#endif

void PhongGLTest::constructInvalid() {
    auto&& data = ConstructInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongGL{data.flags};
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::PhongGL: {}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::constructUniformBuffersInvalid() {
    auto&& data = ConstructUniformBuffersInvalidData[testCaseInstanceId()];
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
    PhongGL{data.flags, data.lightCount, data.materialCount, data.drawCount};
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::PhongGL: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setUniformUniformBuffersEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    PhongGL shader{PhongGL::Flag::UniformBuffers};
    shader.setAmbientColor({})
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
          .setLightRange(0, {});
    CORRADE_COMPARE(out.str(),
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
        "Shaders::PhongGL::setLightRange(): the shader was created with uniform buffers enabled\n");
}

void PhongGLTest::bindBufferUniformBuffersNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Buffer buffer;
    PhongGL shader;
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
          .setDrawOffset(0);
    CORRADE_COMPARE(out.str(),
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
        "Shaders::PhongGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}
#endif

void PhongGLTest::bindTexturesInvalid() {
    auto&& data = BindTexturesInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    PhongGL shader{data.flags};
    shader.bindAmbientTexture(texture)
          .bindDiffuseTexture(texture)
          .bindSpecularTexture(texture)
          .bindNormalTexture(texture)
          .bindTextures(&texture, &texture, &texture, &texture);

    CORRADE_COMPARE(out.str(), data.message);
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::bindTextureArraysInvalid() {
    auto&& data = BindTextureArraysInvalidData[testCaseInstanceId()];
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

    GL::Texture2DArray textureArray;
    PhongGL shader{data.flags};
    shader.bindAmbientTexture(textureArray)
          .bindDiffuseTexture(textureArray)
          .bindSpecularTexture(textureArray)
          .bindNormalTexture(textureArray);

    CORRADE_COMPARE(out.str(), data.message);
}
#endif

void PhongGLTest::setAlphaMaskNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    PhongGL shader;
    shader.setAlphaMask(0.75f);

    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

void PhongGLTest::setSpecularDisabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    PhongGL shader{PhongGL::Flag::NoSpecular};
    shader.setSpecularColor({})
        .setShininess({})
        .setLightSpecularColors({{}})
        .setLightSpecularColor(0, {});
    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setSpecularColor(): the shader was created with specular disabled\n"
        "Shaders::PhongGL::setShininess(): the shader was created with specular disabled\n"
        "Shaders::PhongGL::setLightSpecularColors(): the shader was created with specular disabled\n"
        "Shaders::PhongGL::setLightSpecularColor(): the shader was created with specular disabled\n");
}

void PhongGLTest::setTextureMatrixNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    PhongGL shader;
    shader.setTextureMatrix({});

    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

void PhongGLTest::setNormalTextureScaleNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    PhongGL shader;
    shader.setNormalTextureScale({});

    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setNormalTextureScale(): the shader was not created with normal texture enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setTextureLayerNotArray() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    PhongGL shader;
    shader.setTextureLayer(37);

    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::bindTextureTransformBufferNotEnabled() {
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
    PhongGL shader{PhongGL::Flag::UniformBuffers};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setObjectIdNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    PhongGL shader;
    shader.setObjectId(33376);

    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

void PhongGLTest::setWrongLightCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongGL{{}, 5}
        .setLightColors({Color3{}})
        .setLightPositions({Vector4{}})
        .setLightRanges({0.0f});
    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setLightColors(): expected 5 items but got 1\n"
        "Shaders::PhongGL::setLightPositions(): expected 5 items but got 1\n"
        "Shaders::PhongGL::setLightRanges(): expected 5 items but got 1\n");
}

void PhongGLTest::setWrongLightId() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongGL{{}, 3}
        .setLightColor(3, Color3{})
        .setLightPosition(3, Vector4{})
        .setLightRange(3, 0.0f);
    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setLightColor(): light ID 3 is out of bounds for 3 lights\n"
        "Shaders::PhongGL::setLightPosition(): light ID 3 is out of bounds for 3 lights\n"
        "Shaders::PhongGL::setLightRange(): light ID 3 is out of bounds for 3 lights\n");
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setWrongDrawOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongGL{PhongGL::Flag::UniformBuffers, 1, 2, 5}
        .setDrawOffset(5);
    CORRADE_COMPARE(out.str(),
        "Shaders::PhongGL::setDrawOffset(): draw offset 5 is out of bounds for 5 draws\n");
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
}

void PhongGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

template<PhongGL::Flag flag> void PhongGLTest::renderDefaults() {
    #ifndef MAGNUM_TARGET_GLES2
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

    PhongGL shader{flag};

    if(flag == PhongGL::Flag{}) {
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderColored() {
    auto&& data = RenderColoredData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
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

    PhongGL shader{flag, 2};

    if(flag == PhongGL::Flag{}) {
        shader
            .setLightColors({data.lightColor1, data.lightColor2})
            .setLightPositions({{data.lightPosition1, -3.0f, 2.0f, 0.0f},
                                {data.lightPosition2, -3.0f, 2.0f, 0.0f}})
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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
                .setColor(data.lightColor2)
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
       and Apple A8 has bigger rounding differences. */
    const Float maxThreshold = 8.34f, meanThreshold = 0.100f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/colored.tga"),
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
    if(flag == PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    #endif
    PhongGL shader{flags, 2};

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
        if(flag != PhongGL::Flag::UniformBuffers && data.layer != 0)
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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
       and Apple A8 has bigger rounding differences. */
    const Float maxThreshold = 7.67f, meanThreshold = 0.100f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/colored.tga"),
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
    if(flag == PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    #endif
    PhongGL shader{flags, 2};

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
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/ambient-texture.tga")) && (image = importer->image2D(0)));

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
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/specular-texture.tga")) && (image = importer->image2D(0)));

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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "PhongTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderTexturedNormal() {
    auto&& data = RenderTexturedNormalData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
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
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/normal-texture.tga")) && (image = importer->image2D(0)));
    if(data.flipNormalY) for(auto row: image->mutablePixels<Color3ub>())
        for(Color3ub& pixel: row)
            pixel.y() = 255 - pixel.y();

    PhongGL::Flags flags = PhongGL::Flag::NormalTexture|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == PhongGL::Flag::UniformBuffers && (data.flags & PhongGL::Flag::TextureArrays) && !(data.flags & PhongGL::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= PhongGL::Flag::TextureTransformation;
    }
    #endif
    PhongGL shader{flags, 2};

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
    plane.addVertexBuffer(std::move(tangents), sizeof(Vector4),
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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        Containers::arrayCast<Color3ub>(actual.pixels<Color4ub>());

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
        Utility::Directory::join({_testDir, "PhongTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, PhongGL::Flag flag> void PhongGLTest::renderVertexColor() {
    #ifndef MAGNUM_TARGET_GLES2
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
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    PhongGL shader{PhongGL::Flag::DiffuseTexture|PhongGL::Flag::VertexColor|flag, 2};
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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/vertexColor.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<PhongGL::Flag flag> void PhongGLTest::renderShininess() {
    auto&& data = RenderShininessData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
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

    PhongGL shader{flag|data.flags};
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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "PhongTestFiles", data.expected}),
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
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "PhongTestFiles", "shininess0-overflow.tga"}),
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
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join({_testDir, "TestFiles", data.ambientTexture})) && (image = importer->image2D(0)));
    ambient.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, image->size())
        .setSubImage(0, {}, *image);

    GL::Texture2D diffuse;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join({_testDir, "TestFiles", data.diffuseTexture})) && (image = importer->image2D(0)));
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

    PhongGL shader{data.flags|flag, 2};
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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, data.expected),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::renderObjectIdSetup() {
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
                {PhongGL::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                {PhongGL::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
            })
            .clearColor(1, Vector4ui{27});
    }
}

void PhongGLTest::renderObjectIdTeardown() {
    _color = GL::Renderbuffer{NoCreate};
    _objectId = GL::Renderbuffer{NoCreate};
    _framebuffer = GL::Framebuffer{NoCreate};
}

template<PhongGL::Flag flag> void PhongGLTest::renderObjectId() {
    #ifndef MAGNUM_TARGET_GLES2
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
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    PhongGL shader{PhongGL::Flag::ObjectId|flag, 2};

    if(flag == PhongGL::Flag{}) {
        shader
            .setLightColors({0x993366_rgbf, 0x669933_rgbf})
            .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                                { 3.0f, -3.0f, 2.0f, 0.0f}})
            .setAmbientColor(0x330033_rgbf)
            .setDiffuseColor(0xccffcc_rgbf)
            .setSpecularColor(0x6666ff_rgbf)
            .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setObjectId(48526)
            .draw(sphere);
    } else if(flag == PhongGL::Flag::UniformBuffers) {
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
                .setObjectId(48526)
        }};
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
                .setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x993366_rgbf),
            PhongLightUniform{}
                .setPosition({3.0f, -3.0f, 2.0f, 0.0f})
                .setColor(0x669933_rgbf)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            PhongMaterialUniform{}
                .setAmbientColor(0x330033_rgbf)
                .setDiffuseColor(0xccffcc_rgbf)
                .setSpecularColor(0x6666ff_rgbf)
        }};
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
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has some minor rounding differences (max = 1). ARM Mali G71
       and Apple A8 has bigger rounding differences. */
    const Float maxThreshold = 8.34f, meanThreshold = 0.100f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/colored.tga"),
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

template<PhongGL::Flag flag> void PhongGLTest::renderLights() {
    auto&& data = RenderLightsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
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

    PhongGL shader{flag, 1};
    if(flag == PhongGL::Flag{}) {
        shader
            /* Set non-black ambient to catch accidental NaNs -- the render
               should never be fully black */
            .setAmbientColor(0x222222_rgbf)
            .setSpecularColor(data.specularColor)
            .setLightPositions({data.position})
            .setLightColors({0xff8080_rgbf*data.intensity})
            .setLightSpecularColors({data.lightSpecularColor})
            .setLightRanges({data.range})
            .setShininess(60.0f)
            .setTransformationMatrix(transformation)
            .setNormalMatrix(transformation.normalMatrix())
            .setProjectionMatrix(Matrix4::perspectiveProjection(80.0_degf, 1.0f, 0.1f, 20.0f))
            .draw(plane);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        GL::Buffer lightUniform{GL::Buffer::TargetHint::Uniform, {
            PhongLightUniform{}
                .setPosition({data.position})
                .setColor(0xff8080_rgbf*data.intensity)
                .setSpecularColor(data.lightSpecularColor)
                .setRange(data.range),
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

    const Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});

    /* Analytical output check. Comment this out when image comparison fails
       for easier debugging. */
    for(const auto& pick: data.picks) {
        CORRADE_ITERATION(pick.first);
        CORRADE_COMPARE_WITH(
            image.pixels<Color4ub>()[pick.first.y()][pick.first.x()].xyz(),
            pick.second, TestSuite::Compare::around(0x010101_rgb));
    }

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
        Containers::arrayCast<const Color3ub>(image.pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "PhongTestFiles", data.file}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void PhongGLTest::renderLightsSetOneByOne() {
    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    Matrix4 transformation =
        Matrix4::translation({0.0f, 0.0f, -1.5f});

    PhongGL{{}, 2}
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

    MAGNUM_VERIFY_NO_GL_ERROR();

    const Image2D image = _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm});

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
        Containers::arrayCast<const Color3ub>(image.pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "PhongTestFiles/light-point-range1.5.tga"}),
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
    PhongGL{{}, 1}
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/low-light-angle.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::renderLightCulling() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
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
    /* Put one light into the first 32-bit component, one into the second to
       test that both halves are checked correctly */
    PhongLightUniform lights[64];
    lights[57] = PhongLightUniform{}
        .setPosition({-3.0f, -3.0f, 2.0f, 0.0f})
        .setColor(0x993366_rgbf);
    lights[58] = PhongLightUniform{}
        .setPosition({3.0f, -3.0f, 2.0f, 0.0f})
        .setColor(0x669933_rgbf);
    GL::Buffer lightUniform{lights};

    PhongGL shader{PhongGL::Flag::UniformBuffers|PhongGL::Flag::LightCulling, 64};
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
       and Apple A8 has bigger rounding differences. */
    const Float maxThreshold = 8.34f, meanThreshold = 0.100f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 15.34f, meanThreshold = 3.33f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "PhongTestFiles/colored.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}
#endif

template<PhongGL::Flag flag> void PhongGLTest::renderZeroLights() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES2
    if(flag == PhongGL::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

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
    PhongGL shader{flags|flag, 0};

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D ambient;
    Containers::Optional<Trade::ImageData2D> ambientImage;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-alpha-texture.tga")) && (ambientImage = importer->image2D(0)));
    ambient.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, ambientImage->size())
        .setSubImage(0, {}, *ambientImage);

    shader.bindAmbientTexture(ambient);

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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        /* Should be equivalent to masked Flat3D */
        Utility::Directory::join(_testDir, "FlatTestFiles/textured3D-alpha-mask0.5.tga"),
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

template<PhongGL::Flag flag> void PhongGLTest::renderInstanced() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
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
        .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
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

    /* Enable also Object ID, if supported */
    PhongGL::Flags flags = PhongGL::Flag::VertexColor|
          PhongGL::Flag::InstancedTransformation|data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        flags |= PhongGL::Flag::InstancedObjectId;
    }
    #endif
    PhongGL shader{flags, 2};

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
            CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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
                if(flag != PhongGL::Flag::UniformBuffers)
                    shader.setTextureLayer(2); /* base offset */

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
            CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/normal-texture.tga")) && (image = importer->image2D(0)));

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
        if((data.flags & PhongGL::Flag::TextureArrays) && flag != PhongGL::Flag::UniformBuffers)
            shader.setTextureLayer(2); /* base offset */
        #endif

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
    else if(flag == PhongGL::Flag::UniformBuffers) {
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
                .setObjectId(1000) /* gets added to the per-instance ID */
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "PhongTestFiles", data.file}),
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
void PhongGLTest::renderMulti() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & PhongGL::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
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

    PhongGL shader{PhongGL::Flag::UniformBuffers|PhongGL::Flag::ObjectId|PhongGL::Flag::LightCulling|data.flags, data.lightCount, data.materialCount, data.drawCount};

    GL::Texture2D diffuse{NoCreate};
    GL::Texture2DArray diffuseArray{NoCreate};
    if(data.flags & PhongGL::Flag::DiffuseTexture) {
        if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
           !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
        CORRADE_VERIFY(importer);

        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));

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

            diffuseArray = GL::Texture2DArray{};
            diffuseArray.setMinificationFilter(GL::SamplerFilter::Linear)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                /* Each slice has half the height */
                .setStorage(1, TextureFormatRGB, {image->size().x(), image->size().y()/2, 3})
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
        .setIndexRange(sphereData.indexCount());
    GL::MeshView cone{mesh};
    cone.setCount(coneData.indexCount())
        .setIndexRange(sphereData.indexCount() + planeData.indexCount());

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
        .setMaterialId(data.drawCount == 1 ? 0 : 1)
        .setLightOffsetCount(data.drawCount == 1 ? 0 : 1, 2)
        .setNormalMatrix(transformationData[0*data.uniformIncrement].transformationMatrix.normalMatrix())
        .setObjectId(1211);
    drawData[1*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 0)
        .setLightOffsetCount(data.drawCount == 1 ? 0 : 3, 1)
        .setNormalMatrix(transformationData[1*data.uniformIncrement].transformationMatrix.normalMatrix())
        .setObjectId(5627);
    drawData[2*data.uniformIncrement] = PhongDrawUniform{}
        .setMaterialId(data.drawCount == 1 ? 0 : 1)
        .setLightOffsetCount(data.drawCount == 1 ? 0 : 0, 1)
        .setNormalMatrix(transformationData[2*data.uniformIncrement].transformationMatrix.normalMatrix())
        .setObjectId(36363);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    shader.bindProjectionBuffer(projectionUniform);

    /* Just one draw, rebinding UBOs each time */
    if(data.drawCount == 1) {
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
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "PhongTestFiles", data.expected}),
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

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGLTest)
