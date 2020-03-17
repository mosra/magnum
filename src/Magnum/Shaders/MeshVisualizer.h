#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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
        enum class FlagBase: UnsignedByte {
            #ifndef MAGNUM_TARGET_GLES2
            Wireframe = 1 << 0,
            #else
            Wireframe = (1 << 0) | (1 << 1),
            #endif
            NoGeometryShader = 1 << 1
        };
        typedef Containers::EnumSet<FlagBase> FlagsBase;

        CORRADE_ENUMSET_FRIEND_OPERATORS(FlagsBase)

        explicit MeshVisualizerBase(FlagsBase flags);
        explicit MeshVisualizerBase(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        MAGNUM_SHADERS_LOCAL GL::Version setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs) const;

        MeshVisualizerBase& setViewportSize(const Vector2& size);
        MeshVisualizerBase& setColor(const Color4& color);
        MeshVisualizerBase& setWireframeColor(const Color4& color);
        MeshVisualizerBase& setWireframeWidth(Float width);
        MeshVisualizerBase& setSmoothness(Float smoothness);

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
};

}

/**
@brief 2D mesh visualization shader
@m_since_latest

Uses the geometry shader to visualize wireframe of 3D meshes. You need to
provide the @ref Position attribute in your triangle mesh. By default, the
shader renders the mesh with a white color in an identity transformation. Use
@ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

@image html shaders-meshvisualizer2d.png width=256px

This shader is a 2D variant of @ref MeshVisualizer3D with mostly identical
workflow. See its documentation for more information.
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
        enum class Flag: UnsignedByte {
            /**
             * Visualize wireframe. On OpenGL ES 2.0 this also enables
             * @ref Flag::NoGeometryShader.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Wireframe = 1 << 0,
            #else
            Wireframe = (1 << 0) | (1 << 1),
            #endif

            /**
             * Don't use a geometry shader for wireframe visualization. If
             * enabled, you might need to provide also the @ref VertexIndex
             * attribute in the mesh. In OpenGL ES 2.0 enabled alongside
             * @ref Flag::Wireframe.
             */
            NoGeometryShader = 1 << 1
        };

        /** @brief Flags */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit MeshVisualizer2D(Flags flags = {});

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
            return reinterpret_cast<const Flags&>(Implementation::MeshVisualizerBase::_flags);
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
        MeshVisualizer2D& setViewportSize(const Vector2& size) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::setViewportSize(size));
        }

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
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

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 2.0f @ce. Expects that @ref Flag::Wireframe is
         * enabled.
         */
        MeshVisualizer2D& setSmoothness(Float smoothness) {
            return static_cast<MeshVisualizer2D&>(Implementation::MeshVisualizerBase::setSmoothness(smoothness));
        }

    private:
        Int _transformationProjectionMatrixUniform{0};
};

/**
@brief 3D mesh visualization shader

Uses the geometry shader to visualize wireframe of 3D meshes. You need to
provide the @ref Position attribute in your triangle mesh. By default, the
shader renders the mesh with a white color in an identity transformation. Use
@ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

@image html shaders-meshvisualizer3d.png width=256px

@section Shaders-MeshVisualizer-wireframe Wireframe visualization

Wireframe visualization is done by enabling @ref Flag::Wireframe. It is done
either using geometry shaders or with help of additional vertex information. If
you have geometry shaders available, you don't need to do anything else.

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
is optionally used for improving line appearance.

@section Shaders-MeshVisualizer-usage Example usage

@subsection Shaders-MeshVisualizer-usage-wireframe-geom Wireframe visualization with a geometry shader (desktop GL, OpenGL ES 3.2)

Common mesh setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-geom1

Common rendering setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-geom2

@subsection Shaders-MeshVisualizer-usage-wireframe-no-geom Wireframe visualization of indexed meshes without a geometry shader

The vertices have to be converted to a non-indexed array first. Mesh setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-no-geom1

Rendering setup:

@snippet MagnumShaders.cpp MeshVisualizer-usage-no-geom2

@subsection Shaders-MeshVisualizer-usage-wireframe-no-geom-old Wireframe visualization of non-indexed meshes without a geometry shader on older hardware

You need to provide also the @ref VertexIndex attribute. Mesh setup *in
addition to the above*:

@snippet MagnumShaders.cpp MeshVisualizer-usage-no-geom-old

Rendering setup the same as above.

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
        enum class Flag: UnsignedByte {
            /**
             * Visualize wireframe. On OpenGL ES 2.0 this also enables
             * @ref Flag::NoGeometryShader.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Wireframe = 1 << 0,
            #else
            Wireframe = (1 << 0) | (1 << 1),
            #endif

            /**
             * Don't use a geometry shader for wireframe visualization. If
             * enabled, you might need to provide also the @ref VertexIndex
             * attribute in the mesh. In OpenGL ES 2.0 enabled alongside
             * @ref Flag::Wireframe.
             */
            NoGeometryShader = 1 << 1
        };

        /** @brief Flags */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit MeshVisualizer3D(Flags flags = {});

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
            return reinterpret_cast<const Flags&>(Implementation::MeshVisualizerBase::_flags);
        }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        MeshVisualizer3D& setTransformationProjectionMatrix(const Matrix4& matrix);

        /**
         * @brief Set viewport size
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Wireframe is enabled and geometry
         * shaders are used. Initial value is a zero vector.
         */
        MeshVisualizer3D& setViewportSize(const Vector2& size) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setViewportSize(size));
        }

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
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
         * Initial value is @cpp 1.0f @ce. Has effect only if
         * @ref Flag::Wireframe is enabled.
         */
        MeshVisualizer3D& setWireframeWidth(Float width) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setWireframeWidth(width));
        }

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Value is in screen space (depending on @ref setViewportSize()),
         * initial value is @cpp 2.0f @ce. Expects that @ref Flag::Wireframe
         * is enabled.
         */
        MeshVisualizer3D& setSmoothness(Float smoothness) {
            return static_cast<MeshVisualizer3D&>(Implementation::MeshVisualizerBase::setSmoothness(smoothness));
        }

    private:
        Int _transformationProjectionMatrixUniform{0};
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 3D mesh visualizer shader
@m_deprecated_since_latest Use @ref MeshVisualizer3D instead.
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
