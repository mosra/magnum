#ifndef Magnum_Shader_h
#define Magnum_Shader_h
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
 * @brief Class Magnum::Shader
 */

#include <vector>
#include <string>

#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

/**
 * @brief %Shader
 *
 * Allows loading and compiling the shader from file or directly from source
 * string. Compiled shaders are then passed to AbstractShaderProgram subclasses
 * for linking and usage.
 */
class MAGNUM_EXPORT Shader {
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    public:
        /** @brief %Shader type */
        enum class Type: GLenum {
            Vertex = GL_VERTEX_SHADER,      /**< Vertex shader */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Tessellation control shader
             * @requires_gl
             * @requires_gl40 Extension @extension{ARB,tessellation_shader}
             */
            TessellationControl = GL_TESS_CONTROL_SHADER,

            /**
             * Tessellation evaluation shader
             * @requires_gl
             * @requires_gl40 Extension @extension{ARB,tessellation_shader}
             */
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER,

            /**
             * Geometry shader
             * @requires_gl
             * @requires_gl32 Extension @extension{ARB,geometry_shader4}
             */
            Geometry = GL_GEOMETRY_SHADER,
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
         * @brief Load shader from source
         * @param type      %Shader type
         * @param source    %Shader source
         * @return Shader instance
         *
         * Loads the shader from one source. Shorthand for
         * @code
         * Shader s(type);
         * s.addData(data);
         * @endcode
         * Note that it is also possible to create shader from more than one
         * source.
         */
        inline static Shader fromData(Type type, const std::string& source) {
            Shader s(type);
            s.addSource(source);
            return s;
        }

        /**
         * @brief Load shader from file
         * @param type      %Shader type
         * @param filename  %Source filename
         * @return Shader instance
         *
         * Loads the shader from from one file. Shorthand for
         * @code
         * Shader s(type);
         * s.addFile(filename);
         * @endcode
         * Note that it is also possible to create shader from more than one
         * source.
         */
        inline static Shader fromFile(Type type, const char* filename) {
            Shader s(type);
            s.addFile(filename);
            return s;
        }

        /**
         * @brief Constructor
         *
         * Creates empty OpenGL shader. Sources can be added with addSource()
         * or addFile().
         * @see fromData(), fromFile()
         */
        inline Shader(Type type): _type(type), _state(State::Initialized), shader(0) {
            shader = glCreateShader(static_cast<GLenum>(_type));
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader.
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
         *
         * If the shader is not compiled already, adds given source to source
         * list. Note that it is possible to compile shader from more than
         * one source.
         * @see addFile()
         */
        inline void addSource(const std::string& source) {
            if(_state == State::Initialized) sources.push_back(source);
        }

        /**
         * @brief Add source file
         * @param filename  Name of source file to read from
         * @return False if reading the file fails, true otherwise.
         *
         * @see addSource()
         */
        bool addFile(const std::string& filename);

        /**
         * @brief Compile shader
         * @return Compiled shader or 0 if compilation failed.
         *
         * If the shader has any sources present and hasn't been compiled
         * before, it tries to compile it. If compilation fails or no sources
         * are present, returns 0. If the shader was compiled already, returns
         * already existing shader.
         * @see state()
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
