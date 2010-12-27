/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "IndexedMesh.h"

using namespace std;

namespace Magnum {

void IndexedMesh::draw() {
    /* Finalize the mesh, if it is not already */
    finalize();

    /* Enable vertex arrays for all attributes */
    for(set<GLuint>::const_iterator it = attributes().begin(); it != attributes().end(); ++it)
        glEnableVertexAttribArray(*it);

    /* Bind attributes to vertex buffers */
    for(map<Buffer*, pair<bool, vector<Attribute> > >::const_iterator it = buffers().begin(); it != buffers().end(); ++it) {
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
                    glVertexAttribIPointer(ait->location, ait->size, ait->type, ait->stride, ait->pointer);
                    break;
                default:
                    glVertexAttribPointer(ait->location, ait->size, ait->type, GL_FALSE, ait->stride, ait->pointer);
            }

        /* Unbind buffer */
        it->first->unbind();
    }

    /* Bind index array, draw the elements and unbind */
    _indexBuffer.bind();
    glDrawElements(primitive(), indexCount, indexType, 0);
    _indexBuffer.unbind();

    /* Disable vertex arrays for all attributes */
    for(set<GLuint>::const_iterator it = attributes().begin(); it != attributes().end(); ++it)
        glDisableVertexAttribArray(*it);
}

}
