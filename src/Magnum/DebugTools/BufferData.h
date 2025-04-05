#ifndef Magnum_DebugTools_BufferData_h
#define Magnum_DebugTools_BufferData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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

#if defined(MAGNUM_TARGET_GL) && !(defined(MAGNUM_TARGET_WEBGL) && (defined(MAGNUM_TARGET_GLES2) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 20017))
/** @file
 * @brief Function @ref Magnum::DebugTools::bufferData(), @ref Magnum::DebugTools::bufferSubData()
 */
#endif

#include "Magnum/configure.h"
/* The __EMSCRIPTEN_major__ etc macros used to be passed implicitly, version
   3.1.4 moved them to a version header and version 3.1.23 dropped the
   backwards compatibility. To work consistently on all versions, including the
   header only if the version macros aren't present.
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#if defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(__EMSCRIPTEN_major__)
#include <emscripten/version.h>
#endif

#if defined(MAGNUM_TARGET_GL) && !(defined(MAGNUM_TARGET_WEBGL) && (defined(MAGNUM_TARGET_GLES2) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 20017))
#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/DebugTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/GL/Buffer.h"
#endif

namespace Magnum { namespace DebugTools {

#ifdef MAGNUM_BUILD_DEPRECATED
namespace Implementation {
    /* Used only by deprecated bufferSubData<T>() */
    /** @todo remove when not used anymore */
    MAGNUM_DEBUGTOOLS_EXPORT void bufferSubData(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size, void* output);
}
#endif

/**
@brief Buffer subdata

Emulates @ref GL::Buffer::subData() call on platforms that don't support it
(such as OpenGL ES) by using @ref GL::Buffer::mapRead() and copying the memory
to a newly-allocated array. On desktop GL and WebGL 2.0 it's just an alias to
@ref GL::Buffer::subData().

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.

@requires_gles30 Extension @gl_extension{EXT,map_buffer_range} in OpenGL ES
    2.0.
@requires_webgl20 Buffer data queries or buffer mapping are not available in
    WebGL 1.0. Emscripten 2.0.17 or higher is required in WebGL2.
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::Array<char> bufferSubData(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_WEBGL)
/**
@copybrief bufferSubData()
@m_deprecated_since_latest Use non-templated @ref bufferSubData() and
    @ref Containers::arrayCast() instead

@requires_gl30 Extension @gl_extension{ARB,map_buffer_range}
@requires_gles30 Extension @gl_extension{EXT,map_buffer_range} in OpenGL ES
    2.0.
@requires_gles Buffer mapping is not available in WebGL.
*/
template<class T> CORRADE_DEPRECATED("use non-templated bufferSubData() and Containers::arrayCast() instead") Containers::Array<T> inline bufferSubData(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /* HELLO FUCKING CRAP TOOL, WHY CAN'T YOU DISTINGUISH A TEMPLATED FUNCTION
       FROM A NON-TEMPLATE, SMASHING THE DOCUMENTATION TOGETHER??! */
    , void* doxygenIsCrapAtOverloadResolution = nullptr
    #endif
) {
    /* Yes, this should have NoInit, but let's preserve the deprecated API in
       its original form */
    Containers::Array<T> data{std::size_t(size)};
    if(size) Implementation::bufferSubData(buffer, offset, size*sizeof(T), data);
    return data;
}
#endif

/**
@brief Buffer data

Emulates @ref GL::Buffer::data() call on platforms that don't support it (such
as OpenGL ES) by using @ref GL::Buffer::mapRead() and copying the memory to a
newly-allocated array. On desktop GL and WebGL 2.0 it's just an alias to
@ref GL::Buffer::data().

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.

@requires_gles30 Extension @gl_extension{EXT,map_buffer_range} in OpenGL ES
    2.0.
@requires_webgl20 Buffer data queries or buffer mapping are not available in
    WebGL 1.0. Emscripten 2.0.17 or higher is required in WebGL2.
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::Array<char> bufferData(GL::Buffer& buffer);

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_WEBGL)
/**
@copybrief bufferData()
@m_deprecated_since_latest Use non-templated @ref bufferData() and
    @ref Containers::arrayCast() instead

@requires_gl30 Extension @gl_extension{ARB,map_buffer_range}
@requires_gles30 Extension @gl_extension{EXT,map_buffer_range} in OpenGL ES
    2.0.
@requires_gles Buffer mapping is not available in WebGL.
*/
template<class T> CORRADE_DEPRECATED("use non-templated bufferData() and Containers::arrayCast() instead") Containers::Array<T> inline bufferData(GL::Buffer& buffer
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /* HELLO FUCKING CRAP TOOL, WHY CAN'T YOU DISTINGUISH A TEMPLATED FUNCTION
       FROM A NON-TEMPLATE, SMASHING THE DOCUMENTATION TOGETHER??! */
    , void* doxygenIsCrapAtOverloadResolution = nullptr
    #endif
) {
    const Int bufferSize = buffer.size();
    /* Yes, the assert prefix is wrong, but let's preserve the deprecated API
       in its original form */
    CORRADE_ASSERT(bufferSize%sizeof(T) == 0, "Buffer::data(): the buffer size is" << bufferSize << "bytes, which can't be expressed as array of types with size" << sizeof(T), nullptr);
    CORRADE_IGNORE_DEPRECATED_PUSH
    return bufferSubData<T>(buffer, 0, bufferSize/sizeof(T));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

}}
#else
#error this header is not available in a WebGL 1.0 build or on Emscripten < 2.0.17
#endif

#endif
