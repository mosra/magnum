/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
static_assert(((AbstractTexture::NearestNeighborFilter|AbstractTexture::BaseMipLevel) == GL_NEAREST) &&
              ((AbstractTexture::NearestNeighborFilter|AbstractTexture::NearestMipLevel) == GL_NEAREST_MIPMAP_NEAREST) &&
              ((AbstractTexture::NearestNeighborFilter|AbstractTexture::LinearMipInterpolation) == GL_NEAREST_MIPMAP_LINEAR) &&
              ((AbstractTexture::LinearFilter|AbstractTexture::BaseMipLevel) == GL_LINEAR) &&
              ((AbstractTexture::LinearFilter|AbstractTexture::NearestMipLevel) == GL_LINEAR_MIPMAP_NEAREST) &&
              ((AbstractTexture::LinearFilter|AbstractTexture::LinearMipInterpolation) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#endif

void AbstractTexture::setMinificationFilter(Filter filter, Mipmap mipmap) {
    /* Only base mip level is supported on rectangle textures */
    if(target == GL_TEXTURE_RECTANGLE) mipmap = BaseMipLevel;

    bind();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter|mipmap);
    unbind();
}

void AbstractTexture::generateMipmap() {
    if(target == GL_TEXTURE_RECTANGLE) return;

    bind();
    glGenerateMipmap(target);
    unbind();
}

}
