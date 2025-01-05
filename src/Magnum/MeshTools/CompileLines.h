#ifndef Magnum_MeshTools_CompileLines_h
#define Magnum_MeshTools_CompileLines_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2)
/** @file
 * @brief Function @ref Magnum::MeshTools::compileLines()
 * @m_since_latest
 */
#endif

#include "Magnum/configure.h"

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2)
#include "Magnum/GL/GL.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Compile a line mesh for use with @ref Shaders::LineGL
@m_since_latest

Expects that the @p mesh is returned from @ref generateLines(), see its
documentation for more information.

@experimental

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@requires_gles30 @ref Shaders::LineGL requires integer support in shaders which
    is not available in OpenGL ES 2.0, thus neither this function is defined
    in OpenGL ES 2.0 builds.
@requires_webgl20 @ref Shaders::LineGL requires integer support in shaders which
    is not available in WebGL 1.0, thus neither this function is defined in
    WebGL 1.0 builds.
*/
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compileLines(const Trade::MeshData& mesh);

}}
#else
#error this header is available only in the desktop OpenGL, OpenGL ES 3.0+ and WebGL 2.0 builds
#endif

#endif
