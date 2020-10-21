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
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

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
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct PhongGLTest: GL::OpenGLTester {
    explicit PhongGLTest();

    void construct();

    void constructMove();

    void constructTextureTransformationNotTextured();

    void bindTexturesNotEnabled();
    void setAlphaMaskNotEnabled();
    void setTextureMatrixNotEnabled();
    #ifndef MAGNUM_TARGET_GLES2
    void setObjectIdNotEnabled();
    #endif
    void setWrongLightCount();
    void setWrongLightId();

    void renderSetup();
    void renderTeardown();

    void renderDefaults();
    void renderColored();
    void renderSinglePixelTextured();

    void renderTextured();
    void renderTexturedNormal();

    template<class T> void renderVertexColor();

    void renderShininess();

    void renderAlphaSetup();
    void renderAlphaTeardown();

    void renderAlpha();

    #ifndef MAGNUM_TARGET_GLES2
    void renderObjectIdSetup();
    void renderObjectIdTeardown();

    void renderObjectId();
    #endif

    void renderLights();
    void renderLightsSetOneByOne();
    void renderLowLightAngle();
    void renderZeroLights();

    void renderInstanced();

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
    .   Mesa llvmpipe
    -   SwiftShader ES2/ES3
    -   ARM Mali (Huawei P10) ES2/ES3 (except instancing)
    -   WebGL 1 / 2 (on Mesa Intel) (except instancing)
    -   NVidia Windows (except instancing)
    -   Intel Windows (except instancing)
    -   AMD on macOS (except instancing)
    -   iPhone 6 w/ iOS 12.4 (except instancing)
*/

constexpr struct {
    const char* name;
    Phong::Flags flags;
    UnsignedInt lightCount;
} ConstructData[]{
    {"", {}, 1},
    {"ambient texture", Phong::Flag::AmbientTexture, 1},
    {"diffuse texture", Phong::Flag::DiffuseTexture, 1},
    {"diffuse texture + texture transform", Phong::Flag::DiffuseTexture|Phong::Flag::TextureTransformation, 1},
    {"specular texture", Phong::Flag::SpecularTexture, 1},
    {"normal texture", Phong::Flag::NormalTexture, 1},
    {"normal texture + separate bitangents", Phong::Flag::NormalTexture|Phong::Flag::Bitangent, 1},
    {"separate bitangents alone", Phong::Flag::Bitangent, 1},
    {"ambient + diffuse texture", Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture, 1},
    {"ambient + specular texture", Phong::Flag::AmbientTexture|Phong::Flag::SpecularTexture, 1},
    {"diffuse + specular texture", Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture, 1},
    {"ambient + diffuse + specular texture", Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture, 1},
    {"ambient + diffuse + specular + normal texture", Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture|Phong::Flag::NormalTexture, 1},
    {"alpha mask", Phong::Flag::AlphaMask, 1},
    {"alpha mask + diffuse texture", Phong::Flag::AlphaMask|Phong::Flag::DiffuseTexture, 1},
    {"vertex colors", Phong::Flag::VertexColor, 1},
    {"vertex colors + diffuse texture", Phong::Flag::VertexColor|Phong::Flag::DiffuseTexture, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", Phong::Flag::ObjectId, 1},
    {"instanced object ID", Phong::Flag::InstancedObjectId, 1},
    {"object ID + alpha mask + specular texture", Phong::Flag::ObjectId|Phong::Flag::AlphaMask|Phong::Flag::SpecularTexture, 1},
    #endif
    {"five lights", {}, 5},
    {"zero lights", {}, 0},
    {"instanced transformation", Phong::Flag::InstancedTransformation, 3},
    {"instanced specular texture offset", Phong::Flag::SpecularTexture|Phong::Flag::InstancedTextureOffset, 3},
    {"instanced normal texture offset", Phong::Flag::NormalTexture|Phong::Flag::InstancedTextureOffset, 3}
};

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
    bool multiBind;
} RenderSinglePixelTexturedData[]{
    {"", false},
    {"multi bind", true}
};

const struct {
    const char* name;
    const char* expected;
    Phong::Flags flags;
    Matrix3 textureTransformation;
} RenderTexturedData[]{
    {"all", "textured.tga", Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture, {}},
    {"ambient", "textured-ambient.tga", Phong::Flag::AmbientTexture, {}},
    {"diffuse", "textured-diffuse.tga", Phong::Flag::DiffuseTexture, {}},
    {"diffuse transformed", "textured-diffuse-transformed.tga",
        Phong::Flag::DiffuseTexture|Phong::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f})
    },
    {"specular", "textured-specular.tga", Phong::Flag::SpecularTexture, {}}
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
    Shaders::Phong::Tangent4::Components tangentComponents;
    bool flipNormalY;
    Shaders::Phong::Flags flags;
} RenderTexturedNormalData[]{
    {"", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    {"multi bind", "textured-normal.tga", true, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    {"rotated 90°", "textured-normal.tga", false, 90.0_degf, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    {"rotated -90°", "textured-normal.tga", false, -90.0_degf, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    {"0.5 scale", "textured-normal0.5.tga", false, {}, 0.5f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    {"0.0 scale", "textured-normal0.0.tga", false, {}, 0.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    /* The fourth component, if missing, gets automatically filled up to 1,
       so this should work */
    {"implicit bitangent direction", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 0.0f}, {},
        Shaders::Phong::Tangent4::Components::Three, false, {}},
    {"separate bitangents", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
        Shaders::Phong::Tangent4::Components::Three, false,
        Shaders::Phong::Flag::Bitangent},
    {"right-handed, flipped Y", "textured-normal-left.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, true, {}},
    {"left-handed", "textured-normal-left.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, -1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, false, {}},
    {"left-handed, separate bitangents", "textured-normal-left.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
        Shaders::Phong::Tangent4::Components::Three, false,
        Shaders::Phong::Flag::Bitangent},
    {"left-handed, flipped Y", "textured-normal.tga", false, {}, 1.0f,
        {1.0f, 0.0f, 0.0f, -1.0f}, {},
        Shaders::Phong::Tangent4::Components::Four, true, {}}
};

const struct {
    const char* name;
    const char* expected;
    Float shininess;
    Color4 specular;
} RenderShininessData[] {
    {"80", "shininess80.tga", 80.0f, 0xffffff_rgbf},
    {"10", "shininess10.tga", 10.0f, 0xffffff_rgbf},
    {"0", "shininess0.tga", 0.0f, 0xffffff_rgbf},
    {"0.001", "shininess0.tga", 0.001f, 0xffffff_rgbf},
    {"black specular", "shininess-black-specular.tga", 80.0f, 0x000000_rgbf}
};

const struct {
    const char* name;
    const char* expected;
    bool blending;
    Phong::Flags flags;
    Float threshold;
    const char* ambientTexture;
    const char* diffuseTexture;
    Color4 ambientColor;
    Color4 diffuseColor;
} RenderAlphaData[] {
    /* All those deliberately have a non-white diffuse in order to match the
       expected data from textured() */
    {"none, separate", "PhongTestFiles/textured-diffuse.tga", false,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture, 0.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"none, combined", "PhongTestFiles/textured-diffuse.tga", false,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture, 0.0f,
        "diffuse-alpha-texture.tga", "diffuse-alpha-texture.tga",
        0x000000ff_rgbaf, 0x9999ff00_rgbaf},
    {"blending, separate", "PhongTestFiles/textured-diffuse-alpha.tga", true,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture, 0.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"blending, combined", "PhongTestFiles/textured-diffuse-alpha.tga", true,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture, 0.0f,
        "diffuse-alpha-texture.tga", "diffuse-alpha-texture.tga",
        0x000000ff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 0.0, separate", "PhongTestFiles/textured-diffuse.tga", false,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::AlphaMask, 0.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 0.5, separate", "PhongTestFiles/textured-diffuse-alpha-mask0.5.tga", false,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::AlphaMask, 0.5f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 0.5, combined", "PhongTestFiles/textured-diffuse-alpha-mask0.5.tga", false,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::AlphaMask, 0.5f,
        "diffuse-alpha-texture.tga", "diffuse-alpha-texture.tga",
        0x000000ff_rgbaf, 0x9999ff00_rgbaf},
    {"masking 1.0, separate", "TestFiles/alpha-mask1.0.tga", false,
        Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::AlphaMask, 1.0f,
        "alpha-texture.tga", "diffuse-texture.tga",
        0xffffffff_rgbaf, 0x9999ff00_rgbaf}
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    Phong::Flags flags;
    UnsignedInt uniformId;
    UnsignedInt instanceCount;
    UnsignedInt expected;
} RenderObjectIdData[] {
    {"", /* Verify that it can hold 16 bits at least */
        Phong::Flag::ObjectId, 48526, 0, 48526},
    {"instanced, first instance",
        Phong::Flag::InstancedObjectId, 13524, 1, 24526},
    {"instanced, second instance",
        Phong::Flag::InstancedObjectId, 13524, 2, 62347}
};
#endif

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
        {Containers::InPlaceInit, {
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
        {Containers::InPlaceInit, {
            /* Only ambient color left */
            {{40, 40}, 0x222222_rgb}
        }}},
    /* This is the same as above, except that twice the intensity causes it to
       be 2x brighter */
    {"directional, intensity=2", "light-directional-intensity2.tga",
        {1.0f, -1.5f, 0.5f, 0.0f}, Color3{1.0f}, Color3{1.0f},
        2.0f, 1.0f,
        {Containers::InPlaceInit, {
            {{40, 40}, 0x222222_rgb + 0xff8080_rgb*dot(Vector3{1.0f, -1.5f, 0.5f}.normalized(), Vector3::zAxis())*2.0f}
        }}},
    {"point", "light-point.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, Constants::inf(),
        {Containers::InPlaceInit, {
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
        {Containers::InPlaceInit, {
            /* Colored specular highlight */
            {{60, 19}, 0xc27573_rgb}
        }}},
    {"point, specular light color", "light-point-specular-color.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, 0x80ff80_rgbf,
        1.0f, Constants::inf(),
        {Containers::InPlaceInit, {
            /* Colored specular highlight */
            {{60, 19}, 0xc27573_rgb}
        }}},
    {"point, attenuated specular", "light-point-attenuated-specular.tga",
        {1.0f, -1.0f, -0.25f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 2.5f,
        {Containers::InPlaceInit, {
            /* Specular highlight shouldn't be brighter than the attenuated
               intensity */
            {{57, 22}, 0xa68787_rgb}
        }}},
    {"point, range=1.5, specular color", "light-point-range1.5.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, 0x80ff80_rgbf,
        1.0f, 1.5f,
        {Containers::InPlaceInit, {
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
    /* Distance is 0, which means the direction is always prependicular and
       thus contributes nothing */
    {"point, distance=0", "light-none.tga",
        {0.75f, -0.75f, -0.75f, 1.0f}, Color3{1.0f}, Color3{1.0f},
        1.0f, 0.0f, {}}
};

constexpr struct {
    const char* name;
    const char* file;
    Phong::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData[] {
    {"diffuse", "instanced.tga", {},
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* AMD has one off pixel; SwiftShader a bit more */
        96.34f, 0.113f,
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage */
        96.34f, 0.113f,
        #endif
        },
    {"diffuse + normal", "instanced-normal.tga", Phong::Flag::NormalTexture,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* AMD has one off pixel, llvmpipe more */
        96.0f, 0.333f,
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage */
        96.0f, 0.333f,
        #endif
        }
};

PhongGLTest::PhongGLTest() {
    addInstancedTests({&PhongGLTest::construct}, Containers::arraySize(ConstructData));

    addTests({&PhongGLTest::constructMove,

              &PhongGLTest::constructTextureTransformationNotTextured,

              &PhongGLTest::bindTexturesNotEnabled,
              &PhongGLTest::setAlphaMaskNotEnabled,
              &PhongGLTest::setTextureMatrixNotEnabled,
              #ifndef MAGNUM_TARGET_GLES2
              &PhongGLTest::setObjectIdNotEnabled,
              #endif
              &PhongGLTest::setWrongLightCount,
              &PhongGLTest::setWrongLightId});

    addTests({&PhongGLTest::renderDefaults},
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderColored},
        Containers::arraySize(RenderColoredData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderSinglePixelTextured},
        Containers::arraySize(RenderSinglePixelTexturedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderTextured},
        Containers::arraySize(RenderTexturedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderTexturedNormal},
        Containers::arraySize(RenderTexturedNormalData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addTests({&PhongGLTest::renderVertexColor<Color3>,
              &PhongGLTest::renderVertexColor<Color4>},
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderShininess},
        Containers::arraySize(RenderShininessData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addInstancedTests({&PhongGLTest::renderAlpha},
        Containers::arraySize(RenderAlphaData),
        &PhongGLTest::renderAlphaSetup,
        &PhongGLTest::renderAlphaTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGLTest::renderObjectId},
        Containers::arraySize(RenderObjectIdData),
        &PhongGLTest::renderObjectIdSetup,
        &PhongGLTest::renderObjectIdTeardown);
    #endif

    addInstancedTests({&PhongGLTest::renderLights},
        Containers::arraySize(RenderLightsData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addTests({&PhongGLTest::renderLightsSetOneByOne,
              &PhongGLTest::renderLowLightAngle},
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

    addTests({&PhongGLTest::renderZeroLights},
        #ifndef MAGNUM_TARGET_GLES2
        &PhongGLTest::renderObjectIdSetup,
        &PhongGLTest::renderObjectIdTeardown
        #else
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown
        #endif
    );

    addInstancedTests({&PhongGLTest::renderInstanced},
        Containers::arraySize(RenderInstancedData),
        &PhongGLTest::renderSetup,
        &PhongGLTest::renderTeardown);

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
    if((data.flags & Phong::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    Phong shader{data.flags, data.lightCount};
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

void PhongGLTest::constructMove() {
    Phong a{Phong::Flag::AlphaMask, 3};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Phong b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), Phong::Flag::AlphaMask);
    CORRADE_COMPARE(b.lightCount(), 3);
    CORRADE_VERIFY(!a.id());

    Phong c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), Phong::Flag::AlphaMask);
    CORRADE_COMPARE(c.lightCount(), 3);
    CORRADE_VERIFY(!b.id());
}

void PhongGLTest::constructTextureTransformationNotTextured() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Phong{Phong::Flag::TextureTransformation};
    CORRADE_COMPARE(out.str(),
        "Shaders::Phong: texture transformation enabled but the shader is not textured\n");
}

void PhongGLTest::bindTexturesNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    Phong shader;
    shader.bindAmbientTexture(texture)
          .bindDiffuseTexture(texture)
          .bindSpecularTexture(texture)
          .bindNormalTexture(texture)
          .setNormalTextureScale(0.5f)
          .bindTextures(&texture, &texture, &texture, &texture);

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::bindAmbientTexture(): the shader was not created with ambient texture enabled\n"
        "Shaders::Phong::bindDiffuseTexture(): the shader was not created with diffuse texture enabled\n"
        "Shaders::Phong::bindSpecularTexture(): the shader was not created with specular texture enabled\n"
        "Shaders::Phong::bindNormalTexture(): the shader was not created with normal texture enabled\n"
        "Shaders::Phong::setNormalTextureScale(): the shader was not created with normal texture enabled\n"
        "Shaders::Phong::bindTextures(): the shader was not created with any textures enabled\n");
}

void PhongGLTest::setAlphaMaskNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Phong shader;
    shader.setAlphaMask(0.75f);

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

void PhongGLTest::setTextureMatrixNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Phong shader;
    shader.setTextureMatrix({});

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGLTest::setObjectIdNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Phong shader;
    shader.setObjectId(33376);

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

void PhongGLTest::setWrongLightCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Phong{{}, 5}
        .setLightColors({Color3{}})
        .setLightPositions({Vector4{}})
        .setLightRanges({0.0f});
    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setLightColors(): expected 5 items but got 1\n"
        "Shaders::Phong::setLightPositions(): expected 5 items but got 1\n"
        "Shaders::Phong::setLightRanges(): expected 5 items but got 1\n");
}

void PhongGLTest::setWrongLightId() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Phong{{}, 3}
        .setLightColor(3, Color3{})
        .setLightPosition(3, Vector4{})
        .setLightRange(3, 0.0f);
    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setLightColor(): light ID 3 is out of bounds for 3 lights\n"
        "Shaders::Phong::setLightPosition(): light ID 3 is out of bounds for 3 lights\n"
        "Shaders::Phong::setLightRange(): light ID 3 is out of bounds for 3 lights\n");
}

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

void PhongGLTest::renderDefaults() {
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    Phong{}
        .draw(sphere);

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

void PhongGLTest::renderColored() {
    auto&& data = RenderColoredData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    Phong{{}, 2}
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

void PhongGLTest::renderSinglePixelTextured() {
    auto&& data = RenderSinglePixelTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    const Color4ub ambientData[]{ 0x330033_rgb };
    ImageView2D ambientImage{PixelFormat::RGBA8Unorm, Vector2i{1}, ambientData};
    GL::Texture2D ambient;
    ambient.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, Vector2i{1})
        .setSubImage(0, {}, ambientImage);

    const Color4ub diffuseData[]{ 0xccffcc_rgb };
    ImageView2D diffuseImage{PixelFormat::RGBA8Unorm, Vector2i{1}, diffuseData};
    GL::Texture2D diffuse;
    diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, Vector2i{1})
        .setSubImage(0, {}, diffuseImage);

    const Color4ub specularData[]{ 0x6666ff_rgb };
    ImageView2D specularImage{PixelFormat::RGBA8Unorm, Vector2i{1}, specularData};
    GL::Texture2D specular;
    specular.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGBA, Vector2i{1})
        .setSubImage(0, {}, specularImage);

    Phong shader{Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture, 2};
    shader.setLightColors({0x993366_rgbf, 0x669933_rgbf})
        .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                            { 3.0f, -3.0f, 2.0f, 0.0f}})
        .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f));

    if(data.multiBind)
        shader.bindTextures(&ambient, &diffuse, &specular, nullptr);
    else shader
        .bindAmbientTexture(ambient)
        .bindDiffuseTexture(diffuse)
        .bindSpecularTexture(specular);

    shader.draw(sphere);

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

void PhongGLTest::renderTextured() {
    auto&& data = RenderTexturedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    Phong shader{data.flags, 2};

    if(data.textureTransformation != Matrix3{})
        shader.setTextureMatrix(data.textureTransformation);

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D ambient;
    if(data.flags & Phong::Flag::AmbientTexture) {
        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/ambient-texture.tga")) && (image = importer->image2D(0)));
        ambient.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, image->size())
            .setSubImage(0, {}, *image);
        shader
            .bindAmbientTexture(ambient)
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured() */
            .setAmbientColor(0xff9999_rgbf);
    }

    /* If no diffuse texture is present, dial down the default diffuse color
       so ambient/specular is visible */
    GL::Texture2D diffuse;
    if(data.flags & Phong::Flag::DiffuseTexture) {
        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
        diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, image->size())
            .setSubImage(0, {}, *image);
        shader
            .bindDiffuseTexture(diffuse)
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured() */
            .setDiffuseColor(0x9999ff_rgbf);
    } else shader.setDiffuseColor(0x333333_rgbf);

    GL::Texture2D specular;
    if(data.flags & Phong::Flag::SpecularTexture) {
        Containers::Optional<Trade::ImageData2D> image;
        CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/specular-texture.tga")) && (image = importer->image2D(0)));
        specular.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatRGB, image->size())
            .setSubImage(0, {}, *image);
        shader
            .bindSpecularTexture(specular)
            /* Colorized. Case without a color (where it should be white) is
               tested in renderSinglePixelTextured() */
            .setSpecularColor(0x99ff99_rgbf);
    }

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

void PhongGLTest::renderTexturedNormal() {
    auto&& data = RenderTexturedNormalData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

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

    GL::Texture2D normal;
    normal.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid( Primitives::PlaneFlag::TextureCoordinates));

    /* Add tangents / bitangents of desired component count. Unused components
       are set to zero to ensure the shader doesn't use them. */
    const struct TangentBitangent {
        Vector4 tangent;
        Vector3 bitangent;
    } tangentBitangent{data.tangent, data.bitangent};
    GL::Buffer tangents;
    tangents.setData(Containers::Array<TangentBitangent>{Containers::DirectInit, 4, tangentBitangent});
    plane.addVertexBuffer(tangents, 0, sizeof(TangentBitangent),
        GL::DynamicAttribute{Shaders::Phong::Tangent4{data.tangentComponents}});
    plane.addVertexBuffer(std::move(tangents), sizeof(Vector4),
        sizeof(TangentBitangent),
        GL::DynamicAttribute{Shaders::Phong::Bitangent{}});

    /* Rotating the view a few times (together with light positions). If the
       tangent transformation in the shader is correct, it should result in
       exactly the same images. */
    Phong shader{Phong::Flag::NormalTexture|data.flags, 2};
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
        .setDiffuseColor(0x999999_rgbf);

    /* Verify the default is working properly */
    if(data.scale != 1.0f)
        shader.setNormalTextureScale(data.scale);

    if(data.multiBind)
        shader.bindTextures(nullptr, nullptr, nullptr, &normal);
    else
        shader.bindNormalTexture(normal);

    shader.draw(plane);

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

template<class T> void PhongGLTest::renderVertexColor() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);

    /* Highlight the pole vertices and the middle rings */
    Containers::Array<T> colorData{Containers::DirectInit, sphereData.vertexCount(), 0x999999_rgbf};
    for(std::size_t i = 0; i != 3*33 + 1; ++i)
        colorData[sphereData.vertexCount()  - i - 1] = 0xff0000_rgbf*5.0f;
    for(std::size_t i = 6*33; i != 9*33; ++i)
        colorData[i + 1] = 0xffff99_rgbf*1.5f;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<Shaders::Phong::Color3::Location, T>{});

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

    Phong{Phong::Flag::DiffuseTexture|Phong::Flag::VertexColor, 2}
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
        .bindDiffuseTexture(diffuse)
        .draw(sphere);

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

void PhongGLTest::renderShininess() {
    auto&& data = RenderShininessData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    Phong{}
        .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f}})
        .setDiffuseColor(0xff3333_rgbf)
        .setSpecularColor(data.specular)
        .setShininess(data.shininess)
        .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .draw(sphere);

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
        const Float maxThreshold = 12.0f, meanThreshold = 0.043f;
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
        CORRADE_EXPECT_FAIL_IF(data.shininess == 0.0f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Mesa) && GL::Context::current().rendererString().find("AMD") != std::string::npos,
            "AMD Mesa drivers have a much larger ring for the overflown shininess when it's exactly 0.");
        CORRADE_EXPECT_FAIL_IF(data.shininess <= 0.0011f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Mesa) && GL::Context::current().rendererString().find("llvmpipe") != std::string::npos,
            "Mesa llvmpipe drivers have a much larger ring for the overflown shininess.");
        #endif
        #if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
        CORRADE_EXPECT_FAIL_IF(data.shininess == 0.0f && GL::Context::current().rendererString().find("AMD") != std::string::npos,
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
        || (data.shininess == 0.0f && (GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Mesa) && GL::Context::current().rendererString().find("AMD") != std::string::npos)
        #endif
        #if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
        || (data.shininess == 0.0f && GL::Context::current().rendererString().find("AMD") != std::string::npos)
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

void PhongGLTest::renderAlpha() {
    auto&& data = RenderAlphaData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

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

    Phong shader{data.flags, 2};
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
        .setSpecularColor(0xffffff00_rgbaf)
        .bindTextures(&ambient, &diffuse, nullptr, nullptr);

    /* Test that the default is correct by not setting the threshold if it's
       equal to the default */
    if(data.flags & Phong::Flag::AlphaMask && data.threshold != 0.5f)
        shader.setAlphaMask(data.threshold);

    /* For proper Z order draw back faces first and then front faces */
    GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Front);
    shader.draw(sphere);
    GL::Renderer::setFaceCullingMode(GL::Renderer::PolygonFacing::Back);
    shader.draw(sphere);

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
                {Phong::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                {Phong::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
            })
            .clearColor(1, Vector4ui{27});
    }
}

void PhongGLTest::renderObjectIdTeardown() {
    _color = GL::Renderbuffer{NoCreate};
    _objectId = GL::Renderbuffer{NoCreate};
    _framebuffer = GL::Framebuffer{NoCreate};
}

void PhongGLTest::renderObjectId() {
    auto&& data = RenderObjectIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    if(data.instanceCount) sphere
        .setInstanceCount(data.instanceCount)
        .addVertexBufferInstanced(
            GL::Buffer{Containers::arrayView({11002u, 48823u})},
            1, 0, Phong::ObjectId{});

    Phong{data.flags, 2}
        .setLightColors({0x993366_rgbf, 0x669933_rgbf})
        .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                            { 3.0f, -3.0f, 2.0f, 0.0f}})
        .setAmbientColor(0x330033_rgbf)
        .setDiffuseColor(0xccffcc_rgbf)
        .setSpecularColor(0x6666ff_rgbf)
        .setTransformationMatrix(Matrix4::translation(Vector3::zAxis(-2.15f)))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .setObjectId(data.uniformId)
        .draw(sphere);

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
    CORRADE_COMPARE(image.pixels<UnsignedInt>()[40][46], data.expected);
}
#endif

void PhongGLTest::renderLights() {
    auto&& data = RenderLightsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    Matrix4 transformation =
        Matrix4::translation({0.0f, 0.0f, -1.5f});

    Phong{{}, 1}
        /* Set non-black ambient to catch accidental NaNs -- the render should
           never be fully black */
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

    Phong{{}, 2}
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
    Phong{{}, 1}
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

void PhongGLTest::renderZeroLights() {
    CORRADE_COMPARE(_framebuffer.checkStatus(GL::FramebufferTarget::Draw), GL::Framebuffer::Status::Complete);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates));

    /* Enable also Object ID, if supported */
    Phong::Flags flags = Phong::Flag::AmbientTexture|Phong::Flag::NormalTexture|Phong::Flag::AlphaMask;
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        flags |= Phong::Flag::ObjectId;
    }
    #endif
    Phong shader{flags, 0};

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

    GL::Texture2D bogus;

    shader
        .bindAmbientTexture(ambient)
        .setAmbientColor(0x9999ff_rgbf)
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        /* Keep alpha mask at the default 0.5 to test the default */
        /* Passing a zero-sized light position / color array, shouldn't assert */
        .setLightPositions(Containers::ArrayView<const Vector4>{})
        .setLightColors(Containers::ArrayView<const Color3>{})
        /* Using a bogus normal matrix -- it's not used so it should be okay.
           Same for all other unused values, they should get ignored. */
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

void PhongGLTest::renderInstanced() {
    auto&& data = RenderInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(GL::Extensions::ARB::instanced_arrays::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::instanced_arrays>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("GL_{ANGLE,EXT,NV}_instanced_arrays is not supported");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(GL::Extensions::ANGLE::instanced_arrays::string() + std::string(" is not supported"));
    #endif
    #endif

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates|
        Primitives::UVSphereFlag::Tangents));

    /* Three spheres, each in a different location, differently rotated to
       ensure the normal matrix is properly used as well. */
    struct {
        Matrix4 transformation;
        Matrix3x3 normal;
        Color3 color;
        Vector2 textureOffset;
    } instanceData[] {
        {Matrix4::translation({-1.25f, -1.25f, 0.0f})*
         Matrix4::rotationX(90.0_degf),
            {}, 0xff3333_rgbf, {0.0f, 0.0f}},
        {Matrix4::translation({ 1.25f, -1.25f, 0.0f})*
         Matrix4::rotationY(90.0_degf),
            {}, 0x33ff33_rgbf, {1.0f, 0.0f}},
        {Matrix4::translation({  0.0f,  1.0f, 1.0f})*
         Matrix4::rotationZ(90.0_degf),
            {}, 0x9999ff_rgbf, {0.5f, 1.0f}}
    };
    for(auto& instance: instanceData)
        instance.normal = instance.transformation.normalMatrix();

    sphere
        .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
            Phong::TransformationMatrix{},
            Phong::NormalMatrix{},
            Phong::Color3{},
            Phong::TextureOffset{})
        .setInstanceCount(3);

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/diffuse-texture.tga")) && (image = importer->image2D(0)));
    GL::Texture2D diffuse;
    diffuse.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(_testDir, "TestFiles/normal-texture.tga")) && (image = importer->image2D(0)));
    GL::Texture2D normal;
    normal.setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, TextureFormatRGB, image->size())
        .setSubImage(0, {}, *image);

    Phong shader{Phong::Flag::DiffuseTexture|
          Phong::Flag::VertexColor|
          Phong::Flag::InstancedTransformation|
          Phong::Flag::InstancedTextureOffset|data.flags, 2};
    shader
        .setLightPositions({{-3.0f, -3.0f, 2.0f, 0.0f},
                            { 3.0f, -3.0f, 2.0f, 0.0f}})
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-1.75f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf)*
            Matrix4::scaling(Vector3{0.4f}))
        .setNormalMatrix((Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf)).normalMatrix())
        .setProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .setTextureMatrix(Matrix3::scaling(Vector2{0.5f}))
        .bindDiffuseTexture(diffuse)
        .setDiffuseColor(0xffff99_rgbf);

    if(data.flags & Phong::Flag::NormalTexture)
        shader.bindNormalTexture(normal);

    shader.draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "PhongTestFiles", data.file}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGLTest)
