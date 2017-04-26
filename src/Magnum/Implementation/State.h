#ifndef Magnum_Implementation_State_h
#define Magnum_Implementation_State_h
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

#include <memory>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"

namespace Magnum { namespace Implementation {

struct BufferState;
struct ContextState;
#ifndef MAGNUM_TARGET_WEBGL
struct DebugState;
#endif
struct FramebufferState;
struct MeshState;
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
struct QueryState;
#endif
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

    std::unique_ptr<BufferState> buffer;
    std::unique_ptr<ContextState> context;
    #ifndef MAGNUM_TARGET_WEBGL
    std::unique_ptr<DebugState> debug;
    #endif
    std::unique_ptr<FramebufferState> framebuffer;
    std::unique_ptr<MeshState> mesh;
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    std::unique_ptr<QueryState> query;
    #endif
    std::unique_ptr<RendererState> renderer;
    std::unique_ptr<ShaderState> shader;
    std::unique_ptr<ShaderProgramState> shaderProgram;
    std::unique_ptr<TextureState> texture;
    #ifndef MAGNUM_TARGET_GLES2
    std::unique_ptr<TransformFeedbackState> transformFeedback;
    #endif
};

}}

#endif
