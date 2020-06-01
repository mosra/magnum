#ifndef Magnum_GL_Implementation_State_h
#define Magnum_GL_Implementation_State_h
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

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"

namespace Magnum { namespace GL { namespace Implementation {

struct BufferState;
struct ContextState;
#ifndef MAGNUM_TARGET_WEBGL
struct DebugState;
#endif
struct FramebufferState;
struct MeshState;
struct QueryState;
struct RendererState;
struct ShaderState;
struct ShaderProgramState;
struct TextureState;
#ifndef MAGNUM_TARGET_GLES2
struct TransformFeedbackState;
#endif

struct State {
    /* Initializes context-based functionality */
    explicit State(Context& context, std::ostream* out);

    ~State();

    enum: GLuint { DisengagedBinding = ~0u };

    Containers::Pointer<BufferState> buffer;
    Containers::Pointer<ContextState> context;
    #ifndef MAGNUM_TARGET_WEBGL
    Containers::Pointer<DebugState> debug;
    #endif
    Containers::Pointer<FramebufferState> framebuffer;
    Containers::Pointer<MeshState> mesh;
    Containers::Pointer<QueryState> query;
    Containers::Pointer<RendererState> renderer;
    Containers::Pointer<ShaderState> shader;
    Containers::Pointer<ShaderProgramState> shaderProgram;
    Containers::Pointer<TextureState> texture;
    #ifndef MAGNUM_TARGET_GLES2
    Containers::Pointer<TransformFeedbackState> transformFeedback;
    #endif
};

}}}

#endif
