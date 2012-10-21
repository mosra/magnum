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

#include "Context.h"
#include "Extensions.h"

namespace Magnum {

IndexedMesh::CreateIndexedImplementation IndexedMesh::createIndexedImplementation = &IndexedMesh::createIndexedImplementationDefault;
IndexedMesh::BindIndexedImplementation IndexedMesh::bindIndexedImplementation = &IndexedMesh::bindIndexedImplementationDefault;

IndexedMesh::IndexedMesh(Mesh::Primitive primitive): Mesh(primitive), _indexCount(0), _indexType(Type::UnsignedShort) {
    _indexBuffer.setTargetHint(Buffer::Target::ElementArray);

    (this->*createIndexedImplementation)();
}

IndexedMesh::IndexedMesh(Mesh::Primitive primitive, GLsizei vertexCount, GLsizei indexCount, Type indexType): Mesh(primitive, vertexCount), _indexCount(indexCount), _indexType(indexType) {
    _indexBuffer.setTargetHint(Buffer::Target::ElementArray);

    (this->*createIndexedImplementation)();
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

        createIndexedImplementation = &IndexedMesh::createIndexedImplementationVAO;
        bindIndexedImplementation = &IndexedMesh::bindIndexedImplementationVAO;
        #endif
    }
}

void IndexedMesh::createIndexedImplementationDefault() {}

#ifndef MAGNUM_TARGET_GLES
void IndexedMesh::createIndexedImplementationVAO() {
    glBindVertexArray(vao);

    _indexBuffer.bind(Buffer::Target::ElementArray);
}
#endif

void IndexedMesh::bindIndexedImplementationDefault() {
    _indexBuffer.bind(Buffer::Target::ElementArray);
}

#ifndef MAGNUM_TARGET_GLES
void IndexedMesh::bindIndexedImplementationVAO() {}
#endif

}
