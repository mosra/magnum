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

#include "Egl.h"

namespace Magnum { namespace Platform { namespace Implementation {

const char* eglErrorString(const EGLint error) {
    switch(error) {
        #define _error(name) case name: return #name;
        _error(EGL_SUCCESS)
        _error(EGL_NOT_INITIALIZED)
        _error(EGL_BAD_ACCESS)
        _error(EGL_BAD_ALLOC)
        _error(EGL_BAD_ATTRIBUTE)
        _error(EGL_BAD_CONTEXT)
        _error(EGL_BAD_CONFIG)
        _error(EGL_BAD_CURRENT_SURFACE)
        _error(EGL_BAD_DISPLAY)
        _error(EGL_BAD_SURFACE)
        _error(EGL_BAD_MATCH)
        _error(EGL_BAD_PARAMETER)
        _error(EGL_BAD_NATIVE_PIXMAP)
        _error(EGL_BAD_NATIVE_WINDOW)
        _error(EGL_CONTEXT_LOST)
        #undef _error
    }

    return "EGL_(invalid)";
}

}}}
