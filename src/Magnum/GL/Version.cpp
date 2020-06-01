/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Version.h"

#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace GL {

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const Version value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value, string) case Version::value: return debug << (string);
        _c(None, "None")
        #ifndef MAGNUM_TARGET_GLES
        _c(GL210, "OpenGL 2.1")
        _c(GL300, "OpenGL 3.0")
        _c(GL310, "OpenGL 3.1")
        _c(GL320, "OpenGL 3.2")
        _c(GL330, "OpenGL 3.3")
        _c(GL400, "OpenGL 4.0")
        _c(GL410, "OpenGL 4.1")
        _c(GL420, "OpenGL 4.2")
        _c(GL430, "OpenGL 4.3")
        _c(GL440, "OpenGL 4.4")
        _c(GL450, "OpenGL 4.5")
        _c(GL460, "OpenGL 4.6")
        #endif
        #ifdef MAGNUM_TARGET_WEBGL
        _c(GLES200, "WebGL 1.0")
        _c(GLES300, "WebGL 2.0")
        #else
        _c(GLES200, "OpenGL ES 2.0")
        _c(GLES300, "OpenGL ES 3.0")
        _c(GLES310, "OpenGL ES 3.1")
        _c(GLES320, "OpenGL ES 3.2")
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Invalid(" << Debug::nospace << reinterpret_cast<void*>(Int(value)) << Debug::nospace << ")";
}
#endif

}}
