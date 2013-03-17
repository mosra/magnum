#ifndef Magnum_Shaders_Shader_h
#define Magnum_Shaders_Shader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Forward declarations for Magnum::Shaders namespace
 */

#include "Types.h"

namespace Magnum { namespace Shaders {

/** @todoc remove when doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT

template<UnsignedInt> class DistanceFieldVectorShader;
typedef DistanceFieldVectorShader<2> DistanceFieldVectorShader2D;
typedef DistanceFieldVectorShader<3> DistanceFieldVectorShader3D;

template<UnsignedInt> class AbstractVectorShader;
typedef AbstractVectorShader<2> AbstractVectorShader2D;
typedef AbstractVectorShader<3> AbstractVectorShader3D;

template<UnsignedInt> class FlatShader;
typedef FlatShader<2> FlatShader2D;
typedef FlatShader<3> FlatShader3D;

class PhongShader;

template<UnsignedInt> class VectorShader;
typedef VectorShader<2> VectorShader2D;
typedef VectorShader<3> VectorShader3D;

template<UnsignedInt> class VertexColorShader;
typedef VertexColorShader<2> VertexColorShader2D;
typedef VertexColorShader<3> VertexColorShader3D;
#endif

}}

#endif
