#ifndef Magnum_Buffer_h
#define Magnum_Buffer_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Buffer
 */

#include "Magnum.h"

namespace Magnum {

/**
 * @brief Class for managing buffers
 */
class Buffer {
    DISABLE_COPY(Buffer)

    public:
        /** @brief Buffer type */
        enum Type {
            /**
             * Used for storing vertex attributes.
             */
            ArrayBuffer = GL_ARRAY_BUFFER,

            /**
             * Used for storing vertex indices.
             */
            ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER
        };

        /** @brief Buffer usage */
        enum Usage {
            /**
             * Set once by the application and used infrequently for drawing.
             */
            DrawStream = GL_STREAM_DRAW,

            /**
             * Set once as output from an OpenGL command and used infequently
             * for drawing.
             */
            ReadStream = GL_STREAM_READ,

            /**
             * Set once as output from an OpenGL command and used infrequently
             * for drawing or copying to other buffers.
             */
            CopyStream = GL_STREAM_COPY,

            /**
             * Set once by the application and used frequently for drawing.
             */
            DrawStatic = GL_STATIC_DRAW,

            /**
             * Set once as output from an OpenGL command and queried many times
             * by the application.
             */
            ReadStatic = GL_STATIC_READ,

            /**
             * Set once as output from an OpenGL command and used frequently
             * for drawing or copying to other buffers.
             */
            CopyStatic = GL_STATIC_COPY,

            /**
             * Updated frequently by the application and used frequently
             * for drawing or copying to other images.
             */
            DrawDynamic = GL_DYNAMIC_DRAW,

            /**
             * Updated frequently as output from OpenGL command and queried
             * many times from the application.
             */
            ReadDynamic = GL_DYNAMIC_READ,

            /**
             * Updated frequently as output from OpenGL command and used
             * frequently for drawing or copying to other images.
             */
            CopyDynamic = GL_DYNAMIC_COPY
        };

        /**
         * @brief Constructor
         * @param type      Buffer type
         *
         * Generates new OpenGL buffer.
         */
        inline Buffer(Type type): _type(type) {
            glGenBuffers(1, &buffer);
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL buffer.
         */
        inline virtual ~Buffer() {
            glDeleteBuffers(1, &buffer);
        }

        /** @brief Buffer type */
        inline Type type() const { return _type; }

        /** @brief Bind buffer */
        inline void bind() const {
            glBindBuffer(_type, buffer);
        }

        /** @brief Unbind buffer */
        inline void unbind() const {
            glBindBuffer(_type, 0);
        }

        /**
         * @brief Set buffer data
         * @param size      Data size
         * @param data      Pointer to data
         * @param usage     Buffer usage
         */
        inline void setData(GLsizeiptr size, const GLvoid* data, Usage usage) {
            bind();
            glBufferData(_type, size, data, usage);
            unbind();
        }

    private:
        GLuint buffer;
        Type _type;
};

}

#endif
