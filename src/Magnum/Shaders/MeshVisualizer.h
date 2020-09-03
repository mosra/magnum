#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
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

/** @file
 * @brief Class @ref Magnum::Shaders::MeshVisualizer2D, @ref Magnum::Shaders::MeshVisualizer3D
 */

#include <Corrade/Utility/Utility.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {

class MAGNUM_SHADERS_EXPORT MeshVisualizerBase: public GL::AbstractShaderProgram {
    protected:
        enum class FlagBase: UnsignedShort {
            /* Unlike the public Wireframe flag, this one doesn't include
               NoGeometryShader on ES2 as that would make the checks too
               complex */
            Wireframe = 1 << 0,
            NoGeometryShader = 1 << 1,
            #ifndef MAGNUM_TARGET_GLES2
            InstancedObjectId = 1 << 2,
            VertexId = 1 << 3,
            PrimitiveId = 1 << 4,
            PrimitiveIdFromVertexId = (1 << 5)|PrimitiveId
            #endif
        };
        typedef Containers::EnumSet<FlagBase> FlagsBase;

        CORRADE_ENUMSET_FRIEND_OPERATORS(FlagsBase)

        explicit MeshVisualizerBase(FlagsBase flags);
        explicit MeshVisualizerBase(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        MAGNUM_SHADERS_LOCAL GL::Version setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs) const;

        MeshVisualizerBase& setColor(const Color4& color);
        MeshVisualizerBase& setWireframeColor(const Color4& color);
        MeshVisualizerBase& setWireframeWidth(Float width);
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerBase& setColorMapTransformation(Float offset, Float scale);
        MeshVisualizerBase& bindColorMapTexture(GL::Texture2D& texture);
        #endif

        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        FlagsBase _flags;
        Int _colorUniform{1},
            _wireframeColorUniform{2},
            _wireframeWidthUniform{3},
            _smoothnessUniform{4},
            _viewportSizeUniform{5};
        #ifndef MAGNUM_TARGET_GLES2
        Int _colorMapOffsetScaleUniform{6};
        #endif
};

}

/**
@brief 2D mesh visualization shader
@m_since{2020,06}

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
appropriate @ref Flag to the constructor --- there's no default behavior with
nothing enabled. The shader is a 2D variant of @ref MeshVisualizer3D with
mostly identical workflow. See its documentation for more information.
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizer2D: public Implementation::MeshVisualizerBase {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2".
         */
        typedef typename Generic2D::Position Position;

        /**
         * @brief Vertex index
         *
         * See @ref MeshVisualizer3D::VertexIndex for more information.
         */
        typedef GL::Attribute<4, Float> VertexIndex;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) object ID
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::UnsignedInt.
         * Used only if @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        typedef Generic3D::ObjectId ObjectId;
        #endif

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = Generic2D::ColorOutput
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref MeshVisualizer2D()
         */
        enum class Flag: UnsignedShort {
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
            /** @copydoc MeshVisualizer3D::Flag::InstancedObjectId */
            InstancedObjectId = 1 << 2,

            /** @copydoc MeshVisualizer3D::Flag::VertexId */
            VertexId = 1 << 3,

            #ifndef MAGNUM_TARGET_WEBGL
            /** @copydoc MeshVisualizer3D::Flag::PrimitiveId */
            PrimitiveId = 1 << 4,
            #endif

            /** @copydoc MeshVisualizer3D::Flag::PrimitiveIdFromVertexId */
            #ifndef MAGNUM_TARGET_WEBGL
            PrimitiveIdFromVertexId = (1 << 5)|PrimitiveId
            #else
            PrimitiveIdFromVertexId = (1 << 5)|(1 << 4)
            #endif
            #endif
        };

        /** @brief Flags */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags     Flags
         *
         * At least @ref Flag::Wireframe is expected to be enabled.
         */
        explicit MeshVisualizer2D(Flags flags);

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
        explicit MeshVisualizer2D(NoCreateT) noexcept: Implementation::MeshVisualizerBase{NoCreate} {}

        /** @brief Copying is not allowed */
        MeshVisualizer2D(const MeshVisualizer2D&) = delete;

        /** @brief Move constructor */
        MeshVisualizer2D(MeshVisualizer2D&&) noexcept = default;

        /** @brief Copying is not allowed */
        MeshVisualizer2D& operator=(const MeshVisualizer2D&) = delete;

        /** @brief Move assignment */
        MeshVisualizer2D& operator=(MeshVisualizer2D&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const {
            return Flag(UnsignedShort(Implementation::MeshVisualizerBase::_flags));
        }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        MeshVisualizer2D& setTransformationProjectionMatrix(const Matrix3& matrix);

        /**
         * @brief Set viewport size
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Wireframe is enabled and geometry
         * shaders are used, otherwise it does nothing. Initial value is a zero
         * vector.
         */
        MeshVisualizer2D& setViewportSize(const Vector2& size);

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. Expects that either
         * @ref Flag::Wireframe or @ref Flag::InstancedObjectId /
         * @ref Flag::PrimitiveId is enabled. In case of the latter, the color
         * is multiplied with the color map coming from
         * @ref bindColorMapTexture().
         */
        MeshVisualizer2D& setColor(const Color4& color) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::setColor(color));
        }

        /**
         * @brief Set wireframe color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x000000ff_rgbaf @ce. Expects that
         * @ref Flag::Wireframe is enabled.
         */
        MeshVisualizer2D& setWireframeColor(const Color4& color) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::setWireframeColor(color));
        }

        /**
         * @brief Set wireframe width
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 1.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         */
        MeshVisualizer2D& setWireframeWidth(Float width) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::setWireframeWidth(width));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc MeshVisualizer3D::setColorMapTransformation() */
        MeshVisualizer2D& setColorMapTransformation(Float offset, Float scale) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::setColorMapTransformation(offset, scale));
        }

        /** @copydoc MeshVisualizer3D::bindColorMapTexture() */
        MeshVisualizer2D& bindColorMapTexture(GL::Texture2D& texture) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::bindColorMapTexture(texture));
        }
        #endif

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 2.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         */
        MeshVisualizer2D& setSmoothness(Float smoothness);

    private:
        Int _transformationProjectionMatrixUniform{0};
};

/**
@brief 3D mesh visualization shader

Visualizes wireframe, per-vertex/per-instance object ID, primitive ID or
tangent space of 3D meshes. You need to provide the @ref Position attribute in
your triangle mesh at the very least. Use @ref setTransformationProjectionMatrix(),
@ref setColor() and others to configure the shader.

@m_class{m-row}

@parblock

@m_div{m-col-m-4 m-col-t-6 m-push-m-2 m-text-center m-nopadt m-nopadx}
@image html shaders-meshvisualizer3d.png width=256px
@ref Shaders-MeshVisualizer-wireframe, \n
@ref Shaders-MeshVisualizer-tbn
@m_enddiv

@m_div{m-col-m-4 m-col-t-6 m-push-m-2 m-text-center m-nopadt m-nopadx}
@image html shaders-meshvisualizer3d-primitiveid.png width=256px
@ref Shaders-MeshVisualizer-object-id
@m_enddiv

@endparblock

The shader expects that you enable wireframe visualization, tangent space
visualization or object/primitive ID visualization by passing an appropriate
@ref Flag to the constructor --- there's no default behavior with nothing
enabled.

@section Shaders-MeshVisualizer-wireframe Wireframe visualization

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
(see @ref MeshTools::duplicate() for a possible solution). Additionaly, if you
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

@subsection Shaders-MeshVisualizer-wireframe-geom Example setup with a geometry shader (desktop GL, OpenGL ES 3.2)

Common mesh setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-geom1

Common rendering setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-geom2

@subsection Shaders-MeshVisualizer-wireframe-no-geom Example setup for indexed meshes without a geometry shader

The vertices have to be converted to a non-indexed array first. Mesh setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-no-geom1

Rendering setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-no-geom2

@subsection Shaders-MeshVisualizer-usage-wireframe-no-geom-old Wireframe visualization of non-indexed meshes without a geometry shader on older hardware

You need to provide also the @ref VertexIndex attribute. Mesh setup *in
addition to the above*:

@snippet MagnumShaders.cpp MeshVisualizer-usage-no-geom-old

Rendering setup the same as above.

@section Shaders-MeshVisualizer-tbn Tangent space visualization

On platforms with geometry shaders (desktop GL, OpenGL ES 3.2), the shader is
able to visualize tangents, bitangent and normal direction via colored lines
coming out of vertices (red, green and blue for tangent, bitangent and normal, respectively). This can be enabled together with wireframe visualization,
however note that when both are enabled, the lines are not antialiased to avoid
depth ordering artifacts.

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

@snippet MagnumShaders.cpp MeshVisualizer-usage-tbn1

Rendering setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-tbn2

@section Shaders-MeshVisualizer-object-id Object, vertex and primitive ID visualization

If the mesh contains a per-vertex (or instanced) @ref ObjectId, it can be
visualized by enabling @ref Flag::InstancedObjectId. For the actual
visualization you need to provide a color map using @ref bindColorMapTexture()
and use @ref setColorMapTransformation() to map given range of discrete IDs to
the @f$ [0, 1] @f$ texture range. Various colormap presets are in the
@ref DebugTools::ColorMap namespace. Example usage:

@snippet MagnumShaders.cpp MeshVisualizer-usage-object-id

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

@requires_gl32 The `gl_PrimitiveID` shader variable is not available on OpenGL
    3.1 and lower.
@requires_gl30 The `gl_VertexID` shader variable is not available on OpenGL
    2.1.
@requires_gles32 The `gl_PrimitiveID` shader variable is not available on
    OpenGL ES 3.1 and lower.
@requires_gles30 The `gl_VertexID` shader variable is not available on OpenGL
    ES 2.0.
@requires_gles `gl_PrimitiveID` is not available in WebGL.
@requires_webgl20 `gl_VertexID` is not available in WebGL 1.0.

@see @ref shaders, @ref MeshVisualizer2D
@todo Understand and add support wireframe width/smoothness without GS
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizer3D: public Implementation::MeshVisualizerBase {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef typename Generic3D::Position Position;

        /**
         * @brief Tangent direction
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3". Use either this or the @ref Tangent4
         * attribute. Used only if @ref Flag::TangentDirection is enabled.
         */
        typedef typename Generic3D::Tangent Tangent;

        /**
         * @brief Tangent direction with a bitangent sign
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector4 "Vector4". Use either this or the @ref Tangent
         * attribute. Used only if @ref Flag::TangentDirection or
         * @ref Flag::BitangentFromTangentDirection is enabled.
         */
        typedef typename Generic3D::Tangent4 Tangent4;

        /**
         * @brief Bitangent direction
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3". Use either this or the @ref Tangent4
         * attribute. Used only if @ref Flag::BitangentDirection is enabled.
         */
        typedef typename Generic3D::Bitangent Bitangent;

        /**
         * @brief Normal direction
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3". Used only if
         * @ref Flag::NormalDirection is enabled.
         */
        typedef typename Generic3D::Normal Normal;

        /**
         * @brief Vertex index
         *
         * @ref Magnum::Float "Float", used only in OpenGL < 3.1 and OpenGL ES
         * 2.0 if @ref Flag::Wireframe is enabled. This attribute (modulo 3)
         * specifies index of given vertex in triangle, i.e. @cpp 0.0f @ce for
         * first, @cpp 1.0f @ce for second, @cpp 2.0f @ce for third. In OpenGL
         * 3.1, OpenGL ES 3.0 and newer this value is provided via the
         * @cb{.glsl} gl_VertexID @ce shader builtin, so the attribute is not
         * needed.
         *
         * @note This attribute uses the same slot as @ref Generic::ObjectId,
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
         * @ref shaders-generic "Generic attribute", @ref Magnum::UnsignedInt.
         * Used only if @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        typedef Generic3D::ObjectId ObjectId;
        #endif

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = Generic3D::ColorOutput
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref MeshVisualizer()
         */
        enum class Flag: UnsignedShort {
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
             * Visualize instanced object ID. You need to provide the
             * @ref ObjectId attribute in the mesh. Mutually exclusive with
             * @ref Flag::VertexId and @ref Flag::PrimitiveId.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = 1 << 2,

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
            NormalDirection = 1 << 9
            #endif
        };

        /** @brief Flags */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags     Flags
         *
         * At least @ref Flag::Wireframe or one of @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection, @ref Flag::NormalDirection is
         * expected to be enabled.
         */
        explicit MeshVisualizer3D(Flags flags);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since{2020,06} Use @ref MeshVisualizer3D(Flags) instead.
         */
        explicit CORRADE_DEPRECATED("use MeshVisualizer3D(Flags) instead") MeshVisualizer3D(): MeshVisualizer3D{{}} {}
        #endif

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
        explicit MeshVisualizer3D(NoCreateT) noexcept: Implementation::MeshVisualizerBase{NoCreate} {}

        /** @brief Copying is not allowed */
        MeshVisualizer3D(const MeshVisualizer3D&) = delete;

        /** @brief Move constructor */
        MeshVisualizer3D(MeshVisualizer3D&&) noexcept = default;

        /** @brief Copying is not allowed */
        MeshVisualizer3D& operator=(const MeshVisualizer3D&) = delete;

        /** @brief Move assignment */
        MeshVisualizer3D& operator=(MeshVisualizer3D&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const {
            return Flag(UnsignedShort(Implementation::MeshVisualizerBase::_flags));
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set transformation and projection matrix
         * @m_deprecated_since{2020,06} Use @ref setTransformationMatrix() and
         *      @ref setProjectionMatrix() instead.
         */
        CORRADE_DEPRECATED("use setTransformationMatrix() and setProjectionMatrix() instead") MeshVisualizer3D& setTransformationProjectionMatrix(const Matrix4& matrix) {
            /* Keep projection at identity, which should still work for
               wireframe (but of course not for TBN visualization) */
            return setTransformationMatrix(matrix);
        }
        #endif

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        MeshVisualizer3D& setTransformationMatrix(const Matrix4& matrix);

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix. (i.e., an orthographic
         * projection of the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$
         * cube).
         */
        MeshVisualizer3D& setProjectionMatrix(const Matrix4& matrix);

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
         * Initial value is an identity matrix.
         * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        MeshVisualizer3D& setNormalMatrix(const Matrix3x3& matrix);
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
        MeshVisualizer3D& setViewportSize(const Vector2& size);

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. Expects that either
         * @ref Flag::Wireframe or @ref Flag::InstancedObjectId /
         * @ref Flag::PrimitiveId is enabled. In case of the latter, the color
         * is multiplied with the color map coming from
         * @ref bindColorMapTexture().
         */
        MeshVisualizer3D& setColor(const Color4& color) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setColor(color));
        }

        /**
         * @brief Set wireframe color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x000000ff_rgbaf @ce. Expects that
         * @ref Flag::Wireframe is enabled.
         */
        MeshVisualizer3D& setWireframeColor(const Color4& color) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setWireframeColor(color));
        }

        /**
         * @brief Set wireframe width
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 1.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         */
        MeshVisualizer3D& setWireframeWidth(Float width) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setWireframeWidth(width));
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
         * Note that this shader doesn't directly offer a
         * @ref Flat::setObjectId() "setObjectId()" uniform that's used to
         * offset the per-vertex / per-instance ID. Instead, you need to encode
         * the base offset into the @p offset parameter.
         * @requires_gles30 Object ID visualization requires integer attributes
         *      while primitive ID visualization requires the `gl_VertexID` /
         *      `gl_PrimitiveID` builtins, neither of which is available in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Object ID visualization requires integer
         *      attributes while primitive ID visualization requires at least
         *      the `gl_VertexID` builtin, neither of which is available in
         *      WebGL 1.
         */
        MeshVisualizer3D& setColorMapTransformation(Float offset, Float scale) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setColorMapTransformation(offset, scale));
        }

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
        MeshVisualizer3D& bindColorMapTexture(GL::Texture2D& texture) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::bindColorMapTexture(texture));
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Set line width
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 1.0f @ce. Expects that
         * @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled.
         * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        MeshVisualizer3D& setLineWidth(Float width);

        /**
         * @brief Set line length
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Value is in object space, initial value is @cpp 1.0f @ce. Expects
         * that @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled.
         * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        MeshVisualizer3D& setLineLength(Float length);
        #endif

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 2.0f @ce. Expects that @ref Flag::Wireframe,
         * @ref Flag::TangentDirection,
         * @ref Flag::BitangentFromTangentDirection,
         * @ref Flag::BitangentDirection or @ref Flag::NormalDirection is
         * enabled.
         */
        MeshVisualizer3D& setSmoothness(Float smoothness);

    private:
        Int _transformationMatrixUniform{0},
            _projectionMatrixUniform{7};
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        Int _normalMatrixUniform{8},
            _lineWidthUniform{9},
            _lineLengthUniform{10};
        #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 3D mesh visualizer shader
@m_deprecated_since{2020,06} Use @ref MeshVisualizer3D instead.
*/
typedef CORRADE_DEPRECATED("use MeshVisualizer3D instead") MeshVisualizer3D MeshVisualizer;
#endif

/** @debugoperatorclassenum{MeshVisualizer2D,MeshVisualizer2D::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizer2D::Flag value);

/** @debugoperatorclassenum{MeshVisualizer3D,MeshVisualizer3D::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizer3D::Flag value);

/** @debugoperatorclassenum{MeshVisualizer2D,MeshVisualizer2D::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizer2D::Flags value);

/** @debugoperatorclassenum{MeshVisualizer3D,MeshVisualizer3D::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, MeshVisualizer3D::Flags value);

CORRADE_ENUMSET_OPERATORS(MeshVisualizer2D::Flags)
CORRADE_ENUMSET_OPERATORS(MeshVisualizer3D::Flags)

}}

#endif
