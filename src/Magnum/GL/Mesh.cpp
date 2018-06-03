/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include <vector>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Mesh.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/TransformFeedback.h"
#endif
#include "Magnum/GL/Implementation/BufferState.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/MeshState.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum { namespace GL {

namespace {

constexpr MeshPrimitive PrimitiveMapping[]{
    MeshPrimitive::Points,
    MeshPrimitive::Lines,
    MeshPrimitive::LineLoop,
    MeshPrimitive::LineStrip,
    MeshPrimitive::Triangles,
    MeshPrimitive::TriangleStrip,
    MeshPrimitive::TriangleFan
};

constexpr MeshIndexType IndexTypeMapping[]{
    MeshIndexType::UnsignedByte,
    MeshIndexType::UnsignedShort,
    MeshIndexType::UnsignedInt
};

}

MeshPrimitive meshPrimitive(const Magnum::MeshPrimitive primitive) {
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(primitive == Magnum::MeshPrimitive::LinesAdjacency ||
       primitive == Magnum::MeshPrimitive::LineStripAdjacency ||
       primitive == Magnum::MeshPrimitive::TrianglesAdjacency ||
       primitive == Magnum::MeshPrimitive::TriangleStripAdjacency ||
       primitive == Magnum::MeshPrimitive::Patches)
        return MeshPrimitive(UnsignedInt(primitive));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    CORRADE_ASSERT(UnsignedInt(primitive) < Containers::arraySize(PrimitiveMapping),
        "GL::meshPrimitive(): invalid primitive" << primitive, {});
    return PrimitiveMapping[UnsignedInt(primitive)];
}

MeshIndexType meshIndexType(const Magnum::MeshIndexType type) {
    CORRADE_ASSERT(UnsignedInt(type) < Containers::arraySize(IndexTypeMapping),
        "GL::meshIndexType(): invalid type" << type, {});
    return IndexTypeMapping[UnsignedInt(type)];
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, MeshPrimitive value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshPrimitive::value: return debug << "GL::MeshPrimitive::" #value;
        _c(Points)
        _c(Lines)
        _c(LineLoop)
        _c(LineStrip)
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(LineStripAdjacency)
        _c(LinesAdjacency)
        #endif
        _c(Triangles)
        _c(TriangleStrip)
        _c(TriangleFan)
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(TrianglesAdjacency)
        _c(TriangleStripAdjacency)
        _c(Patches)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "GL::MeshPrimitive(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, MeshIndexType value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshIndexType::value: return debug << "GL::MeshIndexType::" #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "GL::MeshIndexType(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

struct Mesh::AttributeLayout {
    explicit AttributeLayout(const Buffer& buffer, GLuint location, GLint size, GLenum type, DynamicAttribute::Kind kind, GLintptr offset, GLsizei stride, GLuint divisor) noexcept: buffer{Buffer::wrap(buffer.id())}, location{location}, size{size}, type{type}, kind{kind}, offset{offset}, stride{stride}, divisor{divisor} {}

    explicit AttributeLayout(const AttributeLayout& other): buffer{Buffer::wrap(other.buffer.id())}, location{other.location}, size{other.size}, type{other.type}, kind{other.kind}, offset{other.offset}, stride{other.stride}, divisor{other.divisor} {}

    Buffer buffer;
    GLuint location;
    GLint size;
    GLenum type;
    DynamicAttribute::Kind kind;
    GLintptr offset;
    GLsizei stride;
    GLuint divisor;
};

#ifndef MAGNUM_TARGET_GLES2
#ifndef MAGNUM_TARGET_WEBGL
Long Mesh::maxElementIndex()
#else
Int Mesh::maxElementIndex()
#endif
{
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::ES3_compatibility>())
        return 0xFFFFFFFFl;
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    GLint64& value =
    #else
    GLint& value =
    #endif
        Context::current().state().mesh->maxElementIndex;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_WEBGL
        glGetInteger64v(GL_MAX_ELEMENT_INDEX, &value);
        #else
        glGetIntegerv(GL_MAX_ELEMENT_INDEX, &value);
        #endif
    }

    return value;
}

Int Mesh::maxElementsIndices() {
    GLint& value = Context::current().state().mesh->maxElementsIndices;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &value);

    return value;
}

Int Mesh::maxElementsVertices() {
    GLint& value = Context::current().state().mesh->maxElementsVertices;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &value);

    return value;
}
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
std::size_t Mesh::indexSize(Magnum::MeshIndexType type) {
    return meshIndexTypeSize(type);
}
#endif

Mesh::Mesh(const MeshPrimitive primitive): _primitive{primitive}, _flags{ObjectFlag::DeleteOnDestruction} {
    (this->*Context::current().state().mesh->createImplementation)();
}

Mesh::Mesh(NoCreateT) noexcept: _id{0}, _primitive{MeshPrimitive::Triangles}, _flags{ObjectFlag::DeleteOnDestruction} {}

Mesh::~Mesh() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    /* Remove current vao from the state */
    GLuint& current = Context::current().state().mesh->currentVAO;
    if(current == _id) current = 0;

    (this->*Context::current().state().mesh->destroyImplementation)();
}

Mesh::Mesh(Mesh&& other) noexcept: _id(other._id), _primitive(other._primitive), _flags{other._flags}, _countSet{other._countSet}, _count(other._count), _baseVertex{other._baseVertex}, _instanceCount{other._instanceCount},
    #ifndef MAGNUM_TARGET_GLES
    _baseInstance{other._baseInstance},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart(other._indexStart), _indexEnd(other._indexEnd),
    #endif
    _indexOffset(other._indexOffset), _indexType(other._indexType), _indexBuffer{std::move(other._indexBuffer)}
{
    (this->*Context::current().state().mesh->moveConstructImplementation)(std::move(other));
    other._id = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    using std::swap;
    swap(_id, other._id);
    swap(_flags, other._flags);
    swap(_primitive, other._primitive);
    swap(_countSet, other._countSet);
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
    (this->*Context::current().state().mesh->moveAssignImplementation)(std::move(other));

    return *this;
}

Mesh::Mesh(const GLuint id, const MeshPrimitive primitive, const ObjectFlags flags): _id{id}, _primitive{primitive}, _flags{flags} {}

inline void Mesh::createIfNotAlready() {
    /* If VAO extension is not available, the following is always true */
    if(_flags & ObjectFlag::Created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glObjectLabel()) operate with IDs directly and they
       require the object to be created. Binding the VAO finally creates it.
       Also all EXT DSA functions implicitly create it. */
    bindVAO();
    CORRADE_INTERNAL_ASSERT(_flags & ObjectFlag::Created);
}

#ifndef MAGNUM_TARGET_WEBGL
std::string Mesh::label() {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug->getLabelImplementation(GL_VERTEX_ARRAY, _id);
    #else
    return Context::current().state().debug->getLabelImplementation(GL_VERTEX_ARRAY_KHR, _id);
    #endif
}

Mesh& Mesh::setLabelInternal(const Containers::ArrayView<const char> label) {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug->labelImplementation(GL_VERTEX_ARRAY, _id, label);
    #else
    Context::current().state().debug->labelImplementation(GL_VERTEX_ARRAY_KHR, _id, label);
    #endif
    return *this;
}
#endif

MeshIndexType Mesh::indexType() const {
    CORRADE_ASSERT(_indexBuffer.id(), "Mesh::indexType(): mesh is not indexed", {});
    return _indexType;
}

UnsignedInt Mesh::indexTypeSize() const {
    CORRADE_ASSERT(_indexBuffer.id(), "Mesh::indexTypeSize(): mesh is not indexed", {});

    switch(_indexType) {
        case MeshIndexType::UnsignedByte: return 1;
        case MeshIndexType::UnsignedShort: return 2;
        case MeshIndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Mesh& Mesh::addVertexBufferInstanced(Buffer& buffer, const UnsignedInt divisor, const GLintptr offset, const GLsizei stride, const DynamicAttribute& attribute) {
    AttributeLayout l{buffer,
        attribute.location(),
        GLint(attribute.components()),
        GLenum(attribute.dataType()),
        attribute.kind(),
        offset,
        stride,
        divisor};
    attributePointerInternal(l);
    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer& buffer, GLintptr offset, MeshIndexType type, UnsignedInt start, UnsignedInt end) {
    CORRADE_ASSERT(buffer.id(),
        "GL::Mesh::setIndexBuffer(): empty or moved-out Buffer instance was passed", *this);
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(buffer.targetHint() == Buffer::TargetHint::ElementArray,
        "GL::Mesh::setIndexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::ElementArray << "but got" << buffer.targetHint(), *this);
    #endif

    _indexBuffer = Buffer::wrap(buffer.id());
    _indexOffset = offset;
    _indexType = type;
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = start;
    _indexEnd = end;
    #else
    static_cast<void>(start);
    static_cast<void>(end);
    #endif
    (this->*Context::current().state().mesh->bindIndexBufferImplementation)(buffer);
    return *this;
}

void Mesh::draw(AbstractShaderProgram& shader) {
    CORRADE_ASSERT(_countSet, "GL::Mesh::draw(): setCount() was never called, probably a mistake?", );

    /* Nothing to draw, exit without touching any state */
    if(!_count || !_instanceCount) return;

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
    const Implementation::MeshState& state = *Context::current().state().mesh;

    (this->*state.bindImplementation)();

    /* Non-instanced mesh */
    if(instanceCount == 1) {
        /* Non-indexed mesh */
        if(!_indexBuffer.id()) {
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
            CORRADE_ASSERT(false, "GL::Mesh::draw(): desktop OpenGL is required for base vertex specification in indexed meshes", );
            #endif

        /* Indexed mesh */
        } else {
            /** @todo re-enable for WebGL 2.0 when glDrawRangeElements() no longer crashes Firefox */
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /* Indexed mesh with specified range */
            if(indexEnd) {
                glDrawRangeElements(GLenum(_primitive), indexStart, indexEnd, count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset));

            /* Indexed mesh */
            } else
            #elif !defined(MAGNUM_TARGET_GLES2)
            static_cast<void>(indexStart);
            static_cast<void>(indexEnd);
            #endif
            {
                glDrawElements(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset));
            }
        }

    /* Instanced mesh */
    } else {
        /* Non-indexed mesh */
        if(!_indexBuffer.id()) {
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
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            #ifndef MAGNUM_TARGET_GLES
            /* Indexed mesh with base vertex and base instance */
            if(baseInstance) {
                glDrawElementsInstancedBaseVertexBaseInstance(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount, baseVertex, baseInstance);

            /* Indexed mesh with base vertex */
            } else
            #endif
            {
                glDrawElementsInstancedBaseVertex(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount, baseVertex);
            }
            #else
            CORRADE_ASSERT(false, "GL::Mesh::draw(): OpenGL ES 3.2 or desktop GL is required for base vertex specification in indexed meshes", );
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

#ifndef MAGNUM_TARGET_GLES
void Mesh::drawInternal(TransformFeedback& xfb, const UnsignedInt stream, const Int instanceCount) {
    const Implementation::MeshState& state = *Context::current().state().mesh;

    (this->*state.bindImplementation)();

    /* Default stream */
    if(stream == 0) {
        /* Non-instanced mesh */
        if(instanceCount == 1) glDrawTransformFeedback(GLenum(_primitive), xfb.id());

        /* Instanced mesh */
        else glDrawTransformFeedbackInstanced(GLenum(_primitive), xfb.id(), instanceCount);

    /* Specific stream */
    } else {
        /* Non-instanced mesh */
        if(instanceCount == 1) glDrawTransformFeedbackStream(GLenum(_primitive), xfb.id(), stream);

        /* Instanced mesh */
        else glDrawTransformFeedbackStreamInstanced(GLenum(_primitive), xfb.id(), stream, instanceCount);
    }

    (this->*state.unbindImplementation)();
}

void Mesh::draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream) {
    /* Nothing to draw, exit without touching any state */
    if(!_instanceCount) return;

    shader.use();

    drawInternal(xfb, stream, _instanceCount);
}
#endif

void Mesh::bindVAOImplementationDefault(GLuint) {}

void Mesh::bindVAOImplementationVAO(const GLuint id) {
    #ifndef MAGNUM_TARGET_GLES2
    glBindVertexArray
    #else
    glBindVertexArrayOES
    #endif
        (Context::current().state().mesh->currentVAO = id);
}

void Mesh::bindVAO() {
    GLuint& current = Context::current().state().mesh->currentVAO;
    if(current != _id) {
        /* Binding the VAO finally creates it */
        _flags |= ObjectFlag::Created;
        bindVAOImplementationVAO(_id);
    }
}

void Mesh::createImplementationDefault() {
    _id = 0;
    _flags |= ObjectFlag::Created;

    static_assert(sizeof(_attributes) >= sizeof(std::vector<AttributeLayout>),
        "attribute storage buffer size too small");
    new(&_attributes) std::vector<AttributeLayout>;
}

void Mesh::createImplementationVAO() {
    #ifndef MAGNUM_TARGET_GLES2
    glGenVertexArrays(1, &_id);
    #else
    glGenVertexArraysOES(1, &_id);
    #endif
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::createImplementationVAODSA() {
    glCreateVertexArrays(1, &_id);
    _flags |= ObjectFlag::Created;
}
#endif

void Mesh::moveConstructImplementationDefault(Mesh&& other) {
    new(&_attributes) std::vector<AttributeLayout>{std::move(*reinterpret_cast<std::vector<AttributeLayout>*>(&other._attributes))};
}

void Mesh::moveConstructImplementationVAO(Mesh&&) {}

void Mesh::moveAssignImplementationDefault(Mesh&& other) {
    std::swap(*reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes),
        *reinterpret_cast<std::vector<AttributeLayout>*>(&other._attributes));
}

void Mesh::moveAssignImplementationVAO(Mesh&&) {}

void Mesh::destroyImplementationDefault() {
    reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes)->~vector();
}

void Mesh::destroyImplementationVAO() {
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteVertexArrays(1, &_id);
    #else
    glDeleteVertexArraysOES(1, &_id);
    #endif
}

void Mesh::attributePointerInternal(const Buffer& buffer, const GLuint location, const GLint size, const GLenum type, const DynamicAttribute::Kind kind, const GLintptr offset, const GLsizei stride, const GLuint divisor) {
    AttributeLayout l{buffer, location, size, type, kind, offset, stride, divisor};
    attributePointerInternal(l);
}

void Mesh::attributePointerInternal(AttributeLayout& attribute) {
    CORRADE_ASSERT(attribute.buffer.id(),
        "GL::Mesh::addVertexBuffer(): empty or moved-out Buffer instance was passed", );
    (this->*Context::current().state().mesh->attributePointerImplementation)(attribute);
}

void Mesh::attributePointerImplementationDefault(AttributeLayout& attribute) {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(attribute.buffer.targetHint() == Buffer::TargetHint::Array,
        "GL::Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer.targetHint(), );
    #endif

    reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes)->push_back(attribute);
}

void Mesh::attributePointerImplementationVAO(AttributeLayout& attribute) {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(attribute.buffer.targetHint() == Buffer::TargetHint::Array,
        "GL::Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer.targetHint(), );
    #endif

    bindVAO();
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationDSAEXT(AttributeLayout& attribute) {
    _flags |= ObjectFlag::Created;
    glEnableVertexArrayAttribEXT(_id, attribute.location);

    #ifndef MAGNUM_TARGET_GLES2
    if(attribute.kind == DynamicAttribute::Kind::Integral)
        glVertexArrayVertexAttribIOffsetEXT(_id, attribute.buffer.id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
    #ifndef MAGNUM_TARGET_GLES
    else if(attribute.kind == DynamicAttribute::Kind::Long)
        glVertexArrayVertexAttribLOffsetEXT(_id, attribute.buffer.id(), attribute.location, attribute.size, attribute.type, attribute.stride, attribute.offset);
    #endif
    else
    #endif
    {
        glVertexArrayVertexAttribOffsetEXT(_id, attribute.buffer.id(), attribute.location, attribute.size, attribute.type, attribute.kind == DynamicAttribute::Kind::GenericNormalized, attribute.stride, attribute.offset);
    }

    if(attribute.divisor)
        (this->*Context::current().state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
}
#endif

void Mesh::vertexAttribPointer(AttributeLayout& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer.bindInternal(Buffer::TargetHint::Array);

    #ifndef MAGNUM_TARGET_GLES2
    if(attribute.kind == DynamicAttribute::Kind::Integral)
        glVertexAttribIPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    #ifndef MAGNUM_TARGET_GLES
    else if(attribute.kind == DynamicAttribute::Kind::Long)
        glVertexAttribLPointer(attribute.location, attribute.size, attribute.type, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    #endif
    else
    #endif
    {
        glVertexAttribPointer(attribute.location, attribute.size, attribute.type, attribute.kind == DynamicAttribute::Kind::GenericNormalized, attribute.stride, reinterpret_cast<const GLvoid*>(attribute.offset));
    }

    if(attribute.divisor) {
        #ifndef MAGNUM_TARGET_GLES2
        glVertexAttribDivisor(attribute.location, attribute.divisor);
        #else
        (this->*Context::current().state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
        #endif
    }
}

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
    glVertexAttribDivisorANGLE(index, divisor);
}
#ifndef MAGNUM_TARGET_WEBGL
void Mesh::vertexAttribDivisorImplementationEXT(const GLuint index, const GLuint divisor) {
    glVertexAttribDivisorEXT(index, divisor);
}
void Mesh::vertexAttribDivisorImplementationNV(const GLuint index, const GLuint divisor) {
    glVertexAttribDivisorNV(index, divisor);
}
#endif
#endif

void Mesh::bindIndexBufferImplementationDefault(Buffer&) {}

void Mesh::bindIndexBufferImplementationVAO(Buffer& buffer) {
    bindVAO();

    /* Reset ElementArray binding to force explicit glBindBuffer call later */
    /** @todo Do this cleaner way */
    Context::current().state().buffer->bindings[Implementation::BufferState::indexForTarget(Buffer::TargetHint::ElementArray)] = 0;

    buffer.bindInternal(Buffer::TargetHint::ElementArray);
}

void Mesh::bindImplementationDefault() {
    /* Specify vertex attributes */
    for(AttributeLayout& attribute: *reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes))
        vertexAttribPointer(attribute);

    /* Bind index buffer, if the mesh is indexed */
    if(_indexBuffer.id()) _indexBuffer.bindInternal(Buffer::TargetHint::ElementArray);
}

void Mesh::bindImplementationVAO() {
    bindVAO();
}

void Mesh::unbindImplementationDefault() {
    for(const AttributeLayout& attribute: *reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes))
        glDisableVertexAttribArray(attribute.location);
}

void Mesh::unbindImplementationVAO() {}

#ifdef MAGNUM_TARGET_GLES2
void Mesh::drawArraysInstancedImplementationANGLE(const GLint baseVertex, const GLsizei count, const GLsizei instanceCount) {
    glDrawArraysInstancedANGLE(GLenum(_primitive), baseVertex, count, instanceCount);
}

#ifndef MAGNUM_TARGET_WEBGL
void Mesh::drawArraysInstancedImplementationEXT(const GLint baseVertex, const GLsizei count, const GLsizei instanceCount) {
    glDrawArraysInstancedEXT(GLenum(_primitive), baseVertex, count, instanceCount);
}

void Mesh::drawArraysInstancedImplementationNV(const GLint baseVertex, const GLsizei count, const GLsizei instanceCount) {
    glDrawArraysInstancedNV(GLenum(_primitive), baseVertex, count, instanceCount);
}
#endif

void Mesh::drawElementsInstancedImplementationANGLE(const GLsizei count, const GLintptr indexOffset, const GLsizei instanceCount) {
    glDrawElementsInstancedANGLE(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
}

#ifndef MAGNUM_TARGET_WEBGL
void Mesh::drawElementsInstancedImplementationEXT(const GLsizei count, const GLintptr indexOffset, const GLsizei instanceCount) {
    glDrawElementsInstancedEXT(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
}

void Mesh::drawElementsInstancedImplementationNV(const GLsizei count, const GLintptr indexOffset, const GLsizei instanceCount) {
    glDrawElementsInstancedNV(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexOffset), instanceCount);
}
#endif
#endif

}}
