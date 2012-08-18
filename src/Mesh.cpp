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
#include "Buffer.h"

using namespace std;

namespace Magnum {

Mesh::Mesh(Mesh&& other):
    #ifndef MAGNUM_TARGET_GLES
    vao(other.vao),
    #endif
    _primitive(other._primitive), _vertexCount(other._vertexCount), finalized(other.finalized), _buffers(other._buffers), _attributes(other._attributes)
{
    #ifndef MAGNUM_TARGET_GLES
    other.vao = 0;
    #endif
}

void Mesh::destroy() {
    for(auto& it: _buffers)
        delete it.first;

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
    _buffers = other._buffers;
    _attributes = other._attributes;

    #ifndef MAGNUM_TARGET_GLES
    other.vao = 0;
    #endif

    return *this;
}

Buffer* Mesh::addBuffer(BufferType interleaved) {
    Buffer* buffer = new Buffer(Buffer::Target::Array);
    _buffers.insert(make_pair(buffer, make_pair(interleaved, vector<Attribute>())));

    return buffer;
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
    for(set<GLuint>::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it)
        glDisableVertexAttribArray(*it);
    #endif
}

void Mesh::finalize() {
    /* Already finalized */
    if(finalized) return;

    CORRADE_ASSERT(_vertexCount, "Mesh: the mesh has zero vertex count!", );

    /* Finalize attribute positions for every buffer */
    for(auto& it: _buffers) {
        /* Avoid confustion */
        bool interleaved = it.second.first == BufferType::Interleaved;
        vector<Attribute>& attributes = it.second.second;

        /* Interleaved buffer, set stride and position of first attribute */
        if(interleaved) {
            /* Set attribute position and compute stride */
            GLsizei stride = 0;
            for(vector<Attribute>::iterator ait = attributes.begin(); ait != attributes.end(); ++ait) {
                /* The attribute is positioned at the end of previous */
                ait->pointer = reinterpret_cast<const GLvoid*>(stride);

                /* Add attribute size (per vertex) to stride */
                stride += ait->size*TypeInfo::sizeOf(ait->type);
            }

            /* Set computed stride for all attributes */
            for(vector<Attribute>::iterator ait = attributes.begin(); ait != attributes.end(); ++ait)
                ait->stride = stride;

        /* Non-interleaved buffer, set position of every attribute */
        } else {
            /* Set attribute position */
            GLsizei position = 0;
            for(vector<Attribute>::iterator ait = attributes.begin(); ait != attributes.end(); ++ait) {
                /* The attribute is positioned at the end of previous attribute array */
                ait->pointer = reinterpret_cast<const GLvoid*>(position);

                /* Add attribute size (for all vertices) to position */
                position += ait->size*TypeInfo::sizeOf(ait->type)*_vertexCount;
            }
        }
    }

    /* Mesh is now finalized, attribute binding is not allowed */
    finalized = true;

    #ifndef MAGNUM_TARGET_GLES
    bindBuffers();
    #endif
}

void Mesh::bindBuffers() {
    /* Enable vertex arrays for all attributes */
    for(set<GLuint>::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it)
        glEnableVertexAttribArray(*it);

    for(auto& it: _buffers) {
        /* Avoid confusion */
        vector<Attribute>& attributes = it.second.second;

        /* Bind buffer */
        it.first->bind();

        /* Bind all attributes to this buffer */
        for(vector<Attribute>::const_iterator ait = attributes.begin(); ait != attributes.end(); ++ait)
            #ifndef MAGNUM_TARGET_GLES
            if(TypeInfo::isIntegral(ait->type))
                glVertexAttribIPointer(ait->attribute, ait->size, static_cast<GLenum>(ait->type), ait->stride, ait->pointer);
            else
            #endif
                glVertexAttribPointer(ait->attribute, ait->size, static_cast<GLenum>(ait->type), GL_FALSE, ait->stride, ait->pointer);
    }
}
#endif

void Mesh::bindAttribute(Buffer* buffer, GLuint attribute, GLint size, Type type) {
    /* The mesh is finalized or attribute is already bound, nothing to do */
    if(finalized || _attributes.find(attribute) != _attributes.end()) return;

    /* If buffer is not managed by this mesh, nothing to do */
    auto found = _buffers.find(buffer);
    if(found == _buffers.end()) return;

    Attribute a;
    a.attribute = attribute;
    a.size = size;
    a.type = type;
    a.stride = 0;
    a.pointer = nullptr;

    found->second.second.push_back(a);
    _attributes.insert(attribute);
}

}
