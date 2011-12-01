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
