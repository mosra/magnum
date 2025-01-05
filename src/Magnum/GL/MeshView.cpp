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

#include "MeshView.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Iterable.h>
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
    shader.draw(Containers::arrayView(meshes));
}

void MeshView::draw(AbstractShaderProgram&& shader, std::initializer_list<Containers::Reference<MeshView>> meshes) {
    shader.draw(Containers::arrayView(meshes));
}
#endif

MeshView& MeshView::setIndexOffset(Int offset) {
    CORRADE_ASSERT(_original.get()._indexBuffer.id(),
        "GL::MeshView::setIndexOffset(): mesh is not indexed", *this);
    _indexOffset = offset;
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

void MeshView::multiDrawImplementationDefault(const Containers::Iterable<MeshView>& meshes) {
    CORRADE_INTERNAL_ASSERT(!meshes.isEmpty());

    Mesh& original = meshes.front()._original;

    /* Gather the parameters */
    Containers::ArrayView<UnsignedInt> counts;
    Containers::ArrayView<UnsignedInt> vertexOffsets;
    Containers::ArrayView<
        #ifndef CORRADE_TARGET_32BIT
        UnsignedLong
        #else
        UnsignedInt
        #endif
    > indexOffsets;
    Containers::ArrayTuple data{
        {NoInit, meshes.size(), counts},
        {NoInit, meshes.size(), vertexOffsets},
        {NoInit, meshes.size(), indexOffsets}
    };

    /* The vertexOffsets array is used for non-indexed meshes or if the base
       vertex is specified for any of the meshes */
    const UnsignedInt indexTypeSize = original._indexBuffer.id() ?
        meshIndexTypeSize(original._indexType) : 0;
    bool useVertexOffsets = !original.isIndexed();
    for(std::size_t i = 0; i != meshes.size(); ++i) {
        CORRADE_ASSERT(meshes[i]._instanceCount == 1, "GL::AbstractShaderProgram::draw(): cannot multi-draw instanced meshes", );

        counts[i] = meshes[i]._count;
        vertexOffsets[i] = meshes[i]._baseVertex;
        indexOffsets[i] = original._indexBufferOffset + indexTypeSize*meshes[i]._indexOffset;
        if(meshes[i]._baseVertex) useVertexOffsets = true;
    }

    original.drawInternal(counts, useVertexOffsets ? vertexOffsets : nullptr, indexOffsets);
}

#ifdef MAGNUM_TARGET_GLES
void MeshView::multiDrawImplementationFallback(const Containers::Iterable<MeshView>& meshes) {
    for(MeshView& mesh: meshes) {
        /* Nothing to draw in this mesh */
        if(!mesh._count) continue;

        CORRADE_ASSERT(mesh._instanceCount == 1, "GL::AbstractShaderProgram::draw(): cannot multi-draw instanced meshes", );

        #ifndef MAGNUM_TARGET_GLES2
        mesh._original.get().drawInternal(mesh._count, mesh._baseVertex, 1, mesh._baseInstance, mesh._indexOffset, mesh._indexStart, mesh._indexEnd);
        #else
        mesh._original.get().drawInternal(mesh._count, mesh._baseVertex, 1, mesh._indexOffset);
        #endif
    }
}
#endif

}}
