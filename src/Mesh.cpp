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

namespace Magnum {

Mesh::CreateImplementation Mesh::createImplementation = &Mesh::createImplementationDefault;
Mesh::DestroyImplementation Mesh::destroyImplementation = &Mesh::destroyImplementationDefault;
Mesh::AttributePointerImplementation Mesh::attributePointerImplementation = &Mesh::attributePointerImplementationDefault;
#ifndef MAGNUM_TARGET_GLES2
Mesh::AttributeIPointerImplementation Mesh::attributeIPointerImplementation = &Mesh::attributePointerImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
Mesh::AttributeLPointerImplementation Mesh::attributeLPointerImplementation = &Mesh::attributePointerImplementationDefault;
#endif
#endif
Mesh::BindImplementation Mesh::bindImplementation = &Mesh::bindImplementationDefault;
Mesh::UnbindImplementation Mesh::unbindImplementation = &Mesh::unbindImplementationDefault;

Mesh::~Mesh() {
    /* Remove current vao from the state */
    GLuint& current = Context::current()->state()->mesh->currentVAO;
    if(current == vao) current = 0;

    (this->*destroyImplementation)();
}

Mesh::Mesh(Mesh&& other): vao(other.vao), _primitive(other._primitive), _vertexCount(other._vertexCount), attributes(std::move(other.attributes))
    #ifndef MAGNUM_TARGET_GLES2
    , integerAttributes(std::move(other.integerAttributes))
    #ifndef MAGNUM_TARGET_GLES
    , longAttributes(std::move(other.longAttributes))
    #endif
    #endif
{
    other.vao = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
    (this->*destroyImplementation)();

    vao = other.vao;
    _primitive = other._primitive;
    _vertexCount = other._vertexCount;
    attributes = std::move(other.attributes);
    #ifndef MAGNUM_TARGET_GLES2
    integerAttributes = std::move(other.integerAttributes);
    #ifndef MAGNUM_TARGET_GLES
    longAttributes = std::move(other.longAttributes);
    #endif
    #endif

    other.vao = 0;

    return *this;
}

Mesh* Mesh::setVertexCount(GLsizei vertexCount) {
    _vertexCount = vertexCount;
    attributes.clear();
    #ifndef MAGNUM_TARGET_GLES2
    integerAttributes.clear();
    #ifndef MAGNUM_TARGET_GLES
    longAttributes.clear();
    #endif
    #endif
    return this;
}

void Mesh::draw() {
    if(!_vertexCount) return;

    bind();

    /** @todo Start at given index */
    glDrawArrays(static_cast<GLenum>(_primitive), 0, _vertexCount);

    unbind();
}

void Mesh::bindVAO(GLuint vao) {
    /** @todo Get some extension wrangler instead to avoid linker errors to glBindVertexArray() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    GLuint& current = Context::current()->state()->mesh->currentVAO;
    if(current != vao) glBindVertexArray(current = vao);
    #else
    static_cast<void>(vao);
    #endif
}

void Mesh::bind() {
    CORRADE_ASSERT((_vertexCount != 0) || !attributes.empty(), "Mesh: attributes are bound but vertex count is zero", );

    (this->*bindImplementation)();
}

void Mesh::vertexAttribPointer(const Attribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bind(Buffer::Target::Array);
    glVertexAttribPointer(attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}

#ifndef MAGNUM_TARGET_GLES2
void Mesh::vertexAttribPointer(const IntegerAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bind(Buffer::Target::Array);
    glVertexAttribIPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::vertexAttribPointer(const LongAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bind(Buffer::Target::Array);
    glVertexAttribLPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
}
#endif
#endif

void Mesh::initializeContextBasedFunctionality(Context* context) {
    /** @todo VAOs are in ES 3.0 and as extension in ES 2.0, enable them when some extension wrangler is available */
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>()) {
        Debug() << "Mesh: using" << Extensions::GL::APPLE::vertex_array_object::string() << "features";

        createImplementation = &Mesh::createImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;

        if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            Debug() << "Mesh: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

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
    }
    #else
    static_cast<void>(context);
    #endif
}

void Mesh::createImplementationDefault() {}

void Mesh::createImplementationVAO() {
    /** @todo Get some extension wrangler instead to avoid linker errors to glGenVertexArrays() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glGenVertexArrays(1, &vao);
    #endif
}

void Mesh::destroyImplementationDefault() {}

void Mesh::destroyImplementationVAO() {
    /** @todo Get some extension wrangler instead to avoid linker errors to glDeleteVertexArrays() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteVertexArrays(1, &vao);
    #endif
}

void Mesh::attributePointerImplementationDefault(const Attribute&) {}

void Mesh::attributePointerImplementationVAO(const Attribute& attribute) {
    bindVAO(vao);
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSA(const Attribute& attribute) {
    glEnableVertexArrayAttribEXT(vao, attribute.location);
    glVertexArrayVertexAttribOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Mesh::attributePointerImplementationDefault(const IntegerAttribute&) {}

void Mesh::attributePointerImplementationVAO(const IntegerAttribute& attribute) {
    bindVAO(vao);
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSA(const IntegerAttribute& attribute) {
    glEnableVertexArrayAttribEXT(vao, attribute.location);
    glVertexArrayVertexAttribIOffsetEXT(vao, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
}
#endif

#ifndef MAGNUM_TARGET_GLES
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
#endif

void Mesh::bindImplementationDefault() {
    for(const Attribute& attribute: attributes)
        vertexAttribPointer(attribute);

    #ifndef MAGNUM_TARGET_GLES2
    for(const IntegerAttribute& attribute: integerAttributes)
        vertexAttribPointer(attribute);

    #ifndef MAGNUM_TARGET_GLES
    for(const LongAttribute& attribute: longAttributes)
        vertexAttribPointer(attribute);
    #endif
    #endif
}

void Mesh::bindImplementationVAO() {
    bindVAO(vao);
}

void Mesh::unbindImplementationDefault() {
    for(const Attribute& attribute: attributes)
        glDisableVertexAttribArray(attribute.location);

    #ifndef MAGNUM_TARGET_GLES2
    for(const IntegerAttribute& attribute: integerAttributes)
        glDisableVertexAttribArray(attribute.location);

    #ifndef MAGNUM_TARGET_GLES
    for(const LongAttribute& attribute: longAttributes)
        glDisableVertexAttribArray(attribute.location);
    #endif
    #endif
}

void Mesh::unbindImplementationVAO() {}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, Mesh::Primitive value) {
    switch(value) {
        #define _c(value) case Mesh::Primitive::value: return debug << "Mesh::Primitive::" #value;
        _c(Points)
        _c(Lines)
        _c(LineStrip)
        _c(LineLoop)
        _c(Triangles)
        _c(TriangleStrip)
        _c(TriangleFan)
        #undef _c
    }

    return debug << "Mesh::Primitive::(invalid)";
}
#endif

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::Mesh::Primitive>::toString(Magnum::Mesh::Primitive value, ConfigurationValueFlags) {
    switch(value) {
        #define _c(value) case Magnum::Mesh::Primitive::value: return #value;
        _c(Points)
        _c(Lines)
        _c(LineStrip)
        _c(LineLoop)
        _c(Triangles)
        _c(TriangleStrip)
        _c(TriangleFan)
        #undef _c
    }

    return "";
}

Magnum::Mesh::Primitive ConfigurationValue<Magnum::Mesh::Primitive>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    #define _c(value) if(stringValue == #value) return Magnum::Mesh::Primitive::value;
    _c(Lines)
    _c(LineStrip)
    _c(LineLoop)
    _c(Triangles)
    _c(TriangleStrip)
    _c(TriangleFan)
    #undef _c

    return Magnum::Mesh::Primitive::Points;
}

}}
