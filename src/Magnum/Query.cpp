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

#include "Query.h"

#include <Corrade/Utility/Assert.h>

#include "Magnum/Context.h"
#include "Magnum/Implementation/DebugState.h"
#include "Magnum/Implementation/State.h"

namespace Magnum {

AbstractQuery::AbstractQuery(): target() {
    /** @todo Re-enable when extension loader is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &_id);
    #elif defined(CORRADE_TARGET_NACL)
    glGenQueriesEXT(1, &_id);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    #endif
}

AbstractQuery::~AbstractQuery() {
    /* Moved out, nothing to do */
    if(!_id) return;

    /** @todo Re-enable when extension loader is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &_id);
    #elif defined(CORRADE_TARGET_NACL)
    glDeleteQueriesEXT(1, &_id);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    #endif
}

std::string AbstractQuery::label() const {
    #ifndef MAGNUM_TARGET_GLES
    return Context::current()->state().debug->getLabelImplementation(GL_QUERY, _id);
    #else
    return Context::current()->state().debug->getLabelImplementation(GL_QUERY_KHR, _id);
    #endif
}

AbstractQuery& AbstractQuery::setLabel(const std::string& label) {
    #ifndef MAGNUM_TARGET_GLES
    Context::current()->state().debug->labelImplementation(GL_QUERY, _id, label);
    #else
    Context::current()->state().debug->labelImplementation(GL_QUERY_KHR, _id, label);
    #endif
    return *this;
}

bool AbstractQuery::resultAvailable() {
    CORRADE_ASSERT(!target, "AbstractQuery::resultAvailable(): the query is currently running", false);

    /** @todo Re-enable when extension loader is available for ES */
    GLuint result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    #elif defined(CORRADE_TARGET_NACL)
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_AVAILABLE_EXT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    #endif
    return result == GL_TRUE;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> UnsignedInt AbstractQuery::result<UnsignedInt>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension loader is available for ES */
    UnsignedInt result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    #elif defined(CORRADE_TARGET_NACL)
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    #endif
    return result;
}

template<> bool AbstractQuery::result<bool>() { return result<UnsignedInt>() != 0; }

template<> Int AbstractQuery::result<Int>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension loader is available for ES */
    Int result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectiv(_id, GL_QUERY_RESULT, &result);
    #elif defined(CORRADE_TARGET_NACL)
    glGetQueryObjectivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    #endif
    return result;
}

template<> UnsignedLong AbstractQuery::result<UnsignedLong>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension loader is available for ES */
    UnsignedLong result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectui64v(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjectui64vEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result;
}

template<> Long AbstractQuery::result<Long>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension loader is available for ES */
    Long result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjecti64v(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjecti64vEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result;
}
#endif

void AbstractQuery::begin(GLenum target) {
    CORRADE_ASSERT(!this->target, "AbstractQuery::begin(): the query is already running", );

    /** @todo Re-enable when extension loader is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glBeginQuery(this->target = target, id());
    #elif defined(CORRADE_TARGET_NACL)
    glBeginQueryEXT(this->target = target, id());
    #else
    static_cast<void>(target);
    CORRADE_INTERNAL_ASSERT(false);
    #endif
}

void AbstractQuery::end() {
    CORRADE_ASSERT(target, "AbstractQuery::end(): the query is not running", );

    /** @todo Re-enable when extension loader is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glEndQuery(target);
    #elif defined(CORRADE_TARGET_NACL)
    glEndQueryEXT(target);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    #endif
    target = {};
}

}
