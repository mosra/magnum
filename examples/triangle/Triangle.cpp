/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Triangle.h"

#include "Buffer.h"

namespace Magnum { namespace Examples {

Triangle::Triangle(Object* parent): Object(parent), mesh(Mesh::Triangles, 3) {
    /* Vertices and colors, interleaved */
    Vector4 data[] = {
        Vector4(-0.5f, -0.5f, 0.0f),    Vector4(1.0f, 0.0f, 0.0f),  /* Red lower left vertex */
        Vector4(0.5f, -0.5f, 0.0f),     Vector4(0.0f, 1.0f, 0.0f),  /* Green lower right vertex */
        Vector4(0.0f, 0.5f, 0.0f),      Vector4(0.0f, 0.0f, 1.0f)   /* Blue top vertex */
    };

    /* Fill the mesh with data */
    Buffer* buffer = mesh.addBuffer(true);
    buffer->setData(sizeof(data), data, Buffer::DrawStatic);

    /* Bind attributes (first vertex data, then color data) */
    mesh.bindAttribute<Vector4>(buffer, IdentityShader::Vertex);
    mesh.bindAttribute<Vector4>(buffer, IdentityShader::Color);
}

void Triangle::draw(const Matrix4& transformationMatrix, const Matrix4& projectionMatrix) {
    shader.use();
    mesh.draw();
}

}}
