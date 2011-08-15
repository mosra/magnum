#ifndef Magnum_Examples_TGATexture_h
#define Magnum_Examples_TGATexture_h
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

#include <string>

namespace Magnum { namespace Examples {

class TGATexture: public Texture2D {
    public:
        TGATexture(const std::string& filename);

    private:
        #pragma pack(1)
        struct Header {
            GLbyte  identsize;              /**< @brief Size of ID field that follows header (0) */
            GLbyte  colorMapType;           /**< @brief 0 = None, 1 = paletted */
            GLbyte  imageType;              /**< @brief 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle */
            unsigned short  colorMapStart;  /**< @brief First color map entry */
            unsigned short  colorMapLength; /**< @brief Number of colors */
            unsigned char   colorMapBpp;    /**< @brief Bits per palette entry */
            unsigned short  beginX;         /**< @brief Image x origin */
            unsigned short  beginY;         /**< @brief Image y origin */
            unsigned short  width;          /**< @brief Image width */
            unsigned short  height;         /**< @brief Image height */
            GLbyte  bpp;                    /**< @brief Bits per pixel (8 16, 24, 32) */
            GLbyte  descriptor;             /**< @brief Image descriptor */
        };
        #pragma pack(8)
};

}}

#endif
