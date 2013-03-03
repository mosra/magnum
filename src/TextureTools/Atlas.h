#ifndef Magnum_TextureTools_Atlas_h
#define Magnum_TextureTools_Atlas_h
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

/** @file
 * @brief Function Magnum::TextureTools::atlas()
 */

#include <vector>

#include "Math/Vector2.h"
#include "Magnum.h"

#include "magnumTextureToolsVisibility.h"

namespace Magnum { namespace TextureTools {

/**
@brief Pack textures into texture atlas
@param atlasSize    Size of resulting atlas
@param sizes        Sizes of all textures in the atlas
@param padding      Padding around each texture

Packs many small textures into one larger. If the textures cannot be packed
into required size, empty vector is returned.

Padding is added twice to each size and the atlas is laid out so the padding
don't overlap. Returned sizes are the same as original sizes, i.e. without the
padding.
*/
std::vector<Rectanglei> MAGNUM_TEXTURETOOLS_EXPORT atlas(const Vector2i& atlasSize, const std::vector<Vector2i>& sizes, const Vector2i& padding = Vector2i());

}}

#endif
