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

/* The __EMSCRIPTEN_major__ etc macros used to be passed implicitly, version
   3.1.4 moved them to a version header and version 3.1.23 dropped the
   backwards compatibility. To work consistently on all versions, including the
   header only if the version macros aren't present.
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#include <Corrade/configure.h>
#if defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(__EMSCRIPTEN_major__)
#include <emscripten/version.h>
#endif

/* Don't compile this file on Emscripten < 2.0.17. Because it's easier to do
   that here than through CMake. */
#if !defined(__EMSCRIPTEN_major__) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20017
#include "BufferData.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/GL/Buffer.h"

#if !defined(MAGNUM_TARGET_WEBGL) && (defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_BUILD_DEPRECATED))
#include <Corrade/Utility/Algorithms.h>
#endif

namespace Magnum { namespace DebugTools {

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_WEBGL)
namespace Implementation {

/* Used only by deprecated bufferSubData<T>() */
/** @todo remove when not used anymore */
void bufferSubData(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size, void* output) {
    Utility::copy(buffer.mapRead(offset, size), Containers::ArrayView<char>{static_cast<char*>(output), std::size_t(size)});
    buffer.unmap();
}

}
#endif

Containers::Array<char> bufferSubData(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size) {
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_WEBGL)
    return buffer.subData(offset, size);
    #else
    Containers::Array<char> data{NoInit, std::size_t(size)};
    if(size) {
        Utility::copy(buffer.mapRead(offset, size), data);
        buffer.unmap();
    }
    return data;
    #endif
}

Containers::Array<char> bufferData(GL::Buffer& buffer) {
    return bufferSubData(buffer, 0, buffer.size());
}

}}
#endif
