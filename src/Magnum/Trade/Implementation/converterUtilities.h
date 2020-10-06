#ifndef Magnum_Trade_Implementation_converterUtilities_h
#define Magnum_Trade_Implementation_converterUtilities_h
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

#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Implementation {

/* Used only in executables where we don't want it to be exported */
namespace {

struct ImageInfo {
    UnsignedInt image, level;
    bool compressed;
    PixelFormat format;
    CompressedPixelFormat compressedFormat;
    Vector3i size;
    std::string name;
};

Containers::Array<ImageInfo> imageInfo(AbstractImporter& importer, bool& error, bool& compact) {
    Containers::Array<ImageInfo> infos;
    for(UnsignedInt i = 0; i != importer.image1DCount(); ++i) {
        const std::string name = importer.image1DName(i);
        const UnsignedInt levelCount = importer.image1DLevelCount(i);
        if(!name.empty() || levelCount != 1) compact = false;

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData1D> image = importer.image1D(i, j);
            if(!image) {
                error = true;
                continue;
            }
            arrayAppend(infos, Containers::InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                Vector3i::pad(image->size()),
                j ? "" : importer.image1DName(i));
        }
    }
    for(UnsignedInt i = 0; i != importer.image2DCount(); ++i) {
        const std::string name = importer.image2DName(i);
        const UnsignedInt levelCount = importer.image2DLevelCount(i);
        if(!name.empty() || levelCount != 1) compact = false;

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData2D> image = importer.image2D(i, j);
            if(!image) {
                error = true;
                continue;
            }
            arrayAppend(infos, Containers::InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                Vector3i::pad(image->size()),
                j ? "" : name);
        }
    }
    for(UnsignedInt i = 0; i != importer.image3DCount(); ++i) {
        const std::string name = importer.image3DName(i);
        const UnsignedInt levelCount = importer.image3DLevelCount(i);
        if(!name.empty() || levelCount != 1) compact = false;

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData3D> image = importer.image3D(i, j);
            if(!image) {
                error = true;
                continue;
            }
            arrayAppend(infos, Containers::InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                image->size(),
                j ? "" : name);
        }
    }

    return infos;
}

}

}}}

#endif
