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

#include "TexturedTriangle.h"

#include "Buffer.h"

namespace Magnum { namespace Examples {

TexturedTriangle::TexturedTriangle(const std::string& textureFilename, Object* parent): Object(parent), mesh(Mesh::Triangles, 3), texture(textureFilename) {
    /* Vertices and texture coordinates, interleaved */
    GLfloat data[] = {
        -0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 0.0f,  /* Lower left vertex */
        0.5f, -0.5f, 0.0f, 1.0f,    1.0f, 0.0f,  /* Lower right vertex */
        0.0f, 0.5f, 0.0f, 1.0f,     0.5f, 1.0f,  /* Top vertex */
    };

    /* Fill the mesh with data */
    Buffer* buffer = mesh.addBuffer(true);
    buffer->setData(sizeof(data), data, Buffer::DrawStatic);

    /* Bind attributes (first vertex data, then color data) */
    mesh.bindAttribute<Vector4>(buffer, TexturedIdentityShader::Vertex);
    mesh.bindAttribute<Vector2>(buffer, TexturedIdentityShader::TextureCoordinates);

    /* Texture */
    texture.setMagnificationFilter(TGATexture::Linear);
    texture.setMinificationFilter(TGATexture::Linear);
    texture.setWrapping(Math::Vector2<TGATexture::Wrapping>(TGATexture::ClampToEdge, TGATexture::ClampToEdge));
}

void TexturedTriangle::draw(const Matrix4& transformationMatrix, const Matrix4& projectionMatrix) {
    texture.bind();
    shader.use();
    shader.setTextureUniform(0);
    mesh.draw();
}

}}
