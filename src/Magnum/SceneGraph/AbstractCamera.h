#ifndef Magnum_SceneGraph_AbstractCamera_h
#define Magnum_SceneGraph_AbstractCamera_h
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
 * @brief Use @ref Magnum/SceneGraph/Camera.h instead.
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/SceneGraph/Camera.h"
CORRADE_DEPRECATED_FILE("use Magnum/SceneGraph/Camera.h instead")

namespace Magnum { namespace SceneGraph {

/**
 * @copybrief Camera
 * @deprecated Use @ref Camera instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions, class T> using AbstractCamera CORRADE_DEPRECATED_ALIAS("use Camera instead") = Camera<dimensions, T>;
#endif

/**
 * @copybrief BasicCamera2D
 * @deprecated Use @ref BasicCamera2D instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using AbstractBasicCamera2D CORRADE_DEPRECATED_ALIAS("use BasicCamera2D instead") = BasicCamera2D<T>;
#endif

/**
 * @copybrief Camera2D
 * @deprecated Use @ref Camera2D instead.
 */
CORRADE_DEPRECATED("use Camera2D instead") typedef Camera2D AbstractCamera2D;

/**
 * @copybrief BasicCamera3D
 * @deprecated Use @ref BasicCamera3D instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using AbstractBasicCamera3D CORRADE_DEPRECATED_ALIAS("use BasicCamera3D instead") = BasicCamera3D<T>;
#endif

/**
 * @copybrief Camera3D
 * @deprecated Use @ref Camera3D instead.
 */
CORRADE_DEPRECATED("use Camera3D instead") typedef Camera3D AbstractCamera3D;

}}
#else
#error use Magnum/SceneGraph/Camera.h instead
#endif

#endif
