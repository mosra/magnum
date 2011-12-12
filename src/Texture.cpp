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

template<size_t dimensions> void Texture<dimensions>::setWrapping(const Math::Vector<Wrapping, Dimensions>& wrapping) {
    bind();
    for(int i = 0; i != dimensions; ++i) {
        /* Repeat wrap modes are not available on rectangle textures. */
        if(target == GL_TEXTURE_RECTANGLE && (wrapping.at(i) == Wrapping::Repeat || wrapping.at(i) == Wrapping::MirroredRepeat))
            continue;

        switch(i) {
            case 0:
                glTexParameteri(target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.at(i)));
                break;
            case 1:
                glTexParameteri(target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping.at(i)));
                break;
            case 2:
                glTexParameteri(target, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping.at(i)));
                break;
        }
    }
    unbind();
}

/* Instantiate all textures */
template class Texture<1>;
template class Texture<2>;
template class Texture<3>;

}
