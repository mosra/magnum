#ifndef Magnum_Buffer_h
#define Magnum_Buffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Buffer, enum @ref Magnum::BufferUsage
 */

#include <cstddef>
#include <array>
#include <vector>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum {

/**
@brief Buffer usage

@see @ref Buffer, @ref Buffer::setData(Containers::ArrayView<const void>, BufferUsage)
*/
enum class BufferUsage: GLenum {
    /** Set once by the application and used infrequently for drawing. */
    StreamDraw = GL_STREAM_DRAW,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Set once as output from an OpenGL command and used infequently for
     * drawing.
     * @requires_gles30 Only @ref BufferUsage::StreamDraw is available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only @ref BufferUsage::StreamDraw is available in
     *      WebGL 1.0.
     */
    StreamRead = GL_STREAM_READ,

    /**
     * Set once as output from an OpenGL command and used infrequently for
     * drawing or copying to other buffers.
     * @requires_gles30 Only @ref BufferUsage::StreamDraw is available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only @ref BufferUsage::StreamDraw is available in
     *      WebGL 1.0.
     */
    StreamCopy = GL_STREAM_COPY,
    #endif

    /** Set once by the application and used frequently for drawing. */
    StaticDraw = GL_STATIC_DRAW,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Set once as output from an OpenGL command and queried many times by the
     * application.
     * @requires_gles30 Only @ref BufferUsage::StaticDraw is available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only @ref BufferUsage::StaticDraw is available in
     *      WebGL 1.0.
     */
    StaticRead = GL_STATIC_READ,

    /**
     * Set once as output from an OpenGL command and used frequently for
     * drawing or copying to other buffers.
     * @requires_gles30 Only @ref BufferUsage::StaticDraw is available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only @ref BufferUsage::StaticDraw is available in
     *      WebGL 1.0.
     */
    StaticCopy = GL_STATIC_COPY,
    #endif

    /**
     * Updated frequently by the application and used frequently for drawing or
     * copying to other images.
     */
    DynamicDraw = GL_DYNAMIC_DRAW,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Updated frequently as output from OpenGL command and queried many times
     * from the application.
     * @requires_gles30 Only @ref BufferUsage::DynamicDraw is available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only @ref BufferUsage::DynamicDraw is available in
     *      WebGL 1.0.
     */
    DynamicRead = GL_DYNAMIC_READ,

    /**
     * Updated frequently as output from OpenGL command and used frequently for
     * drawing or copying to other images.
     * @requires_gles30 Only @ref BufferUsage::DynamicDraw is available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only @ref BufferUsage::DynamicDraw is available in
     *      WebGL 1.0.
     */
    DynamicCopy = GL_DYNAMIC_COPY
    #endif
};

namespace Implementation { struct BufferState; }

/**
@brief Buffer

Encapsulates one OpenGL buffer object and provides functions for convenient
data updates.

## Data updating

Default way to set or update buffer data with @ref setData() or @ref setSubData()
is to use @ref Corrade::Containers::ArrayView. See its documentation for
more information about automatic conversions etc.
@code
Containers::ArrayView<Vector3> data;
buffer.setData(data, BufferUsage::StaticDraw);
@endcode
There is also overload for array-like containers from STL, such as `std::vector`
or `std::array`:
@code
std::vector<Vector3> data;
buffer.setData(data, BufferUsage::StaticDraw);
@endcode

@anchor Buffer-data-mapping
## Memory mapping

Buffer data can be also updated asynchronously. First you need to allocate
the buffer to desired size by passing `nullptr` to @ref setData(), e.g.:
@code
buffer.setData({nullptr, 200*sizeof(Vector3)}, BufferUsage::StaticDraw);
@endcode
Then you can map the buffer to client memory and operate with the memory
directly. After you are done with the operation, call @ref unmap() to unmap the
buffer again. The @ref map() functions return a view on `char` array and you
may want to cast it to some useful type first using @ref Containers::arrayCast():
@code
Containers::ArrayView<Vector3> data = Containers::arrayCast<Vector3>(buffer.map(0, 200*sizeof(Vector3), Buffer::MapFlag::Write|Buffer::MapFlag::InvalidateBuffer));
CORRADE_INTERNAL_ASSERT(data);
for(Vector3& d: data)
    d = ...;
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
@endcode
If you are updating only a few discrete portions of the buffer, you can use
@ref MapFlag::FlushExplicit and @ref flushMappedRange() to reduce number of
memory operations performed by OpenGL on unmapping. Example:
@code
Containers::ArrayView<Vector3> data = Containers::arrayCast<Vector3>(buffer.map(0, 200*sizeof(Vector3), Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit));
CORRADE_INTERNAL_ASSERT(data);
for(std::size_t i: {7, 27, 56, 128}) {
    data[i] = ...;
    buffer.flushMappedRange(i*sizeof(Vector3), sizeof(Vector3));
}
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
@endcode

## WebGL restrictions

Buffers in @ref MAGNUM_TARGET_WEBGL "WebGL" need to be bound only to one unique
target, i.e., @ref Buffer bound to @ref Buffer::Target::Array cannot be later
rebound to @ref Buffer::Target::ElementArray. However, Magnum by default uses
any sufficient target when binding the buffer internally (e.g. for setting
data). To avoid GL errors, set target hint to desired target, either in
constructor or using @ref Buffer::setTargetHint():
@code
Buffer vertices{Buffer::Target::Array};
Buffer indices{Buffer::Target::ElementArray};
@endcode

To ease up the development, @ref Mesh checks proper target hint when adding
vertex and index buffers in WebGL.

## Performance optimizations

The engine tracks currently bound buffers to avoid unnecessary calls to
@fn_gl{BindBuffer}. If the buffer is already bound to some target, functions
@ref copy(), @ref setData(), @ref setSubData(), @ref map(), @ref mapRead(),
@ref flushMappedRange() and @ref unmap() use that target instead of binding the
buffer to some specific target. You can also use @ref setTargetHint() to
possibly reduce unnecessary rebinding. Buffer limits and implementation-defined
values (such as @ref maxUniformBindings()) are cached, so repeated queries
don't result in repeated @fn_gl{Get} calls. See also @ref Context::resetState()
and @ref Context::State::Buffers.

If either @extension{ARB,direct_state_access} (part of OpenGL 4.5) or
@extension{EXT,direct_state_access} desktop extension is available, functions
@ref copy(), @ref size(), @ref data(), @ref subData(), @ref setData(),
@ref setSubData(), @ref map(), @ref mapRead(), @ref flushMappedRange() and
@ref unmap() use DSA functions to avoid unnecessary calls to @fn_gl{BindBuffer}.
See their respective documentation for more information.

You can use functions @ref invalidateData() and @ref invalidateSubData() if you
don't need buffer data anymore to avoid unnecessary memory operations performed
by OpenGL in order to preserve the data. If running on OpenGL ES or extension
@extension{ARB,invalidate_subdata} (part of OpenGL 4.3) is not available, these
functions do nothing.
 */
class MAGNUM_EXPORT Buffer: public AbstractObject {
    friend Implementation::BufferState;

    public:
        /**
         * @brief Buffer target
         *
         * @see @ref Buffer(), @ref setTargetHint()
         */
        enum class TargetHint: GLenum {
            /** Used for storing vertex attributes. */
            Array = GL_ARRAY_BUFFER,

            #ifndef MAGNUM_TARGET_GLES2
            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Used for storing atomic counters.
             * @requires_gl42 Extension @extension{ARB,shader_atomic_counters}
             * @requires_gles31 Atomic counters are not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Atomic counters are not available in WebGL.
             */
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            #endif

            /**
             * Source for copies. See @ref copy().
             * @requires_gl31 Extension @extension{ARB,copy_buffer}
             * @requires_gles30 Buffer copying is not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Buffer copying is not available in WebGL 1.0.
             */
            CopyRead = GL_COPY_READ_BUFFER,

            /**
             * Target for copies. See @ref copy().
             * @requires_gl31 Extension @extension{ARB,copy_buffer}
             * @requires_gles30 Buffer copying is not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Buffer copying is not available in WebGL 1.0.
             */
            CopyWrite = GL_COPY_WRITE_BUFFER,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Indirect compute dispatch commands.
             * @requires_gl43 Extension @extension{ARB,compute_shader}
             * @requires_gles31 Compute shaders are not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Compute shaders are not available in WebGL.
             */
            DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,

            /**
             * Used for supplying arguments for indirect drawing.
             * @requires_gl40 Extension @extension{ARB,draw_indirect}
             * @requires_gles31 Indirect drawing is not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Indirect drawing is not available in WebGL.
             */
            DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
            #endif
            #endif

            /** Used for storing vertex indices. */
            ElementArray = GL_ELEMENT_ARRAY_BUFFER,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Target for pixel pack operations.
             * @requires_gles30 Pixel buffer objects are not available in
             *      OpenGL ES 2.0.
             * @requires_webgl20 Pixel buffer objects are not available in
             *      WebGL 1.0.
             */
            PixelPack = GL_PIXEL_PACK_BUFFER,

            /**
             * Source for texture update operations.
             * @requires_gles30 Pixel buffer objects are not available in
             *      OpenGL ES 2.0.
             * @requires_webgl20 Pixel buffer objects are not available in
             *      WebGL 1.0.
             */
            PixelUnpack = GL_PIXEL_UNPACK_BUFFER,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Used for shader storage.
             * @requires_gl43 Extension @extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage is not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Shader storage is not available in WebGL.
             */
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            #endif
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Source for texel fetches. See @ref BufferTexture.
             * @requires_gl31 Extension @extension{ARB,texture_buffer_object}
             * @requires_gl Texture buffers are not available in OpenGL ES or
             *      WebGL.
             */
            Texture = GL_TEXTURE_BUFFER,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Target for transform feedback.
             * @requires_gl30 Extension @extension{EXT,transform_feedback}
             * @requires_gles30 Transform feedback is not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Transform feedback is not available in WebGL
             *      1.0.
             */
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,

            /**
             * Used for storing uniforms.
             * @requires_gl31 Extension @extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             */
            Uniform = GL_UNIFORM_BUFFER
            #endif
        };

        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_BUILD_DEPRECATED)
        /**
         * @brief Buffer binding target
         *
         * @see @ref bind(), @ref unbind()
         */
        enum class Target: GLenum {
            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * @copydoc TargetHint::Array
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::Array instead.
             */
            Array CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::Array instead") = GL_ARRAY_BUFFER,
            #endif

            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /**
             * Atomic counter binding
             * @requires_gl42 Extension @extension{ARB,shader_atomic_counters}
             * @requires_gles31 Atomic counters are not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Atomic counters are not available in WebGL.
             */
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            #endif

            #ifdef MAGNUM_BUILD_DEPRECATED
            #ifndef MAGNUM_TARGET_GLES2
            /**
             * @copydoc TargetHint::CopyRead
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::CopyRead instead.
             */
            CopyRead CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::CopyRead instead") = GL_COPY_READ_BUFFER,

            /**
             * @copydoc TargetHint::CopyWrite
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::CopyWrite instead.
             */
            CopyWrite CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::CopyWrite instead") = GL_COPY_WRITE_BUFFER,
            #endif

            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /**
             * @copydoc TargetHint::DispatchIndirect
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::DispatchIndirect instead.
             */
            DispatchIndirect CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::DispatchIndirect instead") = GL_DISPATCH_INDIRECT_BUFFER,

            /**
             * @copydoc TargetHint::DrawIndirect
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::DrawIndirect instead.
             */
            DrawIndirect CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::DrawIndirect instead") = GL_DRAW_INDIRECT_BUFFER,
            #endif

            /**
             * @copydoc TargetHint::ElementArray
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::ElementArray instead.
             */
            ElementArray CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::ElementArray instead") = GL_ELEMENT_ARRAY_BUFFER,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * @copydoc TargetHint::PixelPack
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::PixelPack instead.
             */
            PixelPack CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::PixelPack instead") = GL_PIXEL_PACK_BUFFER,

            /**
             * @copydoc TargetHint::PixelUnpack
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::PixelUnpack instead.
             */
            PixelUnpack CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::PixelUnpack instead") = GL_PIXEL_UNPACK_BUFFER,
            #endif
            #endif

            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /**
             * Shader storage binding
             * @requires_gl43 Extension @extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage is not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Shader storage is not available in WebGL.
             */
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            #endif

            #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES)
            /**
             * @copydoc TargetHint::Texture
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::Texture instead.
             */
            Texture CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::Texture instead") = GL_TEXTURE_BUFFER,
            #endif

            #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
            /**
             * @copydoc TargetHint::TransformFeedback
             * @deprecated For @ref setTargetHint() only, use
             *      @ref TargetHint::TransformFeedback instead.
             */
            TransformFeedback CORRADE_DEPRECATED_ENUM("use Buffer::TargetHint::TransformFeedback instead") = GL_TRANSFORM_FEEDBACK_BUFFER,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Uniform binding
             * @requires_gl31 Extension @extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             */
            Uniform = GL_UNIFORM_BUFFER
            #endif
        };
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief BufferUsage
         * @deprecated Use @ref BufferUsage instead.
         */
        typedef CORRADE_DEPRECATED("use BufferUsage instead") BufferUsage Usage;
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Memory mapping access
         *
         * @see @ref map(MapAccess)
         * @requires_es_extension Extension @extension{OES,mapbuffer} or
         *      @extension{CHROMIUM,map_sub}
         * @requires_gles Buffer mapping is not available in WebGL.
         * @deprecated_gl Prefer to use @ref map(GLintptr, GLsizeiptr, MapFlags)
         *      instead, as it has more complete set of features.
         */
        enum class MapAccess: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Map buffer for reading only.
             * @requires_gl Only @ref MapAccess::WriteOnly is available in
             *      OpenGL ES.
             */
            ReadOnly = GL_READ_ONLY,
            #endif

            /** Map buffer for writing only. */
            #ifdef MAGNUM_TARGET_GLES
            WriteOnly = GL_WRITE_ONLY_OES
            #else
            WriteOnly = GL_WRITE_ONLY,

            /**
             * Map buffer for both reading and writing.
             * @requires_gl Only @ref MapAccess::WriteOnly is available in
             *      OpenGL ES.
             */
            ReadWrite = GL_READ_WRITE
            #endif
        };

        /**
         * @brief Memory mapping flag
         *
         * @see @ref MapFlags, @ref map(GLintptr, GLsizeiptr, MapFlags)
         * @requires_gl30 Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 Extension @extension{EXT,map_buffer_range} in
         *      OpenGL ES 2.0.
         * @requires_gles Buffer mapping is not available in WebGL.
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
             * Previous contents of the entire buffer may be discarded. May not
             * be used in combination with @ref MapFlag::Read.
             * @see @ref invalidateData()
             */
            #ifndef MAGNUM_TARGET_GLES2
            InvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT,
            #else
            InvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT_EXT,
            #endif

            /**
             * Previous contents of mapped range may be discarded. May not be
             * used in combination with @ref MapFlag::Read.
             * @see @ref invalidateSubData()
             */
            #ifndef MAGNUM_TARGET_GLES2
            InvalidateRange = GL_MAP_INVALIDATE_RANGE_BIT,
            #else
            InvalidateRange = GL_MAP_INVALIDATE_RANGE_BIT_EXT,
            #endif

            /**
             * Only one or more discrete subranges of the mapping will be
             * modified. See @ref flushMappedRange() for more information. May
             * only be used in conjuction with @ref MapFlag::Write.
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
         * @see @ref map(GLintptr, GLsizeiptr, MapFlags)
         * @requires_gl30 Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 Extension @extension{EXT,map_buffer_range} in
         *      OpenGL ES 2.0.
         * @requires_gles Buffer mapping is not available in WebGL.
         */
        typedef Containers::EnumSet<MapFlag> MapFlags;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Minimal supported mapping alignment
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,map_buffer_alignment}
         * (part of OpenGL 4.2) is not available, returns `1`.
         * @see @ref map(), @ref mapRead(), @fn_gl{Get} with
         *      @def_gl{MIN_MAP_BUFFER_ALIGNMENT}
         * @requires_gl No minimal value is specified for OpenGL ES. Buffer
         *      mapping is not available in WebGL.
         */
        static Int minMapAlignment();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Max supported atomic counter buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @ref bind(), @ref unbind(), @fn_gl{Get} with
         *      @def_gl{MAX_ATOMIC_COUNTER_BUFFER_BINDINGS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounterBindings();

        /**
         * @brief Max supported shader storage buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @ref bind(), @ref unbind(), @fn_gl{Get} with
         *      @def_gl{MAX_SHADER_STORAGE_BUFFER_BINDINGS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader storage is not available in WebGL.
         */
        static Int maxShaderStorageBindings();
        #endif

        /**
         * @brief Alignment of uniform buffer binding offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_object}
         * (part of OpenGL 3.1) is not available, returns `1`.
         * @see @ref bind(), @fn_gl{Get} with @def_gl{UNIFORM_BUFFER_OFFSET_ALIGNMENT}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        static Int uniformOffsetAlignment();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Alignment of shader storage buffer binding offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `1`.
         * @see @ref bind(), @fn_gl{Get} with @def_gl{SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader storage is not available in WebGL.
         */
        static Int shaderStorageOffsetAlignment();
        #endif

        /**
         * @brief Max supported uniform buffer binding count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_object}
         * (part of OpenGL 3.1) is not available, returns `0`.
         * @see @ref bind(), @ref unbind(), @fn_gl{Get} with
         *      @def_gl{MAX_UNIFORM_BUFFER_BINDINGS}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        static Int maxUniformBindings();

        /**
         * @brief Unbind any buffer from given indexed target
         *
         * The @p index parameter must respect limits for given @p target.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(), @ref maxAtomicCounterBindings(),
         *      @ref maxShaderStorageBindings(), @ref maxUniformBindings(),
         *      @fn_gl{BindBufferBase}
         * @requires_gl30 No form of indexed buffer binding is available in
         *      OpenGL 2.1, see particular @ref Target values for
         *      version/extension requirements.
         * @requires_gles30 No form of indexed buffer binding is available in
         *      OpenGL ES 2.0, see particular @ref Target values for version
         *      requirements.
         * @requires_webgl20 No form of indexed buffer binding is available in
         *      WebGL 1.0, see particular @ref Target values for version
         *      requirements.
         */
        static void unbind(Target target, UnsignedInt index);

        /**
         * @brief Unbind given range of indexed targets
         *
         * Unbinds all buffers in given target in range @f$ [ firstIndex ; firstIndex + count ] @f$.
         * The range of indices must respect limits for given @p target. If
         * @extension{ARB,multi_bind} (part of OpenGL 4.4) is not available,
         * the feature is emulated with sequence of @ref unbind(Target, UnsignedInt)
         * calls.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref unbind(Target, UnsignedInt), @ref maxAtomicCounterBindings(),
         *      @ref maxShaderStorageBindings(), @ref maxUniformBindings(),
         *      @fn_gl{BindBuffersBase} or @fn_gl{BindBufferBase}
         * @requires_gl30 No form of indexed buffer binding is available in
         *      OpenGL 2.1, see particular @ref Target values for
         *      version/extension requirements.
         * @requires_gles30 No form of indexed buffer binding is available in
         *      OpenGL ES 2.0, see particular @ref Target values for version
         *      requirements.
         * @requires_webgl20 No form of indexed buffer binding is available in
         *      WebGL 1.0, see particular @ref Target values for version
         *      requirements.
         */
        static void unbind(Target target, UnsignedInt firstIndex, std::size_t count);

        /**
         * @brief Bind ranges of buffers to given range of indexed targets
         *
         * Binds first buffer in the list to @p firstIndex, second to
         * `firstIndex + 1` etc. Second parameter is offset, third is size. If
         * any buffer is `nullptr`, given indexed target is unbound. The range
         * of indices must respect limits for given @p target. The offsets must
         * respect alignment, which is 4 bytes for @ref Target::AtomicCounter
         * and implementation-defined for other targets. All the buffers must
         * have allocated data store. If @extension{ARB,multi_bind} (part of
         * OpenGL 4.4) is not available, the feature is emulated with sequence
         * of @ref bind(Target, UnsignedInt, GLintptr, GLsizeiptr) /
         * @ref unbind(Target, UnsignedInt) calls.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(Target, UnsignedInt, GLintptr, GLsizeiptr),
         *      @ref maxAtomicCounterBindings(), @ref maxShaderStorageBindings(),
         *      @ref maxUniformBindings(), @ref shaderStorageOffsetAlignment(),
         *      @ref uniformOffsetAlignment(), @ref TransformFeedback::attachBuffers(),
         *      @fn_gl{BindBuffersRange} or @fn_gl{BindBufferRange}
         * @requires_gl30 No form of indexed buffer binding is available in
         *      OpenGL 2.1, see particular @ref Target values for
         *      version/extension requirements.
         * @requires_gles30 No form of indexed buffer binding is available in
         *      OpenGL ES 2.0, see particular @ref Target values for version
         *      requirements.
         * @requires_webgl20 No form of indexed buffer binding is available in
         *      WebGL 1.0, see particular @ref Target values for version
         *      requirements.
         */
        static void bind(Target target, UnsignedInt firstIndex, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers);

        /**
         * @brief Bind buffers to given range of indexed targets
         *
         * Binds first buffer in the list to @p firstIndex, second to
         * `firstIndex + 1` etc. If any buffer is `nullptr`, given indexed
         * target is unbound. The range of indices must respect limits for
         * given @p target. All the buffers must have allocated data store. If
         * @extension{ARB,multi_bind} (part of OpenGL 4.4) is not available,
         * the feature is emulated with sequence of @ref bind(Target, UnsignedInt)
         * / @ref unbind(Target, UnsignedInt) calls.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(Target, UnsignedInt), @ref maxAtomicCounterBindings(),
         *      @ref maxShaderStorageBindings(), @ref maxUniformBindings(),
         *      @ref TransformFeedback::attachBuffers(), @fn_gl{BindBuffersBase}
         *      or @fn_gl{BindBufferBase}
         * @requires_gl30 No form of indexed buffer binding is available in
         *      OpenGL 2.1, see particular @ref Target values for
         *      version/extension requirements.
         * @requires_gles30 No form of indexed buffer binding is available in
         *      OpenGL ES 2.0, see particular @ref Target values for version
         *      requirements.
         * @requires_webgl20 No form of indexed buffer binding is available in
         *      WebGL 1.0, see particular @ref Target values for version
         *      requirements.
         */
        static void bind(Target target, UnsignedInt firstIndex, std::initializer_list<Buffer*> buffers);

        /**
         * @brief Copy one buffer to another
         * @param read          Buffer from which to read
         * @param write         Buffer to which to copy
         * @param readOffset    Offset in the read buffer
         * @param writeOffset   Offset in the write buffer
         * @param size          Data size
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, @p read buffer is bound for reading and @p write buffer
         * is bound for writing before the copy is performed (if not already).
         * @see @fn_gl2{CopyNamedBufferSubData,CopyBufferSubData},
         *      @fn_gl_extension{NamedCopyBufferSubData,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{CopyBufferSubData}
         * @requires_gl31 Extension @extension{ARB,copy_buffer}
         * @requires_gles30 Buffer copying is not available in OpenGL ES 2.0.
         * @requires_webgl20 Buffer copying is not available in WebGL 1.0.
         */
        static void copy(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #endif

        /**
         * @brief Wrap existing OpenGL buffer object
         * @param id            OpenGL buffer ID
         * @param targetHint    Target hint, see @ref setTargetHint() for more
         *      information
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL buffer object.
         * Unlike buffer created using constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Buffer wrap(GLuint id, TargetHint targetHint = TargetHint::Array, ObjectFlags flags = {}) {
            return Buffer{id, targetHint, flags};
        }

        /** @overload */
        static Buffer wrap(GLuint id, ObjectFlags flags) {
            return Buffer(id, TargetHint::Array, flags);
        }

        /**
         * @brief Constructor
         * @param targetHint    Target hint, see @ref setTargetHint() for more
         *      information
         *
         * Creates new OpenGL buffer object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the buffer is created on
         * first use.
         * @see @ref Buffer(NoCreateT), @ref wrap(), @fn_gl{CreateBuffers},
         *      eventually @fn_gl{GenBuffers}
         */
        explicit Buffer(TargetHint targetHint = TargetHint::Array);

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref Buffer(TargetHint), @ref wrap()
         */
        explicit Buffer(NoCreateT) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief Buffer(TargetHint)
         * @deprecated Use @ref Buffer(TargetHint) instead.
         */
        CORRADE_DEPRECATED("use Buffer(TargetHint) instead") explicit Buffer(Target targetHint): Buffer{static_cast<TargetHint>(targetHint)} {}
        #endif

        /** @brief Copying is not allowed */
        Buffer(const Buffer&) = delete;

        /** @brief Move constructor */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline Buffer(Buffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL buffer object.
         * @see @ref wrap(), @ref release(), @fn_gl{DeleteBuffers}
         */
        ~Buffer();

        /** @brief Copying is not allowed */
        Buffer& operator=(const Buffer&) = delete;

        /** @brief Move assignment */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline Buffer& operator=(Buffer&& other) noexcept;

        /** @brief OpenGL buffer ID */
        GLuint id() const { return _id; }

        /**
         * @brief Release OpenGL object
         *
         * Releases ownership of OpenGL buffer object and returns its ID so it
         * is not deleted on destruction. The internal state is then equivalent
         * to moved-from state.
         * @see @ref wrap()
         */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Buffer label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{BUFFER} or
         *      @fn_gl_extension{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{BUFFER_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label();

        /**
         * @brief Set buffer label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with @def_gl{BUFFER}
         *      or @fn_gl_extension{LabelObject,EXT,debug_label} with
         *      @def_gl{BUFFER_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        Buffer& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> Buffer& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /** @brief Target hint */
        TargetHint targetHint() const { return _targetHint; }

        /**
         * @brief Set target hint
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer needs to be internally bound to some target
         * before any operation. You can specify target which will always be
         * used when binding the buffer internally, possibly saving some calls
         * to @fn_gl{BindBuffer}. Default target hint is @ref Target::Array.
         * @see @ref setData(), @ref setSubData()
         * @todo Target::ElementArray cannot be used when no VAO is bound -
         *      http://www.opengl.org/wiki/Vertex_Specification#Index_buffers
         *      ... damned GL state
         */
        Buffer& setTargetHint(TargetHint hint) {
            _targetHint = hint;
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief setTargetHint(TargetHint)
         * @deprecated Use @ref setTargetHint(TargetHint) instead.
         */
        CORRADE_DEPRECATED("use setTargetHint(TargetHint) instead") Buffer& setTargetHint(Target hint) {
            return setTargetHint(static_cast<TargetHint>(hint));
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind buffer range to given binding index
         *
         * The @p index parameter must respect limits for given @p target. The
         * @p offset parameter must respect alignment, which is 4 bytes for
         * @ref Target::AtomicCounter and implementation-defined for other
         * targets. The buffer must have allocated data store.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(Target, UnsignedInt, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>>),
         *      @ref maxAtomicCounterBindings(), @ref maxShaderStorageBindings(),
         *      @ref maxUniformBindings(), @ref shaderStorageOffsetAlignment(),
         *      @ref uniformOffsetAlignment(), @ref TransformFeedback::attachBuffer(),
         *      @fn_gl{BindBufferRange}
         * @requires_gl30 No form of indexed buffer binding is available in
         *      OpenGL 2.1, see particular @ref Target values for
         *      version/extension requirements.
         * @requires_gles30 No form of indexed buffer binding is available in
         *      OpenGL ES 2.0, see particular @ref Target values for version
         *      requirements.
         * @requires_webgl20 No form of indexed buffer binding is available in
         *      WebGL 1.0, see particular @ref Target values for version
         *      requirements.
         * @todo State tracking for indexed binding
         */
        Buffer& bind(Target target, UnsignedInt index, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind buffer to given binding index
         *
         * The @p index parameter must respect limits for given @p target. The
         * buffer must have allocated data store.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bind(Target, UnsignedInt, std::initializer_list<Buffer*>),
         *      @ref maxAtomicCounterBindings(), @ref maxShaderStorageBindings(),
         *      @ref maxUniformBindings(), @ref TransformFeedback::attachBuffer(),
         *      @fn_gl{BindBufferBase}
         * @requires_gl30 No form of indexed buffer binding is available in
         *      OpenGL 2.1, see particular @ref Target values for
         *      version/extension requirements.
         * @requires_gles30 No form of indexed buffer binding is available in
         *      OpenGL ES 2.0, see particular @ref Target values for version
         *      requirements.
         * @requires_webgl20 No form of indexed buffer binding is available in
         *      WebGL 1.0, see particular @ref Target values for version
         *      requirements.
         */
        Buffer& bind(Target target, UnsignedInt index);
        #endif

        /**
         * @brief Buffer size
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref setTargetHint(), @fn_gl2{GetNamedBufferParameter,GetBufferParameter},
         *      @fn_gl_extension{GetNamedBufferParameter,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{GetBufferParameter}
         */
        Int size();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Buffer data
         *
         * Returns data of whole buffer. If neither @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) nor @extension{EXT,direct_state_access} is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref size(), @ref subData(), @ref setData(), @ref setTargetHint(),
         *      @fn_gl2{GetNamedBufferParameter,GetBufferParameter},
         *      @fn_gl_extension{GetNamedBufferParameter,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{GetBufferParameter}
         *      with @def_gl{BUFFER_SIZE}, then @fn_gl2{GetNamedBufferSubData,GetBufferSubData},
         *      @fn_gl_extension{GetNamedBufferSubData,EXT,direct_state_access},
         *      eventually @fn_gl{GetBufferSubData}
         * @requires_gl Buffer data queries are not available in OpenGL ES and
         *      WebGL. Use @ref map(), @ref mapRead() or @ref DebugTools::bufferData()
         *      in OpenGL ES instead.
         */
        Containers::Array<char> data();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief data()
         * @deprecated Use non-templated @ref subData() and @ref Containers::arrayCast()
         *      instead.
         */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        template<class T> CORRADE_DEPRECATED("use non-templated data() and Containers::arrayCast() instead") inline Containers::Array<T> data();
        #endif

        /**
         * @brief Buffer subdata
         * @param offset    Byte offset in the buffer
         * @param size      Data size in bytes
         *
         * Returns data of given buffer portion. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} is available, the buffer is
         * bound to hinted target before the operation (if not already).
         * @see @ref size(), @ref data(), @ref setSubData(), @ref setTargetHint(),
         *      @fn_gl2{GetNamedBufferSubData,GetBufferSubData},
         *      @fn_gl_extension{GetNamedBufferSubData,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{GetBufferSubData}
         * @requires_gl Buffer data queries are not available in OpenGL ES and
         *      WebGL. Use @ref map(), @ref mapRead() or @ref DebugTools::bufferData()
         *      in OpenGL ES instead.
         */
        Containers::Array<char> subData(GLintptr offset, GLsizeiptr size);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief subData()
         * @deprecated Use non-templated @ref subData() and @ref Containers::arrayCast() instead
         */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        template<class T> CORRADE_DEPRECATED("use non-templated subData() and Containers::arrayCast() instead") inline Containers::Array<T> subData(GLintptr offset, GLsizeiptr size);
        #endif
        #endif

        /**
         * @brief Set buffer data
         * @param data      Data
         * @param usage     Buffer usage
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref setTargetHint(), @fn_gl2{NamedBufferData,BufferData},
         *      @fn_gl_extension{NamedBufferData,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{BufferData}
         */
        Buffer& setData(Containers::ArrayView<const void> data, BufferUsage usage);

        /** @overload */
        template<class T> Buffer& setData(const std::vector<T>& data, BufferUsage usage) {
            setData({data.data(), data.size()}, usage);
            return *this;
        }

        /** @overload */
        template<std::size_t size, class T> Buffer& setData(const std::array<T, size>& data, BufferUsage usage) {
            setData({data.data(), data.size()}, usage);
            return *this;
        }

        /**
         * @brief Set buffer subdata
         * @param offset    Byte offset in the buffer
         * @param data      Data
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref setTargetHint(), @fn_gl2{NamedBufferSubData,BufferSubData},
         *      @fn_gl_extension{NamedBufferSubData,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{BufferSubData}
         */
        Buffer& setSubData(GLintptr offset, Containers::ArrayView<const void> data);

        /** @overload */
        template<class T> Buffer& setSubData(GLintptr offset, const std::vector<T>& data) {
            setSubData(offset, {data.data(), data.size()});
            return *this;
        }

        /** @overload */
        template<std::size_t size, class T> Buffer& setSubData(GLintptr offset, const std::array<T, size>& data) {
            setSubData(offset, {data.data(), data.size()});
            return *this;
        }

        /**
         * @brief Invalidate buffer data
         * @return Reference to self (for method chaining)
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * (part of OpenGL 4.3) is not available, this function does nothing.
         * @see @ref MapFlag::InvalidateBuffer, @fn_gl{InvalidateBufferData}
         */
        Buffer& invalidateData();

        /**
         * @brief Invalidate buffer subdata
         * @param offset    Byte offset into the buffer
         * @param length    Length of the invalidated range
         * @return Reference to self (for method chaining)
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * (part of OpenGL 4.3) is not available, this function does nothing.
         * @see @ref MapFlag::InvalidateRange, @fn_gl{InvalidateBufferData}
         */
        Buffer& invalidateSubData(GLintptr offset, GLsizeiptr length);

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Map buffer to client memory
         * @param access    Access
         * @return Pointer to mapped buffer data or `nullptr` on error
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref mapRead(), @ref minMapAlignment(), @ref unmap(),
         *      @ref setTargetHint(), @fn_gl2{MapNamedBuffer,MapBuffer},
         *      @fn_gl_extension{MapNamedBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{MapBuffer}
         * @requires_es_extension Extension @extension{OES,mapbuffer} in
         *      OpenGL ES 2.0, use @ref map(GLintptr, GLsizeiptr, MapFlags) in
         *      OpenGL ES 3.0 instead.
         * @requires_gles Buffer mapping is not available in WebGL.
         * @deprecated_gl Prefer to use @ref map(GLintptr, GLsizeiptr, MapFlags)
         *      instead, as it has more complete set of features.
         */
        char* map(MapAccess access);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Map buffer read-only to client memory
         *
         * Equivalent to @ref map() with @ref MapAccess::ReadOnly.
         * @requires_gl @ref MapAccess::ReadOnly is not available in OpenGL ES;
         *      buffer mapping is not available in WebGL.
         */
        const char* mapRead() { return map(MapAccess::ReadOnly); }
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @overload
         * @deprecated Use non-templated @ref map() and cast the result
         *      manually instead.
         */
        template<class T> CORRADE_DEPRECATED("use non-templated map() and cast the result manually instead") T* map(MapAccess access) {
            return reinterpret_cast<T*>(map(access));
        }
        #endif

        /**
         * @brief Map buffer to client memory
         * @param offset    Byte offset into the buffer
         * @param length    Length of the mapped memory in bytes
         * @param flags     Flags. At least @ref MapFlag::Read or
         *      @ref MapFlag::Write must be specified.
         * @return Sized view to buffer data or `nullptr` on error
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref mapRead(), @ref minMapAlignment(), @ref flushMappedRange(),
         *      @ref unmap(), @ref map(MapAccess), @ref setTargetHint(),
         *      @fn_gl2{MapNamedBufferRange,MapBufferRange},
         *      @fn_gl_extension{MapNamedBufferRange,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{MapBufferRange}
         * @requires_gl30 Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 Extension @extension{EXT,map_buffer_range} in
         *      OpenGL ES 2.0.
         * @requires_gles Buffer mapping is not available in WebGL.
         */
        Containers::ArrayView<char> map(GLintptr offset, GLsizeiptr length, MapFlags flags);

        /**
         * @brief Map buffer read-only to client memory
         *
         * Equivalent to @ref map() with @ref MapFlag::Read added implicitly.
         */
        Containers::ArrayView<const char> mapRead(GLintptr offset, GLsizeiptr length, MapFlags flags = {}) {
            return map(offset, length, flags|MapFlag::Read);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @overload
         * @deprecated Use non-templated @ref map() and @ref Containers::arrayCast()
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use non-templated map() and Containers::arrayCast() instead") T* map(GLintptr offset, GLsizeiptr length, MapFlags flags) {
            return Containers::arrayCast<T>(map(offset, length, flags));
        }
        #endif

        /**
         * @brief Flush mapped range
         * @param offset    Byte offset relative to start of mapped range
         * @param length    Length of the flushed memory in bytes
         * @return Reference to self (for method chaining)
         *
         * Flushes specified subsection of mapped range. Use only if you called
         * @ref map() with @ref MapFlag::FlushExplicit flag. See
         * @ref Buffer-data-mapping "class documentation" for usage example.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the buffer is bound to hinted target before the operation
         * (if not already).
         * @see @ref setTargetHint(), @fn_gl2{FlushMappedNamedBufferRange,FlushMappedBufferRange},
         *      @fn_gl_extension{FlushMappedNamedBufferRange,EXT,direct_state_access},
         *      eventually @fn_gl{BindBuffer} and @fn_gl{FlushMappedBufferRange}
         * @requires_gl30 Extension @extension{ARB,map_buffer_range}
         * @requires_gles30 Extension @extension{EXT,map_buffer_range} in
         *      OpenGL ES 2.0.
         * @requires_gles Buffer mapping is not available in WebGL.
         */
        Buffer& flushMappedRange(GLintptr offset, GLsizeiptr length);

        /**
         * @brief Unmap buffer
         * @return `False` if the data have become corrupt during the time
         *      the buffer was mapped (e.g. after screen was resized), `true`
         *      otherwise.
         *
         * Unmaps buffer previously mapped with @ref map() / @ref mapRead(),
         * invalidating the pointer returned by these functions. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the buffer is bound to hinted target before the operation (if not
         * already).
         * @see @ref setTargetHint(), @fn_gl2{UnmapNamedBuffer,UnmapBuffer},
         *      @fn_gl_extension{UnmapNamedBuffer,EXT,direct_state_access},
         *      eventually  @fn_gl{BindBuffer} and @fn_gl{UnmapBuffer}
         * @requires_gles30 Extension @extension{OES,mapbuffer} in OpenGL
         *      ES 2.0.
         * @requires_gles Buffer mapping is not available in WebGL.
         */
        bool unmap();
        #endif

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        /* There should be no need to use these from user code. Also it's a bit
           unfortunate to have the parameter typed as TargetHint while in this
           case it is no hint at all, but it allows to have cleaner public
           binding API (just with short Target name) */
        static void unbindInternal(TargetHint target) { bindInternal(target, nullptr); }
        void bindInternal(TargetHint target) { bindInternal(target, this); }

    private:
        static void bindInternal(TargetHint hint, Buffer* buffer);
        TargetHint MAGNUM_LOCAL bindSomewhereInternal(TargetHint hint);

        #ifndef MAGNUM_TARGET_GLES2
        static void MAGNUM_LOCAL bindImplementationFallback(Target target, GLuint firstIndex, Containers::ArrayView<Buffer* const> buffers);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL bindImplementationMulti(Target target, GLuint firstIndex, Containers::ArrayView<Buffer* const> buffers);
        #endif

        static void MAGNUM_LOCAL bindImplementationFallback(Target target, GLuint firstIndex, Containers::ArrayView<const std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL bindImplementationMulti(Target target, GLuint firstIndex, Containers::ArrayView<const std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers);
        #endif

        static void MAGNUM_LOCAL copyImplementationDefault(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL copyImplementationDSA(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        static void MAGNUM_LOCAL copyImplementationDSAEXT(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        #endif
        #endif

        explicit Buffer(GLuint id, TargetHint targetHint, ObjectFlags flags) noexcept: _id{id}, _targetHint{targetHint}, _flags{flags} {}

        void MAGNUM_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationDSA();
        #endif

        void MAGNUM_LOCAL createIfNotAlready();

        #ifndef MAGNUM_TARGET_WEBGL
        Buffer& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        #if !defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_BUILD_DEPRECATED)
        CORRADE_DEPRECATED("used only by deprecated subData<T>()") void subDataInternal(GLintptr offset, GLsizeiptr size, GLvoid* data);
        #endif

        void MAGNUM_LOCAL getParameterImplementationDefault(GLenum value, GLint* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getParameterImplementationDSA(GLenum value, GLint* data);
        void MAGNUM_LOCAL getParameterImplementationDSAEXT(GLenum value, GLint* data);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getSubDataImplementationDefault(GLintptr offset, GLsizeiptr size, GLvoid* data);
        void MAGNUM_LOCAL getSubDataImplementationDSA(GLintptr offset, GLsizeiptr size, GLvoid* data);
        void MAGNUM_LOCAL getSubDataImplementationDSAEXT(GLintptr offset, GLsizeiptr size, GLvoid* data);
        #endif

        void MAGNUM_LOCAL dataImplementationDefault(GLsizeiptr size, const GLvoid* data, BufferUsage usage);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL dataImplementationDSA(GLsizeiptr size, const GLvoid* data, BufferUsage usage);
        void MAGNUM_LOCAL dataImplementationDSAEXT(GLsizeiptr size, const GLvoid* data, BufferUsage usage);
        #endif

        void MAGNUM_LOCAL subDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subDataImplementationDSA(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        void MAGNUM_LOCAL subDataImplementationDSAEXT(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL invalidateImplementationNoOp();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationARB();
        #endif

        void MAGNUM_LOCAL invalidateSubImplementationNoOp(GLintptr offset, GLsizeiptr length);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateSubImplementationARB(GLintptr offset, GLsizeiptr length);
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL * mapImplementationDefault(MapAccess access);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL * mapImplementationDSA(MapAccess access);
        void MAGNUM_LOCAL * mapImplementationDSAEXT(MapAccess access);
        #endif

        void MAGNUM_LOCAL * mapRangeImplementationDefault(GLintptr offset, GLsizeiptr length, MapFlags access);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL * mapRangeImplementationDSA(GLintptr offset, GLsizeiptr length, MapFlags access);
        void MAGNUM_LOCAL * mapRangeImplementationDSAEXT(GLintptr offset, GLsizeiptr length, MapFlags access);
        #endif

        void MAGNUM_LOCAL flushMappedRangeImplementationDefault(GLintptr offset, GLsizeiptr length);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL flushMappedRangeImplementationDSA(GLintptr offset, GLsizeiptr length);
        void MAGNUM_LOCAL flushMappedRangeImplementationDSAEXT(GLintptr offset, GLsizeiptr length);
        #endif

        bool MAGNUM_LOCAL unmapImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        bool MAGNUM_LOCAL unmapImplementationDSA();
        bool MAGNUM_LOCAL unmapImplementationDSAEXT();
        #endif
        #endif

        GLuint _id;
        TargetHint _targetHint;
        ObjectFlags _flags;
};

#ifndef MAGNUM_TARGET_WEBGL
CORRADE_ENUMSET_OPERATORS(Buffer::MapFlags)
#endif

/** @debugoperatorclassenum{Magnum::Buffer,Magnum::Buffer::TargetHint} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Buffer::TargetHint value);

#if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_BUILD_DEPRECATED)
/** @debugoperatorclassenum{Magnum::Buffer,Magnum::Buffer::Target} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Buffer::Target value);
#endif

inline Buffer::Buffer(NoCreateT) noexcept: _id{0}, _targetHint{TargetHint::Array}, _flags{ObjectFlag::DeleteOnDestruction} {}

inline Buffer::Buffer(Buffer&& other) noexcept: _id{other._id}, _targetHint{other._targetHint}, _flags{other._flags} {
    other._id = 0;
}

inline Buffer& Buffer::operator=(Buffer&& other) noexcept {
    using std::swap;
    swap(_id, other._id);
    swap(_targetHint, other._targetHint);
    swap(_flags, other._flags);
    return *this;
}

inline GLuint Buffer::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

#if !defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_BUILD_DEPRECATED)
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
template<class T> Containers::Array<T> inline Buffer::data() {
    const Int bufferSize = size();
    CORRADE_ASSERT(bufferSize%sizeof(T) == 0, "Buffer::data(): the buffer size is" << bufferSize << "bytes, which can't be expressed as array of types with size" << sizeof(T), nullptr);
    return subData<T>(0, bufferSize/sizeof(T));
}

template<class T> Containers::Array<T> inline Buffer::subData(const GLintptr offset, const GLsizeiptr size) {
    Containers::Array<T> data(size);
    if(size) subDataInternal(offset, size*sizeof(T), data);
    return data;
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif

}

#endif
