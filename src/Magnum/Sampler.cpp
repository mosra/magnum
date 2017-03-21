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

#include "Sampler.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Context.h"
#include "Magnum/Implementation/State.h"
#include "Magnum/Implementation/TextureState.h"
#include "Magnum/Extensions.h"

namespace Magnum {

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
#define filter_or(filter, mipmap) (GLint(Sampler::Filter::filter)|GLint(Sampler::Mipmap::mipmap))
static_assert((filter_or(Nearest, Base) == GL_NEAREST) &&
              (filter_or(Nearest, Nearest) == GL_NEAREST_MIPMAP_NEAREST) &&
              (filter_or(Nearest, Linear) == GL_NEAREST_MIPMAP_LINEAR) &&
              (filter_or(Linear, Base) == GL_LINEAR) &&
              (filter_or(Linear, Nearest) == GL_LINEAR_MIPMAP_NEAREST) &&
              (filter_or(Linear, Linear) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#undef filter_or

Float Sampler::maxMaxAnisotropy() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_filter_anisotropic>())
        return 0.0f;

    GLfloat& value = Context::current().state().texture->maxMaxAnisotropy;

    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);

    return value;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const Sampler::Filter value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Sampler::Filter::value: return debug << "Sampler::Filter::" #value;
        _c(Nearest)
        _c(Linear)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Sampler::Filter(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Sampler::Mipmap value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Sampler::Mipmap::value: return debug << "Sampler::Mipmap::" #value;
        _c(Base)
        _c(Nearest)
        _c(Linear)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Sampler::Mipmap(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Sampler::Wrapping value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Sampler::Wrapping::value: return debug << "Sampler::Wrapping::" #value;
        _c(Repeat)
        _c(MirroredRepeat)
        _c(ClampToEdge)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(ClampToBorder)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(MirrorClampToEdge)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Sampler::Wrapping(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
Debug& operator<<(Debug& debug, const Sampler::CompareMode value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Sampler::CompareMode::value: return debug << "Sampler::CompareMode::" #value;
        _c(None)
        _c(CompareRefToTexture)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Sampler::CompareMode(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Sampler::CompareFunction value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Sampler::CompareFunction::value: return debug << "Sampler::CompareFunction::" #value;
        _c(Never)
        _c(Always)
        _c(Less)
        _c(LessOrEqual)
        _c(Equal)
        _c(NotEqual)
        _c(GreaterOrEqual)
        _c(Greater)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Sampler::CompareFunction(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

#ifndef MAGNUM_TARGET_GLES
Debug& operator<<(Debug& debug, const Sampler::DepthStencilMode value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Sampler::DepthStencilMode::value: return debug << "Sampler::DepthStencilMode::" #value;
        _c(DepthComponent)
        _c(StencilIndex)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Sampler::DepthStencilMode(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif
#endif

}
