#ifndef Magnum_IndexedMesh_h
#define Magnum_IndexedMesh_h
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
 * @brief Class Magnum::IndexedMesh
 */

#include "Mesh.h"

#include "corradeCompatibility.h"

namespace Magnum {

/**
@brief Indexed mesh

@section IndexedMesh-configuration Indexed mesh configuration

Next to @ref Mesh-configuration "everything needed for non-indexed mesh" you
have to call also setIndexCount() and setIndexType(). Then create index buffer
and assign it to the mesh using setIndexBuffer() or use
MeshTools::compressIndices() to conveniently fill the index buffer and set
index count and type.

Similarly as in Mesh itself the index buffer is not managed by the mesh, so
you have to manage it on your own. On the other hand it allows you to use
one index buffer for more meshes (with different vertex data in each mesh, for
example) or store more than only index data in one buffer.

Example usage -- creating a cube mesh, assigning vertex buffer with
interleaved vertex attributes and compressed index buffer for use with
Shaders::PhongShader:
@code
Buffer *vertexBuffer, *indexBuffer;
Mesh* mesh;

Primitives::Cube cube;
MeshTools::interleave(mesh, vertexBuffer, Buffer::Usage::StaticDraw, *cube.positions(0), *cube.normals(0));
MeshTools::compressIndices(mesh, indexBuffer, Buffer::Usage::StaticDraw, *cube.indices());
mesh->setPrimitive(plane.primitive())
    ->addInterleavedVertexBuffer(vertexBuffer, 0, Shaders::PhongShader::Position(), Shaders::PhongShader::Normal());
@endcode

@section IndexedMesh-drawing Rendering meshes

From user point-of-view the operation is the same as for
@ref Mesh-drawing "non-indexed meshes".

@section IndexedMesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object}, OpenGL ES 3.0 or
@es_extension{OES,vertex_array_object} on OpenGL ES 2.0 is supported, next to
@ref Mesh-performance-optimization "optimizations in Mesh itself" the index
buffer is bound on object construction instead of binding it in every draw()
call.
*/
class MAGNUM_EXPORT IndexedMesh: public Mesh {
    friend class Context;

    public:
        /**
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * Creates indexed mesh with zero vertex count, zero index count and
         * no vertex or index buffers.
         */
        inline explicit IndexedMesh(Primitive primitive = Primitive::Triangles): Mesh(primitive) {}
};

}

#endif
