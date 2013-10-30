#ifndef Magnum_Buffer_h
#define Magnum_Buffer_h
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

/** @file /Buffer.h
 * @brief Class Magnum::Buffer
 */

#include <cstddef>
#include <array>
#include <vector>
#include <Containers/Array.h>
#include <Containers/EnumSet.h>
#include <Utility/Assert.h>

#include "Magnum.h"
#include "OpenGL.h"
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

@subsection Buffer-data-mapping Memory mapping

%Buffer data can be also updated asynchronously. First you need to allocate
the buffer to desired size by passing `nullptr` to setData(), e.g.:
@code
buffer.setData(200*sizeof(Vector3)), nullptr, Buffer::Usage::StaticDraw);
@endcode
Then you can map the buffer to client memory and operate with the memory
directly. After you are done with the operation, call unmap() to unmap the
buffer again.
@code
Vector3* data = static_cast<Vector3*>(buffer.map(0, 200*sizeof(Vector3), Buffer::MapFlag::Write|Buffer::MapFlag::InvalidateBuffer));
for(std::size_t i = 0; i != 200; ++i)
    data[i] = ...;
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
@endcode
If you are updating only a few discrete portions of the buffer, you can use
@ref MapFlag::FlushExplicit and flushMappedRange() to reduce number of memory
operations performed by OpenGL on unmapping. Example:
@code
Vector3* data = static_cast<Vector3*>(buffer.map(0, 200*sizeof(Vector3), Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit));
for(std::size_t i: {7, 27, 56, 128}) {
    data[i] = ...;
    buffer.flushMappedRange(i*sizeof(Vector3), sizeof(Vector3));
}
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
@endcode

@section Buffer-performance-optimization Performance optimizations

The engine tracks currently bound buffers to avoid unnecessary calls to
@fn_gl{BindBuffer}. If the buffer is already bound to some target, functions
copy(), setData(), setSubData(), map(), flushMappedRange() and unmap() use
that target instead of binding the buffer to some specific target. You can
also use setTargetHint() to possibly reduce unnecessary rebinding. %Buffer
limits and implementation-defined values (such as @ref maxVertexAttributeBindings())
are cached, so repeated queries don't result in repeated @fn_gl{Get} calls.

If extension @extension{EXT,direct_state_access} is available, functions
copy(), setData(), setSubData(), map(), flushMappedRange() and unmap() use DSA
functions to avoid unnecessary calls to @fn_gl{BindBuffer}. See their
respective documentation for more information.

You can use functions invalidateData() and invalidateSubData() if you don't
need buffer data anymore to avoid unnecessary memory operations performed by
OpenGL in order to preserve the data. If running on OpenGL ES or extension
@extension{ARB,invalidate_subdata} is not available, these functions do
nothing.

@todo Support for AMD/ARB's query buffer (@extension{AMD,query_buffer_object}, @extension{ARB,query_buffer_object})
@todo BindBufferRange/BindBufferOffset/BindBufferBase for transform feedback (3.0, @extension{EXT,transform_feedback})
 */
class MAGNUM_EXPORT Buffer {
    friend class Context;

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

        #ifndef MAGNUM_TARGET_GLES3
        /**
         * @brief Memory mapping access
         *
         * @deprecated_gl Prefer to use @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      instead, as it has more complete set of features.
         * @see map(MapAccess), mapSub()
         * @requires_es_extension %Extension @es_extension{OES,mapbuffer} or
         *      @es_extension{CHROMIUM,map_sub} in OpenGL ES 2.0, use
         *      @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      in OpenGL ES 3.0 instead.
         */
        enum class MapAccess: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Map buffer for reading only.
             * @requires_gl Only @ref Magnum::Buffer::MapAccess "MapAccess::WriteOnly"
             *      is available in OpenGL ES 2.0.
             */
            ReadOnly = GL_READ_ONLY,
            #endif

            /**
             * Map buffer for writing only.
             */
            #ifdef MAGNUM_TARGET_GLES
            WriteOnly = GL_WRITE_ONLY_OES
            #else
            WriteOnly = GL_WRITE_ONLY,

            /**
             * Map buffer for both reading and writing.
             * @requires_gl Only @ref Magnum::Buffer::MapAccess "MapAccess::WriteOnly"
             *      is available in OpenGL ES 2.0.
             */
            ReadWrite = GL_READ_WRITE
            #endif
        };
        #endif

        /**
         * @brief Memory mapping flag
         *
         * @see MapFlags, map(GLintptr, GLsizeiptr, MapFlags)
         * @requires_gl30 %Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 %Extension @es_extension{EXT,map_buffer_range}
         */
        enum class MapFlag: GLbitfield {
            /** Map buffer for reading. */
            #ifndef MAGNUM_TARGET_GLES2
            Read = GL_MAP_READ_BIT,
            #else
            Read = GL_MAP_READ_BIT_EXT,
            #endif

            /** Map buffer for writing. */
            #ifndef MAGNUM_TARGET_GLES2
            Write = GL_MAP_WRITE_BIT,
            #else
            Write = GL_MAP_WRITE_BIT_EXT,
            #endif

            /**
             * Previous contents of the entire buffer may be discarded. May
             * not be used in combination with @ref MapFlag::Read.
             * @see invalidateData()
             */
            #ifndef MAGNUM_TARGET_GLES2
            InvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT,
            #else
            InvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT_EXT,
            #endif

            /**
             * Previous contents of mapped range may be discarded. May not
             * be used in combination with @ref MapFlag::Read.
             * @see invalidateSubData()
             */
            #ifndef MAGNUM_TARGET_GLES2
            InvalidateRange = GL_MAP_INVALIDATE_RANGE_BIT,
            #else
            InvalidateRange = GL_MAP_INVALIDATE_RANGE_BIT_EXT,
            #endif

            /**
             * Only one or more discrete subranges of the mapping will be
             * modified. See flushMappedRange() for more information. May only
             * be used in conjuction with @ref MapFlag::Write.
             */
            #ifndef MAGNUM_TARGET_GLES2
            FlushExplicit = GL_MAP_FLUSH_EXPLICIT_BIT,
            #else
            FlushExplicit = GL_MAP_FLUSH_EXPLICIT_BIT_EXT,
            #endif

            /**
             * No pending operations on the buffer should be synchronized
             * before mapping.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Unsynchronized = GL_MAP_UNSYNCHRONIZED_BIT
            #else
            Unsynchronized = GL_MAP_UNSYNCHRONIZED_BIT_EXT
            #endif
        };

        /**
         * @brief Memory mapping flags
         *
         * @see map(GLintptr, GLsizeiptr, MapFlags)
         * @requires_gl30 %Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 %Extension @es_extension{EXT,map_buffer_range}
         */
        typedef Containers::EnumSet<MapFlag, GLbitfield> MapFlags;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Minimal supported mapping alignment
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref map(), @fn_gl{Get} with @def_gl{MIN_MAP_BUFFER_ALIGNMENT}
         * @requires_gl No minimal value is specified for OpenGL ES.
         */
        static Int minMapAlignment();

        /**
         * @brief Max supported atomic counter buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_atomic_counters} is
         * not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_ATOMIC_COUNTER_BUFFER_BINDINGS}
         * @requires_gl Atomic counters are not available in OpenGL ES.
         */
        static Int maxAtomicCounterBindings();

        /**
         * @brief Max supported shader storage buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_storage_buffer_object}
         * is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_SHADER_STORAGE_BUFFER_BINDINGS}
         * @requires_gl Atomic counters are not available in OpenGL ES.
         */
        static Int maxShaderStorageBindings();

        /**
         * @brief Alignment of shader storage buffer binding offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_storage_buffer_object}
         * is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT}
         * @requires_gl Atomic counters are not available in OpenGL ES.
         */
        static Int shaderStorageOffsetAlignment();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max supported uniform buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_object}
         * is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_UNIFORM_BUFFER_BINDINGS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         */
        static Int maxUniformBindings();
        #endif

        /**
         * @brief Max supported vertex buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @fn_gl{Get} with @def_gl{MAX_VERTEX_ATTRIB_BINDINGS}
         */
        static Int maxVertexAttributeBindings();

        /**
         * @brief Unbind any buffer from given target
         * @param target    %Target
         *
         * @see @fn_gl{BindBuffer}
         */
        static void unbind(Target target) { bind(target, 0); }

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
        static void copy(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
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
        explicit Buffer(Target targetHint = Target::Array);

        /** @brief Copying is not allowed */
        Buffer(const Buffer&) = delete;

        /** @brief Move constructor */
        Buffer(Buffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL buffer.
         * @see @fn_gl{DeleteBuffers}
         */
        ~Buffer();

        /** @brief Copying is not allowed */
        Buffer& operator=(const Buffer&) = delete;

        /** @brief Move assignment */
        Buffer& operator=(Buffer&& other) noexcept;

        /** @brief OpenGL buffer ID */
        GLuint id() const { return _id; }

        /** @brief Target hint */
        Target targetHint() const { return _targetHint; }

        /**
         * @brief Set target hint
         * @return Reference to self (for method chaining)
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
        Buffer& setTargetHint(Target hint) {
            _targetHint = hint;
            return *this;
        }

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
        void bind(Target target) { bind(target, _id); }

        /**
         * @brief %Buffer size
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see @fn_gl{BindBuffer} and @fn_gl{GetBufferParameter} or
         *      @fn_gl_extension{GetNamedBufferParameter,EXT,direct_state_access}
         *      with @def_gl{BUFFER_SIZE}
         */
        Int size();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief %Buffer data
         *
         * Returns data of whole buffer. If @extension{EXT,direct_state_access}
         * is not available and the buffer is not already bound somewhere, it
         * is bound to hinted target before the operation.
         * @see size(), subData(), setData(), @fn_gl{BindBuffer} and @fn_gl{GetBufferParameter} or
         *      @fn_gl_extension{GetNamedBufferParameter,EXT,direct_state_access}
         *      with @def_gl{BUFFER_SIZE}, @fn_gl{GetBufferSubData} or
         *      @fn_gl_extension{GetNamedBufferSubData,EXT,direct_state_access}
         * @requires_gl %Buffer data queries are not available in OpenGL ES.
         *      Use @ref Magnum::Buffer::map() "map()" instead.
         */
        template<class T> Containers::Array<T> data();

        /**
         * @brief %Buffer subdata
         * @param offset    Byte offset in the buffer
         * @param size      Data size (count of @p T values)
         *
         * Returns data of given buffer portion. If @extension{EXT,direct_state_access}
         * is not available and the buffer is not already bound somewhere, it
         * is bound to hinted target before the operation.
         * @see size(), data(), setSubData(), @fn_gl{BindBuffer} and @fn_gl{GetBufferSubData}
         *      or @fn_gl_extension{GetNamedBufferSubData,EXT,direct_state_access}
         * @requires_gl %Buffer data queries are not available in OpenGL ES.
         *      Use @ref Magnum::Buffer::map() "map()" instead.
         */
        template<class T> Containers::Array<T> subData(GLintptr offset, GLsizeiptr size);
        #endif

        /**
         * @brief Set buffer data
         * @param data      Data
         * @param usage     %Buffer usage
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see setTargetHint(), @fn_gl{BindBuffer} and @fn_gl{BufferData} or
         *      @fn_gl_extension{NamedBufferData,EXT,direct_state_access}
         */
        Buffer& setData(Containers::ArrayReference<const void> data, Usage usage) {
            (this->*dataImplementation)(data.size(), data, usage);
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set buffer data
         * @deprecated Use @ref Magnum::Buffer::setData(Containers::ArrayReference<const void>, Usage) "setData(Containers::ArrayReference<const void>, Usage)"
         *      instead.
         */
        Buffer& setData(GLsizeiptr size, const GLvoid* data, Usage usage) {
            return setData({data, std::size_t(size)}, usage);
        }
        #endif

        /**
         * @brief Set buffer data
         * @param data      Vector with data
         * @param usage     %Buffer usage
         * @return Reference to self (for method chaining)
         *
         * @see setData(GLsizeiptr, const GLvoid*, Usage)
         */
        template<class T> Buffer& setData(const std::vector<T>& data, Usage usage) {
            setData({data.data(), data.size()}, usage);
            return *this;
        }

        /** @overload */
        template<std::size_t size, class T> Buffer& setData(const std::array<T, size>& data, Usage usage) {
            setData({data.data(), data.size()}, usage);
            return *this;
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param data      Data
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see setTargetHint(), @fn_gl{BindBuffer} and @fn_gl{BufferSubData}
         *      or @fn_gl_extension{NamedBufferSubData,EXT,direct_state_access}
         */
        Buffer& setSubData(GLintptr offset, Containers::ArrayReference<const void> data) {
            (this->*subDataImplementation)(offset, data.size(), data);
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set buffer subdata
         * @deprecated Use @ref Magnum::Buffer::setSubData(GLintptr, Containers::ArrayReference<const void>) "setSubData(GLintptr, Containers::ArrayReference<const void>)"
         *      instead.
         */
        Buffer& setSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
            return setSubData(offset, {data, std::size_t(size)});
        }
        #endif

        /**
         * @brief Set buffer subdata
         * @param offset    Offset in the buffer
         * @param data      Vector with data
         * @return Reference to self (for method chaining)
         *
         * @see setSubData(GLintptr, GLsizeiptr, const GLvoid*)
         */
        template<class T> Buffer& setSubData(GLintptr offset, const std::vector<T>& data) {
            setSubData(offset, {data.data(), data.size()});
            return *this;
        }

        /** @overload */
        template<std::size_t size, class T> Buffer& setSubData(GLintptr offset, const std::array<T, size>& data) {
            setSubData(offset, {data.data(), data.size()});
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Invalidate buffer data
         * @return Reference to self (for method chaining)
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * is not available, this function does nothing.
         * @see @ref MapFlag::InvalidateBuffer, @fn_gl{InvalidateBufferData}
         */
        Buffer& invalidateData() {
            (this->*invalidateImplementation)();
            return *this;
        }

        /**
         * @brief Invalidate buffer subdata
         * @param offset    Offset into the buffer
         * @param length    Length of the invalidated range
         * @return Reference to self (for method chaining)
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * is not available, this function does nothing.
         * @see @ref MapFlag::InvalidateRange, @fn_gl{InvalidateBufferData}
         */
        Buffer& invalidateSubData(GLintptr offset, GLsizeiptr length) {
            (this->*invalidateSubImplementation)(offset, length);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES3
        /**
         * @brief Map buffer to client memory
         * @param access    Access
         * @return Pointer to buffer data
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @deprecated_gl Prefer to use @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      instead, as it has more complete set of features.
         * @see @ref minMapAlignment(), @ref unmap(), @ref setTargetHint(),
         *      @fn_gl{BindBuffer} and @fn_gl{MapBuffer} or
         *      @fn_gl_extension{MapNamedBuffer,EXT,direct_state_access}
         * @requires_es_extension %Extension @es_extension{OES,mapbuffer} in
         *      OpenGL ES 2.0, use @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      in OpenGL ES 3.0 instead.
         */
        void* map(MapAccess access) {
            return (this->*mapImplementation)(access);
        }
        #endif

        #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Map portion of buffer to client memory
         * @param offset    Offset into the buffer
         * @param length    Length of the mapped memory
         * @param access    Access
         * @return Pointer to buffer data
         *
         * If the buffer is not already bound somewhere, it is bound to hinted
         * target before the operation.
         * @deprecated_gl Prefer to use @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      instead, as it has more complete set of features.
         * @see unmapSub(), setTargetHint(), @fn_gl_extension{MapBufferSubData,CHROMIUM,map_sub}
         * @requires_gles20 Not available in ES 3.0 or desktop OpenGL. Use
         *      @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      instead.
         * @requires_es_extension %Extension @es_extension{CHROMIUM,map_sub}
         */
        void* mapSub(GLintptr offset, GLsizeiptr length, MapAccess access);
        #endif

        /**
         * @brief Map buffer to client memory
         * @param offset    Offset into the buffer
         * @param length    Length of the mapped memory
         * @param flags     Flags. At least @ref MapFlag::Read or @ref MapFlag::Write
         *      must be specified.
         * @return Pointer to buffer data
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see @ref minMapAlignment(), @ref flushMappedRange(), @ref unmap(),
         *      @ref map(MapAccess), @ref setTargetHint(), @fn_gl{BindBuffer}
         *      and @fn_gl{MapBufferRange} or @fn_gl_extension{MapNamedBufferRange,EXT,direct_state_access}
         * @requires_gl30 %Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 %Extension @es_extension{EXT,map_buffer_range}
         */
        void* map(GLintptr offset, GLsizeiptr length, MapFlags flags) {
            return (this->*mapRangeImplementation)(offset, length, flags);
        }

        /**
         * @brief Flush mapped range
         * @param offset    Offset relative to start of mapped range
         * @param length    Length of the flushed memory
         * @return Reference to self (for method chaining)
         *
         * Flushes specified subsection of mapped range. Use only if you called
         * map() with @ref MapFlag::FlushExplicit flag. See
         * @ref Buffer-data-mapping "class documentation" for usage example.
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * buffer is not already bound somewhere, it is bound to hinted target
         * before the operation.
         * @see setTargetHint(), @fn_gl{BindBuffer} and @fn_gl{FlushMappedBufferRange}
         *      or @fn_gl_extension{FlushMappedNamedBufferRange,EXT,direct_state_access}
         * @requires_gl30 %Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 %Extension @es_extension{EXT,map_buffer_range}
         */
        Buffer& flushMappedRange(GLintptr offset, GLsizeiptr length) {
            (this->*flushMappedRangeImplementation)(offset, length);
            return *this;
        }

        /**
         * @brief Unmap buffer
         * @return `False` if the data have become corrupt during the time
         *      the buffer was mapped (e.g. after screen was resized), `true`
         *      otherwise.
         *
         * Unmaps buffer previously mapped with map(), invalidating the
         * pointer returned by these functions. If @extension{EXT,direct_state_access}
         * is not available and the buffer is not already bound somewhere, it
         * is bound to hinted target before the operation.
         * @see setTargetHint(), @fn_gl{BindBuffer} and @fn_gl{UnmapBuffer} or
         *      @fn_gl_extension{UnmapNamedBuffer,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension{OES,mapbuffer}
         */
        bool unmap() {
            return (this->*unmapImplementation)();
        }

        #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Unmap portion of buffer
         *
         * Unmaps portion of buffer previously mapped with mapSub(),
         * invalidating the pointer returned by the function.
         * @see @fn_gl_extension{UnmapBufferSubData,CHROMIUM,map_sub}
         * @requires_gles20 Not available in ES 3.0 or desktop OpenGL. Use
         *      @ref Magnum::Buffer::map(GLintptr, GLsizeiptr, MapFlags) "map(GLintptr, GLsizeiptr, MapFlags)"
         *      instead.
         * @requires_es_extension %Extension @es_extension{CHROMIUM,map_sub}
         */
        void unmapSub();
        #endif

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        static void bind(Target hint, GLuint id);
        Target MAGNUM_LOCAL bindInternal(Target hint);

        #ifndef MAGNUM_TARGET_GLES2
        typedef void(*CopyImplementation)(Buffer&, Buffer&, GLintptr, GLintptr, GLsizeiptr);
        static void MAGNUM_LOCAL copyImplementationDefault(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL copyImplementationDSA(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #endif
        static CopyImplementation copyImplementation;
        #endif

        typedef void(Buffer::*GetParameterImplementation)(GLenum, GLint*);
        void MAGNUM_LOCAL getParameterImplementationDefault(GLenum value, GLint* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getParameterImplementationDSA(GLenum value, GLint* data);
        #endif
        static MAGNUM_LOCAL GetParameterImplementation getParameterImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(Buffer::*GetSubDataImplementation)(GLintptr, GLsizeiptr, GLvoid*);
        void MAGNUM_LOCAL getSubDataImplementationDefault(GLintptr offset, GLsizeiptr size, GLvoid* data);
        void MAGNUM_LOCAL getSubDataImplementationDSA(GLintptr offset, GLsizeiptr size, GLvoid* data);
        static GetSubDataImplementation getSubDataImplementation;
        #endif

        typedef void(Buffer::*DataImplementation)(GLsizeiptr, const GLvoid*, Usage);
        void MAGNUM_LOCAL dataImplementationDefault(GLsizeiptr size, const GLvoid* data, Usage usage);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL dataImplementationDSA(GLsizeiptr size, const GLvoid* data, Usage usage);
        #endif
        static DataImplementation dataImplementation;

        typedef void(Buffer::*SubDataImplementation)(GLintptr, GLsizeiptr, const GLvoid*);
        void MAGNUM_LOCAL subDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subDataImplementationDSA(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        #endif
        static SubDataImplementation subDataImplementation;

        typedef void(Buffer::*InvalidateImplementation)();
        void MAGNUM_LOCAL invalidateImplementationNoOp();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationARB();
        #endif
        static InvalidateImplementation invalidateImplementation;

        typedef void(Buffer::*InvalidateSubImplementation)(GLintptr, GLsizeiptr);
        void MAGNUM_LOCAL invalidateSubImplementationNoOp(GLintptr offset, GLsizeiptr length);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateSubImplementationARB(GLintptr offset, GLsizeiptr length);
        #endif
        static InvalidateSubImplementation invalidateSubImplementation;

        #ifndef MAGNUM_TARGET_GLES3
        typedef void*(Buffer::*MapImplementation)(MapAccess);
        void MAGNUM_LOCAL * mapImplementationDefault(MapAccess access);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL * mapImplementationDSA(MapAccess access);
        #endif
        static MapImplementation mapImplementation;
        #endif

        typedef void*(Buffer::*MapRangeImplementation)(GLintptr, GLsizeiptr, MapFlags);
        void MAGNUM_LOCAL * mapRangeImplementationDefault(GLintptr offset, GLsizeiptr length, MapFlags access);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL * mapRangeImplementationDSA(GLintptr offset, GLsizeiptr length, MapFlags access);
        #endif
        static MapRangeImplementation mapRangeImplementation;

        typedef void(Buffer::*FlushMappedRangeImplementation)(GLintptr, GLsizeiptr);
        void MAGNUM_LOCAL flushMappedRangeImplementationDefault(GLintptr offset, GLsizeiptr length);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL flushMappedRangeImplementationDSA(GLintptr offset, GLsizeiptr length);
        #endif
        static FlushMappedRangeImplementation flushMappedRangeImplementation;

        typedef bool(Buffer::*UnmapImplementation)();
        bool MAGNUM_LOCAL unmapImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        bool MAGNUM_LOCAL unmapImplementationDSA();
        #endif
        static UnmapImplementation unmapImplementation;

        GLuint _id;
        Target _targetHint;
        #ifdef CORRADE_TARGET_NACL
        void* _mappedBuffer;
        #endif
};

CORRADE_ENUMSET_OPERATORS(Buffer::MapFlags)

/** @debugoperator{Magnum::Buffer} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Buffer::Target value);

inline Buffer::Buffer(Buffer&& other) noexcept: _id(other._id), _targetHint(other._targetHint) {
    other._id = 0;
}

inline Buffer& Buffer::operator=(Buffer&& other) noexcept {
    std::swap(_id, other._id);
    std::swap(_targetHint, other._targetHint);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
template<class T> Containers::Array<T> inline Buffer::data() {
    const Int bufferSize = size();
    #ifndef CORRADE_GCC45_COMPATIBILITY
    CORRADE_ASSERT(bufferSize%sizeof(T) == 0, "Buffer::data(): the buffer size is" << bufferSize << "bytes, which can't be expressed as array of types with size" << sizeof(T), nullptr);
    #else
    CORRADE_ASSERT(bufferSize%sizeof(T) == 0, "Buffer::data(): the buffer size is" << bufferSize << "bytes, which can't be expressed as array of types with size" << sizeof(T), {});
    #endif
    return subData<T>(0, bufferSize/sizeof(T));
}

template<class T> Containers::Array<T> inline Buffer::subData(const GLintptr offset, const GLsizeiptr size) {
    Containers::Array<T> data(size);
    if(size) (this->*getSubDataImplementation)(offset, size*sizeof(T), data);
    return std::move(data);
}
#endif

}

#endif
