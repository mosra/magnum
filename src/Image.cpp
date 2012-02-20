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

#include "Image.h"

namespace Magnum {

void Image2D::setDataFromFramebuffer(const Math::Vector2<GLint>& offset) {
    glReadPixels(offset.at(0), offset.at(1), _dimensions.at(0), _dimensions.at(1), static_cast<GLenum>(_colorFormat), static_cast<GLenum>(_type), _data);
}

}
