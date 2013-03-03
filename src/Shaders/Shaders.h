#ifndef Magnum_Shaders_Shader_h
#define Magnum_Shaders_Shader_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Forward declarations for Magnum::Shaders namespace
 */

#include "Types.h"

namespace Magnum { namespace Shaders {

/** @todoc remove when doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
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
