#ifndef Magnum_GL_Implementation_DebugState_h
#define Magnum_GL_Implementation_DebugState_h
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

#include <string>
#include <vector>

#include "Magnum/GL/DebugOutput.h"
#include "Magnum/GL/GL.h"

#ifdef MAGNUM_TARGET_WEBGL
#error this header is not available in WebGL build
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct DebugState {
    explicit DebugState(Context& context, std::vector<std::string>& extensions);

    std::string(*getLabelImplementation)(GLenum, GLuint);
    void(*labelImplementation)(GLenum, GLuint, Containers::ArrayView<const char>);

    void(*messageInsertImplementation)(DebugMessage::Source, DebugMessage::Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char>);
    void(*controlImplementation)(GLenum, GLenum, GLenum, std::initializer_list<UnsignedInt>, bool);
    void(*callbackImplementation)(DebugOutput::Callback, const void*);
    void(*pushGroupImplementation)(DebugGroup::Source, UnsignedInt, Containers::ArrayView<const char>);
    void(*popGroupImplementation)();

    GLint maxLabelLength, maxLoggedMessages, maxMessageLength, maxStackDepth;
    struct MessageCallback {
        DebugOutput::Callback callback{};
        const void* userParam{};
    } messageCallback;
};

}}}

#endif
