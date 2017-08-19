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

#include "AbstractQuery.h"

#include <Corrade/Utility/Assert.h>

#include "Magnum/Context.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/Implementation/DebugState.h"
#endif
#include "Magnum/Implementation/QueryState.h"
#include "Magnum/Implementation/State.h"

namespace Magnum {

AbstractQuery::AbstractQuery(GLenum target): _target{target}, _flags{ObjectFlag::DeleteOnDestruction} {
    (this->*Context::current().state().query->createImplementation)();
}

#ifdef MAGNUM_BUILD_DEPRECATED
AbstractQuery::AbstractQuery(): _target{}, _flags{ObjectFlag::DeleteOnDestruction} {
    createImplementationDefault();
}
#endif

AbstractQuery::~AbstractQuery() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &_id);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glDeleteQueriesEXT(1, &_id);
    #else
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
    _flags |= ObjectFlag::Created;
}

void AbstractQuery::createImplementationDefault() {
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &_id);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glGenQueriesEXT(1, &_id);
    #else
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractQuery::createImplementationDSA() {
    glCreateQueries(_target, 1, &_id);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
std::string AbstractQuery::label() const {
    #ifndef MAGNUM_TARGET_GLES
    return Context::current().state().debug->getLabelImplementation(GL_QUERY, _id);
    #else
    return Context::current().state().debug->getLabelImplementation(GL_QUERY_KHR, _id);
    #endif
}

AbstractQuery& AbstractQuery::setLabelInternal(const Containers::ArrayView<const char> label) {
    #ifndef MAGNUM_TARGET_GLES
    Context::current().state().debug->labelImplementation(GL_QUERY, _id, label);
    #else
    Context::current().state().debug->labelImplementation(GL_QUERY_KHR, _id, label);
    #endif
    return *this;
}
#endif

bool AbstractQuery::resultAvailable() {
    GLuint result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    #else
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_AVAILABLE_EXT, &result);
    #endif
    return result == GL_TRUE;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> UnsignedInt AbstractQuery::result<UnsignedInt>() {
    UnsignedInt result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

template<> bool AbstractQuery::result<bool>() { return result<UnsignedInt>() != 0; }

#ifndef MAGNUM_TARGET_WEBGL
template<> Int AbstractQuery::result<Int>() {
    Int result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectiv(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjectivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

template<> UnsignedLong AbstractQuery::result<UnsignedLong>() {
    UnsignedLong result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectui64v(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjectui64vEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

template<> Long AbstractQuery::result<Long>() {
    Long result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjecti64v(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjecti64vEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}
#endif
#endif

void AbstractQuery::begin() {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_INTERNAL_ASSERT(_target);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    glBeginQuery(_target, _id);
    #else
    glBeginQueryEXT(_target, _id);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractQuery::begin(const GLenum target) {
    CORRADE_INTERNAL_ASSERT(!_target || _target == target);

    _target = target;
    begin();
}
#endif

void AbstractQuery::end() {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_INTERNAL_ASSERT(_target);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    glEndQuery(_target);
    #else
    glEndQueryEXT(_target);
    #endif
}

}
