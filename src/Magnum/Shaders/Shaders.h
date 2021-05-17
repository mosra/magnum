#ifndef Magnum_Shaders_Shader_h
#define Magnum_Shaders_Shader_h
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
 * @brief Forward declarations for the @ref Magnum::Shaders namespace
 */

#include "Magnum/Types.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Shaders {

#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> class DistanceFieldVectorGL;
typedef DistanceFieldVectorGL<2> DistanceFieldVectorGL2D;
typedef DistanceFieldVectorGL<3> DistanceFieldVectorGL3D;
#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> using DistanceFieldVector CORRADE_DEPRECATED_ALIAS("use DistanceFieldVectorGL instead") = DistanceFieldVectorGL<dimensions>;
typedef CORRADE_DEPRECATED("use DistanceFieldVectorGL2D instead") DistanceFieldVectorGL2D DistanceFieldVector2D;
typedef CORRADE_DEPRECATED("use DistanceFieldVectorGL3D instead") DistanceFieldVectorGL3D DistanceFieldVector3D;
#endif

template<UnsignedInt> class FlatGL;
typedef FlatGL<2> FlatGL2D;
typedef FlatGL<3> FlatGL3D;
#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> using Flat CORRADE_DEPRECATED_ALIAS("use FlatGL instead") = FlatGL<dimensions>;
typedef CORRADE_DEPRECATED("use FlatGL2D instead") FlatGL2D Flat2D;
typedef CORRADE_DEPRECATED("use FlatGL3D instead") FlatGL3D Flat3D;
#endif

/* Generic is used only statically */

class MeshVisualizerGL2D;
class MeshVisualizerGL3D;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use MeshVisualizerGL2D instead") MeshVisualizerGL2D MeshVisualizer2D;
typedef CORRADE_DEPRECATED("use MeshVisualizerGL3D instead") MeshVisualizerGL3D MeshVisualizer3D;
typedef CORRADE_DEPRECATED("use MeshVisualizerGL3D instead") MeshVisualizerGL3D MeshVisualizer;
#endif

class PhongGL;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use PhongGL instead") PhongGL Phong;
#endif

template<UnsignedInt> class VectorGL;
typedef VectorGL<2> VectorGL2D;
typedef VectorGL<3> VectorGL3D;
#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> using Vector CORRADE_DEPRECATED_ALIAS("use VectorGL instead") = VectorGL<dimensions>;
typedef CORRADE_DEPRECATED("use VectorGL2D instead") VectorGL2D Vector2D;
typedef CORRADE_DEPRECATED("use VectorGL3D instead") VectorGL3D Vector3D;
#endif

template<UnsignedInt> class VertexColorGL;
typedef VertexColorGL<2> VertexColorGL2D;
typedef VertexColorGL<3> VertexColorGL3D;
#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> using VertexColor CORRADE_DEPRECATED_ALIAS("use VertexColorGL instead") = VertexColorGL<dimensions>;
typedef CORRADE_DEPRECATED("use VertexColorGL2D instead") VertexColorGL2D VertexColor2D;
typedef CORRADE_DEPRECATED("use VertexColorGL3D instead") VertexColorGL3D VertexColor3D;
#endif
#endif

}}

#endif
