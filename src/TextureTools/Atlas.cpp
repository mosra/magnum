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

#include "Atlas.h"

#include "Math/Functions.h"
#include "Math/Geometry/Rectangle.h"

namespace Magnum { namespace TextureTools {

std::vector<Rectanglei> atlas(const Vector2i& atlasSize, const std::vector<Vector2i>& sizes) {
    if(sizes.empty()) return {};

    /* Size of largest texture */
    Vector2i maxSize;
    for(const Vector2i& size: sizes)
        maxSize = Math::max(maxSize, size);

    std::vector<Rectanglei> atlas;

    /* Columns and rows */
    const Vector2i gridSize = atlasSize/maxSize;
    if(std::size_t(gridSize.product()) < sizes.size()) {
        Error() << "TextureTools::atlas(): requested atlas size" << atlasSize
                << "is too small to fit" << sizes.size() << maxSize
                << "textures. Generated atlas will be empty.";
        return atlas;
    }

    /** @todo actual magic implementation, not this joke */

    atlas.reserve(sizes.size());
    for(std::size_t i = 0; i != sizes.size(); ++i)
        atlas.push_back(Rectanglei::fromSize(Vector2i(i%gridSize.x(), i/gridSize.x())*maxSize, sizes[i]));

    return atlas;
}

}}
