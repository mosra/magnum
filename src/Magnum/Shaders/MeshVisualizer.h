#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
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
 * @brief Typedef @ref Magnum::Shaders::MeshVisualizer2D, @ref Magnum::Shaders::MeshVisualizer3D
 * @m_deprecated_since_latest Use @ref Magnum/Shaders/MeshVisualizerGL.h and
 *      the @ref Magnum::Shaders::MeshVisualizerGL2D "MeshVisualizerGL2D" /
 *      @ref Magnum::Shaders::MeshVisualizerGL3D "MeshVisualizerGL3D" class
 *      instead
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/MeshVisualizerGL.h"

CORRADE_DEPRECATED_FILE("use Magnum/Shaders/MeshVisualizerGL.h and the MeshVisualizerGL2D / MeshVisualizer3D class instead")

namespace Magnum { namespace Shaders {

/** @brief @copybrief MeshVisualizerGL2D
 * @m_deprecated_since_latest Use @ref MeshVisualizerGL2D instead.
 */
typedef CORRADE_DEPRECATED("use MeshVisualizerGL2D instead") MeshVisualizerGL2D MeshVisualizer2D;

/** @brief @copybrief MeshVisualizerGL3D
 * @m_deprecated_since_latest Use @ref MeshVisualizerGL3D instead.
 */
typedef CORRADE_DEPRECATED("use MeshVisualizerGL3D instead") MeshVisualizerGL3D MeshVisualizer3D;

/** @brief @copybrief MeshVisualizerGL3D
 * @m_deprecated_since{2020,06} Use @ref MeshVisualizerGL3D instead.
 */
typedef CORRADE_DEPRECATED("use MeshVisualizerGL3D instead") MeshVisualizerGL3D MeshVisualizer;

}}
#else
#error use Magnum/Shaders/MeshVisualizerGL.h and the MeshVisualizerGL2D / MeshVisualizer3D class instead
#endif

#endif
