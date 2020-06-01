#ifndef Magnum_Implementation_WindowsWeakSymbol_h
#define Magnum_Implementation_WindowsWeakSymbol_h
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

#include <Corrade/configure.h>

#include "Magnum/configure.h"

/* This is a copy of Corrade/Utility/Implementation/windowsWeakSymbol.{h,cpp},
   just adapted for Magnum. Yeah, it's only in order to avoid including
   windows.h in all the code. */

#if !defined(CORRADE_TARGET_WINDOWS) || !defined(MAGNUM_BUILD_STATIC) || defined(CORRADE_TARGET_WINDOWS_RT)
#error this file is only meant to be used in non-RT Windows static builds
#endif

namespace Magnum { namespace Implementation {

void* windowsWeakSymbol(const char* name, void* backup);

}}

#endif
