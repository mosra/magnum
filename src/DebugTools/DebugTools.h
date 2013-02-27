#ifndef Magnum_DebugTools_DebugTools_h
#define Magnum_DebugTools_DebugTools_h
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
 * @brief Forward declarations for Magnum::DebugTools namespace
 */

#include "Types.h"

namespace Magnum { namespace DebugTools {

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> class ObjectRenderer;
typedef ObjectRenderer<2> ObjectRenderer2D;
typedef ObjectRenderer<3> ObjectRenderer3D;
class ObjectRendererOptions;

class Profiler;
class ResourceManager;

template<UnsignedInt> class ShapeRenderer;
typedef ShapeRenderer<2> ShapeRenderer2D;
typedef ShapeRenderer<3> ShapeRenderer3D;
class ShapeRendererOptions;
#endif

}}

#endif
