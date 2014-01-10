#ifndef Magnum_Implementation_State_h
#define Magnum_Implementation_State_h
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

#include "Magnum/Magnum.h"

namespace Magnum { namespace Implementation {

struct BufferState;
struct DebugState;
struct FramebufferState;
struct MeshState;
struct RendererState;
struct ShaderState;
struct ShaderProgramState;
struct TextureState;

struct State {
    /* Initializes context-based functionality */
    State(Context& context);

    ~State();

    BufferState* const buffer;
    DebugState* const debug;
    FramebufferState* const framebuffer;
    MeshState* const mesh;
    RendererState* const renderer;
    ShaderState* const shader;
    ShaderProgramState* const shaderProgram;
    TextureState* const texture;
};

}}

#endif
