#ifndef Magnum_GL_Implementation_MeshState_h
#define Magnum_GL_Implementation_MeshState_h
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

#include <vector>
#include <string>

#include "Magnum/GL/Mesh.h"

namespace Magnum { namespace GL { namespace Implementation {

struct ContextState;

struct MeshState {
    explicit MeshState(Context& context, ContextState& contextState, std::vector<std::string>& extensions);
    ~MeshState();

    void reset();

    void(Mesh::*createImplementation)(bool);
    void(Mesh::*moveConstructImplementation)(Mesh&&);
    void(Mesh::*moveAssignImplementation)(Mesh&&);
    void(Mesh::*destroyImplementation)(bool);
    void(Mesh::*attributePointerImplementation)(Mesh::AttributeLayout&&);
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    void(Mesh::*vertexAttribDivisorImplementation)(GLuint, GLuint);
    #endif
    void(Mesh::*acquireVertexBufferImplementation)(Buffer&&);
    void(Mesh::*bindIndexBufferImplementation)(Buffer&);
    void(Mesh::*bindImplementation)();
    void(Mesh::*unbindImplementation)();

    #ifdef MAGNUM_TARGET_GLES2
    void(Mesh::*drawArraysInstancedImplementation)(GLint, GLsizei, GLsizei);
    void(Mesh::*drawElementsInstancedImplementation)(GLsizei, GLintptr, GLsizei);
    #endif

    #ifdef MAGNUM_TARGET_GLES
    void(*multiDrawImplementation)(Containers::ArrayView<const Containers::Reference<MeshView>>);
    #endif

    void(*bindVAOImplementation)(GLuint);

    #ifndef MAGNUM_TARGET_GLES
    GLuint defaultVAO{}, /* Used on core profile in case ARB_VAO is disabled */
        /* Used for non-VAO-aware external GL code on core profile in case
           ARB_VAO is *not* disabled */
        scratchVAO{};
    #endif

    GLuint currentVAO;
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    GLint maxVertexAttributeStride{};
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    GLint64 maxElementIndex;
    #else
    GLint maxElementIndex;
    #endif
    GLint maxElementsIndices, maxElementsVertices;
    #endif
};

}}}

#endif
