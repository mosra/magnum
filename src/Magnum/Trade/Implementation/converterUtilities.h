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
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Implementation {

/* Used only in executables where we don't want it to be exported -- in
   particular magnum-imageconverter, magnum-sceneconverter and their tests */
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

union ImageInfoFlags {
    /* Wow, C++, YOU FUCKING SUCK, how is this not the implicit behavior?!! */
    ImageInfoFlags(ImageFlags1D flags): one{flags} {}
    ImageInfoFlags(ImageFlags2D flags): two{flags} {}
    ImageInfoFlags(ImageFlags3D flags): three{flags} {}

    ImageFlags1D one;
    ImageFlags2D two;
    ImageFlags3D three;
};

struct ImageInfo {
    UnsignedInt image, level;
    bool compressed;
    PixelFormat format;
    CompressedPixelFormat compressedFormat;
    Vector3i size;
    std::size_t dataSize;
    Trade::DataFlags dataFlags;
    ImageInfoFlags flags;
    Containers::String name;
};

Containers::Array<ImageInfo> imageInfo(AbstractImporter& importer, bool& error, std::chrono::high_resolution_clock::duration& importTime) {
    Containers::Array<ImageInfo> infos;
    for(UnsignedInt i = 0; i != importer.image1DCount(); ++i) {
        const Containers::String name = importer.image1DName(i);
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
                ImageInfoFlags{image->flags()},
                j ? "" : name);
        }
    }
    for(UnsignedInt i = 0; i != importer.image2DCount(); ++i) {
        const Containers::String name = importer.image2DName(i);
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
                ImageInfoFlags{image->flags()},
                j ? "" : name);
        }
    }
    for(UnsignedInt i = 0; i != importer.image3DCount(); ++i) {
        const Containers::String name = importer.image3DName(i);
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
                ImageInfoFlags{image->flags()},
                j ? "" : name);
        }
    }

    return infos;
}

void printImageInfo(const Debug::Flags useColor, const Containers::ArrayView<const ImageInfo> imageInfos, const Containers::ArrayView<const UnsignedInt> image1DReferenceCount, const Containers::ArrayView<const UnsignedInt> image2DReferenceCount, const Containers::ArrayView<const UnsignedInt> image3DReferenceCount) {
    std::size_t totalImageDataSize = 0;
    for(const Trade::Implementation::ImageInfo& info: imageInfos) {
        Debug d{useColor};
        if(info.level == 0) {
            d << Debug::boldColor(Debug::Color::Default);
            if(info.size.z()) d << "3D image";
            else if(info.size.y()) d << "2D image";
            else d << "1D image";
            d << info.image << Debug::resetColor;

            /* Print reference count only if there actually are any (i.e., the
               arrays are non-empty) otherwise this information is useless */
            Containers::Optional<UnsignedInt> count;
            if(info.size.z() && image3DReferenceCount) {
                count = image3DReferenceCount[info.image];
            } else if(info.size.y() && image2DReferenceCount) {
                count = image2DReferenceCount[info.image];
            } else if(image1DReferenceCount) {
                count = image1DReferenceCount[info.image];
            }
            if(count) {
                if(!*count) d << Debug::color(Debug::Color::Red);
                d << "(referenced by" << *count << "textures)";
                if(!*count) d << Debug::resetColor;
            }

            d << Debug::boldColor(Debug::Color::Default) << Debug::nospace << ":"
                << Debug::resetColor;
            if(info.name) d << Debug::boldColor(Debug::Color::Yellow)
                << info.name << Debug::resetColor;
            d << Debug::newline;
        }
        d << "  Level" << info.level << Debug::nospace << ":";
        if(info.flags.one) {
            d << Debug::packed << Debug::color(Debug::Color::Cyan);
            if(info.size.z()) d << info.flags.three;
            else if(info.size.y()) d << info.flags.two;
            else d << info.flags.one;
            d << Debug::resetColor;
        }
        d << Debug::packed;
        if(info.size.z()) d << info.size;
        else if(info.size.y()) d << info.size.xy();
        /* Kinda unnecessary, but makes the output more consistent if also 1D
           size is in {}s */
        else d << Math::Vector<1, Int>(info.size.x());
        d << Debug::color(Debug::Color::Blue) << "@" << Debug::resetColor;
        d << Debug::packed;
        /* Compressed formats are printed yellow. That kinda conflicts with
           custom fields / attributes elsewhere, but is significant enough to
           have it highlighted. */
        if(info.compressed) d << Debug::color(Debug::Color::Yellow) << info.compressedFormat;
        else d << Debug::color(Debug::Color::Cyan) << info.format;
        d << Debug::resetColor << "(" << Debug::nospace << Utility::format("{:.1f}", info.dataSize/1024.0f) << "kB";
        if(info.dataFlags != (Trade::DataFlag::Owned|Trade::DataFlag::Mutable))
            d << Debug::nospace << "," << Debug::packed
                << Debug::color(Debug::Color::Green) << info.dataFlags
                << Debug::resetColor;
        d << Debug::nospace << ")";

        totalImageDataSize += info.dataSize;
    }
    if(!imageInfos.isEmpty())
        Debug{} << "Total image data size:" << Utility::format("{:.1f}", totalImageDataSize/1024.0f) << "kB";
}

}

}}}

#endif
