#ifndef Magnum_GL_Implementation_BufferState_h
#define Magnum_GL_Implementation_BufferState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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

#include "Magnum/GL/Buffer.h"

/* The __EMSCRIPTEN_major__ etc macros used to be passed implicitly, version
   3.1.4 moved them to a version header and version 3.1.23 dropped the
   backwards compatibility. To work consistently on all versions, including the
   header only if the version macros aren't present.
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#if defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(__EMSCRIPTEN_major__)
#include <emscripten/version.h>
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct BufferState {
    enum: std::size_t {
        #ifndef MAGNUM_TARGET_WEBGL
        TargetCount = 13+1
        #elif !defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL)
        TargetCount = 8+1
        #else
        TargetCount = 2+1
        #endif
    };

    /* Target <-> index mapping */
    static std::size_t indexForTarget(Buffer::TargetHint target);
    static const Buffer::TargetHint targetForIndex[TargetCount-1];

    explicit BufferState(Context& context, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions);

    void reset();

    #ifndef MAGNUM_TARGET_GLES2
    void(*bindBasesImplementation)(Buffer::Target, UnsignedInt, Containers::ArrayView<Buffer* const>);
    void(*bindRangesImplementation)(Buffer::Target, UnsignedInt, Containers::ArrayView<const Containers::Triple<Buffer*, GLintptr, GLsizeiptr>>);
    void(*copyImplementation)(Buffer&, Buffer&, GLintptr, GLintptr, GLsizeiptr);
    #endif
    void(*createImplementation)(Buffer&);
    void(*setTargetHintImplementation)(Buffer&, Buffer::TargetHint);
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*storageImplementation)(Buffer&, Containers::ArrayView<const void>, Buffer::StorageFlags);
    #endif
    void(*getParameterImplementation)(Buffer&, GLenum, GLint*);
    #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20017)
    void(*getSubDataImplementation)(Buffer&, GLintptr, GLsizeiptr, GLvoid*);
    #endif
    void(*dataImplementation)(Buffer&, GLsizeiptr, const GLvoid*, BufferUsage);
    void(*subDataImplementation)(Buffer&, GLintptr, GLsizeiptr, const GLvoid*);
    void(*invalidateImplementation)(Buffer&);
    void(*invalidateSubImplementation)(Buffer&, GLintptr, GLsizeiptr);
    #ifndef MAGNUM_TARGET_WEBGL
    void*(*mapImplementation)(Buffer&, Buffer::MapAccess);
    void*(*mapRangeImplementation)(Buffer&, GLintptr, GLsizeiptr, Buffer::MapFlags);
    void(*flushMappedRangeImplementation)(Buffer&, GLintptr, GLsizeiptr);
    bool(*unmapImplementation)(Buffer&);
    #endif

    /* Currently bound buffer for all targets */
    GLuint bindings[TargetCount];

    /* Limits */
    #ifndef MAGNUM_TARGET_GLES2
    GLint
        #ifndef MAGNUM_TARGET_GLES
        minMapAlignment,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        maxAtomicCounterBindings,
        maxShaderStorageBindings,
        shaderStorageOffsetAlignment,
        #endif
        uniformOffsetAlignment,
        maxUniformBindings;
    #endif
};

}}}

#endif
