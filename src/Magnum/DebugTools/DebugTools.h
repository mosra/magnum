#ifndef Magnum_DebugTools_DebugTools_h
#define Magnum_DebugTools_DebugTools_h
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

/** @file
 * @brief Forward declarations for the @ref Magnum::DebugTools namespace
 */

#include "Magnum/Types.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace DebugTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifdef MAGNUM_BUILD_DEPRECATED
class CORRADE_DEPRECATED("use FrameProfiler instead") Profiler;
#endif
class FrameProfiler;

#ifdef MAGNUM_TARGET_GL
class FrameProfilerGL;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use FrameProfilerGL instead") FrameProfilerGL GLFrameProfiler;
#endif

template<UnsignedInt> class ForceRenderer;
typedef ForceRenderer<2> ForceRenderer2D;
typedef ForceRenderer<3> ForceRenderer3D;
class ForceRendererOptions;

template<UnsignedInt> class ObjectRenderer;
typedef ObjectRenderer<2> ObjectRenderer2D;
typedef ObjectRenderer<3> ObjectRenderer3D;
class ObjectRendererOptions;

class ResourceManager;
#endif
#endif

}}

#endif
