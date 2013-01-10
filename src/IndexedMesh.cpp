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
    if(!_indexCount) return;

    bind();

    glDrawElements(static_cast<GLenum>(primitive()), _indexCount, static_cast<GLenum>(_indexType), nullptr);

    unbind();
}

void IndexedMesh::bind() {
    CORRADE_ASSERT(!_indexCount || _indexBuffer, "IndexedMesh: index buffer must be added if index count is non-zero", );

    Mesh::bind();
    (this->*bindIndexedImplementation)();
}

void IndexedMesh::initializeContextBasedFunctionality(Context* context) {
    /** @todo VAOs are in ES 3.0 and as extension in ES 2.0, enable them when some extension wrangler is available */
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>()) {
        Debug() << "IndexedMesh: using" << Extensions::GL::APPLE::vertex_array_object::string() << "features";

        bindIndexBufferImplementation = &IndexedMesh::bindIndexBufferImplementationVAO;
        bindIndexedImplementation = &IndexedMesh::bindIndexedImplementationVAO;
    }
    #else
    static_cast<void>(context);
    #endif
}

void IndexedMesh::bindIndexBufferImplementationDefault() {}

void IndexedMesh::bindIndexBufferImplementationVAO() {
    bindVAO(vao);
    _indexBuffer->bind(Buffer::Target::ElementArray);
}

void IndexedMesh::bindIndexedImplementationDefault() {
    if(_indexBuffer) _indexBuffer->bind(Buffer::Target::ElementArray);
    else Buffer::unbind(Buffer::Target::ElementArray);
}

void IndexedMesh::bindIndexedImplementationVAO() {}

}
