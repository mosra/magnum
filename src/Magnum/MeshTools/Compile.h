#ifndef Magnum_MeshTools_Compile_h
#define Magnum_MeshTools_Compile_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/GL.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/MeshTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <tuple>
#include <memory>
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Compile 2D mesh data

Configures a mesh for @ref Shaders::Generic2D shader with vertex buffer and
possibly also an index buffer, if the mesh is indexed. Positions are bound to
@ref Shaders::Generic2D::Position attribute. If the mesh contains texture
coordinates, these are bound to @ref Shaders::Generic2D::TextureCoordinates
attribute. If the mesh contains colors, these are bound to @ref Shaders::Generic3D::Color4
attribute. No data compression or index optimization (except for index buffer
packing) is done, both the vertex buffer and the index buffer (if any) is owned
by the mesh, both created with @ref GL::BufferUsage::StaticDraw.

This is just a convenience function for creating generic meshes, you might want
to use @ref interleave() and @ref compressIndices() functions together with
@ref GL::Mesh::setPrimitive(), @ref GL::Mesh::setCount(),
@ref GL::Mesh::addVertexBuffer(), @ref GL::Mesh::setIndexBuffer() instead for
greater flexibility.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref shaders-generic
*/
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData2D& meshData);

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief compile(const Trade::MeshData2D&)
 * @deprecated Use @ref compile(const Trade::MeshData2D&) instead. The @p usage
 *      parameter is ignored and returned buffer instances are empty.
 */
CORRADE_DEPRECATED("use compile(const Trade::MeshData2D&) instead") MAGNUM_MESHTOOLS_EXPORT std::tuple<GL::Mesh, std::unique_ptr<GL::Buffer>, std::unique_ptr<GL::Buffer>> compile(const Trade::MeshData2D& meshData, GL::BufferUsage usage);
#endif

/**
@brief Compile 3D mesh data

Configures mesh for @ref Shaders::Generic3D shader with vertex buffer and
possibly also index buffer, if the mesh is indexed. Positions are bound to
@ref Shaders::Generic3D::Position attribute. If the mesh contains normals, they
are bound to @ref Shaders::Generic3D::Normal attribute, texture coordinates are
bound to @ref Shaders::Generic3D::TextureCoordinates attribute. If the mesh
contains colors, they are bound to @ref Shaders::Generic3D::Color4 attribute.
No data compression or index optimization (except for index buffer packing) is
done, both the vertex buffer and the index buffer (if any) is owned by the mesh,
both created with @ref GL::BufferUsage::StaticDraw.

This is just a convenience function for creating generic meshes, you might want
to use @ref interleave() and @ref compressIndices() functions together with
@ref GL::Mesh::setPrimitive(), @ref GL::Mesh::setCount(),
@ref GL::Mesh::addVertexBuffer(), @ref GL::Mesh::setIndexBuffer() instead for
greater flexibility.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref shaders-generic
*/
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData3D& meshData);

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief compile(const Trade::MeshData3D&)
 * @deprecated Use @ref compile(const Trade::MeshData3D&) instead. The @p usage
 *      parameter is ignored and returned buffer instances are empty.
 */
CORRADE_DEPRECATED("use compile(const Trade::MeshData3D&) instead") MAGNUM_MESHTOOLS_EXPORT std::tuple<GL::Mesh, std::unique_ptr<GL::Buffer>, std::unique_ptr<GL::Buffer>> compile(const Trade::MeshData3D& meshData, GL::BufferUsage usage);
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
