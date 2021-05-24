/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "MeshView.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/MeshState.h"

namespace Magnum { namespace GL {

#ifdef MAGNUM_BUILD_DEPRECATED
void MeshView::draw(AbstractShaderProgram& shader, Containers::ArrayView<const Containers::Reference<MeshView>> meshes) {
    shader.draw(meshes);
}

void MeshView::draw(AbstractShaderProgram&& shader, Containers::ArrayView<const Containers::Reference<MeshView>> meshes) {
    shader.draw(meshes);
}

void MeshView::draw(AbstractShaderProgram& shader, std::initializer_list<Containers::Reference<MeshView>> meshes) {
    shader.draw(meshes);
}

void MeshView::draw(AbstractShaderProgram&& shader, std::initializer_list<Containers::Reference<MeshView>> meshes) {
    shader.draw(meshes);
}
#endif

MeshView& MeshView::setIndexRange(Int first) {
     CORRADE_ASSERT(_original.get()._indexBuffer.id(), "MeshView::setIndexRange(): mesh is not indexed", *this);
    _indexOffset = _original.get()._indexOffset + first*_original.get().indexTypeSize();
    return *this;
}

#ifdef MAGNUM_BUILD_DEPRECATED
MeshView& MeshView::draw(AbstractShaderProgram& shader) {
    shader.draw(*this);
    return *this;
}

MeshView& MeshView::draw(AbstractShaderProgram&& shader) {
    shader.draw(*this);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
MeshView& MeshView::draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream) {
    shader.drawTransformFeedback(*this, xfb, stream);
    return *this;
}

MeshView& MeshView::draw(AbstractShaderProgram&& shader, TransformFeedback& xfb, UnsignedInt stream) {
    shader.drawTransformFeedback(*this, xfb, stream);
    return *this;
}
#endif
#endif

void MeshView::multiDrawImplementationDefault(Containers::ArrayView<const Containers::Reference<MeshView>> meshes) {
    CORRADE_INTERNAL_ASSERT(meshes.size());

    const Implementation::MeshState& state = Context::current().state().mesh;

    Mesh& original = meshes.begin()->get()._original;
    Containers::Array<GLsizei> count{meshes.size()};
    Containers::Array<GLvoid*> indices{meshes.size()};
    Containers::Array<GLint> baseVertex{meshes.size()};

    /* Gather the parameters */
    bool hasBaseVertex = false;
    std::size_t i = 0;
    for(MeshView& mesh: meshes) {
        CORRADE_ASSERT(mesh._instanceCount == 1, "GL::AbstractShaderProgram::draw(): cannot draw multiple instanced meshes", );

        count[i] = mesh._count;
        indices[i] = reinterpret_cast<GLvoid*>(mesh._indexOffset);
        baseVertex[i] = mesh._baseVertex;

        if(mesh._baseVertex) hasBaseVertex = true;

        ++i;
    }

    (original.*state.bindImplementation)();

    /* Non-indexed meshes */
    if(!original._indexBuffer.id()) {
        #ifndef MAGNUM_TARGET_GLES
        glMultiDrawArrays
        #else
        state.multiDrawArraysImplementation
        #endif
            (GLenum(original._primitive), baseVertex, count, meshes.size());

    /* Indexed meshes */
    } else {
        /* Indexed meshes with base vertex */
        if(hasBaseVertex) {
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            #ifndef MAGNUM_TARGET_GLES
            glMultiDrawElementsBaseVertex
            #else
            state.multiDrawElementsBaseVertexImplementation
            #endif
                (GLenum(original._primitive), count, GLenum(original._indexType), indices, meshes.size(), baseVertex);
            #else
            CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): indexed mesh multi-draw with base vertex specification possible only since WebGL 2.0", );
            #endif

        /* Indexed meshes */
        } else {
            #ifndef MAGNUM_TARGET_GLES
            glMultiDrawElements
            #else
            state.multiDrawElementsImplementation
            #endif
                (GLenum(original._primitive), count, GLenum(original._indexType), indices, meshes.size());
        }
    }

    (original.*state.unbindImplementation)();
}

#ifdef MAGNUM_TARGET_GLES
void MeshView::multiDrawImplementationFallback(Containers::ArrayView<const Containers::Reference<MeshView>> meshes) {
    for(MeshView& mesh: meshes) {
        /* Nothing to draw in this mesh */
        if(!mesh._count) continue;

        CORRADE_ASSERT(mesh._instanceCount == 1, "GL::AbstractShaderProgram::draw(): cannot draw multiple instanced meshes", );

        #ifndef MAGNUM_TARGET_GLES2
        mesh._original.get().drawInternal(mesh._count, mesh._baseVertex, 1, mesh._baseInstance, mesh._indexOffset, mesh._indexStart, mesh._indexEnd);
        #else
        mesh._original.get().drawInternal(mesh._count, mesh._baseVertex, 1, mesh._indexOffset);
        #endif
    }
}
#endif

#ifdef MAGNUM_TARGET_GLES
#if defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20005
void MeshView::multiDrawElementsBaseVertexImplementationANGLE(const GLenum mode, const GLsizei* const count, const GLenum type, const void* const* const indices, const GLsizei drawCount, const GLint* const baseVertex) {
    /** @todo merge with the allocation in multiDrawImplementationDefault */
    Containers::ArrayView<GLsizei> instanceCount;
    Containers::ArrayView<GLuint> baseInstance;
    Containers::ArrayTuple data{
        {Containers::NoInit, std::size_t(drawCount), instanceCount},
        {Containers::ValueInit, std::size_t(drawCount), baseInstance},
    };
    for(GLsizei& i: instanceCount) i = 1;

    glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE(mode, count, type, indices, instanceCount, baseVertex, baseInstance, drawCount);
}
#endif

void MeshView::multiDrawElementsBaseVertexImplementationAssert(GLenum, const GLsizei*, GLenum, const void* const*, GLsizei, const GLint*) {
    CORRADE_ASSERT_UNREACHABLE("GL::AbstractShaderProgram::draw(): no extension available for indexed mesh multi-draw with base vertex specification", );
}
#endif

}}
