#ifndef Magnum_Shaders_MeshVisualizerGL_h
#define Magnum_Shaders_MeshVisualizerGL_h
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Class @ref Magnum::Shaders::MeshVisualizerGL2D, @ref Magnum::Shaders::MeshVisualizerGL3D
 * @m_since_latest
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Utility/Move.h>
#include <Corrade/Utility/Utility.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

#ifndef MAGNUM_TARGET_GLES2
#include <initializer_list>
#endif

namespace Magnum { namespace Shaders {

namespace Implementation {

class MAGNUM_SHADERS_EXPORT MeshVisualizerGLBase: public GL::AbstractShaderProgram {
    protected:
        enum class FlagBase: UnsignedInt {
            /* Unlike the public Wireframe flag, this one doesn't include
               NoGeometryShader on ES2 as that would make the checks too
               complex */
            Wireframe = 1 << 0,
            NoGeometryShader = 1 << 1,
            InstancedTransformation = 1 << 13,
            #ifndef MAGNUM_TARGET_GLES2
            ObjectId = 1 << 12,
            InstancedObjectId = (1 << 2)|ObjectId,
            ObjectIdTexture = 1 << 14,
            TextureTransformation = 1 << 15,
            InstancedTextureOffset = (1 << 16)|TextureTransformation,
            VertexId = 1 << 3,
            PrimitiveId = 1 << 4,
            PrimitiveIdFromVertexId = (1 << 5)|PrimitiveId,
            /* bit 6, 7, 8, 9 used by 3D-specific TBN visualization */
            UniformBuffers = 1 << 10,
            ShaderStorageBuffers = UniformBuffers|(1 << 19),
            MultiDraw = UniformBuffers|(1 << 11),
            TextureArrays = 1 << 17,
            DynamicPerVertexJointCount = 1 << 18
            #endif
        };
        typedef Containers::EnumSet<FlagBase> FlagsBase;

        CORRADE_ENUMSET_FRIEND_OPERATORS(FlagsBase)

        explicit MeshVisualizerGLBase(NoInitT) {}

        explicit MeshVisualizerGLBase(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        static MAGNUM_SHADERS_LOCAL void assertExtensions(const FlagsBase flags);
        static MAGNUM_SHADERS_LOCAL GL::Version setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs, const FlagsBase flags
            #ifndef MAGNUM_TARGET_GLES2
            , UnsignedInt dimensions, UnsignedInt jointCount, UnsignedInt perVertexJointCount, UnsignedInt secondaryPerVertexJointCount, UnsignedInt materialCount, UnsignedInt drawCount
            #ifndef MAGNUM_TARGET_WEBGL
            , UnsignedInt perInstanceJointCountUniform, UnsignedInt perVertexJointCountUniform
            #endif
            #endif
        );

        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGLBase& setPerVertexJointCount(UnsignedInt count, UnsignedInt secondaryCount);
        MeshVisualizerGLBase& setTextureMatrix(const Matrix3& matrix);
        MeshVisualizerGLBase& setTextureLayer(UnsignedInt layer);
        MeshVisualizerGLBase& setObjectId(UnsignedInt id);
        #endif
        MeshVisualizerGLBase& setColor(const Color4& color);
        MeshVisualizerGLBase& setWireframeColor(const Color4& color);
        MeshVisualizerGLBase& setWireframeWidth(Float width);
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGLBase& setColorMapTransformation(Float offset, Float scale);
        MeshVisualizerGLBase& setPerInstanceJointCount(UnsignedInt count);
        MeshVisualizerGLBase& setDrawOffset(UnsignedInt offset);
        MeshVisualizerGLBase& bindColorMapTexture(GL::Texture2D& texture);
        MeshVisualizerGLBase& bindObjectIdTexture(GL::Texture2D& texture);
        MeshVisualizerGLBase& bindObjectIdTexture(GL::Texture2DArray& texture);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGLBase& bindTextureTransformationBuffer(GL::Buffer& buffer);
        MeshVisualizerGLBase& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);
        MeshVisualizerGLBase& bindMaterialBuffer(GL::Buffer& buffer);
        MeshVisualizerGLBase& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);
        MeshVisualizerGLBase& bindJointBuffer(GL::Buffer& buffer);
        MeshVisualizerGLBase& bindJointBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #endif

        FlagsBase _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _jointCount{},
            _perVertexJointCount{},
            _secondaryPerVertexJointCount{},
            _materialCount{},
            _drawCount{};
        #endif
        Int _viewportSizeUniform{0},
            _colorUniform{1},
            _wireframeColorUniform{2},
            _wireframeWidthUniform{3},
            _smoothnessUniform{4};
        #ifndef MAGNUM_TARGET_GLES2
        Int _colorMapOffsetScaleUniform{5},
            _objectIdUniform{6},
            _textureMatrixUniform{7},
            _textureLayerUniform{8},
            /* 9 to 13 different between MeshVisualizerGL2D and
               MeshVisualizerGL3D */
            _jointMatricesUniform{14},
            _perInstanceJointCountUniform, /* 14 + jointCount */
            /* Used instead of all other uniforms except viewportSize when
               Flag::UniformBuffers is set, so it can alias them */
            _drawOffsetUniform{1},
            _perVertexJointCountUniform; /* 15 + jointCount, or 2 with UBOs */
        #endif
};

}

/**
@brief 2D mesh visualization OpenGL shader
@m_since_latest

Visualizes wireframe, per-vertex/per-instance object ID or primitive ID of 2D
meshes. You need to provide the @ref Position attribute in your triangle mesh.
Use @ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

@m_class{m-row}

@parblock

@m_div{m-col-m-4 m-col-t-6 m-push-m-2 m-nopadt m-nopadx}
@image html shaders-meshvisualizer2d.png width=256px
@m_enddiv

@m_div{m-col-m-4 m-col-t-6 m-push-m-2 m-nopadt m-nopadx}
@image html shaders-meshvisualizer2d-primitiveid.png width=256px
@m_enddiv

@endparblock

The shader expects that you enable wireframe visualization by passing an
appropriate @ref Flag to @ref Configuration::setFlags() --- there's no default
behavior with nothing enabled. The shader is a 2D variant of
@ref MeshVisualizerGL3D with mostly identical workflow. See its documentation
for more information, workflows that differ are shown below.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section Shaders-MeshVisualizerGL2D-instancing Instanced rendering

Enabling @ref Flag::InstancedTransformation will turn the shader into an
instanced one. It'll take per-instance transformation from the
@ref TransformationMatrix attribute, applying it before the matrix set by
@ref setTransformationProjectionMatrix(). The snippet below shows adding a
buffer with per-instance transformation to a mesh:

@snippet Shaders-gl.cpp MeshVisualizerGL2D-usage-instancing

If @ref Flag::ObjectIdTexture is used and @ref Flag::InstancedTextureOffset is
enabled, the @ref TextureOffset attribute (or @ref TextureOffsetLayer in case
@ref Flag::TextureArrays is enabled as well) then can supply per-instance
texture offset (or offset and layer).

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
@requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
    @gl_extension{EXT,instanced_arrays} or @gl_extension{NV,instanced_arrays}
    in OpenGL ES 2.0.
@requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays} in WebGL
    1.0.

@section Shaders-MeshVisualizerGL2D-ubo Uniform buffers

Unlike with @ref Shaders-MeshVisualizerGL2D-ubo "uniform buffers in the 3D variant",
because the shader doesn't need a separate projection and transformation
matrix, a combined one is supplied via a @ref TransformationProjectionUniform2D
buffer bound with @ref bindTransformationProjectionBuffer(), and
a trimmed-down @ref MeshVisualizerDrawUniform2D is used instead of
@ref MeshVisualizerDrawUniform3D. The rest is the same.
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizerGL2D: public Implementation::MeshVisualizerGLBase {
    public:
        /* MSVC needs dllexport here as well */
        class MAGNUM_SHADERS_EXPORT Configuration;
        class CompileState;

        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}.
         */
        typedef typename GenericGL2D::Position Position;

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc MeshVisualizerGL3D::TextureCoordinates */
        typedef GenericGL2D::TextureCoordinates TextureCoordinates;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint ids
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4ui}.
         * Used only if @ref perVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::JointIds JointIds;

        /**
         * @brief Weights
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Used only if
         * @ref perVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::Weights Weights;

        /**
         * @brief Secondary joint ids
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4ui}. Used only if
         * @ref secondaryPerVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::SecondaryJointIds SecondaryJointIds;

        /**
         * @brief Secondary weights
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Used only if
         * @ref secondaryPerVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::SecondaryWeights SecondaryWeights;
        #endif

        /** @copydoc MeshVisualizerGL3D::VertexIndex */
        typedef GL::Attribute<4, Float> VertexIndex;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) object ID
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,UnsignedInt}. Used only if
         * @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        typedef GenericGL2D::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix3}. Used only if
         * @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef GenericGL2D::TransformationMatrix TransformationMatrix;

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc MeshVisualizerGL3D::TextureOffset */
        typedef typename GenericGL2D::TextureOffset TextureOffset;

        /** @copydoc MeshVisualizerGL3D::TextureOffsetLayer */
        typedef typename GenericGL2D::TextureOffsetLayer TextureOffsetLayer;
        #endif

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = GenericGL2D::ColorOutput
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
         */
        enum class Flag: UnsignedInt {
            /**
             * Visualize wireframe. On OpenGL ES 2.0 and WebGL this also
             * enables @ref Flag::NoGeometryShader.
             */
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            Wireframe = 1 << 0,
            #else
            Wireframe = (1 << 0) | (1 << 1),
            #endif

            /**
             * Don't use a geometry shader for wireframe visualization. If
             * enabled, you might need to provide also the @ref VertexIndex
             * attribute in the mesh. On OpenGL ES 2.0 and WebGL enabled
             * alongside @ref Flag::Wireframe.
             */
            NoGeometryShader = 1 << 1,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Visualize object ID set via @ref setObjectId() or
             * @ref MeshVisualizerDrawUniform2D::objectId. Since the ID is
             * uniform for the whole draw, this feature is mainly useful in
             * multidraw scenarios or when combined with
             * @ref Flag::InstancedObjectId. Mutually exclusive with
             * @ref Flag::VertexId and @ref Flag::PrimitiveId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             */
            ObjectId = 1 << 12,

            /**
             * Visualize instanced object ID. You need to provide the
             * @ref ObjectId attribute in the mesh, which then gets summed with
             * the ID coming from @ref setObjectId() or
             * @ref MeshVisualizerDrawUniform2D::objectId. Implicitly enables
             * @ref Flag::ObjectId. Mutually exclusive with @ref Flag::VertexId
             * and @ref Flag::PrimitiveId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 2)|ObjectId,

            /**
             * Object ID texture. Retrieves object IDs from a texture bound
             * with @ref bindObjectIdTexture(), outputting a sum of the object
             * ID texture, the ID coming from @ref setObjectId() or
             * @ref MeshVisualizerDrawUniform2D::objectId and possibly also the
             * per-vertex ID, if @ref Flag::InstancedObjectId is enabled as
             * well. Implicitly enables @ref Flag::ObjectId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             */
            ObjectIdTexture = (1 << 14)|ObjectId,

            /** @copydoc MeshVisualizerGL3D::Flag::VertexId */
            VertexId = 1 << 3,

            #ifndef MAGNUM_TARGET_WEBGL
            /** @copydoc MeshVisualizerGL3D::Flag::PrimitiveId */
            PrimitiveId = 1 << 4,
            #endif

            /** @copydoc MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId */
            #ifndef MAGNUM_TARGET_WEBGL
            PrimitiveIdFromVertexId = (1 << 5)|PrimitiveId,
            #else
            PrimitiveIdFromVertexId = (1 << 5)|(1 << 4),
            #endif
            #endif

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation matrix from the @ref TransformationMatrix
             * attribute and uses it together with the matrix coming from
             * @ref setTransformationProjectionMatrix() or
             * @ref TransformationProjectionUniform2D::transformationProjectionMatrix
             * (first the per-instance, then the uniform matrix). See
             * @ref Shaders-MeshVisualizerGL2D-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since_latest
             */
            InstancedTransformation = 1 << 13,

            #ifndef MAGNUM_TARGET_GLES2
            /** @copydoc MeshVisualizerGL3D::Flag::TextureTransformation */
            TextureTransformation = 1 << 15,

            /**
             * Instanced texture offset for an object ID texture. Retrieves a
             * per-instance offset vector from the @ref TextureOffset attribute
             * and uses it together with the matrix coming from
             * @ref setTextureMatrix() or
             * @ref TextureTransformationUniform::rotationScaling and
             * @ref TextureTransformationUniform::offset (first the
             * per-instance vector, then the uniform matrix). Instanced texture
             * scaling and rotation is not supported at the moment, you can
             * specify that only via the uniform @ref setTextureMatrix().
             * Implicitly enables @ref Flag::TextureTransformation. See
             * @ref Shaders-MeshVisualizerGL3D-instancing for more information.
             *
             * If @ref Flag::TextureArrays is set as well, a three-component
             * @ref TextureOffsetLayer attribute can be used instead of
             * @ref TextureOffset to specify per-instance texture layer, which
             * gets added to the uniform layer numbers set by
             * @ref setTextureLayer() or
             * @ref TextureTransformationUniform::layer.
             * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4} and
             *      @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             * @todoc rewrite the ext requirements once we have more textures
             */
            InstancedTextureOffset = (1 << 16)|TextureTransformation,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer()
             * and @ref bindMaterialBuffer() instead of direct uniform setters.
             * @see @ref Flag::ShaderStorageBuffers
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 10,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Use shader storage buffers. Superset of functionality provided
             * by @ref Flag::UniformBuffers, compared to it doesn't have any
             * size limits on @ref Configuration::setJointCount(),
             * @relativeref{Configuration,setMaterialCount()} and
             * @relativeref{Configuration,setDrawCount()} in exchange for
             * potentially more costly access and narrower platform support.
             * @requires_gl43 Extension @gl_extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage buffers are not available in
             *      OpenGL ES 3.0 and older.
             * @requires_gles Shader storage buffers are not available in
             *      WebGL.
             * @m_since_latest
             */
            ShaderStorageBuffers = UniformBuffers|(1 << 19),
            #endif

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and combines the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
             * and related APIs pick up per-draw parameters directly, without
             * having to rebind the uniform buffers or specify
             * @ref setDrawOffset() before each draw. In a non-multidraw
             * scenario, @glsl gl_DrawID @ce is @cpp 0 @ce, which means a
             * shader with this flag enabled can be used for regular draws as
             * well.
             * @requires_gl46 Extension @gl_extension{ARB,uniform_buffer_object}
             *      and @gl_extension{ARB,shader_draw_parameters}
             * @requires_es_extension OpenGL ES 3.0 and extension
             *      @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
             *      (unlisted). While the extension alone needs only OpenGL ES
             *      2.0, the shader implementation relies on uniform buffers,
             *      which require OpenGL ES 3.0.
             * @requires_webgl_extension WebGL 2.0 and extension
             *      @webgl_extension{ANGLE,multi_draw}. While the extension
             *      alone needs only WebGL 1.0, the shader implementation
             *      relies on uniform buffers, which require WebGL 2.0.
             * @m_since_latest
             */
            MultiDraw = UniformBuffers|(1 << 11),

            /** @copydoc MeshVisualizerGL3D::Flag::TextureArrays */
            TextureArrays = 1 << 17,

            /**
             * Dynamic per-vertex joint count for skinning. Uses only the first
             * M / N primary / secondary components defined by
             * @ref setPerVertexJointCount() instead of
             * all primary / secondary components defined by
             * @ref Configuration::setJointCount() at shader compilation time.
             * Useful in order to avoid having a shader permutation defined for
             * every possible joint count. Unfortunately it's not possible to
             * make use of default values for unspecified input components as
             * the last component is always @cpp 1.0 @ce instead of
             * @cpp 0.0 @ce, on the other hand dynamically limiting the joint
             * count can reduce the time spent executing the vertex shader
             * compared to going through the full set of per-vertex joints
             * always.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Skinning requires integer support in shaders,
             *      which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Skinning requires integer support in shaders,
             *      which is not available in WebGL 1.0.
             * @m_since_latest
             */
            DynamicPerVertexJointCount = 1 << 18,
            #endif
        };

        /**
         * @brief Flags
         *
         * @see @ref flags(), @ref Configuration::setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Compile asynchronously
         * @m_since_latest
         *
         * Compared to @ref MeshVisualizerGL2D(const Configuration&) can
         * perform an asynchronous compilation and linking. See
         * @ref shaders-async for more information.
         * @see @ref MeshVisualizerGL2D(CompileState&&)
         */
        /* No default value, consistently with MeshVisualizerGL2D(Configuration) */
        static CompileState compile(const Configuration& configuration);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Compile asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Compile for a multi-draw scenario asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Constructor
         * @m_since_latest
         */
        /* No default value, as at least one Flag has to be set */
        explicit MeshVisualizerGL2D(const Configuration& configuration);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref MeshVisualizerGL2D(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use MeshVisualizerGL2D(const Configuration& instead") MeshVisualizerGL2D(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @m_deprecated_since_latest Use @ref MeshVisualizerGL2D(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        explicit CORRADE_DEPRECATED("use MeshVisualizerGL2D(const Configuration& instead") MeshVisualizerGL2D(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Finalize an asynchronous compilation
         * @m_since_latest
         *
         * Takes an asynchronous compilation state returned by @ref compile()
         * and forms a ready-to-use shader object. See @ref shaders-async for
         * more information.
         */
        explicit MeshVisualizerGL2D(CompileState&& state);

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to a moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit MeshVisualizerGL2D(NoCreateT) noexcept: Implementation::MeshVisualizerGLBase{NoCreate} {}

        /** @brief Copying is not allowed */
        MeshVisualizerGL2D(const MeshVisualizerGL2D&) = delete;

        /** @brief Move constructor */
        MeshVisualizerGL2D(MeshVisualizerGL2D&&) noexcept = default;

        /** @brief Copying is not allowed */
        MeshVisualizerGL2D& operator=(const MeshVisualizerGL2D&) = delete;

        /** @brief Move assignment */
        MeshVisualizerGL2D& operator=(MeshVisualizerGL2D&&) noexcept = default;

        /**
         * @brief Flags
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const {
            return Flags(UnsignedInt(Implementation::MeshVisualizerGLBase::_flags));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         * @m_since_latest
         *
         * If @ref Flag::UniformBuffers is not set, this is the number of joint
         * matrices accepted by @ref setJointMatrices() / @ref setJointMatrix().
         * If @ref Flag::UniformBuffers is set, this is the statically defined
         * size of the @ref TransformationUniform2D uniform buffer bound with
         * @ref bindJointBuffer(). Has no use if @ref Flag::ShaderStorageBuffers
         * is set.
         * @see @ref Configuration::setJointCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt jointCount() const { return _jointCount; }

        /**
         * @brief Per-vertex joint count
         * @m_since_latest
         *
         * Returns the value set with @ref Configuration::setJointCount(). If
         * @ref Flag::DynamicPerVertexJointCount is set, the count can be
         * additionally modified per-draw using @ref setPerVertexJointCount().
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt perVertexJointCount() const { return _perVertexJointCount; }

        /**
         * @brief Secondary per-vertex joint count
         * @m_since_latest
         *
         * Returns the value set with @ref Configuration::setJointCount(). If
         * @ref Flag::DynamicPerVertexJointCount is set, the count can be
         * additionally modified per-draw using @ref setPerVertexJointCount().
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt secondaryPerVertexJointCount() const { return _secondaryPerVertexJointCount; }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the @ref MeshVisualizerMaterialUniform
         * uniform buffer bound with @ref bindMaterialBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set and @ref Flag::ShaderStorageBuffers
         * is not set.
         * @see @ref Configuration::setMaterialCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D and
         * @ref MeshVisualizerDrawUniform2D uniform buffers bound with
         * @ref bindTransformationProjectionBuffer() and @ref bindDrawBuffer().
         * Has use only if @ref Flag::UniformBuffers is set and
         * @ref Flag::ShaderStorageBuffers is not set.
         * @see @ref Configuration::setDrawCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set dynamic per-vertex skinning joint count
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Allows reducing the count of iterated joints for a particular draw
         * call, making it possible to use a single shader with meshes that
         * contain different count of per-vertex joints. See
         * @ref Flag::DynamicPerVertexJointCount for more information. As the
         * joint count is tied to the mesh layout, this is a per-draw-call
         * setting even in case of @ref Flag::UniformBuffers instead of being
         * a value in @ref MeshVisualizerDrawUniform2D. Initial value is same
         * as @ref perVertexJointCount() and
         * @ref secondaryPerVertexJointCount().
         *
         * Expects that @ref Flag::DynamicPerVertexJointCount is set,
         * @p count is not larger than @ref perVertexJointCount() and
         * @p secondaryCount not larger than @ref secondaryPerVertexJointCount().
         * @see @ref Configuration::setJointCount()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders, which
         *      is not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& setPerVertexJointCount(UnsignedInt count, UnsignedInt secondaryCount = 0) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setPerVertexJointCount(count, secondaryCount));
        }
        #endif

        /** @{
         * @name Uniform setters
         *
         * Used only if @ref Flag::UniformBuffers is not set.
         */

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix. If
         * @ref Flag::InstancedTransformation is set, the per-instance
         * transformation matrix coming from the @ref TransformationMatrix
         * attribute is applied first, before this one.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationProjectionUniform2D::transformationProjectionMatrix
         * and call @ref bindTransformationProjectionBuffer() instead.
         */
        MeshVisualizerGL2D& setTransformationProjectionMatrix(const Matrix3& matrix);

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc MeshVisualizerGL3D::setTextureMatrix() */
        MeshVisualizerGL2D& setTextureMatrix(const Matrix3& matrix) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setTextureMatrix(matrix));
        }

        /** @copydoc MeshVisualizerGL3D::setTextureLayer() */
        MeshVisualizerGL2D& setTextureLayer(UnsignedInt layer) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setTextureLayer(layer));
        }
        #endif

        /**
         * @brief Set viewport size
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Wireframe is enabled and geometry
         * shaders are used, otherwise it does nothing. Initial value is a zero
         * vector.
         */
        MeshVisualizerGL2D& setViewportSize(const Vector2& size);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set object ID
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::ObjectId is enabled. Initial value is
         * @cpp 0 @ce. If @ref Flag::InstancedObjectId is enabled as well, this
         * value is added to the ID coming from the @ref ObjectId attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerDrawUniform2D::objectId and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& setObjectId(UnsignedInt id) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setObjectId(id));
        }
        #endif

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. Expects that either
         * @ref Flag::Wireframe or @ref Flag::InstancedObjectId /
         * @ref Flag::PrimitiveId / @ref Flag::PrimitiveIdFromVertexId is
         * enabled. In case of the latter, the color is multiplied with the
         * color map coming from @ref bindColorMapTexture().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::color and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL2D& setColor(const Color4& color) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setColor(color));
        }

        /**
         * @brief Set wireframe color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x000000ff_rgbaf @ce. Expects that
         * @ref Flag::Wireframe is enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::wireframeColor and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL2D& setWireframeColor(const Color4& color) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setWireframeColor(color));
        }

        /**
         * @brief Set wireframe width
         * @return Reference to self (for method chaining)
         *
         * The value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 1.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::wireframeWidth and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL2D& setWireframeWidth(Float width) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setWireframeWidth(width));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc MeshVisualizerGL3D::setColorMapTransformation() */
        MeshVisualizerGL2D& setColorMapTransformation(Float offset, Float scale) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setColorMapTransformation(offset, scale));
        }
        #endif

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 2.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::smoothness and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL2D& setSmoothness(Float smoothness);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set joint matrices
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Initial values are identity transformations. Expects that the size
         * of the @p matrices array is not larger than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform2D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @see @ref setJointMatrix(UnsignedInt, const Matrix3&)
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& setJointMatrices(Containers::ArrayView<const Matrix3> matrices);

        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL2D& setJointMatrices(std::initializer_list<Matrix3> matrices);

        /**
         * @brief Set joint matrix for given joint
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setJointMatrices() updates just a single joint matrix.
         * Expects that @p id is less than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform2D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& setJointMatrix(UnsignedInt id, const Matrix3& matrix);

        /**
         * @brief Set per-instance joint count
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Offset added to joint IDs in the @ref JointIds and
         * @ref SecondaryJointIds in instanced draws. Should be less than
         * @ref jointCount(). Initial value is @cpp 0 @ce, meaning every
         * instance will use the same joint matrices, setting it to a non-zero
         * value causes the joint IDs to be interpreted as
         * @glsl gl_InstanceID*count + jointId @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerDrawUniform2D::perInstanceJointCount and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& setPerInstanceJointCount(UnsignedInt count) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setPerInstanceJointCount(count));
        }
        #endif

        /**
         * @}
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @{
         * @name Uniform / shader storage buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Set a draw offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Specifies which item in the @ref TransformationProjectionUniform2D
         * and @ref MeshVisualizerDrawUniform2D buffers bound with
         * @ref bindTransformationProjectionBuffer() and @ref bindDrawBuffer()
         * should be used for current draw. Expects that
         * @ref Flag::UniformBuffers is set and @p offset is less than
         * @ref drawCount(). Initial value is @cpp 0 @ce, if @ref drawCount()
         * is @cpp 1 @ce, the function is a no-op as the shader assumes draw
         * offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& setDrawOffset(UnsignedInt offset) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::setDrawOffset(offset));
        }

        /**
         * @brief Bind a transformation and projection uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationUniform3D. At the very least you need to call
         * also @ref bindDrawBuffer() and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL2D& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a draw uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref MeshVisualizerDrawUniform2D. At the very least you need to call
         * also @ref bindTransformationProjectionBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL2D& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /** @copydoc MeshVisualizerGL3D::bindTextureTransformationBuffer(GL::Buffer&) */
        MeshVisualizerGL2D& bindTextureTransformationBuffer(GL::Buffer& buffer) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindTextureTransformationBuffer(buffer));
        }
        /** @copydoc MeshVisualizerGL3D::bindTextureTransformationBuffer(GL::Buffer&, GLintptr, GLsizeiptr) */
        MeshVisualizerGL2D& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindTextureTransformationBuffer(buffer, offset, size));
        }

        /**
         * @brief Bind a material uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref MeshVisualizerMaterialUniform. At the very least you need to
         * call also @ref bindTransformationProjectionBuffer() and
         * @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& bindMaterialBuffer(GL::Buffer& buffer) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindMaterialBuffer(buffer));
        }
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL2D& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindMaterialBuffer(buffer, offset, size));
        }

        /**
         * @brief Bind a joint matrix uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref jointCount() instances of
         * @ref TransformationUniform2D.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL2D& bindJointBuffer(GL::Buffer& buffer) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindJointBuffer(buffer));
        }
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL2D& bindJointBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindJointBuffer(buffer, offset, size));
        }

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc MeshVisualizerGL3D::bindColorMapTexture() */
        MeshVisualizerGL2D& bindColorMapTexture(GL::Texture2D& texture) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindColorMapTexture(texture));
        }

        /** @copydoc MeshVisualizerGL3D::bindObjectIdTexture(GL::Texture2D&) */
        MeshVisualizerGL2D& bindObjectIdTexture(GL::Texture2D& texture) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindObjectIdTexture(texture));
        }

        /** @copydoc MeshVisualizerGL3D::bindObjectIdTexture(GL::Texture2DArray&) */
        MeshVisualizerGL2D& bindObjectIdTexture(GL::Texture2DArray& texture) {
            return static_cast<MeshVisualizerGL2D&>(Implementation::MeshVisualizerGLBase::bindObjectIdTexture(texture));
        }
        #endif

        /**
         * @}
         */

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(MeshVisualizerGL2D)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit MeshVisualizerGL2D(NoInitT): Implementation::MeshVisualizerGLBase{NoInit} {}

        Int _transformationProjectionMatrixUniform{9};
};

/**
@brief Configuration
@m_since_latest

@see @ref MeshVisualizerGL2D(const Configuration&),
    @ref compile(const Configuration&)
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizerGL2D::Configuration {
    public:
        explicit Configuration() = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * At least one of @ref Flag::Wireframe, @ref Flag::ObjectId,
         * @ref Flag::VertexId, @ref Flag::PrimitiveId or
         * @ref Flag::PrimitiveIdFromVertexId is expected to be enabled. No
         * flags are set by default.
         * @see @ref MeshVisualizerGL2D::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt jointCount() const { return _jointCount; }

        /**
         * @brief Per-vertex joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt perVertexJointCount() const { return _perVertexJointCount; }

        /**
         *@brief Secondary per-vertex joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt secondaryPerVertexJointCount() const { return _secondaryPerVertexJointCount; }

        /**
         * @brief Set joint count
         *
         * If @ref Flag::UniformBuffers isn't set, @p count describes an upper
         * bound on how many joint matrices get supplied to each draw with
         * @ref setJointMatrices() / @ref setJointMatrix().
         *
         * If @ref Flag::UniformBuffers is set, @p count describes size of a
         * @ref TransformationUniform2D buffer bound with
         * @ref bindJointBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(TransformationUniform2D) @ce has to be
         * within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-vertex joints index into
         * the array offset by @ref MeshVisualizerDrawUniform2D::jointOffset.
         *
         * The @p perVertexCount and @p secondaryPerVertexCount parameters
         * describe how many components are taken from @ref JointIds /
         * @ref Weights and @ref SecondaryJointIds / @ref SecondaryWeights
         * attributes. Both values are expected to not be larger than
         * @cpp 4 @ce, setting either of these to @cpp 0 @ce means given
         * attribute is not used at all. If both @p perVertexCount and
         * @p secondaryPerVertexCount are set to @cpp 0 @ce, skinning is not
         * performed. Unless @ref Flag::ShaderStorageBuffers is set, if either
         * of them is non-zero, @p count is expected to be non-zero as well.
         *
         * Default value for all three is @cpp 0 @ce.
         * @see @ref MeshVisualizerGL2D::jointCount(),
         *      @ref MeshVisualizerGL2D::perVertexJointCount(),
         *      @ref MeshVisualizerGL2D::secondaryPerVertexJointCount(),
         *      @ref Flag::DynamicPerVertexJointCount,
         *      @ref MeshVisualizerGL2D::setPerVertexJointCount()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        Configuration& setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount = 0);

        /**
         * @brief Material count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Set material count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref MeshVisualizerMaterialUniform buffer bound with
         * @ref bindMaterialBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(MeshVisualizerMaterialUniform) @ce has to
         * be within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-draw materials are
         * specified via @ref MeshVisualizerDrawUniform2D::materialId. Default
         * value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setDrawCount(),
         *      @ref MeshVisualizerGL2D::materialCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setMaterialCount(UnsignedInt count) {
            _materialCount = count;
            return *this;
        }

        /**
         * @brief Draw count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }

        /**
         * @brief Set draw count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref TransformationProjectionUniform2D /
         * @ref MeshVisualizerDrawUniform2D / @ref TextureTransformationUniform
         * buffer bound with @ref bindTransformationProjectionBuffer(),
         * @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer().
         * Uniform buffers have a statically defined size and the maximum of
         * @cpp count*sizeof(TransformationProjectionUniform2D) @ce,
         * @cpp count*sizeof(MeshVisualizerDrawUniform2D) @ce and
         * @cpp count*sizeof(TextureTransformationUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffers are
         * unbounded and @p count is ignored. The draw offset is set via
         * @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setMaterialCount(),
         *      @ref MeshVisualizerGL2D::drawCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setDrawCount(UnsignedInt count) {
            _drawCount = count;
            return *this;
        }
        #endif

    private:
        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _jointCount = 0,
            _perVertexJointCount = 0,
            _secondaryPerVertexJointCount = 0,
            _materialCount = 1,
            _drawCount = 1;
        #endif
};

/**
@brief Asynchronous compilation state
@m_since_latest

Returned by @ref compile(). See @ref shaders-async for more information.
*/
class MeshVisualizerGL2D::CompileState: public MeshVisualizerGL2D {
    /* Everything deliberately private except for the inheritance */
    friend class MeshVisualizerGL2D;

    explicit CompileState(NoCreateT): MeshVisualizerGL2D{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(MeshVisualizerGL2D&& shader, GL::Shader&& vert, GL::Shader&& frag
        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        , GL::Shader* geom
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , GL::Version version
        #endif
    ): MeshVisualizerGL2D{Utility::move(shader)}, _vert{Utility::move(vert)}, _frag{Utility::move(frag)}
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , _version{version}
        #endif
    {
        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        if(geom) _geom = Implementation::GLShaderWrapper{Utility::move(*geom)};
        #endif
    }

    Implementation::GLShaderWrapper _vert, _frag;
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    Implementation::GLShaderWrapper _geom{NoCreate};
    #endif
    #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    GL::Version _version;
    #endif
};

/**
@brief 3D mesh visualization OpenGL shader
@m_since_latest

Visualizes wireframe, per-vertex/per-instance object ID, primitive ID or
tangent space of 3D meshes. You need to provide the @ref Position attribute in
your triangle mesh at the very least. Use @ref setProjectionMatrix(),
@ref setTransformationMatrix(), @ref setColor() and others to configure the
shader.

@m_class{m-row}

@parblock

@m_div{m-col-m-4 m-col-t-6 m-push-m-2 m-text-center m-nopadt m-nopadx}
@image html shaders-meshvisualizer3d.png width=256px
@ref Shaders-MeshVisualizerGL3D-wireframe, \n
@ref Shaders-MeshVisualizerGL3D-tbn
@m_enddiv

@m_div{m-col-m-4 m-col-t-6 m-push-m-2 m-text-center m-nopadt m-nopadx}
@image html shaders-meshvisualizer3d-primitiveid.png width=256px
@ref Shaders-MeshVisualizerGL3D-object-id
@m_enddiv

@endparblock

The shader expects that you enable wireframe visualization, tangent space
visualization or object/primitive ID visualization by passing an appropriate
@ref Flag to @ref Configuration::setFlags() --- there's no default behavior
with nothing enabled.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section Shaders-MeshVisualizerGL3D-wireframe Wireframe visualization

Wireframe visualization is done by enabling @ref Flag::Wireframe. It is done
either using geometry shaders or with help of additional vertex information. If
you have geometry shaders available, you don't need to do anything else except
calling @ref setViewportSize() to correctly size the wireframe --- without
this, the mesh will be rendered in a single color.

@requires_gl32 Extension @gl_extension{ARB,geometry_shader4} for wireframe
    rendering using geometry shaders.
@requires_es_extension Extension @gl_extension{EXT,geometry_shader} for
    wireframe rendering using geometry shaders.

If you don't have geometry shaders, you need to enable @ref Flag::NoGeometryShader
(done by default in OpenGL ES 2.0) and use only **non-indexed** triangle meshes
(see @ref MeshTools::duplicate() for a possible solution). Additionally, if you
have OpenGL < 3.1 or OpenGL ES 2.0, you need to provide also the
@ref VertexIndex attribute.

@requires_gles30 Extension @gl_extension{OES,standard_derivatives} for
    wireframe rendering without geometry shaders.

If using geometry shaders on OpenGL ES, @gl_extension{NV,shader_noperspective_interpolation}
is optionally used for improving line appearance. On desktop OpenGL this is
done implicitly.

If you want to render just the wireframe on top of an existing mesh, call
@ref setColor() with @cpp 0x00000000_rgbaf @ce. Alpha / transparency is
supported by the shader implicitly, but to have it working on the framebuffer,
you need to enable @ref GL::Renderer::Feature::Blending and set up the blending
function. See @ref GL::Renderer::setBlendFunction() for details.

@subsection Shaders-MeshVisualizerGL3D-wireframe-geom Example setup with a geometry shader (desktop GL, OpenGL ES 3.2)

Common mesh setup:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-geom1

Common rendering setup:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-geom2

@subsection Shaders-MeshVisualizerGL3D-wireframe-no-geom Example setup for indexed meshes without a geometry shader

The vertices have to be converted to a non-indexed array first. Mesh setup:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-no-geom1

Rendering setup:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-no-geom2

@subsection Shaders-MeshVisualizerGL3D-usage-wireframe-no-geom-old Wireframe visualization of non-indexed meshes without a geometry shader on older hardware

You need to provide also the @ref VertexIndex attribute. Mesh setup *in
addition to the above*:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-no-geom-old

Rendering setup the same as above.

@section Shaders-MeshVisualizerGL3D-tbn Tangent space visualization

On platforms with geometry shaders (desktop GL, OpenGL ES 3.2), the shader is
able to visualize tangent, bitangent and normal direction via colored lines
coming out of vertices (red, green and blue for tangent, bitangent and normal, respectively). This can be enabled together with wireframe, object ID,
vertex ID or primitive ID visualization, however note that when both are
enabled, the lines are not antialiased to avoid depth ordering artifacts
between faces and lines.

For tangents and normals, you need to provide the @ref Tangent and @ref Normal
attributes and enable @ref Flag::TangentDirection and
@ref Flag::NormalDirection, respectively. If any of the attributes isn't
present, its data are implicitly zero and thus the direction isn't shown ---
which means you don't need to worry about having two active variants of the
shader and switching between either depending on whether tangents are present
or not.

For bitangents however, there are two possible representations --- the more
efficient one is via a fourth component in the tangent attribute that
indicates tangent space handedness, in which case you'll be using the
@ref Tangent4 attribute instead of @ref Tangent, and enable
@ref Flag::BitangentFromTangentDirection. The other, more obvious but less
efficient representation, is a dedicated @ref Bitangent attribute (in which
case you'll enable @ref Flag::BitangentDirection). Note that these two are
mutually exclusive, so you need to choose either of them based on what given
mesh contains. Example for the first case:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-tbn1

Rendering setup:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-tbn2

@section Shaders-MeshVisualizerGL3D-object-id Object, vertex and primitive ID visualization

If the mesh contains a per-vertex (or instanced) @ref ObjectId, it can be
visualized by enabling @ref Flag::InstancedObjectId. For the actual
visualization you need to provide a color map using @ref bindColorMapTexture()
and use @ref setColorMapTransformation() to map given range of discrete IDs to
the @f$ [0, 1] @f$ texture range. Various colormap presets are in the
@ref DebugTools::ColorMap namespace. Example usage:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-object-id

Consistently with the other shaders, textured object ID is also supported if
@ref Flag::ObjectIdTexture is enabled. In that case you need to provide also
the @ref TextureCoordinates attribute and bind an integer texture via
@ref bindObjectIdTexture(). @ref Flag::TextureTransformation then enables
texture transformation and @ref Flag::TextureArrays texture arrays for the
object ID texture, with the layer selected using @ref setTextureLayer().

If you enable @ref Flag::VertexId, the shader will use the color map to
visualize how are vertices shared among primitives. That's useful for
inspecting mesh connectivity --- primitives sharing vertices will have a smooth
color map transition while duplicated vertices will cause a sharp edge. This
relies on the @glsl gl_VertexID @ce GLSL builtin.

The @ref Flag::PrimitiveId then visualizes the order in which primitives are
drawn. That's useful for example to see to see how well is the mesh optimized
for a post-transform vertex cache. This by default relies on the @glsl gl_PrimitiveID @ce GLSL builtin; with @ref Flag::PrimitiveIdFromVertexId it's
emulated using @glsl gl_VertexID @ce, expecting you to draw a non-indexed
triangle mesh. You can use @ref MeshTools::duplicate() (and potentially
@ref MeshTools::generateIndices()) to conveniently convert the mesh to a
non-indexed @ref MeshPrimitive::Triangles.

@requires_gl30 Extension @gl_extension{EXT,gpu_shader4} for object ID input,
    @gl_extension{EXT,texture_array} for object ID texture arrays
@requires_gl30 The `gl_VertexID` shader variable is not available on OpenGL
    2.1.
@requires_gl32 The `gl_PrimitiveID` shader variable is not available on OpenGL
    3.1 and lower.
@requires_gles32 The `gl_PrimitiveID` shader variable is not available on
    OpenGL ES 3.1 and lower.
@requires_gles30 Object ID input requires integer support in shaders, which
    is not available in OpenGL ES 2.0. Texture arrays for object ID texture
    arrays are not available in OpenGL ES 2.0.
@requires_gles30 The `gl_VertexID` shader variable is not available on OpenGL
    ES 2.0.
@requires_gles `gl_PrimitiveID` is not available in WebGL.
@requires_webgl20 Object ID input requires integer support in shaders, which
    is not available in WebGL 1.0. Texture arrays for object ID texture
    arrays are not available in WebGL 1.0.
@requires_webgl20 `gl_VertexID` is not available in WebGL 1.0.

@section Shaders-MeshVisualizerGL3D-skinning Skinning

To render skinned meshes, bind up to two sets of up to four-component joint ID
and weight attributes to @ref JointIds / @ref SecondaryJointIds and
@ref Weights / @ref SecondaryWeights, set an appropriate joint count and
per-vertex primary and secondary joint count in
@ref Configuration::setJointCount() and upload appropriate joint matrices with
@ref setJointMatrices(). The usage is similar for all shaders, see
@ref shaders-usage-skinning for an example. Currently, the mesh visualizer
supports only transforming the mesh vertices for feature parity with other
shaders, no skinning-specific visualization feature is implemented.

To avoid having to compile multiple shader variants for different joint matrix
counts, set the maximum used joint count in @ref Configuration::setJointCount()
and then upload just a prefix via @ref setJointMatrices(). Similarly, to avoid
multiple variants for different per-vertex joint counts, enable
@ref Flag::DynamicPerVertexJointCount, set the maximum per-vertex joint count
in @ref Configuration::setJointCount() and then adjust the actual per-draw
joint count with @ref setPerVertexJointCount().

@requires_gl30 Extension @gl_extension{EXT,texture_integer}
@requires_gles30 Skinning requires integer support in shaders, which is not
    available in OpenGL ES 2.0.
@requires_webgl20 Skinning requires integer support in shaders, which is not
    available in WebGL 1.0.

@section Shaders-MeshVisualizerGL3D-instancing Instanced rendering

Enabling @ref Flag::InstancedTransformation will turn the shader into an
instanced one. It'll take per-instance transformation from the
@ref TransformationMatrix attribute, applying it before the matrix set by
@ref setTransformationMatrix(). If one of @ref Flag::TangentDirection,
@ref Flag::BitangentDirection or @ref Flag::NormalDirection is set,
additionally also a normal matrix from the @ref NormalMatrix attribute is
taken, applied before the matrix set by @ref setNormalMatrix(). The snippet
below shows adding a buffer with per-instance transformation to a mesh,
including a normal matrix attribute for correct TBN visualization:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-usage-instancing

If @ref Flag::ObjectIdTexture is used and @ref Flag::InstancedTextureOffset is
enabled, the @ref TextureOffset attribute (or @ref TextureOffsetLayer in case
@ref Flag::TextureArrays is enabled as well) then can supply per-instance
texture offset (or offset and layer).

For instanced skinning the joint buffer is assumed to contain joint
transformations for all instances. By default all instances use the same joint
transformations, seting @ref setPerInstanceJointCount() will cause the shader
to offset the per-vertex joint IDs with
@glsl gl_InstanceID*perInstanceJointCount @ce.

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
@requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
    @gl_extension{EXT,instanced_arrays} or @gl_extension{NV,instanced_arrays}
    in OpenGL ES 2.0.
@requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays} in WebGL
    1.0.

@section Shaders-MeshVisualizerGL3D-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, the shader needs a separate
@ref ProjectionUniform3D and @ref TransformationUniform3D buffer bound with
@ref bindProjectionBuffer() and @ref bindTransformationBuffer(), respectively.
To maximize use of the limited uniform buffer memory, materials are supplied
separately in a @ref MeshVisualizerMaterialUniform buffer bound with
@ref bindMaterialBuffer() and then referenced via
@relativeref{MeshVisualizerDrawUniform3D,materialId} from a
@ref MeshVisualizerDrawUniform3D buffer bound with @ref bindDrawBuffer(); for optional texture transformation a per-draw
@ref TextureTransformationUniform buffer bound with
@ref bindTextureTransformationBuffer() can be supplied as well. A uniform
buffer setup equivalent to the
@ref Shaders-MeshVisualizerGL3D-wireframe "wireframe case at the top" would
look like this --- note that @ref setViewportSize() is an immediate uniform
here as well, as it's assumed to be set globally and rarely changed:

@snippet Shaders-gl.cpp MeshVisualizerGL3D-ubo

For a multidraw workflow enable @ref Flag::MultiDraw, supply desired material
and draw count via @ref Configuration::setMaterialCount() and
@relativeref{Configuration,setDrawCount()} and specify material references for
every draw. The usage is similar for all shaders, see
@ref shaders-usage-multidraw for an example.

For skinning, joint matrices are supplied via a @ref TransformationUniform3D
buffer bound with @ref bindJointBuffer(). In an instanced scenario the
per-instance joint count is supplied via
@ref MeshVisualizerDrawUniform3D::perInstanceJointCount, a per-draw joint
offset for the multidraw scenario is supplied via
@ref MeshVisualizerDrawUniform3D::jointOffset. Altogether for a particular
draw, each per-vertex joint ID is offset with
@glsl gl_InstanceID*perInstanceJointCount + jointOffset @ce. The
@ref setPerVertexJointCount() stays as an immediate uniform in the UBO and
multidraw scenario as well, as it is tied to a particular mesh layout and thus
doesn't need to vary per draw.

@requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object} for uniform
    buffers.
@requires_gl46 Extension @gl_extension{ARB,shader_draw_parameters} for
    multidraw.
@requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
@requires_webgl20 Uniform buffers are not available in WebGL 1.0.
@requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
    (unlisted) for multidraw.
@requires_webgl_extension Extension @webgl_extension{ANGLE,multi_draw} for
    multidraw.

@see @ref shaders, @ref MeshVisualizerGL2D
@todo Understand and add support wireframe width/smoothness without GS
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizerGL3D: public Implementation::MeshVisualizerGLBase {
    public:
        /* MSVC needs dllexport here as well */
        class MAGNUM_SHADERS_EXPORT Configuration;
        class CompileState;

        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}.
         */
        typedef typename GenericGL3D::Position Position;

        /**
         * @brief Tangent direction
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}. Use either this or the @ref Tangent4
         * attribute. Used only if @ref Flag::TangentDirection is enabled.
         */
        typedef typename GenericGL3D::Tangent Tangent;

        /**
         * @brief Tangent direction with a bitangent sign
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Use either this or the @ref Tangent
         * attribute. Used only if @ref Flag::TangentDirection or
         * @ref Flag::BitangentFromTangentDirection is enabled.
         */
        typedef typename GenericGL3D::Tangent4 Tangent4;

        /**
         * @brief Bitangent direction
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}. Use either this or the @ref Tangent4
         * attribute. Used only if @ref Flag::BitangentDirection is enabled.
         */
        typedef typename GenericGL3D::Bitangent Bitangent;

        /**
         * @brief Normal direction
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}. Used only if
         * @ref Flag::NormalDirection is enabled.
         */
        typedef typename GenericGL3D::Normal Normal;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief 2D texture coordinates
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}. Used only if
         * @ref Flag::ObjectIdTexture is enabled.
         * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::TextureCoordinates TextureCoordinates;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint ids
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4ui}. Used only if
         * @ref perVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::JointIds JointIds;

        /**
         * @brief Weights
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Used only if
         * @ref perVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::Weights Weights;

        /**
         * @brief Secondary joint ids
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4ui}. Used only if
         * @ref secondaryPerVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::SecondaryJointIds SecondaryJointIds;

        /**
         * @brief Secondary weights
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Used only if
         * @ref secondaryPerVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::SecondaryWeights SecondaryWeights;
        #endif

        /**
         * @brief Vertex index
         *
         * @relativeref{Magnum,Float}, used only in OpenGL < 3.1 and OpenGL ES
         * 2.0 if @ref Flag::Wireframe is enabled. This attribute (modulo 3)
         * specifies index of given vertex in triangle, i.e. @cpp 0.0f @ce for
         * first, @cpp 1.0f @ce for second, @cpp 2.0f @ce for third. In OpenGL
         * 3.1, OpenGL ES 3.0 and newer this value is provided via the
         * @cb{.glsl} gl_VertexID @ce shader builtin, so the attribute is not
         * needed.
         *
         * @note This attribute uses the same slot as @ref GenericGL::ObjectId,
         *      but since Object ID is available only on ES3+ and vertex index
         *      is used only on ES2 contexts without @glsl gl_VertexID @ce,
         *      there should be no conflict between these two.
         */
        typedef GL::Attribute<4, Float> VertexIndex;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) object ID
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,UnsignedInt}. Used only if
         * @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix4}. Used only if
         * @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef GenericGL3D::TransformationMatrix TransformationMatrix;

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief (Instanced) normal matrix
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix3x3}. Used only if
         * @ref Flag::InstancedTransformation and at least one of
         * @ref Flag::TangentDirection, @ref Flag::BitangentDirection or
         * @ref Flag::NormalDirection is set.
         * @requires_gl33 Extension @gl_extension{ARB,geometry_shader4} and
         *      @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a}
         *      / @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        typedef GenericGL3D::NormalMatrix NormalMatrix;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) texture offset for an object ID texture
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}. Used only if
         * @ref Flag::InstancedTextureOffset is set.
         * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4} and
         *      @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        typedef typename GenericGL3D::TextureOffset TextureOffset;

        /**
         * @brief (Instanced) texture offset and layer for an object ID texture
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}, with the last component interpreted as
         * an integer. Use either this or the @ref TextureOffset attribute.
         * First two components used only if @ref Flag::InstancedTextureOffset
         * is set, third component only if @ref Flag::TextureArrays is set.
         * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4},
         *      @gl_extension{EXT,texture_array} and
         *      @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0. Texture
         *      arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0. Texture arrays
         *      are not available in WebGL 1.0.
         */
        typedef typename GenericGL3D::TextureOffsetLayer TextureOffsetLayer;
        #endif

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = GenericGL3D::ColorOutput
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
         */
        enum class Flag: UnsignedInt {
            /**
             * Visualize wireframe. On OpenGL ES 2.0 and WebGL this also
             * enables @ref Flag::NoGeometryShader.
             */
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            Wireframe = 1 << 0,
            #else
            Wireframe = (1 << 0) | (1 << 1),
            #endif

            /**
             * Don't use a geometry shader for wireframe visualization. If
             * enabled, you might need to provide also the @ref VertexIndex
             * attribute in the mesh. On OpenGL ES 2.0 and WebGL enabled
             * alongside @ref Flag::Wireframe.
             *
             * Mutually exclusive with @ref Flag::TangentDirection,
             * @ref Flag::BitangentFromTangentDirection,
             * @ref Flag::BitangentDirection and @ref Flag::NormalDirection ---
             * those need a geometry shader always.
             */
            NoGeometryShader = 1 << 1,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Visualize object ID set via @ref setObjectId() or
             * @ref MeshVisualizerDrawUniform3D::objectId. Since the ID is
             * uniform for the whole draw, this feature is mainly useful in
             * multidraw scenarios or when combined with
             * @ref Flag::InstancedObjectId. Mutually exclusive with
             * @ref Flag::VertexId and @ref Flag::PrimitiveId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             */
            ObjectId = 1 << 12,

            /**
             * Visualize instanced object ID. You need to provide the
             * @ref ObjectId attribute in the mesh, which then gets summed with
             * the ID coming from @ref setObjectId() or
             * @ref MeshVisualizerDrawUniform3D::objectId. Implicitly enables
             * @ref Flag::ObjectId. Mutually exclusive with @ref Flag::VertexId
             * and @ref Flag::PrimitiveId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 2)|ObjectId,

            /**
             * Object ID texture. Retrieves object IDs from a texture bound
             * with @ref bindObjectIdTexture(), outputting a sum of the object
             * ID texture, the ID coming from @ref setObjectId() or
             * @ref MeshVisualizerDrawUniform3D::objectId and possibly also the
             * per-vertex ID, if @ref Flag::InstancedObjectId is enabled as
             * well. Implicitly enables @ref Flag::ObjectId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             */
            ObjectIdTexture = (1 << 14)|ObjectId,

            /**
             * Visualize vertex ID (@cpp gl_VertexID @ce). Useful for
             * visualizing mesh connectivity --- primitives sharing vertices
             * will have a smooth color map transition while duplicated
             * vertices will cause a sharp edge. Mutually exclusive with
             * @ref Flag::InstancedObjectId and @ref Flag::PrimitiveId.
             * @requires_gl30 The `gl_VertexID` shader variable is not
             *      available on OpenGL 2.1.
             * @requires_gles30 The `gl_VertexID` shader variable is not
             *      available on OpenGL ES 2.0.
             * @requires_webgl20 `gl_VertexID` is not available in WebGL 1.0.
             * @m_since{2020,06}
             */
            VertexId = 1 << 3,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Visualize primitive ID (@cpp gl_PrimitiveID @ce). Useful for
             * visualizing how well is the mesh optimized for a post-transform
             * vertex cache. Mutually exclusive with
             * @ref Flag::InstancedObjectId and @ref Flag::VertexId. See also
             * @ref Flag::PrimitiveIdFromVertexId.
             * @requires_gl32 The `gl_PrimitiveID` shader variable is not
             *      available on OpenGL 3.1 and lower.
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 The `gl_PrimitiveID` shader variable is not
             *      available on OpenGL ES 3.1 and lower.
             * @requires_gles `gl_PrimitiveID` is not available in WebGL.
             * @m_since{2020,06}
             */
            PrimitiveId = 1 << 4,
            #endif

            /**
             * Visualize primitive ID on a non-indexed triangle mesh using
             * @cpp gl_VertexID/3 @ce. Implicitly enables
             * @ref Flag::PrimitiveId, mutually exclusive with
             * @ref Flag::InstancedObjectId. Usable on OpenGL < 3.2,
             * OpenGL ES < 3.2 and WebGL where @cpp gl_PrimitiveID @ce is not
             * available.
             * @requires_gl30 The `gl_VertexID` shader variable is not
             *      available on OpenGL 2.1.
             * @requires_gles30 The `gl_VertexID` shader variable is not
             *      available on OpenGL ES 2.0.
             * @requires_webgl20 `gl_VertexID` is not available in WebGL 1.0.
             * @m_since{2020,06}
             */
            #ifndef MAGNUM_TARGET_WEBGL
            PrimitiveIdFromVertexId = (1 << 5)|PrimitiveId,
            #else
            PrimitiveIdFromVertexId = (1 << 5)|(1 << 4),
            #endif
            #endif

            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /**
             * Visualize tangent direction with red lines pointing out of
             * vertices. You need to provide the @ref Tangent or @ref Tangent4
             * attribute in the mesh. Mutually exclusive with
             * @ref Flag::NoGeometryShader (as this needs a geometry shader
             * always).
             * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             * @m_since{2020,06}
             */
            TangentDirection = 1 << 6,

            /**
             * Visualize bitangent direction with green lines pointing out of
             * vertices. You need to provide both @ref Normal and @ref Tangent4
             * attributes in the mesh, alternatively you can provide the
             * @ref Bitangent attribute and enable
             * @ref Flag::BitangentDirection instead. Mutually exclusive with
             * @ref Flag::NoGeometryShader (as this needs a geometry shader
             * always).
             * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             * @m_since{2020,06}
             */
            BitangentFromTangentDirection = 1 << 7,

            /**
             * Visualize bitangent direction with green lines pointing out of
             * vertices. You need to provide the @ref Bitangent attribute in
             * the mesh, alternatively you can provide both @ref Normal and
             * @ref Tangent4 attributes and enable
             * @ref Flag::BitangentFromTangentDirection instead. Mutually
             * exclusive with @ref Flag::NoGeometryShader (as this needs a
             * geometry shader always).
             * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             * @m_since{2020,06}
             */
            BitangentDirection = 1 << 8,

            /**
             * Visualize normal direction with blue lines pointing out of
             * vertices. You need to provide the @ref Normal attribute in the
             * mesh. Mutually exclusive with @ref Flag::NoGeometryShader (as
             * this needs a geometry shader always).
             * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             * @m_since{2020,06}
             */
            NormalDirection = 1 << 9,
            #endif

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation and normal matrix from the
             * @ref TransformationMatrix / @ref NormalMatrix attributes and
             * uses them together with matrices coming from
             * @ref setTransformationMatrix() and @ref setNormalMatrix() or
             * @ref TransformationUniform3D::transformationMatrix and
             * @ref MeshVisualizerDrawUniform3D::normalMatrix (first the
             * per-instance, then the uniform matrix). See
             * @ref Shaders-MeshVisualizerGL3D-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since_latest
             */
            InstancedTransformation = 1 << 13,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Enable texture coordinate transformation for an object ID
             * texture. If this flag is set, the shader expects that
             * @ref Flag::ObjectIdTexture is enabled as well.
             * @see @ref setTextureMatrix()
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             * @todoc rewrite the ext requirements once we have more textures
             */
            TextureTransformation = 1 << 15,

            /**
             * Instanced texture offset for an object ID texture. Retrieves a
             * per-instance offset vector from the @ref TextureOffset attribute
             * and uses it together with the matrix coming from
             * @ref setTextureMatrix() or
             * @ref TextureTransformationUniform::rotationScaling and
             * @ref TextureTransformationUniform::offset (first the
             * per-instance vector, then the uniform matrix). Instanced texture
             * scaling and rotation is not supported at the moment, you can
             * specify that only via the uniform @ref setTextureMatrix().
             * Implicitly enables @ref Flag::TextureTransformation. See
             * @ref Shaders-MeshVisualizerGL2D-instancing for more information.
             *
             * If @ref Flag::TextureArrays is set as well, a three-component
             * @ref TextureOffsetLayer attribute can be used instead of
             * @ref TextureOffset to specify per-instance texture layer, which
             * gets added to the uniform layer numbers set by
             * @ref setTextureLayer() or
             * @ref TextureTransformationUniform::layer.
             * @requires_gl30 Extension
             * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4} and
             *      @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             * @todoc rewrite the ext requirements once we have more textures
             */
            InstancedTextureOffset = (1 << 16)|TextureTransformation,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindProjectionBuffer(), @ref bindTransformationBuffer(),
             * @ref bindDrawBuffer() and @ref bindMaterialBuffer() instead of
             * direct uniform setters.
             * @see @ref Flag::ShaderStorageBuffers
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 10,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Use shader storage buffers. Superset of functionality provided
             * by @ref Flag::UniformBuffers, compared to it doesn't have any
             * size limits on @ref Configuration::setJointCount(),
             * @relativeref{Configuration,setMaterialCount()} and
             * @relativeref{Configuration,setDrawCount()} in exchange for
             * potentially more costly access and narrower platform support.
             * @requires_gl43 Extension @gl_extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage buffers are not available in
             *      OpenGL ES 3.0 and older.
             * @requires_gles Shader storage buffers are not available in
             *      WebGL.
             * @m_since_latest
             */
            ShaderStorageBuffers = UniformBuffers|(1 << 19),
            #endif

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and combines the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
             * and related APIs pick up per-draw parameters directly, without
             * having to rebind the uniform buffers or specify
             * @ref setDrawOffset() before each draw. In a non-multidraw
             * scenario, @glsl gl_DrawID @ce is @cpp 0 @ce, which means a
             * shader with this flag enabled can be used for regular draws as
             * well.
             * @requires_gl46 Extension @gl_extension{ARB,uniform_buffer_object}
             *      and @gl_extension{ARB,shader_draw_parameters}
             * @requires_es_extension OpenGL ES 3.0 and extension
             *      @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
             *      (unlisted). While the extension alone needs only OpenGL ES
             *      2.0, the shader implementation relies on uniform buffers,
             *      which require OpenGL ES 3.0.
             * @requires_webgl_extension WebGL 2.0 and extension
             *      @webgl_extension{ANGLE,multi_draw}. While the extension
             *      alone needs only WebGL 1.0, the shader implementation
             *      relies on uniform buffers, which require WebGL 2.0.
             * @m_since_latest
             */
            MultiDraw = UniformBuffers|(1 << 11),

            /**
             * Use 2D texture arrays for an object ID texture. Expects that the
             * texture is supplied via
             * @ref bindObjectIdTexture(GL::Texture2DArray&) and the layer
             * is set via @ref setTextureLayer() or
             * @ref TextureTransformationUniform::layer. If
             * @ref Flag::InstancedTextureOffset is set as well and a
             * three-component @ref TextureOffsetLayer attribute is used
             * instead of @ref TextureOffset, the per-instance and uniform
             * layer numbers are added together.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4} and
             *      @gl_extension{EXT,texture_array}
             * @requires_gles30 Object ID input requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0. Texture
             *      arrays are not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID input requires integer support in
             *      shaders, which is not available in WebGL 1.0. Texture
             *      arrays are not available in WebGL 1.0.
             * @m_since_latest
             * @todoc rewrite the ext requirements once we have more textures
             */
            TextureArrays = 1 << 17,

            /**
             * Dynamic per-vertex joint count for skinning. Uses only the first
             * M / N primary / secondary components defined by
             * @ref setPerVertexJointCount() instead of
             * all primary / secondary components defined by
             * @ref Configuration::setJointCount() at shader compilation time.
             * Useful in order to avoid having a shader permutation defined for
             * every possible joint count. Unfortunately it's not possible to
             * make use of default values for unspecified input components as
             * the last component is always @cpp 1.0 @ce instead of
             * @cpp 0.0 @ce, on the other hand dynamically limiting the joint
             * count can reduce the time spent executing the vertex shader
             * compared to going through the full set of per-vertex joints
             * always.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Skinning requires integer support in shaders,
             *      which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Skinning requires integer support in shaders,
             *      which is not available in WebGL 1.0.
             * @m_since_latest
             */
            DynamicPerVertexJointCount = 1 << 18,
            #endif
        };

        /**
         * @brief Flags
         *
         * @see @ref flags(), @ref Configuration::setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Compile asynchronously
         * @m_since_latest
         *
         * Compared to @ref MeshVisualizerGL3D(const Configuration&) can
         * perform an asynchronous compilation and linking. See
         * @ref shaders-async for more information.
         * @see @ref MeshVisualizerGL3D(CompileState&&)
         */
        /* No default value, consistently with MeshVisualizerGL3D(Configuration) */
        static CompileState compile(const Configuration& configuration);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Compile asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Compile for a multi-draw scenario asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Constructor
         * @m_since_latest
         */
        /* No default value, as at least one Flag has to be set. There's a
           deprecated default-value overload below, though. */
        explicit MeshVisualizerGL3D(const Configuration& configuration);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since{2020,06} Use @ref MeshVisualizerGL3D(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use MeshVisualizerGL3D(Configuration) instead") MeshVisualizerGL3D();

        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref MeshVisualizerGL3D(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use MeshVisualizerGL3D(const Configuration& instead") MeshVisualizerGL3D(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @m_deprecated_since_latest Use @ref MeshVisualizerGL3D(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        explicit CORRADE_DEPRECATED("use MeshVisualizerGL3D(const Configuration& instead") MeshVisualizerGL3D(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Finalize an asynchronous compilation
         * @m_since_latest
         *
         * Takes an asynchronous compilation state returned by @ref compile()
         * and forms a ready-to-use shader object. See @ref shaders-async for
         * more information.
         */
        explicit MeshVisualizerGL3D(CompileState&& state);

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to a moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit MeshVisualizerGL3D(NoCreateT) noexcept: Implementation::MeshVisualizerGLBase{NoCreate} {}

        /** @brief Copying is not allowed */
        MeshVisualizerGL3D(const MeshVisualizerGL3D&) = delete;

        /** @brief Move constructor */
        MeshVisualizerGL3D(MeshVisualizerGL3D&&) noexcept = default;

        /** @brief Copying is not allowed */
        MeshVisualizerGL3D& operator=(const MeshVisualizerGL3D&) = delete;

        /** @brief Move assignment */
        MeshVisualizerGL3D& operator=(MeshVisualizerGL3D&&) noexcept = default;

        /**
         * @brief Flags
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const {
            return Flags(UnsignedInt(Implementation::MeshVisualizerGLBase::_flags));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         * @m_since_latest
         *
         * If @ref Flag::UniformBuffers is not set, this is the number of joint
         * matrices accepted by @ref setJointMatrices() / @ref setJointMatrix().
         * If @ref Flag::UniformBuffers is set, this is the statically defined
         * size of the @ref TransformationUniform3D uniform buffer bound with
         * @ref bindJointBuffer(). Has no use if @ref Flag::ShaderStorageBuffers
         * is set.
         * @see @ref Configuration::setJointCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt jointCount() const { return _jointCount; }

        /**
         * @brief Per-vertex joint count
         * @m_since_latest
         *
         * Returns the value set with @ref Configuration::setJointCount(). If
         * @ref Flag::DynamicPerVertexJointCount is set, the count can be
         * additionally modified per-draw using @ref setPerVertexJointCount().
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt perVertexJointCount() const { return _perVertexJointCount; }

        /**
         * @brief Secondary per-vertex joint count
         * @m_since_latest
         *
         * Returns the value set with @ref Configuration::setJointCount(). If
         * @ref Flag::DynamicPerVertexJointCount is set, the count can be
         * additionally modified per-draw using @ref setPerVertexJointCount().
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt secondaryPerVertexJointCount() const { return _secondaryPerVertexJointCount; }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the @ref MeshVisualizerMaterialUniform
         * uniform buffer bound with @ref bindMaterialBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set and @ref Flag::ShaderStorageBuffers
         * is not set.
         * @see @ref Configuration::setMaterialCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationUniform3D and
         * @ref MeshVisualizerDrawUniform3D uniform buffers, bound with
         * @ref bindTransformationBuffer() and @ref bindDrawBuffer(). Has use
         * only if @ref Flag::UniformBuffers is set and
         * @ref Flag::ShaderStorageBuffers is not set.
         * @see @ref Configuration::setDrawCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set dynamic per-vertex skinning joint count
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Allows reducing the count of iterated joints for a particular draw
         * call, making it possible to use a single shader with meshes that
         * contain different count of per-vertex joints. See
         * @ref Flag::DynamicPerVertexJointCount for more information. As the
         * joint count is tied to the mesh layout, this is a per-draw-call
         * setting even in case of @ref Flag::UniformBuffers instead of being
         * a value in @ref MeshVisualizerDrawUniform3D. Initial value is same
         * as @ref perVertexJointCount() and
         * @ref secondaryPerVertexJointCount().
         *
         * Expects that @ref Flag::DynamicPerVertexJointCount is set,
         * @p count is not larger than @ref perVertexJointCount() and
         * @p secondaryCount not larger than @ref secondaryPerVertexJointCount().
         * @see @ref Configuration::setJointCount()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders, which
         *      is not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& setPerVertexJointCount(UnsignedInt count, UnsignedInt secondaryCount = 0) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setPerVertexJointCount(count, secondaryCount));
        }
        #endif

        /** @{
         * @name Uniform setters
         *
         * Used only if @ref Flag::UniformBuffers is not set.
         */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set transformation and projection matrix
         * @m_deprecated_since{2020,06} Use @ref setTransformationMatrix() and
         *      @ref setProjectionMatrix() instead.
         */
        CORRADE_DEPRECATED("use setTransformationMatrix() and setProjectionMatrix() instead") MeshVisualizerGL3D& setTransformationProjectionMatrix(const Matrix4& matrix) {
            /* Keep projection at identity, which should still work for
               wireframe (but of course not for TBN visualization) */
            return setTransformationMatrix(matrix);
        }
        #endif

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix. If
         * @ref Flag::InstancedTransformation is set, the per-instance
         * transformation coming from the @ref TransformationMatrix attribute
         * is applied first, before this one.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindTransformationBuffer() instead.
         */
        MeshVisualizerGL3D& setTransformationMatrix(const Matrix4& matrix);

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix. (i.e., an orthographic
         * projection of the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$
         * cube).
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref ProjectionUniform3D::projectionMatrix and call
         * @ref bindProjectionBuffer() instead.
         */
        MeshVisualizerGL3D& setProjectionMatrix(const Matrix4& matrix);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that @ref Flag::TangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled. The matrix doesn't need to be normalized, as
         * renormalization is done per-fragment anyway.
         * Initial value is an identity matrix. If
         * @ref Flag::InstancedTransformation is set, the per-instance normal
         * matrix coming from the @ref NormalMatrix attribute is applied first,
         * before this one.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerDrawUniform3D::normalMatrix and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        MeshVisualizerGL3D& setNormalMatrix(const Matrix3x3& matrix);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set texture coordinate transformation matrix for an object ID texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix. If @ref Flag::InstancedTextureOffset is set, the
         * per-instance offset coming from the @ref TextureOffset attribute is
         * applied first, before this matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::rotationScaling and
         * @ref TextureTransformationUniform::offset and call
         * @ref bindTextureTransformationBuffer() instead.
         * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4} and
         *      @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         * @todoc rewrite the ext requirements once we have more textures
         */
        MeshVisualizerGL3D& setTextureMatrix(const Matrix3& matrix) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setTextureMatrix(matrix));
        }

        /**
         * @brief Set texture array layer for an object ID texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with @ref Flag::TextureArrays
         * enabled. Initial value is @cpp 0 @ce. If
         * @ref Flag::InstancedTextureOffset is set and a three-component
         * @ref TextureOffsetLayer attribute is used instead of
         * @ref TextureOffset, this value is added to the layer coming from the
         * third component.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::layer and call
         * @ref bindTextureTransformationBuffer() instead.
         * @requires_gl33 Extension @gl_extension{EXT,gpu_shader4} and
         *      @gl_extension{EXT,texture_array}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0. Texture
         *      arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0.Texture arrays are
         *      not available in WebGL 1.0.
         * @todoc rewrite the ext requirements once we have more textures
         */
        MeshVisualizerGL3D& setTextureLayer(UnsignedInt layer) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setTextureLayer(layer));
        }
        #endif

        /**
         * @brief Set viewport size
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Wireframe is enabled and geometry
         * shaders are used; or if @ref Flag::TangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled, otherwise it does nothing. Initial value is a zero vector.
         */
        MeshVisualizerGL3D& setViewportSize(const Vector2& size);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set object ID
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::ObjectId is enabled. Initial value is
         * @cpp 0 @ce. If @ref Flag::InstancedObjectId is enabled as well, this
         * value is added to the ID coming from the @ref ObjectId attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerDrawUniform3D::objectId and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID input requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID input requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& setObjectId(UnsignedInt id) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setObjectId(id));
        }
        #endif

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. Expects that either
         * @ref Flag::Wireframe or @ref Flag::InstancedObjectId /
         * @ref Flag::PrimitiveId / @ref Flag::PrimitiveIdFromVertexId is
         * enabled. In case of the latter, the color is multiplied with the
         * color map coming from @ref bindColorMapTexture().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::color and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL3D& setColor(const Color4& color) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setColor(color));
        }

        /**
         * @brief Set wireframe color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x000000ff_rgbaf @ce. Expects that
         * @ref Flag::Wireframe is enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::wireframeColor and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL3D& setWireframeColor(const Color4& color) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setWireframeColor(color));
        }

        /**
         * @brief Set wireframe width
         * @return Reference to self (for method chaining)
         *
         * The value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 1.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::wireframeWidth and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL3D& setWireframeWidth(Float width) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setWireframeWidth(width));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set color map transformation
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Offset and scale applied to the input value coming either from the
         * @ref ObjectId attribute or @glsl gl_PrimitiveID @ce, resulting value
         * is then used to fetch a color from a color map bound with
         * @ref bindColorMapTexture(). Initial value is @cpp 1.0f/512.0f @ce
         * and @cpp 1.0/256.0f @ce, meaning that for a 256-entry colormap the
         * first 256 values get an exact color from it and the next values will
         * be either clamped to last color or repeated depending on the color
         * map texture wrapping mode. Expects that either
         * @ref Flag::InstancedObjectId or @ref Flag::PrimitiveId /
         * @ref Flag::PrimitiveIdFromVertexId is enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::colorMapOffset and
         * @ref MeshVisualizerMaterialUniform::colorMapScale and call
         * @ref bindMaterialBuffer() instead.
         * @requires_gles30 Object ID visualization requires integer attributes
         *      while primitive ID visualization requires the `gl_VertexID` /
         *      `gl_PrimitiveID` builtins, neither of which is available in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Object ID visualization requires integer
         *      attributes while primitive ID visualization requires at least
         *      the `gl_VertexID` builtin, neither of which is available in
         *      WebGL 1.
         */
        MeshVisualizerGL3D& setColorMapTransformation(Float offset, Float scale) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setColorMapTransformation(offset, scale));
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Set line width
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * The value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 1.0f @ce. Expects that
         * @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::lineWidth and call
         * @ref bindMaterialBuffer() instead.
         * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        MeshVisualizerGL3D& setLineWidth(Float width);

        /**
         * @brief Set line length
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * The value is in object space, initial value is @cpp 1.0f @ce.
         * Expects that @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::lineLength and call
         * @ref bindMaterialBuffer() instead.
         * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        MeshVisualizerGL3D& setLineLength(Float length);
        #endif

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * The value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 2.0f @ce. Expects that @ref Flag::Wireframe,
         * @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerMaterialUniform::smoothness and call
         * @ref bindMaterialBuffer() instead.
         */
        MeshVisualizerGL3D& setSmoothness(Float smoothness);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set joint matrices
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Initial values are identity transformations. Expects that the size
         * of the @p matrices array is not larger than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @see @ref setJointMatrix(UnsignedInt, const Matrix4&)
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& setJointMatrices(const Containers::ArrayView<const Matrix4> matrices);

        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& setJointMatrices(std::initializer_list<Matrix4> matrices);

        /**
         * @brief Set joint matrix for given joint
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setJointMatrices() updates just a single joint matrix.
         * Expects that @p id is less than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& setJointMatrix(UnsignedInt id, const Matrix4& matrix);

        /**
         * @brief Set per-instance joint count
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Offset added to joint IDs in the @ref JointIds and
         * @ref SecondaryJointIds in instanced draws. Should be less than
         * @ref jointCount(). Initial value is @cpp 0 @ce, meaning every
         * instance will use the same joint matrices, setting it to a non-zero
         * value causes the joint IDs to be interpreted as
         * @glsl gl_InstanceID*count + jointId @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref MeshVisualizerDrawUniform3D::perInstanceJointCount and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& setPerInstanceJointCount(UnsignedInt count) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setPerInstanceJointCount(count));
        }
        #endif

        /**
         * @}
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @{
         * @name Uniform / shader storage buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Set a draw offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Specifies which item in the @ref TransformationUniform3D and
         * @ref MeshVisualizerDrawUniform3D buffers bound with
         * @ref bindTransformationBuffer() and @ref bindDrawBuffer() should be
         * used for current draw. Expects that @ref Flag::UniformBuffers is set
         * and @p offset is less than @ref drawCount(). Initial value is
         * @cpp 0 @ce, if @ref drawCount() is @cpp 1 @ce, the function is a
         * no-op as the shader assumes draw offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& setDrawOffset(UnsignedInt offset) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::setDrawOffset(offset));
        }

        /**
         * @brief Bind a projection uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain at least one instance of
         * @ref ProjectionUniform3D. At the very least you need to call also
         * @ref bindTransformationBuffer(), @ref bindDrawBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& bindProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a transformation uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationUniform3D. At the very least you need to call
         * also @ref bindDrawBuffer() and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& bindTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a draw uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref MeshVisualizerDrawUniform3D. At the very least you need to call
         * also @ref bindProjectionBuffer(), @ref bindTransformationBuffer()
         * and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a texture transformation uniform / shader storage buffer for an object ID texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that both @ref Flag::UniformBuffers and
         * @ref Flag::TextureTransformation is set. The buffer is expected to
         * contain @ref drawCount() instances of
         * @ref TextureTransformationUniform.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         *      Object ID input requires integer support in shaders, which is
         *      not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         *      Object ID input requires integer support in shaders, which is
         *      not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindTextureTransformationBuffer(GL::Buffer& buffer) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindTextureTransformationBuffer(buffer));
        }
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindTextureTransformationBuffer(buffer, offset, size));
        }

        /**
         * @brief Bind a material uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref MeshVisualizerMaterialUniform. At the very least you need to
         * call also @ref bindProjectionBuffer(),
         * @ref bindTransformationBuffer() and @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindMaterialBuffer(GL::Buffer& buffer) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindMaterialBuffer(buffer));
        }
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindMaterialBuffer(buffer, offset, size));
        }

        /**
         * @brief Bind a joint matrix uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref jointCount() instances of
         * @ref TransformationUniform3D.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindJointBuffer(GL::Buffer& buffer) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindJointBuffer(buffer));
        }
        /**
         * @overload
         * @m_since_latest
         */
        MeshVisualizerGL3D& bindJointBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindJointBuffer(buffer, offset, size));
        }

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind a color map texture
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * See also @ref setColorMapTransformation(). Expects that either
         * @ref Flag::InstancedObjectId or @ref Flag::PrimitiveId /
         * @ref Flag::PrimitiveIdFromVertexId is enabled.
         * @requires_gles30 Object ID visualization requires integer attributes
         *      while primitive ID visualization requires the `gl_VertexID` /
         *      `gl_PrimitiveID` builtins, neither of which is available in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Object ID visualization requires integer
         *      attributes while primitive ID visualization requires at least
         *      the `gl_VertexID` builtin, neither of which is available in
         *      WebGL 1.
         */
        MeshVisualizerGL3D& bindColorMapTexture(GL::Texture2D& texture) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindColorMapTexture(texture));
        }

        /**
         * @brief Bind an object ID texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with @ref Flag::ObjectIdTexture
         * enabled. If @ref Flag::TextureArrays is enabled as well, use
         * @ref bindObjectIdTexture(GL::Texture2DArray&) instead. The texture
         * needs to have an unsigned integer format.
         * @see @ref setObjectId(), @ref Flag::TextureTransformation,
         *      @ref setTextureMatrix()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID visualization requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID visualization requires integer support
         *      in shaders, which is not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindObjectIdTexture(GL::Texture2D& texture) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindObjectIdTexture(texture));
        }

        /**
         * @brief Bind an object ID array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both
         * @ref Flag::ObjectIdTexture and @ref Flag::TextureArrays enabled. If
         * @ref Flag::UniformBuffers is not enabled, the layer is set via
         * @ref setTextureLayer(); if @ref Flag::UniformBuffers is enabled,
         * @ref Flag::TextureTransformation has to be enabled as well and the
         * layer is set via @ref TextureTransformationUniform::layer.
         * @see @ref setObjectId(), @ref Flag::TextureTransformation,
         *      @ref setTextureLayer()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4} and
         *      @gl_extension{EXT,texture_array}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0. Texture
         *      arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0. Texture arrays
         *      are not available in WebGL 1.0.
         */
        MeshVisualizerGL3D& bindObjectIdTexture(GL::Texture2DArray& texture) {
            return static_cast<MeshVisualizerGL3D&>(Implementation::MeshVisualizerGLBase::bindObjectIdTexture(texture));
        }
        #endif

        /**
         * @}
         */

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(MeshVisualizerGL3D)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit MeshVisualizerGL3D(NoInitT): Implementation::MeshVisualizerGLBase{NoInit} {}

        Int _transformationMatrixUniform{9},
            _projectionMatrixUniform{10};
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        Int _normalMatrixUniform{11},
            _lineWidthUniform{12},
            _lineLengthUniform{13};
        #endif
};

/**
@brief Configuration
@m_since_latest

@see @ref MeshVisualizerGL3D(const Configuration&),
    @ref compile(const Configuration&)
*/
class MeshVisualizerGL3D::Configuration {
    public:
        explicit Configuration() = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * At least one of @ref Flag::Wireframe, @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection, @ref Flag::NormalDirection,
         * @ref Flag::ObjectId, @ref Flag::VertexId, @ref Flag::PrimitiveId or
         * @ref Flag::PrimitiveIdFromVertexId is expected to be enabled. No
         * flags are set by default.
         * @see @ref MeshVisualizerGL3D::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt jointCount() const { return _jointCount; }

        /**
         * @brief Per-vertex joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt perVertexJointCount() const { return _perVertexJointCount; }

        /**
         *@brief Secondary per-vertex joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt secondaryPerVertexJointCount() const { return _secondaryPerVertexJointCount; }

        /**
         * @brief Set joint count
         *
         * If @ref Flag::UniformBuffers isn't set, @p count describes an upper
         * bound on how many joint matrices get supplied to each draw with
         * @ref setJointMatrices() / @ref setJointMatrix().
         *
         * If @ref Flag::UniformBuffers is set, @p count describes size of a
         * @ref TransformationUniform3D buffer bound with
         * @ref bindJointBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(TransformationUniform3D) @ce has to be
         * within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-vertex joints index into
         * the array offset by @ref MeshVisualizerDrawUniform3D::jointOffset.
         *
         * The @p perVertexCount and @p secondaryPerVertexCount parameters
         * describe how many components are taken from @ref JointIds /
         * @ref Weights and @ref SecondaryJointIds / @ref SecondaryWeights
         * attributes. Both values are expected to not be larger than
         * @cpp 4 @ce, setting either of these to @cpp 0 @ce means given
         * attribute is not used at all. If both @p perVertexCount and
         * @p secondaryPerVertexCount are set to @cpp 0 @ce, skinning is not
         * performed. Unless @ref Flag::ShaderStorageBuffers is set, if either
         * of them is non-zero, @p count is expected to be non-zero as well.
         *
         * Default value for all three is @cpp 0 @ce.
         * @see @ref MeshVisualizerGL2D::jointCount(),
         *      @ref MeshVisualizerGL2D::perVertexJointCount(),
         *      @ref MeshVisualizerGL2D::secondaryPerVertexJointCount(),
         *      @ref Flag::DynamicPerVertexJointCount,
         *      @ref MeshVisualizerGL2D::setPerVertexJointCount()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        Configuration& setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount = 0);

        /**
         * @brief Material count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Set material count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref MeshVisualizerMaterialUniform buffer bound with
         * @ref bindMaterialBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(MeshVisualizerMaterialUniform) @ce has to
         * be within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-draw materials are
         * specified via @ref MeshVisualizerDrawUniform3D::materialId. Default
         * value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setDrawCount(),
         *      @ref MeshVisualizerGL3D::materialCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setMaterialCount(UnsignedInt count) {
            _materialCount = count;
            return *this;
        }

        /**
         * @brief Draw count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }

        /**
         * @brief Set draw count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref TransformationUniform3D / @ref MeshVisualizerDrawUniform3D /
         * @ref TextureTransformationUniform buffer bound with
         * @ref bindTransformationBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer(). Uniform buffers have a
         * statically defined size and the maximum of
         * @cpp count*sizeof(TransformationUniform3D) @ce,
         * @cpp count*sizeof(MeshVisualizerDrawUniform3D) @ce and
         * @cpp count*sizeof(TextureTransformationUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffers are
         * unbounded and @p count is ignored. The draw offset is set via
         * @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setMaterialCount(),
         *      @ref MeshVisualizerGL3D::drawCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setDrawCount(UnsignedInt count) {
            _drawCount = count;
            return *this;
        }
        #endif

    private:
        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _jointCount = 0,
            _perVertexJointCount = 0,
            _secondaryPerVertexJointCount = 0,
            _materialCount = 1,
            _drawCount = 1;
        #endif
};

/**
@brief Asynchronous compilation state
@m_since_latest

Returned by @ref compile(). See @ref shaders-async for more information.
*/
class MeshVisualizerGL3D::CompileState: public MeshVisualizerGL3D {
    /* Everything deliberately private except for the inheritance */
    friend class MeshVisualizerGL3D;

    explicit CompileState(NoCreateT): MeshVisualizerGL3D{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(MeshVisualizerGL3D&& shader, GL::Shader&& vert, GL::Shader&& frag
        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        , GL::Shader* geom
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , GL::Version version
        #endif
    ): MeshVisualizerGL3D{Utility::move(shader)}, _vert{Utility::move(vert)}, _frag{Utility::move(frag)}
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , _version{version}
        #endif
    {
        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        if(geom) _geom = Implementation::GLShaderWrapper{Utility::move(*geom)};
        #endif
    }

    Implementation::GLShaderWrapper _vert, _frag;
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    Implementation::GLShaderWrapper _geom{NoCreate};
    #endif
    #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    GL::Version _version;
    #endif
};

/** @debugoperatorclassenum{MeshVisualizerGL2D,MeshVisualizerGL2D::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizerGL2D::Flag value);

/** @debugoperatorclassenum{MeshVisualizerGL3D,MeshVisualizerGL3D::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizerGL3D::Flag value);

/** @debugoperatorclassenum{MeshVisualizerGL2D,MeshVisualizerGL2D::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizerGL2D::Flags value);

/** @debugoperatorclassenum{MeshVisualizerGL3D,MeshVisualizerGL3D::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizerGL3D::Flags value);

CORRADE_ENUMSET_OPERATORS(MeshVisualizerGL2D::Flags)
CORRADE_ENUMSET_OPERATORS(MeshVisualizerGL3D::Flags)

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
