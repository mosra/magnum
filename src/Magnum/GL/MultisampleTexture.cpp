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

#include "MultisampleTexture.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#include "Magnum/GL/Implementation/maxTextureSize.h"

namespace Magnum { namespace GL { namespace Implementation {

template<> Vector2i MAGNUM_GL_EXPORT maxMultisampleTextureSize<2>() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return Vector2i{0};

    return Vector2i{Implementation::maxTextureSideSize()};
}

template<> Vector3i MAGNUM_GL_EXPORT maxMultisampleTextureSize<3>() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_multisample>())
        return Vector3i{0};
    #else
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_storage_multisample_2d_array>())
        return Vector3i{0};
    #endif

    return {Vector2i{Implementation::maxTextureSideSize()}, Implementation::max3DTextureDepth()};
}

}}}
#endif
