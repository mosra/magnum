/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "TgaImageConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

/** @todo doesn't populate config options correctly, deprecate (used in
    MagnumFontConverter currently) */
TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractImageConverter{manager, plugin} {}

ImageConverterFeatures TgaImageConverter::doFeatures() const { return ImageConverterFeature::Convert2DToData; }

Containers::String TgaImageConverter::doExtension() const { return "tga"_s; }

Containers::String TgaImageConverter::doMimeType() const {
    /* https://en.wikipedia.org/wiki/Truevision_TGA says there's no registered
       MIME type. It probably never will be. Using `file --mime-type` on a TGA
       file returns image/x-tga, so using that here as well. */
    return "image/x-tga"_s;
}

template<class T> T swizzle(const T& value);
template<> inline UnsignedByte swizzle(const UnsignedByte& value) {
    return value;
}
template<> inline Vector3ub swizzle(const Vector3ub& value) {
    return Math::gather<'b', 'g', 'r'>(value);
}
template<> inline Vector4ub swizzle(const Vector4ub& value) {
    return Math::gather<'b', 'g', 'r', 'a'>(value);
}

template<class T> void rleEncode(Containers::Array<char>& data, const ImageView2D& image, const bool rleAcrossScanlines) {
    /* Pixel array and current position in it. Can't iterate linearly in data()
       because the input may have arbitrary padding between rows. */
    const Containers::StridedArrayView2D<const T> pixels = image.pixels<T>();
    std::size_t y = 0;
    std::size_t x = 1;
    if(pixels.size()[1] == 1) {
        y = 1;
        x = 0;
    }

    /* Value of previous pixel, as an union to make it easy to append to the
       char array. Pre-swizzled so we don't need to swizzle in each
       arrayAppend() call. */
    union {
        T pixel;
        char data[sizeof(T)];
    } prev{swizzle(pixels[0][0])};

    /* Offset where a sequence run header placeholder is stored. Gets filled
       with the actual count once the sequence ends. */
    Containers::Optional<std::size_t> sequenceRunHeaderOffset;
    /* Size of a sequence run / repeat count in a repeat run. If 1, it can be
       either of the two, if > 1 then it depends on whether
       sequenceRunHeaderOffset is NullOpt or not. */
    std::size_t count = 1;

    /* Go through all pixels, cache the row access for better perf on debug
       builds */
    Containers::StridedArrayView1D<const T> currentRow;
    while(y < pixels.size()[0]) {
        if(!currentRow) currentRow = pixels[y];
        /* Current pixel, again pre-swizzled so we don't need to swizzle in
           each arrayAppend() call */
        const T current = swizzle(currentRow[x]);

        /* Reset the counter if it's 128, as we can't store more than that, or
           if we're at the new scanline and RLE across scanlines is disabled */
        if(count == 128 || (x == 0 && !rleAcrossScanlines)) {
            if(sequenceRunHeaderOffset) {
                arrayAppend(data, prev.data);
                /* The amount of data written since the header be should equal
                   to the sequence run size */
                CORRADE_INTERNAL_ASSERT(data.size() - *sequenceRunHeaderOffset - 1 == count*sizeof(T));
                data[*sequenceRunHeaderOffset] = char(UnsignedByte(0x00|(count - 1)));
                sequenceRunHeaderOffset = {};
            } else {
                /* If it's just one pixel, make it a sequence instead for
                   consistency */
                arrayAppend(data, count == 1 ? '\x00' : char(UnsignedByte(0x80|(count - 1))));
                arrayAppend(data, prev.data);
            }

            count = 0;

        /* Otherwise, if the next pixel is same like previous, count towards a
           repeat run */
        } else if(current == prev.pixel) {
            /* There was a sequence run before, finish it with the value before
               the previous pixel (i.e., so both the previous and current pixel
               are a part of the new repeat run) */
            if(sequenceRunHeaderOffset) {
                /* If count is 1, runHeader should be nullptr */
                CORRADE_INTERNAL_ASSERT(count > 1);
                /* The amount of data written since the header be should equal
                   to the sequence run size (excluding the previous pixel) */
                CORRADE_INTERNAL_ASSERT(data.size() - *sequenceRunHeaderOffset - 1 == (count - 1)*sizeof(T));
                data[*sequenceRunHeaderOffset] = char(UnsignedByte(0x00|(count - 2)));
                sequenceRunHeaderOffset = {};
                count = 1;
            }

        /* Otherwise, if the current pixel is different from the previous,
           count towards a sequence run */
        } else {
            /* If we don't have a sequence run header written yet, it can mean
               that there's either a repeat run, or the previous pixel was
               also different */
            if(!sequenceRunHeaderOffset) {
                /* If the previous pixel was standalone, write it with a
                   placeholder for a sequence run header before. If the next
                   pixel is different from the current one, this run will be
                   extended, otherwise it'll be ended and a new repeat run will
                   be started from the current pixel. */
                if(count == 1) {
                    sequenceRunHeaderOffset = data.size();
                    arrayAppend(data, NoInit, 1);
                    arrayAppend(data, prev.data);
                    /* Keeping count at 1 */

                /* Otherwise, there was a repeat run before. Finish it with the
                   previous pixel (i.e., so the current pixel is a start of a
                   new run). */
                } else {
                    arrayAppend(data, char(UnsignedByte(0x80|(count - 1))));
                    arrayAppend(data, prev.data);
                    count = 0;
                }

            /* If we have a sequence run header written, write the prev pixel.
               *Not* the current one because it might be the beginning of a
               repeat run. */
            } else arrayAppend(data, prev.data);
        }

        prev.pixel = current;
        ++count;
        ++x;

        if(x == pixels.size()[1]) {
            ++y;
            currentRow = nullptr;
            x = 0;
        }
    }

    /* We should be at the end of the input and there should be at least one
       yet-unwritten pixel left */
    CORRADE_INTERNAL_ASSERT(x == 0 && y == pixels.size()[0]);
    CORRADE_INTERNAL_ASSERT(count >= 1);

    /* If there's an unfinished sequence run header, write the count to it,
       and put the last unwritten pixel there as well */
    if(sequenceRunHeaderOffset) {
        arrayAppend(data, prev.data);
        /* The amount of data written since the header should be again equal to
           the sequence run size */
        CORRADE_INTERNAL_ASSERT(data.size() - *sequenceRunHeaderOffset - 1 == count*sizeof(T));
        data[*sequenceRunHeaderOffset] = char(UnsignedByte(0x00|(count - 1)));

    /* Otherwise write a repeat header with the last pixel */
    } else {
        /* If it's just one pixel, make it a sequence instead for consistency */
        arrayAppend(data, count == 1 ? '\x00' : char(UnsignedByte(0x80|(count - 1))));
        arrayAppend(data, prev.data);
    }
}

Containers::Optional<Containers::Array<char>> TgaImageConverter::doConvertToData(const ImageView2D& image) {
    /* Warn about lost metadata */
    if((image.flags() & ImageFlag2D::Array) && !(flags() & ImageConverterFlag::Quiet)) {
        Warning{} << "Trade::TgaImageConverter::convertToData(): 1D array images are unrepresentable in TGA, saving as a regular 2D image";
    }

    /* Initialize data buffer. If we're writing a RLE-encoded file, create a
       growable array (which we have to copy to a non-growable after after), if
       not then allocate exactly the amount of bytes so we don't need to copy
       after. */
    const auto pixelSize = UnsignedByte(image.pixelSize());
    const std::size_t uncompressedSize = sizeof(Implementation::TgaHeader) + pixelSize*image.size().product();
    const bool rle = configuration().value<bool>("rle");
    Containers::Array<char> data;
    if(rle) {
        arrayAppend(data, NoInit, sizeof(Implementation::TgaHeader));
    } else {
        data = Containers::Array<char>{NoInit, uncompressedSize};
    }

    /* Clear the header and fill non-zero values */
    auto& header = *reinterpret_cast<Implementation::TgaHeader*>(data.begin());
    header = {};
    switch(image.format()) {
        case PixelFormat::RGB8Unorm:
            if(flags() & ImageConverterFlag::Verbose)
                Debug{} << "Trade::TgaImageConverter::convertToData(): converting from RGB to BGR";
            header.imageType = 2;
            break;
        case PixelFormat::RGBA8Unorm:
            if(flags() & ImageConverterFlag::Verbose)
                Debug{} << "Trade::TgaImageConverter::convertToData(): converting from RGBA to BGRA";
            header.imageType = 2;
            break;
        case PixelFormat::R8Unorm:
            header.imageType = 3;
            break;
        default:
            Error() << "Trade::TgaImageConverter::convertToData(): unsupported pixel format" << image.format();
            return {};
    }
    header.bpp = pixelSize*8;
    header.width = UnsignedShort(Utility::Endianness::littleEndian(image.size().x()));
    header.height = UnsignedShort(Utility::Endianness::littleEndian(image.size().y()));

    /* Perform RLE encoding */
    if(rle) {
        header.imageType |= 8;

        const bool rleAcrossScanlines = configuration().value<bool>("rleAcrossScanlines");
        switch(image.format()) {
            case PixelFormat::R8Unorm:
                rleEncode<UnsignedByte>(data, image, rleAcrossScanlines);
                break;
            case PixelFormat::RGB8Unorm:
                rleEncode<Vector3ub>(data, image, rleAcrossScanlines);
                break;
            case PixelFormat::RGBA8Unorm:
                rleEncode<Vector4ub>(data, image, rleAcrossScanlines);
                break;
            default: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* If RLE wasn't used or if a RLE output is larger than uncompressed
       output, write an uncompressed output instead */
    if(!rle || (data.size() > uncompressedSize && configuration().value<bool>("rleFallbackIfLarger"))) {
        if(rle) {
            if(flags() & ImageConverterFlag::Verbose)
                Debug{} << "Trade::TgaImageConverter::convertToData(): RLE output" << data.size() - uncompressedSize << "bytes larger than uncompressed, falling back to uncompressed";

            /* Resize the array to exactly the uncompressed size (this will
               always shrink, never grow) */
            arrayResize(data, NoInit, uncompressedSize);

            /* Remove the RLE bit from the header. Can't use the header
               variable from above as it may be dangling at this point. */
            reinterpret_cast<Implementation::TgaHeader*>(data.begin())->imageType &= ~8;
        }

        const Containers::ArrayView<char> pixels = data.exceptPrefix(sizeof(Implementation::TgaHeader));
        Utility::copy(image.pixels(), Containers::StridedArrayView3D<char>{pixels,
            {std::size_t(image.size().y()), std::size_t(image.size().x()), pixelSize}});

        if(image.format() == PixelFormat::RGB8Unorm) {
            for(Vector3ub& pixel: Containers::arrayCast<Vector3ub>(pixels))
                pixel = Math::gather<'b', 'g', 'r'>(pixel);
        } else if(image.format() == PixelFormat::RGBA8Unorm) {
            for(Vector4ub& pixel: Containers::arrayCast<Vector4ub>(pixels))
                pixel = Math::gather<'b', 'g', 'r', 'a'>(pixel);
        }
    }

    /* If we started with a RLE-encoded file, turn the array back into a
       non-growable to avoid a dangling deleter on plugin unload */
    if(rle) arrayShrink(data);

    /* GCC 4.8 needs extra help here */
    return Containers::optional(Utility::move(data));
}

}}

CORRADE_PLUGIN_REGISTER(TgaImageConverter, Magnum::Trade::TgaImageConverter,
    MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE)
