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

#include "Sampler.h"

#include <Utility/Debug.h>

#include "Context.h"
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

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

#ifndef MAGNUM_TARGET_GLES3
Float Sampler::maxSupportedAnisotropy() {
    GLfloat& value = Context::current()->state()->texture->maxSupportedAnisotropy;

    /** @todo Re-enable when extension header is available */
    #ifndef MAGNUM_TARGET_GLES
    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
    #endif

    return value;
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, const Sampler::Filter value) {
    switch(value) {
        #define _c(value) case Sampler::Filter::value: return debug << "Sampler::Filter::" #value;
        _c(Nearest)
        _c(Linear)
        #undef _c
    }

    return debug << "Sampler::Filter::(invalid)";
}

Debug operator<<(Debug debug, const Sampler::Mipmap value) {
    switch(value) {
        #define _c(value) case Sampler::Mipmap::value: return debug << "Sampler::Mipmap::" #value;
        _c(Base)
        _c(Nearest)
        _c(Linear)
        #undef _c
    }

    return debug << "Sampler::Mipmap::(invalid)";
}

Debug operator<<(Debug debug, const Sampler::Wrapping value) {
    switch(value) {
        #define _c(value) case Sampler::Wrapping::value: return debug << "Sampler::Wrapping::" #value;
        _c(Repeat)
        _c(MirroredRepeat)
        _c(ClampToEdge)
        #ifndef MAGNUM_TARGET_GLES3
        _c(ClampToBorder)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(MirrorClampToEdge)
        #endif
        #undef _c
    }

    return debug << "Sampler::Wrapping::(invalid)";
}
#endif

}
