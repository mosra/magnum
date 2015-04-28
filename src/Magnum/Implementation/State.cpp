/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "State.h"

#include <algorithm>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "BufferState.h"
#include "DebugState.h"
#include "FramebufferState.h"
#include "MeshState.h"
#include "QueryState.h"
#include "RendererState.h"
#include "ShaderState.h"
#include "ShaderProgramState.h"
#include "TextureState.h"
#ifndef MAGNUM_TARGET_GLES2
#include "TransformFeedbackState.h"
#endif

namespace Magnum { namespace Implementation {

State::State(Context& context) {
    /* List of extensions used in current context. Guesstimate count to avoid
       unnecessary reallocations. */
    std::vector<std::string> extensions;
    #ifndef MAGNUM_TARGET_GLES
    extensions.reserve(32);
    #else
    extensions.reserve(8);
    #endif

    buffer.reset(new BufferState{context, extensions});
    debug.reset(new DebugState{context, extensions});
    framebuffer.reset(new FramebufferState{context, extensions});
    mesh.reset(new MeshState{context, extensions});
    query.reset(new QueryState{context, extensions});
    renderer.reset(new RendererState{context, extensions});
    shader.reset(new ShaderState);
    shaderProgram.reset(new ShaderProgramState{context, extensions});
    texture.reset(new TextureState{context, extensions});
    #ifndef MAGNUM_TARGET_GLES2
    transformFeedback.reset(new TransformFeedbackState{context, extensions});
    #endif

    /* Sort the features and remove duplicates */
    std::sort(extensions.begin(), extensions.end());
    extensions.erase(std::unique(extensions.begin(), extensions.end()), extensions.end());

    Debug() << "Using optional features:";
    for(auto it = extensions.begin(); it != extensions.end(); ++it)
        Debug() << "   " << *it;
}

State::~State() = default;

}}
