/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif
#include <Corrade/Utility/Algorithms.h>
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
    MeshPrimitive(~UnsignedInt{}), /* Edges */
    MeshPrimitive(~UnsignedInt{})  /* Meshlets */
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
    if(isMeshIndexTypeImplementationSpecific(type))
        return meshIndexTypeUnwrap<GL::MeshIndexType>(type);

    CORRADE_ASSERT(UnsignedInt(type) - 1 < Containers::arraySize(IndexTypeMapping),
        "GL::meshIndexType(): invalid type" << type, {});
    return IndexTypeMapping[UnsignedInt(type) - 1];
}

UnsignedInt meshIndexTypeSize(const MeshIndexType type) {
    switch(type) {
        case MeshIndexType::UnsignedByte: return 1;
        case MeshIndexType::UnsignedShort: return 2;
        case MeshIndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE("GL::meshIndexTypeSize(): invalid type" << type, {});
}

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

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
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

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

struct Mesh::AttributeLayout {
    /* Records attribute layout with a non-owning Buffer reference. Used as a
       temporary data holder when VAOs are used, saved to the _attributes
       member when not. If a Buffer instance needs to be owned, it's
       subsequently moved in (usually with ObjectFlag::DeleteOnDestruction
       set). */
    explicit AttributeLayout(const Buffer& buffer, GLuint location, GLint size, GLenum type, DynamicAttribute::Kind kind, GLintptr offset, GLsizei stride, GLuint divisor) noexcept: buffer{Buffer::wrap(buffer.id())}, location{UnsignedByte(location)},
        /* Have to use () instead of {}, otherwise GCC 4.8 complains that
           parameter kind is set but not used */
        kindSize(UnsignedByte(kind)), type{UnsignedShort(type)}, divisor{divisor}, offsetStride{(UnsignedLong(offset) << 16)|stride}
    {
        CORRADE_INTERNAL_ASSERT(location < 256 && type < 65536 && UnsignedLong(offset) < (1ull << 48) && stride < 65536);
        #ifndef MAGNUM_TARGET_GLES
        if(size == GL_BGRA) {
            kindSize |= 0 << 2;
        } else
        #endif
        {
            CORRADE_INTERNAL_ASSERT(size >= 1 && size <= 4);
            kindSize |= size << 2;
        }
    }

    /* Takes ownership of a Buffer instance. Abuses the _attributes storage in
       cases where VAOs are used. That wastes a bit of space as only 8 out of
       the 24 bytes is actually used, but that should be okay as there's likely
       only very few buffers (compared to attributes, which can be quite
       many). */
    explicit AttributeLayout(Buffer&& buffer): buffer{Utility::move(buffer)}, location{}, kindSize{}, type{}, divisor{}, offsetStride{} {}

    AttributeLayout(AttributeLayout&&) noexcept = default;
    AttributeLayout(const AttributeLayout&) noexcept = delete;
    AttributeLayout& operator=(AttributeLayout&&) noexcept = default;
    AttributeLayout& operator=(const AttributeLayout&) noexcept = delete;

    DynamicAttribute::Kind kind() const {
        return DynamicAttribute::Kind(kindSize & 0x03);
    }

    GLint size() const {
        const GLint size = kindSize >> 2;
        #ifndef MAGNUM_TARGET_GLES
        if(!size) return GL_BGRA;
        #endif
        return size;
    }

    GLintptr offset() const {
        return offsetStride >> 16;
    };

    GLsizei stride() const {
        return offsetStride & 0xffff;
    }

    /* Packing to just 20 bytes would be possible with unwrapping the buffer,
       keeping just the ID from it putting the 2-bit ObjectFlags into the
       remaining free bits in `kindSize`, at the cost of extra logic that would
       be needed to properly destruct it if it's owned. Then, on 32-bit WebGL
       we don't need the offset to be more than 32 bits and the stride can be
       just 1 byte, leaving us with just 17 bytes. The last byte could be then
       stolen from the `divisor`, for example. Not doing that as I don't feel
       it's necessary to optimize that much, additionally the AttributeLayout
       instances are only stored if VAOs are disabled, which is a rare
       scenario. */

    /* 4 bytes +
       2 bits:  if unwrapped (for flags, the TargetHint is always Array) */
    Buffer buffer;
    /* 4 bits:  GPUs have usually max 8 or 16 locations */
    UnsignedByte location;
    /* 2 bits for a kind +
       3 bits for size: kind is just 4 values, size is  1, 2, 3, 4 components
                or GL_BGRA, which is treated as 0 */
    UnsignedByte kindSize;
    /* 2 bytes: the type values are all just 16-bit */
    UnsignedShort type;
    /* 4 bytes: not sure what's the limit on this, but looks like it can be a
                full 32 bit range, same as vertex / element count (unlike in
                Vulkan, where it's often either just 0 or 1) */
    GLuint divisor;
    /* 6 bytes offset +
       2 byte stride: offset has to be more than 32 bits to work with buffers
                larger than 4 GB, but 48 bits (256 TB?) could be enough. Max
                stride is usually 2048, it's just 256 on WebGL so 16 bits for
                it should be enough. */
    UnsignedLong offsetStride;
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
    GLint& value = Context::current().state().mesh.maxVertexAttributeStride;

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
        Context::current().state().mesh.maxElementIndex;

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
    GLint& value = Context::current().state().mesh.maxElementsIndices;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &value);

    return value;
}

Int Mesh::maxElementsVertices() {
    GLint& value = Context::current().state().mesh.maxElementsVertices;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &value);

    return value;
}
#endif

Mesh::Mesh(const MeshPrimitive primitive): _primitive{primitive}, _flags{ObjectFlag::DeleteOnDestruction} {
    Context::current().state().mesh.createImplementation(*this);
}

Mesh::Mesh(NoCreateT) noexcept: _id{0}, _primitive{MeshPrimitive::Triangles}, _flags{ObjectFlag::DeleteOnDestruction} {}

Mesh::~Mesh() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction))
        return;

    /* Remove current vao from the state */
    GLuint& current = Context::current().state().mesh.currentVAO;
    if(current == _id) current = 0;

    Context::current().state().mesh.destroyImplementation(*this);
}

Mesh::Mesh(Mesh&& other) noexcept: _id(other._id), _primitive(other._primitive), _flags{other._flags}, _countSet{other._countSet}, _count(other._count), _baseVertex{other._baseVertex}, _instanceCount{other._instanceCount},
    #ifndef MAGNUM_TARGET_GLES
    _baseInstance{other._baseInstance},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart(other._indexStart), _indexEnd(other._indexEnd),
    #endif
    _indexType(other._indexType), _indexBufferOffset(other._indexBufferOffset), _indexOffset(other._indexOffset), _indexBuffer{Utility::move(other._indexBuffer)},
    _attributes{Utility::move(other._attributes)}
{
    other._id = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    using Utility::swap;
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
    swap(_indexBufferOffset, other._indexBufferOffset);
    swap(_indexType, other._indexType);
    swap(_indexOffset, other._indexOffset);
    swap(_indexBuffer, other._indexBuffer);
    swap(_attributes, other._attributes);

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
Containers::String Mesh::label() {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug.getLabelImplementation(GL_VERTEX_ARRAY, _id);
    #else
    return Context::current().state().debug.getLabelImplementation(GL_VERTEX_ARRAY_KHR, _id);
    #endif
}

Mesh& Mesh::setLabel(const Containers::StringView label) {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug.labelImplementation(GL_VERTEX_ARRAY, _id, label);
    #else
    Context::current().state().debug.labelImplementation(GL_VERTEX_ARRAY_KHR, _id, label);
    #endif
    return *this;
}
#endif

MeshIndexType Mesh::indexType() const {
    CORRADE_ASSERT(_indexBuffer.id(), "GL::Mesh::indexType(): mesh is not indexed", {});
    return _indexType;
}

#ifdef MAGNUM_BUILD_DEPRECATED
UnsignedInt Mesh::indexTypeSize() const {
    CORRADE_ASSERT(_indexBuffer.id(), "GL::Mesh::indexTypeSize(): mesh is not indexed", {});

    return meshIndexTypeSize(_indexType);
}
#endif

Mesh& Mesh::setIndexOffset(GLintptr offset) {
    CORRADE_ASSERT(_indexBuffer.id(),
        "GL::Mesh::setIndexOffset(): mesh is not indexed", *this);
    _indexOffset = offset;
    return *this;
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
    Context::current().state().mesh.bindIndexBufferImplementation(*this, buffer);

    _indexBuffer = Utility::move(buffer);
    _indexBufferOffset = offset;
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

void Mesh::drawInternal(const Containers::ArrayView<const UnsignedInt>& counts, const Containers::ArrayView<const UnsignedInt>& vertexOffsets,
    #ifdef CORRADE_TARGET_32BIT
    const Containers::ArrayView<const UnsignedInt>& indexOffsets
    #else
    const Containers::ArrayView<const UnsignedLong>& indexOffsets
    #endif
) {
    /* Not asserting for _instanceCount == 1, as this is *not* taken from the
       original mesh, the counts/vertexOffsets/indexOffsets completely describe
       the range being drawn */

    const Implementation::MeshState& state = Context::current().state().mesh;
    state.bindImplementation(*this);

    /* Non-indexed meshes */
    if(!_indexBuffer.id()) {
        CORRADE_ASSERT(vertexOffsets.size() == counts.size(),
            "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "vertex offset items but got" << vertexOffsets.size(), );

        #ifndef MAGNUM_TARGET_GLES
        glMultiDrawArrays
        #else
        state.multiDrawArraysImplementation
        #endif
            (GLenum(_primitive), reinterpret_cast<const GLint*>(vertexOffsets.data()), reinterpret_cast<const GLsizei*>(counts.data()), counts.size());

    /* Indexed meshes */
    } else {
        CORRADE_ASSERT(indexOffsets.size() == counts.size(),
            "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "index offset items but got" << indexOffsets.size(), );

        /* Indexed meshes */
        if(vertexOffsets.isEmpty()) {
            #ifndef MAGNUM_TARGET_GLES
            glMultiDrawElements
            #else
            state.multiDrawElementsImplementation
            #endif
                (GLenum(_primitive), reinterpret_cast<const GLsizei*>(counts.data()), GLenum(_indexType), reinterpret_cast<const void* const*>(indexOffsets.data()), counts.size());

        /* Indexed meshes with base vertex */
        } else {
            CORRADE_ASSERT(vertexOffsets.size() == counts.size(),
                "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "vertex offset items but got" << vertexOffsets.size(), );

            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            #ifndef MAGNUM_TARGET_GLES
            glMultiDrawElementsBaseVertex
            #else
            state.multiDrawElementsBaseVertexImplementation
            #endif
                (GLenum(_primitive), reinterpret_cast<const GLsizei*>(counts.data()), GLenum(_indexType), reinterpret_cast<const void* const*>(indexOffsets.data()), counts.size(), reinterpret_cast<const GLint*>(vertexOffsets.data()));
            #else
            CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): indexed mesh multi-draw with base vertex specification possible only since WebGL 2.0", );
            #endif
        }
    }

    state.unbindImplementation(*this);
}

#ifdef MAGNUM_TARGET_GLES
void Mesh::drawInternal(const Containers::ArrayView<const UnsignedInt>& counts, const Containers::ArrayView<const UnsignedInt>& instanceCounts, const Containers::ArrayView<const UnsignedInt>& vertexOffsets,
    #ifdef CORRADE_TARGET_32BIT
    const Containers::ArrayView<const UnsignedInt>& indexOffsets
    #else
    const Containers::ArrayView<const UnsignedLong>& indexOffsets
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    , const Containers::ArrayView<const UnsignedInt>& instanceOffsets
    #endif
) {
    const Implementation::MeshState& state = Context::current().state().mesh;
    state.bindImplementation(*this);

    CORRADE_ASSERT(instanceCounts.size() == counts.size(),
        "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "instance count items but got" << instanceCounts.size(), );

    /* Non-indexed instanced meshes */
    if(!_indexBuffer.id()) {
        CORRADE_ASSERT(vertexOffsets.size() == counts.size(),
            "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "vertex offset items but got" << vertexOffsets.size(), );

        /* Non-indexed instanced meshes */
        #ifndef MAGNUM_TARGET_GLES2
        if(instanceOffsets.isEmpty())
        #endif
        {
            state.multiDrawArraysInstancedImplementation(GLenum(_primitive), reinterpret_cast<const GLint*>(vertexOffsets.data()), reinterpret_cast<const GLsizei*>(counts.data()), reinterpret_cast<const GLsizei*>(instanceCounts.data()), counts.size());
        }

        /* Non-indexed instanced meshes with base instance */
        #ifndef MAGNUM_TARGET_GLES2
        else {
            CORRADE_ASSERT(instanceOffsets.size() == counts.size(),
                "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "instance offset items but got" << instanceOffsets.size(), );

            state.multiDrawArraysInstancedBaseInstanceImplementation(GLenum(_primitive), reinterpret_cast<const GLint*>(vertexOffsets.data()), reinterpret_cast<const GLsizei*>(counts.data()), reinterpret_cast<const GLsizei*>(instanceCounts.data()),
            reinterpret_cast<const GLuint*>(instanceOffsets.data()), counts.size());
        }
        #endif

    /* Indexed meshes */
    } else {
        CORRADE_ASSERT(indexOffsets.size() == counts.size(),
            "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "index offset items but got" << indexOffsets.size(), );

        /* Indexed meshes */
        if(vertexOffsets.isEmpty()
            #ifndef MAGNUM_TARGET_GLES2
            && instanceOffsets.isEmpty()
            #endif
        ) {
            state.multiDrawElementsInstancedImplementation(GLenum(_primitive), reinterpret_cast<const GLsizei*>(counts.data()), GLenum(_indexType), reinterpret_cast<const void* const*>(indexOffsets.data()), reinterpret_cast<const GLsizei*>(instanceCounts.data()), counts.size());

        /* Indexed meshes with base vertex / base instance. According to the
           extension spec both have to be present, not just one. */
        } else {
            CORRADE_ASSERT(vertexOffsets.size() == counts.size(),
                "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "vertex offset items but got" << vertexOffsets.size(), );
            #ifndef MAGNUM_TARGET_GLES2
            CORRADE_ASSERT(instanceOffsets.size() == counts.size(),
                "GL::AbstractShaderProgram::draw(): expected" << counts.size() << "instance offset items but got" << instanceOffsets.size(), );
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            state.multiDrawElementsInstancedBaseVertexBaseInstanceImplementation(GLenum(_primitive), reinterpret_cast<const GLsizei*>(counts.data()), GLenum(_indexType), reinterpret_cast<const void* const*>(indexOffsets.data()), reinterpret_cast<const GLsizei*>(instanceCounts.data()), reinterpret_cast<const GLint*>(vertexOffsets.data()),
            reinterpret_cast<const GLuint*>(instanceOffsets.data()), counts.size());
            #else
            CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): instanced indexed mesh multi-draw with base vertex specification possible only since OpenGL ES 3.0 and WebGL 2.0", );
            #endif
        }
    }

    state.unbindImplementation(*this);
}
#endif

void Mesh::drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets) {
    #ifdef CORRADE_TARGET_32BIT
    /* If all views are contiguous and we're on 32-bit, call the implementation
       directly */
    if(counts.isContiguous() && vertexOffsets.isContiguous() && indexOffsets.isContiguous())
        return drawInternal(counts.asContiguous(), vertexOffsets.asContiguous(), indexOffsets.asContiguous());
    #endif

    /* Otherwise allocate contiguous copies. While it's possible that some
       views could have been contigous already and some not, such scenario is
       unlikely to make a practical sense, so we'll allocate & copy always. */
    Containers::ArrayView<UnsignedInt> countsContiguous;
    Containers::ArrayView<UnsignedInt> vertexOffsetsContiguous;
    #ifdef CORRADE_TARGET_32BIT
    Containers::ArrayView<UnsignedInt>
    #else
    Containers::ArrayView<UnsignedLong>
    #endif
        indexOffsetsContiguous;
    Containers::ArrayTuple data{
        {NoInit, counts.size(), countsContiguous},
        {NoInit, vertexOffsets.size(), vertexOffsetsContiguous},
        /* On 64-bit we'll be filling just the lower 32 bits so zero-init the
           array. On 32-bit we'll overwrite it completely, so NoInit is fine. */
        {
            #ifdef CORRADE_TARGET_32BIT
            NoInit
            #else
            ValueInit
            #endif
            , indexOffsets.size(), indexOffsetsContiguous}
    };
    Utility::copy(counts, countsContiguous);
    Utility::copy(vertexOffsets, vertexOffsetsContiguous);
    Utility::copy(indexOffsets,
        #ifdef CORRADE_TARGET_32BIT
        indexOffsetsContiguous
        #else
        /* Write to the lower 32 bits of the index offsets, which is the
           leftmost bits on Little-Endian and rightmost on Big-Endian. On LE it
           could be just Containers::arrayCast<const UnsignedInt>(indexOffsets)
           but to minimize a chance of error on BE platforms that are hard to
           test on, the same code is used for both. */
        Containers::arrayCast<2, UnsignedInt>(stridedArrayView(indexOffsetsContiguous)).transposed<0, 1>()[
            #ifndef CORRADE_TARGET_BIG_ENDIAN
            0
            #else
            1
            #endif
        ]
        #endif
    );

    drawInternal(countsContiguous, vertexOffsetsContiguous, indexOffsetsContiguous);
}

#ifndef CORRADE_TARGET_32BIT
void Mesh::drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets) {
    /* If all views are contiguous, call the implementation directly */
    if(counts.isContiguous() && vertexOffsets.isContiguous() && indexOffsets.isContiguous())
        return drawInternal(counts.asContiguous(), vertexOffsets.asContiguous(), indexOffsets.asContiguous());

    /* Otherwise delegate into the 32-bit variant, which will allocate a
       contiguous copy */
    drawInternalStrided(counts, vertexOffsets,
        /* Get the lower 32 bits of the index offsets, which is the leftmost
           bits on Little-Endian and rightmost on Big-Endian. On LE it could be
           just Containers::arrayCast<const UnsignedInt>(indexOffsets) but to
           minimize a chance of error on BE platforms that are hard to test on,
           the same code is used for both. */
        Containers::arrayCast<2, const UnsignedInt>(indexOffsets).transposed<0, 1>()[
            #ifndef CORRADE_TARGET_BIG_ENDIAN
            0
            #else
            1
            #endif
        ]
    );
}
#endif

#ifdef MAGNUM_TARGET_GLES
void Mesh::drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets
    #ifndef MAGNUM_TARGET_GLES2
    , const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets
    #endif
) {
    #ifdef CORRADE_TARGET_32BIT
    /* If all views are contiguous, the mesh specifies either both base vertex
       and base instance or neither and we're on 32-bit, call the
       implementation directly */
    if(counts.isContiguous() && instanceCounts.isContiguous() && vertexOffsets.isContiguous() && indexOffsets.isContiguous()
        #ifndef MAGNUM_TARGET_GLES2
        && instanceOffsets.isContiguous() && (!_indexBuffer.id() || vertexOffsets.size() == instanceOffsets.size())
        #endif
    )
        return drawInternal(counts.asContiguous(), instanceCounts.asContiguous(), vertexOffsets.asContiguous(), indexOffsets.asContiguous()
            #ifndef MAGNUM_TARGET_GLES2
            , instanceOffsets.asContiguous()
            #endif
        );
    #endif

    /* Expected vertex offset and instance offset count. If the mesh is
       indexed, they either have to be both used or both empty. */
    std::size_t expectedVertexOffsetCount = vertexOffsets.size();
    #ifndef MAGNUM_TARGET_GLES2
    std::size_t expectedInstanceOffsetCount = instanceOffsets.size();
    if(_indexBuffer.id()) {
        /* Use counts.size() instead of <the-other>OffsetCount to avoid hitting
           a wrong assert in case the vertex/instance count doesn't match */
        if(expectedVertexOffsetCount && !expectedInstanceOffsetCount)
            expectedInstanceOffsetCount = counts.size();
        else if(expectedInstanceOffsetCount && !expectedVertexOffsetCount)
            expectedVertexOffsetCount = counts.size();
    }
    #endif

    /* Otherwise allocate contiguous copies. While it's possible that some
       views could have been contigous already and some not, such scenario is
       unlikely to make a practical sense, so we'll allocate & copy always. */
    Containers::ArrayView<UnsignedInt> countsContiguous;
    Containers::ArrayView<UnsignedInt> instanceCountsContiguous;
    Containers::ArrayView<UnsignedInt> vertexOffsetsContiguous;
    #ifdef CORRADE_TARGET_32BIT
    Containers::ArrayView<UnsignedInt>
    #else
    Containers::ArrayView<UnsignedLong>
    #endif
        indexOffsetsContiguous;
    #ifndef MAGNUM_TARGET_GLES2
    Containers::ArrayView<UnsignedInt> instanceOffsetsContiguous;
    #endif
    Containers::ArrayTuple data{
        {NoInit, counts.size(), countsContiguous},
        {NoInit, instanceCounts.size(), instanceCountsContiguous},
        /* Zero init vertex offsets if we don't have them */
        vertexOffsets.size() ?
            Containers::ArrayTuple::Item{NoInit, expectedVertexOffsetCount, vertexOffsetsContiguous} :
            Containers::ArrayTuple::Item{ValueInit, expectedVertexOffsetCount, vertexOffsetsContiguous},
        /* On 64-bit we'll be filling just the lower 32 bits so zero-init the
           array. On 32-bit we'll overwrite it completely, so NoInit is fine. */
        {
            #ifdef CORRADE_TARGET_32BIT
            NoInit
            #else
            ValueInit
            #endif
            , indexOffsets.size(), indexOffsetsContiguous},
        #ifndef MAGNUM_TARGET_GLES2
        /* Zero init instance offsets if we don't have them */
        instanceOffsets.size() ?
            Containers::ArrayTuple::Item{NoInit, expectedInstanceOffsetCount, instanceOffsetsContiguous} :
            Containers::ArrayTuple::Item{ValueInit, expectedInstanceOffsetCount, instanceOffsetsContiguous},
        #endif
    };
    Utility::copy(counts, countsContiguous);
    Utility::copy(instanceCounts, instanceCountsContiguous);
    /* Copy vertex offsets only if we have them and leave them zero-init'd
       otherwise */
    if(vertexOffsets.size())
        Utility::copy(vertexOffsets, vertexOffsetsContiguous);
    Utility::copy(indexOffsets,
        #ifdef CORRADE_TARGET_32BIT
        indexOffsetsContiguous
        #else
        /* Write to the lower 32 bits of the index offsets, which is the
           leftmost bits on Little-Endian and rightmost on Big-Endian. On LE it
           could be just Containers::arrayCast<const UnsignedInt>(indexOffsets)
           but to minimize a chance of error on BE platforms that are hard to
           test on, the same code is used for both. */
        Containers::arrayCast<2, UnsignedInt>(stridedArrayView(indexOffsetsContiguous)).transposed<0, 1>()[
            #ifndef CORRADE_TARGET_BIG_ENDIAN
            0
            #else
            1
            #endif
        ]
        #endif
    );
    #ifndef MAGNUM_TARGET_GLES2
    /* Copy instance offsets only if we have them and leave them zero-init'd
       otherwise */
    if(instanceOffsets.size())
        Utility::copy(instanceOffsets, instanceOffsetsContiguous);
    #endif

    drawInternal(countsContiguous, instanceCountsContiguous, vertexOffsetsContiguous, indexOffsetsContiguous
        #ifndef MAGNUM_TARGET_GLES2
        , instanceOffsetsContiguous
        #endif
    );
}

#ifndef CORRADE_TARGET_32BIT
void Mesh::drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets
    #ifndef MAGNUM_TARGET_GLES2
    , const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets
    #endif
) {
    /* If all views are contiguous, the mesh specifies either both base vertex
       and base instance or neither, call the implementation directly */
    if(counts.isContiguous() && instanceCounts.isContiguous() && vertexOffsets.isContiguous() && indexOffsets.isContiguous()
        #ifndef MAGNUM_TARGET_GLES2
        && instanceOffsets.isContiguous() && (!_indexBuffer.id() || vertexOffsets.size() == instanceOffsets.size())
        #endif
    )
        return drawInternal(counts.asContiguous(), instanceCounts.asContiguous(), vertexOffsets.asContiguous(), indexOffsets.asContiguous()
            #ifndef MAGNUM_TARGET_GLES2
            , instanceOffsets.asContiguous()
            #endif
        );

    /* Otherwise delegate into the 32-bit variant, which will allocate a
       contiguous copy */
    drawInternalStrided(counts, instanceCounts, vertexOffsets,
        /* Get the lower 32 bits of the index offsets, which is the leftmost
           bits on Little-Endian and rightmost on Big-Endian. On LE it could be
           just Containers::arrayCast<const UnsignedInt>(indexOffsets) but to
           minimize a chance of error on BE platforms that are hard to test on,
           the same code is used for both. */
        Containers::arrayCast<2, const UnsignedInt>(indexOffsets).transposed<0, 1>()[
            #ifndef CORRADE_TARGET_BIG_ENDIAN
            0
            #else
            1
            #endif
        ]
        #ifndef MAGNUM_TARGET_GLES2
        , instanceOffsets
        #endif
    );
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES2
void Mesh::drawInternal(Int count, Int baseVertex, Int instanceCount, UnsignedInt baseInstance, GLintptr indexOffset, Int indexStart, Int indexEnd)
#else
void Mesh::drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset)
#endif
{
    const Implementation::MeshState& state = Context::current().state().mesh;

    const GLintptr indexByteOffset = _indexBuffer.id() ?
        _indexBufferOffset + indexOffset*meshIndexTypeSize(_indexType) :
        0;

    state.bindImplementation(*this);

    /* Non-instanced mesh */
    if(instanceCount == 1
        #ifdef MAGNUM_TARGET_GLES
        /* See the "angle-instanced-attributes-always-draw-instanced"
           workaround */
        && !_instanced
        #endif
    ) {
        /* Non-indexed mesh */
        if(!_indexBuffer.id()) {
            glDrawArrays(GLenum(_primitive), baseVertex, count);

        /* Indexed mesh with base vertex */
        } else if(baseVertex) {
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            #ifndef MAGNUM_TARGET_GLES2
            /* Indexed mesh with specified range */
            if(indexEnd) {
                #ifndef MAGNUM_TARGET_GLES
                glDrawRangeElementsBaseVertex
                #else
                state.drawRangeElementsBaseVertexImplementation
                #endif
                    (GLenum(_primitive), indexStart, indexEnd, count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset), baseVertex);

            /* Indexed mesh */
            } else
            #endif
            {
                #ifndef MAGNUM_TARGET_GLES
                glDrawElementsBaseVertex
                #else
                state.drawElementsBaseVertexImplementation
                #endif
                    (GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset), baseVertex);
            }
            #else
            CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): indexed mesh draw with base vertex specification possible only since WebGL 2.0", );
            #endif

        /* Indexed mesh */
        } else {
            #ifndef MAGNUM_TARGET_GLES2
            /* Indexed mesh with specified range */
            if(indexEnd) {
                glDrawRangeElements(GLenum(_primitive), indexStart, indexEnd, count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset));

            /* Indexed mesh */
            } else
            #elif !defined(MAGNUM_TARGET_GLES2)
            static_cast<void>(indexStart);
            static_cast<void>(indexEnd);
            #endif
            {
                glDrawElements(GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset));
            }
        }

    /* Instanced mesh */
    } else {
        /* Non-indexed mesh */
        if(!_indexBuffer.id()) {
            #ifndef MAGNUM_TARGET_GLES2
            /* Non-indexed mesh with base instance */
            if(baseInstance) {
                #ifndef MAGNUM_TARGET_GLES
                glDrawArraysInstancedBaseInstance
                #else
                state.drawArraysInstancedBaseInstanceImplementation
                #endif
                    (GLenum(_primitive), baseVertex, count, instanceCount, baseInstance);

            /* Non-indexed mesh */
            } else
            #endif
            {
                #ifndef MAGNUM_TARGET_GLES2
                glDrawArraysInstanced
                #else
                state.drawArraysInstancedImplementation
                #endif
                    (GLenum(_primitive), baseVertex, count, instanceCount);
            }

        /* Indexed mesh with base vertex */
        } else if(baseVertex) {
            #ifndef MAGNUM_TARGET_GLES2
            /* Indexed mesh with base vertex and base instance */
            if(baseInstance) {
                #ifndef MAGNUM_TARGET_GLES
                glDrawElementsInstancedBaseVertexBaseInstance
                #else
                state.drawElementsInstancedBaseVertexBaseInstanceImplementation
                #endif
                    (GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset), instanceCount, baseVertex, baseInstance);

            /* Indexed mesh with base vertex */
            } else {
                #ifndef MAGNUM_TARGET_GLES
                glDrawElementsInstancedBaseVertex
                #else
                state.drawElementsInstancedBaseVertexImplementation
                #endif
                    (GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset), instanceCount, baseVertex);
            }
            #else
            CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): instanced indexed mesh draw with base vertex specification possible only since OpenGL ES 3.0", );
            #endif

        /* Indexed mesh */
        } else {
            #ifndef MAGNUM_TARGET_GLES2
            /* Indexed mesh with base instance */
            if(baseInstance) {
                #ifndef MAGNUM_TARGET_GLES
                glDrawElementsInstancedBaseInstance
                #else
                state.drawElementsInstancedBaseInstanceImplementation
                #endif
                    (GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset), instanceCount, baseInstance);

            /* Instanced mesh */
            } else
            #endif
            {
                #ifndef MAGNUM_TARGET_GLES2
                glDrawElementsInstanced
                #else
                state.drawElementsInstancedImplementation
                #endif
                    (GLenum(_primitive), count, GLenum(_indexType), reinterpret_cast<GLvoid*>(indexByteOffset), instanceCount);
            }
        }
    }

    state.unbindImplementation(*this);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::drawInternal(TransformFeedback& xfb, const UnsignedInt stream, const Int instanceCount) {
    const Implementation::MeshState& state = Context::current().state().mesh;

    state.bindImplementation(*this);

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

    state.unbindImplementation(*this);
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
        (Context::current().state().mesh.currentVAO = id);
}

void Mesh::bindVAO() {
    if(Context::current().state().mesh.currentVAO != _id) {
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
        Context::current().state().buffer.bindings[Implementation::BufferState::indexForTarget(Buffer::TargetHint::ElementArray)] = _indexBuffer.id();
    }
}

void Mesh::createImplementationDefault(Mesh& self) {
    self._id = 0;
    self._flags |= ObjectFlag::Created;
}

void Mesh::createImplementationVAO(Mesh& self) {
    #ifndef MAGNUM_TARGET_GLES2
    glGenVertexArrays(1, &self._id);
    #else
    glGenVertexArraysOES(1, &self._id);
    #endif
    CORRADE_INTERNAL_ASSERT(self._id != Implementation::State::DisengagedBinding);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::createImplementationVAODSA(Mesh& self) {
    glCreateVertexArrays(1, &self._id);
    self._flags |= ObjectFlag::Created;
}
#endif

void Mesh::destroyImplementationDefault(Mesh&) {}

void Mesh::destroyImplementationVAO(Mesh& self) {
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteVertexArrays(1, &self._id);
    #else
    glDeleteVertexArraysOES(1, &self._id);
    #endif
}

void Mesh::attributePointerInternal(const Buffer& buffer, const GLuint location, const GLint size, const GLenum type, const DynamicAttribute::Kind kind, const GLintptr offset, const GLsizei stride, const GLuint divisor) {
    attributePointerInternal(AttributeLayout{buffer, location, size, type, kind, offset, stride, divisor});
}

void Mesh::attributePointerInternal(AttributeLayout&& attribute) {
    CORRADE_ASSERT(attribute.buffer.id(),
        "GL::Mesh::addVertexBuffer(): empty or moved-out Buffer instance was passed", );
    Context::current().state().mesh.attributePointerImplementation(*this, Utility::move(attribute));
}

void Mesh::attributePointerImplementationDefault(Mesh& self, AttributeLayout&& attribute) {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(attribute.buffer.targetHint() == Buffer::TargetHint::Array,
        "GL::Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer.targetHint(), );
    #endif

    arrayAppend(self._attributes, Utility::move(attribute));
}

void Mesh::attributePointerImplementationVAO(Mesh& self, AttributeLayout&& attribute) {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(attribute.buffer.targetHint() == Buffer::TargetHint::Array,
        "GL::Mesh::addVertexBuffer(): the buffer has unexpected target hint, expected" << Buffer::TargetHint::Array << "but got" << attribute.buffer.targetHint(), );
    #endif

    self.bindVAO();
    self.vertexAttribPointer(attribute);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::attributePointerImplementationVAODSA(Mesh& self, AttributeLayout&& attribute) {
    glEnableVertexArrayAttrib(self._id, attribute.location);

    const DynamicAttribute::Kind attributeKind = attribute.kind();
    const GLint attributeSize = attribute.size();
    const GLintptr attributeOffset = attribute.offset();
    const GLsizei attributeStride = attribute.stride();

    #ifndef MAGNUM_TARGET_GLES2
    if(attributeKind == DynamicAttribute::Kind::Integral)
        glVertexArrayAttribIFormat(self._id, attribute.location, attributeSize, attribute.type, 0);
    #ifndef MAGNUM_TARGET_GLES
    else if(attributeKind == DynamicAttribute::Kind::Long)
        glVertexArrayAttribLFormat(self._id, attribute.location, attributeSize, attribute.type, 0);
    #endif
    else
    #endif
    {
        glVertexArrayAttribFormat(self._id, attribute.location, attributeSize, attribute.type, attributeKind == DynamicAttribute::Kind::GenericNormalized, 0);
    }

    glVertexArrayAttribBinding(self._id, attribute.location, attribute.location);
    CORRADE_INTERNAL_ASSERT(attributeStride != 0);
    glVertexArrayVertexBuffer(self._id, attribute.location, attribute.buffer.id(), attributeOffset, attributeStride);

    if(attribute.divisor)
        Context::current().state().mesh.vertexAttribDivisorImplementation(self, attribute.location, attribute.divisor);
}

#ifdef CORRADE_TARGET_WINDOWS
void Mesh::attributePointerImplementationVAODSAIntelWindows(Mesh& self, AttributeLayout&& attribute) {
    /* See the "intel-windows-broken-dsa-integer-vertex-attributes" workaround
       for more information. */
    if(attribute.kind() == DynamicAttribute::Kind::Integral)
        return attributePointerImplementationVAO(self, Utility::move(attribute));
    else
        return attributePointerImplementationVAODSA(self, Utility::move(attribute));
}
#endif
#endif

#ifdef MAGNUM_TARGET_GLES
/* See the "angle-instanced-attributes-always-draw-instanced" workaround for
   these two */
void Mesh::attributePointerImplementationDefaultAngleAlwaysInstanced(Mesh& self, AttributeLayout&& attribute) {
    if(attribute.divisor) self._instanced = true;
    return attributePointerImplementationDefault(self, Utility::move(attribute));
}
void Mesh::attributePointerImplementationVAOAngleAlwaysInstanced(Mesh& self, AttributeLayout&& attribute) {
    if(attribute.divisor) self._instanced = true;
    return attributePointerImplementationVAO(self, Utility::move(attribute));
}
#endif

void Mesh::vertexAttribPointer(AttributeLayout& attribute) {
    glEnableVertexAttribArray(attribute.location);
    attribute.buffer.bindInternal(Buffer::TargetHint::Array);

    const DynamicAttribute::Kind attributeKind = attribute.kind();
    const GLint attributeSize = attribute.size();
    const GLintptr attributeOffset = attribute.offset();
    const GLsizei attributeStride = attribute.stride();

    #ifndef MAGNUM_TARGET_GLES2
    if(attributeKind == DynamicAttribute::Kind::Integral)
        glVertexAttribIPointer(attribute.location, attributeSize, attribute.type, attributeStride, reinterpret_cast<const GLvoid*>(attributeOffset));
    #ifndef MAGNUM_TARGET_GLES
    else if(attributeKind == DynamicAttribute::Kind::Long)
        glVertexAttribLPointer(attribute.location, attributeSize, attribute.type, attributeStride, reinterpret_cast<const GLvoid*>(attributeOffset));
    #endif
    else
    #endif
    {
        glVertexAttribPointer(attribute.location, attributeSize, attribute.type, attributeKind == DynamicAttribute::Kind::GenericNormalized, attributeStride, reinterpret_cast<const GLvoid*>(attributeOffset));
    }

    if(attribute.divisor) {
        #ifndef MAGNUM_TARGET_GLES2
        glVertexAttribDivisor(attribute.location, attribute.divisor);
        #else
        Context::current().state().mesh.vertexAttribDivisorImplementation(*this, attribute.location, attribute.divisor);
        #endif
    }
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::vertexAttribDivisorImplementationVAO(Mesh& self, const GLuint index, const GLuint divisor) {
    self.bindVAO();
    glVertexAttribDivisor(index, divisor);
}
void Mesh::vertexAttribDivisorImplementationVAODSA(Mesh& self, const GLuint index, const GLuint divisor) {
    glVertexArrayBindingDivisor(self._id, index, divisor);
}
#elif defined(MAGNUM_TARGET_GLES2)
void Mesh::vertexAttribDivisorImplementationANGLE(Mesh&, const GLuint index, const GLuint divisor) {
    glVertexAttribDivisorANGLE(index, divisor);
}
#ifndef MAGNUM_TARGET_WEBGL
void Mesh::vertexAttribDivisorImplementationEXT(Mesh&, const GLuint index, const GLuint divisor) {
    glVertexAttribDivisorEXT(index, divisor);
}
void Mesh::vertexAttribDivisorImplementationNV(Mesh&, const GLuint index, const GLuint divisor) {
    glVertexAttribDivisorNV(index, divisor);
}
#endif
#endif

void Mesh::acquireVertexBuffer(Buffer&& buffer) {
    Context::current().state().mesh.acquireVertexBufferImplementation(*this, Utility::move(buffer));
}

void Mesh::acquireVertexBufferImplementationDefault(Mesh& self, Buffer&& buffer) {
    /* The last added buffer should be this one, replace it with a owning one */
    CORRADE_INTERNAL_ASSERT(!self._attributes.isEmpty() && self._attributes.back().buffer.id() == buffer.id() && buffer.id());
    self._attributes.back().buffer.release(); /* so we swap back a zero ID */
    self._attributes.back().buffer = Utility::move(buffer);
}

void Mesh::acquireVertexBufferImplementationVAO(Mesh& self, Buffer&& buffer) {
    CORRADE_INTERNAL_ASSERT(buffer.id());
    /* With VAOs we are not maintaining the attribute list, so abuse the
       storage for just owning the buffer. That wastes a bit of space as only 8
       out of the 24 bytes is actually used, but that should be okay as there's
       likely only very few buffers (compared to attributes, which can be quite
       many). */
    arrayAppend(self._attributes, InPlaceInit, Utility::move(buffer));
}

void Mesh::bindIndexBufferImplementationDefault(Mesh&, Buffer&) {}

void Mesh::bindIndexBufferImplementationVAO(Mesh& self, Buffer& buffer) {
    self.bindVAO();

    /* Binding the VAO in the above function resets element buffer binding,
       meaning the following will always cause the glBindBuffer() to be called */
    buffer.bindInternal(Buffer::TargetHint::ElementArray);
}

#ifndef MAGNUM_TARGET_GLES
void Mesh::bindIndexBufferImplementationVAODSA(Mesh& self, Buffer& buffer) {
    glVertexArrayElementBuffer(self._id, buffer.id());
}
#endif

void Mesh::bindImplementationDefault(Mesh& self) {
    /* Specify vertex attributes */
    for(AttributeLayout& attribute: self._attributes)
        self.vertexAttribPointer(attribute);

    /* Bind index buffer, if the mesh is indexed */
    if(self._indexBuffer.id()) self._indexBuffer.bindInternal(Buffer::TargetHint::ElementArray);
}

void Mesh::bindImplementationVAO(Mesh& self) {
    self.bindVAO();
}

void Mesh::unbindImplementationDefault(Mesh& self) {
    for(const AttributeLayout& attribute: self._attributes) {
        glDisableVertexAttribArray(attribute.location);

        /* Reset also the divisor back so it doesn't affect  */
        if(attribute.divisor) {
            #ifndef MAGNUM_TARGET_GLES2
            glVertexAttribDivisor(attribute.location, 0);
            #else
            Context::current().state().mesh.vertexAttribDivisorImplementation(self, attribute.location, 0);
            #endif
        }
    }
}

void Mesh::unbindImplementationVAO(Mesh&) {}

#ifdef MAGNUM_TARGET_GLES
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
#if !defined(MAGNUM_TARGET_GLES2) && (!defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915)
void Mesh::drawElementsBaseVertexImplementationANGLE(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint baseVertex) {
    glDrawElementsInstancedBaseVertexBaseInstanceANGLE(mode, count, type, indices, 1, baseVertex, 0);
}
#endif

void Mesh::drawElementsBaseVertexImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLint) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for indexed mesh draw with base vertex specification", );
}
#endif

#ifndef MAGNUM_TARGET_GLES2
#if !defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
void Mesh::drawRangeElementsBaseVertexImplementationANGLE(const GLenum mode, GLuint, GLuint, GLsizei count, GLenum type, const void* indices, GLint baseVertex) {
    glDrawElementsInstancedBaseVertexBaseInstanceANGLE(mode, count, type, indices, 1, baseVertex, 0);
}
#endif

void Mesh::drawRangeElementsBaseVertexImplementationAssert(GLenum, GLuint, GLuint, GLsizei, GLenum, const void*, GLint) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for indexed mesh draw with base vertex specification", );
}

void Mesh::drawArraysInstancedBaseInstanceImplementationAssert(GLenum, GLint, GLsizei, GLsizei, GLuint) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for instanced mesh draw with base instance specification", );
}

#if !defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
void Mesh::drawElementsInstancedBaseInstanceImplementationANGLE(const GLenum mode, const GLsizei count, const GLenum type, const void* const indices, const GLsizei instanceCount, const GLuint baseInstance) {
    glDrawElementsInstancedBaseVertexBaseInstanceANGLE(mode, count, type, indices, instanceCount, 0, baseInstance);
}
#endif

void Mesh::drawElementsInstancedBaseInstanceImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLsizei, GLuint) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh draw with base instance specification", );
}

void Mesh::drawElementsInstancedBaseVertexBaseInstanceImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint, GLuint) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh draw with base vertex and base instance specification", );
}

#if !defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
void Mesh::drawElementsInstancedBaseVertexImplementationANGLE(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instanceCount, GLint baseVertex) {
    glDrawElementsInstancedBaseVertexBaseInstanceANGLE(mode, count, type, indices, instanceCount, baseVertex, 0);
}
#endif

void Mesh::drawElementsInstancedBaseVertexImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh draw with base vertex specification", );
}
#endif
#endif

#ifdef MAGNUM_TARGET_GLES
#if !defined(MAGNUM_TARGET_GLES2) && (!defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20005)
void Mesh::multiDrawElementsBaseVertexImplementationANGLE(const GLenum mode, const GLsizei* const count, const GLenum type, const void* const* const indices, const GLsizei drawCount, const GLint* const baseVertex) {
    /** @todo merge with the allocation in multiDrawImplementationDefault */
    Containers::ArrayView<GLsizei> instanceCount;
    Containers::ArrayView<GLuint> baseInstance;
    Containers::ArrayTuple data{
        {NoInit, std::size_t(drawCount), instanceCount},
        {ValueInit, std::size_t(drawCount), baseInstance},
    };
    for(GLsizei& i: instanceCount) i = 1;

    glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE(mode, count, type, indices, instanceCount, baseVertex, baseInstance, drawCount);
}
#endif

void Mesh::multiDrawElementsBaseVertexImplementationAssert(GLenum, const GLsizei*, GLenum, const void* const*, GLsizei, const GLint*) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for indexed mesh multi-draw with base vertex specification", );
}

#ifndef MAGNUM_TARGET_GLES2
void Mesh::multiDrawArraysInstancedBaseInstanceImplementationAssert(GLenum, const GLint*, const GLsizei*, const GLsizei*, const GLuint*, GLsizei) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for instanced mesh multi-draw with base instance specification", );
}

void Mesh::multiDrawElementsInstancedBaseVertexBaseInstanceImplementationAssert(GLenum, const GLint*, GLenum, const void* const*, const GLsizei*, const GLint*, const GLuint*, GLsizei) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh multi-draw with base vertex and base instance specification", );
}
#endif
#endif

}}
