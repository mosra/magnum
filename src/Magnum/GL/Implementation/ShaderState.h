#ifndef Magnum_GL_Implementation_ShaderState_h
#define Magnum_GL_Implementation_ShaderState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"

namespace Magnum { namespace GL { namespace Implementation {

struct ShaderState {
    explicit ShaderState(Context& context, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions);

    enum: std::size_t {
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        StageCount = 6
        #else
        StageCount = 2
        #endif
    };

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_APPLE)
    Containers::StringView(*workaroundDefinesImplementation)(Version);
    #endif
    void(*addSourceImplementation)(Shader&, Containers::String&&);
    void(*cleanLogImplementation)(Containers::String&);
    /* This is a direct pointer to a GL function, so needs a __stdcall on
       Windows to compile properly on 32 bits */
    void(APIENTRY *completionStatusImplementation)(GLuint, GLenum, GLint* value);

    GLint maxVertexOutputComponents,
        maxFragmentInputComponents;
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    GLint maxTessellationControlInputComponents,
        maxTessellationControlOutputComponents,
        maxTessellationControlTotalOutputComponents,
        maxTessellationEvaluationInputComponents,
        maxTessellationEvaluationOutputComponents,
        maxGeometryInputComponents,
        maxGeometryOutputComponents,
        maxGeometryTotalOutputComponents;
    GLint maxAtomicCounterBuffers[StageCount];
    GLint maxCombinedAtomicCounterBuffers;
    GLint maxAtomicCounters[StageCount];
    GLint maxCombinedAtomicCounters;
    GLint maxImageUniforms[StageCount];
    GLint maxCombinedImageUniforms;
    GLint maxShaderStorageBlocks[StageCount];
    GLint maxCombinedShaderStorageBlocks;
    #endif
    GLint maxTextureImageUnits[StageCount];
    GLint maxTextureImageUnitsCombined;
    #ifndef MAGNUM_TARGET_GLES2
    GLint maxUniformBlocks[StageCount];
    GLint maxCombinedUniformBlocks;
    #endif
    GLint maxUniformComponents[StageCount];
    GLint maxUniformComponentsCombined;
    #ifndef MAGNUM_TARGET_GLES2
    GLint maxCombinedUniformComponents[StageCount];
    #endif
};

}}}

#endif
