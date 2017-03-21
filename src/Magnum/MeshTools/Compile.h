#ifndef Magnum_MeshTools_Compile_h
#define Magnum_MeshTools_Compile_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

/** @file
 * @brief Function @ref Magnum::MeshTools::compile()
 */

#include <tuple>
#include <memory>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/MeshTools/visibility.h"

namespace Magnum { namespace MeshTools {

/**
@brief Compile 2D mesh data

Configures mesh for @ref Shaders::Generic2D shader with vertex buffer and
possibly also index buffer, if the mesh is indexed. Positions are bound to
@ref Shaders::Generic2D::Position attribute. If the mesh contains texture
coordinates, they are bound to @ref Shaders::Generic2D::TextureCoordinates
attribute. No data compression or index optimization (except for index buffer
packing) is done. The @p usage parameter is used for both vertex and index
buffer.

The second returned buffer may be `nullptr` if the mesh is not indexed.

This is just a convenience function for creating generic meshes, you might want
to use @ref interleave() and @ref compressIndices() functions instead for
greater flexibility.

@see @ref shaders-generic
*/
MAGNUM_MESHTOOLS_EXPORT std::tuple<Mesh, std::unique_ptr<Buffer>, std::unique_ptr<Buffer>> compile(const Trade::MeshData2D& meshData, BufferUsage usage);

/**
@brief Compile 3D mesh data

Configures mesh for @ref Shaders::Generic3D shader with vertex buffer and
possibly also index buffer, if the mesh is indexed. Positions are bound to
@ref Shaders::Generic3D::Position attribute. If the mesh contains normals, they
are bound to @ref Shaders::Generic3D::Normal attribute, texture coordinates are
bound to @ref Shaders::Generic2D::TextureCoordinates attribute. No data
compression or index optimization (except for index buffer packing) is done.
The @p usage parameter is used for both vertex and index buffer.

The second returned buffer may be `nullptr` if the mesh is not indexed.

This is just a convenience function for creating generic meshes, you might want
to use @ref interleave() and @ref compressIndices() functions instead for
greater flexibility.

@see @ref shaders-generic
*/
MAGNUM_MESHTOOLS_EXPORT std::tuple<Mesh, std::unique_ptr<Buffer>, std::unique_ptr<Buffer>> compile(const Trade::MeshData3D& meshData, BufferUsage usage);

}}

#endif
