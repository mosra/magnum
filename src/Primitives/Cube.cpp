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

#include "Cube.h"

#include "Buffer.h"
#include "IndexedMesh.h"

namespace Magnum { namespace Primitives {

const Vector4 Cube::_vertices[] = {
    Vector4(-1.0f, -1.0f, -1.0f),
    Vector4( 1.0f, -1.0f, -1.0f),
    Vector4(-1.0f,  1.0f, -1.0f),
    Vector4( 1.0f,  1.0f, -1.0f),
    Vector4(-1.0f, -1.0f,  1.0f),
    Vector4( 1.0f, -1.0f,  1.0f),
    Vector4(-1.0f,  1.0f,  1.0f),
    Vector4( 1.0f,  1.0f,  1.0f)
};

const GLubyte Cube::_indices[] = {
    0, 1, 2,
    2, 1, 3,
    1, 5, 3,
    3, 5, 7,
    5, 4, 7,
    7, 4, 6,
    4, 0, 6,
    6, 0, 2,
    2, 3, 7,
    2, 7, 6,
    4, 5, 1,
    4, 1, 0
};

void Cube::build(IndexedMesh* mesh, Buffer* vertexBuffer) {
    prepareMesh(mesh);

    vertexBuffer->setData(sizeof(_vertices), _vertices, Buffer::DrawStatic);
    mesh->indexBuffer()->setData(sizeof(_indices), _indices, Buffer::DrawStatic);
}

}}
