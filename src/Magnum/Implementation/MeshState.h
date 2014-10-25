#ifndef Magnum_Implementation_MeshState_h
#define Magnum_Implementation_MeshState_h
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

#include <vector>
#include <string>

#include "Magnum/Mesh.h"

namespace Magnum { namespace Implementation {

struct MeshState {
    explicit MeshState(Context& context, std::vector<std::string>& extensions);

    void reset();

    void(Mesh::*createImplementation)();
    void(Mesh::*destroyImplementation)();
    void(Mesh::*attributePointerImplementation)(const Mesh::GenericAttribute&);
    #ifndef MAGNUM_TARGET_GLES2
    void(Mesh::*attributeIPointerImplementation)(const Mesh::IntegerAttribute&);
    #ifndef MAGNUM_TARGET_GLES
    void(Mesh::*attributeLPointerImplementation)(const Mesh::LongAttribute&);
    #endif
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    void(Mesh::*vertexAttribDivisorImplementation)(GLuint, GLuint);
    #endif
    void(Mesh::*bindIndexBufferImplementation)(Buffer&);
    void(Mesh::*bindImplementation)();
    void(Mesh::*unbindImplementation)();

    #ifdef MAGNUM_TARGET_GLES2
    void(Mesh::*drawArraysInstancedImplementation)(GLint, GLsizei, GLsizei);
    void(Mesh::*drawElementsInstancedImplementation)(GLsizei, GLintptr, GLsizei);
    #endif

    #ifdef MAGNUM_TARGET_GLES
    void(*multiDrawImplementation)(std::initializer_list<std::reference_wrapper<MeshView>>);
    #endif

    GLuint currentVAO;
    #ifndef MAGNUM_TARGET_GLES2
    GLint64 maxElementIndex;
    GLint maxElementsIndices, maxElementsVertices;
    #endif
};

}}

#endif
