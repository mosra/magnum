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

#include "RectangleTexture.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

Vector2i RectangleTexture::maxSize() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        return {};

    GLint& value = Context::current()->state().texture->maxRectangleSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &value);

    return Vector2i{value};
}

}
#endif
