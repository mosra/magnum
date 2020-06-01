#ifndef Magnum_Trade_TgaHeader_h
#define Magnum_Trade_TgaHeader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Magnum/Types.h"

/* Used by both TgaImporter and TgaImageConverter, which is why it isn't
   directly inside TgaImporter.cpp. OTOH it doesn't need to be exposed
   publicly, which is why it has no docblocks. */

namespace Magnum { namespace Trade { namespace Implementation {

#pragma pack(1)
/* TGA file header */
struct TgaHeader {
    UnsignedByte    identsize;      /* Size of ID field that follows header (0) */
    UnsignedByte    colorMapType;   /* 0 = None, 1 = paletted */
    UnsignedByte    imageType;      /* 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle */
    UnsignedShort   colorMapStart;  /* First color map entry */
    UnsignedShort   colorMapLength; /* Number of colors */
    UnsignedByte    colorMapBpp;    /* Bits per palette entry */
    UnsignedShort   beginX;         /* Image x origin */
    UnsignedShort   beginY;         /* Image y origin */
    UnsignedShort   width;          /* Image width */
    UnsignedShort   height;         /* Image height */
    UnsignedByte    bpp;            /* Bits per pixel (8, 16, 24, 32) */
    UnsignedByte    descriptor;     /* Image descriptor */
};
#pragma pack()

static_assert(sizeof(TgaHeader) == 18, "TgaHeader size is not 18 bytes");

}}}

#endif
