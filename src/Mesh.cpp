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
#include "Context.h"
#include "Extensions.h"
#include "Implementation/MeshState.h"
#include "Implementation/State.h"

using namespace std;

namespace Magnum {

Mesh::CreateImplementation Mesh::createImplementation = &Mesh::createImplementationDefault;
Mesh::DestroyImplementation Mesh::destroyImplementation = &Mesh::destroyImplementationDefault;
Mesh::BindAttributeImplementation Mesh::bindAttributeImplementation = &Mesh::bindAttributeImplementationDefault;
Mesh::BindImplementation Mesh::bindImplementation = &Mesh::bindImplementationDefault;
Mesh::UnbindImplementation Mesh::unbindImplementation = &Mesh::unbindImplementationDefault;

Mesh::~Mesh() {
    /* Remove current vao from the state */
    GLuint& current = Context::current()->state()->mesh->currentVAO;
    if(current == vao) current = 0;

    (this->*destroyImplementation)();
}

Mesh::Mesh(Mesh&& other): vao(other.vao), _primitive(other._primitive), _vertexCount(other._vertexCount), attributes(other.attributes) {
    other.vao = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
    (this->*destroyImplementation)();

    vao = other.vao;
    _primitive = other._primitive;
    _vertexCount = other._vertexCount;
    attributes = other.attributes;

    other.vao = 0;

    return *this;
}

void Mesh::draw() {
    bind();

    /** @todo Start at given index */
    glDrawArrays(static_cast<GLenum>(_primitive), 0, _vertexCount);

    unbind();
}

void Mesh::bindVAO(GLuint vao) {
    GLuint& current = Context::current()->state()->mesh->currentVAO;
    if(current != vao) glBindVertexArray(current = vao);
}

void Mesh::bind() {
    CORRADE_ASSERT((_vertexCount == 0) == attributes.empty(), "Mesh: vertex count is non-zero, but no attributes are bound", );

    (this->*bindImplementation)();
}

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

    (this->*bindAttributeImplementation)(attributes.back());
}

void Mesh::vertexAttribPointer(const Mesh::Attribute& attribute) {
    glEnableVertexAttribArray(attribute.location);

    attribute.buffer->bind(Buffer::Target::Array);

    #ifndef MAGNUM_TARGET_GLES
    if(TypeInfo::isIntegral(attribute.type))
        glVertexAttribIPointer(attribute.location, attribute.count, static_cast<GLenum>(attribute.type), attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    else
    #endif
        glVertexAttribPointer(attribute.location, attribute.count, static_cast<GLenum>(attribute.type), GL_FALSE, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}

void Mesh::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>()) {
        #ifndef MAGNUM_TARGET_GLES
        Debug() << "Mesh: using" << Extensions::GL::APPLE::vertex_array_object::string() << "features";

        createImplementation = &Mesh::createImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;

        if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            Debug() << "Mesg: using" << Extensions::GL::EXT::direct_state_access::string() << "features";
            bindAttributeImplementation = &Mesh::bindAttributeImplementationDSA;
        } else bindAttributeImplementation = &Mesh::bindAttributeImplementationVAO;

        bindImplementation = &Mesh::bindImplementationVAO;
        unbindImplementation = &Mesh::unbindImplementationVAO;
        #endif
    }
}

void Mesh::createImplementationDefault() {}

#ifndef MAGNUM_TARGET_GLES
void Mesh::createImplementationVAO() {
    glGenVertexArrays(1, &vao);
}
#endif

void Mesh::destroyImplementationDefault() {}

#ifndef MAGNUM_TARGET_GLES
void Mesh::destroyImplementationVAO() {
    glDeleteVertexArrays(1, &vao);
}
#endif

void Mesh::bindAttributeImplementationDefault(const Attribute&) {}

#ifndef MAGNUM_TARGET_GLES
void Mesh::bindAttributeImplementationVAO(const Attribute& attribute) {
    bindVAO(vao);
    vertexAttribPointer(attribute);
}
void Mesh::bindAttributeImplementationDSA(const Attribute& attribute) {
    glEnableVertexArrayAttribEXT(vao, attribute.location);

    #ifndef MAGNUM_TARGET_GLES
    if(TypeInfo::isIntegral(attribute.type))
        glVertexArrayVertexAttribIOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.count, static_cast<GLenum>(attribute.type), attribute.stride, attribute.offset);
    else
    #endif
        glVertexArrayVertexAttribOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.count, static_cast<GLenum>(attribute.type), GL_FALSE, attribute.stride, attribute.offset);
}
#endif

void Mesh::bindImplementationDefault() {
    for(const Attribute& attribute: attributes)
        vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::bindImplementationVAO() {
    bindVAO(vao);
}
#endif

void Mesh::unbindImplementationDefault() {
    for(const Attribute& attribute: attributes)
        glDisableVertexAttribArray(attribute.location);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::unbindImplementationVAO() {}
#endif

}
