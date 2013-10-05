#ifndef Magnum_Implementation_ShaderProgramState_h
#define Magnum_Implementation_ShaderProgramState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "OpenGL.h"

namespace Magnum { namespace Implementation {

struct ShaderProgramState {
    constexpr ShaderProgramState(): current(0), maxVertexAttributes(0)
        #ifndef MAGNUM_TARGET_GLES
        , maxAtomicCounterBufferSize(0), maxComputeSharedMemorySize(0), maxComputeWorkGroupInvocations(0), maxImageUnits(0), maxImageSamples(0), maxCombinedShaderOutputResources(0), maxUniformLocations(0), maxShaderStorageBlockSize(0)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        , minTexelOffset(0), maxTexelOffset(0), maxUniformBlockSize(0)
        #endif
        {}

    /* Currently used program */
    GLuint current;

    GLint maxVertexAttributes;
    #ifndef MAGNUM_TARGET_GLES
    GLint maxAtomicCounterBufferSize,
        maxComputeSharedMemorySize,
        maxComputeWorkGroupInvocations,
        maxImageUnits,
        maxImageSamples,
        maxCombinedShaderOutputResources,
        maxUniformLocations;
    GLint64 maxShaderStorageBlockSize;
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    GLint minTexelOffset, maxTexelOffset, maxUniformBlockSize;
    #endif
};

}}

#endif
