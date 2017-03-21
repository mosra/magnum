#ifndef Magnum_ImageReference_h
#define Magnum_ImageReference_h
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
 * @deprecated Use @ref Magnum/ImageView.h instead.
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/ImageView.h"
CORRADE_DEPRECATED_FILE("use Magnum/ImageView.h instead")

namespace Magnum {

/** @copybrief ImageView
 * @deprecated Use @ref ImageView instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using CORRADE_DEPRECATED_ALIAS("use ImageView instead") ImageReference = ImageView<dimensions>;
#endif

/** @copybrief ImageView1D
 * @deprecated Use @ref ImageView1D instead.
 */
typedef CORRADE_DEPRECATED("use ImageView1D instead") ImageView1D ImageReference1D;

/** @copybrief ImageView2D
 * @deprecated Use @ref ImageView2D instead.
 */
typedef CORRADE_DEPRECATED("use ImageView2D instead") ImageView2D ImageReference2D;

/** @copybrief ImageView3D
 * @deprecated Use @ref ImageView3D instead.
 */
typedef CORRADE_DEPRECATED("use ImageView3D instead") ImageView3D ImageReference3D;

}
#else
#error use Magnum/ImageView.h instead
#endif

#endif
