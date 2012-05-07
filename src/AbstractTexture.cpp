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

#include "AbstractTexture.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Check corretness of GL_TEXTURE0 + N == GL_TEXTUREN */
#define texture_assert(num) static_assert(GL_TEXTURE0 + num == GL_TEXTURE##num,\
    "Unsupported constants for GL texture layers")
texture_assert(0); texture_assert(8);  texture_assert(16); texture_assert(24);
texture_assert(1); texture_assert(9);  texture_assert(17); texture_assert(25);
texture_assert(2); texture_assert(10); texture_assert(18); texture_assert(26);
texture_assert(3); texture_assert(11); texture_assert(19); texture_assert(27);
texture_assert(4); texture_assert(12); texture_assert(20); texture_assert(28);
texture_assert(5); texture_assert(13); texture_assert(21); texture_assert(29);
texture_assert(6); texture_assert(14); texture_assert(22); texture_assert(30);
texture_assert(7); texture_assert(15); texture_assert(23); texture_assert(31);
#undef texture_assert

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
#define filter_or(filter, mipmap) \
    (static_cast<GLint>(AbstractTexture::Filter::filter)|static_cast<GLint>(AbstractTexture::Mipmap::mipmap))
static_assert((filter_or(NearestNeighbor, BaseLevel) == GL_NEAREST) &&
              (filter_or(NearestNeighbor, NearestLevel) == GL_NEAREST_MIPMAP_NEAREST) &&
              (filter_or(NearestNeighbor, LinearInterpolation) == GL_NEAREST_MIPMAP_LINEAR) &&
              (filter_or(LinearInterpolation, BaseLevel) == GL_LINEAR) &&
              (filter_or(LinearInterpolation, NearestLevel) == GL_LINEAR_MIPMAP_NEAREST) &&
              (filter_or(LinearInterpolation, LinearInterpolation) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#undef filter_or
#endif

void AbstractTexture::setMinificationFilter(Filter filter, Mipmap mipmap) {
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE || mipmap == Mipmap::BaseLevel, "AbstractTexture: rectangle textures cannot have mipmaps", )

    bind();
    glTexParameteri(_target, GL_TEXTURE_MIN_FILTER,
        static_cast<GLint>(filter)|static_cast<GLint>(mipmap));
}

void AbstractTexture::generateMipmap() {
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE, "AbstractTexture: rectangle textures cannot have mipmaps", )

    bind();
    glGenerateMipmap(_target);
}

AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components components, AbstractTexture::ComponentType type) {
    #define internalFormatSwitch(c) switch(type) {                          \
        case ComponentType::UnsignedByte:                                   \
            internalFormat = GL_##c##8UI; break;                            \
        case ComponentType::Byte:                                           \
            internalFormat = GL_##c##8I; break;                             \
        case ComponentType::UnsignedShort:                                  \
            internalFormat = GL_##c##16UI; break;                           \
        case ComponentType::Short:                                          \
            internalFormat = GL_##c##16I; break;                            \
        case ComponentType::UnsignedInt:                                    \
            internalFormat = GL_##c##32UI; break;                           \
        case ComponentType::Int:                                            \
            internalFormat = GL_##c##32I; break;                            \
        case ComponentType::Half:                                           \
            internalFormat = GL_##c##16F; break;                            \
        case ComponentType::Float:                                          \
            internalFormat = GL_##c##32F; break;                            \
        case ComponentType::NormalizedUnsignedByte:                         \
            internalFormat = GL_##c##8; break;                              \
        case ComponentType::NormalizedByte:                                 \
            internalFormat = GL_##c##8_SNORM; break;                        \
        case ComponentType::NormalizedUnsignedShort:                        \
            internalFormat = GL_##c##16; break;                             \
        case ComponentType::NormalizedShort:                                \
            internalFormat = GL_##c##16_SNORM; break;                       \
    }
    if(components == Components::Red)
        internalFormatSwitch(R)
    else if(components == Components::RedGreen)
        internalFormatSwitch(RG)
    else if(components == Components::RGB)
        internalFormatSwitch(RGB)
    else if(components == Components::RGBA)
        internalFormatSwitch(RGBA)
    #undef internalFormatSwitch
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractTexture::DataHelper<2>::setWrapping(GLenum target, const Math::Vector<Wrapping, 2>& wrapping) {
    CORRADE_ASSERT(target != GL_TEXTURE_RECTANGLE || ((wrapping[0] == Wrapping::ClampToEdge || wrapping[0] == Wrapping::ClampToBorder) && (wrapping[0] == Wrapping::ClampToEdge || wrapping[1] == Wrapping::ClampToEdge)), "AbstractTexture: rectangle texture wrapping must either clamp to border or to edge", )

    glTexParameteri(target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[1]));
}

void AbstractTexture::DataHelper<3>::setWrapping(GLenum target, const Math::Vector<Wrapping, 3>& wrapping) {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[1]));
    glTexParameteri(target, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping[2]));
}
#endif

}
