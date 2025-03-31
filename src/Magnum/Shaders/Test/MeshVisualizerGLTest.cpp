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

#include <numeric> /* std::iota() */
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/System.h>

#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/MeshTools/Combine.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/Icosphere.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/MeshVisualizerGL.h"
#include "Magnum/Trade/AbstractImporter.h"
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
#include "Magnum/Shaders/MeshVisualizer.h"
#endif

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct MeshVisualizerGLTest: GL::OpenGLTester {
    explicit MeshVisualizerGLTest();

    void construct2D();
    void construct3D();
    #ifndef MAGNUM_TARGET_GLES2
    void constructSkinning2D();
    void constructSkinning3D();
    #endif
    void constructAsync2D();
    void constructAsync3D();
    #ifndef MAGNUM_TARGET_GLES2
    void constructUniformBuffers2D();
    void constructUniformBuffers3D();
    void constructUniformBuffersAsync2D();
    void constructUniformBuffersAsync3D();
    #endif

    void constructMove2D();
    void constructMove3D();
    #ifndef MAGNUM_TARGET_GLES2
    void constructMoveUniformBuffers2D();
    void constructMoveUniformBuffers3D();
    #endif

    void constructInvalid2D();
    void constructInvalid3D();
    #ifndef MAGNUM_TARGET_GLES2
    void constructUniformBuffersInvalid2D();
    void constructUniformBuffersInvalid3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void setPerVertexJointCountInvalid2D();
    void setPerVertexJointCountInvalid3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setUniformUniformBuffersEnabled2D();
    void setUniformUniformBuffersEnabled3D();
    void bindBufferUniformBuffersNotEnabled2D();
    void bindBufferUniformBuffersNotEnabled3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void bindObjectIdTextureInvalid2D();
    void bindObjectIdTextureInvalid3D();
    void bindObjectIdTextureArrayInvalid2D();
    void bindObjectIdTextureArrayInvalid3D();
    #endif
    void setWireframeNotEnabled2D();
    void setWireframeNotEnabled3D();
    #ifndef MAGNUM_TARGET_GLES2
    void setTextureMatrixNotEnabled2D();
    void setTextureMatrixNotEnabled3D();
    void setTextureLayerNotArray2D();
    void setTextureLayerNotArray3D();
    void bindTextureTransformBufferNotEnabled2D();
    void bindTextureTransformBufferNotEnabled3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setObjectIdNotEnabled2D();
    void setObjectIdNotEnabled3D();
    void setColorMapNotEnabled2D();
    void setColorMapNotEnabled3D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void setTangentBitangentNormalNotEnabled3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setWrongJointCountOrId2D();
    void setWrongJointCountOrId3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setWrongDrawOffset2D();
    void setWrongDrawOffset3D();
    #endif

    void renderSetup();
    void renderTeardown();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderDefaultsWireframe2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderDefaultsWireframe3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderDefaultsObjectId2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderDefaultsObjectId3D();
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderDefaultsInstancedObjectId2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderDefaultsInstancedObjectId3D();
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderDefaultsVertexId2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderDefaultsVertexId3D();
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderDefaultsPrimitiveId2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderDefaultsPrimitiveId3D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderDefaultsTangentBitangentNormal();
    #endif
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderWireframe2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderWireframe3D();
    #ifndef MAGNUM_TARGET_GLES2
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderObjectVertexPrimitiveId2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderObjectVertexPrimitiveId3D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* This tests something that's irrelevant to UBOs */
    void renderWireframe3DPerspective();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderTangentBitangentNormal();
    #endif

    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderSkinningWireframe2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderSkinningWireframe3D();

    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderInstanced2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderInstanced3D();
    #ifndef MAGNUM_TARGET_GLES2
    template<MeshVisualizerGL2D::Flag flag = MeshVisualizerGL2D::Flag{}> void renderInstancedSkinningWireframe2D();
    template<MeshVisualizerGL3D::Flag flag = MeshVisualizerGL3D::Flag{}> void renderInstancedSkinningWireframe3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti2D();
    void renderMulti3D();
    void renderMultiSkinningWireframe2D();
    void renderMultiSkinningWireframe3D();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        Containers::String _testDir;

        GL::Renderbuffer _color{NoCreate},
            _depth{NoCreate};
        GL::Framebuffer _framebuffer{NoCreate};
        #ifndef MAGNUM_TARGET_GLES2
        GL::Texture2D _colorMapTexture;
        #endif
};

/*
    Rendering tests done:

    [W] wireframe
    [D] primitive/vertex/object ID
    [T] TBN visualization
    [O] draw offset
    [M] multidraw

    Mesa Intel                      WDTOM
               ES2                     xx
               ES3                      x
    Mesa AMD                        WDT
    Mesa llvmpipe                   WDT
    SwiftShader ES2                 WDxxx
                ES3                 WDx
    ANGLE ES2                          xx
          ES3                       WDxOM
    ARM Mali (Huawei P10) ES2       W xxx
                          ES3       W  Ox (WDT big diffs, needs investigation)
    WebGL (on Mesa Intel) 1.0       W xxx
                          2.0       W x M
    NVidia
    Intel Windows
    AMD macOS
    Intel macOS                     WDTOx
    iPhone 6 w/ iOS 12.4 ES3        W x x
*/

using namespace Math::Literals;

constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags;
} ConstructData2D[] {
    /* Whatever is added here should probably go also into
       ConstructUniformBuffersData2D */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe", MeshVisualizerGL2D::Flag::Wireframe},
    #endif
    {"wireframe w/o GS", MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", MeshVisualizerGL2D::Flag::ObjectId},
    {"instanced object ID", MeshVisualizerGL2D::Flag::InstancedObjectId},
    {"object ID texture", MeshVisualizerGL2D::Flag::ObjectIdTexture},
    {"object ID texture array", MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays},
    {"object ID texture + instanced texture transformation", MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset},
    {"object ID texture array + instanced texture transformation", MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::InstancedTextureOffset},
    {"instanced object ID texture array + texture transformation", MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::TextureTransformation},
    {"wireframe + object ID texture + instanced texture transformation", MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset},
    {"vertex ID", MeshVisualizerGL2D::Flag::VertexId},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizerGL2D::Flag::PrimitiveId},
    #endif
    {"primitive ID from vertex ID", MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId}
    #endif
};

constexpr struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
} ConstructData3D[] {
    /* Whatever is added here should probably go also into
       ConstructUniformBuffersData3D */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe", MeshVisualizerGL3D::Flag::Wireframe},
    #endif
    {"wireframe w/o GS", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", MeshVisualizerGL3D::Flag::ObjectId},
    {"instanced object ID", MeshVisualizerGL3D::Flag::InstancedObjectId},
    {"object ID texture", MeshVisualizerGL3D::Flag::ObjectIdTexture},
    {"object ID texture array", MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays},
    {"object ID texture + instanced texture transformation", MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset},
    {"object ID texture array + instanced texture transformation", MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::InstancedTextureOffset},
    {"instanced object ID texture array + texture transformation", MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::TextureTransformation},
    {"wireframe + object ID texture + instanced texture transformation", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset},
    {"vertex ID", MeshVisualizerGL3D::Flag::VertexId},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizerGL3D::Flag::PrimitiveId},
    #endif
    {"primitive ID from vertex ID", MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId},
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"tangent direction", MeshVisualizerGL3D::Flag::TangentDirection},
    {"bitangent direction from tangent", MeshVisualizerGL3D::Flag::BitangentFromTangentDirection},
    {"bitangent direction", MeshVisualizerGL3D::Flag::BitangentDirection},
    {"normal direction", MeshVisualizerGL3D::Flag::NormalDirection},
    {"tbn direction", MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::NormalDirection},
    {"tbn direction with bitangent from tangent", MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection},
    {"wireframe + vertex ID", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::VertexId},
    {"wireframe + T/N direction", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::NormalDirection},
    {"wireframe + instanced object ID + T/N direction", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::NormalDirection},
    {"wireframe + vertex ID + T/B direction", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection},
    /* InstancedObjectId|BitangentDirection is disallowed (checked in
       ConstructInvalidData3D), but both ObjectId alone and
       BitangentFromTangentDirection should work */
    {"object ID + bitangent direction", MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::BitangentDirection},
    {"instanced object ID + bitangent from tangent direction", MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags2D;
    MeshVisualizerGL3D::Flags flags3D;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructSkinningData[]{
    {"no skinning", {}, {},
        0, 0, 0},
    {"one set", {}, {},
        16, 4, 0},
    {"two partial sets", {}, {},
        32, 2, 3},
    {"secondary set only", {}, {},
        12, 0, 4},
    {"dynamic per-vertex sets",
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        16, 4, 3},
};

constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructUniformBuffersData2D[] {
    {"classic fallback", MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        1, 1, 0, 0, 0},
    {"", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        1, 1, 0, 0, 0},
    /* SwiftShader has 256 uniform vectors at most, per-2D-draw is 4,
       per-material 4, two need to be left for drawOffset + viewportSize */
    {"multiple materials, draws", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        8, 55, 0, 0, 0},
    {"skinning", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe,
        1, 1, 32, 3, 2},
    {"skinning, dynamic per-vertex sets", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        1, 1, 32, 3, 4},
    {"multidraw with wireframe w/o GS and vertex ID", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader|MeshVisualizerGL2D::Flag::VertexId,
        8, 55, 0, 0, 0},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw with wireframe and primitive ID", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::PrimitiveId,
        8, 55, 0, 0, 0},
    #endif
    {"multidraw with wireframe w/o GS, instancing and dynamic primary skinning per-vertex sets", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader|MeshVisualizerGL2D::Flag::InstancedTransformation|MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        8, 55, 16, 4, 0},
    {"multidraw with wireframe w/o GS and dynamic primary+secondary skinning per-vertex sets", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader|MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        8, 55, 16, 3, 4},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with wireframe w/o GS, instancing and dynamic primary skinning per-vertex sets", MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader|MeshVisualizerGL2D::Flag::InstancedTransformation|MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        0, 0, 0, 4, 0},
    {"shader storage + multidraw with wireframe w/o GS and dynamic primary+secondary skinning per-vertex sets", MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader|MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        0, 0, 0, 3, 4},
    #endif
    /* The rest is basically a copy of ConstructData2D with UniformBuffers
       added */
    #ifndef MAGNUM_TARGET_WEBGL
    {"wireframe", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe,
        1, 1, 0, 0, 0},
    #endif
    {"wireframe w/o GS", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        1, 1, 0, 0, 0},
    {"object ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectId,
        1, 1, 0, 0, 0},
    {"instanced object ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::InstancedObjectId,
        1, 1, 0, 0, 0},
    {"object ID texture", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture,
        1, 1, 0, 0, 0},
    {"object ID texture array", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"object ID texture + instanced texture transformation", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"object ID texture array + instanced texture transformation", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"instanced object ID texture array + texture transformation", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"wireframe + object ID texture + instanced texture transformation", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"vertex ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::VertexId,
        1, 1, 0, 0, 0},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::PrimitiveId,
        1, 1, 0, 0, 0},
    #endif
    {"primitive ID from vertex ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId,
        1, 1, 0, 0, 0}
};

constexpr struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
    UnsignedInt materialCount, drawCount;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
} ConstructUniformBuffersData3D[] {
    {"classic fallback", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        1, 1, 0, 0, 0},
    {"", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        1, 1, 0, 0, 0},
    /* SwiftShader has 256 uniform vectors at most, per-3D-draw is 4+4,
       per-material 4, plus 4 for projection */
    {"multiple materials, draws", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        6, 28, 0, 0, 0},
    {"skinning", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe,
        1, 1, 32, 3, 2},
    {"skinning, dynamic per-vertex sets", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        1, 1, 32, 3, 4},
    {"multidraw with wireframe w/o GS and vertex ID", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader|MeshVisualizerGL3D::Flag::VertexId,
        6, 28, 0, 0, 0},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw with wireframe, primitive ID and TBN", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::PrimitiveId|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        6, 28, 0, 0, 0},
    #endif
    {"multidraw with wireframe w/o GS, instancing and dynamic primary skinning per-vertex sets", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader|MeshVisualizerGL3D::Flag::InstancedTransformation|MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        8, 55, 16, 4, 0},
    {"multidraw with wireframe w/o GS and dynamic primary+secondary skinning per-vertex sets", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader|MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        8, 55, 16, 3, 4},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with wireframe, instancing and dynamic primary skinning per-vertex sets", MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::InstancedTransformation|MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        0, 0, 0, 4, 0},
    {"shader storage + multidraw with wireframe and dynamic primary+secondary skinning per-vertex sets", MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        0, 0, 0, 3, 4},
    #endif
    /* The rest is basically a copy of ConstructData2D with UniformBuffers
       added */
    #ifndef MAGNUM_TARGET_WEBGL
    {"wireframe", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe,
        1, 1, 0, 0, 0},
    #endif
    {"wireframe w/o GS", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        1, 1, 0, 0, 0},
    {"object ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::ObjectId,
        1, 1, 0, 0, 0},
    {"instanced object ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::InstancedObjectId,
        1, 1, 0, 0, 0},
    {"object ID texture", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::ObjectIdTexture,
        1, 1, 0, 0, 0},
    {"object ID texture array", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"object ID texture + instanced texture transformation", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"object ID texture array + instanced texture transformation", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"instanced object ID texture array + texture transformation", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::TextureTransformation,
        1, 1, 0, 0, 0},
    {"wireframe + object ID texture + instanced texture transformation", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset,
        1, 1, 0, 0, 0},
    {"vertex ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::VertexId,
        1, 1, 0, 0, 0},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::PrimitiveId,
        1, 1, 0, 0, 0},
    #endif
    {"primitive ID from vertex ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId,
        1, 1, 0, 0, 0},
    #ifndef MAGNUM_TARGET_WEBGL
    {"tangent direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::TangentDirection,
        1, 1, 0, 0, 0},
    {"bitangent direction from tangent", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection,
        1, 1, 0, 0, 0},
    {"bitangent direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::BitangentDirection,
        1, 1, 0, 0, 0},
    {"normal direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::NormalDirection,
        1, 1, 0, 0, 0},
    {"tbn direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        1, 1, 0, 0, 0},
    {"tbn direction with bitangent from tangent", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        1, 1, 0, 0, 0},
    {"wireframe + vertex ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::VertexId,
        1, 1, 0, 0, 0},
    {"wireframe + T/N direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        1, 1, 0, 0, 0},
    {"wireframe + instanced object ID + T/N direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        1, 1, 0, 0, 0},
    {"wireframe + vertex ID + T/B direction", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection,
        1, 1, 0, 0, 0}
    #endif
};
#endif

constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    const char* message;
} ConstructInvalidData2D[] {
    {"no feature enabled",
        MeshVisualizerGL2D::Flag::NoGeometryShader, /* not a feature flag */
        0, 0, 0,
        #ifndef MAGNUM_TARGET_GLES2
        "2D: at least one visualization feature has to be enabled"
        #else
        "2D: at least Flag::Wireframe has to be enabled"
        #endif
        },
    #ifndef MAGNUM_TARGET_GLES2
    {"both object and primitive ID",
        MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId,
        0, 0, 0,
        ": Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"both instanced object and primitive ID",
        MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId,
        0, 0, 0,
        ": Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"both object and vertex ID",
        MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::VertexId,
        0, 0, 0,
        ": Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"texture transformation but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectId,
        0, 0, 0,
        ": texture transformation enabled but the shader is not textured"},
    {"texture arrays but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::ObjectId,
        0, 0, 0,
        ": texture arrays enabled but the shader is not textured"},
    {"dynamic per-vertex joint count but no static per-vertex joint count",
        MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        0, 0, 0,
        "2D: dynamic per-vertex joint count enabled for zero joints"},
    {"instancing together with secondary per-vertex sets",
        MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::InstancedTransformation,
        10, 4, 1,
        "2D: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead"}
    #endif
};

constexpr struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    const char* message;
} ConstructInvalidData3D[] {
    {"no feature enabled",
        MeshVisualizerGL3D::Flag::NoGeometryShader, /* not a feature flag */
        0, 0, 0,
        #ifndef MAGNUM_TARGET_GLES2
        "3D: at least one visualization feature has to be enabled"
        #else
        "3D: at least Flag::Wireframe has to be enabled"
        #endif
        },
    #ifndef MAGNUM_TARGET_GLES2
    {"both object and primitive ID",
        MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId,
        0, 0, 0,
        ": Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"both instanced object and primitive ID",
        MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId,
        0, 0, 0,
        ": Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"both vertex and primitive ID",
        MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId,
        0, 0, 0,
        ": Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive"},
    {"texture transformation but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectId,
        0, 0, 0,
        ": texture transformation enabled but the shader is not textured"},
    {"texture arrays but not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::ObjectId,
        0, 0, 0,
        ": texture arrays enabled but the shader is not textured"},
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"geometry shader disabled but needed",
        MeshVisualizerGL3D::Flag::NoGeometryShader|MeshVisualizerGL3D::Flag::NormalDirection,
        0, 0, 0,
        "3D: geometry shader has to be enabled when rendering TBN direction"},
    {"conflicting bitangent input",
        MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection,
        0, 0, 0,
        "3D: Flag::BitangentDirection and Flag::BitangentFromTangentDirection are mutually exclusive"},
    {"conflicting bitangent and instanced object ID attribute",
        MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::InstancedObjectId,
        0, 0, 0,
        "3D: Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead"},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    {"dynamic per-vertex joint count but no static per-vertex joint count",
        MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        0, 0, 0,
        "3D: dynamic per-vertex joint count enabled for zero joints"},
    {"instancing together with secondary per-vertex sets",
        MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::InstancedTransformation,
        10, 4, 1,
        "3D: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead"}
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount, materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData2D[] {
    /* These two fail for UBOs but not SSBOs */
    {"zero draws",
        MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        0, 0, 0, 1, 0,
        "draw count can't be zero"},
    {"zero materials",
        MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        0, 0, 0, 0, 1,
        "material count can't be zero"},
    /* These two fail for UBOs but not SSBOs */
    {"per-vertex joint count but no joint count",
        MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        0, 2, 0, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
    {"secondary per-vertex joint count but no joint count",
        MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        0, 0, 3, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
};

constexpr struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount, materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData3D[] {
    /* These two fail for UBOs but not SSBOs */
    {"zero draws",
        MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        0, 0, 0, 1, 0,
        "draw count can't be zero"},
    {"zero materials",
        MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        0, 0, 0, 0, 1,
        "material count can't be zero"},
    /* These two fail for UBOs but not SSBOs */
    {"per-vertex joint count but no joint count",
        MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        0, 2, 0, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
    {"secondary per-vertex joint count but no joint count",
        MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        0, 0, 3, 1, 1,
        "joint count can't be zero if per-vertex joint count is non-zero"},
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags2D;
    MeshVisualizerGL3D::Flags flags3D;
    const char* message;
} BindObjectIdTextureInvalidData[]{
    {"not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        MeshVisualizerGL2D::Flag::ObjectId,
        MeshVisualizerGL3D::Flag::ObjectId,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled\n"},
    {"array",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead\n"}
};

constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags2D;
    MeshVisualizerGL3D::Flags flags3D;
    const char* message;
} BindObjectIdTextureArrayInvalidData[]{
    {"not textured",
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        /* ObjectId shares bits with ObjectIdTexture but should still trigger
           the assert */
        MeshVisualizerGL2D::Flag::ObjectId,
        MeshVisualizerGL3D::Flag::ObjectId,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled\n"},
    {"not array",
        MeshVisualizerGL2D::Flag::ObjectIdTexture,
        MeshVisualizerGL3D::Flag::ObjectIdTexture,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead\n"}
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    SamplerFilter filter;
    SamplerWrapping wrapping;
} RenderInstancedObjectIdDefaultsData[] {
    {"nearest, clamp", SamplerFilter::Nearest, SamplerWrapping::ClampToEdge},
    {"nearest, repeat", SamplerFilter::Nearest, SamplerWrapping::Repeat},
    {"linear, clamp", SamplerFilter::Linear, SamplerWrapping::ClampToEdge},
    {"linear, repeat", SamplerFilter::Linear, SamplerWrapping::Repeat}
};
#endif

constexpr struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags;
    Float width, smoothness;
    const char* file;
    const char* fileXfail;
} RenderWireframeData2D[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"", MeshVisualizerGL2D::Flags{},
        1.0f, 2.0f, "wireframe2D.tga", nullptr},
    {"wide/sharp", MeshVisualizerGL2D::Flags{},
        3.0f, 1.0f, "wireframe-wide2D.tga", nullptr},
    #endif
    {"no geometry shader", MeshVisualizerGL2D::Flag::NoGeometryShader,
        1.0f, 2.0f, "wireframe2D.tga", "wireframe-nogeo2D.tga"},
    {"no geometry shader, wide/sharp", MeshVisualizerGL2D::Flag::NoGeometryShader,
        3.0f, 1.0f, "wireframe-wide2D.tga", "wireframe-nogeo2D.tga"}
};

constexpr struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
    Float width, smoothness;
    const char* file;
    const char* fileXfail;
} RenderWireframeData3D[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"", MeshVisualizerGL3D::Flags{},
        1.0f, 2.0f, "wireframe3D.tga", nullptr},
    {"wide/sharp", MeshVisualizerGL3D::Flags{},
        3.0f, 1.0f, "wireframe-wide3D.tga", nullptr},
    #endif
    {"no geometry shader",
        MeshVisualizerGL3D::Flag::NoGeometryShader,
        1.0f, 2.0f, "wireframe3D.tga", "wireframe-nogeo3D.tga"},
    {"no geometry shader, wide/sharp",
        MeshVisualizerGL3D::Flag::NoGeometryShader,
        3.0f, 1.0f, "wireframe-wide3D.tga", "wireframe-nogeo3D.tga"}
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags2D;
    MeshVisualizerGL3D::Flags flags3D;
    Matrix3 textureTransformation;
    bool flip;
    Int layer;
    const char* file2D;
    const char* file3D;
} RenderObjectVertexPrimitiveIdData[] {
    {"object ID",
        MeshVisualizerGL2D::Flag::ObjectId,
        MeshVisualizerGL3D::Flag::ObjectId,
        {}, false, 0,
        "objectid2D.tga", "objectid3D.tga"},
    {"instanced object ID",
        MeshVisualizerGL2D::Flag::InstancedObjectId,
        MeshVisualizerGL3D::Flag::InstancedObjectId,
        {}, false, 0,
        "instancedobjectid2D.tga", "instancedobjectid3D.tga"},
    {"textured object ID",
        MeshVisualizerGL2D::Flag::ObjectIdTexture,
        MeshVisualizerGL3D::Flag::ObjectIdTexture,
        {}, false, 0,
        "objectidtexture2D.tga", "objectidtexture3D.tga"},
    {"textured object ID, texture transformation",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureTransformation,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), true, 0,
        "objectidtexture2D.tga", "objectidtexture3D.tga"},
    {"texture array object ID, first layer",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        {}, false, 0,
        "objectidtexture2D.tga", "objectidtexture3D.tga"},
    {"texture array object ID, arbitrary layer",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        {}, false, 6,
        "objectidtexture2D.tga", "objectidtexture3D.tga"},
    {"texture array object ID, texture transformation, arbitrary layer",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::TextureTransformation,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::TextureTransformation,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), true, 6,
        "objectidtexture2D.tga", "objectidtexture3D.tga"},
    {"vertex ID",
        MeshVisualizerGL2D::Flag::VertexId,
        MeshVisualizerGL3D::Flag::VertexId,
        {}, false, 0,
        "vertexid2D.tga", "vertexid3D.tga"},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID",
        MeshVisualizerGL2D::Flag::PrimitiveId,
        MeshVisualizerGL3D::Flag::PrimitiveId,
        {}, false, 0,
        "primitiveid2D.tga", "primitiveid3D.tga"},
    #endif
    {"primitive ID from vertex ID",
        MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId,
        MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId,
        {}, false, 0,
        "primitiveid2D.tga", "primitiveid3D.tga"},
    {"wireframe + instanced object ID",
        MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::Wireframe,
        MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::Wireframe,
        {}, false, 0,
        "wireframe-instancedobjectid2D.tga", "wireframe-instancedobjectid3D.tga"},
    {"wireframe + instanced object ID, no geometry shader",
        MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::Wireframe|
        MeshVisualizerGL2D::Flag::NoGeometryShader,
        MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::Wireframe|
        MeshVisualizerGL3D::Flag::NoGeometryShader,
        {}, false, 0,
        "wireframe-nogeo-instancedobjectid2D.tga", "wireframe-nogeo-instancedobjectid3D.tga"},
    /* These two are here to test that all required texture-related attributes
       are properly passed through the GS */
    {"wireframe + textured object ID",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::Wireframe,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::Wireframe,
        {}, false, 0,
        "wireframe-objectidtexture2D.tga", "wireframe-objectidtexture3D.tga"},
    {"wireframe + texture array object ID, texture transformation, arbitrary layer",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays|MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::Wireframe,
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays|MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::Wireframe,
        Matrix3::translation(Vector2{1.0f})*Matrix3::scaling(Vector2{-1.0f}), true, 6,
        "wireframe-objectidtexture2D.tga", "wireframe-objectidtexture3D.tga"},
    {"wireframe + vertex ID",
        MeshVisualizerGL2D::Flag::VertexId|MeshVisualizerGL2D::Flag::Wireframe,
        MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::Wireframe,
        {}, false, 0,
        "wireframe-vertexid2D.tga", "wireframe-vertexid3D.tga"}
};
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
constexpr struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
    MeshVisualizerGL3D::Flags secondPassFlags;
    bool skipBitagnentEvenIfEnabledInFlags;
    Float smoothness;
    Float lineWidth;
    Float lineLength;
    Float multiply;
    const char* file;
} RenderTangentBitangentNormalData[] {
    {"",
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::BitangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "tbn.tga"},
    {"bitangents from tangents",
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "tbn.tga"},
    {"scaled data",
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::BitangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 5.0f, "tbn.tga"},
    {"wide blurry lines",
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::BitangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 5.0f, 5.0f, 0.8f, 1.0f, "tbn-wide.tga"},
    {"only bitangent from tangent",
        MeshVisualizerGL3D::Flag::BitangentFromTangentDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "bitangents-from-tangents.tga"},
    {"wireframe + primitive ID + tangents + normals, single pass",
        MeshVisualizerGL3D::Flag::Wireframe|
        MeshVisualizerGL3D::Flag::PrimitiveId|
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "wireframe-primitiveid-tn.tga"},
    {"wireframe + primitive ID, rendering all, but only tangents + normals present",
        MeshVisualizerGL3D::Flag::Wireframe|
        MeshVisualizerGL3D::Flag::PrimitiveId|
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::BitangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        true, 2.0f, 1.0f, 0.6f, 1.0f, "wireframe-primitiveid-tn.tga"},
    {"wireframe + tangents + normals, two passes",
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection,
        MeshVisualizerGL3D::Flag::Wireframe,
        false, 2.0f, 1.0f, 0.6f, 1.0f, "wireframe-tn-smooth.tga"},
    {"vertex ID + tangents + normals",
        MeshVisualizerGL3D::Flag::VertexId|
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "vertexid-tn.tga"},
    {"primitive ID + tangents + normals",
        MeshVisualizerGL3D::Flag::PrimitiveId|
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "primitiveid-tn.tga"},
    {"object ID + tangents + normals",
        /* Not instanced, so it's testing the case where the GS doesn't need to
           propagate any attribute but still has to render the actual face */
        MeshVisualizerGL3D::Flag::ObjectId|
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "objectid-tn.tga"},
    {"instanced object ID + tangents + normals",
        /* No instance data supplied, thus the output should be exactly the
           same as the non-instanced case */
        MeshVisualizerGL3D::Flag::InstancedObjectId|
        MeshVisualizerGL3D::Flag::TangentDirection|
        MeshVisualizerGL3D::Flag::NormalDirection, {},
        false, 2.0f, 1.0f, 0.6f, 1.0f, "objectid-tn.tga"}
};
#endif

#ifndef MAGNUM_TARGET_GLES2
/* Same as in FlatGL and PhongGL tests */
const struct {
    const char* name;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    UnsignedInt dynamicPerVertexJointCount, dynamicSecondaryPerVertexJointCount;
    MeshVisualizerGL2D::Flags flags2D;
    MeshVisualizerGL3D::Flags flags3D;
    Containers::Array<Containers::Pair<UnsignedInt, GL::DynamicAttribute>> attributes;
    bool setDynamicPerVertexJointCount, setJointMatrices, setJointMatricesOneByOne;
    const char* expected;
} RenderSkinningData[]{
    {"no skinning", 0, 0, 0, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, false, false, false,
        "skinning-default.tga"},
    {"default joint matrices", 5, 3, 0, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, false, false, false,
        "skinning-default.tga"},
    {"single set", 5, 3, 0, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, upload just a prefix of joint matrices", 15, 3, 0, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, upload joint matrices one by one", 5, 3, 0, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, false, true, true,
        "skinning.tga"},
    {"single set, dynamic, left at defaults", 5, 3, 0, 0, 0,
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"single set, dynamic", 5, 4, 0, 3, 0,
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three}},
        }}, true, true, false,
        "skinning.tga"},
    {"two sets", 5, 1, 2, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::One}},
            {4, MeshVisualizerGL2D::SecondaryJointIds{MeshVisualizerGL2D::SecondaryJointIds::Components::Two}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::One}},
            {4*4, MeshVisualizerGL2D::SecondaryWeights{MeshVisualizerGL2D::SecondaryWeights::Components::Two}},
        }}, false, true, false,
        "skinning.tga"},
    {"two sets, dynamic, left at defaults", 5, 1, 2, 0, 0,
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::One}},
            {4, MeshVisualizerGL2D::SecondaryJointIds{MeshVisualizerGL2D::SecondaryJointIds::Components::Two}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::One}},
            {4*4, MeshVisualizerGL2D::SecondaryWeights{MeshVisualizerGL2D::SecondaryWeights::Components::Two}},
        }}, false, true, false,
        "skinning.tga"},
    {"two sets, dynamic", 5, 4, 4, 1, 2,
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount, MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::One}},
            {4, MeshVisualizerGL2D::SecondaryJointIds{MeshVisualizerGL2D::SecondaryJointIds::Components::Two}},
            {3*4, MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::One}},
            {4*4, MeshVisualizerGL2D::SecondaryWeights{MeshVisualizerGL2D::SecondaryWeights::Components::Two}},
        }}, true, true, false,
        "skinning.tga"},
    {"only secondary set", 5, 0, 3, 0, 0, {}, {}, {InPlaceInit, {
            {0, MeshVisualizerGL2D::SecondaryJointIds{MeshVisualizerGL2D::SecondaryJointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::SecondaryWeights{MeshVisualizerGL2D::SecondaryWeights::Components::Three}},
        }}, false, true, false,
        "skinning.tga"},
    {"only secondary set, dynamic", 5, 4, 4, 0, 3,
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount, {InPlaceInit, {
            #ifdef MAGNUM_TARGET_WEBGL
            /* On WebGL the primary joint vertex attribute has to be bound to
               something even if not (dynamically) used in the end, otherwise
               it causes an error. So just alias it with the secondary one. */
            {0, MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three}},
            #endif
            {0, MeshVisualizerGL2D::SecondaryJointIds{MeshVisualizerGL2D::SecondaryJointIds::Components::Three}},
            {3*4, MeshVisualizerGL2D::SecondaryWeights{MeshVisualizerGL2D::SecondaryWeights::Components::Three}},
        }}, true, true, false,
        "skinning.tga"},
};
#endif

const struct {
    const char* name;
    const char* expected;
    MeshVisualizerGL2D::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData2D[]{
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe", "instanced-wireframe2D.tga",
        MeshVisualizerGL2D::Flag::Wireframe,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.012f},
    #endif
    {"wireframe w/o GS", "instanced-wireframe-nogeo2D.tga",
        MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        /* SwiftShader has a few rounding errors on edges */
        73.67f, 0.230f},
    #ifndef MAGNUM_TARGET_GLES2
    {"vertex ID", "instanced-vertexid2D.tga",
        MeshVisualizerGL2D::Flag::VertexId,
        /* SwiftShader has a few rounding errors on edges */
        138.7f, 0.08f},
    {"instanced object ID", "instanced-instancedobjectid2D.tga",
        MeshVisualizerGL2D::Flag::InstancedObjectId,
        /* SwiftShader has a few rounding errors on edges */
        133.0f, 0.12f},
    {"textured object ID", "instanced-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset,
        /* SwiftShader has a few rounding errors on edges */
        146.7f, 0.097f},
    {"instanced textured object ID", "instanced-instancedobjectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset,
        /* SwiftShader has a few rounding errors on edges */
        133.0f, 0.071f},
    {"instanced textured array object ID", "instanced-instancedobjectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::InstancedTextureOffset|MeshVisualizerGL2D::Flag::TextureArrays,
        /* SwiftShader has a few rounding errors on edges */
        133.0f, 0.071f},
    #endif
};

const struct {
    const char* name;
    const char* expected;
    MeshVisualizerGL3D::Flags flags;
    Float maxThreshold, meanThreshold;
} RenderInstancedData3D[]{
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe", "instanced-wireframe3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe,
        /* Minor differences on NVidia */
        0.667f, 0.0054f},
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe + TBN", "instanced-wireframe-tbn3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        /* Minor differences on NVidia */
        0.667f, 0.0050f},
    #endif
    {"wireframe w/o GS", "instanced-wireframe-nogeo3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        /* SwiftShader has a minor rounding error */
        7.334f, 0.192f},
    #ifndef MAGNUM_TARGET_GLES2
    {"vertex ID", "instanced-vertexid3D.tga",
        MeshVisualizerGL3D::Flag::VertexId,
        /* SwiftShader has a minor rounding error */
        5.667f, 0.034f},
    {"instanced object ID", "instanced-instancedobjectid3D.tga",
        MeshVisualizerGL3D::Flag::InstancedObjectId,
        /* SwiftShader has an off-by-one error on certain colors */
        0.334f, 0.042f},
    {"textured object ID", "instanced-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset,
        /* SwiftShader has a few rounding errors on edges */
        28.67f, 0.097f},
    {"instanced textured object ID", "instanced-instancedobjectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset,
        /* SwiftShader has a few rounding errors on edges */
        32.67f, 0.101f},
    {"instanced textured array object ID", "instanced-instancedobjectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::InstancedTextureOffset|MeshVisualizerGL3D::Flag::TextureArrays,
        /* SwiftShader has a few rounding errors on edges */
        32.67f, 0.101f},
    #endif
};

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    const char* expected;
    MeshVisualizerGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData2D[] {
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, wireframe", "multidraw-wireframe2D.tga",
        MeshVisualizerGL2D::Flag::Wireframe,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    #endif
    {"bind with offset, wireframe w/o GS", "multidraw-wireframe-nogeo2D.tga",
        MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.02f},
    {"bind with offset, vertex ID", "multidraw-vertexid2D.tga",
        MeshVisualizerGL2D::Flag::VertexId,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    {"bind with offset, instanced object ID", "multidraw-instancedobjectid2D.tga",
        MeshVisualizerGL2D::Flag::InstancedObjectId,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.108f},
    {"bind with offset, textured object ID", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.073f},
    {"bind with offset, textured array object ID", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.073f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, textured array object ID, shader storage", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        0, 0, true, 16,
        /* Minor differences on NVidia */
        0.667f, 0.056f},
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, wireframe", "multidraw-wireframe2D.tga",
        MeshVisualizerGL2D::Flag::Wireframe,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    #endif
    {"draw offset, wireframe w/o GS", "multidraw-wireframe-nogeo2D.tga",
        MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.02f},
    {"draw offset, vertex ID", "multidraw-vertexid2D.tga",
        MeshVisualizerGL2D::Flag::VertexId,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    {"draw offset, instanced object ID", "multidraw-instancedobjectid2D.tga",
        MeshVisualizerGL2D::Flag::InstancedObjectId,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.108f},
    {"draw offset, textured object ID", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.073f},
    {"draw offset, textured array object ID", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.073f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, textured array object ID, shader storage", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        0, 0, false, 1,
        /* Minor differences on NVidia */
        0.667f, 0.056f},
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, wireframe", "multidraw-wireframe2D.tga",
        MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    #endif
    {"multidraw, wireframe w/o GS", "multidraw-wireframe-nogeo2D.tga",
        MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.02f},
    {"multidraw, vertex ID", "multidraw-vertexid2D.tga",
        MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::VertexId,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    {"multidraw, instanced object ID", "multidraw-instancedobjectid2D.tga",
        MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::InstancedObjectId,
        2, 3, false, 1,
        0.0f, 0.0f},
    {"multidraw, textured object ID", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture,
        2, 3, false, 1,
        0.0f, 0.0f},
    {"multidraw, textured array object ID", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        2, 3, false, 1,
        0.0f, 0.0f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, textured array object ID, shader storage", "multidraw-objectidtexture2D.tga",
        MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::ObjectIdTexture|MeshVisualizerGL2D::Flag::TextureArrays,
        0, 0, false, 1,
        0.0f, 0.0f},
    #endif
};

constexpr struct {
    const char* name;
    const char* expected;
    MeshVisualizerGL3D::Flags flags;
    UnsignedInt materialCount, drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData3D[] {
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, wireframe", "multidraw-wireframe3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe,
        1, 1, true, 16,
        /* Minor differences on NVidia */
        0.667f, 0.0055f},
    {"bind with offset, wireframe + TBN", "multidraw-wireframe-tbn3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        1, 1, true, 16,
        /* Minor differences on NVidia */
        0.667f, 0.0054f},
    #endif
    {"bind with offset, wireframe w/o GS", "multidraw-wireframe-nogeo3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        1, 1, true, 16,
        /* Minor differences on ARM Mali, NVidia */
        11.34f, 0.068f},
    {"bind with offset, vertex ID", "multidraw-vertexid3D.tga",
        MeshVisualizerGL3D::Flag::VertexId,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    {"bind with offset, instanced object ID", "multidraw-instancedobjectid3D.tga",
        MeshVisualizerGL3D::Flag::InstancedObjectId,
        1, 1, true, 16,
        /* Minor differences on ARM Mali */
        0.34f, 0.007f},
    {"bind with offset, textured object ID", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture,
        1, 1, true, 16,
        0.0f, 0.0f},
    {"bind with offset, textured array object ID", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        1, 1, true, 16,
        0.0f, 0.0f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, textured array object ID, shader storage", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        0, 0, true, 16,
        0.0f, 0.0f},
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, wireframe", "multidraw-wireframe3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe,
        2, 3, false, 1,
        /* Minor differences on NVidia */
        0.667f, 0.0055f},
    {"draw offset, wireframe + TBN", "multidraw-wireframe-tbn3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        2, 3, false, 1,
        /* Minor differences on NVidia */
        0.667f, 0.0054f},
    #endif
    {"draw offset, wireframe w/o GS", "multidraw-wireframe-nogeo3D.tga",
        MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        2, 3, false, 1,
        /* Minor differences on ARM Mali, NVidia */
        11.34f, 0.068f},
    {"draw offset, vertex ID", "multidraw-vertexid3D.tga",
        MeshVisualizerGL3D::Flag::VertexId,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    {"draw offset, instanced object ID", "multidraw-instancedobjectid3D.tga",
        MeshVisualizerGL3D::Flag::InstancedObjectId,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.34f, 0.007f},
    {"draw offset, textured object ID", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture,
        2, 3, false, 1,
        0.0f, 0.0f},
    {"draw offset, textured array object ID", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        2, 3, false, 1,
        0.0f, 0.0f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, textured array object ID, shader storage", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        0, 0, false, 1,
        0.0f, 0.0f},
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, wireframe", "multidraw-wireframe3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe,
        2, 3, false, 1,
        /* Minor differences on NVidia */
        0.667f, 0.003f},
    {"multidraw, wireframe + TBN", "multidraw-wireframe-tbn3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection,
        2, 3, false, 1,
        /* Minor differences on NVidia */
        0.667f, 0.003f},
    #endif
    {"multidraw, wireframe w/o GS", "multidraw-wireframe-nogeo3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader,
        2, 3, false, 1,
        /* Minor differences on ARM Mali, NVidia */
        11.34f, 0.066f},
    {"multidraw, vertex ID", "multidraw-vertexid3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::VertexId,
        2, 3, false, 1,
        /* Minor differences on ARM Mali */
        0.67f, 0.01f},
    {"multidraw, instanced object ID", "multidraw-instancedobjectid3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::InstancedObjectId,
        2, 3, false, 1,
        0.0f, 0.0f},
    {"multidraw, textured object ID", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture,
        2, 3, false, 1,
        0.0f, 0.0f},
    {"multidraw, textured array object ID", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        2, 3, false, 1,
        0.0f, 0.0f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, textured array object ID, shader storage", "multidraw-objectidtexture3D.tga",
        MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::ObjectIdTexture|MeshVisualizerGL3D::Flag::TextureArrays,
        0, 0, false, 1,
        0.0f, 0.0f},
    #endif
};

/* Same as in FlatGL and PhongGL tests */
const struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags2D;
    MeshVisualizerGL3D::Flags flags3D;
    UnsignedInt materialCount, drawCount, jointCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
} RenderMultiSkinningData[]{
    {"bind with offset",
        {},
        {},
        1, 1, 4, true, 16},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, shader storage",
        MeshVisualizerGL2D::Flag::ShaderStorageBuffers,
        MeshVisualizerGL3D::Flag::ShaderStorageBuffers,
        0, 0, 0, true, 16},
    #endif
    {"draw offset",
        {},
        {},
        2, 3, 9, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, shader storage",
        MeshVisualizerGL2D::Flag::ShaderStorageBuffers,
        MeshVisualizerGL3D::Flag::ShaderStorageBuffers,
        0, 0, 0, false, 1},
    #endif
    {"multidraw",
        MeshVisualizerGL2D::Flag::MultiDraw,
        MeshVisualizerGL3D::Flag::MultiDraw,
        2, 3, 9, false, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, shader storage",
        MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::MultiDraw,
        MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::MultiDraw,
        0, 0, 0, false, 1}
    #endif
};
#endif

MeshVisualizerGLTest::MeshVisualizerGLTest() {
    addInstancedTests({&MeshVisualizerGLTest::construct2D},
        Containers::arraySize(ConstructData2D));
    addInstancedTests({&MeshVisualizerGLTest::construct3D},
        Containers::arraySize(ConstructData3D));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::constructSkinning2D,
                       &MeshVisualizerGLTest::constructSkinning3D},
        Containers::arraySize(ConstructSkinningData));
    #endif

    addTests({&MeshVisualizerGLTest::constructAsync2D,
              &MeshVisualizerGLTest::constructAsync3D});

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::constructUniformBuffers2D},
        Containers::arraySize(ConstructUniformBuffersData2D));
    addInstancedTests({&MeshVisualizerGLTest::constructUniformBuffers3D},
        Containers::arraySize(ConstructUniformBuffersData3D));

    addTests({&MeshVisualizerGLTest::constructUniformBuffersAsync2D,
              &MeshVisualizerGLTest::constructUniformBuffersAsync3D});
    #endif

    addTests({
        &MeshVisualizerGLTest::constructMove2D,
        &MeshVisualizerGLTest::constructMove3D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::constructMoveUniformBuffers2D,
        &MeshVisualizerGLTest::constructMoveUniformBuffers3D,
        #endif
        });

    addInstancedTests({&MeshVisualizerGLTest::constructInvalid2D},
        Containers::arraySize(ConstructInvalidData2D));
    addInstancedTests({&MeshVisualizerGLTest::constructInvalid3D},
        Containers::arraySize(ConstructInvalidData3D));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::constructUniformBuffersInvalid2D},
        Containers::arraySize(ConstructUniformBuffersInvalidData2D));
    addInstancedTests({&MeshVisualizerGLTest::constructUniformBuffersInvalid3D},
        Containers::arraySize(ConstructUniformBuffersInvalidData3D));
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addTests({&MeshVisualizerGLTest::setPerVertexJointCountInvalid2D,
              &MeshVisualizerGLTest::setPerVertexJointCountInvalid3D,
              &MeshVisualizerGLTest::setUniformUniformBuffersEnabled2D,
              &MeshVisualizerGLTest::setUniformUniformBuffersEnabled3D,
              &MeshVisualizerGLTest::bindBufferUniformBuffersNotEnabled2D,
              &MeshVisualizerGLTest::bindBufferUniformBuffersNotEnabled3D});
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::bindObjectIdTextureInvalid2D,
                       &MeshVisualizerGLTest::bindObjectIdTextureInvalid3D},
        Containers::arraySize(BindObjectIdTextureInvalidData));
    addInstancedTests({&MeshVisualizerGLTest::bindObjectIdTextureArrayInvalid2D,
                       &MeshVisualizerGLTest::bindObjectIdTextureArrayInvalid3D},
        Containers::arraySize(BindObjectIdTextureArrayInvalidData));
    #endif

    addTests({
        &MeshVisualizerGLTest::setWireframeNotEnabled2D,
        &MeshVisualizerGLTest::setWireframeNotEnabled3D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::setTextureMatrixNotEnabled2D,
        &MeshVisualizerGLTest::setTextureMatrixNotEnabled3D,
        &MeshVisualizerGLTest::setTextureLayerNotArray2D,
        &MeshVisualizerGLTest::setTextureLayerNotArray3D,
        &MeshVisualizerGLTest::bindTextureTransformBufferNotEnabled2D,
        &MeshVisualizerGLTest::bindTextureTransformBufferNotEnabled3D,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::setObjectIdNotEnabled2D,
        &MeshVisualizerGLTest::setObjectIdNotEnabled3D,
        &MeshVisualizerGLTest::setColorMapNotEnabled2D,
        &MeshVisualizerGLTest::setColorMapNotEnabled3D,
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        &MeshVisualizerGLTest::setTangentBitangentNormalNotEnabled3D,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::setWrongJointCountOrId2D,
        &MeshVisualizerGLTest::setWrongJointCountOrId3D,
        &MeshVisualizerGLTest::setWrongDrawOffset2D,
        &MeshVisualizerGLTest::setWrongDrawOffset3D
        #endif
        });

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* MSVC needs explicit type due to default template args */
    addTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderDefaultsWireframe2D,
        &MeshVisualizerGLTest::renderDefaultsWireframe2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        &MeshVisualizerGLTest::renderDefaultsWireframe2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        &MeshVisualizerGLTest::renderDefaultsWireframe3D,
        &MeshVisualizerGLTest::renderDefaultsWireframe3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        &MeshVisualizerGLTest::renderDefaultsWireframe3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        },
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderDefaultsObjectId2D,
        &MeshVisualizerGLTest::renderDefaultsObjectId2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsObjectId2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderDefaultsObjectId3D,
        &MeshVisualizerGLTest::renderDefaultsObjectId3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsObjectId3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderDefaultsInstancedObjectId2D,
        &MeshVisualizerGLTest::renderDefaultsInstancedObjectId2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsInstancedObjectId2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderDefaultsInstancedObjectId3D,
        &MeshVisualizerGLTest::renderDefaultsInstancedObjectId3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsInstancedObjectId3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        Containers::arraySize(RenderInstancedObjectIdDefaultsData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderDefaultsVertexId2D,
        &MeshVisualizerGLTest::renderDefaultsVertexId2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsVertexId2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderDefaultsVertexId3D,
        &MeshVisualizerGLTest::renderDefaultsVertexId3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsVertexId3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId2D,
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId3D,
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsPrimitiveId3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderDefaultsTangentBitangentNormal,
        &MeshVisualizerGLTest::renderDefaultsTangentBitangentNormal<MeshVisualizerGL3D::Flag::UniformBuffers>,
        &MeshVisualizerGLTest::renderDefaultsTangentBitangentNormal<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderWireframe2D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::renderWireframe2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderWireframe2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderWireframeData2D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderWireframe3D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::renderWireframe3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderWireframe3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderWireframeData3D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderObjectVertexPrimitiveId2D,
        &MeshVisualizerGLTest::renderObjectVertexPrimitiveId2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderObjectVertexPrimitiveId2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderObjectVertexPrimitiveId3D,
        &MeshVisualizerGLTest::renderObjectVertexPrimitiveId3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderObjectVertexPrimitiveId3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        },
        Containers::arraySize(RenderObjectVertexPrimitiveIdData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    addTests({&MeshVisualizerGLTest::renderWireframe3DPerspective},
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderTangentBitangentNormal,
        &MeshVisualizerGLTest::renderTangentBitangentNormal<MeshVisualizerGL3D::Flag::UniformBuffers>,
        &MeshVisualizerGLTest::renderTangentBitangentNormal<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        },
        Containers::arraySize(RenderTangentBitangentNormalData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderSkinningWireframe2D,
        &MeshVisualizerGLTest::renderSkinningWireframe2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderSkinningWireframe2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderSkinningWireframe3D,
        &MeshVisualizerGLTest::renderSkinningWireframe3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderSkinningWireframe3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>
        #endif
        },
        Containers::arraySize(RenderSkinningData),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    /* MSVC needs explicit type due to default template args */
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderInstanced2D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::renderInstanced2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderInstanced2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderInstancedData2D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    addInstancedTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderInstanced3D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGLTest::renderInstanced3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderInstanced3D<MeshVisualizerGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        Containers::arraySize(RenderInstancedData3D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    /* MSVC needs explicit type due to default template args */
    addTests<MeshVisualizerGLTest>({
        &MeshVisualizerGLTest::renderInstancedSkinningWireframe2D,
        &MeshVisualizerGLTest::renderInstancedSkinningWireframe2D<MeshVisualizerGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderInstancedSkinningWireframe2D<MeshVisualizerGL2D::Flag::ShaderStorageBuffers>,
        #endif
        &MeshVisualizerGLTest::renderInstancedSkinningWireframe3D,
        &MeshVisualizerGLTest::renderInstancedSkinningWireframe3D<MeshVisualizerGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &MeshVisualizerGLTest::renderInstancedSkinningWireframe3D<MeshVisualizerGL3D::Flag::UniformBuffers>
        #endif
        },
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&MeshVisualizerGLTest::renderMulti2D},
        Containers::arraySize(RenderMultiData2D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    addInstancedTests({&MeshVisualizerGLTest::renderMulti3D},
        Containers::arraySize(RenderMultiData3D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    addInstancedTests({&MeshVisualizerGLTest::renderMultiSkinningWireframe2D,
                       &MeshVisualizerGLTest::renderMultiSkinningWireframe3D},
        Containers::arraySize(RenderMultiSkinningData),
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
    if((data.flags & MeshVisualizerGL2D::Flag::InstancedObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags & MeshVisualizerGL2D::Flag::PrimitiveId && !(data.flags >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if((data.flags & MeshVisualizerGL2D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif

        #ifdef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
            CORRADE_INFO("Using" << GL::Extensions::NV::shader_noperspective_interpolation::string());
        #endif
    }
    #endif

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
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

void MeshVisualizerGLTest::construct3D() {
    auto&& data = ConstructData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL3D::Flag::InstancedObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(data.flags & MeshVisualizerGL3D::Flag::PrimitiveId && !(data.flags >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(((data.flags & MeshVisualizerGL3D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)) || (data.flags & (MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection))) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif

        #ifdef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
            CORRADE_INFO("Using" << GL::Extensions::NV::shader_noperspective_interpolation::string());
        #endif
    }
    #endif

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
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
void MeshVisualizerGLTest::constructSkinning2D() {
    auto&& data = ConstructSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        /* At least one visualization feature has to be enabled; disable GS so
           we don't need to check for it on ES */
        .setFlags(data.flags2D|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    CORRADE_COMPARE(shader.flags(), data.flags2D|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
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

void MeshVisualizerGLTest::constructSkinning3D() {
    auto&& data = ConstructSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        /* At least one visualization feature has to be enabled; disable GS so
           we don't need to check for it on ES */
        .setFlags(data.flags3D|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    CORRADE_COMPARE(shader.flags(), data.flags3D|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
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

void MeshVisualizerGLTest::constructAsync2D() {
    MeshVisualizerGL2D::CompileState state = MeshVisualizerGL2D::compile(MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        /* Skinning properties tested in constructUniformBuffersAsync2D(), as
           there we don't need to bother with ES2 */
    );
    CORRADE_COMPARE(state.flags(), MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    MeshVisualizerGL2D shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
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

void MeshVisualizerGLTest::constructAsync3D() {
    MeshVisualizerGL3D::CompileState state = MeshVisualizerGL3D::compile(MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        /* Skinning properties tested in constructUniformBuffersAsync3D(), as
           there we don't need to bother with ES2 */
    );
    CORRADE_COMPARE(state.flags(), MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    MeshVisualizerGL3D shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
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
void MeshVisualizerGLTest::constructUniformBuffers2D() {
    auto&& data = ConstructUniformBuffersData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL2D::Flag::InstancedObjectId || data.jointCount) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags & MeshVisualizerGL2D::Flag::PrimitiveId && !(data.flags >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if((data.flags & MeshVisualizerGL2D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif

        #ifdef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
            CORRADE_INFO("Using" << GL::Extensions::NV::shader_noperspective_interpolation::string());
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags & MeshVisualizerGL2D::Flag::UniformBuffers && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
    #endif

    if(data.flags >= MeshVisualizerGL2D::Flag::MultiDraw) {
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

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
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

void MeshVisualizerGLTest::constructUniformBuffers3D() {
    auto&& data = ConstructUniformBuffersData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL3D::Flag::InstancedObjectId || data.jointCount) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL300)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES300)
        #endif
    ) CORRADE_SKIP("gl_VertexID not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags & MeshVisualizerGL3D::Flag::PrimitiveId && !(data.flags >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(((data.flags & MeshVisualizerGL3D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)) || (data.flags & (MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection))) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif

        #ifdef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
            CORRADE_INFO("Using" << GL::Extensions::NV::shader_noperspective_interpolation::string());
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags & MeshVisualizerGL3D::Flag::UniformBuffers && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
    #endif

    if(data.flags >= MeshVisualizerGL3D::Flag::MultiDraw) {
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

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
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

void MeshVisualizerGLTest::constructUniformBuffersAsync2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    MeshVisualizerGL2D::CompileState state = MeshVisualizerGL2D::compile(MeshVisualizerGL2D::Configuration{}
        .setFlags( MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setMaterialCount(8)
        .setDrawCount(48)
        .setJointCount(7, 3, 4));
    CORRADE_COMPARE(state.flags(),  MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
    CORRADE_COMPARE(state.materialCount(), 8);
    CORRADE_COMPARE(state.drawCount(), 48);
    CORRADE_COMPARE(state.jointCount(), 7);
    CORRADE_COMPARE(state.perVertexJointCount(), 3);
    CORRADE_COMPARE(state.secondaryPerVertexJointCount(), 4);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    MeshVisualizerGL2D shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(),  MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
    CORRADE_COMPARE(shader.materialCount(), 8);
    CORRADE_COMPARE(shader.drawCount(), 48);
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

void MeshVisualizerGLTest::constructUniformBuffersAsync3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    MeshVisualizerGL3D::CompileState state = MeshVisualizerGL3D::compile(MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setMaterialCount(6)
        .setDrawCount(24)
        .setJointCount(7, 3, 4));
    CORRADE_COMPARE(state.flags(), MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
    CORRADE_COMPARE(state.materialCount(), 6);
    CORRADE_COMPARE(state.drawCount(), 24);
    CORRADE_COMPARE(state.jointCount(), 7);
    CORRADE_COMPARE(state.perVertexJointCount(), 3);
    CORRADE_COMPARE(state.secondaryPerVertexJointCount(), 4);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    MeshVisualizerGL3D shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
    CORRADE_COMPARE(state.materialCount(), 6);
    CORRADE_COMPARE(state.drawCount(), 24);
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

void MeshVisualizerGLTest::constructMove2D() {
    MeshVisualizerGL2D a{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        /* Skinning properties tested in constructMoveUniformBuffers2D(), as
           there we don't need to bother with ES2 */
    };
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizerGL2D b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizerGL2D c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

void MeshVisualizerGLTest::constructMove3D() {
    MeshVisualizerGL3D a{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        /* Skinning properties tested in constructMoveUniformBuffers3D(), as
           there we don't need to bother with ES2 */
    };
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizerGL3D b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizerGL3D c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::constructMoveUniformBuffers2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    MeshVisualizerGL2D a{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setMaterialCount(2)
        .setDrawCount(5)
        .setJointCount(16, 4, 3)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizerGL2D b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_COMPARE(b.jointCount(), 16);
    CORRADE_COMPARE(b.perVertexJointCount(), 4);
    CORRADE_COMPARE(b.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!a.id());

    MeshVisualizerGL2D c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_COMPARE(c.jointCount(), 16);
    CORRADE_COMPARE(c.perVertexJointCount(), 4);
    CORRADE_COMPARE(c.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!b.id());
}

void MeshVisualizerGLTest::constructMoveUniformBuffers3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    MeshVisualizerGL3D a{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setMaterialCount(2)
        .setDrawCount(5)
        .setJointCount(16, 4, 3)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizerGL3D b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_COMPARE(b.jointCount(), 16);
    CORRADE_COMPARE(b.perVertexJointCount(), 4);
    CORRADE_COMPARE(b.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!a.id());

    MeshVisualizerGL3D c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_COMPARE(c.jointCount(), 16);
    CORRADE_COMPARE(c.perVertexJointCount(), 4);
    CORRADE_COMPARE(c.secondaryPerVertexJointCount(), 3);
    CORRADE_VERIFY(!b.id());
}
#endif

void MeshVisualizerGLTest::constructInvalid2D() {
    auto&& data = ConstructInvalidData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshVisualizerGL2D{MeshVisualizerGL2D::Configuration{}
        .setFlags(data.flags)
        #ifndef MAGNUM_TARGET_GLES2
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        #endif
    };
    CORRADE_COMPARE(out, Utility::format("Shaders::MeshVisualizerGL{}\n", data.message));
}

void MeshVisualizerGLTest::constructInvalid3D() {
    auto&& data = ConstructInvalidData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshVisualizerGL3D{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags)
        #ifndef MAGNUM_TARGET_GLES2
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        #endif
    };
    CORRADE_COMPARE(out, Utility::format("Shaders::MeshVisualizerGL{}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::constructUniformBuffersInvalid2D() {
    auto&& data = ConstructUniformBuffersInvalidData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    Containers::String out;
    Error redirectError{&out};
    MeshVisualizerGL2D{MeshVisualizerGL2D::Configuration{}
        .setFlags(data.flags)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out, Utility::format("Shaders::MeshVisualizerGL2D: {}\n", data.message));
}

void MeshVisualizerGLTest::constructUniformBuffersInvalid3D() {
    auto&& data = ConstructUniformBuffersInvalidData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    Containers::String out;
    Error redirectError{&out};
    MeshVisualizerGL3D{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out, Utility::format("Shaders::MeshVisualizerGL3D: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setPerVertexJointCountInvalid2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* At least one visualization feature has to be enabled; disable GS so we
       don't need to check for it on ES */
    MeshVisualizerGL2D a{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)};
    MeshVisualizerGL2D b{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setJointCount(16, 3, 2)};

    Containers::String out;
    Error redirectError{&out};
    a.setPerVertexJointCount(3, 2);
    b.setPerVertexJointCount(4);
    b.setPerVertexJointCount(3, 3);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled\n"
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): expected at most 3 per-vertex joints, got 4\n"
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): expected at most 2 secondary per-vertex joints, got 3\n");
}

void MeshVisualizerGLTest::setPerVertexJointCountInvalid3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* At least one visualization feature has to be enabled; disable GS so we
       don't need to check for it on ES */
    MeshVisualizerGL3D a{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)};
    MeshVisualizerGL3D b{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setJointCount(16, 3, 2)};

    Containers::String out;
    Error redirectError{&out};
    a.setPerVertexJointCount(3, 2);
    b.setPerVertexJointCount(4);
    b.setPerVertexJointCount(3, 3);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled\n"
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): expected at most 3 per-vertex joints, got 4\n"
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): expected at most 2 secondary per-vertex joints, got 3\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setUniformUniformBuffersEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)};

    /* This should work fine */
    shader.setViewportSize({});

    Containers::String out;
    Error redirectError{&out};
    shader
        /* setPerVertexJointCount() works on both UBOs and classic */
        .setTransformationProjectionMatrix({})
        .setTextureMatrix({})
        .setTextureLayer({})
        .setObjectId({})
        .setColor({})
        .setWireframeColor({})
        .setWireframeWidth({})
        .setColorMapTransformation({}, {})
        .setSmoothness({})
        .setJointMatrices({})
        .setJointMatrix(0, {})
        .setPerInstanceJointCount(0);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL2D::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setTextureMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setTextureLayer(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setObjectId(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setWireframeColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setWireframeWidth(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL2D::setSmoothness(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL2D::setJointMatrices(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL2D::setJointMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled\n");
}

void MeshVisualizerGLTest::setUniformUniformBuffersEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)};

    /* This should work fine */
    shader.setViewportSize({});

    Containers::String out;
    Error redirectError{&out};
    shader
        /* setPerVertexJointCount() works on both UBOs and classic */
        .setProjectionMatrix({})
        .setTransformationMatrix({})
        .setTextureMatrix({})
        .setTextureLayer({})
        .setObjectId({})
        .setColor({})
        .setWireframeColor({})
        .setWireframeWidth({})
        .setColorMapTransformation({}, {})
        .setSmoothness({})
        .setJointMatrices({})
        .setJointMatrix(0, {})
        .setPerInstanceJointCount(0);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL3D::setProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::setTransformationMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setTextureMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setTextureLayer(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setObjectId(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setWireframeColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setWireframeWidth(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::setSmoothness(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::setJointMatrices(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::setJointMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled\n");

    out = {};

    #ifndef MAGNUM_TARGET_WEBGL
    shader
        .setNormalMatrix({})
        .setLineWidth({})
        .setLineLength({});
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL3D::setNormalMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::setLineWidth(): the shader was created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::setLineLength(): the shader was created with uniform buffers enabled\n");
    #endif
}

void MeshVisualizerGLTest::bindBufferUniformBuffersNotEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Buffer buffer;
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)};

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
        "Shaders::MeshVisualizerGL2D::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL2D::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL2D::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL2D::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}

void MeshVisualizerGLTest::bindBufferUniformBuffersNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Buffer buffer;
    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)};

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
          .bindJointBuffer(buffer)
          .bindJointBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL3D::bindProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::bindProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::bindTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::bindTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL3D::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::bindJointBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::MeshVisualizerGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::bindObjectIdTextureInvalid2D() {
    auto&& data = BindObjectIdTextureInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags2D & MeshVisualizerGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(data.flags2D)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindObjectIdTexture(texture);
    CORRADE_COMPARE(out, data.message);
}

void MeshVisualizerGLTest::bindObjectIdTextureInvalid3D() {
    auto&& data = BindObjectIdTextureInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags3D & MeshVisualizerGL3D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags3D)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindObjectIdTexture(texture);
    CORRADE_COMPARE(out, data.message);
}

void MeshVisualizerGLTest::bindObjectIdTextureArrayInvalid2D() {
    auto&& data = BindObjectIdTextureArrayInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2DArray textureArray;
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(data.flags2D)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindObjectIdTexture(textureArray);
    CORRADE_COMPARE(out, data.message);
}

void MeshVisualizerGLTest::bindObjectIdTextureArrayInvalid3D() {
    auto&& data = BindObjectIdTextureArrayInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    GL::Texture2DArray textureArray;
    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags3D)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindObjectIdTexture(textureArray);
    CORRADE_COMPARE(out, data.message);
}
#endif

void MeshVisualizerGLTest::setWireframeNotEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    /* The constructor asserts for at least some feature being enabled (which
       is just wireframe in case of 2D), so fake it with a NoCreate */
    MeshVisualizerGL2D shader{NoCreate};
    shader
        .setColor({});

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setColor(): the shader was not created with wireframe or object/vertex/primitive ID enabled\n");
    #else
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setColor(): the shader was not created with wireframe enabled\n");
    #endif

    out = {};
    shader
        .setWireframeColor({})
        .setWireframeWidth({})
        .setSmoothness({});

    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setWireframeColor(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizerGL::setWireframeWidth(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizerGL2D::setSmoothness(): the shader was not created with wireframe enabled\n");
}

void MeshVisualizerGLTest::setWireframeNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    /* The constructor asserts for at least some feature being enabled (which
       is just wireframe in case we're not on desktop or ES3.2), so fake it
       with a NoCreate */
    MeshVisualizerGL3D shader{NoCreate};
    shader
        .setColor({});

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setColor(): the shader was not created with wireframe or object/vertex/primitive ID enabled\n");
    #else
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setColor(): the shader was not created with wireframe enabled\n");
    #endif

    out = {};
    shader
        .setWireframeColor({})
        .setWireframeWidth({})
        .setSmoothness({});

    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setWireframeColor(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizerGL::setWireframeWidth(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizerGL3D::setSmoothness(): the shader was not created with wireframe or TBN direction enabled\n");
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setTextureMatrixNotEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::ObjectIdTexture)};

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureMatrix({});
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

void MeshVisualizerGLTest::setTextureMatrixNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::ObjectIdTexture)};

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureMatrix({});
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setTextureMatrix(): the shader was not created with texture transformation enabled\n");
}

void MeshVisualizerGLTest::setTextureLayerNotArray2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::ObjectIdTexture)};

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureLayer(37);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}

void MeshVisualizerGLTest::setTextureLayerNotArray3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::ObjectIdTexture)};

    Containers::String out;
    Error redirectError{&out};
    shader.setTextureLayer(37);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setTextureLayer(): the shader was not created with texture arrays enabled\n");
}

void MeshVisualizerGLTest::bindTextureTransformBufferNotEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    GL::Buffer buffer{GL::Buffer::TargetHint::Uniform};
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}

void MeshVisualizerGLTest::bindTextureTransformBufferNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    GL::Buffer buffer{GL::Buffer::TargetHint::Uniform};
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::ObjectIdTexture)};

    Containers::String out;
    Error redirectError{&out};
    shader.bindTextureTransformationBuffer(buffer)
          .bindTextureTransformationBuffer(buffer, 0, 16);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n"
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setObjectIdNotEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL2D shader{NoCreate};

    Containers::String out;
    Error redirectError{&out};
    shader.setObjectId({});
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL::setObjectId(): the shader was not created with object ID enabled\n");
}

void MeshVisualizerGLTest::setObjectIdNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL3D shader{NoCreate};

    Containers::String out;
    Error redirectError{&out};
    shader.setObjectId({});
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL::setObjectId(): the shader was not created with object ID enabled\n");
}

void MeshVisualizerGLTest::setColorMapNotEnabled2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Texture2D texture;
    MeshVisualizerGL2D shader{NoCreate};

    Containers::String out;
    Error redirectError{&out};
    shader.setColorMapTransformation({}, {})
        .bindColorMapTexture(texture);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled\n"
        "Shaders::MeshVisualizerGL::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled\n");
}

void MeshVisualizerGLTest::setColorMapNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Texture2D texture;
    MeshVisualizerGL3D shader{NoCreate};

    Containers::String out;
    Error redirectError{&out};
    shader.setColorMapTransformation({}, {})
        .bindColorMapTexture(texture);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled\n"
        "Shaders::MeshVisualizerGL::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled\n");
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::setTangentBitangentNormalNotEnabled3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe)};

    Containers::String out;
    Error redirectError{&out};
    shader.setNormalMatrix({})
        .setLineWidth({})
        .setLineLength({});
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL3D::setNormalMatrix(): the shader was not created with TBN direction enabled\n"
        "Shaders::MeshVisualizerGL3D::setLineWidth(): the shader was not created with TBN direction enabled\n"
        "Shaders::MeshVisualizerGL3D::setLineLength(): the shader was not created with TBN direction enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setWrongJointCountOrId2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* At least one visualization feature has to be enabled; disable GS so we
       don't need to check for it on ES */
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setJointCount(5, 1)};

    Containers::String out;
    Error redirectError{&out};
    /* Calling setJointMatrices() with less items is fine, tested in
       renderSkinningWireframe2D() */
    shader.setJointMatrices({{}, {}, {}, {}, {}, {}})
        .setJointMatrix(5, Matrix3{});
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL2D::setJointMatrices(): expected at most 5 items but got 6\n"
        "Shaders::MeshVisualizerGL2D::setJointMatrix(): joint ID 5 is out of range for 5 joints\n");
}

void MeshVisualizerGLTest::setWrongJointCountOrId3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    /* At least one visualization feature has to be enabled; disable GS so we
       don't need to check for it on ES */
    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setJointCount(5, 1)};

    Containers::String out;
    Error redirectError{&out};
    /* Calling setJointMatrices() with less items is fine, tested in
       renderSkinningWireframe3D() */
    shader.setJointMatrices({{}, {}, {}, {}, {}, {}})
        .setJointMatrix(5, Matrix4{});
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL3D::setJointMatrices(): expected at most 5 items but got 6\n"
        "Shaders::MeshVisualizerGL3D::setJointMatrix(): joint ID 5 is out of range for 5 joints\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::setWrongDrawOffset2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setMaterialCount(2)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
}

void MeshVisualizerGLTest::setWrongDrawOffset3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setMaterialCount(2)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::MeshVisualizerGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
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
template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderDefaultsWireframe2D() {
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
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
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(16));

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::Wireframe|flag)};

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL("Defaults don't work for wireframe as line width is derived from viewport size.");
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /** @todo make this unnecessary */
    shader.setViewportSize(Vector2{RenderSize});

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe2D.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.082f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderDefaultsWireframe3D() {
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::icosphereSolid(1));

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe|flag)};

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(sphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL("Defaults don't work for wireframe as line width is derived from viewport size.");
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe3D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /** @todo make this unnecessary */
    shader.setViewportSize(Vector2{RenderSize});

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(sphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe3D.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.06f}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderDefaultsObjectId2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
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

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(16));

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::ObjectId|flag)};
    shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-objectid2D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 24.67f, 0.11f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderDefaultsObjectId3D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
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

    GL::Mesh icosphere = MeshTools::compile(Primitives::icosphereSolid(0));

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::ObjectId|flag)};
    shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(icosphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(icosphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-objectid3D.tga"),
        /* SwiftShader has a few rounding errors on edges and off-by-two
           pixels */
        (DebugTools::CompareImageToFile{_manager, 24.67f, 2.55f}));
}

template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderDefaultsInstancedObjectId2D() {
    auto&& data = RenderInstancedObjectIdDefaultsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
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

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::InstancedObjectId|flag)};
    shader.bindColorMapTexture(colorMapTexture);

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-instancedobjectid2D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 150.67f, 0.45f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderDefaultsInstancedObjectId3D() {
    auto&& data = RenderInstancedObjectIdDefaultsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
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

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::InstancedObjectId|flag)};
    shader.bindColorMapTexture(colorMapTexture);

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(icosphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(icosphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-instancedobjectid3D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 150.67f, 0.165f}));
}

template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderDefaultsVertexId2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* On SwiftShader gl_VertexID doesn't work in this case, skipping */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(16));

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::VertexId|flag)};
    shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-vertexid2D.tga"),
        /* Minor differences on NVidia */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.022f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderDefaultsVertexId3D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* On SwiftShader gl_VertexID doesn't work in this case, skipping */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    GL::Mesh icosphere = MeshTools::compile(Primitives::icosphereSolid(0));

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::VertexId|flag)};
    shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(icosphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(icosphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-vertexid3D.tga"),
        /* Minor differences on NVidia */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.018f}));
}

template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderDefaultsPrimitiveId2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

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

    MeshVisualizerGL2D::Flags flags;
    #ifdef MAGNUM_TARGET_WEBGL
    flags = MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId;
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isVersionSupported(GL::Version::GL320))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES320))
    #endif
    {
        Debug{} << "Using primitive ID from vertex ID";
        flags = MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId;
    }
    else flags = MeshVisualizerGL2D::Flag::PrimitiveId;
    #endif

    Trade::MeshData circleData = Primitives::circle2DSolid(16);
    if(flags >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId)
        circleData = MeshTools::duplicate(MeshTools::generateIndices(circleData));

    GL::Mesh circle = MeshTools::compile(circleData);

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(flags|flag)};
    shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-primitiveid2D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 76.67f, 0.23f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderDefaultsPrimitiveId3D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

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

    MeshVisualizerGL3D::Flags flags;
    #ifdef MAGNUM_TARGET_WEBGL
    flags = MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId;
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isVersionSupported(GL::Version::GL320))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES320))
    #endif
    {
        Debug{} << "Using primitive ID from vertex ID";
        flags = MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId;
    }
    else flags = MeshVisualizerGL3D::Flag::PrimitiveId;
    #endif

    Trade::MeshData icosphereData = Primitives::icosphereSolid(0);
    if(flags >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId)
        icosphereData = MeshTools::duplicate(icosphereData);

    GL::Mesh icosphere = MeshTools::compile(icosphereData);

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(flags|flag)};
    shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(icosphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(icosphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-primitiveid3D.tga"),
        /* SwiftShader has a few rounding errors on edges */
        (DebugTools::CompareImageToFile{_manager, 88.34f, 0.071f}));
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderDefaultsTangentBitangentNormal() {
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(4, 8,
        Primitives::UVSphereFlag::Tangents));

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection|flag)};
    /** @todo make this unnecessary */
    shader.setViewportSize(Vector2{RenderSize});

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.draw(sphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/defaults-tbn.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.06f}));
}
#endif

template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderWireframe2D() {
    auto&& data = RenderWireframeData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif
    #endif

    const Trade::MeshData circleData = Primitives::circle2DSolid(16);

    GL::Mesh circle{NoCreate};
    if(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader) {
        /* Duplicate the vertices. The circle primitive is a triangle fan, so
           we first need to turn it into indexed triangles. */
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
            circle.addVertexBuffer(Utility::move(vertexId), 0, MeshVisualizerGL2D::VertexIndex{});
        }
    } else circle = MeshTools::compile(circleData);

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(data.flags|MeshVisualizerGL2D::Flag::Wireframe|flag)};
    shader.setViewportSize(Vector2{RenderSize});

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .setWireframeWidth(data.width)
            .setSmoothness(data.smoothness)
            .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
            .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .setWireframeWidth(data.width)
            .setSmoothness(data.smoothness)
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

    {
        CORRADE_EXPECT_FAIL_IF(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader,
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
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.file}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test it's not *too* off, at least */
    if(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader) {
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
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.fileXfail}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderWireframe3D() {
    auto&& data = RenderWireframeData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif
    #endif

    const Trade::MeshData sphereData = Primitives::icosphereSolid(1);

    GL::Mesh sphere{NoCreate};
    if(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader) {
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
            sphere.addVertexBuffer(Utility::move(vertexId), 0, MeshVisualizerGL3D::VertexIndex{});
        }
    } else sphere = MeshTools::compile(sphereData);

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags|MeshVisualizerGL3D::Flag::Wireframe|flag)};
    shader.setViewportSize(Vector2{RenderSize});

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .setWireframeWidth(data.width)
            .setSmoothness(data.smoothness)
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
                .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf)
                .setWireframeWidth(data.width)
                .setSmoothness(data.smoothness)
        }};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
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

    {
        CORRADE_EXPECT_FAIL_IF(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader,
            "Line width is currently not configurable w/o geometry shader.");
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels. On a
           GS, if GL_NV_shader_noperspective_interpolation is not supported,
           the artifacts are bigger. */
        Float maxThreshold = 170.0f, meanThreshold = 0.327f;
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(!(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
            meanThreshold = 2.166f;
        #endif
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.file}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test it's not *too* off, at least */
    if(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader) {
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
            _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
            Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.fileXfail}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }
}

#ifndef MAGNUM_TARGET_GLES2
template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderObjectVertexPrimitiveId2D() {
    auto&& data = RenderObjectVertexPrimitiveIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags2D & (MeshVisualizerGL2D::Flag::TextureTransformation|MeshVisualizerGL2D::Flag::TextureArrays) ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags2D & MeshVisualizerGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags2D & MeshVisualizerGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    /* Interestingly for PrimitiveIdFromVertexId gl_VertexID in SwiftShader
       works -- maybe it works only for nonindexed triangle draws? */
    if(data.flags2D & MeshVisualizerGL2D::Flag::VertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags2D & MeshVisualizerGL2D::Flag::PrimitiveId && !(data.flags2D >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags2D & MeshVisualizerGL2D::Flag::Wireframe && !(data.flags2D & MeshVisualizerGL2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(data.flags2D & MeshVisualizerGL2D::Flag::Wireframe && !(data.flags2D & MeshVisualizerGL2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif
    #endif

    Primitives::Circle2DFlags circleFlags;
    if(data.flags2D & MeshVisualizerGL2D::Flag::ObjectIdTexture)
        circleFlags |= Primitives::Circle2DFlag::TextureCoordinates;
    Trade::MeshData circleData = Primitives::circle2DSolid(16, circleFlags);

    /* Add the instanced Object ID data even if visualizing just uniform object
       ID, to test the attribute isn't accidentally accessed always */
    if(data.flags2D & MeshVisualizerGL2D::Flag::ObjectId) {
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
    if(data.flags2D >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId)
        circleData = MeshTools::generateIndices(circleData);
    if(data.flags2D >= MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId ||
       data.flags2D & MeshVisualizerGL2D::Flag::NoGeometryShader) {
        if(circleData.primitive() != MeshPrimitive::Triangles)
            circleData = MeshTools::generateIndices(circleData);
        circleData = MeshTools::duplicate(circleData);
    }

    GL::Mesh circle = MeshTools::compile(circleData);

    MeshVisualizerGL2D::Flags flags = data.flags2D|flag;
    if(flag & MeshVisualizerGL2D::Flag::UniformBuffers && (data.flags2D & MeshVisualizerGL2D::Flag::TextureArrays) && !(data.flags2D & MeshVisualizerGL2D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= MeshVisualizerGL2D::Flag::TextureTransformation;
    }
    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(flags)};
    shader
        /* Shouldn't assert (nor warn) when wireframe is not enabled */
        .setViewportSize(Vector2{RenderSize})
        .bindColorMapTexture(_colorMapTexture);

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags2D >= MeshVisualizerGL2D::Flag::ObjectIdTexture) {
        const UnsignedShort imageData[]{1, 0, 0, 7};
        const UnsignedShort imageDataFlipped[]{7, 0, 0, 1};
        ImageView2D image{PixelFormat::R16UI, {2, 2}, data.flip ? imageDataFlipped : imageData};

        if(data.flags2D & MeshVisualizerGL2D::Flag::TextureArrays) {
            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, {image.size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, image);
            shader.bindObjectIdTexture(textureArray);
            if(!(flag & MeshVisualizerGL2D::Flag::UniformBuffers) && data.layer != 0)
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

    if(flag == MeshVisualizerGL2D::Flag{}) {
        /* Remove blue so it's clear the (wireframe) background and mapped ID
           colors got mixed */
        shader.setColor(0xffff00_rgbf)
            .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}));
        /* OTOH the wireframe color should stay at full channels, not mixed */
        if(data.flags2D & MeshVisualizerGL2D::Flag::Wireframe)
            shader.setWireframeColor(0xffffff_rgbf);
        /* For object ID we set a base ID to verify the uniform and instanced
           ID get summed. */
        if(data.flags2D & MeshVisualizerGL2D::Flag::ObjectId)
            shader.setObjectId(8);
        /* For vertex ID we don't want any repeat/wraparound as that causes
           disruptions in the gradient and test failures. There's 17 vertices
           also. */
        if(data.flags2D & MeshVisualizerGL2D::Flag::VertexId)
            shader.setColorMapTransformation(1.0f, -1.0f/17.0f);
        /* For object/primitive ID there's no gradient so a wraparound is okay.
           For the object ID this should cover the second half of the colormap
           (due to the uniform object ID), in reverse order; for primitive ID
           the whole colormap due to the repeat wrapping */
        else
            shader.setColorMapTransformation(0.5f, -1.0f/16.0f);
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader.draw(circle);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        /* See above for comments */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform2D{}
                .setObjectId(8)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layer)
        }};
        MeshVisualizerMaterialUniform materialUniformData[1];
        materialUniformData->setColor(0xffff00_rgbf);
        if(data.flags2D & MeshVisualizerGL2D::Flag::Wireframe)
            materialUniformData->setWireframeColor(0xffffff_rgbf);
        if(data.flags2D & MeshVisualizerGL2D::Flag::VertexId)
            materialUniformData->setColorMapTransformation(1.0f, -1.0f/17.0f);
        else
            materialUniformData->setColorMapTransformation(0.5f, -1.0f/16.0f);
        GL::Buffer materialUniform{materialUniformData};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & MeshVisualizerGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(circle);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.file2D}),
        /* AMD has slight off-by-one errors compared to Intel, SwiftShader a
           bit more */
        (DebugTools::CompareImageToFile{_manager, 4.67f, 0.141f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderObjectVertexPrimitiveId3D() {
    auto&& data = RenderObjectVertexPrimitiveIdData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags3D & (MeshVisualizerGL3D::Flag::TextureTransformation|MeshVisualizerGL3D::Flag::TextureArrays) ? 5 : 4))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags3D & MeshVisualizerGL3D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags2D & MeshVisualizerGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    /* Interestingly for PrimitiveIdFromVertexId gl_VertexID in SwiftShader
       works -- maybe it works only for nonindexed triangle draws? */
    if(data.flags3D & MeshVisualizerGL3D::Flag::VertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags3D & MeshVisualizerGL3D::Flag::PrimitiveId && !(data.flags3D >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId) &&
        #ifndef MAGNUM_TARGET_GLES
        !GL::Context::current().isVersionSupported(GL::Version::GL320)
        #else
        !GL::Context::current().isVersionSupported(GL::Version::GLES320)
        #endif
    ) CORRADE_SKIP("gl_PrimitiveID not supported.");

    #ifndef MAGNUM_TARGET_GLES
    if(data.flags3D & MeshVisualizerGL3D::Flag::Wireframe && !(data.flags3D & MeshVisualizerGL3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(data.flags3D & MeshVisualizerGL3D::Flag::Wireframe && !(data.flags3D & MeshVisualizerGL3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif
    #endif

    Primitives::UVSphereFlags sphereFlags;
    if(data.flags2D & MeshVisualizerGL2D::Flag::ObjectIdTexture)
        sphereFlags |= Primitives::UVSphereFlag::TextureCoordinates;
    Trade::MeshData sphereData = Primitives::uvSphereSolid(4, 8, sphereFlags);

    /* Add the instanced Object ID data even if visualizing just uniform object
       ID, to test the attribute isn't accidentally accessed always */
    if(data.flags3D & MeshVisualizerGL3D::Flag::ObjectId) {
        Containers::Array<UnsignedInt> ids{sphereData.indexCount()/3};
        /* Each two faces share the same ID */
        for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i/2;
        sphereData = MeshTools::combineFaceAttributes(
            sphereData, {
                Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                    Containers::arrayView(ids)}
            });
    }

    /* Duplicate the data if using primitive ID from vertex ID or if geometry
       shader is disabled */
    if(data.flags3D >= MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId ||
       data.flags3D & MeshVisualizerGL3D::Flag::NoGeometryShader)
        sphereData = MeshTools::duplicate(sphereData);

    GL::Mesh sphere = MeshTools::compile(sphereData);

    MeshVisualizerGL3D::Flags flags = data.flags3D|flag;
    if(flag & MeshVisualizerGL3D::Flag::UniformBuffers && (data.flags3D & MeshVisualizerGL3D::Flag::TextureArrays) && !(data.flags3D & MeshVisualizerGL3D::Flag::TextureTransformation)) {
        CORRADE_INFO("Texture arrays currently require texture transformation if UBOs are used, enabling implicitly.");
        flags |= MeshVisualizerGL3D::Flag::TextureTransformation;
    }
    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(flags)};
    shader
        /* Shouldn't assert (nor warn) when wireframe is not enabled */
        .setViewportSize(Vector2{RenderSize})
        .bindColorMapTexture(_colorMapTexture);

    GL::Texture2D texture{NoCreate};
    GL::Texture2DArray textureArray{NoCreate};
    if(data.flags3D >= MeshVisualizerGL3D::Flag::ObjectIdTexture) {
        const UnsignedShort imageData[]{1, 0, 0, UnsignedShort(sphere.count()/6 - 1)};
        const UnsignedShort imageDataFlipped[]{UnsignedShort(sphere.count()/6 - 1), 0, 0, 1};
        ImageView2D image{PixelFormat::R16UI, {2, 2}, data.flip ? imageDataFlipped : imageData};

        if(data.flags2D & MeshVisualizerGL2D::Flag::TextureArrays) {
            textureArray = GL::Texture2DArray{};
            textureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, {image.size(), data.layer + 1})
                .setSubImage(0, {0, 0, data.layer}, image);
            shader.bindObjectIdTexture(textureArray);
            if(!(flag & MeshVisualizerGL3D::Flag::UniformBuffers) && data.layer != 0)
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

    if(flag == MeshVisualizerGL3D::Flag{}) {
        /* Remove blue so it's clear the wireframe background and mapped ID
           colors got mixed */
        shader.setColor(0xffff00_rgbf)
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(-15.0_degf)*
                Matrix4::rotationX(15.0_degf))
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f));
        /* OTOH the wireframe color should stay at full channels, not mixed */
        if(data.flags3D & MeshVisualizerGL3D::Flag::Wireframe)
            shader.setWireframeColor(0xffffff_rgbf);
        /* For object ID we set a base ID to verify the uniform and instanced
           ID get summed. */
        if(data.flags3D & MeshVisualizerGL3D::Flag::ObjectId)
            shader.setObjectId(sphere.count()/6);
        /* For vertex ID we don't want any repeat/wraparound as that causes
           disruptions in the gradient and test failures */
        if(data.flags3D & MeshVisualizerGL3D::Flag::VertexId)
            shader.setColorMapTransformation(1.0f, -1.0f/sphereData.vertexCount());
        /* For object/primitive ID there's no gradient so a wraparound is okay.
           For the object ID this should cover the second half of the colormap
           (due to the uniform object ID), in reverse order; for primitive ID
           the whole colormap due to the repeat wrapping */
        else
            shader.setColorMapTransformation(0.5f, -1.0f/(sphere.count()/3));
        if(data.textureTransformation != Matrix3{})
            shader.setTextureMatrix(data.textureTransformation);
        shader.draw(sphere);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        /* See above for comments */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
                .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform3D{}
                .setObjectId(sphere.count()/6)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(data.textureTransformation)
                .setLayer(data.layer)
        }};
        MeshVisualizerMaterialUniform materialUniformData[1];
        materialUniformData->setColor(0xffff00_rgbf);
        if(data.flags3D & MeshVisualizerGL3D::Flag::Wireframe)
            materialUniformData->setWireframeColor(0xffffff_rgbf);
        if(data.flags3D & MeshVisualizerGL3D::Flag::VertexId)
            materialUniformData->setColorMapTransformation(1.0f, -1.0f/sphereData.vertexCount());
        else
            materialUniformData->setColorMapTransformation(0.5f, -1.0f/(sphere.count()/3));
        GL::Buffer materialUniform{materialUniformData};
        /* Also take into account the case when texture transform needs to be
           enabled for texture arrays, so not data.flags but flags */
        if(flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(sphere);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
    if(data.flags3D & MeshVisualizerGL3D::Flag::Wireframe && !GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>()) {
        /* SwiftShader has a bit more rounding errors */
        maxThreshold = 238.0f;
        meanThreshold = 1.957f;
    }
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.file3D}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::renderWireframe3DPerspective() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
    #endif

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::Wireframe)};
    shader.setWireframeWidth(8.0f)
        .setWireframeColor(0xff0000_rgbf)
        .setViewportSize(Vector2{RenderSize})
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
        GL::Extensions::NV::shader_noperspective_interpolation::string() << "not supported.");
    #endif

    /* Slight rasterization differences on AMD. */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/wireframe-perspective.tga"),
        (DebugTools::CompareImageToFile{_manager, 0.667f, 0.002f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderTangentBitangentNormal() {
    auto&& data = RenderTangentBitangentNormalData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
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
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
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
            Shaders::MeshVisualizerGL3D::Position{},
            sizeof(Vector4), /* conditionally added below */
            sizeof(Vector3), /* conditionally added below */
            Shaders::MeshVisualizerGL3D::Normal{});
    if(data.flags & MeshVisualizerGL3D::Flag::BitangentFromTangentDirection && !data.skipBitagnentEvenIfEnabledInFlags)
        mesh.addVertexBuffer(vertices, 0,
            sizeof(Vector3),
            Shaders::MeshVisualizerGL3D::Tangent4{},
            sizeof(Vector3),
            sizeof(Vector3));
    else if(data.flags & MeshVisualizerGL3D::Flag::TangentDirection)
        mesh.addVertexBuffer(vertices, 0,
            sizeof(Vector3),
            Shaders::MeshVisualizerGL3D::Tangent{}, sizeof(Float), sizeof(Vector3),
            sizeof(Vector3));
    if(data.flags & MeshVisualizerGL3D::Flag::BitangentDirection && !data.skipBitagnentEvenIfEnabledInFlags)
        mesh.addVertexBuffer(vertices, 0,
            sizeof(Vector3),
            sizeof(Vector4),
            Shaders::MeshVisualizerGL3D::Bitangent{},
            sizeof(Vector3));

    Matrix4 transformation = Matrix4::translation({0.0f, 0.5f, -3.5f})*
        Matrix4::rotationX(-60.0_degf)*
        Matrix4::scaling(Vector3::yScale(1.5f));

    if(data.secondPassFlags) {
        MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
            .setFlags(data.secondPassFlags)};
        shader
            /** @todo make this unnecessary */
            .setViewportSize(Vector2{RenderSize})
            .setTransformationMatrix(transformation)
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .draw(mesh);
    }

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags|flag)};
    /** @todo make this unnecessary */
    shader.setViewportSize(Vector2{RenderSize});
    if(data.flags & (MeshVisualizerGL3D::Flag::PrimitiveId|MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::VertexId))
        shader.bindColorMapTexture(_colorMapTexture);

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader.setTransformationMatrix(transformation)
            .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
            .setNormalMatrix(transformation.normalMatrix()*data.multiply)
            .setSmoothness(data.smoothness)
            .setLineLength(data.lineLength)
            .setLineWidth(data.lineWidth);
        if(data.flags & MeshVisualizerGL3D::Flag::Wireframe)
            shader
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf);
        if(data.flags & MeshVisualizerGL3D::Flag::PrimitiveId)
            shader.setColorMapTransformation(1.0f/512.0f, 0.5f);
        else if(data.flags & MeshVisualizerGL3D::Flag::VertexId)
            shader.setColorMapTransformation(1.0f/8.0f, 1.0f/4.0f);
        else if(data.flags & MeshVisualizerGL3D::Flag::ObjectId)
            shader.setObjectId(127);
        shader.draw(mesh);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
                .setProjectionMatrix(Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f))
        }};
        GL::Buffer transformationUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationUniform3D{}
                .setTransformationMatrix(transformation)
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerDrawUniform3D{}
                .setNormalMatrix(transformation.normalMatrix()*data.multiply)
                .setObjectId(127)
        }};
        MeshVisualizerMaterialUniform materialUniformData[1];
        (*materialUniformData)
            .setSmoothness(data.smoothness)
            .setLineLength(data.lineLength)
            .setLineWidth(data.lineWidth);
        if(data.flags & MeshVisualizerGL3D::Flag::Wireframe)
            (*materialUniformData)
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf);
        if(data.flags & MeshVisualizerGL3D::Flag::PrimitiveId)
            materialUniformData->setColorMapTransformation(1.0f/512.0f, 0.5f);
        else if(data.flags & MeshVisualizerGL3D::Flag::VertexId)
            materialUniformData->setColorMapTransformation(1.0f/8.0f, 1.0f/4.0f);
        GL::Buffer materialUniform{materialUniformData};
        shader
            .bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(mesh);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
        maxThreshold = 58.0f;
        meanThreshold = 1.547f;
    }
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.file}),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderSkinningWireframe2D() {
    auto&& data = RenderSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.jointCount ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
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

    /* Same as in FlatGLTest::renderSkinning2D(), except that the shared
       vertices are duplicated in order to work with GS-less wireframe */
    struct Vertex {
        Vector2 position;
        UnsignedInt jointIds[3];
        Float weights[3];
    } vertices[]{
        /* Top right corner gets moved to the right and up, top left just up,
           bottom right just right, bottom left corner gets slightly scaled.

           5--4 1
           | / /|
           |/ / |
           3 2--0 */
        {{ 1.0f, -1.0f}, {0, 2, 0}, {0.25f, 0.0f, 0.75f}},
        {{ 1.0f,  1.0f}, {1, 0, 0}, {0.5f, 0.5f, 0.0f}},
        {{-1.0f, -1.0f}, {3, 4, 4}, {0.5f, 0.25f, 0.25f}},
        {{-1.0f, -1.0f}, {3, 4, 4}, {0.5f, 0.25f, 0.25f}},
        {{ 1.0f,  1.0f}, {1, 0, 0}, {0.5f, 0.5f, 0.0f}},
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
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers && data.jointCount > Containers::arraySize(jointMatrices))
        CORRADE_SKIP("Uploading an uniform buffer smaller than the size hardcoded in the shader is an error in WebGL.");
    #endif

    GL::Buffer buffer{vertices};

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(6);
    mesh.addVertexBuffer(buffer, 0, sizeof(Vertex), GL::DynamicAttribute{MeshVisualizerGL2D::Position{}});
    for(auto&& attribute: data.attributes)
        mesh.addVertexBuffer(buffer, 2*4 + attribute.first(), sizeof(Vertex), attribute.second());

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(data.flags2D|flag|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    if(data.setDynamicPerVertexJointCount)
        shader.setPerVertexJointCount(data.dynamicPerVertexJointCount, data.dynamicSecondaryPerVertexJointCount);

    if(flag == MeshVisualizerGL2D::Flag{}) {
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
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .draw(mesh);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform2D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf)
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
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.expected}),
        /* Minor differences on ARM Mali */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.012f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderSkinningWireframe3D() {
    auto&& data = RenderSkinningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(data.jointCount && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.jointCount ? 5 : 4))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
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

    /* Same as in FlatGLTest::renderSkinning3D(), except that the shared
       vertices are duplicated in order to work with GS-less wireframe */
    struct Vertex {
        Vector3 position;
        UnsignedInt jointIds[3];
        Float weights[3];
    } vertices[]{
        /* Top right corner gets moved to the right and up, top left just up,
           bottom right just right, bottom left corner gets slightly scaled.

           5--4 1
           | / /|
           |/ / |
           3 2--0 */
        {{ 1.0f, -1.0f, 0.0f}, {0, 2, 0}, {0.25f, 0.0f, 0.75f}},
        {{ 1.0f,  1.0f, 0.0f}, {1, 0, 0}, {0.5f, 0.5f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {3, 4, 4}, {0.5f, 0.25f, 0.25f}},
        {{-1.0f, -1.0f, 0.0f}, {3, 4, 4}, {0.5f, 0.25f, 0.25f}},
        {{ 1.0f,  1.0f, 0.0f}, {1, 0, 0}, {0.5f, 0.5f, 0.0f}},
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
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers && data.jointCount > Containers::arraySize(jointMatrices))
        CORRADE_SKIP("Uploading an uniform buffer smaller than the size hardcoded in the shader is an error in WebGL.");
    #endif

    GL::Buffer buffer{vertices};

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(6);
    mesh.addVertexBuffer(buffer, 0, sizeof(Vertex), GL::DynamicAttribute{MeshVisualizerGL3D::Position{}});
    for(auto&& attribute: data.attributes)
        mesh.addVertexBuffer(buffer, 3*4 + attribute.first(), sizeof(Vertex), attribute.second());

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(data.flags3D|flag|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount)};
    if(data.setDynamicPerVertexJointCount)
        shader.setPerVertexJointCount(data.dynamicPerVertexJointCount, data.dynamicSecondaryPerVertexJointCount);

    if(flag == MeshVisualizerGL3D::Flag{}) {
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
            .setTransformationMatrix(Matrix4::scaling(Vector3{0.5f}))
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .draw(mesh);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf)
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
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.expected}),
        /* Minor differences on ARM Mali */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.012f}));
}
#endif

template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderInstanced2D() {
    auto&& data = RenderInstancedData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & MeshVisualizerGL2D::Flag::TextureTransformation ? 3 : 4))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if((data.flags & MeshVisualizerGL2D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL2D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* Interestingly enough, on SwiftShader it only fails in case UBOs are
       used. Dafuq is this buggy crap?! */
    if(data.flags & MeshVisualizerGL2D::Flag::VertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
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

    Trade::MeshData circleData = Primitives::circle2DSolid(8, Primitives::Circle2DFlag::TextureCoordinates);

    GL::Mesh circle{NoCreate};
    if(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader) {
        /* Duplicate the vertices. The circle primitive is a triangle fan, so
           we first need to turn it into indexed triangles. */
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
            circle.addVertexBuffer(Utility::move(vertexId), 0, MeshVisualizerGL2D::VertexIndex{});
        }
    } else circle = MeshTools::compile(circleData);

    /* Three circles, each in a different location */
    struct {
        Matrix3 transformation;
        Vector3 textureOffsetLayer;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix3::translation({-1.25f, -1.25f}),
            /* 6 gets added to objectId, wrapping it around to 0, making it
               visually close to the multidraw test */
            {0.0f, 0.0f, 0.0f}, 6},
        {Matrix3::translation({ 1.25f, -1.25f}),
            {1.0f, 0.0f, 1.0f}, 10},
        {Matrix3::translation({ 0.00f,  1.25f}),
            #ifndef MAGNUM_TARGET_GLES2
            data.flags & MeshVisualizerGL2D::Flag::TextureArrays ? Vector3{0.0f, 0.0f, 2.0f} :
            #endif
            Vector3{0.5f, 1.0f, 2.0f}, 14},
    };

    circle
        .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
            MeshVisualizerGL2D::TransformationMatrix{},
            #ifndef MAGNUM_TARGET_GLES2
            MeshVisualizerGL2D::TextureOffsetLayer{},
            MeshVisualizerGL2D::ObjectId{}
            #else
            4*4
            #endif
        )
        .setInstanceCount(3);

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::InstancedTransformation|data.flags|flag)};
    shader.setViewportSize(Vector2{RenderSize});
    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & (MeshVisualizerGL2D::Flag::VertexId|MeshVisualizerGL2D::Flag::ObjectId))
        shader.bindColorMapTexture(_colorMapTexture);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= MeshVisualizerGL2D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & MeshVisualizerGL2D::Flag::TextureArrays) {
            /* 2 extra slices as a base offset, each slice has half height,
               second slice has the data in the right half */
            const UnsignedShort imageData[]{
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,

                5, 0, 0, 0,
                0, 5, 0, 0,

                0, 0, 3, 0,
                0, 0, 0, 3,

                1, 0, 0, 0,
                0, 1, 0, 0
            };
            ImageView3D image{PixelFormat::R16UI, {4, 2, 5}, imageData};

            objectIdTextureArray = GL::Texture2DArray{};
            objectIdTextureArray.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTextureArray);
        } else {
            /* First is taken from bottom left, second from bottom right, third
               from top center */
            const UnsignedShort imageData[]{
                5, 0, 3, 0,
                0, 5, 0, 3,
                0, 1, 0, 0,
                0, 0, 1, 0
            };
            ImageView2D image{PixelFormat::R16UI, {4, 4}, imageData};

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

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader
            .setColor(0xffffcc_rgbf)
            .setTransformationProjectionMatrix(
                Matrix3::projection({2.1f, 2.1f})*
                Matrix3::scaling(Vector2{0.4f}));

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & MeshVisualizerGL2D::Flag::TextureTransformation)
            shader.setTextureMatrix(Matrix3::scaling(
                /* Slices of the texture array have half the height */
                data.flags & MeshVisualizerGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) : Vector2{0.5f}
            ));
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & MeshVisualizerGL2D::Flag::TextureArrays)
            shader.setTextureLayer(2); /* base offset */
        #endif

        if(data.flags & MeshVisualizerGL2D::Flag::Wireframe)
            shader.setWireframeColor(0xcc0000_rgbf);

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & MeshVisualizerGL2D::Flag::VertexId)
            shader.setColorMapTransformation(0.5f/circleData.vertexCount(), 1.0f/circleData.vertexCount());
        else if(data.flags & MeshVisualizerGL2D::Flag::ObjectId) {
            /* To make this visually close to the multidraw test */
            shader
                .setObjectId(6)
                .setColorMapTransformation(0.5f/12, 1.0f/12);
        }
        #endif

        shader.draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform2D{}
                .setObjectId(6)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(Matrix3::scaling(
                    #ifndef MAGNUM_TARGET_GLES2
                    /* Slices of the texture array have half the height */
                    data.flags & MeshVisualizerGL2D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                    #endif
                    Vector2{0.5f}))
                .setLayer(2) /* base offset */
        }};
        MeshVisualizerMaterialUniform materialUniformData[1];
        (*materialUniformData)
            .setColor(0xffffcc_rgbf)
            .setWireframeColor(0xcc0000_rgbf);
        if(data.flags & MeshVisualizerGL2D::Flag::VertexId)
            materialUniformData->setColorMapTransformation(0.5f/circleData.vertexCount(), 1.0f/circleData.vertexCount());
        else if(data.flags & MeshVisualizerGL2D::Flag::ObjectId)
            materialUniformData->setColorMapTransformation(0.5f/12, 1.0f/12);
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialUniformData};
        if(data.flags & MeshVisualizerGL2D::Flag::TextureTransformation)
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
        First circle should be lower left, second lower right, third up center.

        -   Wireframe all looking the same (the only instanced thing that can
            differ is the transformation
        -   Vertex ID should all have the full color map range
        -   Object ID should be visually close to the multidraw case, except
            that each circle is just a single color
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderInstanced3D() {
    auto&& data = RenderInstancedData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) <  (data.flags & MeshVisualizerGL3D::Flag::TextureTransformation ? 5 : 4))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(((data.flags & MeshVisualizerGL3D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)) || (data.flags & (MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection))) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL3D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL3D::Flag::TextureArrays) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /* Interestingly enough, on SwiftShader it only fails in case UBOs are
       used. Dafuq is this buggy crap?! */
    if(data.flags & MeshVisualizerGL3D::Flag::VertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
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

    Trade::MeshData sphereData = Primitives::uvSphereSolid(2, 4, Primitives::UVSphereFlag::TextureCoordinates|Primitives::UVSphereFlag::Tangents);

    GL::Mesh sphere{NoCreate};
    if(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader) {
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
            sphere.addVertexBuffer(Utility::move(vertexId), 0, MeshVisualizerGL3D::VertexIndex{});
        }
    } else sphere = MeshTools::compile(sphereData);

    /* Three spheres, each in a different location. To test normal matrix
       concatenation, everything is rotated 90° on Y, thus X is now -Z and Z is
       now X. */
    struct {
        Matrix4 transformation;
        Matrix3x3 normal;
        Vector3 textureOffsetLayer;
        UnsignedInt objectId;
    } instanceData[] {
        {Matrix4::translation(Math::gather<'z', 'y', 'x'>(Vector3{-1.25f, -1.25f, 0.0f}))*Matrix4::rotationY(-45.0_degf)*Matrix4::rotationX(45.0_degf),
            /* to test also per-instance normal matrix is applied properly --
               the first sphere should *not* have axis-aligned TBN directions */
            (Matrix4::rotationY(-45.0_degf)*Matrix4::rotationX(45.0_degf)).normalMatrix(),
            /* 6 gets added to the uniform objectId, wrapping it around to 0,
               making it visually close to the multidraw test */
            {0.0f, 0.0f, 0.0f}, 6},
        {Matrix4::translation(Math::gather<'z', 'y', 'x'>(Vector3{ 1.25f, -1.25f, 0.0f})),
            {},
            {1.0f, 0.0f, 1.0f}, 10},
        {Matrix4::translation(Math::gather<'z', 'y', 'x'>(Vector3{  0.0f,  1.0f, -1.0f})),
            {},
            #ifndef MAGNUM_TARGET_GLES2
            data.flags & MeshVisualizerGL3D::Flag::TextureArrays ? Vector3{0.0f, 0.0f, 2.0f} :
            #endif
            Vector3{0.5f, 1.0f, 2.0f}, 14}
    };

    sphere
        .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
            MeshVisualizerGL3D::TransformationMatrix{},
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            MeshVisualizerGL3D::NormalMatrix{},
            #else
            sizeof(Matrix3x3),
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            MeshVisualizerGL3D::TextureOffsetLayer{},
            MeshVisualizerGL3D::ObjectId{}
            #else
            4*4
            #endif
        )
        .setInstanceCount(3);

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::InstancedTransformation|data.flags|flag)};
    shader.setViewportSize(Vector2{RenderSize});
    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & (MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::ObjectId))
        shader.bindColorMapTexture(_colorMapTexture);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= MeshVisualizerGL3D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & MeshVisualizerGL3D::Flag::TextureArrays) {
            /* 2 extra slices as a base offset, each slice has half height,
               second slice has the data in the right half */
            const UnsignedShort imageData[]{
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,

                5, 0, 0, 0,
                0, 5, 0, 0,

                0, 0, 3, 0,
                0, 0, 0, 3,

                1, 0, 0, 0,
                0, 1, 0, 0
            };
            ImageView3D image{PixelFormat::R16UI, {4, 2, 5}, imageData};

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
                5, 0, 3, 0,
                0, 5, 0, 3,
                0, 1, 0, 0,
                0, 0, 1, 0
            };
            ImageView2D image{PixelFormat::R16UI, {4, 4}, imageData};

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

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader
            .setColor(0xffffcc_rgbf)
            .setTransformationMatrix(
                Matrix4::translation(Vector3::zAxis(-2.15f))*
                Matrix4::rotationY(90.0_degf)*
                Matrix4::scaling(Vector3{0.4f}))
            .setProjectionMatrix(
                Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f));

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(data.flags & (MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::NormalDirection))
            shader
                .setNormalMatrix(Matrix4::rotationY(90.0_degf).normalMatrix())
                .setLineLength(0.25f);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.setTextureMatrix(Matrix3::scaling(
                /* Slices of the texture array have half the height */
                data.flags & MeshVisualizerGL3D::Flag::TextureArrays ? Vector2::xScale(0.5f) : Vector2{0.5f}
            ));
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & MeshVisualizerGL3D::Flag::TextureArrays)
            shader.setTextureLayer(2); /* base offset */
        #endif

        if(data.flags & MeshVisualizerGL3D::Flag::Wireframe)
            shader.setWireframeColor(0xcc0000_rgbf);

        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & MeshVisualizerGL3D::Flag::VertexId)
            shader.setColorMapTransformation(0.5f/sphereData.vertexCount(), 1.0f/sphereData.vertexCount());
        else if(data.flags & MeshVisualizerGL3D::Flag::ObjectId)
            /* To make this visually close to the multidraw test */
            shader
                .setObjectId(6)
                .setColorMapTransformation(0.5f/12, 1.0f/12);
        #endif

        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
                .setNormalMatrix(Matrix4::rotationY(90.0_degf).normalMatrix())
                .setObjectId(6)
        }};
        GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, {
            TextureTransformationUniform{}
                .setTextureMatrix(Matrix3::scaling(
                    #ifndef MAGNUM_TARGET_GLES2
                    /* Slices of the texture array have half the height */
                    data.flags & MeshVisualizerGL3D::Flag::TextureArrays ? Vector2::xScale(0.5f) :
                    #endif
                    Vector2{0.5f}))
                .setLayer(2) /* base offset */
        }};
        MeshVisualizerMaterialUniform materialUniformData[1];
        (*materialUniformData)
            .setColor(0xffffcc_rgbf)
            .setWireframeColor(0xcc0000_rgbf)
            .setLineLength(0.25f);
        if(data.flags & MeshVisualizerGL3D::Flag::VertexId)
            materialUniformData->setColorMapTransformation(0.5f/sphereData.vertexCount(), 1.0f/sphereData.vertexCount());
        else if(data.flags & MeshVisualizerGL3D::Flag::ObjectId)
            materialUniformData->setColorMapTransformation(0.5f/12, 1.0f/12);
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialUniformData};
        if(data.flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
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
        First circle should be lower left, second lower right, third up center.

        -   Wireframe all looking the same (the only instanced thing that can
            differ is the transformation
        -   TBN should have the lower right with different orientation than the
            other two
        -   Vertex ID should all have the full color map range
        -   Object ID should be visually close to the multidraw case, except
            that each circle is just a single color
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
template<MeshVisualizerGL2D::Flag flag> void MeshVisualizerGLTest::renderInstancedSkinningWireframe2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 4)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL2D::Flag::UniformBuffers) {
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

           5--4 1
           | / /|
           |/ / |
           3 2--0 */
        {{ 1.0f, -1.0f}, {0, 0, 0}, {1.0f, 0.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {0, 3, 0}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f}, {0, 0, 1}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, -1.0f}, {0, 0, 1}, {0.0f, 0.0f, 1.0f}},
        {{ 1.0f,  1.0f}, {0, 3, 0}, {0.0f, 1.0f, 0.0f}},
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

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(6)
        .addVertexBuffer(GL::Buffer{vertices}, 0,
            MeshVisualizerGL2D::Position{},
            MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Three},
            MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Three})
        .addVertexBufferInstanced(GL::Buffer{instanceTransformations}, 1, 0,
            MeshVisualizerGL2D::TransformationMatrix{})
        .setInstanceCount(3);

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::InstancedTransformation|flag|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
        .setJointCount(15, 3, 0)};

    if(flag == MeshVisualizerGL2D::Flag{}) {
        shader
            .setJointMatrices(jointMatrices)
            .setPerInstanceJointCount(5)
            .setTransformationProjectionMatrix(Matrix3::scaling(Vector2{0.3f}))
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .draw(mesh);
    } else if(flag == MeshVisualizerGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL2D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform2D{}
                .setPerInstanceJointCount(5)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf)
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
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/skinning-instanced.tga"),
        /* SwiftShader has minor differences in the output, ARM Mali too,
           NVidia as well */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.022f}));
}

template<MeshVisualizerGL3D::Flag flag> void MeshVisualizerGLTest::renderInstancedSkinningWireframe3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName("Flag::ShaderStorageBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 5)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == MeshVisualizerGL3D::Flag::UniformBuffers) {
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

           5--4 1
           | / /|
           |/ / |
           3 2--0 */
        {{ 1.0f, -1.0f, 0.0f}, {0, 0, 0}, {1.0f, 0.0f, 0.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {0, 3, 0}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0, 0, 1}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0, 0, 1}, {0.0f, 0.0f, 1.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {0, 3, 0}, {0.0f, 1.0f, 0.0f}},
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

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(6)
        .addVertexBuffer(GL::Buffer{vertices}, 0,
            MeshVisualizerGL3D::Position{},
            MeshVisualizerGL3D::JointIds{MeshVisualizerGL3D::JointIds::Components::Three},
            MeshVisualizerGL3D::Weights{MeshVisualizerGL3D::Weights::Components::Three})
        .addVertexBufferInstanced(GL::Buffer{instanceTransformations}, 1, 0,
            MeshVisualizerGL3D::TransformationMatrix{})
        .setInstanceCount(3);

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::InstancedTransformation|flag|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
        .setJointCount(15, 3, 0)};

    if(flag == MeshVisualizerGL3D::Flag{}) {
        shader
            .setJointMatrices(jointMatrices)
            .setPerInstanceJointCount(5)
            .setTransformationMatrix(Matrix4::scaling(Vector3{0.3f}))
            .setColor(0xffff99_rgbf)
            .setWireframeColor(0x9999ff_rgbf)
            .draw(mesh);
    } else if(flag == MeshVisualizerGL3D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == MeshVisualizerGL3D::Flag::ShaderStorageBuffers
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
            MeshVisualizerDrawUniform3D{}
                .setPerInstanceJointCount(5)
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            MeshVisualizerMaterialUniform{}
                .setColor(0xffff99_rgbf)
                .setWireframeColor(0x9999ff_rgbf)
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
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/skinning-instanced.tga"),
        /* SwiftShader has minor differences in the output, ARM Mali too,
           NVidia as well */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.022f}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGLTest::renderMulti2D() {
    auto&& data = RenderMultiData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if((data.flags & MeshVisualizerGL2D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & MeshVisualizerGL2D::Flag::TextureTransformation ? 4 : 3))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= MeshVisualizerGL2D::Flag::MultiDraw) {
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

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    shader.setViewportSize(Vector2{RenderSize});
    if(data.flags & (MeshVisualizerGL2D::Flag::VertexId|MeshVisualizerGL2D::Flag::ObjectId))
        shader.bindColorMapTexture(_colorMapTexture);

    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= MeshVisualizerGL2D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & MeshVisualizerGL2D::Flag::TextureArrays) {
            /* Each slice has half height, second slice has the data in the
               right half */
            const UnsignedShort imageData[]{
                5, 0, 0, 0,
                0, 5, 0, 0,

                0, 0, 3, 0,
                0, 0, 0, 3,

                1, 0, 0, 0,
                0, 1, 0, 0
            };
            ImageView3D image{PixelFormat::R16UI, {4, 2, 3}, imageData};

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
                5, 0, 3, 0,
                0, 5, 0, 3,
                0, 1, 0, 0,
                0, 0, 1, 0
            };
            ImageView2D image{PixelFormat::R16UI, {4, 4}, imageData};

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
    Trade::MeshData circleData = MeshTools::generateIndices(Primitives::circle2DSolid(8, Primitives::Circle2DFlag::TextureCoordinates));
    Trade::MeshData squareData = MeshTools::generateIndices(Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates));
    Trade::MeshData triangleData = MeshTools::generateIndices(Primitives::circle2DSolid(3, Primitives::Circle2DFlag::TextureCoordinates));
    /* For instanced object ID rendering we have to add the object ID
       attribute. Use the same numbers for all meshes, it'll get differentiated
       by the per-draw object ID. */
    if(data.flags & MeshVisualizerGL2D::Flag::ObjectId) {
        Containers::Array<UnsignedInt> ids{8};
        /* Each two faces share the same ID */
        for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i/2;
        for(Trade::MeshData* i: {&circleData, &squareData, &triangleData}) {
            *i = MeshTools::combineFaceAttributes(*i, {
                Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                    ids.prefix(i->indexCount()/3)}
            });
        }
    }
    /* For a GS-less wireframe we have to deindex the meshes */
    if(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)
        for(Trade::MeshData* i: {&circleData, &squareData, &triangleData})
            *i = MeshTools::duplicate(*i);
    Trade::MeshData concatenated = MeshTools::concatenate({circleData, squareData, triangleData});
    GL::Mesh mesh = MeshTools::compile(concatenated);
    GL::MeshView circle{mesh};
    circle.setCount(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader ?
        circleData.vertexCount() : circleData.indexCount());
    GL::MeshView square{mesh};
    square.setCount(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader ?
        squareData.vertexCount() : squareData.indexCount());
    if(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)
        square.setBaseVertex(circleData.vertexCount());
    else
        square.setIndexOffset(circleData.indexCount());
    GL::MeshView triangle{mesh};
    triangle.setCount(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader ?
        triangleData.vertexCount() : triangleData.indexCount());
    if(data.flags & MeshVisualizerGL2D::Flag::NoGeometryShader)
        triangle.setBaseVertex(circleData.vertexCount() + squareData.vertexCount());
    else triangle.setIndexOffset(circleData.indexCount() + squareData.indexCount());

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<MeshVisualizerMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xffffcc_rgbf)
        .setWireframeColor(0xcc0000_rgbf);
    if(data.flags & MeshVisualizerGL2D::Flag::VertexId)
        /* Here, gl_VertexID is taken *including* the base offset, which means
           we have to count all vertices to avoid colormap wraparounds */
        materialData[0*data.uniformIncrement].setColorMapTransformation(0.5f/concatenated.vertexCount(), 1.0f/concatenated.vertexCount());
    else if(data.flags & MeshVisualizerGL2D::Flag::ObjectId)
        /* There's at most 4 colors (one every 2 faces) per draw and 3 draws,
           so make it fit 12 colors */
        materialData[0*data.uniformIncrement].setColorMapTransformation(0.5f/12, 1.0f/12);
    materialData[1*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xccffff_rgbf)
        .setWireframeColor(0x0000cc_rgbf)
        .setWireframeWidth(2.5f);
    if(data.flags & MeshVisualizerGL2D::Flag::VertexId)
        /* Here, gl_VertexID is taken *including* the base offset, which means
           we have to count all vertices to avoid colormap wraparounds */
        materialData[1*data.uniformIncrement].setColorMapTransformation(0.5f/concatenated.vertexCount(), 1.0f/concatenated.vertexCount());
    else if(data.flags & MeshVisualizerGL2D::Flag::ObjectId)
        /* There's at most 4 colors (one every 2 faces) per draw and 3 draws,
           so make it fit 12 colors */
        materialData[1*data.uniformIncrement].setColorMapTransformation(0.5f/12, 1.0f/12);
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
            data.flags & MeshVisualizerGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.0f, 0.0f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & MeshVisualizerGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({1.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({1.0f, 0.0f}))
        .setLayer(1); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & MeshVisualizerGL2D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.5f, 1.0f}))
        .setLayer(2); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<MeshVisualizerDrawUniform2D> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are done with
       UBO offset bindings instead. */
    drawData[0*data.uniformIncrement] = MeshVisualizerDrawUniform2D{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setObjectId(0);
    drawData[1*data.uniformIncrement] = MeshVisualizerDrawUniform2D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(4);
    drawData[2*data.uniformIncrement] = MeshVisualizerDrawUniform2D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(8);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform2D),
            sizeof(MeshVisualizerDrawUniform2D));
        if(data.flags & MeshVisualizerGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(circle);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform2D),
            sizeof(MeshVisualizerDrawUniform2D));
        if(data.flags & MeshVisualizerGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform2D),
            sizeof(MeshVisualizerDrawUniform2D));
        if(data.flags & MeshVisualizerGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(triangle);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform);
        if(data.flags & MeshVisualizerGL2D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);

        if(data.flags >= MeshVisualizerGL2D::Flag::MultiDraw)
            shader.draw({circle, square, triangle});
        else {
            shader.setDrawOffset(0)
                .draw(circle);
            shader.setDrawOffset(1)
                .draw(square);
            shader.setDrawOffset(2)
                .draw(triangle);
        }
    };

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        Wireframe case:

        -   Circle should be lower left, pink with red wireframe
        -   Square lower right, cyan with thick blue wireframe
        -   Triangle up center, cyan with thick blue wireframe

        Vertex ID case:

        -   Circle and triangle should have both almost the full color map
            range, one tinted pink, one cyan
        -   Square tinted cyan, with just two colors
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

void MeshVisualizerGLTest::renderMulti3D() {
    auto&& data = RenderMultiData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(((data.flags & MeshVisualizerGL3D::Flag::Wireframe) && !(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)) || (data.flags & (MeshVisualizerGL3D::Flag::TangentDirection|MeshVisualizerGL3D::Flag::BitangentDirection|MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|MeshVisualizerGL3D::Flag::NormalDirection))) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
            CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
            CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() << "is not supported.");
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < (data.flags & MeshVisualizerGL3D::Flag::TextureTransformation ? 5 : 4))
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= MeshVisualizerGL3D::Flag::MultiDraw) {
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

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::UniformBuffers|data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    shader.setViewportSize(Vector2{RenderSize});
    if(data.flags & (MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::ObjectId))
        shader.bindColorMapTexture(_colorMapTexture);

    GL::Texture2D objectIdTexture{NoCreate};
    GL::Texture2DArray objectIdTextureArray{NoCreate};
    if(data.flags >= MeshVisualizerGL3D::Flag::ObjectIdTexture) {
        /* This should match transformation done for the diffuse/normal
           texture */
        if(data.flags & MeshVisualizerGL3D::Flag::TextureArrays) {
            /* Each slice has half height, second slice has the data in the
               right half */
            const UnsignedShort imageData[]{
                5, 0, 0, 0,
                0, 5, 0, 0,

                0, 0, 3, 0,
                0, 0, 0, 3,

                1, 0, 0, 0,
                0, 1, 0, 0
            };
            ImageView3D image{PixelFormat::R16UI, {4, 2, 3}, imageData};

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
                5, 0, 3, 0,
                0, 5, 0, 3,
                0, 1, 0, 0,
                0, 0, 1, 0
            };
            ImageView2D image{PixelFormat::R16UI, {4, 4}, imageData};

            objectIdTexture = GL::Texture2D{};
            objectIdTexture.setMinificationFilter(GL::SamplerFilter::Nearest)
                .setMagnificationFilter(GL::SamplerFilter::Nearest)
                .setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setStorage(1, GL::TextureFormat::R16UI, image.size())
                .setSubImage(0, {}, image);
            shader.bindObjectIdTexture(objectIdTexture);
        }
    }

    /* We don't visualize tangents for the sphere, but concatenate() will
       ignore the tangents of others if the first mesh doesn't have them */
    Trade::MeshData sphereData = Primitives::uvSphereSolid(2, 4, Primitives::UVSphereFlag::Tangents|Primitives::UVSphereFlag::TextureCoordinates);
    /* Plane is a strip, make it indexed first */
    Trade::MeshData planeData = MeshTools::generateIndices(Primitives::planeSolid(Primitives::PlaneFlag::Tangents|Primitives::PlaneFlag::TextureCoordinates));
    Trade::MeshData coneData = Primitives::coneSolid(1, 8, 1.0f, Primitives::ConeFlag::Tangents|Primitives::ConeFlag::TextureCoordinates);
    /* For instanced object ID rendering we have to add the object ID
       attribute. Use the same numbers for all meshes, it'll get differentiated
       by the per-draw object ID. */
    if(data.flags & MeshVisualizerGL3D::Flag::ObjectId) {
        Containers::Array<UnsignedInt> ids{20};
        /* Each two faces share the same ID */
        for(std::size_t i = 0; i != ids.size(); ++i) ids[i] = i/2;
        for(Trade::MeshData* i: {&sphereData, &planeData, &coneData}) {
            *i = MeshTools::combineFaceAttributes(*i, {
                Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
                    ids.prefix(i->indexCount()/3)}
            });
        }
    }
    /* For a GS-less wireframe we have to deindex the meshes */
    if(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)
        for(Trade::MeshData* i: {&sphereData, &planeData, &coneData})
            *i = MeshTools::duplicate(*i);
    Trade::MeshData concatenated = MeshTools::concatenate({sphereData, planeData, coneData});
    GL::Mesh mesh = MeshTools::compile(concatenated);
    GL::MeshView sphere{mesh};
    sphere.setCount(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader ?
        sphereData.vertexCount() : sphereData.indexCount());
    GL::MeshView plane{mesh};
    plane.setCount(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader ?
        planeData.vertexCount() : planeData.indexCount());
    if(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)
        plane.setBaseVertex(sphereData.vertexCount());
    else
        plane.setIndexOffset(sphereData.indexCount());
    GL::MeshView cone{mesh};
    cone.setCount(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader ?
        coneData.vertexCount() : coneData.indexCount());
    if(data.flags & MeshVisualizerGL3D::Flag::NoGeometryShader)
        cone.setBaseVertex(sphereData.vertexCount() + planeData.vertexCount());
    else cone.setIndexOffset(sphereData.indexCount() + planeData.indexCount());

    GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
        ProjectionUniform3D{}.setProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)
        )
    }};
    shader.bindProjectionBuffer(projectionUniform);

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<MeshVisualizerMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xffffcc_rgbf)
        .setWireframeColor(0xcc0000_rgbf)
        .setLineLength(0.0f); /* no TBN */
    if(data.flags & MeshVisualizerGL3D::Flag::VertexId)
        /* Here, gl_VertexID is taken *including* the base offset, which means
           we have to count all vertices to avoid colormap wraparounds */
        materialData[0*data.uniformIncrement].setColorMapTransformation(0.5f/concatenated.vertexCount(), 1.0f/concatenated.vertexCount());
    else if(data.flags & MeshVisualizerGL3D::Flag::ObjectId)
        /* There's at most 10 colors (one every 2 faces) per draw and 3 draws,
           so make it fit 30 colors */
        materialData[0*data.uniformIncrement].setColorMapTransformation(0.5f/30, 1.0f/30);
    materialData[1*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xccffff_rgbf)
        .setWireframeColor(0x0000cc_rgbf)
        .setLineLength(0.25f)
        .setWireframeWidth(2.5f);
    if(data.flags & MeshVisualizerGL3D::Flag::VertexId)
        /* Here, gl_VertexID is taken *including* the base offset, which means
           we have to count all vertices to avoid colormap wraparounds */
        materialData[1*data.uniformIncrement].setColorMapTransformation(0.5f/concatenated.vertexCount(), 1.0f/concatenated.vertexCount());
    else if(data.flags & MeshVisualizerGL3D::Flag::ObjectId)
        /* There's at most 10 colors (one every 2 faces) per draw and 3 draws,
           so make it fit 30 colors */
        materialData[1*data.uniformIncrement].setColorMapTransformation(0.5f/30, 1.0f/30);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationUniform3D> transformationData{2*data.uniformIncrement + 1};
    transformationData[0*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({-1.25f, -1.25f, 0.0f})
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
            data.flags & MeshVisualizerGL3D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.0f, 0.0f}))
        .setLayer(0); /* ignored if not array */
    textureTransformationData[1*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & MeshVisualizerGL3D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({1.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({1.0f, 0.0f}))
        .setLayer(1); /* ignored if not array */
    textureTransformationData[2*data.uniformIncrement] = TextureTransformationUniform{}
        .setTextureMatrix(
            data.flags & MeshVisualizerGL3D::Flag::TextureArrays ?
                Matrix3::scaling(Vector2::xScale(0.5f))*
                Matrix3::translation({0.0f, 0.0f}) :
                Matrix3::scaling(Vector2{0.5f})*
                Matrix3::translation({0.5f, 1.0f}))
        .setLayer(2); /* ignored if not array */
    GL::Buffer textureTransformationUniform{GL::Buffer::TargetHint::Uniform, textureTransformationData};

    Containers::Array<MeshVisualizerDrawUniform3D> drawData{2*data.uniformIncrement + 1};
    /* Material offsets are zero if we have single draw, as those are done with
       UBO offset bindings instead. Also no need to supply a normal matrix. */
    drawData[0*data.uniformIncrement] = MeshVisualizerDrawUniform3D{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        .setObjectId(0);
    drawData[1*data.uniformIncrement] = MeshVisualizerDrawUniform3D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(10);
    drawData[2*data.uniformIncrement] = MeshVisualizerDrawUniform3D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setObjectId(20);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform3D),
            sizeof(MeshVisualizerDrawUniform3D));
        if(data.flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            0*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(sphere);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform3D),
            sizeof(MeshVisualizerDrawUniform3D));
        if(data.flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            1*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(plane);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform3D),
            sizeof(MeshVisualizerDrawUniform3D));
        if(data.flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform,
            2*data.uniformIncrement*sizeof(TextureTransformationUniform),
            sizeof(TextureTransformationUniform));
        shader.draw(cone);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform);
        if(data.flags & MeshVisualizerGL3D::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);

        if(data.flags >= MeshVisualizerGL3D::Flag::MultiDraw)
            shader.draw({sphere, plane, cone});
        else {
            shader.setDrawOffset(0)
                .draw(sphere);
            shader.setDrawOffset(1)
                .draw(plane);
            shader.setDrawOffset(2)
                .draw(cone);
        }
    };

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        Wireframe case:

        -   Sphere should be lower left, pink with red wireframe (and no TBN)
        -   Plane lower right, cyan with thick blue wireframe and TBN
        -   Cone up center, cyan with thick blue wireframe and TBN

        Vertex ID case:

        -   Sphere and cone should have both almost the full color map
            range, one tinted pink, one cyan
        -   Plane tinted cyan, with just two colors
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "MeshVisualizerTestFiles", data.expected}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

void MeshVisualizerGLTest::renderMultiSkinningWireframe2D() {
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
    if(data.flags2D >= MeshVisualizerGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 4)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags2D >= MeshVisualizerGL2D::Flag::MultiDraw) {
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

    MeshVisualizerGL2D shader{MeshVisualizerGL2D::Configuration{}
        .setFlags(MeshVisualizerGL2D::Flag::UniformBuffers|data.flags2D|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader)
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

           5--4 1    7 11--10
           | / /|   /|  | /
           |/ / |  / |  |/
           3 2--0 8--6  9 */
        {{ 1.0f, -1.0f}, {0, 0}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {0, 2}, {0.0f, 1.0f}},
        {{-1.0f, -1.0f}, {1, 2}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f}, {1, 2}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {0, 2}, {0.0f, 1.0f}},
        {{-1.0f,  1.0f}, {0, 3}, {0.0f, 1.0f}},

        {{ 1.0f, -1.0f}, {0, 3}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f}, {2, 1}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f}, {0, 0}, {1.0f, 0.0f}},

        {{-1.0f, -1.0f}, {0, 1}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f}, {1, 0}, {1.0f, 0.0f}},
        {{-1.0f,  1.0f}, {2, 2}, {0.5f, 0.5f}}
    };

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(12)
        .addVertexBuffer(GL::Buffer{vertices}, 0,
            MeshVisualizerGL2D::Position{},
            MeshVisualizerGL2D::JointIds{MeshVisualizerGL2D::JointIds::Components::Two},
            MeshVisualizerGL2D::Weights{MeshVisualizerGL2D::Weights::Components::Two});
    GL::MeshView square{mesh};
    square.setCount(6);
    GL::MeshView triangle1{mesh};
    triangle1.setCount(3)
        .setBaseVertex(6);
    GL::MeshView triangle2{mesh};
    triangle2.setCount(3)
        .setBaseVertex(9);

    GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
        ProjectionUniform3D{}
    }};;

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<MeshVisualizerMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xffffcc_rgbf)
        .setWireframeColor(0xcc0000_rgbf);
    materialData[1*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xccffff_rgbf)
        .setWireframeColor(0x0000cc_rgbf);
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

    Containers::Array<MeshVisualizerDrawUniform2D> drawData{2*data.uniformIncrement + 1};
    /* Material / joint offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead */
    drawData[0*data.uniformIncrement] = MeshVisualizerDrawUniform2D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 0);
    drawData[1*data.uniformIncrement] = MeshVisualizerDrawUniform2D{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        /* Overlaps with the first joint set with two matrices, unless the
           padding in the single-draw case prevents that */
        .setJointOffset(data.bindWithOffset ? 0 : 2);
    drawData[2*data.uniformIncrement] = MeshVisualizerDrawUniform2D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 6);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindJointBuffer(jointUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform2D),
            4*sizeof(TransformationUniform2D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform2D),
            sizeof(MeshVisualizerDrawUniform2D));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindJointBuffer(jointUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform2D),
            4*sizeof(TransformationUniform2D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform2D),
            sizeof(MeshVisualizerDrawUniform2D));
        shader.draw(triangle1);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.bindJointBuffer(jointUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform2D),
            4*sizeof(TransformationUniform2D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform2D),
            sizeof(MeshVisualizerDrawUniform2D));
        shader.draw(triangle2);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindJointBuffer(jointUniform)
            .bindDrawBuffer(drawUniform);

        if(data.flags2D >= MeshVisualizerGL2D::Flag::MultiDraw)
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
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/skinning-multi.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}

void MeshVisualizerGLTest::renderMultiSkinningWireframe3D() {
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
    if(data.flags3D >= MeshVisualizerGL3D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders,
           some others (Qualcomm Adreno 730) support just 4 blocks */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 5)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags3D >= MeshVisualizerGL3D::Flag::MultiDraw) {
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

    MeshVisualizerGL3D shader{MeshVisualizerGL3D::Configuration{}
        .setFlags(MeshVisualizerGL3D::Flag::UniformBuffers|data.flags3D|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader)
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

           5--4 1    7 11--10
           | / /|   /|  | /
           |/ / |  / |  |/
           3 2--0 8--6  9 */
        {{ 1.0f, -1.0f, 0.0f}, {0, 0}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {0, 2}, {0.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {1, 2}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {1, 2}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {0, 2}, {0.0f, 1.0f}},
        {{-1.0f,  1.0f, 0.0f}, {0, 3}, {0.0f, 1.0f}},

        {{ 1.0f, -1.0f, 0.0f}, {0, 3}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {2, 1}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0, 0}, {1.0f, 0.0f}},

        {{-1.0f, -1.0f, 0.0f}, {0, 1}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {1, 0}, {1.0f, 0.0f}},
        {{-1.0f,  1.0f, 0.0f}, {2, 2}, {0.5f, 0.5f}}
    };

    GL::Mesh mesh{MeshPrimitive::Triangles};
    mesh.setCount(12)
        .addVertexBuffer(GL::Buffer{vertices}, 0,
            MeshVisualizerGL3D::Position{},
            MeshVisualizerGL3D::JointIds{MeshVisualizerGL3D::JointIds::Components::Two},
            MeshVisualizerGL3D::Weights{MeshVisualizerGL3D::Weights::Components::Two});
    GL::MeshView square{mesh};
    square.setCount(6);
    GL::MeshView triangle1{mesh};
    triangle1.setCount(3)
        .setBaseVertex(6);
    GL::MeshView triangle2{mesh};
    triangle2.setCount(3)
        .setBaseVertex(9);

    GL::Buffer projectionUniform{GL::Buffer::TargetHint::Uniform, {
        ProjectionUniform3D{}
    }};;

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<MeshVisualizerMaterialUniform> materialData{data.uniformIncrement + 1};
    materialData[0*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xffffcc_rgbf)
        .setWireframeColor(0xcc0000_rgbf);
    materialData[1*data.uniformIncrement] = MeshVisualizerMaterialUniform{}
        .setColor(0xccffff_rgbf)
        .setWireframeColor(0x0000cc_rgbf);
    GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, materialData};

    Containers::Array<TransformationUniform3D> transformationData{2*data.uniformIncrement + 1};
    transformationData[0*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::scaling(Vector3{0.3f})*
            Matrix4::translation({ 0.0f, -1.5f, 0.0f}));
    transformationData[1*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::scaling(Vector3{0.3f})*
            Matrix4::translation({ 1.5f,  1.5f, 0.0f}));
    transformationData[2*data.uniformIncrement] = TransformationUniform3D{}
        .setTransformationMatrix(
            Matrix4::scaling(Vector3{0.3f})*
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

    Containers::Array<MeshVisualizerDrawUniform3D> drawData{2*data.uniformIncrement + 1};
    /* Material / joint offsets are zero if we have single draw, as those are
       done with UBO offset bindings instead */
    drawData[0*data.uniformIncrement] = MeshVisualizerDrawUniform3D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 0);
    drawData[1*data.uniformIncrement] = MeshVisualizerDrawUniform3D{}
        .setMaterialId(data.bindWithOffset ? 0 : 0)
        /* Overlaps with the first joint set with two matrices, unless the
           padding in the single-draw case prevents that */
        .setJointOffset(data.bindWithOffset ? 0 : 2);
    drawData[2*data.uniformIncrement] = MeshVisualizerDrawUniform3D{}
        .setMaterialId(data.bindWithOffset ? 0 : 1)
        .setJointOffset(data.bindWithOffset ? 0 : 6);
    GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, drawData};

    shader.bindProjectionBuffer(projectionUniform);

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindJointBuffer(jointUniform,
            0*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform3D),
            sizeof(MeshVisualizerDrawUniform3D));
        shader.draw(square);

        shader.bindMaterialBuffer(materialUniform,
            0*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindJointBuffer(jointUniform,
            1*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform3D),
            sizeof(MeshVisualizerDrawUniform3D));
        shader.draw(triangle1);

        shader.bindMaterialBuffer(materialUniform,
            1*data.uniformIncrement*sizeof(MeshVisualizerMaterialUniform),
            sizeof(MeshVisualizerMaterialUniform));
        shader.bindTransformationBuffer(transformationUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            sizeof(TransformationUniform3D));
        shader.bindJointBuffer(jointUniform,
            2*data.uniformIncrement*sizeof(TransformationUniform3D),
            4*sizeof(TransformationUniform3D));
        shader.bindDrawBuffer(drawUniform,
            2*data.uniformIncrement*sizeof(MeshVisualizerDrawUniform3D),
            sizeof(MeshVisualizerDrawUniform3D));
        shader.draw(triangle2);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindMaterialBuffer(materialUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindJointBuffer(jointUniform)
            .bindDrawBuffer(drawUniform);

        if(data.flags3D >= MeshVisualizerGL3D::Flag::MultiDraw)
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
        Utility::Path::join(_testDir, "MeshVisualizerTestFiles/skinning-multi.tga"),
        (DebugTools::CompareImageToFile{_manager}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGLTest)
