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

#include "State.h"

#include <algorithm>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/BufferState.h"
#include "Magnum/GL/Implementation/ContextState.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/FramebufferState.h"
#include "Magnum/GL/Implementation/MeshState.h"
#include "Magnum/GL/Implementation/QueryState.h"
#include "Magnum/GL/Implementation/RendererState.h"
#include "Magnum/GL/Implementation/ShaderState.h"
#include "Magnum/GL/Implementation/ShaderProgramState.h"
#include "Magnum/GL/Implementation/TextureState.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Implementation/TransformFeedbackState.h"
#endif

namespace Magnum { namespace GL { namespace Implementation {

State::State(Context& context, std::ostream* const out) {
    /* List of extensions used in current context. Guesstimate count to avoid
       unnecessary reallocations. */
    std::vector<std::string> extensions;
    #ifndef MAGNUM_TARGET_GLES
    extensions.reserve(32);
    #else
    extensions.reserve(8);
    #endif

    buffer.reset(new BufferState{context, extensions});
    this->context.reset(new ContextState{context, extensions});
    #ifndef MAGNUM_TARGET_WEBGL
    debug.reset(new DebugState{context, extensions});
    #endif
    framebuffer.reset(new FramebufferState{context, extensions});
    mesh.reset(new MeshState{context, *this->context, extensions});
    query.reset(new QueryState{context, extensions});
    renderer.reset(new RendererState{context, *this->context, extensions});
    shader.reset(new ShaderState(context, extensions));
    shaderProgram.reset(new ShaderProgramState{context, extensions});
    texture.reset(new TextureState{context, extensions});
    #ifndef MAGNUM_TARGET_GLES2
    transformFeedback.reset(new TransformFeedbackState{context, extensions});
    #endif

    /* Sort the features and remove duplicates */
    std::sort(extensions.begin(), extensions.end());
    extensions.erase(std::unique(extensions.begin(), extensions.end()), extensions.end());

    Debug{out} << "Using optional features:";
    for(const auto& ext: extensions) Debug(out) << "   " << ext;
}

State::~State() = default;

}}}
