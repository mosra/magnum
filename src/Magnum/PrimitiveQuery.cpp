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

#include "PrimitiveQuery.h"

namespace Magnum {

void PrimitiveQuery::begin() {
    #ifndef MAGNUM_TARGET_GLES
    _index = 0;
    #endif
    AbstractQuery::begin();
}

#ifdef MAGNUM_BUILD_DEPRECATED
void PrimitiveQuery::begin(const Target target) {
    #ifndef MAGNUM_TARGET_GLES
    _index = 0;
    #endif
    AbstractQuery::begin(GLenum(target));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void PrimitiveQuery::begin(const UnsignedInt index) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_INTERNAL_ASSERT(_target);
    #endif

    glBeginQueryIndexed(_target, _index = index, _id);
}
#endif

void PrimitiveQuery::end() {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_INTERNAL_ASSERT(_target);
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!_index) glEndQuery(_target);
    else glEndQueryIndexed(_target, _index);
    #else
    AbstractQuery::end();
    #endif
}

}
