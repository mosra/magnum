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

#include "IndexedMesh.h"

#include <Utility/Debug.h>

#include "Buffer.h"
#include "Context.h"
#include "Extensions.h"

namespace Magnum {

IndexedMesh::BindIndexBufferImplementation IndexedMesh::bindIndexBufferImplementation = &IndexedMesh::bindIndexBufferImplementationDefault;
IndexedMesh::BindIndexedImplementation IndexedMesh::bindIndexedImplementation = &IndexedMesh::bindIndexedImplementationDefault;

IndexedMesh* IndexedMesh::setIndexBuffer(Buffer* buffer) {
    _indexBuffer = buffer;
    (this->*bindIndexBufferImplementation)();
    return this;
}

void IndexedMesh::draw() {
    bind();

    /** @todo Start at given index */
    glDrawElements(static_cast<GLenum>(primitive()), _indexCount, static_cast<GLenum>(_indexType), nullptr);

    unbind();
}

void IndexedMesh::bind() {
    CORRADE_ASSERT(_indexCount, "IndexedMesh: the mesh has zero index count!", );

    Mesh::bind();
    (this->*bindIndexedImplementation)();
}

void IndexedMesh::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>()) {
        #ifndef MAGNUM_TARGET_GLES
        Debug() << "IndexedMesh: using" << Extensions::GL::APPLE::vertex_array_object::string() << "features";

        bindIndexBufferImplementation = &IndexedMesh::bindIndexBufferImplementationVAO;
        bindIndexedImplementation = &IndexedMesh::bindIndexedImplementationVAO;
        #endif
    }
}

void IndexedMesh::bindIndexBufferImplementationDefault() {}

#ifndef MAGNUM_TARGET_GLES
void IndexedMesh::bindIndexBufferImplementationVAO() {
    glBindVertexArray(vao);

    _indexBuffer->bind(Buffer::Target::ElementArray);
}
#endif

void IndexedMesh::bindIndexedImplementationDefault() {
    _indexBuffer->bind(Buffer::Target::ElementArray);
}

#ifndef MAGNUM_TARGET_GLES
void IndexedMesh::bindIndexedImplementationVAO() {}
#endif

}
