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

#include "Sampler.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Sampler.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TextureState.h"

namespace Magnum { namespace GL {

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
#define filter_or(filter, mipmap) (GLint(SamplerFilter::filter)|GLint(SamplerMipmap::mipmap))
static_assert((filter_or(Nearest, Base) == GL_NEAREST) &&
              (filter_or(Nearest, Nearest) == GL_NEAREST_MIPMAP_NEAREST) &&
              (filter_or(Nearest, Linear) == GL_NEAREST_MIPMAP_LINEAR) &&
              (filter_or(Linear, Base) == GL_LINEAR) &&
              (filter_or(Linear, Nearest) == GL_LINEAR_MIPMAP_NEAREST) &&
              (filter_or(Linear, Linear) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#undef filter_or

namespace {

constexpr SamplerFilter FilterMapping[]{
    SamplerFilter::Nearest,
    SamplerFilter::Linear
};

constexpr SamplerMipmap MipmapMapping[]{
    SamplerMipmap::Base,
    SamplerMipmap::Nearest,
    SamplerMipmap::Linear
};

constexpr SamplerWrapping WrappingMapping[]{
    SamplerWrapping::Repeat,
    SamplerWrapping::MirroredRepeat,
    SamplerWrapping::ClampToEdge,
    #ifndef MAGNUM_TARGET_WEBGL
    SamplerWrapping::ClampToBorder,
    #else
    SamplerWrapping{},
    #endif
    #ifndef MAGNUM_TARGET_GLES
    SamplerWrapping::MirrorClampToEdge
    #else
    SamplerWrapping{}
    #endif
};

}

SamplerFilter samplerFilter(const Magnum::SamplerFilter filter) {
    CORRADE_ASSERT(UnsignedInt(filter) < Containers::arraySize(FilterMapping),
        "GL::samplerFilter(): invalid filter" << filter, {});
    return FilterMapping[UnsignedInt(filter)];
}

SamplerMipmap samplerMipmap(const Magnum::SamplerMipmap mipmap) {
    CORRADE_ASSERT(UnsignedInt(mipmap) < Containers::arraySize(MipmapMapping),
        "GL::samplerMipmap(): invalid mode" << mipmap, {});
    return MipmapMapping[UnsignedInt(mipmap)];
}

bool hasSamplerWrapping(const Magnum::SamplerWrapping wrapping) {
    CORRADE_ASSERT(UnsignedInt(wrapping) < Containers::arraySize(WrappingMapping),
        "GL::hasSamplerWrapping(): invalid wrapping" << wrapping, {});
    return UnsignedInt(WrappingMapping[UnsignedInt(wrapping)]);
}

SamplerWrapping samplerWrapping(const Magnum::SamplerWrapping wrapping) {
    CORRADE_ASSERT(UnsignedInt(wrapping) < Containers::arraySize(WrappingMapping),
        "GL::samplerWrapping(): invalid wrapping" << wrapping, {});
    const SamplerWrapping out = WrappingMapping[UnsignedInt(wrapping)];
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::samplerWrapping(): wrapping" << wrapping << "is not supported on this target", {});
    return out;
}

Float Sampler::maxMaxAnisotropy() {
    GLfloat& value = Context::current().state().texture->maxMaxAnisotropy;

    /* Get the value, if not already cached */
    if(value == 0.0f) {
        #ifndef MAGNUM_TARGET_GLES
        if(Context::current().isExtensionSupported<Extensions::ARB::texture_filter_anisotropic>())
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);
        else
        #endif
        if(Context::current().isExtensionSupported<Extensions::EXT::texture_filter_anisotropic>())
            glGetFloatv(
                #ifndef MAGNUM_TARGET_GLES
                GL_MAX_TEXTURE_MAX_ANISOTROPY
                #else
                GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
                #endif
            , &value);
    }

    return value;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const SamplerFilter value) {
    debug << "GL::SamplerFilter" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SamplerFilter::value: return debug << "::" #value;
        _c(Nearest)
        _c(Linear)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SamplerMipmap value) {
    debug << "GL::SamplerMipmap" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SamplerMipmap::value: return debug << "::" #value;
        _c(Base)
        _c(Nearest)
        _c(Linear)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SamplerWrapping value) {
    debug << "GL::SamplerWrapping" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SamplerWrapping::value: return debug << "::" #value;
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

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
Debug& operator<<(Debug& debug, const SamplerCompareMode value) {
    debug << "GL::SamplerCompareMode" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SamplerCompareMode::value: return debug << "::" #value;
        _c(None)
        _c(CompareRefToTexture)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SamplerCompareFunction value) {
    debug << "GL::SamplerCompareFunction" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SamplerCompareFunction::value: return debug << "::" #value;
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

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

#ifndef MAGNUM_TARGET_GLES
Debug& operator<<(Debug& debug, const SamplerDepthStencilMode value) {
    debug << "GL::SamplerDepthStencilMode" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SamplerDepthStencilMode::value: return debug << "::" #value;
        _c(DepthComponent)
        _c(StencilIndex)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif
#endif

}}
