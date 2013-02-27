#ifndef Magnum_Trade_Trade_h
#define Magnum_Trade_Trade_h
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
 * @brief Forward declarations for Magnum::Trade namespace
 */

#include "Types.h"

namespace Magnum { namespace Trade {

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
class AbstractImporter;
class AbstractMaterialData;
class CameraData;

template<UnsignedInt> class ImageData;
typedef ImageData<1> ImageData1D;
typedef ImageData<2> ImageData2D;
typedef ImageData<3> ImageData3D;

class LightData;
class MeshData2D;
class MeshData3D;
class MeshObjectData2D;
class MeshObjectData3D;
class ObjectData2D;
class ObjectData3D;
class PhongMaterialData;
class SceneData;
class TextureData;
#endif

}}

#endif
