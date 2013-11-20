#ifndef Magnum_Shaders_Phong_h
#define Magnum_Shaders_Phong_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Shaders::Phong
 */

#include "Math/Matrix4.h"
#include "AbstractShaderProgram.h"
#include "Color.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief %Phong shader

Uses ambient, diffuse and specular color or texture. For colored mesh you need
to provide @ref Position and @ref Normal attributes in your triangle mesh and
call at least @ref setTransformationMatrix(), @ref setNormalMatrix(),
@ref setProjectionMatrix(), @ref setDiffuseColor() and @ref setLightPosition().

If you want to use texture instead of color, you need to provide also
@ref TextureCoordinates attribute. Pass appropriate flags to constructor and
then at render time bind the texture to its respective layer instead of setting
the color. Example:
@code
Shaders::Phong shader(Shaders::Phong::Flag::DiffuseTexture);

// ...

myDiffuseTexture.bind(Shaders::Phong::DiffuseTextureLayer);
@endcode
*/
class MAGNUM_SHADERS_EXPORT Phong: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position; /**< @brief Vertex position */
        typedef Attribute<1, Vector3> Normal;   /**< @brief Normal direction */

        /**
         * @brief Texture coordinates
         *
         * Used only if one of @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture
         * or @ref Flag::SpecularTexture is set.
         */
        typedef Attribute<2, Vector2> TextureCoordinates;

        enum: Int {
            /**
             * Layer for ambient texture. Used only if @ref Flag::AmbientTexture
             * is set.
             */
            AmbientTextureLayer = 0,

            /**
             * Layer for diffuse texture. Used only if @ref Flag::DiffuseTexture
             * is set.
             */
            DiffuseTextureLayer = 1,

            /**
             * Layer for specular texture. Used only if @ref Flag::SpecularTexture
             * is set.
             */
            SpecularTextureLayer = 2
        };

        /**
         * @brief Shader flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            AmbientTexture = 1 << 0,    /**< The shader uses ambient texture instead of color */
            DiffuseTexture = 1 << 1,    /**< The shader uses diffuse texture instead of color */
            SpecularTexture = 1 << 2    /**< The shader uses specular texture instead of color */
        };

        /**
         * @brief Shader flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag, UnsignedByte> Flags;

        /**
         * @brief Constructor
         * @param flags     Shader flags
         */
        explicit Phong(Flags flags = Flags());

        /** @brief Shader flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set ambient color
         * @return Reference to self (for method chaining)
         *
         * If not set, default value is `(0.0f, 0.0f, 0.0f)`. Has no effect if
         * @ref Flag::AmbientTexture is set.
         */
        Phong& setAmbientColor(const Color3& color);

        /**
         * @brief Set diffuse color
         * @return Reference to self (for method chaining)
         *
         * Has no effect if @ref Flag::AmbientTexture is used.
         */
        Phong& setDiffuseColor(const Color3& color);

        /**
         * @brief Set specular color
         * @return Reference to self (for method chaining)
         *
         * If not set, default value is `(1.0f, 1.0f, 1.0f)`. Has no effect if
         * @ref Flag::SpecularTexture is set.
         */
        Phong& setSpecularColor(const Color3& color);

        /**
         * @brief Set shininess
         * @return Reference to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * If not set, default value is `80.0f`.
         */
        Phong& setShininess(Float shininess) {
            setUniform(shininessUniform, shininess);
            return *this;
        }

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         */
        Phong& setTransformationMatrix(const Matrix4& matrix) {
            setUniform(transformationMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set normal matrix
         * @return Reference to self (for method chaining)
         */
        Phong& setNormalMatrix(const Matrix3x3& matrix) {
            setUniform(normalMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         */
        Phong& setProjectionMatrix(const Matrix4& matrix) {
            setUniform(projectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set light position
         * @return Reference to self (for method chaining)
         */
        Phong& setLightPosition(const Vector3& light) {
            setUniform(lightUniform, light);
            return *this;
        }

        /**
         * @brief Set light color
         * @return Reference to self (for method chaining)
         *
         * If not set, default value is `(1.0f, 1.0f, 1.0f)`.
         */
        Phong& setLightColor(const Color3& color) {
            setUniform(lightColorUniform, color);
            return *this;
        }

    private:
        Int transformationMatrixUniform,
            projectionMatrixUniform,
            normalMatrixUniform,
            lightUniform,
            diffuseColorUniform,
            ambientColorUniform,
            specularColorUniform,
            lightColorUniform,
            shininessUniform;

        Flags _flags;
};

CORRADE_ENUMSET_OPERATORS(Phong::Flags)

inline Phong& Phong::setAmbientColor(const Color3& color) {
    if(!(_flags & Flag::AmbientTexture)) setUniform(ambientColorUniform, color);
    return *this;
}

inline Phong& Phong::setDiffuseColor(const Color3& color) {
    if(!(_flags & Flag::DiffuseTexture)) setUniform(diffuseColorUniform, color);
    return *this;
}

inline Phong& Phong::setSpecularColor(const Color3& color) {
    if(!(_flags & Flag::SpecularTexture)) setUniform(specularColorUniform, color);
    return *this;
}

}}

#endif
