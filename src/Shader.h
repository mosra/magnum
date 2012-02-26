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

#include "Magnum.h"

#include <vector>
#include <string>

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
    Shader(Shader&& other) = delete;
    Shader& operator=(const Shader& other) = delete;
    Shader& operator=(Shader&& other) = delete;

    public:
        /** @brief %Shader type */
        enum Type {
            Vertex = GL_VERTEX_SHADER,      /**< @brief Vertex shader */
            Geometry = GL_GEOMETRY_SHADER,  /**< @brief Geometry shader */
            Fragment = GL_FRAGMENT_SHADER   /**< @brief Fragment shader */
        };

        /** @brief %Shader state */
        enum State {
            Initialized,    /**< @brief %Shader is loaded */
            Compiled,       /**< @brief %Shader is compiled */
            Failed          /**< @brief Compilation failed */
        };

        /**
         * @brief Load shader from source
         * @param type      %Shader type
         * @param source    %Shader source
         * @return Pointer to compiled shader
         *
         * Loads the shader from one source. Shorthand for
         * @code
         * Shader* s = new Shader(type);
         * s->addData(data);
         * @endcode
         * Note that it is also possible to compile shader from more than one
         * source.
         */
        inline static Shader* fromData(Type type, const std::string& source) {
            Shader* s = new Shader(type);
            s->addSource(source);
            return s;
        }

        /**
         * @brief Load shader from file
         * @param type      %Shader type
         * @param filename  %Source filename
         * @return Pointer to compiled shader or zero, if file cannot be opened.
         *
         * Loads the shader from from one file. Shorthand for
         * @code
         * Shader* s = new Shader(type);
         * if(!s->addFile(filename)) delete s;
         * @endcode
         * Note that it is also possible to compile shader from more than one
         * source.
         */
        inline static Shader* fromFile(Type type, const char* filename) {
            Shader* s = new Shader(type);
            if(!s->addFile(filename)) {
                delete s;
                return nullptr;
            }

            return s;
        }

        /**
         * @brief Constructor
         *
         * Creates empty shader. Sources can be added with addData() or addFile().
         * @see fromData(), fromFile()
         */
        inline Shader(Type type): _type(type), _state(Initialized), shader(0) {}

        /**
         * @brief Destructor
         *
         * If the shader is compiled, deletes it.
         */
        inline ~Shader() { if(shader) glDeleteShader(shader); }

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
         * If the shader is not compiled already, adds given
         * source to source list. Note that it is possible to compile source
         * from more than one source.
         * @see addFile()
         */
        inline void addSource(const std::string& source) {
            if(_state == Initialized) sources.push_back(source);
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
