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

#include "AbstractQuery.h"

#include <Corrade/Utility/Assert.h>

#include "Magnum/Context.h"
#include "Magnum/Implementation/DebugState.h"
#include "Magnum/Implementation/State.h"

namespace Magnum {

AbstractQuery::AbstractQuery(): _target{} {
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &_id);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glGenQueriesEXT(1, &_id);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

AbstractQuery::~AbstractQuery() {
    /* Moved out, nothing to do */
    if(!_id) return;

    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &_id);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glDeleteQueriesEXT(1, &_id);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

std::string AbstractQuery::label() const {
    #ifndef MAGNUM_TARGET_GLES
    return Context::current()->state().debug->getLabelImplementation(GL_QUERY, _id);
    #else
    return Context::current()->state().debug->getLabelImplementation(GL_QUERY_KHR, _id);
    #endif
}

AbstractQuery& AbstractQuery::setLabelInternal(const Containers::ArrayReference<const char> label) {
    #ifndef MAGNUM_TARGET_GLES
    Context::current()->state().debug->labelImplementation(GL_QUERY, _id, label);
    #else
    Context::current()->state().debug->labelImplementation(GL_QUERY_KHR, _id, label);
    #endif
    return *this;
}

bool AbstractQuery::resultAvailable() {
    CORRADE_ASSERT(!_target, "AbstractQuery::resultAvailable(): the query is currently running", false);

    GLuint result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_AVAILABLE_EXT, &result);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    return result == GL_TRUE;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> UnsignedInt AbstractQuery::result<UnsignedInt>() {
    CORRADE_ASSERT(!_target, "AbstractQuery::result(): the query is currently running", {});

    UnsignedInt result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    return result;
}

template<> bool AbstractQuery::result<bool>() { return result<UnsignedInt>() != 0; }

template<> Int AbstractQuery::result<Int>() {
    CORRADE_ASSERT(!_target, "AbstractQuery::result(): the query is currently running", {});

    Int result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectiv(_id, GL_QUERY_RESULT, &result);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glGetQueryObjectivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    return result;
}

#ifndef MAGNUM_TARGET_WEBGL
template<> UnsignedLong AbstractQuery::result<UnsignedLong>() {
    CORRADE_ASSERT(!_target, "AbstractQuery::result(): the query is currently running", {});

    UnsignedLong result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectui64v(_id, GL_QUERY_RESULT, &result);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glGetQueryObjectui64vEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    return result;
}

template<> Long AbstractQuery::result<Long>() {
    CORRADE_ASSERT(!_target, "AbstractQuery::result(): the query is currently running", {});

    Long result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjecti64v(_id, GL_QUERY_RESULT, &result);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glGetQueryObjecti64vEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    return result;
}
#endif
#endif

void AbstractQuery::begin(const GLenum target) {
    CORRADE_ASSERT(!_target, "AbstractQuery::begin(): the query is already running", );

    #ifndef MAGNUM_TARGET_GLES2
    glBeginQuery(_target = target, id());
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glBeginQueryEXT(_target = target, id());
    #else
    static_cast<void>(target);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractQuery::end() {
    CORRADE_ASSERT(_target, "AbstractQuery::end(): the query is not running", );

    #ifndef MAGNUM_TARGET_GLES2
    glEndQuery(_target);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glEndQueryEXT(_target);
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
    _target = {};
}

}
