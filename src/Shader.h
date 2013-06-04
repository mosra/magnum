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

See AbstractShaderProgram for more information.
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
        ~Shader();

        /** @brief Move constructor */
        Shader(Shader&& other);

        /** @brief Move assignment operator */
        Shader& operator=(Shader&& other);

        /** @brief OpenGL shader ID */
        GLuint id() const { return _id; }

        /**
         * @brief Add shader source
         * @param source    String with shader source
         * @return Reference to self (for method chaining)
         *
         * Adds given source to source list, preceeded with @c \#line directive
         * marking first line of the source as `n(1)` where n is number of
         * added source. If passed string is empty, the function does nothing.
         * @see addFile()
         */
        Shader& addSource(std::string source);

        /**
         * @brief Add source file
         * @param filename  Name of source file to read from
         * @return Reference to self (for method chaining)
         *
         * The file must exist and must be readable. Calls addSource() with
         * the contents.
         */
        Shader& addFile(const std::string& filename);

        /**
         * @brief Compile shader
         *
         * Returns `false` if compilation failed, `true` otherwise. Compiler
         * message (if any) is printed to error output.
         * @see @fn_gl{ShaderSource}, @fn_gl{CompileShader}, @fn_gl{GetShader}
         *      with @def_gl{COMPILE_STATUS} and @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl{GetShaderInfoLog}
         */
        bool compile();

    private:
        Type _type;
        GLuint _id;

        std::vector<std::string> sources;
};

}

#endif
