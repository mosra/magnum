#ifndef Magnum_AbstractShaderProgram_h
#define Magnum_AbstractShaderProgram_h
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
 * @brief Class Magnum::AbstractShaderProgram
 */

#include <map>

#include "Shader.h"
#include "AbstractTexture.h"
#include "BufferedTexture.h"

namespace Magnum {

/**
@brief Base class for shaders

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - **Attribute location** typedefs defining locations and types for attribute
   binding with Mesh::bindAttribute(), for example:
@code
typedef Attribute<0, Vector4> Vertex;
typedef Attribute<1, Vector3> Normal;
typedef Attribute<2, Vector2> TextureCoords;
@endcode
   See also bindAttribute().
 - **Constructor**, which attaches particular shaders, links the program,
   binds attribute locations and gets uniform locations, for example:
@code
// Load shaders from file and attach them to the program
attachShader(Shader::fromFile(Shader::Vertex, "PhongShader.vert"));
attachShader(Shader::fromFile(Shader::Fragment, "PhongShader.frag"));

// Bind attribute names to IDs
bindAttribute(Vertex::Location, "vertex");
bindAttribute(Normal::Location, "normal");
bindAttribute(TextureCoords::Location, "textureCoords");

// Link
link();

// Get locations of uniforms
transformationMatrixUniform = uniformLocation("transformationMatrix");
projectionMatrixUniform = uniformLocation("projectionMatrix");
// more uniforms like light location, colors etc.
@endcode
 - **Uniform binding functions**, which set shader uniforms using
   setUniform() and setUniformArray() functions. Example:
@code
void setTransformationMatrixUniform(const Matrix4& matrix) {
    setUniform(transformationMatrixUniform, matrix);
}
@endcode

Basic workflow with AbstractShaderProgram subclasses is: instancing the class
(once at the beginning), then in every frame calling use(), setting uniforms
and calling Mesh::draw() (see its documentation for more).

@section MultipleFragmentOutputs Multiple fragment shader outputs
If your shader uses multiple fragment outputs, you can use
bindFragmentDataLocation() *before linking* to bind their names to desired
location, e.g.:
@code
bindFragmentDataLocation(0, "color");
@endcode

You should then clearly state in the documentation which output is on what
position, so the user can set framebuffer attachments for them using
Framebuffer::mapForDraw() or Framebuffer::mapDefaultForDraw().
 */
class MAGNUM_EXPORT AbstractShaderProgram {
    AbstractShaderProgram(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram(AbstractShaderProgram&& other) = delete;
    AbstractShaderProgram& operator=(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram& operator=(AbstractShaderProgram&& other) = delete;

    public:
        /**
         * @brief Base struct for attribute location and type
         *
         * See AbstractShaderProgram documentation or Mesh::bindAttribute()
         * for an example.
         *
         * @todo Support for BGRA attribute type (OpenGL 3.2, ARB_vertex_array_bgra)
         */
        template<size_t i, class T> struct Attribute {
            static const size_t Location = i;   /**< Location to which the attribute is bound */
            typedef T Type;                     /**< %Attribute type */
        };

        /** @brief Default constructor */
        AbstractShaderProgram();

        /**
         * @brief Destructor
         *
         * Deletes the shader program.
         */
        ~AbstractShaderProgram();

        /**
         * @brief Use shader
         * @return False if the program wasn't successfully linked, true
         *      otherwise.
         */
        bool use();

    protected:
        /**
         * @brief Load shader
         * @return False if the shader wasn't successfully compiled, true
         *      otherwise.
         *
         * Compiles the shader, if it is not already, and prepares it for
         * linking.
         */
        bool attachShader(Shader& shader);

        /** @copydoc attachShader(Shader&) */
        inline bool attachShader(Shader&& shader) {
            return attachShader(shader);
        }

        /**
         * @brief Bind attribute to given location
         * @param location      Location
         * @param name          Attribute name
         *
         * Binds attribute to location which is be used later for binding
         * vertex buffers.
         * @note This function should be called between loadShader() calls
         * and link().
         */
        void bindAttribute(GLuint location, const std::string& name);

        /**
         * @brief Bind fragment data to given location
         * @param location      Location
         * @param name          Fragment output variable name
         *
         * @note This function should be called between loadShader() calls
         * and link().
         *
         * @requires_gl30 Extension <tt>EXT_gpu_shader4</tt>
         */
        void bindFragmentDataLocation(GLuint location, const std::string& name);

        /**
         * @brief Link the shader
         *
         * Binds previously specified attributes to given indexes and links the
         * shader program together.
         */
        void link();

        /**
         * @brief Get uniform location
         * @param name          Uniform name
         *
         * @note This function should be called after link().
         */
        GLint uniformLocation(const std::string& name);

        /**
         * @brief Set uniform value
         * @param location      Uniform location (see uniformLocation())
         * @param value         Value
         *
         * @attention This function doesn't check whether this shader is in use!
         */
        inline void setUniform(GLint location, GLint value) {
            glUniform1i(location, value);
        }

        /** @todo setUniform() for arbitrary vectors (size and also type) */

        /**
         * @copydoc setUniform(GLint, GLint)
         *
         * @requires_gl30 Extension <tt>EXT_gpu_shader4</tt>
         */
        void setUniform(GLint location, GLuint value) {
            glUniform1ui(location, value);
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, GLfloat value) {
            glUniform1f(location, value);
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, const Vector3& value) {
            glUniform3fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, const Vector4& value) {
            glUniform4fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, const Matrix3& value) {
            glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, const Matrix4& value) {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, const AbstractTexture* value) {
            setUniform(location, value->layer());
        }

        /** @copydoc setUniform(GLint, GLint) */
        void setUniform(GLint location, const BufferedTexture* value) {
            setUniform(location, value->layer());
        }

    private:
        enum State {
            Initialized,
            Linked,
            Failed
        };

        GLuint program;
        State state;
};

}

#endif
