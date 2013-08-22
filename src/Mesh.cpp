/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Mesh.h"

#include <Utility/Debug.h>

#include "Buffer.h"
#include "Context.h"
#include "Extensions.h"
#include "Implementation/BufferState.h"
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
Mesh::BindIndexBufferImplementation Mesh::bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationDefault;
Mesh::BindImplementation Mesh::bindImplementation = &Mesh::bindImplementationDefault;
Mesh::UnbindImplementation Mesh::unbindImplementation = &Mesh::unbindImplementationDefault;

std::size_t Mesh::indexSize(IndexType type) {
    switch(type) {
        case IndexType::UnsignedByte: return 1;
        case IndexType::UnsignedShort: return 2;
        case IndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE();
}

Mesh::Mesh(Primitive primitive): _primitive(primitive), _vertexCount(0), _indexCount(0)
    #ifndef MAGNUM_TARGET_GLES2
    , _indexStart(0), _indexEnd(0)
    #endif
    , _indexOffset(0), _indexType(IndexType::UnsignedInt), _indexBuffer(nullptr)
{
    (this->*createImplementation)();
}

Mesh::~Mesh() {
    /* Remove current vao from the state */
    GLuint& current = Context::current()->state().mesh->currentVAO;
    if(current == _id) current = 0;

    (this->*destroyImplementation)();
}

Mesh::Mesh(Mesh&& other): _id(other._id), _primitive(other._primitive), _vertexCount(other._vertexCount), _indexCount(other._indexCount)
    #ifndef MAGNUM_TARGET_GLES2
    , _indexStart(other._indexStart), _indexEnd(other._indexEnd)
    #endif
    , _indexOffset(other._indexOffset), _indexType(other._indexType), _indexBuffer(other._indexBuffer), _attributes(std::move(other._attributes))
    #ifndef MAGNUM_TARGET_GLES2
    , _integerAttributes(std::move(other._integerAttributes))
    #ifndef MAGNUM_TARGET_GLES
    , _longAttributes(std::move(other._longAttributes))
    #endif
    #endif
{
    other._id = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
    (this->*destroyImplementation)();

    _id = other._id;
    _primitive = other._primitive;
    _vertexCount = other._vertexCount;
    _indexCount = other._indexCount;
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = other._indexStart;
    _indexEnd = other._indexEnd;
    #endif
    _indexOffset = other._indexOffset;
    _indexType = other._indexType;
    _indexBuffer = other._indexBuffer;
    _attributes = std::move(other._attributes);
    #ifndef MAGNUM_TARGET_GLES2
    _integerAttributes = std::move(other._integerAttributes);
    #ifndef MAGNUM_TARGET_GLES
    _longAttributes = std::move(other._longAttributes);
    #endif
    #endif

    other._id = 0;

    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type, UnsignedInt start, UnsignedInt end) {
    #ifdef CORRADE_TARGET_NACL
    CORRADE_ASSERT(buffer.targetHint() == Buffer::Target::ElementArray,
        "Mesh::setIndexBuffer(): the buffer has unexpected target hint, expected" << Buffer::Target::ElementArray << "but got" << buffer.targetHint(), *this);
    #endif

    _indexOffset = offset;
    _indexType = type;
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = start;
    _indexEnd = end;
    #else
    static_cast<void>(start);
    static_cast<void>(end);
    #endif
    (this->*bindIndexBufferImplementation)(buffer);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
void Mesh::drawInternal(Int firstVertex, Int vertexCount, GLintptr indexOffset, Int indexCount, Int indexStart, Int indexEnd)
#else
void Mesh::drawInternal(Int firstVertex, Int vertexCount, GLintptr indexOffset, Int indexCount)
#endif
{
    /* Nothing to draw */
    if(!vertexCount && !indexCount) return;

    (this->*bindImplementation)();

    /* Non-indexed mesh */
    if(!indexCount)
        glDrawArrays(static_cast<GLenum>(_primitive), firstVertex, vertexCount);

    #ifndef MAGNUM_TARGET_GLES2
    /* Indexed mesh with specified range */
    else if(indexEnd)
        glDrawRangeElements(static_cast<GLenum>(_primitive), indexStart, indexEnd, indexCount, static_cast<GLenum>(_indexType), reinterpret_cast<GLvoid*>(indexOffset));
    #endif

    /* Indexed mesh without specified range */
    else
        glDrawElements(static_cast<GLenum>(_primitive), indexCount, static_cast<GLenum>(_indexType), reinterpret_cast<GLvoid*>(indexOffset));

    (this->*unbindImplementation)();
}

void Mesh::bindVAO(GLuint vao) {
    /** @todo Get some extension wrangler instead to avoid linker errors to glBindVertexArray() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    GLuint& current = Context::current()->state().mesh->currentVAO;
    if(current != vao) glBindVertexArray(current = vao);
    #else
    static_cast<void>(vao);
    #endif
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

void Mesh::initializeContextBasedFunctionality(Context& context) {
    /** @todo VAOs are in ES 3.0 and as extension in ES 2.0, enable them when some extension wrangler is available */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>()) {
        Debug() << "Mesh: using" << Extensions::GL::APPLE::vertex_array_object::string() << "features";

        createImplementation = &Mesh::createImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;

        if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            Debug() << "Mesh: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

            attributePointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeIPointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeLPointerImplementation = &Mesh::attributePointerImplementationDSA;
        } else {
            attributePointerImplementation = &Mesh::attributePointerImplementationVAO;
            attributeIPointerImplementation = &Mesh::attributePointerImplementationVAO;
            attributeLPointerImplementation = &Mesh::attributePointerImplementationVAO;
        }

        bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationVAO;
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
    glGenVertexArrays(1, &_id);
    #endif
}

void Mesh::destroyImplementationDefault() {}

void Mesh::destroyImplementationVAO() {
    /** @todo Get some extension wrangler instead to avoid linker errors to glDeleteVertexArrays() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    if(_id) glDeleteVertexArrays(1, &_id);
    #endif
}

void Mesh::attributePointerImplementationDefault(const Attribute& attribute) {
    #ifdef CORRADE_TARGET_NACL
    CORRADE_ASSERT(attribute.buffer->targetHint() == Buffer::Target::Array,
        "Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::Target::Array << "but got" << attribute.buffer->targetHint(), );
    #endif

    _attributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const Attribute& attribute) {
    #ifdef CORRADE_TARGET_NACL
    CORRADE_ASSERT(attribute.buffer->targetHint() == Buffer::Target::Array,
        "Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::Target::Array << "but got" << attribute.buffer->targetHint(), );
    #endif

    bindVAO(_id);
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSA(const Attribute& attribute) {
    glEnableVertexArrayAttribEXT(_id, attribute.location);
    glVertexArrayVertexAttribOffsetEXT(_id, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Mesh::attributePointerImplementationDefault(const IntegerAttribute& attribute) {
    _integerAttributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const IntegerAttribute& attribute) {
    bindVAO(_id);
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSA(const IntegerAttribute& attribute) {
    glEnableVertexArrayAttribEXT(_id, attribute.location);
    glVertexArrayVertexAttribIOffsetEXT(_id, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDefault(const LongAttribute& attribute) {
    _longAttributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const LongAttribute& attribute) {
    bindVAO(_id);
    vertexAttribPointer(attribute);
}

void Mesh::attributePointerImplementationDSA(const LongAttribute& attribute) {
    glEnableVertexArrayAttribEXT(_id, attribute.location);
    glVertexArrayVertexAttribLOffsetEXT(_id, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
}
#endif
#endif

void Mesh::bindIndexBufferImplementationDefault(Buffer& buffer) {
    _indexBuffer = &buffer;
}

void Mesh::bindIndexBufferImplementationVAO(Buffer& buffer) {
    bindVAO(_id);

    /* Reset ElementArray binding to force explicit glBindBuffer call later */
    /** @todo Do this cleaner way */
    Context::current()->state().buffer->bindings[Implementation::BufferState::indexForTarget(Buffer::Target::ElementArray)] = 0;

    buffer.bind(Buffer::Target::ElementArray);
}

void Mesh::bindImplementationDefault() {
    /* Specify vertex attributes */
    for(const Attribute& attribute: _attributes)
        vertexAttribPointer(attribute);

    #ifndef MAGNUM_TARGET_GLES2
    for(const IntegerAttribute& attribute: _integerAttributes)
        vertexAttribPointer(attribute);

    #ifndef MAGNUM_TARGET_GLES
    for(const LongAttribute& attribute: _longAttributes)
        vertexAttribPointer(attribute);
    #endif
    #endif

    /* Bind index buffer, if the mesh is indexed */
    if(_indexCount) _indexBuffer->bind(Buffer::Target::ElementArray);
}

void Mesh::bindImplementationVAO() {
    bindVAO(_id);
}

void Mesh::unbindImplementationDefault() {
    for(const Attribute& attribute: _attributes)
        glDisableVertexAttribArray(attribute.location);

    #ifndef MAGNUM_TARGET_GLES2
    for(const IntegerAttribute& attribute: _integerAttributes)
        glDisableVertexAttribArray(attribute.location);

    #ifndef MAGNUM_TARGET_GLES
    for(const LongAttribute& attribute: _longAttributes)
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
        _c(LineStrip)
        _c(LineLoop)
        _c(Lines)
        #ifndef MAGNUM_TARGET_GLES
        _c(LineStripAdjacency)
        _c(LinesAdjacency)
        #endif
        _c(TriangleStrip)
        _c(TriangleFan)
        _c(Triangles)
        #ifndef MAGNUM_TARGET_GLES
        _c(TriangleStripAdjacency)
        _c(TrianglesAdjacency)
        _c(Patches)
        #endif
        #undef _c
    }

    return debug << "Mesh::Primitive::(invalid)";
}

Debug operator<<(Debug debug, Mesh::IndexType value) {
    switch(value) {
        #define _c(value) case Mesh::IndexType::value: return debug << "Mesh::IndexType::" #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
    }

    return debug << "Mesh::IndexType::(invalid)";
}
#endif

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::Mesh::Primitive>::toString(Magnum::Mesh::Primitive value, ConfigurationValueFlags) {
    switch(value) {
        #define _c(value) case Magnum::Mesh::Primitive::value: return #value;
        _c(Points)
        _c(LineStrip)
        _c(LineLoop)
        _c(Lines)
        #ifndef MAGNUM_TARGET_GLES
        _c(LineStripAdjacency)
        _c(LinesAdjacency)
        #endif
        _c(TriangleStrip)
        _c(TriangleFan)
        _c(Triangles)
        #ifndef MAGNUM_TARGET_GLES
        _c(TriangleStripAdjacency)
        _c(TrianglesAdjacency)
        _c(Patches)
        #endif
        #undef _c
    }

    return {};
}

Magnum::Mesh::Primitive ConfigurationValue<Magnum::Mesh::Primitive>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    #define _c(value) if(stringValue == #value) return Magnum::Mesh::Primitive::value;
    _c(LineStrip)
    _c(LineLoop)
    _c(Lines)
    #ifndef MAGNUM_TARGET_GLES
    _c(LineStripAdjacency)
    _c(LinesAdjacency)
    #endif
    _c(TriangleStrip)
    _c(TriangleFan)
    _c(Triangles)
    #ifndef MAGNUM_TARGET_GLES
    _c(TriangleStripAdjacency)
    _c(TrianglesAdjacency)
    _c(Patches)
    #endif
    #undef _c

    return Magnum::Mesh::Primitive::Points;
}

std::string ConfigurationValue<Magnum::Mesh::IndexType>::toString(Magnum::Mesh::IndexType value, ConfigurationValueFlags) {
    switch(value) {
        #define _c(value) case Magnum::Mesh::IndexType::value: return #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
    }

    return {};
}

Magnum::Mesh::IndexType ConfigurationValue<Magnum::Mesh::IndexType>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    #define _c(value) if(stringValue == #value) return Magnum::Mesh::IndexType::value;
    _c(UnsignedByte)
    _c(UnsignedShort)
    _c(UnsignedInt)
    #undef _c

    return Magnum::Mesh::IndexType::UnsignedInt;
}

}}
