/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "MultisampleTexture.h"

#ifndef MAGNUM_TARGET_GLES2
#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/StringView.h>
#endif

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#include "Magnum/GL/Implementation/maxTextureSize.h"

namespace Magnum { namespace GL {

namespace Implementation {

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

}

template<UnsignedInt dimensions> MultisampleTexture<dimensions> MultisampleTexture<dimensions>::view(MultisampleTexture2D& original, const TextureFormat internalFormat) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    MultisampleTexture<dimensions> out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, 0, 1, 0, 1);
    return out;
}

/* On Windows (MSVC, clang-cl and MinGw) these need an explicit export
   otherwise the symbol doesn't get exported. */
template<> template<> MAGNUM_GL_EXPORT MultisampleTexture2D MultisampleTexture2D::view(MultisampleTexture2DArray& original, const TextureFormat internalFormat, const Int layer) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    MultisampleTexture2D out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, 0, 1, layer, 1);
    return out;
}

template<> template<> MAGNUM_GL_EXPORT MultisampleTexture2DArray MultisampleTexture2DArray::view(MultisampleTexture2DArray& original, const TextureFormat internalFormat, const Int layerOffset, const Int layerCount) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    MultisampleTexture2DArray out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, 0, 1, layerOffset, layerCount);
    return out;
}

template<UnsignedInt dimensions> MultisampleTexture<dimensions>& MultisampleTexture<dimensions>::setLabel(Containers::StringView label) {
    AbstractTexture::setLabel(label);
    return *this;
}

template class
    /* GCC needs the export macro on the class definition (and here it warns
       that the type is already defined so the export is ignored), while Clang
       and MSVC need it here (and ignore it on the declaration) */
    #if defined(CORRADE_TARGET_CLANG) || defined(CORRADE_TARGET_MSVC)
    MAGNUM_GL_EXPORT
    #endif
    MultisampleTexture<2>;
template class
    #if defined(CORRADE_TARGET_CLANG) || defined(CORRADE_TARGET_MSVC)
    MAGNUM_GL_EXPORT
    #endif
    MultisampleTexture<3>;

}}
#endif
