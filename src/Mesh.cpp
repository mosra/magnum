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

#include "Mesh.h"
#include "Buffer.h"

#include <iostream>

using namespace std;

namespace Magnum {

Mesh::~Mesh() {
    for(map<Buffer*, pair<bool, vector<Attribute> > >::iterator it = _buffers.begin(); it != _buffers.end(); ++it)
        delete it->first;
}

Buffer* Mesh::addBuffer(bool interleaved) {
    Buffer* buffer = new Buffer(Buffer::ArrayBuffer);
    _buffers.insert(pair<Buffer*, pair<bool, vector<Attribute> > >(
        buffer,
        pair<bool, vector<Attribute> >(interleaved, vector<Attribute>())
    ));

    return buffer;
}

void Mesh::draw() {
    /* Finalize the mesh, if it is not already */
    finalize();

    /* Enable vertex arrays for all attributes */
    for(set<GLuint>::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it)
        glEnableVertexAttribArray(*it);

    /* Bind attributes to vertex buffers */
    for(map<Buffer*, pair<bool, vector<Attribute> > >::const_iterator it = _buffers.begin(); it != _buffers.end(); ++it) {
        /* Bind buffer */
        it->first->bind();

        /* Bind all attributes to this buffer */
        for(vector<Attribute>::const_iterator ait = it->second.second.begin(); ait != it->second.second.end(); ++ait)
            switch(ait->type) {
                case GL_BYTE:
                case GL_UNSIGNED_BYTE:
                case GL_SHORT:
                case GL_UNSIGNED_SHORT:
                case GL_INT:
                case GL_UNSIGNED_INT:
                    glVertexAttribIPointer(ait->attribute, ait->size, ait->type, ait->stride, ait->pointer);
                    break;
                default:
                    glVertexAttribPointer(ait->attribute, ait->size, ait->type, GL_FALSE, ait->stride, ait->pointer);
            }

        /* Unbind buffer */
        it->first->unbind();
    }

    glDrawArrays(_primitive, 0, _vertexCount);

    /* Disable vertex arrays for all attributes */
    for(set<GLuint>::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it)
        glDisableVertexAttribArray(*it);
}

void Mesh::finalize() {
    /* Already finalized */
    if(finalized) return;

    /* Finalize attribute positions for every buffer */
    for(map<Buffer*, pair<bool, vector<Attribute> > >::iterator it = _buffers.begin(); it != _buffers.end(); ++it) {
        /* Avoid confustion */
        bool interleaved = it->second.first;
        vector<Attribute>& attributes = it->second.second;

        /* Interleaved buffer, set stride and position of first attribute */
        if(interleaved) {
            /* Set attribute position and compute stride */
            GLsizei stride = 0;
            for(vector<Attribute>::iterator ait = attributes.begin(); ait != attributes.end(); ++ait) {
                /* The attribute is positioned at the end of previous */
                ait->pointer = reinterpret_cast<const GLvoid*>(stride);

                /* Add attribute size (per vertex) to stride */
                stride += ait->size*sizeOf(ait->type);
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
                position += ait->size*sizeOf(ait->type)*_vertexCount;
            }
        }
    }

    /* Mesh is now finalized, attribute binding is not allowed */
    finalized = true;
}

void Mesh::bindAttribute(Buffer* buffer, GLuint attribute, GLint size, GLenum type) {
    /* The mesh is finalized or attribute is already bound, nothing to do */
    if(finalized || _attributes.find(attribute) != _attributes.end()) return;

    /* If buffer is not managed by this mesh, nothing to do */
    map<Buffer*, pair<bool, vector<Attribute> > >::iterator found = _buffers.find(buffer);
    if(found == _buffers.end()) return;

    Attribute a;
    a.attribute = attribute;
    a.size = size;
    a.type = type;
    a.stride = 0;
    a.pointer = 0;

    found->second.second.push_back(a);
    _attributes.insert(attribute);
}

GLsizei Mesh::sizeOf(GLenum type) {
    switch(type) {
        case GL_BYTE:           return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
        case GL_SHORT:          return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_INT:            return sizeof(GLint);
        case GL_UNSIGNED_INT:   return sizeof(GLuint);
        case GL_FLOAT:          return sizeof(GLfloat);
        case GL_DOUBLE:         return sizeof(GLdouble);
        default: return 0;
    }
}

}
