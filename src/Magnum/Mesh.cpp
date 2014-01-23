/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Utility/Debug.h>

#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "Implementation/DebugState.h"
#include "Implementation/BufferState.h"
#include "Implementation/MeshState.h"
#include "Implementation/State.h"

namespace Magnum {

Int Mesh::maxVertexAttributes() { return AbstractShaderProgram::maxVertexAttributes(); }

#ifndef MAGNUM_TARGET_GLES2
Int Mesh::maxElementsIndices() {
    GLint& value = Context::current()->state().mesh->maxElementsIndices;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &value);

    return value;
}

Int Mesh::maxElementsVertices() {
    GLint& value = Context::current()->state().mesh->maxElementsVertices;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &value);

    return value;
}
#endif

std::size_t Mesh::indexSize(IndexType type) {
    switch(type) {
        case IndexType::UnsignedByte: return 1;
        case IndexType::UnsignedShort: return 2;
        case IndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE();
}

Mesh::Mesh(MeshPrimitive primitive): _primitive(primitive), _vertexCount(0), _indexCount(0)
    #ifndef MAGNUM_TARGET_GLES2
    , _indexStart(0), _indexEnd(0)
    #endif
    , _indexOffset(0), _indexType(IndexType::UnsignedInt), _indexBuffer(nullptr)
{
    (this->*Context::current()->state().mesh->createImplementation)();
}

Mesh::~Mesh() {
    /* Moved out, nothing to do */
    if(!_id) return;

    /* Remove current vao from the state */
    GLuint& current = Context::current()->state().mesh->currentVAO;
    if(current == _id) current = 0;

    (this->*Context::current()->state().mesh->destroyImplementation)();
}

Mesh::Mesh(Mesh&& other) noexcept: _id(other._id), _primitive(other._primitive), _vertexCount(other._vertexCount), _indexCount(other._indexCount)
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

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    std::swap(_id, other._id);
    std::swap(_primitive, other._primitive);
    std::swap(_vertexCount, other._vertexCount);
    std::swap(_indexCount, other._indexCount);
    #ifndef MAGNUM_TARGET_GLES2
    std::swap(_indexStart, other._indexStart);
    std::swap(_indexEnd, other._indexEnd);
    #endif
    std::swap(_indexOffset, other._indexOffset);
    std::swap(_indexType, other._indexType);
    std::swap(_indexBuffer, other._indexBuffer);
    std::swap(_attributes, other._attributes);
    #ifndef MAGNUM_TARGET_GLES2
    std::swap(_integerAttributes, other._integerAttributes);
    #ifndef MAGNUM_TARGET_GLES
    std::swap(_longAttributes, other._longAttributes);
    #endif
    #endif

    return *this;
}

std::string Mesh::label() const {
    #ifndef MAGNUM_TARGET_GLES
    return Context::current()->state().debug->getLabelImplementation(GL_VERTEX_ARRAY, _id);
    #else
    return Context::current()->state().debug->getLabelImplementation(GL_VERTEX_ARRAY_KHR, _id);
    #endif
}

Mesh& Mesh::setLabel(const std::string& label) {
    #ifndef MAGNUM_TARGET_GLES
    Context::current()->state().debug->labelImplementation(GL_VERTEX_ARRAY, _id, label);
    #else
    Context::current()->state().debug->labelImplementation(GL_VERTEX_ARRAY_KHR, _id, label);
    #endif
    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type, UnsignedInt start, UnsignedInt end) {
    #if defined(CORRADE_TARGET_NACL) || defined(CORRADE_TARGET_EMSCRIPTEN)
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
    (this->*Context::current()->state().mesh->bindIndexBufferImplementation)(buffer);
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

    (this->*Context::current()->state().mesh->bindImplementation)();

    /* Non-indexed mesh */
    if(!indexCount)
        glDrawArrays(GLenum(_primitive), firstVertex, vertexCount);

    #ifndef MAGNUM_TARGET_GLES2
    /* Indexed mesh with specified range */
    else if(indexEnd)
        glDrawRangeElements(GLenum(_primitive), indexStart, indexEnd, indexCount, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset));
    #endif

    /* Indexed mesh without specified range */
    else
        glDrawElements(GLenum(_primitive), indexCount, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset));

    (this->*Context::current()->state().mesh->unbindImplementation)();
}

void Mesh::bindVAO(GLuint vao) {
    /** @todo Re-enable when extension loader is available for ES */
    GLuint& current = Context::current()->state().mesh->currentVAO;
    if(current != vao) {
        #ifndef MAGNUM_TARGET_GLES2
        glBindVertexArray(current = vao);
        #else
        CORRADE_INTERNAL_ASSERT(false);
        //glBindVertexArrayOES(current = vao);
        #endif
    }
}

void Mesh::attributePointerInternal(const Attribute& attribute) {
    (this->*Context::current()->state().mesh->attributePointerImplementation)(attribute);
}

#ifndef MAGNUM_TARGET_GLES2
void Mesh::attributePointerInternal(const IntegerAttribute& attribute) {
    (this->*Context::current()->state().mesh->attributeIPointerImplementation)(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerInternal(const LongAttribute& attribute) {
    (this->*Context::current()->state().mesh->attributeLPointerImplementation)(attribute);
}
#endif
#endif

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

void Mesh::createImplementationDefault() { _id = 0; }

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
    glDeleteVertexArrays(1, &_id);
    #endif
}

void Mesh::attributePointerImplementationDefault(const Attribute& attribute) {
    #if defined(CORRADE_TARGET_NACL) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_ASSERT(attribute.buffer->targetHint() == Buffer::Target::Array,
        "Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::Target::Array << "but got" << attribute.buffer->targetHint(), );
    #endif

    _attributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const Attribute& attribute) {
    #if defined(CORRADE_TARGET_NACL) || defined(CORRADE_TARGET_EMSCRIPTEN)
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
Debug operator<<(Debug debug, MeshPrimitive value) {
    switch(value) {
        #define _c(value) case MeshPrimitive::value: return debug << "MeshPrimitive::" #value;
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

    return debug << "MeshPrimitive::(invalid)";
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

std::string ConfigurationValue<Magnum::MeshPrimitive>::toString(Magnum::MeshPrimitive value, ConfigurationValueFlags) {
    switch(value) {
        #define _c(value) case Magnum::MeshPrimitive::value: return #value;
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

Magnum::MeshPrimitive ConfigurationValue<Magnum::MeshPrimitive>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    #define _c(value) if(stringValue == #value) return Magnum::MeshPrimitive::value;
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

    return Magnum::MeshPrimitive::Points;
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
