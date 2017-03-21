#ifndef Magnum_ColorFormat_h
#define Magnum_ColorFormat_h
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
 * @deprecated Use @ref Magnum/PixelFormat.h instead.
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/PixelFormat.h"
CORRADE_DEPRECATED_FILE("use Magnum/PixelFormat.h instead")

namespace Magnum {

/** @copybrief PixelFormat
 * @deprecated Use @ref PixelFormat instead.
 */
typedef CORRADE_DEPRECATED("use PixelFormat instead") PixelFormat ColorFormat;

/** @copybrief PixelType
 * @deprecated Use @ref PixelType instead.
 */
typedef CORRADE_DEPRECATED("use PixelType instead") PixelType ColorType;

/** @copybrief CompressedPixelFormat
 * @deprecated Use @ref CompressedPixelFormat instead.
 */
typedef CORRADE_DEPRECATED("use CompressedPixelFormat instead") CompressedPixelFormat CompressedColorFormat;

}
#else
#error use Magnum/PixelFormat.h instead
#endif

#endif
