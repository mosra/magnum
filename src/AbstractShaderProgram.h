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
#include "Texture.h"

namespace Magnum {

/**
@brief Base class for shaders

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - <strong>Attribute location</strong> enum with indexes where the particular
   attribute is bound, for example:
@code
enum Attribute {
    Vertex = 1,
    Normal = 2,
    TextureCoords = 3,
};
@endcode
   See also bindAttribute().
 - @b Constructor, which attaches particular shaders, links the program, binds
   attribute locations and gets uniform locations, for example:
@code
    // Load shaders from file and attach them to the program
    Shader* vertexShader = Shader::fromFile(Shader::Vertex, "PhongShader.vert");
    Shader* fragmentShader = Shader::fromFile(Shader::Fragment, "PhongShader.frag");
    attachShader(vertexShader);
    attachShader(fragmentShader);

    // Bind attribute names to IDs
    bindAttribute(Vertex, "vertex");
    bindAttribute(Normal, "normal");
    bindAttribute(TextureCoords, "textureCoords");

    // Link, then delete now uneeded shaders
    link();
    delete vertexShader;
    delete fragmentShader;

    // Get locations of uniforms
    transformationMatrixUniform = uniformLocation("transformationMatrix");
    projectionMatrixUniform = uniformLocation("projectionMatrix");
    // more uniforms like light location, colors etc.
@endcode
 - <strong>Uniform binding functions</strong>, which set shader uniforms with
   setUniform() and setUniformArray() functions. Example:
@code
void setTransformationMatrixUniform(const Matrix4& matrix) {
    setUniform(transformationMatrixUniform, matrix);
}
@endcode

Basic workflow with AbstractShaderProgram subclasses is: instancing the class
(once at the beginning), then in every frame calling use(), setting uniforms
and calling Mesh::draw() (see its documentation for more).
 */
class MAGNUM_EXPORT AbstractShaderProgram {
    AbstractShaderProgram(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram(AbstractShaderProgram&& other) = delete;
    AbstractShaderProgram& operator=(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram& operator=(AbstractShaderProgram&& other) = delete;

    public:
        /** @brief Logging level */
        enum LogLevel {
            None,       /**< @brief Don't display anything */
            Errors,     /**< @brief Display only errors */
            Warnings    /**< @brief Display only errors and warnings */
        };

        /**
         * @brief Log level
         *
         * Log level for displaying compilation messages. Default is Errors.
         */
        inline static LogLevel logLevel() { return _logLevel; }

        /** @brief Set log level */
        inline static void setLogLevel(LogLevel level) { _logLevel = level; }

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
         * @note The shader should be deleted by caller after linking.
         */
        bool attachShader(Shader* shader);

        /**
         * @brief Bind attribute to given location
         * @param location      Location
         * @param name          Attribute name
         * @return False if the location or name is already bound, true
         * otherwise.
         *
         * Binds attribute to the location which can be used later when binding
         * vertex buffers.
         * @note This function should be called between loadShader() calls
         * and link().
         */
        bool bindAttribute(GLuint location, const std::string& name);

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

        /** @copydoc setUniform(GLint, GLint) */
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

    private:
        enum State {
            Initialized,
            Linked,
            Failed
        };

        static LogLevel _logLevel;

        GLuint program;
        State state;
        std::map<GLuint, std::string> attributes;
};

}

#endif
