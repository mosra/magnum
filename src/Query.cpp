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

#include "Query.h"

#include <Utility/Assert.h>

namespace Magnum {

AbstractQuery::AbstractQuery(): target() {
    /** @todo Get some extension wrangler instead to avoid undeclared glGenQueries() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &_id);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGenQueriesEXT(1, &_id);
    #endif
}

AbstractQuery::~AbstractQuery() {
    /** @todo Get some extension wrangler instead to avoid undeclared glGenQueries() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &_id);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glDeleteQueriesEXT(1, &_id);
    #endif
}

bool AbstractQuery::resultAvailable() {
    CORRADE_ASSERT(!target, "AbstractQuery::resultAvailable(): the query is currently running", false);

    /** @todo Re-enable when extension wrangler is available for ES */
    GLuint result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    #endif
    return result == GL_TRUE;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> bool AbstractQuery::result<bool>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension wrangler is available for ES */
    GLuint result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result == GL_TRUE;
}

template<> UnsignedInt AbstractQuery::result<UnsignedInt>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension wrangler is available for ES */
    UnsignedInt result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result;
}

#ifndef MAGNUM_TARGET_GLES3
template<> Int AbstractQuery::result<Int>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension wrangler is available for ES */
    Int result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectiv(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjectivEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result;
}

template<> UnsignedLong AbstractQuery::result<UnsignedLong>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension wrangler is available for ES */
    UnsignedLong result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectui64v(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjectui64vEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result;
}

template<> Long AbstractQuery::result<Long>() {
    CORRADE_ASSERT(!target, "AbstractQuery::result(): the query is currently running", {});

    /** @todo Re-enable when extension wrangler is available for ES */
    Long result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjecti64v(_id, GL_QUERY_RESULT, &result);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glGetQueryObjecti64vEXT(_id, GL_QUERY_RESULT, &result);
    #endif
    return result;
}
#endif
#endif

void AbstractQuery::begin(GLenum target) {
    CORRADE_ASSERT(!this->target, "AbstractQuery::begin(): the query is already running", );

    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glBeginQuery(this->target = target, id());
    #else
    CORRADE_INTERNAL_ASSERT(false);
    static_cast<void>(target);
    //glBeginQueryEXT(this->target = target, id());
    #endif
}

void AbstractQuery::end() {
    CORRADE_ASSERT(target, "AbstractQuery::end(): the query is not running", );

    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glEndQuery(target);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glEndQueryEXT(target);
    #endif
    target = {};
}

}
