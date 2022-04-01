#ifndef Magnum_Trade_Implementation_converterUtilities_h
#define Magnum_Trade_Implementation_converterUtilities_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include <chrono>
#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Implementation {

/* Used only in executables where we don't want it to be exported */
namespace {

struct Duration {
    explicit Duration(std::chrono::high_resolution_clock::duration& output): _output(output), _t{std::chrono::high_resolution_clock::now()} {}

    ~Duration() {
        _output += std::chrono::high_resolution_clock::now() - _t;
    }

    private:
        std::chrono::high_resolution_clock::duration& _output;
        std::chrono::high_resolution_clock::time_point _t;
};

struct ImageInfo {
    UnsignedInt image, level;
    bool compressed;
    PixelFormat format;
    CompressedPixelFormat compressedFormat;
    Vector3i size;
    std::size_t dataSize;
    Trade::DataFlags dataFlags;
    Containers::String name;
};

Containers::Array<ImageInfo> imageInfo(AbstractImporter& importer, bool& error, std::chrono::high_resolution_clock::duration& importTime) {
    Containers::Array<ImageInfo> infos;
    for(UnsignedInt i = 0; i != importer.image1DCount(); ++i) {
        const std::string name = importer.image1DName(i);
        const UnsignedInt levelCount = importer.image1DLevelCount(i);

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData1D> image;
            {
                Duration d{importTime};
                if(!(image = importer.image1D(i, j))) {
                    error = true;
                    continue;
                }
            }
            arrayAppend(infos, InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                Vector3i::pad(image->size()),
                image->data().size(),
                image->dataFlags(),
                j ? "" : importer.image1DName(i));
        }
    }
    for(UnsignedInt i = 0; i != importer.image2DCount(); ++i) {
        const std::string name = importer.image2DName(i);
        const UnsignedInt levelCount = importer.image2DLevelCount(i);

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData2D> image;
            {
                Duration d{importTime};
                if(!(image = importer.image2D(i, j))) {
                    error = true;
                    continue;
                }
            }
            arrayAppend(infos, InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                Vector3i::pad(image->size()),
                image->data().size(),
                image->dataFlags(),
                j ? "" : name);
        }
    }
    for(UnsignedInt i = 0; i != importer.image3DCount(); ++i) {
        const std::string name = importer.image3DName(i);
        const UnsignedInt levelCount = importer.image3DLevelCount(i);

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData3D> image;
            {
                Duration d{importTime};
                if(!(image = importer.image3D(i, j))) {
                    error = true;
                    continue;
                }
            }
            arrayAppend(infos, InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                image->size(),
                image->data().size(),
                image->dataFlags(),
                j ? "" : name);
        }
    }

    return infos;
}

}

}}}

#endif
