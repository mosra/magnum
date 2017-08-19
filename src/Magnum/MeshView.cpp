/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "MeshView.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Context.h"
#include "Magnum/Mesh.h"

#include "Implementation/State.h"
#include "Implementation/MeshState.h"

namespace Magnum {

void MeshView::draw(AbstractShaderProgram& shader, std::initializer_list<std::reference_wrapper<MeshView>> meshes) {
    /* Why std::initializer_list doesn't have empty()? */
    if(!meshes.size()) return;

    shader.use();

    #ifndef CORRADE_NO_ASSERT
    const Mesh* original = &meshes.begin()->get()._original.get();
    for(MeshView& mesh: meshes)
        CORRADE_ASSERT(&mesh._original.get() == original, "MeshView::draw(): all meshes must be views of the same original mesh", );
    #endif

    #ifndef MAGNUM_TARGET_GLES
    multiDrawImplementationDefault(meshes);
    #else
    Context::current().state().mesh->multiDrawImplementation(meshes);
    #endif
}

#ifndef MAGNUM_TARGET_WEBGL
void MeshView::multiDrawImplementationDefault(std::initializer_list<std::reference_wrapper<MeshView>> meshes) {
    CORRADE_INTERNAL_ASSERT(meshes.size());

    const Implementation::MeshState& state = *Context::current().state().mesh;

    Mesh& original = meshes.begin()->get()._original;
    Containers::Array<GLsizei> count{meshes.size()};
    Containers::Array<GLvoid*> indices{meshes.size()};
    Containers::Array<GLint> baseVertex{meshes.size()};

    /* Gather the parameters */
    #ifndef MAGNUM_TARGET_GLES
    bool hasBaseVertex = false;
    #endif
    std::size_t i = 0;
    for(MeshView& mesh: meshes) {
        CORRADE_ASSERT(mesh._instanceCount == 1, "MeshView::draw(): cannot draw multiple instanced meshes", );

        count[i] = mesh._count;
        indices[i] = reinterpret_cast<GLvoid*>(mesh._indexOffset);
        baseVertex[i] = mesh._baseVertex;

        if(mesh._baseVertex) {
            #ifndef MAGNUM_TARGET_GLES
            hasBaseVertex = true;
            #else
            CORRADE_ASSERT(!original._indexBuffer, "MeshView::draw(): desktop OpenGL is required for base vertex specification in indexed meshes", );
            #endif
        }

        ++i;
    }

    (original.*state.bindImplementation)();

    /* Non-indexed meshes */
    if(!original._indexBuffer) {
        #ifndef MAGNUM_TARGET_GLES
        glMultiDrawArrays(GLenum(original._primitive), baseVertex, count, meshes.size());
        #else
        glMultiDrawArraysEXT(GLenum(original._primitive), baseVertex, count, meshes.size());
        #endif

    /* Indexed meshes */
    } else {
        /* Indexed meshes with base vertex */
        #ifndef MAGNUM_TARGET_GLES
        if(hasBaseVertex) {
            glMultiDrawElementsBaseVertex(GLenum(original._primitive), count, GLenum(original._indexType), indices, meshes.size(), baseVertex);

        /* Indexed meshes */
        } else
        #endif
        {
            #ifndef MAGNUM_TARGET_GLES
            glMultiDrawElements(GLenum(original._primitive), count, GLenum(original._indexType), indices, meshes.size());
            #else
            glMultiDrawElementsEXT(GLenum(original._primitive), count, GLenum(original._indexType), indices, meshes.size());
            #endif
        }
    }

    (original.*state.unbindImplementation)();
}
#endif

#ifdef MAGNUM_TARGET_GLES
void MeshView::multiDrawImplementationFallback(std::initializer_list<std::reference_wrapper<MeshView>> meshes) {
    for(MeshView& mesh: meshes) {
        /* Nothing to draw in this mesh */
        if(!mesh._count) continue;

        CORRADE_ASSERT(mesh._instanceCount == 1, "MeshView::draw(): cannot draw multiple instanced meshes", );

        #ifndef MAGNUM_TARGET_GLES2
        mesh._original.get().drawInternal(mesh._count, mesh._baseVertex, 1, mesh._indexOffset, mesh._indexStart, mesh._indexEnd);
        #else
        mesh._original.get().drawInternal(mesh._count, mesh._baseVertex, 1, mesh._indexOffset);
        #endif
    }
}
#endif

MeshView& MeshView::setIndexRange(Int first) {
    _indexOffset = _original.get()._indexOffset + first*_original.get().indexSize();
    return *this;
}

void MeshView::draw(AbstractShaderProgram& shader) {
    /* Nothing to draw, exit without touching any state */
    if(!_count || !_instanceCount) return;

    shader.use();

    #ifndef MAGNUM_TARGET_GLES
    _original.get().drawInternal(_count, _baseVertex, _instanceCount, _baseInstance, _indexOffset, _indexStart, _indexEnd);
    #elif !defined(MAGNUM_TARGET_GLES2)
    _original.get().drawInternal(_count, _baseVertex, _instanceCount, _indexOffset, _indexStart, _indexEnd);
    #else
    _original.get().drawInternal(_count, _baseVertex, _instanceCount, _indexOffset);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshView::draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream) {
    /* Nothing to draw, exit without touching any state */
    if(!_instanceCount) return;

    shader.use();

    _original.get().drawInternal(xfb, stream, _instanceCount);
}
#endif

}
