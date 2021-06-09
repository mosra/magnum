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

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Primitives/Grid.h"
#include "Magnum/Shaders/DistanceFieldVectorGL.h"
#include "Magnum/Shaders/FlatGL.h"
#include "Magnum/Shaders/MeshVisualizerGL.h"
#include "Magnum/Shaders/PhongGL.h"
#include "Magnum/Shaders/VertexColorGL.h"
#include "Magnum/Shaders/VectorGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#include "Magnum/Shaders/DistanceFieldVector.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Shaders/Vector.h"
#endif

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/*
    The goal of this is to not duplicate all testing work here, but instead
    have a set of simple-to-setup benchmarks with trivial output that allow for
    measuring cost of particular shader features or seeing performance
    differences between different implementations of the same (e.g., using the
    VertexColor shader vs Flat with vertex colors enabled). Thus:

    -   all shaders render the same mesh, 2D shaders only ignore the Z
        coordinate (so it should be possible to compare the perf between 2D and
        3D)
    -   the mesh contains all attributes the shader might ever need including
        instanced ones, to avoid differences caused by different memory access
        patterns
    -   transformation and projection is identity
    -   textures are always single-pixel to measure the sampler overhead, not
        memory access overhead
    -   if texture transformation is enabled, it's identity
    -   if instancing features are enabled, there's exactly one instance
    -   if alpha mask is enabled, it's 0.0
    -   uniforms / binding overhead is not included in the benchmark
*/

struct ShadersGLBenchmark: GL::OpenGLTester {
    explicit ShadersGLBenchmark();

    void renderSetup();
    void renderTeardown();

    template<UnsignedInt dimensions> void flat();
    void phong();
    template<UnsignedInt dimensions> void vertexColor();
    template<UnsignedInt dimensions> void vector();
    template<UnsignedInt dimensions> void distanceFieldVector();
    void meshVisualizer2D();
    void meshVisualizer3D();
    /** @todo mesh visualizer TBN, how to verify output? */

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};

        GL::Renderbuffer _color;
        #ifndef MAGNUM_TARGET_GLES2
        GL::Renderbuffer _objectId{NoCreate};
        #endif
        GL::Framebuffer _framebuffer;

        GL::Buffer _indices, _vertices;
        GL::Mesh _mesh, _meshInstanced, _meshDuplicated;

        GL::Texture2D _textureWhite, _textureBlue;
        #ifndef MAGNUM_TARGET_GLES2
        GL::Texture2DArray _textureWhiteArray, _textureBlueArray;
        #endif
};

using namespace Math::Literals;

constexpr Vector2i GridSubdivisions{64, 64};
constexpr Vector2i RenderSize{512, 512};
constexpr std::size_t WarmupIterations{100};
constexpr std::size_t BenchmarkIterations{1000};
constexpr std::size_t BenchmarkRepeats{4};

const struct {
    const char* name;
    FlatGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} FlatData[] {
    {"", {}, 1, 1},
    {"vertex color", FlatGL2D::Flag::VertexColor, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", FlatGL2D::Flag::ObjectId, 1, 1},
    #endif
    {"textured", FlatGL2D::Flag::Textured, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"texture array", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays, 1, 1},
    #endif
    {"textured + alpha mask", FlatGL2D::Flag::Textured|FlatGL2D::Flag::AlphaMask, 1, 1},
    {"texture transformation", FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation, 1, 1},
    {"instanced transformation", FlatGL2D::Flag::InstancedTransformation, 1, 1},
    {"instanced transformation + color", FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::VertexColor, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"instanced transformation + object ID", FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::InstancedObjectId, 1, 1},
    #endif
    {"instanced transformation + texture offset", FlatGL2D::Flag::Textured|FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::InstancedTextureOffset, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"UBO single", FlatGL2D::Flag::UniformBuffers, 1, 1},
    {"UBO single, texture transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureTransformation, 1, 1},
    {"UBO single, texture array transformation", FlatGL2D::Flag::UniformBuffers|FlatGL2D::Flag::Textured|FlatGL2D::Flag::TextureArrays|FlatGL2D::Flag::TextureTransformation, 1, 1},
    {"UBO multi", FlatGL2D::Flag::UniformBuffers, 32, 128},
    {"multidraw", FlatGL2D::Flag::MultiDraw, 32, 128},
    #endif
};

const struct {
    const char* name;
    PhongGL::Flags flags;
    UnsignedInt lightCount, materialCount, drawCount;
    bool bufferStorage;
} PhongData[] {
    {"", {}, 1, 1, 1, false},
    {"zero lights", {}, 0, 1, 1, false},
    {"five lights", {}, 5, 1, 1, false},
    {"no specular", PhongGL::Flag::NoSpecular, 1, 1, 1, false},
    {"vertex color", PhongGL::Flag::VertexColor, 1, 1, 1, false},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", PhongGL::Flag::ObjectId, 1, 1, 1, false},
    #endif
    {"diffuse texture", PhongGL::Flag::DiffuseTexture, 1, 1, 1, false},
    {"ADS textures", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture, 1, 1, 1, false},
    #ifndef MAGNUM_TARGET_GLES2
    {"ADS texture arrays", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureArrays, 1, 1, 1, false},
    #endif
    {"ADS textures + alpha mask", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::AlphaMask, 1, 1, 1, false},
    {"ADS textures + transformation", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureTransformation, 1, 1, 1, false},
    {"normal texture", PhongGL::Flag::NormalTexture, 1, 1, 1, false},
    {"normal texture with separate bitangent", PhongGL::Flag::NormalTexture|PhongGL::Flag::Bitangent, 1, 1, 1, false},
    {"instanced transformation", PhongGL::Flag::InstancedTransformation, 1, 1, 1, false},
    {"instanced transformation + color", PhongGL::Flag::InstancedTransformation|PhongGL::Flag::VertexColor, 1, 1, 1, false},
    #ifndef MAGNUM_TARGET_GLES2
    {"instanced transformation + object ID", PhongGL::Flag::InstancedTransformation|PhongGL::Flag::InstancedObjectId, 1, 1, 1, false},
    #endif
    {"instanced transformation + ADS texture offset", PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::InstancedTransformation|PhongGL::Flag::InstancedTextureOffset, 1, 1, 1, false},
    #ifndef MAGNUM_TARGET_GLES2
    {"UBO single", PhongGL::Flag::UniformBuffers, 1, 1, 1, false},
    {"UBO single, zero lights", PhongGL::Flag::UniformBuffers, 0, 1, 1, false},
    {"UBO single five lights", PhongGL::Flag::UniformBuffers, 5, 1, 1, false},
    {"UBO single, ADS textures + transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureTransformation, 1, 1, 1, false},
    {"UBO single, ADS texture arrays + transformation", PhongGL::Flag::UniformBuffers|PhongGL::Flag::AmbientTexture|PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture|PhongGL::Flag::TextureArrays|PhongGL::Flag::TextureTransformation, 1, 1, 1, false},
    {"UBO multi, one light", PhongGL::Flag::UniformBuffers, 1, 32, 128, false},
    {"multidraw, one light", PhongGL::Flag::MultiDraw, 1, 32, 128, false},
    #ifndef MAGNUM_TARGET_GLES
    {"multidraw, one light, immutable buffer storage", PhongGL::Flag::MultiDraw, 1, 32, 128, true},
    #endif
    {"multidraw, one light, light culling enabled", PhongGL::Flag::MultiDraw|PhongGL::Flag::LightCulling, 1, 32, 128, false},
    {"multidraw, 64 lights, light culling enabled, five used", PhongGL::Flag::MultiDraw|PhongGL::Flag::LightCulling, 64, 32, 128, false},
    #endif
};

const struct {
    const char* name;
    VertexColorGL2D::Flags flags;
    UnsignedInt drawCount;
} VertexColorData[] {
    {"", {}, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"UBO single", VertexColorGL2D::Flag::UniformBuffers, 1},
    {"UBO multi", VertexColorGL2D::Flag::UniformBuffers, 128},
    {"multidraw", VertexColorGL2D::Flag::MultiDraw, 128}
    #endif
};

const struct {
    const char* name;
    VectorGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} VectorData[] {
    {"", {}, 1, 1},
    {"texture transformation", VectorGL2D::Flag::TextureTransformation, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"UBO single", VectorGL2D::Flag::UniformBuffers, 1, 1},
    {"UBO single, texture transformation", VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation, 1, 1},
    {"UBO multi", VectorGL2D::Flag::UniformBuffers, 32, 128},
    {"UBO multi, texture transformation", VectorGL2D::Flag::UniformBuffers|VectorGL2D::Flag::TextureTransformation, 32, 128},
    {"multidraw", VectorGL2D::Flag::MultiDraw, 32, 128},
    #endif
};

const struct {
    const char* name;
    DistanceFieldVectorGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} DistanceFieldVectorData[] {
    {"", {}, 1, 1},
    {"texture transformation", DistanceFieldVectorGL2D::Flag::TextureTransformation, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"UBO single", DistanceFieldVectorGL2D::Flag::UniformBuffers, 1, 1},
    {"UBO single, texture transformation", DistanceFieldVectorGL2D::Flag::UniformBuffers|DistanceFieldVectorGL2D::Flag::TextureTransformation, 1, 1},
    {"UBO multi", DistanceFieldVectorGL2D::Flag::UniformBuffers, 32, 128},
    {"UBO multi, texture transformation", DistanceFieldVectorGL2D::Flag::UniformBuffers|DistanceFieldVectorGL2D::Flag::TextureTransformation, 32, 128},
    {"multidraw", DistanceFieldVectorGL2D::Flag::MultiDraw, 32, 128},
    #endif
};

const struct {
    const char* name;
    MeshVisualizerGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} MeshVisualizer2DData[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe", MeshVisualizerGL2D::Flag::Wireframe, 1, 1},
    #endif
    {"wireframe w/o a GS", MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"instanced object ID", MeshVisualizerGL2D::Flag::InstancedObjectId, 1, 1},
    {"vertex ID", MeshVisualizerGL2D::Flag::VertexId, 1, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizerGL2D::Flag::PrimitiveId, 1, 1},
    {"primitive ID from vertex ID", MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId, 1, 1},
    #endif
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    {"UBO single, wireframe", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe, 1, 1},
    #endif
    {"UBO single, wireframe w/o a GS", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader, 1, 1},
    {"UBO single, vertex ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::VertexId, 1, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"UBO multi, wireframe", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe, 32, 128},
    #endif
    {"UBO multi, wireframe w/o a GS", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader, 32, 128},
    {"UBO multi, vertex ID", MeshVisualizerGL2D::Flag::UniformBuffers|MeshVisualizerGL2D::Flag::VertexId, 32, 128},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, wireframe", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe, 32, 128},
    #endif
    {"multidraw, wireframe w/o a GS", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader, 32, 128},
    {"multidraw, vertex ID", MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::VertexId, 32, 128},
    #endif
};

const struct {
    const char* name;
    MeshVisualizerGL3D::Flags flags;
    UnsignedInt materialCount, drawCount;
} MeshVisualizer3DData[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"wireframe", MeshVisualizerGL3D::Flag::Wireframe, 1, 1},
    #endif
    {"wireframe w/o a GS", MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader, 1, 1},
    #ifndef MAGNUM_TARGET_GLES2
    {"instanced object ID", MeshVisualizerGL3D::Flag::InstancedObjectId, 1, 1},
    {"vertex ID", MeshVisualizerGL3D::Flag::VertexId, 1, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"primitive ID", MeshVisualizerGL3D::Flag::PrimitiveId, 1, 1},
    {"primitive ID from vertex ID", MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId, 1, 1},
    #endif
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    {"UBO single, wireframe", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe, 1, 1},
    #endif
    {"UBO single, wireframe w/o a GS", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader, 1, 1},
    {"UBO single, vertex ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::VertexId, 1, 1},
    #ifndef MAGNUM_TARGET_WEBGL
    {"UBO multi, wireframe", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe, 32, 128},
    #endif
    {"UBO multi, wireframe w/o a GS", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader, 32, 128},
    {"UBO multi, vertex ID", MeshVisualizerGL3D::Flag::UniformBuffers|MeshVisualizerGL3D::Flag::VertexId, 32, 128},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, wireframe", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe, 32, 128},
    #endif
    {"multidraw, wireframe w/o a GS", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader, 32, 128},
    {"multidraw, vertex ID", MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::VertexId, 32, 128},
    #endif
};

ShadersGLBenchmark::ShadersGLBenchmark(): _framebuffer{{{}, RenderSize}} {
    addInstancedBenchmarks({&ShadersGLBenchmark::flat<2>,
                            &ShadersGLBenchmark::flat<3>},
        BenchmarkRepeats, Containers::arraySize(FlatData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    addInstancedBenchmarks({&ShadersGLBenchmark::phong},
        BenchmarkRepeats, Containers::arraySize(PhongData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    addInstancedBenchmarks({&ShadersGLBenchmark::vertexColor<2>,
                   &ShadersGLBenchmark::vertexColor<3>},
        BenchmarkRepeats, Containers::arraySize(VertexColorData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    addInstancedBenchmarks({&ShadersGLBenchmark::vector<2>,
                            &ShadersGLBenchmark::vector<3>},
        BenchmarkRepeats, Containers::arraySize(VectorData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    addInstancedBenchmarks({&ShadersGLBenchmark::distanceFieldVector<2>,
                            &ShadersGLBenchmark::distanceFieldVector<3>},
        BenchmarkRepeats, Containers::arraySize(DistanceFieldVectorData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    addInstancedBenchmarks({&ShadersGLBenchmark::meshVisualizer2D},
        BenchmarkRepeats, Containers::arraySize(MeshVisualizer2DData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    addInstancedBenchmarks({&ShadersGLBenchmark::meshVisualizer3D},
        BenchmarkRepeats, Containers::arraySize(MeshVisualizer3DData),
        &ShadersGLBenchmark::renderSetup,
        &ShadersGLBenchmark::renderTeardown,
        BenchmarkType::GpuTime);

    /* Set up the framebuffer */
    _color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        RenderSize);
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
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
                {FlatGL2D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                {FlatGL2D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}
            });
    }
    #endif

    /* Set up the mesh */
    {
        Trade::MeshData data = Primitives::grid3DSolid(GridSubdivisions,
            Primitives::GridFlag::TextureCoordinates|
            Primitives::GridFlag::Normals|
            Primitives::GridFlag::Tangents);
        Containers::Array<Color4> vertexColors{DirectInit, data.vertexCount(), 0xffffffff_rgbaf};
        Containers::Array<Vector3> bitangents{DirectInit, data.vertexCount(), Vector3{0.0f, 1.0f, 0.0f}};
        Trade::MeshData dataWithVertexColors = MeshTools::interleave(std::move(data), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Color, arrayView(vertexColors)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, arrayView(bitangents)}
        });
        _indices.setData(dataWithVertexColors.indexData());
        _vertices.setData(dataWithVertexColors.vertexData());
        _mesh = MeshTools::compile(dataWithVertexColors, _indices, _vertices);

        /* Instanced variant, if the divisor-related extension is supported */
        #ifndef MAGNUM_TARGET_GLES
        if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::instanced_arrays>())
        #elif defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_WEBGL
        if(GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>() ||
           GL::Context::current().isExtensionSupported<GL::Extensions::EXT::instanced_arrays>() ||
           GL::Context::current().isExtensionSupported<GL::Extensions::NV::instanced_arrays>())
        #else
        if(GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>())
        #endif
        #endif
        {
            _meshInstanced = MeshTools::compile(dataWithVertexColors, _indices, _vertices);
            struct {
                /* Given the way the matrix attribute is specified (column by
                column), it should work for 2D as well */
                Matrix4 transformation{Math::IdentityInit};
                Matrix3x3 normalMatrix{Math::IdentityInit};
                Vector2 textureOffset{0.0f, 0.0f};
                Color4 color{0xffffffff_rgbaf};
                UnsignedInt objectId{0};
            } instanceData[1];
            _meshInstanced.addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
                GenericGL3D::TransformationMatrix{},
                GenericGL3D::NormalMatrix{},
                GenericGL3D::TextureOffset{},
                #ifndef MAGNUM_TARGET_GLES2
                GenericGL3D::ObjectId{}
                #else
                sizeof(UnsignedInt)
                #endif
            );
            /** @todo hmm, this doesn't really issue an instanced draw call, does
                that matter? */
            _meshInstanced.setInstanceCount(1);
        }

        /* Non-indexed variant for GS-less wireframe drawing */
        _meshDuplicated = MeshTools::compile(MeshTools::duplicate(dataWithVertexColors));
    }

    /* Set up the textures */
    {
        const Color4ub white[1] { 0xffffffff_rgba };
        _textureWhite.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1,
                #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
                GL::TextureFormat::RGBA8
                #else
                GL::TextureFormat::RGBA
                #endif
                , {1, 1})
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, white});
        #ifndef MAGNUM_TARGET_GLES2
        _textureWhiteArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, GL::TextureFormat::RGBA8, {1, 1, 1})
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, white});
        #endif
    } {
        const Color4ub blue[1] { 0x0000ffff_rgba };
        _textureBlue.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1,
                #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
                GL::TextureFormat::RGBA8
                #else
                GL::TextureFormat::RGBA
                #endif
                , {1, 1})
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, blue});
        #ifndef MAGNUM_TARGET_GLES2
        _textureBlueArray.setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setStorage(1, GL::TextureFormat::RGBA8, {1, 1, 1})
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, blue});
        #endif
    }

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void ShadersGLBenchmark::renderSetup() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        _framebuffer
            .clearColor(0, Color4{})
            .clearColor(1, Vector4ui{});
    }
    #ifndef MAGNUM_TARGET_GLES
    else
    #endif
    #endif
    {
        _framebuffer.clear(GL::FramebufferClear::Color);
    }
}

void ShadersGLBenchmark::renderTeardown() {
    /* Nothing to do here */
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt> struct UniformTraits;
template<> struct UniformTraits<2> {
    typedef TransformationProjectionUniform2D TransformationProjection;
};
template<> struct UniformTraits<3> {
    typedef TransformationProjectionUniform3D TransformationProjection;
};
#endif

template<UnsignedInt dimensions> void ShadersGLBenchmark::flat() {
    auto&& data = FlatData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::formatString("{}", dimensions));
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & FlatGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
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
    #endif

    FlatGL<dimensions> shader{data.flags
        #ifndef MAGNUM_TARGET_GLES2
        , data.materialCount, data.drawCount
        #endif
    };

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer transformationProjectionUniform{NoCreate};
    GL::Buffer drawUniform{NoCreate};
    GL::Buffer textureTransformationUniform{NoCreate};
    GL::Buffer materialUniform{NoCreate};
    if(data.flags & FlatGL2D::Flag::UniformBuffers) {
        transformationProjectionUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<typename UniformTraits<dimensions>::TransformationProjection>{data.drawCount}};
        drawUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<FlatDrawUniform>{data.drawCount}};
        Containers::Array<FlatMaterialUniform> materialData{data.materialCount};
        materialData[0].setAlphaMask(0.0f);
        materialUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, materialData};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
        if(data.flags & FlatGL2D::Flag::TextureTransformation) {
            textureTransformationUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<TextureTransformationUniform>{data.drawCount}};
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        }
    } else
    #endif
    {
        if(data.flags >= FlatGL2D::Flag::AlphaMask)
            shader.setAlphaMask(0.0f);
    }
    if(data.flags >= FlatGL2D::Flag::Textured) {
        #ifndef MAGNUM_TARGET_GLES2
        if(data.flags & FlatGL2D::Flag::TextureArrays) {
            shader.bindTexture(_textureWhiteArray);
        } else
        #endif
        {
            shader.bindTexture(_textureWhite);
        }
    }

    GL::Mesh* mesh;
    /* InstancedTextureOffset is a superset of TextureTransformation, so
       remove those bits first when deciding if instanced */
    if((data.flags & ~FlatGL2D::Flag::TextureTransformation) & (FlatGL2D::Flag::InstancedTransformation|FlatGL2D::Flag::InstancedTextureOffset
        #ifndef MAGNUM_TARGET_GLES2
        |FlatGL2D::Flag::InstancedObjectId
        #endif
    )) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::instanced_arrays>())
            CORRADE_SKIP(GL::Extensions::ARB::instanced_arrays::string() << "is not supported.");
        #elif defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_WEBGL
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>() &&
        !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::instanced_arrays>() &&
        !GL::Context::current().isExtensionSupported<GL::Extensions::NV::instanced_arrays>())
            CORRADE_SKIP("Required extension is not available.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>())
            CORRADE_SKIP(GL::Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
        #endif
        #endif
        mesh = &_meshInstanced;
    } else {
        mesh = &_mesh;
    }

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != 100; ++i)
        shader.draw(*mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(*mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void ShadersGLBenchmark::phong() {
    auto&& data = PhongData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & PhongGL::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
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
    #endif

    PhongGL shader{data.flags, data.lightCount
        #ifndef MAGNUM_TARGET_GLES2
        , data.materialCount, data.drawCount
        #endif
    };

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer projectionUniform{NoCreate};
    GL::Buffer transformationUniform{NoCreate};
    GL::Buffer drawUniform{NoCreate};
    GL::Buffer materialUniform{NoCreate};
    GL::Buffer lightUniform{NoCreate};
    GL::Buffer textureTransformationUniform{NoCreate};
    if(data.flags & PhongGL::Flag::UniformBuffers) {
        projectionUniform = GL::Buffer{};
        transformationUniform = GL::Buffer{};
        drawUniform = GL::Buffer{};
        materialUniform = GL::Buffer{};
        lightUniform = GL::Buffer{};
        textureTransformationUniform = GL::Buffer{};

        Containers::Array<TransformationUniform3D> transformationData{data.drawCount};
        Containers::Array<PhongDrawUniform> drawData{data.drawCount};
        drawData[0].lightCount = 5; /* Cap at 5 lights, even if more is set */
        Containers::Array<PhongMaterialUniform> materialData{data.materialCount};
        materialData[0]
            /* White ambient so we always have a white output */
            .setAmbientColor(0xffffffff_rgbaf)
            .setAlphaMask(0.0f);
        Containers::Array<PhongLightUniform> lightData{data.lightCount};
        Containers::Array<TextureTransformationUniform> textureTransformationData{data.drawCount};

        #ifndef MAGNUM_TARGET_GLES
        if(data.bufferStorage) {
            if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::buffer_storage>())
                CORRADE_SKIP(GL::Extensions::ARB::buffer_storage::string() << "is not supported.");

            projectionUniform.setStorage(Containers::arrayView({ProjectionUniform3D{}}), {});
            transformationUniform.setStorage(transformationData, {});
            drawUniform.setStorage(drawData, {});
            materialUniform.setStorage(materialData, {});
            lightUniform.setStorage(lightData, {});

            if(data.flags & PhongGL::Flag::TextureTransformation)
                textureTransformationUniform.setStorage(textureTransformationData, {});
        } else
        #endif
        {
            projectionUniform.setData({ProjectionUniform3D{}});
            transformationUniform.setData(transformationData);
            drawUniform.setData(drawData);
            materialUniform.setData(materialData);
            lightUniform.setData(lightData);

            if(data.flags & PhongGL::Flag::TextureTransformation)
                textureTransformationUniform.setData(textureTransformationData);
        }

        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .bindLightBuffer(lightUniform);
        if(data.flags & PhongGL::Flag::TextureTransformation)
            shader.bindTextureTransformationBuffer(textureTransformationUniform);

    } else
    #endif
    {
        /* White ambient so we always have a white output */
        shader.setAmbientColor(0xffffffff_rgbaf);
        if(data.flags >= PhongGL::Flag::AlphaMask)
            shader.setAlphaMask(0.0f);
    }
    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & PhongGL::Flag::TextureArrays) {
        if(data.flags >= PhongGL::Flag::AmbientTexture)
            shader.bindAmbientTexture(_textureWhiteArray);
        if(data.flags >= PhongGL::Flag::DiffuseTexture)
            shader.bindDiffuseTexture(_textureWhiteArray);
        if(data.flags >= PhongGL::Flag::SpecularTexture)
            shader.bindSpecularTexture(_textureWhiteArray);
        if(data.flags >= PhongGL::Flag::NormalTexture)
            shader.bindNormalTexture(_textureBlueArray);
    } else
    #endif
    {
        if(data.flags >= PhongGL::Flag::AmbientTexture)
            shader.bindAmbientTexture(_textureWhite);
        if(data.flags >= PhongGL::Flag::DiffuseTexture)
            shader.bindDiffuseTexture(_textureWhite);
        if(data.flags >= PhongGL::Flag::SpecularTexture)
            shader.bindSpecularTexture(_textureWhite);
        if(data.flags >= PhongGL::Flag::NormalTexture)
            shader.bindNormalTexture(_textureBlue);
    }

    GL::Mesh* mesh;
    /* InstancedTextureOffset is a superset of TextureTransformation, so
       remove those bits first when deciding if instanced */
    if((data.flags & ~PhongGL::Flag::TextureTransformation) & (PhongGL::Flag::InstancedTransformation|PhongGL::Flag::InstancedTextureOffset
        #ifndef MAGNUM_TARGET_GLES2
        |PhongGL::Flag::InstancedObjectId
        #endif
    )) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::draw_instanced>())
            CORRADE_SKIP(GL::Extensions::ARB::draw_instanced::string() << "is not supported.");
        #elif defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_WEBGL
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>() &&
        !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::instanced_arrays>() &&
        !GL::Context::current().isExtensionSupported<GL::Extensions::NV::instanced_arrays>())
            CORRADE_SKIP("Required extension is not available.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::instanced_arrays>())
            CORRADE_SKIP(GL::Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
        #endif
        #endif
        mesh = &_meshInstanced;
    } else {
        mesh = &_mesh;
    }

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != WarmupIterations; ++i)
        shader.draw(*mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(*mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

template<UnsignedInt dimensions> void ShadersGLBenchmark::vertexColor() {
    auto&& data = VertexColorData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::formatString("{}", dimensions));
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & VertexColorGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags >= VertexColorGL2D::Flag::MultiDraw) {
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
    #endif

    VertexColorGL<dimensions> shader{data.flags
        #ifndef MAGNUM_TARGET_GLES2
        , data.drawCount
        #endif
    };

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer transformationProjectionUniform{NoCreate};
    GL::Buffer textureTransformationUniform{NoCreate};
    if(data.flags & VertexColorGL<dimensions>::Flag::UniformBuffers) {
        transformationProjectionUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<typename UniformTraits<dimensions>::TransformationProjection>{data.drawCount}};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform);
    }
    #endif

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != WarmupIterations; ++i)
        shader.draw(_mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(_mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

template<UnsignedInt dimensions> void ShadersGLBenchmark::vector() {
    auto&& data = VectorData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::formatString("{}", dimensions));
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & VectorGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
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
    #endif

    VectorGL<dimensions> shader{data.flags
        #ifndef MAGNUM_TARGET_GLES2
        , data.materialCount, data.drawCount
        #endif
    };
    shader.bindVectorTexture(_textureWhite);

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer transformationProjectionUniform{NoCreate};
    GL::Buffer drawUniform{NoCreate};
    GL::Buffer textureTransformationUniform{NoCreate};
    GL::Buffer materialUniform{NoCreate};
    if(data.flags & VectorGL2D::Flag::UniformBuffers) {
        transformationProjectionUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<typename UniformTraits<dimensions>::TransformationProjection>{data.drawCount}};
        drawUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<VectorDrawUniform>{data.drawCount}};
        materialUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<VectorMaterialUniform>{data.materialCount}};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
        if(data.flags & VectorGL2D::Flag::TextureTransformation) {
            textureTransformationUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, {
                TextureTransformationUniform{}
            }};
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        }
    }
    #endif

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != WarmupIterations; ++i)
        shader.draw(_mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(_mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

template<UnsignedInt dimensions> void ShadersGLBenchmark::distanceFieldVector() {
    auto&& data = DistanceFieldVectorData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::formatString("{}", dimensions));
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & DistanceFieldVectorGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags >= DistanceFieldVectorGL2D::Flag::MultiDraw) {
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
    #endif

    DistanceFieldVectorGL<dimensions> shader{data.flags
        #ifndef MAGNUM_TARGET_GLES2
        , data.materialCount, data.drawCount
        #endif
    };
    shader.bindVectorTexture(_textureWhite);

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer transformationProjectionUniform{NoCreate};
    GL::Buffer drawUniform{NoCreate};
    GL::Buffer materialUniform{NoCreate};
    GL::Buffer textureTransformationUniform{NoCreate};
    if(data.flags & DistanceFieldVectorGL2D::Flag::UniformBuffers) {
        transformationProjectionUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<typename UniformTraits<dimensions>::TransformationProjection>{data.drawCount}};
        drawUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<DistanceFieldVectorDrawUniform>{data.drawCount}};
        materialUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<DistanceFieldVectorMaterialUniform>{data.materialCount}};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
        if(data.flags & DistanceFieldVectorGL2D::Flag::TextureTransformation) {
            textureTransformationUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<TextureTransformationUniform>{data.drawCount}};
            shader.bindTextureTransformationBuffer(textureTransformationUniform);
        }
    }
    #endif

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != WarmupIterations; ++i)
        shader.draw(_mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(_mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void ShadersGLBenchmark::meshVisualizer2D() {
    auto&& data = MeshVisualizer2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    /* Checks verbatim copied from MeshVisualizerGLTest::construct2D() */
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

    #ifndef MAGNUM_TARGET_GLES2
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
    #endif

    MeshVisualizerGL2D shader{data.flags};
    shader.setViewportSize(Vector2{RenderSize});
    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & (MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::VertexId|MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId
        #ifndef MAGNUM_TARGET_WEBGL
        |MeshVisualizerGL2D::Flag::PrimitiveId
        #endif
    ))
        shader.bindColorMapTexture(_textureWhite);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer transformationProjectionUniform{NoCreate};
    GL::Buffer drawUniform{NoCreate};
    GL::Buffer materialUniform{NoCreate};
    if(data.flags & MeshVisualizerGL2D::Flag::UniformBuffers) {
        transformationProjectionUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<TransformationProjectionUniform2D>{data.drawCount}};
        drawUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<MeshVisualizerDrawUniform2D>{data.drawCount}};
        Containers::Array<MeshVisualizerMaterialUniform> materialData{data.materialCount};
        materialData[0].setWireframeColor(0xffffffff_rgbaf);
        materialUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, materialData};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
    } else
    #endif
    {
        if(data.flags >= MeshVisualizerGL2D::Flag::Wireframe)
            shader.setWireframeColor(0xffffffff_rgbaf);
    }

    GL::Mesh* mesh;
    if(data.flags >= MeshVisualizerGL2D::Flag::NoGeometryShader) {
        mesh = &_meshDuplicated;
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(data.flags & MeshVisualizerGL2D::Flag::InstancedObjectId) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::draw_instanced>())
            CORRADE_SKIP(GL::Extensions::ARB::draw_instanced::string() << "is not supported.");
        #endif
        mesh = &_meshInstanced;
    }
    #endif
    else {
        mesh = &_mesh;
    }

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != WarmupIterations; ++i)
        shader.draw(*mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(*mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void ShadersGLBenchmark::meshVisualizer3D() {
    auto&& data = MeshVisualizer3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & MeshVisualizerGL3D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    /* Checks verbatim copied from MeshVisualizerGLTest:.construct3D() */
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

    #ifndef MAGNUM_TARGET_GLES2
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
    #endif

    MeshVisualizerGL3D shader{data.flags};
    shader.setViewportSize(Vector2{RenderSize});

    #ifndef MAGNUM_TARGET_GLES2
    if(data.flags & (MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::VertexId|MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId
        #ifndef MAGNUM_TARGET_WEBGL
        |MeshVisualizerGL3D::Flag::PrimitiveId
        #endif
    ))
        shader.bindColorMapTexture(_textureWhite);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    GL::Buffer projectionUniform{NoCreate};
    GL::Buffer transformationUniform{NoCreate};
    GL::Buffer drawUniform{NoCreate};
    GL::Buffer materialUniform{NoCreate};
    if(data.flags & MeshVisualizerGL3D::Flag::UniformBuffers) {
        projectionUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, {
            ProjectionUniform3D{}
        }};
        transformationUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<TransformationUniform3D>{data.drawCount}};
        drawUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, Containers::Array<MeshVisualizerDrawUniform3D>{data.drawCount}};
        Containers::Array<MeshVisualizerMaterialUniform> materialData{data.materialCount};
        materialData[0].setWireframeColor(0xffffffff_rgbaf);
        materialUniform = GL::Buffer{GL::Buffer::TargetHint::Uniform, materialData};
        shader.bindProjectionBuffer(projectionUniform)
            .bindTransformationBuffer(transformationUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform);
    } else
    #endif
    {
        if(data.flags >= MeshVisualizerGL3D::Flag::Wireframe)
            shader.setWireframeColor(0xffffffff_rgbaf);
    }

    GL::Mesh* mesh;
    if(data.flags >= MeshVisualizerGL3D::Flag::NoGeometryShader)
        mesh = &_meshDuplicated;
    #ifndef MAGNUM_TARGET_GLES2
    else if(data.flags & MeshVisualizerGL3D::Flag::InstancedObjectId)
        mesh = &_meshInstanced;
    #endif
    else
        mesh = &_mesh;

    /* Warmup run */
    /** @todo make this possible to do inside CORRADE_BENCHMARK() */
    for(std::size_t i = 0; i != WarmupIterations; ++i)
        shader.draw(*mesh);

    CORRADE_BENCHMARK(BenchmarkIterations)
        shader.draw(*mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(SHADERS_TEST_DIR, "BenchmarkFiles/trivial.tga"),
        DebugTools::CompareImageToFile{_manager});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::ShadersGLBenchmark)
