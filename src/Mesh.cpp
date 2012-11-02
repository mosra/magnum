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
Mesh::AttributePointerImplementation Mesh::attributePointerImplementation = &Mesh::attributePointerImplementationDefault;
Mesh::AttributeIPointerImplementation Mesh::attributeIPointerImplementation = &Mesh::attributePointerImplementationDefault;
Mesh::AttributeLPointerImplementation Mesh::attributeLPointerImplementation = &Mesh::attributePointerImplementationDefault;
Mesh::BindImplementation Mesh::bindImplementation = &Mesh::bindImplementationDefault;
Mesh::UnbindImplementation Mesh::unbindImplementation = &Mesh::unbindImplementationDefault;

Mesh::~Mesh() {
    /* Remove current vao from the state */
    GLuint& current = Context::current()->state()->mesh->currentVAO;
    if(current == vao) current = 0;

    (this->*destroyImplementation)();
}

Mesh::Mesh(Mesh&& other): vao(other.vao), _primitive(other._primitive), _vertexCount(other._vertexCount), attributes(std::move(other.attributes)), integerAttributes(std::move(other.integerAttributes)), longAttributes(std::move(other.longAttributes)) {
    other.vao = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
    (this->*destroyImplementation)();

    vao = other.vao;
    _primitive = other._primitive;
    _vertexCount = other._vertexCount;
    attributes = std::move(other.attributes);
    integerAttributes = std::move(other.integerAttributes);
    longAttributes = std::move(other.longAttributes);

    other.vao = 0;

    return *this;
}

Mesh* Mesh::setVertexCount(GLsizei vertexCount) {
    _vertexCount = vertexCount;
    attributes.clear();
    #ifndef MAGNUM_TARGET_GLES
    integerAttributes.clear();
    longAttributes.clear();
    #endif
    return this;
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

void Mesh::vertexAttribPointer(const Attribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bind(Buffer::Target::Array);
    glVertexAttribPointer(attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::vertexAttribPointer(const IntegerAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bind(Buffer::Target::Array);
    glVertexAttribIPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}

void Mesh::vertexAttribPointer(const LongAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bind(Buffer::Target::Array);
    glVertexAttribLPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}
#endif

void Mesh::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>()) {
        #ifndef MAGNUM_TARGET_GLES
        Debug() << "Mesh: using" << Extensions::GL::APPLE::vertex_array_object::string() << "features";

        createImplementation = &Mesh::createImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;

        if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            Debug() << "Mesg: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

            attributePointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeIPointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeLPointerImplementation = &Mesh::attributePointerImplementationDSA;
        } else {
            attributePointerImplementation = &Mesh::attributePointerImplementationVAO;
            attributeIPointerImplementation = &Mesh::attributePointerImplementationVAO;
            attributeLPointerImplementation = &Mesh::attributePointerImplementationVAO;
        }

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

void Mesh::attributePointerImplementationDefault(const Attribute&) {}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationVAO(const Attribute& attribute) {
    bindVAO(vao);
    vertexAttribPointer(attribute);
}

void Mesh::attributePointerImplementationDSA(const Attribute& attribute) {
    glEnableVertexArrayAttribEXT(vao, attribute.location);
    glVertexArrayVertexAttribOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
}

void Mesh::attributePointerImplementationDefault(const IntegerAttribute&) {}

void Mesh::attributePointerImplementationVAO(const IntegerAttribute& attribute) {
    bindVAO(vao);
    vertexAttribPointer(attribute);
}

void Mesh::attributePointerImplementationDSA(const IntegerAttribute& attribute) {
    glEnableVertexArrayAttribEXT(vao, attribute.location);
    glVertexArrayVertexAttribIOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
}

void Mesh::attributePointerImplementationDefault(const LongAttribute&) {}

void Mesh::attributePointerImplementationVAO(const LongAttribute& attribute) {
    bindVAO(vao);
    vertexAttribPointer(attribute);
}

void Mesh::attributePointerImplementationDSA(const LongAttribute& attribute) {
    glEnableVertexArrayAttribEXT(vao, attribute.location);
    glVertexArrayVertexAttribLOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
}
#endif

void Mesh::bindImplementationDefault() {
    for(const Attribute& attribute: attributes)
        vertexAttribPointer(attribute);

    #ifndef MAGNUM_TARGET_GLES
    for(const IntegerAttribute& attribute: integerAttributes)
        vertexAttribPointer(attribute);

    for(const LongAttribute& attribute: longAttributes)
        vertexAttribPointer(attribute);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::bindImplementationVAO() {
    bindVAO(vao);
}
#endif

void Mesh::unbindImplementationDefault() {
    for(const Attribute& attribute: attributes)
        glDisableVertexAttribArray(attribute.location);

    #ifndef MAGNUM_TARGET_GLES
    for(const IntegerAttribute& attribute: integerAttributes)
        glDisableVertexAttribArray(attribute.location);

    for(const LongAttribute& attribute: longAttributes)
        glDisableVertexAttribArray(attribute.location);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::unbindImplementationVAO() {}
#endif

}
