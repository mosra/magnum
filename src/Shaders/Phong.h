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
@brief Phong shader

If supported, uses GLSL 3.20 and @extension{ARB,explicit_attrib_location},
otherwise falls back to GLSL 1.20.
*/
class MAGNUM_SHADERS_EXPORT Phong: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position; /**< @brief Vertex position */
        typedef Attribute<1, Vector3> Normal;   /**< @brief Normal direction */

        explicit Phong();

        /**
         * @brief Set ambient color
         * @return Pointer to self (for method chaining)
         *
         * If not set, default value is `(0.0f, 0.0f, 0.0f)`.
         */
        Phong* setAmbientColor(const Color3& color) {
            setUniform(ambientColorUniform, color);
            return this;
        }

        /**
         * @brief Set diffuse color
         * @return Pointer to self (for method chaining)
         */
        Phong* setDiffuseColor(const Color3& color) {
            setUniform(diffuseColorUniform, color);
            return this;
        }

        /**
         * @brief Set specular color
         * @return Pointer to self (for method chaining)
         *
         * If not set, default value is `(1.0f, 1.0f, 1.0f)`.
         */
        Phong* setSpecularColor(const Color3& color) {
            setUniform(specularColorUniform, color);
            return this;
        }

        /**
         * @brief Set shininess
         * @return Pointer to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * If not set, default value is `80.0f`.
         */
        Phong* setShininess(Float shininess) {
            setUniform(shininessUniform, shininess);
            return this;
        }

        /**
         * @brief Set transformation and normal matrix
         * @return Pointer to self (for method chaining)
         */
        Phong* setTransformationMatrix(const Matrix4& matrix) {
            setUniform(transformationMatrixUniform, matrix);
            setUniform(normalMatrixUniform, matrix.rotation());
            return this;
        }

        /**
         * @brief Set projection matrix
         * @return Pointer to self (for method chaining)
         */
        Phong* setProjectionMatrix(const Matrix4& matrix) {
            setUniform(projectionMatrixUniform, matrix);
            return this;
        }

        /**
         * @brief Set light position
         * @return Pointer to self (for method chaining)
         */
        Phong* setLightPosition(const Vector3& light) {
            setUniform(lightUniform, light);
            return this;
        }

        /**
         * @brief Set light color
         * @return Pointer to self (for method chaining)
         *
         * If not set, default value is `(1.0f, 1.0f, 1.0f)`.
         */
        Phong* setLightColor(const Color3& color) {
            setUniform(lightColorUniform, color);
            return this;
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
};

}}

#endif
