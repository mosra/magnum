/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/GL/AbstractShaderProgram.h"
#endif

namespace Magnum { namespace GL {

namespace {

constexpr MeshPrimitive PrimitiveMapping[]{
    MeshPrimitive::Points,
    MeshPrimitive::Lines,
    MeshPrimitive::LineLoop,
    MeshPrimitive::LineStrip,
    MeshPrimitive::Triangles,
    MeshPrimitive::TriangleStrip,
    MeshPrimitive::TriangleFan,
    MeshPrimitive(~UnsignedInt{}), /* Instances */
    MeshPrimitive(~UnsignedInt{}), /* Faces */
    MeshPrimitive(~UnsignedInt{})  /* Edges */
};

constexpr MeshIndexType IndexTypeMapping[]{
    MeshIndexType::UnsignedByte,
    MeshIndexType::UnsignedShort,
    MeshIndexType::UnsignedInt
};

}

bool hasMeshPrimitive(const Magnum::MeshPrimitive primitive) {
    if(isMeshPrimitiveImplementationSpecific(primitive))
        return true;

    CORRADE_ASSERT(UnsignedInt(primitive) - 1 < Containers::arraySize(PrimitiveMapping),
        "GL::hasPrimitive(): invalid primitive" << primitive, {});
    return UnsignedInt(PrimitiveMapping[UnsignedInt(primitive) - 1]) != ~UnsignedInt{};
}

MeshPrimitive meshPrimitive(const Magnum::MeshPrimitive primitive) {
    if(isMeshPrimitiveImplementationSpecific(primitive))
        return meshPrimitiveUnwrap<GL::MeshPrimitive>(primitive);

    CORRADE_ASSERT(UnsignedInt(primitive) - 1 < Containers::arraySize(PrimitiveMapping),
        "GL::meshPrimitive(): invalid primitive" << primitive, {});
    const MeshPrimitive out = PrimitiveMapping[UnsignedInt(primitive) - 1];
    CORRADE_ASSERT(out != MeshPrimitive(~UnsignedInt{}),
        "GL::meshPrimitive(): unsupported primitive" << primitive, {});
    return out;
}

MeshIndexType meshIndexType(const Magnum::MeshIndexType type) {
    CORRADE_ASSERT(UnsignedInt(type) - 1 < Containers::arraySize(IndexTypeMapping),
        "GL::meshIndexType(): invalid type" << type, {});
    return IndexTypeMapping[UnsignedInt(type) - 1];
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const MeshPrimitive value) {
    debug << "GL::MeshPrimitive" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshPrimitive::value: return debug << "::" #value;
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

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshIndexType value) {
    debug << "GL::MeshIndexType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshIndexType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

struct Mesh::AttributeLayout {
    explicit AttributeLayout(const Buffer& buffer, GLuint location, GLint size, GLenum type, DynamicAttribute::Kind kind, GLintptr offset, GLsizei stride, GLuint divisor) noexcept: buffer{Buffer::wrap(buffer.id())}, location{location}, size{size}, type{type}, kind{kind}, offset{offset}, stride{stride}, divisor{divisor} {}

    AttributeLayout(AttributeLayout&&) noexcept = default;
    AttributeLayout(const AttributeLayout&) noexcept = delete;
    AttributeLayout& operator=(AttributeLayout&&) noexcept = default;
    AttributeLayout& operator=(const AttributeLayout&) noexcept = delete;

    Buffer buffer;
    GLuint location;
    GLint size;
    GLenum type;
    DynamicAttribute::Kind kind;
    GLintptr offset;
    GLsizei stride;
    GLuint divisor;
};

UnsignedInt Mesh::maxVertexAttributeStride() {
    #ifdef MAGNUM_TARGET_WEBGL
    /* Defined for WebGL 1 and for the new vertexAttribIPointer in WebGL 2 too:
        https://www.khronos.org/registry/webgl/specs/latest/1.0/index.html#5.14.10
        https://www.khronos.org/registry/webgl/specs/latest/2.0/#3.7.8
    */
    return 255;
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL440))
    #elif !defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
    {
        return 0xffffffffu;
    }

    #ifndef MAGNUM_TARGET_GLES2
    GLint& value = Context::current().state().mesh->maxVertexAttributeStride;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &value);

    return value;
    #endif
    #endif
}

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

Mesh::Mesh(const MeshPrimitive primitive): _primitive{primitive}, _flags{ObjectFlag::DeleteOnDestruction} {
    (this->*Context::current().state().mesh->createImplementation)(true);
}

Mesh::Mesh(NoCreateT) noexcept: _id{0}, _primitive{MeshPrimitive::Triangles}, _flags{ObjectFlag::DeleteOnDestruction} {}

Mesh::~Mesh() {
    const bool deleteObject = _id && (_flags & ObjectFlag::DeleteOnDestruction);

    /* Moved out or not deleting on destruction, nothing to do */
    if(deleteObject) {
        /* Remove current vao from the state */
        GLuint& current = Context::current().state().mesh->currentVAO;
        if(current == _id) current = 0;
    }

    if(_constructed) (this->*Context::current().state().mesh->destroyImplementation)(deleteObject);
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
    if(_constructed || other._constructed)
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

    if(_constructed || other._constructed)
        (this->*Context::current().state().mesh->moveAssignImplementation)(std::move(other));

    return *this;
}

Mesh::Mesh(const GLuint id, const MeshPrimitive primitive, const ObjectFlags flags): _id{id}, _primitive{primitive}, _flags{flags} {
    (this->*Context::current().state().mesh->createImplementation)(false);
}

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

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Mesh& Mesh::addVertexBufferInstanced(Buffer& buffer, const UnsignedInt divisor, const GLintptr offset, const GLsizei stride, const DynamicAttribute& attribute) {
    for(UnsignedInt i = 0; i != attribute.vectors(); ++i)
        attributePointerInternal(AttributeLayout{buffer,
            attribute.location() + i,
            GLint(attribute.components()),
            GLenum(attribute.dataType()),
            attribute.kind(),
            GLintptr(offset + i*attribute.vectorStride()),
            stride,
            divisor});
    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer&& buffer, GLintptr offset, MeshIndexType type, UnsignedInt start, UnsignedInt end) {
    CORRADE_ASSERT(buffer.id(),
        "GL::Mesh::setIndexBuffer(): empty or moved-out Buffer instance was passed", *this);
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(buffer.targetHint() == Buffer::TargetHint::ElementArray,
        "GL::Mesh::setIndexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::ElementArray << "but got" << buffer.targetHint(), *this);
    #endif

    /* It's IMPORTANT to do this *before* the _indexBuffer is set, since the
       bindVAO() function called from here is resetting element buffer state
       tracker to _indexBuffer.id(). */
    (this->*Context::current().state().mesh->bindIndexBufferImplementation)(buffer);

    _indexBuffer = std::move(buffer);
    _indexOffset = offset;
    _indexType = type;
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = start;
    _indexEnd = end;
    #else
    static_cast<void>(start);
    static_cast<void>(end);
    #endif
    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer& buffer, const GLintptr offset, const MeshIndexType type, const UnsignedInt start, const UnsignedInt end) {
    setIndexBuffer(Buffer::wrap(buffer.id(), buffer.targetHint()), offset, type, start, end);
    return *this;
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
            CORRADE_ASSERT_UNREACHABLE("GL::Mesh::draw(): desktop OpenGL is required for base vertex specification in indexed meshes", );
            #endif

        /* Indexed mesh */
        } else {
            /** @todo re-enable once https://github.com/kripken/emscripten/pull/7112
                is merged and Emscripten versions with this change are
                widespread enough */
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
            CORRADE_ASSERT_UNREACHABLE("GL::Mesh::draw(): OpenGL ES 3.2 or desktop GL is required for base vertex specification in indexed meshes", );
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
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
Mesh& Mesh::draw(AbstractShaderProgram& shader) {
    shader.draw(*this);
    return *this;
}

Mesh& Mesh::draw(AbstractShaderProgram&& shader) {
    shader.draw(*this);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Mesh& Mesh::draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream) {
    shader.drawTransformFeedback(*this, xfb, stream);
    return *this;
}

Mesh& Mesh::draw(AbstractShaderProgram&& shader, TransformFeedback& xfb, UnsignedInt stream) {
    shader.drawTransformFeedback(*this, xfb, stream);
    return *this;
}
#endif
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

        /* Reset element buffer binding, because binding a different VAO with a
           different index buffer will change that binding as well. (GL state,
           what the hell.). The _indexBuffer.id() is the index buffer that's
           already attached to this particular VAO (or 0, if there's none). In
           particular, the setIndexBuffer() buffers call this function *and
           then* sets the _indexBuffer, which means at this point the ID will
           be still 0. */
        Context::current().state().buffer->bindings[Implementation::BufferState::indexForTarget(Buffer::TargetHint::ElementArray)] = _indexBuffer.id();
    }
}

void Mesh::createImplementationDefault(bool) {
    _id = 0;
    _flags |= ObjectFlag::Created;

    static_assert(sizeof(_attributes) >= sizeof(std::vector<AttributeLayout>),
        "attribute storage buffer size too small");
    new(&_attributes) std::vector<AttributeLayout>;
    _constructed = true;
}

void Mesh::createImplementationVAO(const bool createObject) {
    if(createObject) {
        #ifndef MAGNUM_TARGET_GLES2
        glGenVertexArrays(1, &_id);
        #else
        glGenVertexArraysOES(1, &_id);
        #endif
        CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
    }

    static_assert(sizeof(_attributes) >= sizeof(std::vector<Buffer>),
        "attribute storage buffer size too small");
    new(&_attributes) std::vector<Buffer>;
    _constructed = true;
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::createImplementationVAODSA(const bool createObject) {
    if(createObject) {
        glCreateVertexArrays(1, &_id);
        _flags |= ObjectFlag::Created;
    }

    static_assert(sizeof(_attributes) >= sizeof(std::vector<Buffer>),
        "attribute storage buffer size too small");
    new(&_attributes) std::vector<Buffer>;
    _constructed = true;
}
#endif

void Mesh::moveConstructImplementationDefault(Mesh&& other) {
    new(&_attributes) std::vector<AttributeLayout>{std::move(*reinterpret_cast<std::vector<AttributeLayout>*>(&other._attributes))};
    _constructed = true;
}

void Mesh::moveConstructImplementationVAO(Mesh&& other) {
    new(&_attributes) std::vector<Buffer>{std::move(*reinterpret_cast<std::vector<Buffer>*>(&other._attributes))};
    _constructed = true;
}

/* In the move construction / assignment we need to stay noexcept, which means
   it's only possible to swap or move-construct the vector, can't delete or
   allocate. So, in the particular case where `this` is constructed and `other`
   is not, we do a "partial swap" -- `other` gets our data and our data gets
   emptied. */

void Mesh::moveAssignImplementationDefault(Mesh&& other) {
    if(_constructed && other._constructed)
        std::swap(*reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes),
            *reinterpret_cast<std::vector<AttributeLayout>*>(&other._attributes));
    else if(_constructed && !other._constructed) {
        other._constructed = true;
        new(&other._attributes) std::vector<AttributeLayout>{std::move(*reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes))};
    } else if(!_constructed && other._constructed) {
        _constructed = true;
        new(&_attributes) std::vector<AttributeLayout>{std::move(*reinterpret_cast<std::vector<AttributeLayout>*>(&other._attributes))};
    }
}

void Mesh::moveAssignImplementationVAO(Mesh&& other) {
    if(_constructed && other._constructed)
        std::swap(*reinterpret_cast<std::vector<Buffer>*>(&_attributes),
            *reinterpret_cast<std::vector<Buffer>*>(&other._attributes));
    else if(_constructed && !other._constructed) {
        other._constructed = true;
        new(&other._attributes) std::vector<Buffer>{std::move(*reinterpret_cast<std::vector<Buffer>*>(&_attributes))};
    } else if(!_constructed && other._constructed) {
        _constructed = true;
        new(&_attributes) std::vector<Buffer>{std::move(*reinterpret_cast<std::vector<Buffer>*>(&other._attributes))};
    }
}

void Mesh::destroyImplementationDefault(bool) {
    reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes)->~vector();
}

void Mesh::destroyImplementationVAO(const bool deleteObject) {
    if(deleteObject) {
        #ifndef MAGNUM_TARGET_GLES2
        glDeleteVertexArrays(1, &_id);
        #else
        glDeleteVertexArraysOES(1, &_id);
        #endif
    }

    reinterpret_cast<std::vector<Buffer>*>(&_attributes)->~vector();
}

void Mesh::attributePointerInternal(const Buffer& buffer, const GLuint location, const GLint size, const GLenum type, const DynamicAttribute::Kind kind, const GLintptr offset, const GLsizei stride, const GLuint divisor) {
    attributePointerInternal(AttributeLayout{buffer, location, size, type, kind, offset, stride, divisor});
}

void Mesh::attributePointerInternal(AttributeLayout&& attribute) {
    CORRADE_ASSERT(attribute.buffer.id(),
        "GL::Mesh::addVertexBuffer(): empty or moved-out Buffer instance was passed", );
    (this->*Context::current().state().mesh->attributePointerImplementation)(std::move(attribute));
}

void Mesh::attributePointerImplementationDefault(AttributeLayout&& attribute) {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(attribute.buffer.targetHint() == Buffer::TargetHint::Array,
        "GL::Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer.targetHint(), );
    #endif

    reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes)->push_back(std::move(attribute));
}

void Mesh::attributePointerImplementationVAO(AttributeLayout&& attribute) {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(attribute.buffer.targetHint() == Buffer::TargetHint::Array,
        "GL::Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer.targetHint(), );
    #endif

    bindVAO();
    vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationVAODSA(AttributeLayout&& attribute) {
    glEnableVertexArrayAttrib(_id, attribute.location);

    #ifndef MAGNUM_TARGET_GLES2
    if(attribute.kind == DynamicAttribute::Kind::Integral)
        glVertexArrayAttribIFormat(_id, attribute.location, attribute.size, attribute.type, 0);
    #ifndef MAGNUM_TARGET_GLES
    else if(attribute.kind == DynamicAttribute::Kind::Long)
        glVertexArrayAttribLFormat(_id, attribute.location, attribute.size, attribute.type, 0);
    #endif
    else
    #endif
    {
        glVertexArrayAttribFormat(_id, attribute.location, attribute.size, attribute.type, attribute.kind == DynamicAttribute::Kind::GenericNormalized, 0);
    }

    glVertexArrayAttribBinding(_id, attribute.location, attribute.location);
    CORRADE_INTERNAL_ASSERT(attribute.stride != 0);
    glVertexArrayVertexBuffer(_id, attribute.location, attribute.buffer.id(), attribute.offset, attribute.stride);

    if(attribute.divisor)
        (this->*Context::current().state().mesh->vertexAttribDivisorImplementation)(attribute.location, attribute.divisor);
}

#ifdef CORRADE_TARGET_WINDOWS
void Mesh::attributePointerImplementationVAODSAIntelWindows(AttributeLayout&& attribute) {
    /* See the "intel-windows-broken-dsa-integer-vertex-attributes" workaround
       for more information. */
    if(attribute.kind == DynamicAttribute::Kind::Integral)
        return attributePointerImplementationVAO(std::move(attribute));
    else
        return attributePointerImplementationVAODSA(std::move(attribute));
}
#endif
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
void Mesh::vertexAttribDivisorImplementationVAODSA(const GLuint index, const GLuint divisor) {
    glVertexArrayBindingDivisor(_id, index, divisor);
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

void Mesh::acquireVertexBuffer(Buffer&& buffer) {
    (this->*Context::current().state().mesh->acquireVertexBufferImplementation)(std::move(buffer));
}

void Mesh::acquireVertexBufferImplementationDefault(Buffer&& buffer) {
    /* The last added buffer should be this one, replace it with a owning one */
    auto& attributes = *reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes);
    CORRADE_INTERNAL_ASSERT(!attributes.empty() && attributes.back().buffer.id() == buffer.id() && buffer.id());
    attributes.back().buffer.release(); /* so we swap back a zero ID */
    attributes.back().buffer = std::move(buffer);
}

void Mesh::acquireVertexBufferImplementationVAO(Buffer&& buffer) {
    CORRADE_INTERNAL_ASSERT(buffer.id());
    /* With VAOs we are not maintaining the attribute list, so just store the
       buffer directly */
    reinterpret_cast<std::vector<Buffer>*>(&_attributes)->emplace_back(std::move(buffer));
}

void Mesh::bindIndexBufferImplementationDefault(Buffer&) {}

void Mesh::bindIndexBufferImplementationVAO(Buffer& buffer) {
    bindVAO();

    /* Binding the VAO in the above function resets element buffer binding,
       meaning the following will always cause the glBindBuffer() to be called */
    buffer.bindInternal(Buffer::TargetHint::ElementArray);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::bindIndexBufferImplementationVAODSA(Buffer& buffer) {
    glVertexArrayElementBuffer(_id, buffer.id());
}
#endif

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
    for(const AttributeLayout& attribute: *reinterpret_cast<std::vector<AttributeLayout>*>(&_attributes)) {
        glDisableVertexAttribArray(attribute.location);

        /* Reset also the divisor back so it doesn't affect  */
        if(attribute.divisor) {
            #ifndef MAGNUM_TARGET_GLES2
            glVertexAttribDivisor(attribute.location, 0);
            #else
            (this->*Context::current().state().mesh->vertexAttribDivisorImplementation)(attribute.location, 0);
            #endif
        }
    }
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
