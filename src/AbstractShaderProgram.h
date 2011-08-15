#ifndef Magnum_AbstractShaderProgram_h
#define Magnum_AbstractShaderProgram_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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

namespace Magnum {

/**
@brief Base class for shaders

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - @b Constructor, which attaches particular shaders, links the program, binds
   attribute locations and gets uniform locations.
 - <strong>Attribute location</strong> enum with indexes where the particular
   attribute is bound, for example:
@code
enum Attribute {
    VertexCoordinates = 1,
    Color = 2,
    TextureCoordinates = 3,
    Normal = 4
};
@endcode
   See also bindAttribute().
 - <strong>Uniform binding functions</strong>, which set shader uniforms with
   setUniform() and setUniformArray() functions. Example:
@code
void setProjectionMatrixUniform(const Matrix4& matrix);
@endcode

Basic workflow with AbstractShaderProgram subclasses is: instancing the class
(once at the beginning), then in every frame calling use(), binding uniforms
and assigning vertex buffers to given attribute locations.
 */
class AbstractShaderProgram {
    DISABLE_COPY(AbstractShaderProgram)

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
