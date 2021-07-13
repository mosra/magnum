#ifndef Magnum_Shaders_Generic_h
#define Magnum_Shaders_Generic_h
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

/** @file
 * @brief Struct @ref Magnum::Shaders::ProjectionUniform2D, @ref Magnum::Shaders::ProjectionUniform3D, @ref Magnum::Shaders::TransformationUniform2D, @ref Magnum::Shaders::TransformationUniform3D, @ref Magnum::Shaders::TextureTransformationUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/GenericGL.h"
#endif

namespace Magnum { namespace Shaders {

/**
@brief 2D projection uniform common for all shaders
@m_since_latest

Contains the per-view projection matrix.
*/
struct ProjectionUniform2D {
    /** @brief Construct with default parameters */
    constexpr explicit ProjectionUniform2D(DefaultInitT = DefaultInit) noexcept: projectionMatrix{Math::IdentityInit} {}
    /** @brief Construct without initializing the contents */
    explicit ProjectionUniform2D(NoInitT) noexcept: projectionMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref projectionMatrix field
     * @return Reference to self (for method chaining)
     *
     * The matrix is expanded to @relativeref{Magnum,Matrix3x4}, with the
     * bottom row being zeros.
     */
    ProjectionUniform2D& setProjectionMatrix(const Matrix3& matrix) {
        projectionMatrix = Matrix3x4{matrix};
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Projection matrix
     *
     * Default value is an identity matrix (i.e., an orthographic projection of
     * the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$ cube). The
     * bottom row is unused and acts only as a padding to match uniform buffer
     * packing rules.
     */
    Matrix3x4 projectionMatrix;
};

/**
@brief 3D projection uniform common for all shaders
@m_since_latest

Contains the per-view projection matrix used by the @ref MeshVisualizerGL3D
and @ref PhongGL shaders that need a separate projection and transformation
matrix.
@see @ref MeshVisualizerGL3D::bindProjectionBuffer(),
    @ref PhongGL::bindProjectionBuffer()
*/
struct ProjectionUniform3D {
    /** @brief Construct with default parameters */
    constexpr explicit ProjectionUniform3D(DefaultInitT = DefaultInit) noexcept: projectionMatrix{Math::IdentityInit} {}
    /** @brief Construct without initializing the contents */
    explicit ProjectionUniform3D(NoInitT) noexcept: projectionMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref projectionMatrix field
     * @return Reference to self (for method chaining)
     */
    ProjectionUniform3D& setProjectionMatrix(const Matrix4& matrix) {
        projectionMatrix = matrix;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Projection matrix
     *
     * Default value is an identity matrix (i.e., an orthographic projection of
     * the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$ cube).
     * @see @ref PhongGL::setProjectionMatrix()
     */
    Matrix4 projectionMatrix;
};

/**
@brief 2D transformation uniform common for all shaders
@m_since_latest

Contains the per-draw transformation matrix.
*/
struct TransformationUniform2D {
    /** @brief Construct with default parameters */
    constexpr explicit TransformationUniform2D(DefaultInitT = DefaultInit) noexcept: transformationMatrix{Math::IdentityInit} {}
    /** @brief Construct without initializing the contents */
    explicit TransformationUniform2D(NoInitT) noexcept: transformationMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref transformationMatrix field
     * @return Reference to self (for method chaining)
     *
     * The matrix is expanded to @relativeref{Magnum,Matrix3x4}, with the
     * bottom row being zeros.
     */
    TransformationUniform2D& setTransformationMatrix(const Matrix3& matrix) {
        transformationMatrix = Matrix3x4{matrix};
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Transformation matrix
     *
     * Default value is an identity matrix. The bottom row is unused and acts
     * only as a padding to match uniform buffer packing rules.
     */
    Matrix3x4 transformationMatrix;
};

/**
@brief 3D transformation uniform common for all shaders
@m_since_latest

Contains the per-draw transformation matrix used by the @ref MeshVisualizerGL3D
and @ref PhongGL shaders that need a separate projection and transformation
matrix.
@see @ref MeshVisualizerGL3D::bindTransformationBuffer(),
    @ref PhongGL::bindTransformationBuffer()
*/
struct TransformationUniform3D {
    /** @brief Construct with default parameters */
    constexpr explicit TransformationUniform3D(DefaultInitT = DefaultInit) noexcept: transformationMatrix{Math::IdentityInit} {}
    /** @brief Construct without initializing the contents */
    explicit TransformationUniform3D(NoInitT) noexcept: transformationMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref transformationMatrix field
     * @return Reference to self (for method chaining)
     */
    TransformationUniform3D& setTransformationMatrix(const Matrix4& matrix) {
        transformationMatrix = matrix;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Transformation matrix
     *
     * Default value is an identity matrix.
     *
     * If @ref PhongGL::Flag::InstancedTransformation is enabled, the
     * per-instance transformation coming from the
     * @ref PhongGL::TransformationMatrix attribute is applied first, before
     * this one.
     * @see @ref PhongGL::setTransformationMatrix(),
     *      @ref MeshVisualizerGL3D::setTransformationMatrix()
     */
    Matrix4 transformationMatrix;
};

/**
@brief Combined 2D projection and transformation uniform common for all shaders
@m_since_latest

Used by @ref FlatGL, @ref MeshVisualizerGL2D, @ref DistanceFieldVectorGL,
@ref VectorGL and @ref VertexColorGL that don't need to have a separate
projection matrix supplied.
@see @ref DistanceFieldVectorGL2D::bindTransformationProjectionBuffer(),
    @ref FlatGL2D::bindTransformationProjectionBuffer(),
    @ref MeshVisualizerGL2D::bindTransformationProjectionBuffer(),
    @ref VectorGL2D::bindTransformationProjectionBuffer(),
    @ref VertexColorGL2D::bindTransformationProjectionBuffer()
*/
struct TransformationProjectionUniform2D {
    /** @brief Construct with default parameters */
    constexpr explicit TransformationProjectionUniform2D(DefaultInitT = DefaultInit) noexcept: transformationProjectionMatrix{Math::IdentityInit} {}
    /** @brief Construct without initializing the contents */
    explicit TransformationProjectionUniform2D(NoInitT) noexcept: transformationProjectionMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref transformationProjectionMatrix field
     * @return Reference to self (for method chaining)
     *
     * The matrix is expanded to @relativeref{Magnum,Matrix3x4}, with the
     * bottom row being zeros.
     */
    TransformationProjectionUniform2D& setTransformationProjectionMatrix(const Matrix3& matrix) {
        transformationProjectionMatrix = Matrix3x4{matrix};
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Transformation and projection matrix
     *
     * Default value is an identity matrix. The bottom row is unused and acts
     * only as a padding to match uniform buffer packing rules.
     *
     * If @ref FlatGL::Flag::InstancedTransformation is enabled, the
     * per-instance transformation coming from the
     * @ref FlatGL::TransformationMatrix attribute is applied first, before
     * this one.
     * @see @ref DistanceFieldVectorGL2D::setTransformationProjectionMatrix(),
     *      @ref FlatGL2D::setTransformationProjectionMatrix(),
     *      @ref MeshVisualizerGL2D::setTransformationProjectionMatrix(),
     *      @ref VectorGL2D::setTransformationProjectionMatrix(),
     *      @ref VertexColorGL2D::setTransformationProjectionMatrix()
     */
    Matrix3x4 transformationProjectionMatrix;
};

/**
@brief Combined 3D projection and transformation uniform common for all shaders
@m_since_latest

Used by @ref FlatGL, @ref DistanceFieldVectorGL, @ref VectorGL and
@ref VertexColorGL that don't need to have a separate projection matrix
supplied.
@see @ref DistanceFieldVectorGL3D::bindTransformationProjectionBuffer(),
    @ref FlatGL3D::bindTransformationProjectionBuffer(),
    @ref VectorGL3D::bindTransformationProjectionBuffer(),
    @ref VertexColorGL3D::bindTransformationProjectionBuffer()
*/
struct TransformationProjectionUniform3D {
    /** @brief Construct with default parameters */
    constexpr explicit TransformationProjectionUniform3D(DefaultInitT = DefaultInit) noexcept: transformationProjectionMatrix{Math::IdentityInit} {}
    /** @brief Construct without initializing the contents */
    explicit TransformationProjectionUniform3D(NoInitT) noexcept: transformationProjectionMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref transformationProjectionMatrix field
     * @return Reference to self (for method chaining)
     */
    TransformationProjectionUniform3D& setTransformationProjectionMatrix(const Matrix4& matrix) {
        transformationProjectionMatrix = matrix;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Transformation and projection matrix
     *
     * Default value is an identity matrix.
     *
     * If @ref FlatGL::Flag::InstancedTransformation is enabled, the
     * per-instance transformation coming from the
     * @ref FlatGL::TransformationMatrix attribute is applied first, before
     * this one.
     * @see @ref DistanceFieldVectorGL3D::setTransformationProjectionMatrix(),
     *      @ref FlatGL3D::setTransformationProjectionMatrix(),
     *      @ref VectorGL3D::setTransformationProjectionMatrix(),
     *      @ref VertexColorGL3D::setTransformationProjectionMatrix()
     */
    Matrix4 transformationProjectionMatrix;
};

/**
@brief Texture transformation uniform common for all shaders
@m_since_latest

Expands upon @ref TransformationUniform2D / @ref TransformationUniform3D with
texture-related parameters.

Used only if @ref DistanceFieldVectorGL::Flag::TextureTransformation,
@ref FlatGL::Flag::TextureTransformation,
@ref PhongGL::Flag::TextureTransformation or
@ref VectorGL::Flag::TextureTransformation is enabled.
@see @ref DistanceFieldVectorGL::bindTextureTransformationBuffer(),
    @ref FlatGL::bindTextureTransformationBuffer(),
    @ref PhongGL::bindTextureTransformationBuffer(),
    @ref VectorGL::bindTextureTransformationBuffer()
*/
struct TextureTransformationUniform {
    /** @brief Construct with default parameters */
    constexpr explicit TextureTransformationUniform(DefaultInitT = DefaultInit) noexcept: rotationScaling{Math::IdentityInit}, offset{0.0f, 0.0f}, layer{0}
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        , _pad0{} /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        {}
    /** @brief Construct without initializing the contents */
    explicit TextureTransformationUniform(NoInitT) noexcept: rotationScaling{NoInit}, offset{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref rotationScaling and @ref offset fields
     * @return Reference to self (for method chaining)
     *
     * The @ref rotationScaling field is set to the upper left 2x2 corner of
     * @p transformation, @ref offset to the two upper elements of the
     * rightmost column of @p transformation. Bottom row is ignored, as it's
     * expected to be always @cpp {0.0f, 0.0f, 1.0f} @ce.
     */
    TextureTransformationUniform& setTextureMatrix(const Matrix3& transformation) {
        rotationScaling = transformation.rotationScaling();
        offset = transformation.translation();
        return *this;
    }

    /**
     * @brief Set the @ref layer field
     * @return Reference to self (for method chaining)
     */
    TextureTransformationUniform& setLayer(UnsignedInt layer) {
        this->layer = layer;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Texture rotation and scaling
     *
     * The top left part of a 3x3 texture transformation matrix. The
     * transformation is split between @ref rotationScaling and @ref offset to
     * make it occupy just two @cb{.glsl} vec4 @ce slots in the uniform buffer
     * instead of three. Default value is an identity matrix.
     *
     * If @ref FlatGL::Flag::InstancedTextureOffset /
     * @ref PhongGL::Flag::InstancedTextureOffset is enabled, the
     * per-instance offset coming from the @ref FlatGL::TextureOffset /
     * @ref PhongGL::TextureOffset attribute is applied first, before
     * this transformation.
     * @see @ref DistanceFieldVectorGL::setTextureMatrix(),
     *      @ref FlatGL::setTextureMatrix(), @ref PhongGL::setTextureMatrix(),
     *      @ref VectorGL::setTextureMatrix()
     */
    Matrix2x2 rotationScaling;

    /**
     * @brief Texture offset
     *
     * Top two elements of the rightmost column of a 3x3 texture transformation
     * matrix. The transformation is split between @ref rotationScaling and
     * @ref offset to make it occupy just two @cb{.glsl} vec4 @ce slots in the
     * uniform buffer instead of three. Default value is a zero vector.
     *
     * If @ref FlatGL::Flag::InstancedTextureOffset /
     * @ref PhongGL::Flag::InstancedTextureOffset is enabled, the
     * per-instance offset coming from the @ref FlatGL::TextureOffset /
     * @ref PhongGL::TextureOffset attribute is applied first, before
     * this transformation.
     * @see @ref DistanceFieldVectorGL::setTextureMatrix(),
     *      @ref FlatGL::setTextureMatrix(), @ref PhongGL::setTextureMatrix(),
     *      @ref VectorGL::setTextureMatrix()
     */
    Vector2 offset;

    /**
     * @brief Texture layer
     *
     * Descibes which layer of a texture array to use. Default value is
     * @cpp 0.5f @ce.
     *
     * Used only if @ref FlatGL::Flag::TextureArrays /
     * @ref PhongGL::Flag::TextureArrays is enabled, ignored otherwise. If
     * @ref FlatGL::Flag::InstancedTextureOffset /
     * @ref PhongGL::Flag::InstancedTextureOffset is enabled as well, the
     * per-instance layer coming from the @ref FlatGL::TextureOffsetLayer /
     * @ref PhongGL::TextureOffsetLayer attribute is added to this value.
     * @see @ref FlatGL::setTextureLayer(), @ref PhongGL::setTextureLayer()
     */
    UnsignedInt layer;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32; /* reserved for coordinateSet */
    #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/* Deprecated aliases not present here but in GenericGL.h instead, as a lot of
   existing code relies on these being transitively included from Phong.h etc.,
   and there are no forward declarations in Shaders.h as the type is never used
   like that. While we *could* include Generic.h from Phong.h, we'd have to
   also temporarily disable the CORRADE_DEPRECATED_FILE() macro there and it's
   more pain than it's worth. */
#endif

}}

#endif
