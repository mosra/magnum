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

#include "magnumVisibility.h"

namespace Magnum {

/**
@brief %Buffer

Encapsulates one OpenGL buffer object and provides functions for convenient
data updates.

@section Buffer-data Data updating

Default way to set or update buffer data with setData() or setSubData() is to
explicitly specify data size and pass the pointer to it:
@code
Buffer buffer;
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

@section Buffer-performance-optimization Performance optimizations

The engine tracks currently bound buffers to avoid unnecessary calls to
@fn_gl{BindBuffer}. If the buffer is already bound to some target,
functions copy(), setData() and setSubData() use that target in
@fn_gl{CopyBufferSubData}, @fn_gl{BufferData} and @fn_gl{BufferSubData}
functions instead of binding the buffer to some specific target. You can also
use setTargetHint() to possibly reduce unnecessary rebinding.

If extension @extension{EXT,direct_state_access} is available, functions
copy(), setData() and setSubData() use DSA functions to avoid unnecessary
calls to @fn_gl{BindBuffer}. See their respective documentation for more
information.

@todo Support for AMD's query buffer (@extension{AMD,query_buffer_object})
@todo BindBufferRange/BindBufferOffset/BindBufferBase for transform feedback (3.0, @extension{EXT,transform_feedback})
 */
class MAGNUM_EXPORT Buffer {
    friend class Context;

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) = delete;

    public:
        /**
         * @brief %Buffer target
         *
         * @see bind(Target), unbind(Target)
         */
        enum class Target: GLenum {
            /** Used for storing vertex attributes. */
            Array = GL_ARRAY_BUFFER,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Used for storing atomic counters.
             * @requires_gl42 %Extension @extension{ARB,shader_atomic_counters}
             * @requires_gl Atomic counters are not available in OpenGL ES.
             */
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Source for copies. See copy().
             * @requires_gl31 %Extension @extension{ARB,copy_buffer}
             * @requires_gles30 Buffer copying is not available in OpenGL ES
             *      2.0.
             */
            CopyRead = GL_COPY_READ_BUFFER,

            /**
             * Target for copies. See copy().
             * @requires_gl31 %Extension @extension{ARB,copy_buffer}
             * @requires_gles30 Buffer copying is not available in OpenGL ES
             *      2.0.
             */
            CopyWrite = GL_COPY_WRITE_BUFFER,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Indirect compute dispatch commands.
             * @requires_gl43 %Extension @extension{ARB,compute_shader}
             * @requires_gl Compute shaders are not available in OpenGL ES.
             */
            DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,

            /**
             * Used for supplying arguments for indirect drawing.
             * @requires_gl40 %Extension @extension{ARB,draw_indirect}
             * @requires_gl Indirect drawing not available in OpenGL ES.
             */
            DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
            #endif

            /** Used for storing vertex indices. */
            ElementArray = GL_ELEMENT_ARRAY_BUFFER

            #ifndef MAGNUM_TARGET_GLES2
            ,

            /**
             * Target for pixel pack operations.
             * @requires_gles30 Pixel buffer objects are not available in
             *      OpenGL ES 2.0.
             */
            PixelPack = GL_PIXEL_PACK_BUFFER,

            /**
             * Source for texture update operations.
             * @requires_gles30 Pixel buffer objects are not available in
             *      OpenGL ES 2.0.
             */
            PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Used for shader storage.
             * @requires_gl43 %Extension @extension{ARB,shader_storage_buffer_object}
             * @requires_gl Shader storage is not available in OpenGL ES.
             */
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,

            /**
             * Source for texel fetches. See BufferTexture.
             * @requires_gl31 %Extension @extension{ARB,texture_buffer_object}
             * @requires_gl Texture buffers are not available in OpenGL ES.
             */
            Texture = GL_TEXTURE_BUFFER,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Target for transform feedback.
             * @requires_gl30 %Extension @extension{EXT,transform_feedback}
             * @requires_gles30 Transform feedback is not available in OpenGL
             *      ES 2.0.
             */
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,

            /**
             * Used for storing uniforms.
             * @requires_gl31 %Extension @extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             */
            Uniform = GL_UNIFORM_BUFFER
            #endif
        };

        /**
         * @brief %Buffer usage
         *
         * @see setData(GLsizeiptr, const GLvoid*, Usage)
         */
        enum class Usage: GLenum {
            /**
             * Set once by the application and used infrequently for drawing.
             */
            StreamDraw = GL_STREAM_DRAW,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Set once as output from an OpenGL command and used infequently
             * for drawing.
             * @requires_gles30 Only @ref Magnum::Buffer::Usage "Usage::StreamDraw"
             *      is available in OpenGL ES 2.0.
             */
            StreamRead = GL_STREAM_READ,

            /**
             * Set once as output from an OpenGL command and used infrequently
             * for drawing or copying to other buffers.
             * @requires_gles30 Only @ref Magnum::Buffer::Usage "Usage::StreamDraw"
             *      is available in OpenGL ES 2.0.
             */
            StreamCopy = GL_STREAM_COPY,
            #endif

            /**
             * Set once by the application and used frequently for drawing.
             */
            StaticDraw = GL_STATIC_DRAW,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Set once as output from an OpenGL command and queried many
             * times by the application.
             * @requires_gles30 Only @ref Magnum::Buffer::Usage "Usage::StaticDraw"
             *      is available in OpenGL ES 2.0.
             */
            StaticRead = GL_STATIC_READ,

            /**
             * Set once as output from an OpenGL command and used frequently
             * for drawing or copying to other buffers.
             * @requires_gles30 Only @ref Magnum::Buffer::Usage "Usage::StaticDraw"
             *      is available in OpenGL ES 2.0.
             */
            StaticCopy = GL_STATIC_COPY,
            #endif

            /**
             * Updated frequently by the application and used frequently
             * for drawing or copying to other images.
             */
            DynamicDraw = GL_DYNAMIC_DRAW

            #ifndef MAGNUM_TARGET_GLES2
            ,

            /**
             * Updated frequently as output from OpenGL command and queried
             * many times from the application.
             * @requires_gles30 Only @ref Magnum::Buffer::Usage "Usage::DynamicDraw"
             *      is available in OpenGL ES 2.0.
             */
            DynamicRead = GL_DYNAMIC_READ,

            /**
             * Updated frequently as output from OpenGL command and used
             * frequently for drawing or copying to other images.
             * @requires_gles30 Only @ref Magnum::Buffer::Usage "Usage::DynamicDraw"
             *      is available in OpenGL ES 2.0.
             */
            DynamicCopy = GL_DYNAMIC_COPY
            #endif
        };

        /**
         * @brief Unbind any buffer from given target
         * @param target    %Target
         *
         * @see @fn_gl{BindBuffer}
         */
        inline static void unbind(Target target) { bind(target, 0); }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Copy one buffer to another
         * @param read          %Buffer from which to read
         * @param write         %Buffer to which to copy
         * @param readOffset    Offset in the read buffer
         * @param writeOffset   Offset in the write buffer
         * @param size          Data size
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffers aren't already bound somewhere, they are bound to
         * `Target::CopyRead` and `Target::CopyWrite` before the copy is
         * performed.
         * @see @fn_gl{BindBuffer} and @fn_gl{CopyBufferSubData} or
         *      @fn_gl_extension{NamedCopyBufferSubData,EXT,direct_state_access}
         * @requires_gl31 %Extension @extension{ARB,copy_buffer}
         * @requires_gles30 %Buffer copying is not available in OpenGL ES 2.0.
         */
        inline static void copy(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
            copyImplementation(read, write, readOffset, writeOffset, size);
        }
        #endif

        /**
         * @brief Constructor
         * @param targetHint    Target hint, see setTargetHint() for more
         *      information
         *
         * Generates new OpenGL buffer.
         * @see @fn_gl{GenBuffers}
         */
        inline explicit Buffer(Target targetHint = Target::Array): _targetHint(targetHint) {
            glGenBuffers(1, &_id);
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL buffer.
         * @see @fn_gl{DeleteBuffers}
         */
        virtual ~Buffer();

        /** @brief OpenGL buffer ID */
        inline GLuint id() const { return _id; }

        /** @brief Target hint */
        inline Target targetHint() const { return _targetHint; }

        /**
         * @brief Set target hint
         *
         * If @extension{EXT,direct_state_access} is not available, the buffer
         * must be internally bound to some target before any operation. You
         * can specify target which will always be used when binding the
         * buffer internally, possibly saving some calls to @fn_gl{BindBuffer}.
         *
         * Default target hint is `Target::Array`.
         * @see setData(), setSubData()
         * @todo Target::ElementArray cannot be used when no VAO is bound -
         *      http://www.opengl.org/wiki/Vertex_Specification#Index_buffers
         *      ... damned GL state
         */
        inline void setTargetHint(Target hint) { _targetHint = hint; }

        /**
         * @brief Bind buffer
         * @param target    %Target
         *
         * @todo Allow binding to Target::ElementArray only if VAO is bound
         *      to avoid potential issues?
         * @bug Binding to ElementArray if any VAO is active will corrupt the mesh
         * @todo Don't allow user to bind buffers?
         * @see @fn_gl{BindBuffer}
         */
        inline void bind(Target target) { bind(target, _id); }

        /**
         * @brief Set buffer data
         * @param size      Data size
         * @param data      Pointer to data
         * @param usage     %Buffer usage
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see setTargetHint(), @fn_gl{BindBuffer} and @fn_gl{BufferData} or
         *      @fn_gl_extension{NamedBufferData,EXT,direct_state_access}
         */
        inline void setData(GLsizeiptr size, const GLvoid* data, Usage usage) {
            (this->*setDataImplementation)(size, data, usage);
        }

        /**
         * @brief Set buffer data
         * @param data      Fixed-size array with data
         * @param usage     %Buffer usage
         *
         * @see setData(GLsizeiptr, const GLvoid*, Usage).
         */
        template<std::size_t size, class T> inline void setData(const T(&data)[size], Usage usage) {
            setData(size*sizeof(T), data, usage);
        }

        /**
         * @brief Set buffer data
         * @param data      Vector with data
         * @param usage     %Buffer usage
         *
         * @see setData(GLsizeiptr, const GLvoid*, Usage)
         */
        template<class T> inline void setData(const std::vector<T>& data, Usage usage) {
            setData(data.size()*sizeof(T), data.data(), usage);
        }

        /** @overload */
        template<std::size_t size, class T> inline void setData(const std::array<T, size>& data, Usage usage) {
            setData(data.size()*sizeof(T), data.data(), usage);
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param size      Data size
         * @param data      Pointer to data
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see setTargetHint(), @fn_gl{BindBuffer} and @fn_gl{BufferSubData}
         *      or @fn_gl_extension{NamedBufferSubData,EXT,direct_state_access}
         */
        inline void setSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
            (this->*setSubDataImplementation)(offset, size, data);
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param data      Fixed-size array with data
         *
         * @see setSubData(GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<std::size_t size, class T> inline void setSubData(GLintptr offset, const T(&data)[size]) {
            setSubData(offset, size*sizeof(T), data);
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param data      Vector with data
         *
         * @see setSubData(GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<class T> inline void setSubData(GLintptr offset, const std::vector<T>& data) {
            setSubData(offset, data.size()*sizeof(T), data.data());
        }

        /** @overload */
        template<std::size_t size, class T> inline void setSubData(GLintptr offset, const std::array<T, size>& data) {
            setSubData(offset, data.size()*sizeof(T), data.data());
        }

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        static void bind(Target hint, GLuint id);
        Target MAGNUM_LOCAL bindInternal(Target hint);

        #ifndef MAGNUM_TARGET_GLES2
        typedef void(*CopyImplementation)(Buffer*, Buffer*, GLintptr, GLintptr, GLsizeiptr);
        static void MAGNUM_LOCAL copyImplementationDefault(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL copyImplementationDSA(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #endif
        static CopyImplementation copyImplementation;
        #endif

        typedef void(Buffer::*SetDataImplementation)(GLsizeiptr, const GLvoid*, Usage);
        void MAGNUM_LOCAL setDataImplementationDefault(GLsizeiptr size, const GLvoid* data, Usage usage);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL setDataImplementationDSA(GLsizeiptr size, const GLvoid* data, Usage usage);
        #endif
        static SetDataImplementation setDataImplementation;

        typedef void(Buffer::*SetSubDataImplementation)(GLintptr, GLsizeiptr, const GLvoid*);
        void MAGNUM_LOCAL setSubDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL setSubDataImplementationDSA(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        #endif
        static SetSubDataImplementation setSubDataImplementation;

        GLuint _id;
        Target _targetHint;
};

}

#endif
