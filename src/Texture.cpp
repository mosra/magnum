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

#include "Texture.h"

namespace Magnum {

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on Texture1D. */
static_assert(((Texture1D::NearestNeighborFilter|Texture1D::BaseMipLevel) == GL_NEAREST) &&
              ((Texture1D::NearestNeighborFilter|Texture1D::NearestMipLevel) == GL_NEAREST_MIPMAP_NEAREST) &&
              ((Texture1D::NearestNeighborFilter|Texture1D::LinearMipInterpolation) == GL_NEAREST_MIPMAP_LINEAR) &&
              ((Texture1D::LinearFilter|Texture1D::BaseMipLevel) == GL_LINEAR) &&
              ((Texture1D::LinearFilter|Texture1D::NearestMipLevel) == GL_LINEAR_MIPMAP_NEAREST) &&
              ((Texture1D::LinearFilter|Texture1D::LinearMipInterpolation) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");

template<size_t dimensions> void Texture<dimensions>::setWrapping(const Math::Vector<Wrapping, dimensions>& wrapping) {
    bind();
    for(int i = 0; i != dimensions; ++i) {
        /* Repeat wrap modes are not available on rectangle textures. */
        if(target == GL_TEXTURE_RECTANGLE && (wrapping.at(i) == Repeat || wrapping.at(i) == MirroredRepeat))
            continue;

        switch(i) {
            case 0:
                glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapping.at(i));
                break;
            case 1:
                glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapping.at(i));
                break;
            case 2:
                glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapping.at(i));
                break;
        }
    }
    unbind();
}

template<size_t dimensions> void Texture<dimensions>::setMinificationFilter(Filter filter, Mipmap mipmap) {
    /* Only base mip level is supported on rectangle textures */
    if(target == GL_TEXTURE_RECTANGLE) mipmap = BaseMipLevel;

    bind();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter|mipmap);
    unbind();
}

template<size_t dimensions> void Texture<dimensions>::generateMipmap() {
    if(target == GL_TEXTURE_RECTANGLE) return;

    bind();
    glGenerateMipmap(target);
    unbind();
}

/* Instantiate all textures */
template class Texture<1>;
template class Texture<2>;
template class Texture<3>;

}
