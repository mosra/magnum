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

#include "maxTextureSize.h"

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TextureState.h"

namespace Magnum { namespace GL { namespace Implementation {

GLint maxTextureSideSize() {
    GLint& value = Context::current().state().texture->maxSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);

    return value;
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
GLint max3DTextureDepth() {
    GLint& value = Context::current().state().texture->max3DSize;

    if(value == 0)
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &value);
        #else
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE_OES, &value);
        #endif

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
GLint maxTextureArrayLayers() {
    GLint& value = Context::current().state().texture->maxArrayLayers;

    if(value == 0)
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &value);

    return value;
}
#endif

GLint maxCubeMapTextureSideSize() {
    GLint& value = Context::current().state().texture->maxCubeMapSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &value);

    return value;
}

}}}
