#ifndef Magnum_Shaders_VertexColor_h
#define Magnum_Shaders_VertexColor_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Typedef @ref Magnum::Shaders::VertexColor, alias @ref Magnum::Shaders::VertexColor2D, @ref Magnum::Shaders::VertexColor3D
 * @m_deprecated_since_latest Use @ref Magnum/Shaders/VertexColorGL.h, the
 *      @ref Magnum::Shaders::VertexColorGL "VertexColorGL" class and
 *      related typedefs instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/VertexColorGL.h"

CORRADE_DEPRECATED_FILE("use Magnum/Shaders/VertexColorGL.h, the VertexColorGL class and related typedefs instead")

namespace Magnum { namespace Shaders {

/** @brief @copybrief VertexColorGL
 * @m_deprecated_since_latest Use @ref VertexColorGL instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using VertexColor CORRADE_DEPRECATED_ALIAS("use VertexColorGL instead") = VertexColorGL<dimensions>;
#endif

/** @brief @copybrief VertexColorGL2D
 * @m_deprecated_since_latest Use @ref VertexColorGL2D instead.
 */
typedef CORRADE_DEPRECATED("use VertexColorGL2D instead") VertexColorGL2D VertexColor2D;

/** @brief @copybrief VertexColorGL3D
 * @m_deprecated_since_latest Use @ref VertexColorGL3D instead.
 */
typedef CORRADE_DEPRECATED("use VertexColorGL3D instead") VertexColorGL3D VertexColor3D;

}}
#else
#error use Magnum/Shaders/VertexColorGL.h, the VertexColorGL class and related typedefs instead
#endif

#endif
