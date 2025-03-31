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
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/System.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Shaders/VectorGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/TextureArray.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/Cone.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Vector.h"
#endif

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VectorGLTest: GL::OpenGLTester {
    explicit VectorGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructAsync();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffers();
    template<UnsignedInt dimensions> void constructUniformBuffersAsync();
    #endif

    template<UnsignedInt dimensions> void constructMove();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructMoveUniformBuffers();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffersInvalid();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setUniformUniformBuffersEnabled();
    template<UnsignedInt dimensions> void bindBufferUniformBuffersNotEnabled();
    template<UnsignedInt dimensions> void bindTextureInvalid();
    template<UnsignedInt dimensions> void bindTextureArrayInvalid();
    #endif
    template<UnsignedInt dimensions> void setTextureMatrixNotEnabled();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setTextureLayerNotArray();
    template<UnsignedInt dimensions> void bindTextureTransformBufferNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setWrongDrawOffset();
    #endif

    void renderSetup();
    void renderTeardown();

    template<VectorGL2D::Flag flag = VectorGL2D::Flag{}> void renderDefaults2D();
    template<VectorGL3D::Flag flag = VectorGL3D::Flag{}> void renderDefaults3D();
    template<VectorGL2D::Flag flag = VectorGL2D::Flag{}> void render2D();
    template<VectorGL3D::Flag flag = VectorGL3D::Flag{}> void render3D();

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti2D();
    void renderMulti3D();
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
    [O] UBOs + draw offset
    [M] multidraw

    Mesa Intel                      BOM
               ES2                   xx
               ES3                  BOx
    Mesa AMD                        B
    Mesa llvmpipe                   B
    SwiftShader ES2                 Bxx
                ES3                 B
    ANGLE ES2                        xx
          ES3                       BOM
    ARM Mali (Huawei P10) ES2       Bxx
                          ES3       BOx
    WebGL (on Mesa Intel) 1.0       Bxx
                          2.0       BOM
    NVidia
    Intel Windows
    AMD macOS                         x
    Intel macOS                     BOx
    iPhone 6 w/ iOS 12.4 ES3        B x
*/

using namespace Math::Literals;

constexpr struct {
    const char* name;
    VectorGL2D::Flags flags;
} ConstructData[]{
    {"", {}},
    {"texture transformation", VectorGL2D::Flag::TextureTransformation},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture arrays", VectorGL2D::Flag::TextureArrays},
    {"texture transformation + texture arrays", VectorGL2D::Flag::TextureTransformation|VectorGL2D::Flag::TextureArrays},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    VectorGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {}, 1, 1},
    {"", VectorGL2D::Flag::UniformBuffers, 1, 1},
    {"texture transformation", VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation, 1, 1},
    {"texture arrays", VectorGL2D::Flag::TextureArrays, 1, 1},
    {"texture transformation + texture arrays", VectorGL2D::Flag::TextureTransformation|VectorGL2D::Flag::TextureArrays, 1, 1},
    /* SwiftShader has 256 uniform vectors at most, per-draw is 4+1 in 3D case
       and 3+1 in 2D, per-material 3 */
    {"multiple materials, draws", VectorGL2D::Flag::UniformBuffers, 15, 42},
    {"multidraw with all the things", VectorGL2D::Flag::MultiDraw|VectorGL2D::Flag::TextureTransformation|VectorGL2D::Flag::TextureArrays, 15, 42},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with all the things", VectorGL2D::Flag::ShaderStorageBuffers|VectorGL2D::Flag::MultiDraw|VectorGL2D::Flag::TextureTransformation|VectorGL2D::Flag::TextureArrays, 0, 0}
    #endif
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    VectorGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    /* These two fail for UBOs but not SSBOs */
    {"zero draws", VectorGL2D::Flag::UniformBuffers, 1, 0,
        "draw count can't be zero"},
    {"zero materials", VectorGL2D::Flag::UniformBuffers, 0, 1,
        "material count can't be zero"},
};
#endif

const struct {
    const char* name;
    VectorGL2D::Flags flags;
    Matrix3 textureTransformation;
    bool arrayTextureCoordinates;
    Int layerAttribute, layerUniform;
    Color4 backgroundColor, color;
    const char* file2D;
    const char* file3D;
    bool flip;
} RenderData[] {
    {"texture transformation",
        VectorGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        false, 0, 0, 0x00000000_rgbaf, 0xffffff_rgbf,
        "defaults.tga", "defaults.tga", true},
    {"",
        {}, {},
        false, 0, 0, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    #ifndef MAGNUM_TARGET_GLES2
    {"array texture, 2D coordinates, first layer",
        VectorGL2D::Flag::TextureArrays, {},
        false, 0, 0, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    {"array texture, 2D coordinates, arbitrary layer from uniform",
        VectorGL2D::Flag::TextureArrays, {},
        false, 0, 6, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    {"array texture, 2D coordinates, texture transformation, arbitrary layer from uniform",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        false, 0, 6, 0x00000000_rgbaf, 0xffffff_rgbf,
        "defaults.tga", "defaults.tga", true},
    {"array texture, array coordinates, first layer",
        VectorGL2D::Flag::TextureArrays, {},
        true, 0, 0, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    {"array texture, array coordinates, arbitrary layer from attribute",
        VectorGL2D::Flag::TextureArrays, {},
        true, 6, 0, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    {"array texture, array coordinates, arbitrary layer from uniform",
        VectorGL2D::Flag::TextureArrays, {},
        true, 0, 6, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    {"array texture, array coordinates, arbitrary layer from both",
        VectorGL2D::Flag::TextureArrays, {},
        true, 2, 4, 0x9999ff_rgbf, 0xffff99_rgbf,
        "vector2D.tga", "vector3D.tga", false},
    {"array texture, array coordinates, texture transformation, arbitrary layer from attribute",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        true, 6, 0, 0x00000000_rgbaf, 0xffffff_rgbf,
        "defaults.tga", "defaults.tga", true},
    {"array texture, array coordinates, texture transformation, arbitrary layer from uniform",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        true, 0, 6, 0x00000000_rgbaf, 0xffffff_rgbf,
        "defaults.tga", "defaults.tga", true},
    {"array texture, array coordinates, texture transformation, arbitrary layer from both",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}),
        true, 2, 4, 0x00000000_rgbaf, 0xffffff_rgbf,
        "defaults.tga", "defaults.tga", true},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    VectorGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData[] {
    {"bind with offset", "multidraw2D.tga", "multidraw3D.tga",
        {}, 1, 1, true, 16,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    {"bind with offset, texture array", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::TextureArrays, 1, 1, true, 16,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::ShaderStorageBuffers, 0, 0, true, 16,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    {"bind with offset, texture array, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::ShaderStorageBuffers, 0, 0, true, 16,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    #endif
    {"draw offset", "multidraw2D.tga", "multidraw3D.tga",
        {}, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    {"draw offset, texture array", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::TextureArrays, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::ShaderStorageBuffers, 0, 0, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    {"draw offset, texture array, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::ShaderStorageBuffers, 0, 0, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    #endif
    {"multidraw", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::MultiDraw, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    {"multidraw, texture array", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::MultiDraw, 2, 3, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::ShaderStorageBuffers|VectorGL2D::Flag::MultiDraw, 0, 0, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    {"multidraw, texture array, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VectorGL2D::Flag::TextureArrays|VectorGL2D::Flag::ShaderStorageBuffers|VectorGL2D::Flag::MultiDraw, 0, 0, false, 1,
        /* Minor differences on ARM Mali */
        1.34f, 0.02f},
    #endif
};
#endif

VectorGLTest::VectorGLTest() {
    addInstancedTests<VectorGLTest>({
        &VectorGLTest::construct<2>,
        &VectorGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    addTests<VectorGLTest>({
        &VectorGLTest::constructAsync<2>,
        &VectorGLTest::constructAsync<3>});

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<VectorGLTest>({
        &VectorGLTest::constructUniformBuffers<2>,
        &VectorGLTest::constructUniformBuffers<3>},
        Containers::arraySize(ConstructUniformBuffersData));

    addTests<VectorGLTest>({
        &VectorGLTest::constructUniformBuffersAsync<2>,
        &VectorGLTest::constructUniformBuffersAsync<3>});
    #endif

    addTests<VectorGLTest>({
        &VectorGLTest::constructMove<2>,
        &VectorGLTest::constructMove<3>,

        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::constructMoveUniformBuffers<2>,
        &VectorGLTest::constructMoveUniformBuffers<3>,
        #endif
        });

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<VectorGLTest>({
        &VectorGLTest::constructUniformBuffersInvalid<2>,
        &VectorGLTest::constructUniformBuffersInvalid<3>},
        Containers::arraySize(ConstructUniformBuffersInvalidData));
    #endif

    addTests<VectorGLTest>({
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::setUniformUniformBuffersEnabled<2>,
        &VectorGLTest::setUniformUniformBuffersEnabled<3>,
        &VectorGLTest::bindBufferUniformBuffersNotEnabled<2>,
        &VectorGLTest::bindBufferUniformBuffersNotEnabled<3>,
        &VectorGLTest::bindTextureInvalid<2>,
        &VectorGLTest::bindTextureInvalid<3>,
        &VectorGLTest::bindTextureArrayInvalid<2>,
        &VectorGLTest::bindTextureArrayInvalid<3>,
        #endif
        &VectorGLTest::setTextureMatrixNotEnabled<2>,
        &VectorGLTest::setTextureMatrixNotEnabled<3>,
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::setTextureLayerNotArray<2>,
        &VectorGLTest::setTextureLayerNotArray<3>,
        &VectorGLTest::bindTextureTransformBufferNotEnabled<2>,
        &VectorGLTest::bindTextureTransformBufferNotEnabled<3>,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::setWrongDrawOffset<2>,
        &VectorGLTest::setWrongDrawOffset<3>
        #endif
        });

    /* MSVC needs explicit type due to default template args */
    addTests<VectorGLTest>({
        &VectorGLTest::renderDefaults2D,
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::renderDefaults2D<VectorGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VectorGLTest::renderDefaults2D<VectorGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VectorGLTest::renderDefaults3D,
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::renderDefaults3D<VectorGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VectorGLTest::renderDefaults3D<VectorGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        &VectorGLTest::renderSetup,
        &VectorGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<VectorGLTest>({
        &VectorGLTest::render2D,
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::render2D<VectorGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VectorGLTest::render2D<VectorGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VectorGLTest::render3D,
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGLTest::render3D<VectorGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VectorGLTest::render3D<VectorGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderData),
        &VectorGLTest::renderSetup,
        &VectorGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&VectorGLTest::renderMulti2D,
                       &VectorGLTest::renderMulti3D},
        Containers::arraySize(RenderMultiData),
        &VectorGLTest::renderSetup,
        &VectorGLTest::renderTeardown);
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

template<UnsignedInt dimensions> void VectorGLTest::construct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    VectorGL<dimensions> shader{typename VectorGL<dimensions>::Configuration{}
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

template<UnsignedInt dimensions> void VectorGLTest::constructAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename VectorGL<dimensions>::CompileState state = VectorGL<dimensions>::compile(typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL2D::Flag::TextureTransformation));
    CORRADE_COMPARE(state.flags(), VectorGL2D::Flag::TextureTransformation);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    VectorGL<dimensions> shader{Utility::move(state)};
    CORRADE_VERIFY(shader.isLinkFinished());
    CORRADE_COMPARE(shader.flags(), VectorGL2D::Flag::TextureTransformation);
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
template<UnsignedInt dimensions> void VectorGLTest::constructUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & VectorGL<dimensions>::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= VectorGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
    #endif

    if(data.flags >= VectorGL2D::Flag::MultiDraw) {
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

    VectorGL<dimensions> shader{typename VectorGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
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

template<UnsignedInt dimensions> void VectorGLTest::constructUniformBuffersAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    typename VectorGL<dimensions>::CompileState state = VectorGL<dimensions>::compile(typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation)
        .setMaterialCount(15)
        .setDrawCount(42));
    CORRADE_COMPARE(state.flags(), VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation);
    CORRADE_COMPARE(state.materialCount(), 15);
    CORRADE_COMPARE(state.drawCount(), 42);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    VectorGL<dimensions> shader{Utility::move(state)};
    CORRADE_VERIFY(shader.isLinkFinished());
    CORRADE_COMPARE(shader.flags(), VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation);
    CORRADE_COMPARE(shader.materialCount(), 15);
    CORRADE_COMPARE(shader.drawCount(), 42);
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

template<UnsignedInt dimensions> void VectorGLTest::constructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    VectorGL<dimensions> a{typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::TextureTransformation)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    VectorGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_VERIFY(!a.id());

    VectorGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VectorGLTest::constructMoveUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    VectorGL<dimensions> a{typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    VectorGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), VectorGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_VERIFY(!a.id());

    VectorGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), VectorGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_VERIFY(!b.id());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VectorGLTest::constructUniformBuffersInvalid() {
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
    VectorGL<dimensions>{typename VectorGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out, Utility::format(
        "Shaders::VectorGL: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VectorGLTest::setUniformUniformBuffersEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    VectorGL<dimensions> shader{typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader.setTransformationProjectionMatrix({})
        .setTextureMatrix({})
        .setTextureLayer({})
        .setBackgroundColor({})
        .setColor({});
    CORRADE_COMPARE(out,
        "Shaders::VectorGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::VectorGL::setTextureMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::VectorGL::setTextureLayer(): the shader was created with uniform buffers enabled\n"
        "Shaders::VectorGL::setBackgroundColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::VectorGL::setColor(): the shader was created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void VectorGLTest::bindBufferUniformBuffersNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Buffer buffer;
    VectorGL<dimensions> shader;

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
          .setDrawOffset(0);
    CORRADE_COMPARE(out,
        "Shaders::VectorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VectorGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void VectorGLTest::bindTextureInvalid() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    VectorGL<dimensions> shader{typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::TextureArrays)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindVectorTexture(texture);
    CORRADE_COMPARE(out, "Shaders::VectorGL::bindVectorTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n");
}

template<UnsignedInt dimensions> void VectorGLTest::bindTextureArrayInvalid() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2DArray texture;
    VectorGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.bindVectorTexture(texture);
    CORRADE_COMPARE(out, "Shaders::VectorGL::bindVectorTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n");
}
#endif

template<UnsignedInt dimensions> void VectorGLTest::setTextureMatrixNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    VectorGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureMatrix({});
    CORRADE_COMPARE(out,
        "Shaders::VectorGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VectorGLTest::setTextureLayerNotArray() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    VectorGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureLayer(37);
    CORRADE_COMPARE(out, "Shaders::VectorGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}

template<UnsignedInt dimensions> void VectorGLTest::bindTextureTransformBufferNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    GL::Buffer buffer{GL::Buffer::TargetHint::Uniform};
    VectorGL<dimensions> shader{typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out,
        "Shaders::VectorGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::VectorGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VectorGLTest::setWrongDrawOffset() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    VectorGL<dimensions> shader{typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::VectorGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
}
#endif

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

template<VectorGL2D::Flag flag> void VectorGLTest::renderDefaults2D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VectorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VectorGL2D::Flag::UniformBuffers) {
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

    GL::Mesh square = MeshTools::compile(Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
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

    VectorGL2D shader{VectorGL2D::Configuration{}
        .setFlags(flag)};
    shader.bindVectorTexture(texture);

    if(flag == VectorGL2D::Flag{}) {
        shader.draw(square);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VectorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VectorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            VectorDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            VectorMaterialUniform{}
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(square);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "VectorTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<VectorGL3D::Flag flag> void VectorGLTest::renderDefaults3D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VectorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VectorGL3D::Flag::UniformBuffers) {
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

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates));

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture;
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
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

    VectorGL3D shader{VectorGL3D::Configuration{}
        .setFlags(flag)};
    shader.bindVectorTexture(texture);

    if(flag == VectorGL3D::Flag{}) {
        shader.draw(plane);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VectorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VectorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            VectorDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            VectorMaterialUniform{}
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(plane);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "VectorTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<VectorGL2D::Flag flag> void VectorGLTest::render2D() {
    auto&& data = RenderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VectorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & VectorGL2D::Flag::TextureTransformation ? 2 : 1))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VectorGL2D::Flag::UniformBuffers) {
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

    /** @todo clean this up once MeshData (and primitives?) support array
        coordinates directly */
    struct Vertex {
        Vector2 position;
        Vector3 textureCoords;
    } squareData[] {
        {{ 1.0f, -1.0f}, {1.0f, 0.0f, Float(data.layerAttribute)}},
        {{ 1.0f,  1.0f}, {1.0f, 1.0f, Float(data.layerAttribute)}},
        {{-1.0f, -1.0f}, {0.0f, 0.0f, Float(data.layerAttribute)}},
        {{-1.0f,  1.0f}, {0.0f, 1.0f, Float(data.layerAttribute)}}
    };
    GL::Mesh square{GL::MeshPrimitive::TriangleStrip};
    #ifndef MAGNUM_TARGET_GLES2
    if(data.arrayTextureCoordinates) {
        square.addVertexBuffer(GL::Buffer{squareData}, 0,
            GenericGL2D::Position{},
            GenericGL2D::TextureArrayCoordinates{});
    } else
    #endif
    {
        square.addVertexBuffer(GL::Buffer{squareData}, 0,
            GenericGL2D::Position{},
            GenericGL2D::TextureCoordinates{},
            sizeof(Float));
    }
    square.setCount(4);

    VectorGL2D::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & VectorGL2D::Flag::UniformBuffers && (data.flags & VectorGL2D::Flag::TextureArrays) && !(data.flags & VectorGL2D::Flag::TextureTransformation) && data.layerUniform) {
        CORRADE_INFO("Texture arrays with layer passed from a uniform currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= VectorGL2D::Flag::TextureTransformation;
    }
    #endif
    VectorGL2D shader{VectorGL2D::Configuration{}
        .setFlags(flags)};

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    #endif
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & VectorGL2D::Flag::TextureArrays) {
        textureArray = GL::Texture2DArray{};
        textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatR, {image->size(), data.layerUniform + data.layerAttribute + 1})
            .setSubImage(0, {0, 0, data.layerUniform + data.layerAttribute}, ImageView2D{*image});

        shader.bindVectorTexture(textureArray);
    } else
    #endif
    {
        texture = GL::Texture2D{};
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

        shader.bindVectorTexture(texture);
    }

    if(flag == VectorGL2D::Flag{}) {
        shader.setBackgroundColor(data.backgroundColor)
            .setColor(data.color);
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        else shader.setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::rotation(5.0_degf));
        #ifndef MAGNUM_TARGET_GLES2
        if(data.layerUniform != 0) /* to verify the default */
            shader.setTextureLayer(data.layerUniform);
        #endif
        shader.draw(square);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VectorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VectorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(
                    data.textureTransformation == Matrix3{} ?
                        Matrix3::projection({2.1f, 2.1f})*
                        Matrix3::rotation(5.0_degf) : Matrix3{}
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            VectorDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layerUniform)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            VectorMaterialUniform{}
                .setBackgroundColor(data.backgroundColor)
                .setColor(data.color)
        }};
        if(flags & VectorGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(square);
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
    /* SwiftShader has differently rasterized edges on four pixels */
    const Float maxThreshold = 170.0f, meanThreshold = 0.146f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.962f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Path::join({_testDir, "VectorTestFiles", data.file2D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<VectorGL3D::Flag flag> void VectorGLTest::render3D() {
    auto&& data = RenderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VectorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & VectorGL2D::Flag::TextureTransformation ? 2 : 1))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VectorGL3D::Flag::UniformBuffers) {
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

    struct Vertex {
        Vector3 position;
        Vector3 textureCoords;
    } planeData[] {
        {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, Float(data.layerAttribute)}},
        {{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, Float(data.layerAttribute)}},
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, Float(data.layerAttribute)}},
        {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f, Float(data.layerAttribute)}}
    };
    GL::Mesh plane{GL::MeshPrimitive::TriangleStrip};
    #ifndef MAGNUM_TARGET_GLES2
    if(data.arrayTextureCoordinates) {
        plane.addVertexBuffer(GL::Buffer{planeData}, 0,
            GenericGL3D::Position{},
            GenericGL3D::TextureArrayCoordinates{});
    } else
    #endif
    {
        plane.addVertexBuffer(GL::Buffer{planeData}, 0,
            GenericGL3D::Position{},
            GenericGL3D::TextureCoordinates{},
            sizeof(Float));
    }
    plane.setCount(4);

    VectorGL3D::Flags flags = data.flags|flag;
    #ifndef MAGNUM_TARGET_GLES2
    if(flag & VectorGL3D::Flag::UniformBuffers && (data.flags & VectorGL3D::Flag::TextureArrays) && !(data.flags & VectorGL3D::Flag::TextureTransformation) && data.layerUniform) {
        CORRADE_INFO("Texture arrays with layer passed from a uniform currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= VectorGL3D::Flag::TextureTransformation;
    }
    #endif
    VectorGL3D shader{VectorGL3D::Configuration{}
        .setFlags(flags)};

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    GL::Texture2D texture{NoCreate};
    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2DArray textureArray{NoCreate};
    #endif
    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));
    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & VectorGL3D::Flag::TextureArrays) {
        textureArray = GL::Texture2DArray{};
        textureArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, TextureFormatR, {image->size(), data.layerUniform + data.layerAttribute + 1})
            .setSubImage(0, {0, 0, data.layerUniform + data.layerAttribute}, ImageView2D{*image});

        shader.bindVectorTexture(textureArray);
    } else
    #endif
    {
        texture = GL::Texture2D{};
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

        shader.bindVectorTexture(texture);
    }

    if(flag == VectorGL3D::Flag{}) {
        shader.setBackgroundColor(data.backgroundColor)
            .setColor(data.color);
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        else shader.setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationZ(15.0_degf));
        #ifndef MAGNUM_TARGET_GLES2
        if(data.layerUniform != 0) /* to verify the default */
            shader.setTextureLayer(data.layerUniform);
        #endif
        shader.draw(plane);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VectorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VectorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    data.textureTransformation == Matrix3{} ?
                        Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                        Matrix4::translation(Vector3::zAxis(-2.15f))*
                        Matrix4::rotationY(-15.0_degf)*
                        Matrix4::rotationZ(15.0_degf) : Matrix4{}
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            VectorDrawUniform{}
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layerUniform)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            VectorMaterialUniform{}
                .setBackgroundColor(data.backgroundColor)
                .setColor(data.color)
        }};
        if(flags & VectorGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(plane);
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
    /* SwiftShader has differently rasterized edges on four pixels */
    const Float maxThreshold = 170.0f, meanThreshold = 0.171f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.660f;
    #endif
    CORRADE_COMPARE_WITH(pixels,
        Utility::Path::join({_testDir, "VectorTestFiles", data.file3D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
void VectorGLTest::renderMulti2D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= VectorGL2D::Flag::ShaderStorageBuffers) {
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

    if(data.flags >= VectorGL2D::Flag::MultiDraw) {
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

    VectorGL2D shader{VectorGL2D::Configuration{}
        .setFlags(VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));

    /* For arrays we the original image three times to different offsets in
       three different slices */
    GL::Texture2D vector{NoCreate};
    GL::Texture2DArray vectorArray{NoCreate};
    if(data.flags & VectorGL2D::Flag::TextureArrays) {
        Vector3i size{image->size().x(), image->size().y()*2, 6};

        vectorArray = GL::Texture2DArray{};
        vectorArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, GL::TextureFormat::R8, size)
            /* Clear to all zeros for reproducible output */
            .setSubImage(0, {}, Image3D{PixelFormat::R8Unorm, size, Containers::Array<char>{ValueInit, std::size_t(size.product())}})
            .setSubImage(0, {0, size.y()/4, 1}, ImageView2D{*image})
            .setSubImage(0, {0, size.y()/2, 3}, ImageView2D{*image})
            .setSubImage(0, {0, 0, 5}, ImageView2D{*image});

        shader.bindVectorTexture(vectorArray);
    } else {
        vector = GL::Texture2D{};
        vector.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, GL::TextureFormat::R8, image->size())
            .setSubImage(0, {}, *image);

        shader.bindVectorTexture(vector);
    }

    /* Circle is a fan, square is a strip, make it indexed first */
    Trade::MeshData circleData = MeshTools::generateIndices(Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates));
    Trade::MeshData squareData = MeshTools::generateIndices(Primitives::squareSolid(
        Primitives::SquareFlag::TextureCoordinates));
    Trade::MeshData triangleData = MeshTools::generateIndices(Primitives::circle2DSolid(3,
        Primitives::Circle2DFlag::TextureCoordinates));

    /* Assuming the texture coordinates are the last attribute, add a four-byte
       padding after, which we subsequently abuse as the layer index */
    /** @todo clean this up once MeshData (and primitives?) support array
        coordinates directly */
    Trade::MeshData meshData = MeshTools::interleave(
        MeshTools::concatenate({circleData, squareData, triangleData}),
        {Trade::MeshAttributeData{4}});
    CORRADE_COMPARE(meshData.attributeCount(), 2);
    CORRADE_COMPARE(meshData.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(meshData.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    /* Manual cast because the real attribute type is Vector2 */
    const Containers::StridedArrayView1D<Vector3> textureCoordinates = Containers::arrayCast<Vector3>(meshData.mutableAttribute<Vector2>(Trade::MeshAttribute::TextureCoordinates));

    /* The circle will use the last slice, coming from just the attribute
       alone */
    for(UnsignedInt i = 0; i != circleData.vertexCount(); ++i)
        textureCoordinates[i].z() = 5;
    /* The square will use the third slice, coming from both the attribute and
       the uniform */
    for(UnsignedInt i = 0; i != squareData.vertexCount(); ++i)
        textureCoordinates[circleData.vertexCount() + i].z() = 1;
    /* The triangle will use the second slice, coming from just the uniform.
       The memory isn't initialized by default however, so set the attribute to
       0. */
    for(UnsignedInt i = 0; i != triangleData.vertexCount(); ++i)
        textureCoordinates[circleData.vertexCount() + squareData.vertexCount() + i].z() = 0;

    /* Making some assumptions about the layout for simplicity */
    CORRADE_COMPARE(meshData.attributeStride(0), sizeof(Vector2) + sizeof(Vector3));
    CORRADE_COMPARE(meshData.attributeStride(1), sizeof(Vector2) + sizeof(Vector3));
    CORRADE_COMPARE(meshData.attributeOffset(0), 0);
    CORRADE_COMPARE(meshData.attributeOffset(1), sizeof(Vector2));
    GL::Mesh mesh;
    mesh.addVertexBuffer(GL::Buffer{meshData.vertexData()}, 0,
            GenericGL2D::Position{},
            GenericGL2D::TextureArrayCoordinates{})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, meshData.indexData()}, 0,
            meshData.indexType())
        .setCount(meshData.indexCount());

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

    Containers::Array<VectorMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = VectorMaterialUniform{}
        .setColor(0xff0000_rgbf)
        .setBackgroundColor(0xffcccc_rgbf);
    materialData[1*data.uniformIncrement] = VectorMaterialUniform{}
        .setColor(0x00ff00_rgbf)
        .setBackgroundColor(0xccffcc_rgbf);
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
            /* Additional Y shift + scale in the array slice */
            (data.flags & VectorGL2D::Flag::TextureArrays ?
                Matrix3::translation(Vector2::yAxis(0.0f))*
                Matrix3::scaling(Vector2::yScale(0.5f)) : Matrix3{})*
            Matrix3::translation({0.5f, 0.5f})*
            Matrix3::rotation(180.0_degf)*
            Matrix3::translation({-0.5f, -0.5f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            /* Additional Y shift + scale in the array slice */
            (data.flags & VectorGL2D::Flag::TextureArrays ?
                Matrix3::translation(Vector2::yAxis(0.5f))*
                Matrix3::scaling(Vector2::yScale(0.5f)) : Matrix3{})*
            Matrix3::translation(Vector2::xAxis(1.0f))*
            Matrix3::scaling(Vector2::xScale(-1.0f)))
        .setLayer(2); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            /* Additional Y shift + scale in the array slice */
            (data.flags & VectorGL2D::Flag::TextureArrays ?
                Matrix3::translation(Vector2::yAxis(0.25f))*
                Matrix3::scaling(Vector2::yScale(0.5f)) : Matrix3{}))
        .setLayer(1); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<VectorDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are done with
       UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = VectorDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1);
    drawData[1*data.uniformIncrement] = VectorDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0);
    drawData[2*data.uniformIncrement] = VectorDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(VectorMaterialUniform),
            sizeof(VectorMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(VectorDrawUniform),
            sizeof(VectorDrawUniform));
        shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(circle);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(VectorMaterialUniform),
            sizeof(VectorMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(VectorDrawUniform),
            sizeof(VectorDrawUniform));
        shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(VectorMaterialUniform),
            sizeof(VectorMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(VectorDrawUniform),
            sizeof(VectorDrawUniform));
        shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(triangle);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindTextureTransformationBuffer(textureTransformationUniform)
            .bindMaterialBuffer(materialUniform);

        if(data.flags >= VectorGL2D::Flag::MultiDraw)
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

    /*
        -   Circle lower left, green, upside down
        -   Square lower right, red, mirrored
        -   Triangle up center, green
    */
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "VectorTestFiles", data.expected2D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

void VectorGLTest::renderMulti3D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= VectorGL3D::Flag::ShaderStorageBuffers) {
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

    if(data.flags >= VectorGL3D::Flag::MultiDraw) {
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

    VectorGL3D shader{VectorGL3D::Configuration{}
        .setFlags(VectorGL3D::Flag::UniformBuffers|VectorGL3D::Flag::TextureTransformation|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.loadAndInstantiate("AnyImageImporter");
    CORRADE_VERIFY(importer);

    Containers::Optional<Trade::ImageData2D> image;
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(_testDir, "TestFiles/vector.tga")) && (image = importer->image2D(0)));

    /* For arrays we the original image three times to different offsets in
       three different slices */
    GL::Texture2D vector{NoCreate};
    GL::Texture2DArray vectorArray{NoCreate};
    if(data.flags & VectorGL2D::Flag::TextureArrays) {
        Vector3i size{image->size().x(), image->size().y()*2, 6};

        vectorArray = GL::Texture2DArray{};
        vectorArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, GL::TextureFormat::R8, size)
            /* Clear to all zeros for reproducible output */
            .setSubImage(0, {}, Image3D{PixelFormat::R8Unorm, size, Containers::Array<char>{ValueInit, std::size_t(size.product())}})
            .setSubImage(0, {0, size.y()/4, 1}, ImageView2D{*image})
            .setSubImage(0, {0, size.y()/2, 3}, ImageView2D{*image})
            .setSubImage(0, {0, 0, 5}, ImageView2D{*image});

        shader.bindVectorTexture(vectorArray);
    } else {
        vector = GL::Texture2D{};
        vector.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, GL::TextureFormat::R8, image->size())
            .setSubImage(0, {}, *image);

        shader.bindVectorTexture(vector);
    }

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);
    /* Plane is a strip, make it indexed first */
    Trade::MeshData planeData = MeshTools::generateIndices(Primitives::planeSolid(
        Primitives::PlaneFlag::TextureCoordinates));
    Trade::MeshData coneData = Primitives::coneSolid(1, 32, 1.0f,
        Primitives::ConeFlag::TextureCoordinates);

    /* Assuming the texture coordinates are the last attribute, add a four-byte
       padding after, which we subsequently abuse as the layer index */
    /** @todo clean this up once MeshData (and primitives?) support array
        coordinates directly */
    Trade::MeshData meshData = MeshTools::interleave(
        MeshTools::concatenate({sphereData, planeData, coneData}),
        {Trade::MeshAttributeData{4}});
    CORRADE_COMPARE(meshData.attributeCount(), 3);
    CORRADE_COMPARE(meshData.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(meshData.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(meshData.attributeName(2), Trade::MeshAttribute::TextureCoordinates);
    /* Manual cast because the real attribute type is Vector2 */
    const Containers::StridedArrayView1D<Vector3> textureCoordinates = Containers::arrayCast<Vector3>(meshData.mutableAttribute<Vector2>(Trade::MeshAttribute::TextureCoordinates));

    /* The sphere will use the last slice, coming from just the attribute
       alone */
    for(UnsignedInt i = 0; i != sphereData.vertexCount(); ++i)
        textureCoordinates[i].z() = 5;
    /* The plane will use the third slice, coming from both the attribute and
       the uniform */
    for(UnsignedInt i = 0; i != planeData.vertexCount(); ++i)
        textureCoordinates[sphereData.vertexCount() + i].z() = 1;
    /* The cone will use the first slice, coming from just the uniform. The
       memory isn't initialized by default however, so set the attribute to
       0. */
    for(UnsignedInt i = 0; i != coneData.vertexCount(); ++i)
        textureCoordinates[sphereData.vertexCount() + planeData.vertexCount() + i].z() = 0;

    /* Making some assumptions about the layout for simplicity */
    CORRADE_COMPARE(meshData.attributeStride(0), sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3));
    CORRADE_COMPARE(meshData.attributeStride(1), sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3));
    CORRADE_COMPARE(meshData.attributeStride(2), sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3));
    CORRADE_COMPARE(meshData.attributeOffset(0), 0);
    CORRADE_COMPARE(meshData.attributeOffset(1), sizeof(Vector3));
    CORRADE_COMPARE(meshData.attributeOffset(2), sizeof(Vector3) + sizeof(Vector3));
    GL::Mesh mesh;
    mesh.addVertexBuffer(GL::Buffer{meshData.vertexData()}, 0,
            GenericGL3D::Position{},
            GenericGL3D::Normal{},
            GenericGL3D::TextureArrayCoordinates{})
        .setIndexBuffer(GL::Buffer{GL::Buffer::TargetHint::ElementArray, meshData.indexData()}, 0,
            meshData.indexType())
        .setCount(meshData.indexCount());

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

    Containers::Array<VectorMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = VectorMaterialUniform{}
        .setColor(0xff0000_rgbf)
        .setBackgroundColor(0xffcccc_rgbf);
    materialData[1*data.uniformIncrement] = VectorMaterialUniform{}
        .setColor(0x00ff00_rgbf)
        .setBackgroundColor(0xccffcc_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationProjectionUniform3D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({-1.25f, -1.25f, 0.0f})*
            Matrix4::rotationY(180.0_degf) /* so the texture is visible */
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
            Matrix4::translation({  0.0f,  1.0f, 1.0f})*
            Matrix4::rotationY(180.0_degf) /* so the texture is visible */
        );
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    Containers::Array<TextureTransformationUniform> textureTransformationData{2*data.uniformIncrement + 1};
    textureTransformationData[0*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            /* Additional Y shift + scale in the array slice */
            (data.flags & VectorGL3D::Flag::TextureArrays ?
                Matrix3::translation(Vector2::yAxis(0.0f))*
                Matrix3::scaling(Vector2::yScale(0.5f)) : Matrix3{})*
            Matrix3::translation({0.5f, 0.5f})*
            Matrix3::rotation(180.0_degf)*
            Matrix3::translation({-0.5f, -0.5f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            /* Additional Y shift + scale in the array slice */
            (data.flags & VectorGL3D::Flag::TextureArrays ?
                Matrix3::translation(Vector2::yAxis(0.5f))*
                Matrix3::scaling(Vector2::yScale(0.5f)) : Matrix3{})*
            Matrix3::translation(Vector2::xAxis(1.0f))*
            Matrix3::scaling(Vector2::xScale(-1.0f)))
        .setLayer(2); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            /* Additional Y shift + scale in the array slice */
            (data.flags & VectorGL3D::Flag::TextureArrays ?
                Matrix3::translation(Vector2::yAxis(0.25f))*
                Matrix3::scaling(Vector2::yScale(0.5f)) : Matrix3{}))
        .setLayer(1); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<VectorDrawUniform> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are done with
       UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = VectorDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1);
    drawData[1*data.uniformIncrement] = VectorDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 0);
    drawData[2*data.uniformIncrement] = VectorDrawUniform{}
        .setMaterialId(data.bindWithOffset ? 0 : 1);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(VectorMaterialUniform),
            sizeof(VectorMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(VectorDrawUniform),
            sizeof(VectorDrawUniform));
        shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(sphere);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(VectorMaterialUniform),
            sizeof(VectorMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(VectorDrawUniform),
            sizeof(VectorDrawUniform));
        shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(plane);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(VectorMaterialUniform),
            sizeof(VectorMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(VectorDrawUniform),
            sizeof(VectorDrawUniform));
        shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(cone);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindTextureTransformationBuffer(textureTransformationUniform)
            .bindMaterialBuffer(materialUniform);

        if(data.flags >= VectorGL3D::Flag::MultiDraw)
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
        -   Sphere lower left, green, upside down
        -   Plane lower right, red, mirrored
        -   Cone up center, green
    */
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "VectorTestFiles", data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorGLTest)
