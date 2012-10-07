#ifndef Magnum_Buffer_h
#define Magnum_Buffer_h
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
 * @brief Class Magnum::Buffer
 */

#include <cstddef>
#include <array>
#include <vector>

#include "Magnum.h"

namespace Magnum {

/**
@brief %Buffer

Encapsulates one OpenGL buffer object and provides functions for convenient
data updates.

@section Buffer-data Data updating
Default way to set or update buffer data with setData() or setSubData() is to
explicitly specify data size and pass the pointer to it:
@code
Vector3* data = new Vector3[200];
buffer.setData(200*sizeof(Vector3), data, Buffer::Usage::StaticDraw);
@endcode
Howewer, in some cases, you have the data in fixed-size array with size known
at compile time. There is an convenient overload which detects the size of
passed array, so you don't have to repeat it:
@code
Vector3 data[] = {
    // ...
};
buffer.setData(data, Buffer::Usage::StaticDraw);
@endcode
There is also overload for array-like containers from STL, such as `std::vector` or
`std::array`:
@code
std::vector<Vector3> data;
buffer.setData(data, Buffer::Usage::StaticDraw);
@endcode

@todo Support for buffer copying (OpenGL 3.1, @extension{ARB,copy_buffer})
@todo Support for AMD's query buffer (@extension{AMD,query_buffer_object})
 */
class Buffer {
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) = delete;

    public:
        /**
         * @brief %Buffer target
         *
         * @see bind(Target), unbind(Target),
         *      setData(Target, GLsizeiptr, const GLvoid*, Usage),
         *      setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*)
         */
        enum class Target: GLenum {
            /** Used for storing vertex attributes. */
            Array = GL_ARRAY_BUFFER,

            /**
             * Source for copies.
             * @requires_gl31 Extension @extension{ARB,copy_buffer}
             * @requires_gles30 (no extension providing this functionality)
             */
            CopyRead = GL_COPY_READ_BUFFER,

            /**
             * Target for copies.
             * @requires_gl31 Extension @extension{ARB,copy_buffer}
             * @requires_gles30 (no extension providing this functionality)
             */
            CopyWrite = GL_COPY_WRITE_BUFFER,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Used for supplying arguments for instanced drawing.
             * @requires_gl
             * @requires_gl40 Extension @extension{ARB,draw_indirect}
             */
            DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
            #endif

            /** Used for storing vertex indices. */
            ElementArray = GL_ELEMENT_ARRAY_BUFFER,

            /**
             * Target for pixel pack operations.
             * @requires_gles30 (no extension providing this functionality)
             */
            PixelPack = GL_PIXEL_PACK_BUFFER,

            /**
             * Source for texture update operations.
             * @requires_gles30 (no extension providing this functionality)
             */
            PixelUnpack = GL_PIXEL_UNPACK_BUFFER,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Source for texel fetches. See BufferedTexture.
             * @requires_gl
             * @requires_gl31 Extension @extension{ARB,texture_buffer_object}
             */
            Texture = GL_TEXTURE_BUFFER,
            #endif

            /**
             * Target for transform feedback.
             * @requires_gl30 Extension @extension{EXT,transform_feedback}
             * @requires_gles30 (no extension providing this functionality)
             */
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,

            /**
             * Used for storing uniforms.
             * @requires_gl31 Extension @extension{ARB,uniform_buffer_object}
             * @requires_gles30 (no extension providing this functionality)
             */
            Uniform = GL_UNIFORM_BUFFER
        };

        /**
         * @brief %Buffer usage
         *
         * @see setData(Target, GLsizeiptr, const GLvoid*, Usage)
         */
        enum class Usage: GLenum {
            /**
             * Set once by the application and used infrequently for drawing.
             */
            StreamDraw = GL_STREAM_DRAW,

            /**
             * Set once as output from an OpenGL command and used infequently
             * for drawing.
             * @requires_gles30 (no extension providing this functionality)
             */
            StreamRead = GL_STREAM_READ,

            /**
             * Set once as output from an OpenGL command and used infrequently
             * for drawing or copying to other buffers.
             * @requires_gles30 (no extension providing this functionality)
             */
            StreamCopy = GL_STREAM_COPY,

            /**
             * Set once by the application and used frequently for drawing.
             */
            StaticDraw = GL_STATIC_DRAW,

            /**
             * Set once as output from an OpenGL command and queried many
             * times by the application.
             * @requires_gles30 (no extension providing this functionality)
             */
            StaticRead = GL_STATIC_READ,

            /**
             * Set once as output from an OpenGL command and used frequently
             * for drawing or copying to other buffers.
             * @requires_gles30 (no extension providing this functionality)
             */
            StaticCopy = GL_STATIC_COPY,

            /**
             * Updated frequently by the application and used frequently
             * for drawing or copying to other images.
             */
            DynamicDraw = GL_DYNAMIC_DRAW,

            /**
             * Updated frequently as output from OpenGL command and queried
             * many times from the application.
             * @requires_gles30 (no extension providing this functionality)
             */
            DynamicRead = GL_DYNAMIC_READ,

            /**
             * Updated frequently as output from OpenGL command and used
             * frequently for drawing or copying to other images.
             * @requires_gles30 (no extension providing this functionality)
             */
            DynamicCopy = GL_DYNAMIC_COPY
        };

        /**
         * @brief Unbind any buffer from given target
         * @param target    %Target
         *
         * @see @fn_gl{BindBuffer}
         */
        inline static void unbind(Target target) {
            glBindBuffer(static_cast<GLenum>(target), 0);
        }

        /**
         * @brief Constructor
         * @param defaultTarget Default target (used when calling bind()
         *      without parameter)
         *
         * Generates new OpenGL buffer.
         * @see @fn_gl{GenBuffers}
         */
        inline Buffer(Target defaultTarget): _defaultTarget(defaultTarget) {
            glGenBuffers(1, &buffer);
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL buffer.
         * @see @fn_gl{DeleteBuffers}
         */
        inline virtual ~Buffer() {
            glDeleteBuffers(1, &buffer);
        }

        /** @brief Default bind type */
        inline Target defaultTarget() const { return _defaultTarget; }

        /** @brief OpenGL internal buffer ID */
        inline GLuint id() const { return buffer; }

        /**
         * @brief Bind buffer
         *
         * Binds buffer with default target.
         * @see bind(Target)
         */
        inline void bind() { bind(_defaultTarget); }

        /**
         * @brief Bind buffer
         * @param target    %Target
         *
         * @see @fn_gl{BindBuffer}
         */
        inline void bind(Target target) {
            glBindBuffer(static_cast<GLenum>(target), buffer);
        }

        /**
         * @brief Set buffer data
         * @param size      Data size
         * @param data      Pointer to data
         * @param usage     %Buffer usage
         *
         * Sets buffer data with default target.
         * @see setData(Target, GLsizeiptr, const GLvoid*, Usage)
         */
        inline void setData(GLsizeiptr size, const GLvoid* data, Usage usage) {
            setData(_defaultTarget, size, data, usage);
        }

        /**
         * @brief Set buffer data
         * @param data      Fixed-size array with data
         * @param usage     %Buffer usage
         *
         * Sets buffer data with default target. More convenient for setting
         * data from fixed-size arrays than
         * setData(GLsizeiptr, const GLvoid*, Usage).
         * @see setData(Target, GLsizeiptr, const GLvoid*, Usage)
         */
        template<std::size_t size, class T> inline void setData(const T(&data)[size], Usage usage) {
            setData(_defaultTarget, data, usage);
        }

        /**
         * @brief Set buffer data
         * @param data      Vector with data
         * @param usage     %Buffer usage
         *
         * Sets buffer data with default target.
         * @see setData(Target, GLsizeiptr, const GLvoid*, Usage)
         */
        template<class T> inline void setData(const std::vector<T>& data, Usage usage) {
            setData(_defaultTarget, data, usage);
        }

        /** @overload */
        template<std::size_t size, class T> inline void setData(const std::array<T, size>& data, Usage usage) {
            setData(_defaultTarget, data, usage);
        }

        /**
         * @brief Set buffer data
         * @param target    %Target
         * @param size      Data size
         * @param data      Pointer to data
         * @param usage     %Buffer usage
         *
         * @see bind(Target), @fn_gl{BufferData}
         */
        inline void setData(Target target, GLsizeiptr size, const GLvoid* data, Usage usage) {
            bind(target);
            glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
        }

        /**
         * @brief Set buffer data
         * @param target    %Target
         * @param data      Fixed-size array with data
         * @param usage     %Buffer usage
         *
         * More convenient for setting data from fixed-size arrays than
         * setData(Target, GLsizeiptr, const GLvoid*, Usage).
         *
         * @see setData(Target, GLsizeiptr, const GLvoid*, Usage)
         */
        template<std::size_t size, class T> inline void setData(Target target, const T(&data)[size], Usage usage) {
            setData(target, size*sizeof(T), data, usage);
        }

        /**
         * @brief Set buffer data
         * @param target    %Target
         * @param data      Vector with data
         * @param usage     %Buffer usage
         *
         * @see setData(Target, GLsizeiptr, const GLvoid*, Usage)
         */
        template<class T> inline void setData(Target target, const std::vector<T>& data, Usage usage) {
            setData(target, data.size()*sizeof(T), data.data(), usage);
        }

        /** @overload */
        template<std::size_t size, class T> inline void setData(Target target, const std::array<T, size>& data, Usage usage) {
            setData(target, data.size()*sizeof(T), data.data(), usage);
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param size      Data size
         * @param data      Pointer to data
         *
         * Sets buffer subdata with default target.
         * @see setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*)
         */
        inline void setSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
            setSubData(_defaultTarget, offset, size, data);
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param data      Fixed-size array with data
         *
         * Sets buffer subdata with default target. More convenient for
         * setting data from fixed-size arrays than
         * setSubData(GLintptr, GLsizeiptr, const GLvoid*).
         * @see setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<std::size_t size, class T> inline void setSubData(GLintptr offset, const T(&data)[size]) {
            setSubData(_defaultTarget, offset, data);
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param data      Vector with data
         *
         * Sets buffer subdata with default target.
         * @see setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<class T> inline void setSubData(GLintptr offset, const std::vector<T>& data) {
            setSubData(_defaultTarget, offset, data);
        }

        /** @overload */
        template<std::size_t size, class T> inline void setSubData(GLintptr offset, const std::array<T, size>& data) {
            setSubData(_defaultTarget, offset, data);
        }

        /**
         * @brief Set buffer subdata
         * @param target    %Target
         * @param offset    Offset in the buffer
         * @param size      Data size
         * @param data      Pointer to data
         *
         * @see bind(Target), @fn_gl{BufferSubData}
         */
        inline void setSubData(Target target, GLintptr offset, GLsizeiptr size, const GLvoid* data) {
            bind(target);
            glBufferSubData(static_cast<GLenum>(target), offset, size, data);
        }

        /**
         * @brief Set buffer subdata
         * @param target    %Target
         * @param offset    Offset in the buffer
         * @param data      Fixed-size array with data
         *
         * More convenient for setting data from fixed-size arrays than
         * setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*).
         * @see setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<std::size_t size, class T> inline void setSubData(Target target, GLintptr offset, const T(&data)[size]) {
            setSubData(target, offset, size*sizeof(T), data);
        }

        /**
         * @brief Set buffer subdata
         * @param target    %Target
         * @param offset    Offset in the buffer
         * @param data      Vector with data
         *
         * @see setSubData(Target, GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<class T> inline void setSubData(Target target, GLintptr offset, const std::vector<T>& data) {
            setSubData(target, offset, data.size()*sizeof(T), data.data());
        }

        /** @overload */
        template<std::size_t size, class T> inline void setSubData(Target target, GLintptr offset, const std::array<T, size>& data) {
            setSubData(target, offset, data.size()*sizeof(T), data.data());
        }

    private:
        GLuint buffer;
        Target _defaultTarget;
};

}

#endif
