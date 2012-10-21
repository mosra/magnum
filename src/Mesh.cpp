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

#include "Mesh.h"

#include <Utility/Debug.h>

#include "Buffer.h"

using namespace std;

namespace Magnum {

Mesh::Mesh(Mesh&& other):
    #ifndef MAGNUM_TARGET_GLES
    vao(other.vao),
    #endif
    _primitive(other._primitive), _vertexCount(other._vertexCount), finalized(other.finalized), attributes(other.attributes)
{
    #ifndef MAGNUM_TARGET_GLES
    other.vao = 0;
    #endif
}

void Mesh::destroy() {
    #ifndef MAGNUM_TARGET_GLES
    glDeleteVertexArrays(1, &vao);
    #endif
}

Mesh& Mesh::operator=(Mesh&& other) {
    destroy();

    #ifndef MAGNUM_TARGET_GLES
    vao = other.vao;
    #endif
    _primitive = other._primitive;
    _vertexCount = other._vertexCount;
    finalized = other.finalized;
    attributes = other.attributes;

    #ifndef MAGNUM_TARGET_GLES
    other.vao = 0;
    #endif

    return *this;
}

void Mesh::draw() {
    /* Vertex array must be bound before finalization */
    #ifndef MAGNUM_TARGET_GLES
    bind();
    #endif

    /* Finalize, if not already */
    finalize();

    /* Buffers must be bound after initialization */
    #ifdef MAGNUM_TARGET_GLES
    bind();
    #endif

    /** @todo Start at given index */
    glDrawArrays(static_cast<GLenum>(_primitive), 0, _vertexCount);

    unbind();
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void Mesh::bind() {
    #ifndef MAGNUM_TARGET_GLES
    glBindVertexArray(vao);
    #else
    bindBuffers();
    #endif
}

void Mesh::unbind() {
    #ifndef MAGNUM_TARGET_GLES
    glBindVertexArray(0);
    #else
    for(const Attribute& attribute: attributes)
        glDisableVertexAttribArray(attribute.location);
    #endif
}

void Mesh::finalize() {
    /* Already finalized */
    if(finalized) return;

    CORRADE_ASSERT((_vertexCount == 0) == attributes.empty(), "Mesh: vertex count is non-zero, but no attributes are bound", );

    finalized = true;

    #ifndef MAGNUM_TARGET_GLES
    bindBuffers();
    #endif
}

void Mesh::bindBuffers() {
    /* Bind all attributes to this buffer */
    for(const Attribute& attribute: attributes) {
        glEnableVertexAttribArray(attribute.location);

        attribute.buffer->bind(Buffer::Target::Array);

        #ifndef MAGNUM_TARGET_GLES
        if(TypeInfo::isIntegral(attribute.type))
            glVertexAttribIPointer(attribute.location, attribute.count, static_cast<GLenum>(attribute.type), attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
        else
        #endif
            glVertexAttribPointer(attribute.location, attribute.count, static_cast<GLenum>(attribute.type), GL_FALSE, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    }
}
#endif

void Mesh::addVertexAttribute(Buffer* buffer, GLuint location, GLint count, Type type, GLintptr offset, GLsizei stride) {
    CORRADE_ASSERT(_vertexCount != 0, "Mesh: vertex count must be set before binding attributes", );

    attributes.push_back({
        buffer,
        location,
        count,
        type,
        offset,
        stride
    });
}

}
