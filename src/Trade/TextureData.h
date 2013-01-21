#ifndef Magnum_Trade_TextureData_h
#define Magnum_Trade_TextureData_h
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
 * @brief Class Magnum::Trade::TextureData
 */

#include <string>

namespace Magnum { namespace Trade {

/**
@brief %Texture data
*/
class MAGNUM_EXPORT TextureData {
    TextureData(const TextureData& other) = delete;
    TextureData(TextureData&& other) = delete;
    TextureData& operator=(const TextureData& other) = delete;
    TextureData& operator=(TextureData&& other) = delete;
};

}}

#endif
