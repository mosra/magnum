/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/ArrayTuple.h>

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

std::pair<Containers::ArrayTuple, State&> State::allocate(Context& context, std::ostream* const out) {
    /* I have to say, the ArrayTuple is quite a crazy thing */
    Containers::ArrayView<State> stateView;
    Containers::ArrayView<BufferState> bufferStateView;
    Containers::ArrayView<ContextState> contextStateView;
    #ifndef MAGNUM_TARGET_WEBGL
    Containers::ArrayView<DebugState> debugStateView;
    #endif
    Containers::ArrayView<FramebufferState> framebufferStateView;
    Containers::ArrayView<MeshState> meshStateView;
    Containers::ArrayView<QueryState> queryStateView;
    Containers::ArrayView<RendererState> rendererStateView;
    Containers::ArrayView<ShaderState> shaderStateView;
    Containers::ArrayView<ShaderProgramState> shaderProgramStateView;
    Containers::ArrayView<TextureState> textureStateView;
    #ifndef MAGNUM_TARGET_GLES2
    Containers::ArrayView<TransformFeedbackState> transformFeedbackStateView;
    #endif
    Containers::ArrayTuple data{
        {Containers::NoInit, 1, stateView},
        {Containers::NoInit, 1, bufferStateView},
        {Containers::NoInit, 1, contextStateView},
        #ifndef MAGNUM_TARGET_WEBGL
        {Containers::NoInit, 1, debugStateView},
        #endif
        {Containers::NoInit, 1, framebufferStateView},
        {Containers::NoInit, 1, meshStateView},
        {Containers::NoInit, 1, queryStateView},
        {Containers::NoInit, 1, rendererStateView},
        {Containers::NoInit, 1, shaderStateView},
        {Containers::NoInit, 1, shaderProgramStateView},
        {Containers::NoInit, 1, textureStateView},
        #ifndef MAGNUM_TARGET_GLES2
        {Containers::NoInit, 1, transformFeedbackStateView}
        #endif
    };

    /* Extensions that might get used by current context. The State classes
       will set strings based on Extension::index() and then we'll go through
       the list and print ones that aren't null. It's 1.5 kB of temporary data
       but I think in terms of code size and overhead it's better than
       populating a heap array and then std::sort() it to remove duplicates. */
    const char* extensions[Implementation::ExtensionCount]{};

    State& state = *(new(&stateView.front()) State{
        bufferStateView.front(),
        contextStateView.front(),
        #ifndef MAGNUM_TARGET_WEBGL
        debugStateView.front(),
        #endif
        framebufferStateView.front(),
        meshStateView.front(),
        queryStateView.front(),
        rendererStateView.front(),
        shaderStateView.front(),
        shaderProgramStateView.front(),
        textureStateView.front(),
        #ifndef MAGNUM_TARGET_GLES2
        transformFeedbackStateView.front()
        #endif
    });

    new(&state.buffer) BufferState{context, extensions};
    new(&state.context) ContextState{context, extensions};
    #ifndef MAGNUM_TARGET_WEBGL
    new(&state.debug) DebugState{context, extensions};
    #endif
    new(&state.framebuffer) FramebufferState{context, extensions};
    new(&state.mesh) MeshState{context, stateView.front().context, extensions};
    new(&state.query) QueryState{context, extensions};
    new(&state.renderer) RendererState{context, stateView.front().context, extensions};
    new(&state.shader) ShaderState(context, extensions);
    new(&state.shaderProgram) ShaderProgramState{context, extensions};
    new(&state.texture) TextureState{context, extensions};
    #ifndef MAGNUM_TARGET_GLES2
    new(&state.transformFeedback) TransformFeedbackState{context, extensions};
    #endif

    Debug{out} << "Using optional features:";
    for(const char* extension: extensions)
        if(extension) Debug(out) << "   " << extension;

    return {std::move(data), state};
}

}}}
