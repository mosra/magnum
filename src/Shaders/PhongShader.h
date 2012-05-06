#ifndef Magnum_Shaders_PhongShader_h
#define Magnum_Shaders_PhongShader_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Shaders::PhongShader
 */

#include "AbstractShaderProgram.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong shader

@requires_gl33 The shader is written in GLSL 3.3, although it should be trivial
    to port it to older versions.
*/
class PhongShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector4> Vertex;   /**< @brief Vertex position */
        typedef Attribute<1, Vector3> Normal;   /**< @brief Normal direction */

        /** @brief Constructor */
        PhongShader();

        /** @brief %Object ambient color */
        inline void setAmbientColorUniform(const Vector3& color) {
            setUniform(ambientColorUniform, color);
        }

        /** @brief %Object diffuse color */
        inline void setDiffuseColorUniform(const Vector3& color) {
            setUniform(diffuseColorUniform, color);
        }

        /** @brief %Object specular color */
        inline void setSpecularColorUniform(const Vector3& color) {
            setUniform(specularColorUniform, color);
        }

        /** @brief %Object shininess */
        inline void setShininessUniform(GLfloat shininess) {
            setUniform(shininessUniform, shininess);
        }

        /** @brief Transformation matrix */
        inline void setTransformationMatrixUniform(const Matrix4& matrix) {
            setUniform(transformationMatrixUniform, matrix);
        }

        /** @brief Projection matrix */
        inline void setProjectionMatrixUniform(const Matrix4& matrix) {
            setUniform(projectionMatrixUniform, matrix);
        }

        /** @brief %Light position */
        inline void setLightUniform(const Vector3& light) {
            setUniform(lightUniform, light);
        }

        /** @brief %Light ambient color */
        inline void setLightAmbientColorUniform(const Vector3& color) {
            setUniform(lightAmbientColorUniform, color);
        }

        /** @brief %Light diffuse color */
        inline void setLightDiffuseColorUniform(const Vector3& color) {
            setUniform(lightDiffuseColorUniform, color);
        }

        /** @brief %Light specular color */
        inline void setLightSpecularColorUniform(const Vector3& color) {
            setUniform(lightSpecularColorUniform, color);
        }

    private:
        GLint ambientColorUniform,
            diffuseColorUniform,
            specularColorUniform,
            shininessUniform,
            transformationMatrixUniform,
            projectionMatrixUniform,
            lightUniform,
            lightAmbientColorUniform,
            lightDiffuseColorUniform,
            lightSpecularColorUniform;
};

}}

#endif
