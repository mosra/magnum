#ifndef Magnum_MeshTools_FullScreenTriangle_h
#define Magnum_MeshTools_FullScreenTriangle_h
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Function @ref Magnum::MeshTools::fullScreenTriangle()
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/GL.h"
#include "Magnum/MeshTools/visibility.h"

namespace Magnum { namespace MeshTools {

/**
@brief Full screen triangle mesh

Returns a pre-configured mesh along with vertex buffer which can be used for
full-screen post-processing effects. The mesh is a single triangle covering
whole screen area (@f$ (-1, -1) - (1, 1) @f$ in both dimensions) and provides
only vertex positions, as other attributes (such as texture coordinates) can be
calculated from them. The vertex positions are, in order:
@f[
    \begin{pmatrix} -1 \\ 1 \end{pmatrix},
    \begin{pmatrix} -1 \\ -3 \end{pmatrix},
    \begin{pmatrix} 3 \\ 1 \end{pmatrix}
@f]

Based on the @p version parameter, on OpenGL 2.1, OpenGL ES 2.0 and WebGL 1 the
vertex positions are passed explicitly as attribute @cpp 0 @ce, contained in
a vertex buffer owned by the mesh. On OpenGL 3.0+, OpenGL ES 3.0+ and WebGL 2
the mesh is attribute-less and the vertex positions can be calculated using the
@glsl gl_VertexID @ce builtin shader variable.

Calculating positions in the shader in a portable way can be done like this.
For OpenGL 2.1 and OpenGL ES 2.0 you then need to bind location of the
`position` attribute to @cpp 0 @ce.

@code{.glsl}
#if (!defined(GL_ES) && __VERSION__ >= 130) || (defined(GL_ES) && __VERSION__ >= 300)
#define NEW_GLSL
#endif

#ifndef NEW_GLSL
attribute lowp vec4 position;
#endif

void main() {
    #ifdef NEW_GLSL
    gl_Position = vec4((gl_VertexID == 2) ?  3.0 : -1.0,
                       (gl_VertexID == 1) ? -3.0 :  1.0, 0.0, 1.0);
    #else
    gl_Position = position;
    #endif
}
@endcode

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
GL::Mesh MAGNUM_MESHTOOLS_EXPORT fullScreenTriangle(GL::Version version);

/** @overload

This function implicitly uses current context version.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
GL::Mesh MAGNUM_MESHTOOLS_EXPORT fullScreenTriangle();
#else
#error this header is available only in the OpenGL build
#endif

}}

#endif
