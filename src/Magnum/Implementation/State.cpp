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

#include "State.h"

#include <algorithm>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "BufferState.h"
#include "DebugState.h"
#include "FramebufferState.h"
#include "MeshState.h"
#include "RendererState.h"
#include "ShaderState.h"
#include "ShaderProgramState.h"
#include "TextureState.h"

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

    buffer = new BufferState(context, extensions);
    debug = new DebugState(context, extensions);
    framebuffer = new FramebufferState(context, extensions);
    mesh = new MeshState(context, extensions);
    renderer = new RendererState(context, extensions);
    shader = new ShaderState;
    shaderProgram = new ShaderProgramState(context, extensions);
    texture = new TextureState(context, extensions);

    /* Sort the features and remove duplicates */
    std::sort(extensions.begin(), extensions.end());
    extensions.erase(std::unique(extensions.begin(), extensions.end()), extensions.end());

    Debug() << "Using optional features:";
    for(auto it = extensions.begin(); it != extensions.end(); ++it)
        Debug() << "   " << *it;
}

State::~State() {
    delete texture;
    delete shaderProgram;
    delete shader;
    delete renderer;
    delete mesh;
    delete framebuffer;
    delete debug;
    delete buffer;
}

}}
