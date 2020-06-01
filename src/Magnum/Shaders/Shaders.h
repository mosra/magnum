#ifndef Magnum_Shaders_Shader_h
#define Magnum_Shaders_Shader_h
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

/** @file
 * @brief Forward declarations for the @ref Magnum::Shaders namespace
 */

#include "Magnum/Types.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Shaders {

#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> class DistanceFieldVector;
typedef DistanceFieldVector<2> DistanceFieldVector2D;
typedef DistanceFieldVector<3> DistanceFieldVector3D;

template<UnsignedInt> class AbstractVector;
typedef AbstractVector<2> AbstractVector2D;
typedef AbstractVector<3> AbstractVector3D;

template<UnsignedInt> class Flat;
typedef Flat<2> Flat2D;
typedef Flat<3> Flat3D;

/* Generic is used only statically */

class MeshVisualizer2D;
class MeshVisualizer3D;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use MeshVisualizer3D instead") MeshVisualizer3D MeshVisualizer;
#endif

class Phong;

template<UnsignedInt> class Vector;
typedef Vector<2> Vector2D;
typedef Vector<3> Vector3D;

template<UnsignedInt> class VertexColor;
typedef VertexColor<2> VertexColor2D;
typedef VertexColor<3> VertexColor3D;
#endif

}}

#endif
