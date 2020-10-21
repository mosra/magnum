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

#include <numeric>
#include <sstream>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Combine.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/Icosphere.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct MeshVisualizerGLTest: GL::OpenGLTester {
    explicit MeshVisualizerGLTest();

    void construct2D();
    void construct3D();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void constructWireframeGeometryShader2D();
    void constructGeometryShader3D();
    #endif

    void construct2DInvalid();
    void construct3DInvalid();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void construct3DGeometryShaderDisabledButNeeded();
    void construct3DConflictingBitangentInput();
    #endif

    void constructMove2D();
    void constructMove3D();

    void setWireframeNotEnabled2D();
    void setWireframeNotEnabled3D();
    #ifndef MAGNUM_TARGET_GLES2
    void setColorMapNotEnabled2D();
    void setColorMapNotEnabled3D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void setTangentBitangentNormalNotEnabled3D();
    #endif

    void renderSetup();
    void renderTeardown();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void renderDefaultsWireframe2D();
    void renderDefaultsWireframe3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void renderDefaultsObjectId2D();
    void renderDefaultsObjectId3D();
    void renderDefaultsVertexId2D();
    void renderDefaultsVertexId3D();
    void renderDefaultsPrimitiveId2D();
    void renderDefaultsPrimitiveId3D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void renderDefaultsTangentBitangentNormal();
    #endif
    void renderWireframe2D();
    void renderWireframe3D();
    #ifndef MAGNUM_TARGET_GLES2
    void renderObjectVertexPrimitiveId2D();
    void renderObjectVertexPrimitiveId3D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void renderWireframe3DPerspective();
    void renderTangentBitangentNormal();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        std::string _testDir;

        GL::Renderbuffer _color{NoCreate},
            _depth{NoCreate};
        GL::Framebuffer _framebuffer{NoCreate};
        #ifndef MAGNUM_TARGET_GLES2
        GL::Texture2D _colorMapTexture;
        #endif
};

/*
    Rendering tests done on:

    -   Mesa Intel
    -   Mesa AMD
    .   Mesa llvmpipe
    -   SwiftShader ES2/ES3
    -   ARM Mali (Huawei P10) ES2/ES3 (except TBN visualization)
    -   WebGL 1 / 2 (on Mesa Intel) (except primitive/vertex/object ID)
    -   iPhone 6 w/ iOS 12.4 (except primitive/vertex/object ID)
*/

using namespace Math::Literals;

constexpr struct {
    const char* name;
    MeshVisualizer2D::Flags flags;
} ConstructData2D[] {
    {"wireframe w/o GS", MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", MeshVisualizer2D::Flag::InstancedObjectId},
    {"vertex ID", MeshVisualizer2D::Flag::VertexId},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizer2D::Flag::PrimitiveId},
    #endif
    {"primitive ID from vertex ID", MeshVisualizer2D::Flag::PrimitiveIdFromVertexId}
    #endif
};

constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
} ConstructData3D[] {
    {"wireframe w/o GS", MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", MeshVisualizer3D::Flag::InstancedObjectId},
    {"vertex ID", MeshVisualizer3D::Flag::VertexId},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizer3D::Flag::PrimitiveId},
    #endif
    {"primitive ID from vertex ID", MeshVisualizer3D::Flag::PrimitiveIdFromVertexId}
    #endif
};

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
} ConstructGeometryShaderData3D[] {
    {"wireframe", MeshVisualizer3D::Flag::Wireframe},
    {"tangent direction", MeshVisualizer3D::Flag::TangentDirection},
    {"bitangent direction from tangent", MeshVisualizer3D::Flag::BitangentFromTangentDirection},
    {"bitangent direction", MeshVisualizer3D::Flag::BitangentDirection},
    {"normal direction", MeshVisualizer3D::Flag::NormalDirection},
    {"tbn direction", MeshVisualizer3D::Flag::TangentDirection|MeshVisualizer3D::Flag::BitangentDirection|MeshVisualizer3D::Flag::NormalDirection},
    {"tbn direction with bitangent from tangent", MeshVisualizer3D::Flag::TangentDirection|MeshVisualizer3D::Flag::BitangentFromTangentDirection|MeshVisualizer3D::Flag::NormalDirection},
    {"wireframe + vertex id", MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::VertexId},
    {"wireframe + t/n direction", MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::TangentDirection|MeshVisualizer3D::Flag::NormalDirection},
    {"wireframe + object id + t/n direction", MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::InstancedObjectId|MeshVisualizer3D::Flag::TangentDirection|MeshVisualizer3D::Flag::NormalDirection},
    {"wireframe + vertex id + t/b direction", MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::VertexId|MeshVisualizer3D::Flag::TangentDirection|MeshVisualizer3D::Flag::BitangentDirection}
};
#endif

constexpr struct {
    const char* name;
    MeshVisualizer2D::Flags flags;
    const char* message;
} ConstructInvalidData2D[] {
    {"no feature enabled",
        MeshVisualizer2D::Flag::NoGeometryShader, /* not a feature flag */
        #ifndef MAGNUM_TARGET_GLES2
        "2D: at least one visualization feature has to be enabled"
        #else
        "2D: at least Flag::Wireframe has to be enabled"
        #endif
        },
    #ifndef MAGNUM_TARGET_GLES2
    {"both object and primitive id",
        MeshVisualizer2D::Flag::InstancedObjectId|MeshVisualizer2D::Flag::PrimitiveIdFromVertexId,
        ": Flag::InstancedObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"both object and vertex id",
        MeshVisualizer2D::Flag::InstancedObjectId|MeshVisualizer2D::Flag::VertexId,
        ": Flag::InstancedObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"}
    #endif
};

constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
    const char* message;
} ConstructInvalidData3D[] {
    {"no feature enabled",
        MeshVisualizer3D::Flag::NoGeometryShader, /* not a feature flag */
        #ifndef MAGNUM_TARGET_GLES2
        "3D: at least one visualization feature has to be enabled"
        #else
        "3D: at least Flag::Wireframe has to be enabled"
        #endif
        },
    #ifndef MAGNUM_TARGET_GLES2
    {"both object and primitive id",
        MeshVisualizer3D::Flag::InstancedObjectId|MeshVisualizer3D::Flag::PrimitiveIdFromVertexId,
        ": Flag::InstancedObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"both vertex and primitive id",
        MeshVisualizer3D::Flag::VertexId|MeshVisualizer3D::Flag::PrimitiveIdFromVertexId,
        ": Flag::InstancedObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    SamplerFilter filter;
    SamplerWrapping wrapping;
} ObjectIdDefaultsData[] {
    {"nearest, clamp", SamplerFilter::Nearest, SamplerWrapping::ClampToEdge},
    {"nearest, repeat", SamplerFilter::Nearest, SamplerWrapping::Repeat},
    {"linear, clamp", SamplerFilter::Linear, SamplerWrapping::ClampToEdge},
    {"linear, repeat", SamplerFilter::Linear, SamplerWrapping::Repeat}
};
#endif

constexpr struct {
    const char* name;
    MeshVisualizer2D::Flags flags;
    Float width, smoothness;
    const char* file;
    const char* fileXfail;
} WireframeData2D[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"", MeshVisualizer2D::Flags{},
        1.0f, 2.0f, "wireframe2D.tga", nullptr},
    {"wide/sharp", MeshVisualizer2D::Flags{},
        3.0f, 1.0f, "wireframe-wide2D.tga", nullptr},
    #endif
    {"no geometry shader", MeshVisualizer2D::Flag::NoGeometryShader,
        1.0f, 2.0f, "wireframe2D.tga", "wireframe-nogeo2D.tga"},
    {"no geometry shader, wide/sharp", MeshVisualizer2D::Flag::NoGeometryShader,
        3.0f, 1.0f, "wireframe-wide2D.tga", "wireframe-nogeo2D.tga"}
};

constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
    Float width, smoothness;
    const char* file;
    const char* fileXfail;
} WireframeData3D[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"", MeshVisualizer3D::Flags{},
        1.0f, 2.0f, "wireframe3D.tga", nullptr},
    {"wide/sharp", MeshVisualizer3D::Flags{},
        3.0f, 1.0f, "wireframe-wide3D.tga", nullptr},
    #endif
    {"no geometry shader",
        MeshVisualizer3D::Flag::NoGeometryShader,
        1.0f, 2.0f, "wireframe3D.tga", "wireframe-nogeo3D.tga"},
    {"no geometry shader, wide/sharp",
        MeshVisualizer3D::Flag::NoGeometryShader,
        3.0f, 1.0f, "wireframe-wide3D.tga", "wireframe-nogeo3D.tga"}
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    MeshVisualizer2D::Flags flags2D;
    MeshVisualizer3D::Flags flags3D;
    const char* file2D;
    const char* file3D;
} ObjectVertexPrimitiveIdData[] {
    {"object ID",
        MeshVisualizer2D::Flag::InstancedObjectId,
        MeshVisualizer3D::Flag::InstancedObjectId,
        "objectid2D.tga", "objectid3D.tga"},
    {"vertex ID",
        MeshVisualizer2D::Flag::VertexId,
        MeshVisualizer3D::Flag::VertexId,
        "vertexid2D.tga", "vertexid3D.tga"},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID",
        MeshVisualizer2D::Flag::PrimitiveId,
        MeshVisualizer3D::Flag::PrimitiveId,
        "primitiveid2D.tga", "primitiveid3D.tga"},
    #endif
    {"primitive ID from vertex ID",
        MeshVisualizer2D::Flag::PrimitiveIdFromVertexId,
        MeshVisualizer3D::Flag::PrimitiveIdFromVertexId,
        "primitiveid2D.tga", "primitiveid3D.tga"},
    {"wireframe + object ID",
        MeshVisualizer2D::Flag::InstancedObjectId|MeshVisualizer2D::Flag::Wireframe,
        MeshVisualizer3D::Flag::InstancedObjectId|MeshVisualizer3D::Flag::Wireframe,
        "wireframe-objectid2D.tga", "wireframe-objectid3D.tga"},
    {"wireframe + object ID, no geometry shader",
        MeshVisualizer2D::Flag::InstancedObjectId|MeshVisualizer2D::Flag::Wireframe|
        MeshVisualizer2D::Flag::NoGeometryShader,
        MeshVisualizer3D::Flag::InstancedObjectId|MeshVisualizer3D::Flag::Wireframe|
        MeshVisualizer3D::Flag::NoGeometryShader,
        "wireframe-nogeo-objectid2D.tga", "wireframe-nogeo-objectid3D.tga"},
    {"wireframe + vertex ID",
        MeshVisualizer2D::Flag::VertexId|MeshVisualizer2D::Flag::Wireframe,
        MeshVisualizer3D::Flag::VertexId|MeshVisualizer3D::Flag::Wireframe,
        "wireframe-vertexid2D.tga", "wireframe-vertexid3D.tga"}
};
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
    MeshVisualizer3D::Flags secondPassFlags;
    bool skipBitagnentEvenIfEnabledInFlags;
    Float smoothness;
    Float lineWidth;
    Float lineLength;
    Float multiply;
    const char* file;
} TangentBitangentNormalData[] {
    {"",
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::BitangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "tbn.tga"},
    {"bitangents from tangents",
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::BitangentFromTangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "tbn.tga"},
    {"scaled data",
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::BitangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 5.0f, "tbn.tga"},
    {"wide blurry lines",
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::BitangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        false, 5.0f, 5.0f, 0.8f, 1.0f, "tbn-wide.tga"},
    {"only bitangent from tangent",
        MeshVisualizer3D::Flag::BitangentFromTangentDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "bitangents-from-tangents.tga"},
    {"wireframe + primitive ID + tangents + normals, single pass",
        MeshVisualizer3D::Flag::Wireframe|
        MeshVisualizer3D::Flag::PrimitiveId|
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "wireframe-primitiveid-tn.tga"},
    {"wireframe + primitive ID, rendering all, but only tangents + normals present",
        MeshVisualizer3D::Flag::Wireframe|
        MeshVisualizer3D::Flag::PrimitiveId|
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::BitangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        true, 2.0f, 1.0f, 0.6f, 1.0f, "wireframe-primitiveid-tn.tga"},
    {"wireframe + tangents + normals, two passes",
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::NormalDirection,
        MeshVisualizer3D::Flag::Wireframe,
        false, 2.0f, 1.0f, 0.6f, 1.0f, "wireframe-tn-smooth.tga"},
    {"primitive ID + tangents + normals",
        MeshVisualizer3D::Flag::PrimitiveId|
        MeshVisualizer3D::Flag::TangentDirection|
        MeshVisualizer3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "primitiveid-tn.tga"}
};
#endif

MeshVisualizerGLTest::MeshVisualizerGLTest() {
    addInstancedTests({&MeshVisualizerGLTest::construct2D},
        Containers::arraySize(ConstructData2D));

    addInstancedTests({&MeshVisualizerGLTest::construct3D},
        Containers::arraySize(ConstructData3D));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    addTests({&MeshVisualizerGLTest::constructWireframeGeometryShader2D});

    addInstancedTests({&MeshVisualizerGLTest::constructGeometryShader3D},
        Containers::arraySize(ConstructGeometryShaderData3D));
    #endif

    addInstancedTests({&MeshVisualizerGLTest::construct2DInvalid},
        Containers::arraySize(ConstructInvalidData2D));
    addInstancedTests({&MeshVisualizerGLTest::construct3DInvalid},
        Containers::arraySize(ConstructInvalidData3D));

    addTests({
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshVisualizerGLTest::construct3DGeometryShaderDisabledButNeeded,
              &MeshVisualizerGLTest::construct3DConflictingBitangentInput,
              #endif

              &MeshVisualizerGLTest::constructMove2D,
              &MeshVisualizerGLTest::constructMove3D,

              &MeshVisualizerGLTest::setWireframeNotEnabled2D,
              &MeshVisualizerGLTest::setWireframeNotEnabled3D,
              #ifndef MAGNUM_TARGET_GLES2
              &MeshVisualizerGLTest::setColorMapNotEnabled2D,
              &MeshVisualizerGLTest::setColorMapNotEnabled3D,
              #endif
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshVisualizerGLTest::setTangentBitangentNormalNotEnabled3D,
              #endif
              });

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    addTests({&MeshVisualizerGLTest::renderDefaultsWireframe2D,
              &MeshVisualizerGLTest::renderDefaultsWireframe3D},
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::renderDefaultsObjectId2D,
                       &MeshVisualizerGLTest::renderDefaultsObjectId3D},
        Containers::arraySize(ObjectIdDefaultsData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addTests({
        &MeshVisualizerGLTest::renderDefaultsVertexId2D,
        &MeshVisualizerGLTest::renderDefaultsVertexId3D,
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId2D,
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId3D,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsTangentBitangentNormal
        #endif
        },
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    addInstancedTests({&MeshVisualizerGLTest::renderWireframe2D},
        Containers::arraySize(WireframeData2D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    addInstancedTests({&MeshVisualizerGLTest::renderWireframe3D},
        Containers::arraySize(WireframeData3D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::renderObjectVertexPrimitiveId2D,
                       &MeshVisualizerGLTest::renderObjectVertexPrimitiveId3D},
        Containers::arraySize(ObjectVertexPrimitiveIdData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    addTests({&MeshVisualizerGLTest::renderWireframe3DPerspective},
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    addInstancedTests({&MeshVisualizerGLTest::renderTangentBitangentNormal},
        Containers::arraySize(TangentBitangentNormalData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
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

    /* Set up a color map texture for use by object / primitive ID tests */
    #ifndef MAGNUM_TARGET_GLES2
    {
        const auto map = DebugTools::ColorMap::turbo();
        const Vector2i size{Int(map.size()), 1};
        _colorMapTexture
            .setMinificationFilter(SamplerFilter::Linear)
            .setMagnificationFilter(SamplerFilter::Linear)
            .setWrapping(SamplerWrapping::Repeat)
            .setStorage(1, GL::TextureFormat::RGB8, size)
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});
    }
    #endif
}

void MeshVisualizerGLTest::construct2D() {
    auto&& data = ConstructData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizer2D::Flag::InstancedObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags & MeshVisualizer2D::Flag::PrimitiveId && !(data.flags >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");
    #endif

    MeshVisualizer2D shader{data.flags};
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

void MeshVisualizerGLTest::construct3D() {
    auto&& data = ConstructData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizer3D::Flag::InstancedObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags >= MeshVisualizer3D::Flag::PrimitiveIdFromVertexId &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags & MeshVisualizer3D::Flag::PrimitiveId && !(data.flags >= MeshVisualizer3D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");
    #endif

    MeshVisualizer3D shader{data.flags};
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::constructWireframeGeometryShader2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    MeshVisualizer2D shader{MeshVisualizer2D::Flag::Wireframe};
    CORRADE_COMPARE(shader.flags(), MeshVisualizer2D::Flag::Wireframe);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void MeshVisualizerGLTest::constructGeometryShader3D() {
    auto&& data = ConstructGeometryShaderData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    MeshVisualizer3D shader{data.flags};
    CORRADE_COMPARE(shader.flags(), data.flags);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}
#endif

void MeshVisualizerGLTest::construct2DInvalid() {
    auto&& data = ConstructInvalidData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshVisualizer2D{data.flags};
    CORRADE_COMPARE(out.str(), Utility::formatString("Shaders::MeshVisualizer{}\n", data.message));
}

void MeshVisualizerGLTest::construct3DInvalid() {
    auto&& data = ConstructInvalidData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshVisualizer3D{data.flags};
    CORRADE_COMPARE(out.str(), Utility::formatString("Shaders::MeshVisualizer{}\n", data.message));
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::construct3DGeometryShaderDisabledButNeeded() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshVisualizer3D{MeshVisualizer3D::Flag::NoGeometryShader|MeshVisualizer3D::Flag::NormalDirection};
    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer3D: geometry shader has to be enabled when rendering TBN direction\n");
}

void MeshVisualizerGLTest::construct3DConflictingBitangentInput() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshVisualizer3D{MeshVisualizer3D::Flag::BitangentFromTangentDirection|MeshVisualizer3D::Flag::BitangentDirection};
    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer3D: Flag::BitangentDirection and Flag::BitangentFromTangentDirection are mutually exclusive\n");
}
#endif

void MeshVisualizerGLTest::constructMove2D() {
    MeshVisualizer2D a{MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizer2D b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizer2D c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

void MeshVisualizerGLTest::constructMove3D() {
    MeshVisualizer3D a{MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizer3D b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizer3D c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

void MeshVisualizerGLTest::setWireframeNotEnabled2D() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* The constructor asserts for at least some feature being enabled (which
       is just wireframe in case of 2D), so fake it with a NoCreate */
    MeshVisualizer2D shader{NoCreate};
    shader
        .setColor({});

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setColor(): the shader was not created with wireframe or object/vertex/primitive ID enabled\n");
    #else
    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setColor(): the shader was not created with wireframe enabled\n");
    #endif

    out.str({});
    shader
        .setWireframeColor({})
        .setWireframeWidth({})
        .setSmoothness({});

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setWireframeColor(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer::setWireframeWidth(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer2D::setSmoothness(): the shader was not created with wireframe enabled\n");
}

void MeshVisualizerGLTest::setWireframeNotEnabled3D() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* The constructor asserts for at least some feature being enabled (which
       is just wireframe in case we're not on desktop or ES3.2), so fake it
       with a NoCreate */
    MeshVisualizer3D shader{NoCreate};
    shader
        .setColor({});

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setColor(): the shader was not created with wireframe or object/vertex/primitive ID enabled\n");
    #else
    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setColor(): the shader was not created with wireframe enabled\n");
    #endif

    out.str({});
    shader
        .setWireframeColor({})
        .setWireframeWidth({})
        .setSmoothness({});

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setWireframeColor(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer::setWireframeWidth(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer3D::setSmoothness(): the shader was not created with wireframe or TBN direction enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setColorMapNotEnabled2D() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    MeshVisualizer2D shader{NoCreate};
    shader.setColorMapTransformation({}, {})
        .bindColorMapTexture(texture);

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled\n"
        "Shaders::MeshVisualizer::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled\n");
}

void MeshVisualizerGLTest::setColorMapNotEnabled3D() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    MeshVisualizer3D shader{NoCreate};
    shader.setColorMapTransformation({}, {})
        .bindColorMapTexture(texture);

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled\n"
        "Shaders::MeshVisualizer::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled\n");
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::setTangentBitangentNormalNotEnabled3D() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    MeshVisualizer3D shader{MeshVisualizer3D::Flag::Wireframe};
    shader.setNormalMatrix({})
        .setLineWidth({})
        .setLineLength({});

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer3D::setNormalMatrix(): the shader was not created with TBN direction enabled\n"
        "Shaders::MeshVisualizer3D::setLineWidth(): the shader was not created with TBN direction enabled\n"
        "Shaders::MeshVisualizer3D::setLineLength(): the shader was not created with TBN direction enabled\n");
}
#endif

constexpr Vector2i RenderSize{80, 80};

void MeshVisualizerGLTest::renderSetup() {
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
    _depth = GL::Renderbuffer{};
    _depth.setStorage(GL::RenderbufferFormat::DepthComponent16, RenderSize);
    _framebuffer = GL::Framebuffer{{{}, RenderSize}};
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
        .clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth)
        .bind();

    /* Disable depth test & blending by default, particular tests enable it if
       needed */
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);
}

void MeshVisualizerGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::renderDefaultsWireframe2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(16));

    MeshVisualizer2D shader{MeshVisualizer2D::Flag::Wireframe};
    shader.draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL("Defaults don't work for wireframe as line width is derived from viewport size.");
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /** @todo make this unnecessary */
    shader
        .setViewportSize({80, 80})
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe2D.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.082f}));
}

void MeshVisualizerGLTest::renderDefaultsWireframe3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::icosphereSolid(1));

    MeshVisualizer3D shader{MeshVisualizer3D::Flag::Wireframe};
    shader.draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL("Defaults don't work for wireframe as line width is derived from viewport size.");
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe3D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /** @todo make this unnecessary */
    shader
        .setViewportSize({80, 80})
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe3D.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.06f}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::renderDefaultsObjectId2D() {
    auto&& data = ObjectIdDefaultsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    /* Configure a texture with preset filtering and wrapping. The goal here is
       that the default config should be filtering/wrapping-independent for the
       first 256 items */
    const auto map = DebugTools::ColorMap::turbo();
    const Vector2i size{Int(map.size()), 1};
    GL::Texture2D colorMapTexture;
    colorMapTexture
        .setMinificationFilter(data.filter)
        .setMagnificationFilter(data.filter)
        .setWrapping(data.wrapping)
        .setStorage(1, GL::TextureFormat::RGB8, size)
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});

    /* Generate per-face IDs going from 0 to 240 to cover the whole range */
    Containers::Array<UnsignedInt> ids{16};
    for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i*16;
    GL::Mesh circle = MeshTools::compile(MeshTools::combineFaceAttributes(
        MeshTools::generateIndices(Primitives::circle2DSolid(16)), {
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                Containers::arrayView(ids)}
        }));

    MeshVisualizer2D{MeshVisualizer2D::Flag::InstancedObjectId}
        .bindColorMapTexture(colorMapTexture)
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-objectid2D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 150.67f, 0.45f}));
}

void MeshVisualizerGLTest::renderDefaultsObjectId3D() {
    auto&& data = ObjectIdDefaultsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    /* Configure a texture with preset filtering and wrapping. The goal here is
       that the default config should be filtering/wrapping-independent for the
       first 256 items */
    const auto map = DebugTools::ColorMap::turbo();
    const Vector2i size{Int(map.size()), 1};
    GL::Texture2D colorMapTexture;
    colorMapTexture
        .setMinificationFilter(data.filter)
        .setMagnificationFilter(data.filter)
        .setWrapping(data.wrapping)
        .setStorage(1, GL::TextureFormat::RGB8, size)
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});

    /* Generate per-face IDs going from 0 to 228 to cover the whole range */
    Containers::Array<UnsignedInt> ids{20};
    for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i*12;
    GL::Mesh icosphere = MeshTools::compile(MeshTools::combineFaceAttributes(
        Primitives::icosphereSolid(0), {
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                Containers::arrayView(ids)}
        }));

    MeshVisualizer3D{MeshVisualizer3D::Flag::InstancedObjectId}
        .bindColorMapTexture(colorMapTexture)
        .draw(icosphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-objectid3D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 150.67f, 0.165f}));
}

void MeshVisualizerGLTest::renderDefaultsVertexId2D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* On SwiftShader gl_VertexID doesn't work in this case, skipping */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    MeshVisualizer2D{MeshVisualizer2D::Flag::VertexId}
        .bindColorMapTexture(_colorMapTexture)
        .draw(MeshTools::compile(Primitives::circle2DSolid(16)));

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-vertexid2D.tga"),
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.017f}));
}

void MeshVisualizerGLTest::renderDefaultsVertexId3D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* On SwiftShader gl_VertexID doesn't work in this case, skipping */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    MeshVisualizer2D{MeshVisualizer2D::Flag::VertexId}
        .bindColorMapTexture(_colorMapTexture)
        .draw(MeshTools::compile(Primitives::icosphereSolid(0)));

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-vertexid3D.tga"),
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.012f}));
}
void MeshVisualizerGLTest::renderDefaultsPrimitiveId2D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Interestingly in this case gl_VertexID in SwiftShader works (thus not
       checking for MAGNUM_shader_vertex_id -- maybe it works only for
       nonindexed triangle draws? */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    MeshVisualizer2D::Flags flags;
    #ifdef MAGNUM_TARGET_WEBGL
    flags = MeshVisualizer2D::Flag::PrimitiveIdFromVertexId;
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isVersionSupported(GL::Version::GL320))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES320))
    #endif
    {
        Debug{} << "Using primitive ID from vertex ID";
        flags = MeshVisualizer2D::Flag::PrimitiveIdFromVertexId;
    }
    else flags = MeshVisualizer2D::Flag::PrimitiveId;
    #endif

    Trade::MeshData circleData = Primitives::circle2DSolid(16);
    if(flags >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId)
        circleData = MeshTools::duplicate(MeshTools::generateIndices(circleData));

    MeshVisualizer2D{flags}
        .bindColorMapTexture(_colorMapTexture)
        .draw(MeshTools::compile(circleData));

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-primitiveid2D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 76.67f, 0.23f}));
}

void MeshVisualizerGLTest::renderDefaultsPrimitiveId3D() {
    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Interestingly in this case gl_VertexID in SwiftShader works (thus not
       checking for MAGNUM_shader_vertex_id -- maybe it works only for
       nonindexed triangle draws? */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    MeshVisualizer2D::Flags flags;
    #ifdef MAGNUM_TARGET_WEBGL
    flags = MeshVisualizer2D::Flag::PrimitiveIdFromVertexId;
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isVersionSupported(GL::Version::GL320))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES320))
    #endif
    {
        Debug{} << "Using primitive ID from vertex ID";
        flags = MeshVisualizer2D::Flag::PrimitiveIdFromVertexId;
    }
    else flags = MeshVisualizer2D::Flag::PrimitiveId;
    #endif

    Trade::MeshData icosphereData = Primitives::icosphereSolid(0);
    if(flags >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId)
        icosphereData = MeshTools::duplicate(icosphereData);

    MeshVisualizer2D{flags}
        .bindColorMapTexture(_colorMapTexture)
        .draw(MeshTools::compile(icosphereData));

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-primitiveid3D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 88.34f, 0.071f}));
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::renderDefaultsTangentBitangentNormal() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(4, 8,
        Primitives::UVSphereFlag::Tangents));

    MeshVisualizer3D{MeshVisualizer3D::Flag::TangentDirection|
            MeshVisualizer3D::Flag::BitangentFromTangentDirection|
            MeshVisualizer3D::Flag::NormalDirection}
        .setViewportSize({80, 80}) /** @todo make this unnecessary */
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-tbn.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.06f}));
}
#endif

void MeshVisualizerGLTest::renderWireframe2D() {
    auto&& data = WireframeData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif
    #endif

    const Trade::MeshData circleData = Primitives::circle2DSolid(16);

    GL::Mesh circle{NoCreate};
    if(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) {
        /* Duplicate the vertices. The circle primitive is  */
        const Trade::MeshData circleDataIndexed =
            MeshTools::generateIndices(circleData);
        circle = MeshTools::compile(MeshTools::duplicate(circleDataIndexed));

        /* Supply also the vertex ID, if needed */
        #ifndef MAGNUM_TARGET_GLES2
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        #endif
        {
            Containers::Array<Float> vertexIndex{circleDataIndexed.indexCount()};
            std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

            GL::Buffer vertexId;
            vertexId.setData(vertexIndex);
            circle.addVertexBuffer(std::move(vertexId), 0, MeshVisualizer2D::VertexIndex{});
        }
    } else circle = MeshTools::compile(circleData);

    MeshVisualizer2D{data.flags|MeshVisualizer2D::Flag::Wireframe}
        .setColor(0xffff99_rgbf)
        .setWireframeColor(0x9999ff_rgbf)
        .setWireframeWidth(data.width)
        .setSmoothness(data.smoothness)
        .setViewportSize({80, 80})
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL_IF(data.flags & MeshVisualizer2D::Flag::NoGeometryShader,
            "Line width is currently not configurable w/o geometry shader.");
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels */
        const Float maxThreshold = 170.0f, meanThreshold = 0.327f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test it's not *too* off, at least */
    if(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) {
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels. Apple
           A8 on more. */
        const Float maxThreshold = 170.0f, meanThreshold = 0.330f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 2.077f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.fileXfail}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }
}

void MeshVisualizerGLTest::renderWireframe3D() {
    auto&& data = WireframeData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif
    #endif

    const Trade::MeshData sphereData = Primitives::icosphereSolid(1);

    GL::Mesh sphere{NoCreate};
    if(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) {
        /* Duplicate the vertices */
        sphere = MeshTools::compile(MeshTools::duplicate(sphereData));

        /* Supply also the vertex ID, if needed */
        #ifndef MAGNUM_TARGET_GLES2
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        #endif
        {
            Containers::Array<Float> vertexIndex{sphereData.indexCount()};
            std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

            GL::Buffer vertexId;
            vertexId.setData(vertexIndex);
            sphere.addVertexBuffer(std::move(vertexId), 0, MeshVisualizer3D::VertexIndex{});
        }
    } else sphere = MeshTools::compile(sphereData);

    MeshVisualizer3D{data.flags|MeshVisualizer3D::Flag::Wireframe}
        .setColor(0xffff99_rgbf)
        .setWireframeColor(0x9999ff_rgbf)
        .setWireframeWidth(data.width)
        .setSmoothness(data.smoothness)
        .setViewportSize({80, 80})
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL_IF(data.flags & MeshVisualizer3D::Flag::NoGeometryShader,
            "Line width is currently not configurable w/o geometry shader.");
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels. On a
           GS, if GL_NV_shader_noperspective_interpolation is not supported,
           the artifacts are bigger. */
        Float maxThreshold = 170.0f, meanThreshold = 0.327f;
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(!(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
            meanThreshold = 2.166f;
        #endif
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test it's not *too* off, at least */
    if(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) {
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels. Apple
           A8 on more. */
        const Float maxThreshold = 170.0f, meanThreshold = 0.330f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.fileXfail}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::renderObjectVertexPrimitiveId2D() {
    auto&& data = ObjectVertexPrimitiveIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags2D & MeshVisualizer2D::Flag::InstancedObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    /* Interestingly for PrimitiveIdFromVertexId gl_VertexID in SwiftShader
       works -- maybe it works only for nonindexed triangle draws? */
    if(data.flags2D & MeshVisualizer2D::Flag::VertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags2D & MeshVisualizer2D::Flag::PrimitiveId && !(data.flags2D >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags2D & MeshVisualizer2D::Flag::Wireframe && !(data.flags2D & MeshVisualizer2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(data.flags2D & MeshVisualizer2D::Flag::Wireframe && !(data.flags2D & MeshVisualizer2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif
    #endif

    Trade::MeshData circleData = Primitives::circle2DSolid(16);

    if(data.flags2D & MeshVisualizer2D::Flag::InstancedObjectId) {
        Containers::Array<UnsignedInt> ids{16};
        /* Each two faces share the same ID */
        for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i/2;
        circleData = MeshTools::combineFaceAttributes(
            MeshTools::generateIndices(circleData), {
                Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                    Containers::arrayView(ids)}
            });
    }

    /* Duplicate the data if using primitive ID from vertex ID or if geometry
       shader is disabled */
    if(data.flags2D >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId)
        circleData = MeshTools::generateIndices(circleData);
    if(data.flags2D >= MeshVisualizer2D::Flag::PrimitiveIdFromVertexId ||
       data.flags2D & MeshVisualizer2D::Flag::NoGeometryShader) {
        if(circleData.primitive() != MeshPrimitive::Triangles)
            circleData = MeshTools::generateIndices(circleData);
        circleData = MeshTools::duplicate(circleData);
    }

    GL::Mesh circle = MeshTools::compile(circleData);

    MeshVisualizer2D shader{data.flags2D};
    shader
        /* Remove blue so it's clear the (wireframe) background and mapped ID
           colors got mixed */
        .setColor(0xffff00_rgbf)
        /* Shouldn't assert (nor warn) when wireframe is not enabled */
        .setViewportSize({80, 80})
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .bindColorMapTexture(_colorMapTexture);

    /* OTOH the wireframe color should stay at full channels, not mixed */
    if(data.flags3D & MeshVisualizer3D::Flag::Wireframe)
        shader.setWireframeColor(0xffffff_rgbf);

    /* For vertex ID we don't want any repeat/wraparound as that causes
       disruptions in the gradient and test failures. There's 17 vertices
       also. */
    if(data.flags2D & MeshVisualizer2D::Flag::VertexId)
        shader.setColorMapTransformation(1.0f, -1.0f/17.0f);
    /* For object/primitive ID there's no gradient so a wraparound is okay.
       This should cover the first half of the colormap, in reverse order; for
       primitive ID the whole colormap due to the repeat wrapping */
    else
        shader.setColorMapTransformation(0.5f, -1.0f/16.0f);

    shader.draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file2D}),
        /* AMD has slight off-by-one errors compared to Intel, SwiftShader a
           bit more */
        (DebugTools::CompareImageToFile{_manager, 4.0f, 0.141f}));
}

void MeshVisualizerGLTest::renderObjectVertexPrimitiveId3D() {
    auto&& data = ObjectVertexPrimitiveIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags3D & MeshVisualizer3D::Flag::InstancedObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() + std::string(" is not supported"));
    #endif

    /* Interestingly for PrimitiveIdFromVertexId gl_VertexID in SwiftShader
       works -- maybe it works only for nonindexed triangle draws? */
    if(data.flags3D & MeshVisualizer3D::Flag::VertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags3D & MeshVisualizer3D::Flag::PrimitiveId && !(data.flags3D >= MeshVisualizer3D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags3D & MeshVisualizer3D::Flag::Wireframe && !(data.flags3D & MeshVisualizer3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(data.flags3D & MeshVisualizer3D::Flag::Wireframe && !(data.flags3D & MeshVisualizer3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif
    #endif

    Trade::MeshData icosphereData = Primitives::icosphereSolid(1);

    if(data.flags3D & MeshVisualizer3D::Flag::InstancedObjectId) {
        Containers::Array<UnsignedInt> ids{80};
        /* Each four faces share the same ID */
        for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i/4;
        icosphereData = MeshTools::combineFaceAttributes(
            icosphereData, {
                Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                    Containers::arrayView(ids)}
            });
    }

    /* Duplicate the data if using primitive ID from vertex ID or if geometry
       shader is disabled */
    if(data.flags3D >= MeshVisualizer3D::Flag::PrimitiveIdFromVertexId ||
       data.flags3D & MeshVisualizer3D::Flag::NoGeometryShader)
        icosphereData = MeshTools::duplicate(icosphereData);

    GL::Mesh circle = MeshTools::compile(icosphereData);

    MeshVisualizer3D shader{data.flags3D};
    shader
        /* Remove blue so it's clear the wireframe background and mapped ID
           colors got mixed */
        .setColor(0xffff00_rgbf)
        /* Shouldn't assert (nor warn) when wireframe is not enabled */
        .setViewportSize({80, 80})
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .bindColorMapTexture(_colorMapTexture);

    /* OTOH the wireframe color should stay at full channels, not mixed */
    if(data.flags2D & MeshVisualizer2D::Flag::Wireframe)
        shader.setWireframeColor(0xffffff_rgbf);

    /* For vertex ID we don't want any repeat/wraparound as that causes
       disruptions in the gradient and test failures. There's 42 vertices also. */
    if(data.flags2D & MeshVisualizer2D::Flag::VertexId)
        shader.setColorMapTransformation(1.0f, -1.0f/42.0f);
    /* For object/primitive ID there's no gradient so a wraparound is okay.
       This should cover the first half of the colormap, in reverse order; for
       primitive ID the whole colormap due to the repeat wrapping */
    else
        shader.setColorMapTransformation(0.5f, -1.0f/40.0f);

    shader.draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Release build has 1 pixel slightly off. Huh. AMD & llvmpipe has
       additional off-by-one errors compared to Intel, SwiftShader some errors
       on the edges. If GL_NV_shader_noperspective_interpolation is not
       supported, the artifacts are bigger when wireframe is enabled. */
    Float maxThreshold = 138.4f, meanThreshold = 0.279f;
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags3D & MeshVisualizer3D::Flag::Wireframe && !GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>()) {
        /* SwiftShader has a bit more rounding errors */
        maxThreshold = 238.0f;
        meanThreshold = 1.957f;
    }
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file3D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::renderWireframe3DPerspective() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    MeshVisualizer3D{MeshVisualizer3D::Flag::Wireframe}
        .setWireframeWidth(8.0f)
        .setWireframeColor(0xff0000_rgbf)
        .setViewportSize({80, 80})
        .setTransformationMatrix(
            Matrix4::translation({0.0f, 0.5f, -3.5f})*
            Matrix4::rotationX(-60.0_degf)*
            Matrix4::scaling(Vector3::yScale(2.0f)))
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .draw(plane);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifdef MAGNUM_TARGET_GLES
    CORRADE_EXPECT_FAIL_IF(!GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>(),
        "GL_NV_shader_noperspective_interpolation not available.");
    #endif

    /* Slight rasterization differences on AMD. */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/wireframe-perspective.tga"),
        (DebugTools::CompareImageToFile{_manager, 0.667f, 0.002f}));
}

void MeshVisualizerGLTest::renderTangentBitangentNormal() {
    auto&& data = TangentBitangentNormalData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    /* Creating a primitive from scratch because Primitives::planeSolid() is
       too regular to test everything properly */
    struct Vertex {
        Vector3 position;
        Vector4 tangent;
        Vector3 bitangent;
        Vector3 normal;
    } vertexData[] {
        {{ 1.0f, -1.0f, 0.0f},
         Vector4{Vector3{1.25f, 0.0f, 0.25f}.normalized(), -1.0f}, {},
         Vector3{0.25f, 0.0f, -1.25f}.normalized()},
        {{ 1.0f,  1.0f, 0.0f},
        Vector4{Vector3{-1.0f, 0.25f, 0.0f}.normalized(), -1.0f}, {},
         Vector3{-0.25f, -1.0f, 0.0f}.normalized()},
        {{-1.0f, -1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, -1.0f}, {},
         {0.0f, 0.0f, 1.0f}},
        {{-1.0f,  1.0f, 0.0f},
         Vector4{Vector3{0.75f, 0.0f, -0.25f}.normalized(), 1.0f}, {},
         Vector3{0.25f, 0.0f, 0.75f}.normalized()}
    };

    /* Calculate bitangents from normal+tangent */
    for(Vertex& i: vertexData)
        i.bitangent = Math::cross(i.normal, i.tangent.xyz())*i.tangent.w();

    /* Verify the TBN is orthogonal */
    for(Vertex& i: vertexData) {
        CORRADE_ITERATION(i.position);
        CORRADE_VERIFY(i.tangent.xyz().isNormalized());
        CORRADE_VERIFY(i.bitangent.isNormalized());
        CORRADE_VERIFY(i.normal.isNormalized());
        CORRADE_COMPARE(dot(i.normal, i.tangent.xyz()), 0.0f);
        CORRADE_COMPARE(dot(i.normal, i.bitangent), 0.0f);
        CORRADE_COMPARE(dot(i.tangent.xyz(), i.bitangent), 0.0f);
    }

    /* Apply scale to all */
    for(Vertex& i: vertexData) {
        i.tangent *= data.multiply;
        i.bitangent *= data.multiply;
        i.normal *= data.multiply;
    }

    GL::Buffer vertices{vertexData};
    GL::Mesh mesh{MeshPrimitive::TriangleStrip};
    mesh.setCount(4)
        .addVertexBuffer(vertices, 0,
            Shaders::MeshVisualizer3D::Position{},
            sizeof(Vector4), /* conditionally added below */
            sizeof(Vector3), /* conditionally added below */
            Shaders::MeshVisualizer3D::Normal{});
    if(data.flags & MeshVisualizer3D::Flag::BitangentFromTangentDirection && !data.skipBitagnentEvenIfEnabledInFlags)
        mesh.addVertexBuffer(vertices, 0,
            sizeof(Vector3),
            Shaders::MeshVisualizer3D::Tangent4{},
            sizeof(Vector3),
            sizeof(Vector3));
    else if(data.flags & MeshVisualizer3D::Flag::TangentDirection)
        mesh.addVertexBuffer(vertices, 0,
            sizeof(Vector3),
            Shaders::MeshVisualizer3D::Tangent{}, sizeof(Float), sizeof(Vector3),
            sizeof(Vector3));
    if(data.flags & MeshVisualizer3D::Flag::BitangentDirection && !data.skipBitagnentEvenIfEnabledInFlags)
        mesh.addVertexBuffer(vertices, 0,
            sizeof(Vector3),
            sizeof(Vector4),
            Shaders::MeshVisualizer3D::Bitangent{},
            sizeof(Vector3));

    Matrix4 transformation = Matrix4::translation({0.0f, 0.5f, -3.5f})*
        Matrix4::rotationX(-60.0_degf)*
        Matrix4::scaling(Vector3::yScale(1.5f));

    if(data.secondPassFlags) {
        MeshVisualizer3D{data.secondPassFlags}
            /** @todo make this unnecessary */
            .setViewportSize({80, 80})
            .setTransformationMatrix(transformation)
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .draw(mesh);
    }

    MeshVisualizer3D shader{data.flags};
    shader
        /** @todo make this unnecessary */
        .setViewportSize({80, 80})
        .setTransformationMatrix(transformation)
        .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        .setNormalMatrix(transformation.normalMatrix()*data.multiply)
        .setSmoothness(data.smoothness)
        .setLineLength(data.lineLength)
        .setLineWidth(data.lineWidth);

    if(data.flags & MeshVisualizer3D::Flag::Wireframe) shader
        .setColor(0xffff99_rgbf)
        .setWireframeColor(0x9999ff_rgbf);
    if(data.flags & MeshVisualizer3D::Flag::PrimitiveId) shader
        .bindColorMapTexture(_colorMapTexture)
        .setColorMapTransformation(1.0f/512.0f, 0.5f);

    shader.draw(mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Slight rasterization differences on AMD. If
       GL_NV_shader_noperspective_interpolation is not supported, the artifacts
       are bigger. */
    Float maxThreshold = 1.334f, meanThreshold = 0.018f;
    #ifdef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>()) {
        maxThreshold = 39.0f;
        meanThreshold = 1.207f;
    }
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGLTest)
