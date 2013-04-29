#ifndef Magnum_Shader_h
#define Magnum_Shader_h
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
 * @brief Class Magnum::Shader
 */

#include <vector>
#include <string>

#include "Magnum.h"
#include "Context.h"

#include "magnumVisibility.h"

namespace Magnum {

/**
@brief %Shader

Allows loading and compiling the shader from file or directly from source
string. See AbstractShaderProgram for more information.
 */
class MAGNUM_EXPORT Shader {
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    public:
        /** @brief %Shader type */
        enum class Type: GLenum {
            Vertex = GL_VERTEX_SHADER,      /**< Vertex shader */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Tessellation control shader
             * @requires_gl40 %Extension @extension{ARB,tessellation_shader}
             * @requires_gl Tessellation shaders are not available in OpenGL ES.
             */
            TessellationControl = GL_TESS_CONTROL_SHADER,

            /**
             * Tessellation evaluation shader
             * @requires_gl40 %Extension @extension{ARB,tessellation_shader}
             * @requires_gl Tessellation shaders are not available in OpenGL ES.
             */
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER,

            /**
             * Geometry shader
             * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
             * @requires_gl Geometry shaders are not available in OpenGL ES.
             */
            Geometry = GL_GEOMETRY_SHADER,

            /**
             * Compute shader
             * @requires_gl43 %Extension @extension{ARB,compute_shader}
             * @requires_gl Compute shaders are not available in OpenGL ES.
             */
            Compute = GL_COMPUTE_SHADER,
            #endif

            Fragment = GL_FRAGMENT_SHADER   /**< Fragment shader */
        };

        /** @brief %Shader state */
        enum class State {
            Initialized,    /**< %Shader is loaded */
            Compiled,       /**< %Shader is compiled */
            Failed          /**< Compilation failed */
        };

        /**
         * @brief Constructor
         * @param version   Target version
         * @param type      %Shader type
         *
         * Creates empty OpenGL shader and adds @c \#version directive at the
         * beginning. Sources can be added with addSource() or addFile().
         * @see @fn_gl{CreateShader}
         */
        explicit Shader(Version version, Type type);

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader.
         * @see @fn_gl{DeleteShader}
         */
        inline ~Shader() { if(shader) glDeleteShader(shader); }

        /** @brief Move constructor */
        Shader(Shader&& other);

        /** @brief Move assignment operator */
        Shader& operator=(Shader&& other);

        /**
         * @brief %Shader type
         *
         * Set in constructor.
         */
        inline Type type() const { return _type; }

        /**
         * @brief Compilation state
         *
         * Changes after calling compile().
         */
        inline State state() const { return _state; }

        /**
         * @brief Add shader source
         * @param source    String with shader source
         * @return Reference to self (for method chaining)
         *
         * If the shader is not compiled already, adds given source to source
         * list. Note that it is possible to compile shader from more than
         * one source.
         * @see addFile()
         */
        Shader& addSource(const std::string& source);

        /**
         * @brief Add source file
         * @param filename  Name of source file to read from
         * @return Reference to self (for method chaining)
         *
         * The file must exist and must be readable.
         * @see addSource()
         */
        Shader& addFile(const std::string& filename);

        /**
         * @brief Compile shader
         * @return Compiled shader or 0 if compilation failed.
         *
         * If the shader has any sources present and hasn't been compiled
         * before, it tries to compile it. If compilation fails or no sources
         * are present, returns 0. If the shader was compiled already, returns
         * already existing shader.
         * @see state(), @fn_gl{ShaderSource}, @fn_gl{CompileShader},
         *      @fn_gl{GetShader} with @def_gl{COMPILE_STATUS},
         *      @fn_gl{GetShaderInfoLog}
         */
        GLuint compile();

    private:
        Type _type;
        State _state;

        std::vector<std::string> sources;

        GLuint shader;
};

}

#endif
