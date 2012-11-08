/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Query.h"

namespace Magnum {

bool AbstractQuery::resultAvailable() {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    GLuint result;
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    return result == GL_TRUE;
    #else
    return false;
    #endif
}

template<> bool AbstractQuery::result<bool>() {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    GLuint result;
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    return result == GL_TRUE;
    #else
    return false;
    #endif
}

template<> GLuint AbstractQuery::result<GLuint>() {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    GLuint result;
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    return result;
    #else
    return 0;
    #endif
}

#ifndef MAGNUM_TARGET_GLES
template<> GLint AbstractQuery::result<GLint>() {
    GLint result;
    glGetQueryObjectiv(_id, GL_QUERY_RESULT, &result);
    return result;
}

template<> GLuint64 AbstractQuery::result<GLuint64>() {
    GLuint64 result;
    glGetQueryObjectui64v(_id, GL_QUERY_RESULT, &result);
    return result;
}

template<> GLint64 AbstractQuery::result<GLint64>() {
    GLint64 result;
    glGetQueryObjecti64v(_id, GL_QUERY_RESULT, &result);
    return result;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Query::begin(Query::Target target) {
    glBeginQuery(static_cast<GLenum>(target), id());
    this->target = new Target(target);
}

void Query::end() {
    if(!target) return;

    glEndQuery(static_cast<GLenum>(*target));
    delete target;
    target = nullptr;
}
#endif

void SampleQuery::begin(SampleQuery::Target target) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glBeginQuery(static_cast<GLenum>(target), id());
    #endif
    this->target = new Target(target);
}

void SampleQuery::end() {
    if(!target) return;

    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glEndQuery(static_cast<GLenum>(*target));
    #endif
    delete target;
    target = nullptr;
}

}
