/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "Implementation/DebugState.h"
#include "Implementation/BufferState.h"
#include "Implementation/MeshState.h"
#include "Implementation/State.h"

namespace Magnum {

#ifdef MAGNUM_BUILD_DEPRECATED
Int Mesh::maxVertexAttributes() { return AbstractShaderProgram::maxVertexAttributes(); }
#endif

#ifndef MAGNUM_TARGET_GLES2
Long Mesh::maxElementIndex() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::ES3_compatibility>())
        return 0xFFFFFFFFl;
    #endif

    GLint64& value = Context::current()->state().mesh->maxElementIndex;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetInteger64v(GL_MAX_ELEMENT_INDEX, &value);

    return value;
}

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

Mesh::Mesh(const MeshPrimitive primitive): _primitive{primitive}, _count{0}, _baseVertex{0}, _instanceCount{1},
    #ifndef MAGNUM_TARGET_GLES
    _baseInstance{0},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart(0), _indexEnd(0),
    #endif
    _indexOffset(0), _indexType(IndexType::UnsignedInt), _indexBuffer(nullptr)
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

Mesh::Mesh(Mesh&& other) noexcept: _id(other._id), _created{other._created}, _primitive(other._primitive), _count(other._count), _baseVertex{other._baseVertex}, _instanceCount{other._instanceCount},
    #ifndef MAGNUM_TARGET_GLES
    _baseInstance{other._baseInstance},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart(other._indexStart), _indexEnd(other._indexEnd),
    #endif
    _indexOffset(other._indexOffset), _indexType(other._indexType), _indexBuffer(other._indexBuffer), _attributes(std::move(other._attributes))
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
    using std::swap;
    swap(_id, other._id);
    swap(_created, other._created);
    swap(_primitive, other._primitive);
    swap(_count, other._count);
    swap(_baseVertex, other._baseVertex);
    swap(_instanceCount, other._instanceCount);
    #ifndef MAGNUM_TARGET_GLES
    swap(_baseInstance, other._baseInstance);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    swap(_indexStart, other._indexStart);
    swap(_indexEnd, other._indexEnd);
    #endif
    swap(_indexOffset, other._indexOffset);
    swap(_indexType, other._indexType);
    swap(_indexBuffer, other._indexBuffer);
    swap(_attributes, other._attributes);
    #ifndef MAGNUM_TARGET_GLES2
    swap(_integerAttributes, other._integerAttributes);
    #ifndef MAGNUM_TARGET_GLES
    swap(_longAttributes, other._longAttributes);
    #endif
    #endif

    return *this;
}

inline void Mesh::createIfNotAlready() {
    /* If VAO extension is not available, _created is always true */
    if(_created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glObjectLabel()) operate with IDs directly and they
       require the object to be created. Binding the VAO finally creates it.
       Also all EXT DSA functions implicitly create it. */
    bindVAO();
    CORRADE_INTERNAL_ASSERT(_created);
}

std::string Mesh::label() {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES
    return Context::current()->state().debug->getLabelImplementation(GL_VERTEX_ARRAY, _id);
    #else
    return Context::current()->state().debug->getLabelImplementation(GL_VERTEX_ARRAY_KHR, _id);
    #endif
}

Mesh& Mesh::setLabelInternal(const Containers::ArrayReference<const char> label) {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES
    Context::current()->state().debug->labelImplementation(GL_VERTEX_ARRAY, _id, label);
    #else
    Context::current()->state().debug->labelImplementation(GL_VERTEX_ARRAY_KHR, _id, label);
    #endif
    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type, UnsignedInt start, UnsignedInt end) {
    #if defined(CORRADE_TARGET_NACL) || defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(buffer.targetHint() == Buffer::TargetHint::ElementArray,
        "Mesh::setIndexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::ElementArray << "but got" << buffer.targetHint(), *this);
    #endif

    _indexBuffer = &buffer;
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

void Mesh::draw(AbstractShaderProgram& shader) {
    shader.use();

    #ifndef MAGNUM_TARGET_GLES
    drawInternal(_count, _baseVertex, _instanceCount, _baseInstance, _indexOffset, _indexStart, _indexEnd);
    #elif !defined(MAGNUM_TARGET_GLES2)
    drawInternal(_count, _baseVertex, _instanceCount, _indexOffset, _indexStart, _indexEnd);
    #else
    drawInternal(_count, _baseVertex, _instanceCount, _indexOffset);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::drawInternal(Int count, Int baseVertex, Int instanceCount, UnsignedInt baseInstance, GLintptr indexOffset, Int indexStart, Int indexEnd)
#elif !defined(MAGNUM_TARGET_GLES2)
void Mesh::drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset, Int indexStart, Int indexEnd)
#else
void Mesh::drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset)
#endif
{

    const Implementation::MeshState& state = *Context::current()->state().mesh;

    /* Nothing to draw */
    if(!count || !instanceCount) return;

    (this->*state.bindImplementation)();

    /* Non-instanced mesh */
    if(instanceCount == 1) {
        /* Non-indexed mesh */
        if(!_indexBuffer) {
            glDrawArrays(GLenum(_primitive), baseVertex, count);

        /* Indexed mesh with base vertex */
        } else if(baseVertex) {
            #ifndef MAGNUM_TARGET_GLES
            /* Indexed mesh with specified range */
            if(indexEnd) {
                glDrawRangeElementsBaseVertex(GLenum(_primitive), indexStart, indexEnd, count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), baseVertex);

            /* Indexed mesh */
            } else glDrawElementsBaseVertex(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), baseVertex);
            #else
            CORRADE_ASSERT(false, "Mesh::draw(): desktop OpenGL is required for base vertex specification in indexed meshes", );
            #endif

        /* Indexed mesh */
        } else {
            #ifndef MAGNUM_TARGET_GLES2
            /* Indexed mesh with specified range */
            if(indexEnd) {
                glDrawRangeElements(GLenum(_primitive), indexStart, indexEnd, count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset));

            /* Indexed mesh */
            } else
            #endif
            {
                glDrawElements(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset));
            }
        }

    /* Instanced mesh */
    } else {
        /* Non-indexed mesh */
        if(!_indexBuffer) {
            #ifndef MAGNUM_TARGET_GLES
            /* Non-indexed mesh with base instance */
            if(baseInstance) {
                glDrawArraysInstancedBaseInstance(GLenum(_primitive), baseVertex, count, instanceCount, baseInstance);

            /* Non-indexed mesh */
            } else
            #endif
            {
                #ifndef MAGNUM_TARGET_GLES2
                glDrawArraysInstanced(GLenum(_primitive), baseVertex, count, instanceCount);
                #else
                (this->*state.drawArraysInstancedImplementation)(baseVertex, count, instanceCount);
                #endif
            }

        /* Indexed mesh with base vertex */
        } else if(baseVertex) {
            #ifndef MAGNUM_TARGET_GLES
            /* Indexed mesh with base vertex and base instance */
            if(baseInstance)
                glDrawElementsInstancedBaseVertexBaseInstance(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount, baseVertex, baseInstance);

            /* Indexed mesh with base vertex */
            else
                glDrawElementsInstancedBaseVertex(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount, baseVertex);
            #else
            CORRADE_ASSERT(false, "Mesh::draw(): desktop OpenGL is required for base vertex specification in indexed meshes", );
            #endif

        /* Indexed mesh */
        } else {
            #ifndef MAGNUM_TARGET_GLES
            /* Indexed mesh with base instance */
            if(baseInstance) {
                glDrawElementsInstancedBaseInstance(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount, baseInstance);

            /* Instanced mesh */
            } else
            #endif
            {
                #ifndef MAGNUM_TARGET_GLES2
                glDrawElementsInstanced(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
                #else
                (this->*state.drawElementsInstancedImplementation)(count, indexOffset, instanceCount);
                #endif
            }
        }
    }

    (this->*state.unbindImplementation)();
}

void Mesh::bindVAO() {
    GLuint& current = Context::current()->state().mesh->currentVAO;
    if(current != _id) {
        /* Binding the VAO finally creates it */
        _created = true;
        #ifndef MAGNUM_TARGET_GLES2
        glBindVertexArray(current = _id);
        #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
        glBindVertexArrayOES(current = _id);
        #else
        CORRADE_ASSERT_UNREACHABLE();
        #endif
    }
}

void Mesh::createImplementationDefault() {
    _id = 0;
    _created = true;
}

void Mesh::createImplementationVAO() {
    #ifndef MAGNUM_TARGET_GLES2
    glGenVertexArrays(1, &_id);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glGenVertexArraysOES(1, &_id);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    _created = false;
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::createImplementationVAODSA() {
    glCreateVertexArrays(1, &_id);
    _created = true;
}
#endif

void Mesh::destroyImplementationDefault() {}

void Mesh::destroyImplementationVAO() {
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteVertexArrays(1, &_id);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDeleteVertexArraysOES(1, &_id);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void Mesh::attributePointerInternal(const GenericAttribute& attribute) {
    (this->*Context::current()->state().mesh->attributePointerImplementation)(attribute);
}

void Mesh::attributePointerImplementationDefault(const GenericAttribute& attribute) {
    #if defined(CORRADE_TARGET_NACL) || defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(attribute.buffer->targetHint() == Buffer::TargetHint::Array,
        "Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer->targetHint(), );
    #endif

    _attributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const GenericAttribute& attribute) {
    #if defined(CORRADE_TARGET_NACL) || defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(attribute.buffer->targetHint() == Buffer::TargetHint::Array,
        "Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer->targetHint(), );
    #endif

    bindVAO();
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSAEXT(const GenericAttribute& attribute) {
    _created = true;
    glEnableVertexArrayAttribEXT(_id, attribute.location);
    glVertexArrayVertexAttribOffsetEXT(_id, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
    if(attribute.divisor)
        (this->*Context::current()->state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
}
#endif

void Mesh::vertexAttribPointer(const GenericAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bindInternal(Buffer::TargetHint::Array);
    glVertexAttribPointer(attribute.location, attribute.size, attribute.type, attribute.normalized, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    if(attribute.divisor) {
        #ifndef MAGNUM_TARGET_GLES2
        glVertexAttribDivisor(attribute.location, attribute.divisor);
        #else
        (this->*Context::current()->state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
        #endif
    }
}

#ifndef MAGNUM_TARGET_GLES2
void Mesh::attributePointerInternal(const IntegerAttribute& attribute) {
    (this->*Context::current()->state().mesh->attributeIPointerImplementation)(attribute);
}

void Mesh::attributePointerImplementationDefault(const IntegerAttribute& attribute) {
    _integerAttributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const IntegerAttribute& attribute) {
    bindVAO();
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSAEXT(const IntegerAttribute& attribute) {
    _created = true;
    glEnableVertexArrayAttribEXT(_id, attribute.location);
    glVertexArrayVertexAttribIOffsetEXT(_id, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
    if(attribute.divisor)
        (this->*Context::current()->state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
}
#endif

void Mesh::vertexAttribPointer(const IntegerAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bindInternal(Buffer::TargetHint::Array);
    glVertexAttribIPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    if(attribute.divisor) glVertexAttribDivisor(attribute.location, attribute.divisor);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerInternal(const LongAttribute& attribute) {
    (this->*Context::current()->state().mesh->attributeLPointerImplementation)(attribute);
}

void Mesh::attributePointerImplementationDefault(const LongAttribute& attribute) {
    _longAttributes.push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(const LongAttribute& attribute) {
    bindVAO();
    vertexAttribPointer(attribute);
}

void Mesh::attributePointerImplementationDSAEXT(const LongAttribute& attribute) {
    _created = true;
    glEnableVertexArrayAttribEXT(_id, attribute.location);
    glVertexArrayVertexAttribLOffsetEXT(_id, attribute.buffer->id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
    if(attribute.divisor)
        (this->*Context::current()->state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
}

void Mesh::vertexAttribPointer(const LongAttribute& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer->bindInternal(Buffer::TargetHint::Array);
    glVertexAttribLPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    if(attribute.divisor) glVertexAttribDivisor(attribute.location, attribute.divisor);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Mesh::vertexAttribDivisorImplementationVAO(const GLuint index, const GLuint divisor) {
    bindVAO();
    glVertexAttribDivisor(index, divisor);
}
void Mesh::vertexAttribDivisorImplementationDSAEXT(const GLuint index, const GLuint divisor) {
    glVertexArrayVertexAttribDivisorEXT(_id, index, divisor);
}
#elif defined(MAGNUM_TARGET_GLES2)
void Mesh::vertexAttribDivisorImplementationANGLE(const GLuint index, const GLuint divisor) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glVertexAttribDivisorANGLE(index, divisor);
    #else
    static_cast<void>(index);
    static_cast<void>(divisor);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}
void Mesh::vertexAttribDivisorImplementationEXT(const GLuint index, const GLuint divisor) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glVertexAttribDivisorEXT(index, divisor);
    #else
    static_cast<void>(index);
    static_cast<void>(divisor);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}
void Mesh::vertexAttribDivisorImplementationNV(const GLuint index, const GLuint divisor) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glVertexAttribDivisorNV(index, divisor);
    #else
    static_cast<void>(index);
    static_cast<void>(divisor);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}
#endif

void Mesh::bindIndexBufferImplementationDefault(Buffer&) {}

void Mesh::bindIndexBufferImplementationVAO(Buffer& buffer) {
    bindVAO();

    /* Reset ElementArray binding to force explicit glBindBuffer call later */
    /** @todo Do this cleaner way */
    Context::current()->state().buffer->bindings[Implementation::BufferState::indexForTarget(Buffer::TargetHint::ElementArray)] = 0;

    buffer.bindInternal(Buffer::TargetHint::ElementArray);
}

void Mesh::bindImplementationDefault() {
    /* Specify vertex attributes */
    for(auto it = _attributes.begin(); it != _attributes.end(); ++it)
        vertexAttribPointer(*it);

    #ifndef MAGNUM_TARGET_GLES2
    for(auto it = _integerAttributes.begin(); it != _integerAttributes.end(); ++it)
        vertexAttribPointer(*it);

    #ifndef MAGNUM_TARGET_GLES
    for(auto it = _longAttributes.begin(); it != _longAttributes.end(); ++it)
        vertexAttribPointer(*it);
    #endif
    #endif

    /* Bind index buffer, if the mesh is indexed */
    if(_indexBuffer) _indexBuffer->bindInternal(Buffer::TargetHint::ElementArray);
}

void Mesh::bindImplementationVAO() {
    bindVAO();
}

void Mesh::unbindImplementationDefault() {
    for(auto it = _attributes.begin(); it != _attributes.end(); ++it)
        glDisableVertexAttribArray(it->location);

    #ifndef MAGNUM_TARGET_GLES2
    for(auto it = _integerAttributes.begin(); it != _integerAttributes.end(); ++it)
        glDisableVertexAttribArray(it->location);

    #ifndef MAGNUM_TARGET_GLES
    for(auto it = _longAttributes.begin(); it != _longAttributes.end(); ++it)
        glDisableVertexAttribArray(it->location);
    #endif
    #endif
}

void Mesh::unbindImplementationVAO() {}

#ifdef MAGNUM_TARGET_GLES2
void Mesh::drawArraysInstancedImplementationANGLE(const GLint baseVertex, const GLsizei count, const GLsizei instanceCount) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDrawArraysInstancedANGLE(GLenum(_primitive), baseVertex, count, instanceCount);
    #else
    static_cast<void>(baseVertex);
    static_cast<void>(count);
    static_cast<void>(instanceCount);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void Mesh::drawArraysInstancedImplementationEXT(const GLint baseVertex, const GLsizei count, const GLsizei instanceCount) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDrawArraysInstancedEXT(GLenum(_primitive), baseVertex, count, instanceCount);
    #else
    static_cast<void>(baseVertex);
    static_cast<void>(count);
    static_cast<void>(instanceCount);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void Mesh::drawArraysInstancedImplementationNV(const GLint baseVertex, const GLsizei count, const GLsizei instanceCount) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDrawArraysInstancedNV(GLenum(_primitive), baseVertex, count, instanceCount);
    #else
    static_cast<void>(baseVertex);
    static_cast<void>(count);
    static_cast<void>(instanceCount);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void Mesh::drawElementsInstancedImplementationANGLE(const GLsizei count, const GLintptr indexOffset, const GLsizei instanceCount) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDrawElementsInstancedANGLE(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
    #else
    static_cast<void>(count);
    static_cast<void>(indexOffset);
    static_cast<void>(instanceCount);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void Mesh::drawElementsInstancedImplementationEXT(const GLsizei count, const GLintptr indexOffset, const GLsizei instanceCount) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDrawElementsInstancedEXT(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
    #else
    static_cast<void>(count);
    static_cast<void>(indexOffset);
    static_cast<void>(instanceCount);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void Mesh::drawElementsInstancedImplementationNV(const GLsizei count, const GLintptr indexOffset, const GLsizei instanceCount) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glDrawElementsInstancedNV(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
    #else
    static_cast<void>(count);
    static_cast<void>(indexOffset);
    static_cast<void>(instanceCount);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}
#endif

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
