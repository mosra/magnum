#ifndef Magnum_Primitives_Icosphere_h
#define Magnum_Primitives_Icosphere_h
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
 * @brief Function @ref Magnum::Primitives::icosphereSolid()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Solid 3D icosphere
@param subdivisions      Number of subdivisions

Sphere of radius @cpp 1.0f @ce, centered at origin.
@ref MeshPrimitive::Triangles with @ref MeshIndexType::UnsignedInt indices,
interleaved @ref VertexFormat::Vector3 positions and @ref VertexFormat::Vector3
normals.

@image html primitives-icospheresolid.png width=256px

The @p subdivisions parameter describes how many times is each icosphere
triangle subdivided, recursively. Specifying @cpp 0 @ce will result in an
icosphere with 12 vertices and 20 faces, saying @cpp 1 @ce will result in an
icosphere with 80 faces (each triangle subdivided into four smaller), saying
@cpp 2 @ce will result in 320 faces and so on. In particular, this is different
from the `subdivisions` parameter in @ref grid3DSolid() or @ref grid3DWireframe().
@see @ref uvSphereSolid(), @ref uvSphereWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData icosphereSolid(UnsignedInt subdivisions);

/**
@brief Wireframe 3D icosphere
@m_since{2020,06}

Sphere of radius @cpp 1.0f @ce with 12 vertices and 30 edges, centered at
origin. @ref MeshPrimitive::Lines with @ref MeshIndexType::UnsignedShort
indices and @ref VertexFormat::Vector3 positions. The returned instance
references data stored in constant memory.

@image html primitives-icospherewireframe.png width=256px

@see @ref icosphereWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData icosphereWireframe();

}}

#endif
