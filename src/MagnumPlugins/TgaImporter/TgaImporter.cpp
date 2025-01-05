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

#include "TgaImporter.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

TgaImporter::TgaImporter() = default;

TgaImporter::TgaImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractImporter{manager, plugin} {}

TgaImporter::~TgaImporter() = default;

ImporterFeatures TgaImporter::doFeatures() const { return ImporterFeature::OpenData; }

bool TgaImporter::doIsOpened() const { return _in; }

void TgaImporter::doClose() { _in = nullptr; }

void TgaImporter::doOpenData(Containers::Array<char>&& data, const DataFlags dataFlags) {
    /* Because here we're copying the data and using the _in to check if file
       is opened, having them nullptr would mean openData() would fail without
       any error message. It's not possible to do this check on the importer
       side, because empty file is valid in some formats (OBJ or glTF). We also
       can't do the full import here because then doImage2D() would need to
       copy the imported data instead anyway. This way it'll also work nicely
       with a future openMemory(). */
    if(data.isEmpty()) {
        Error{} << "Trade::TgaImporter::openData(): the file is empty";
        return;
    }

    /* Ttake over the existing array or copy the data if we can't */
    if(dataFlags & (DataFlag::Owned|DataFlag::ExternallyOwned)) {
        _in = Utility::move(data);
    } else {
        _in = Containers::Array<char>{NoInit, data.size()};
        Utility::copy(data, _in);
    }
}

UnsignedInt TgaImporter::doImage2DCount() const { return 1; }

Containers::Optional<ImageData2D> TgaImporter::doImage2D(UnsignedInt, UnsignedInt) {
    /* Check if the file is long enough */
    if(_in.size() < sizeof(Implementation::TgaHeader)) {
        Error{} << "Trade::TgaImporter::image2D(): file too short, expected at least" << sizeof(Implementation::TgaHeader) << "bytes but got" << _in.size();
        return {};
    }

    const Implementation::TgaHeader& header = *reinterpret_cast<const Implementation::TgaHeader*>(_in.data());

    /* Size in machine endian */
    const Vector2i size{Utility::Endianness::littleEndian(header.width),
                        Utility::Endianness::littleEndian(header.height)};

    /* Image format */
    PixelFormat format;
    if(header.colorMapType != 0) {
        Error() << "Trade::TgaImporter::image2D(): paletted files are not supported";
        return {};
    }

    /* RLE encoding. Reference: http://www.paulbourke.net/dataformats/tga/ */
    const bool rle = (header.imageType & 8);

    /* Color */
    if((header.imageType & ~8) == 2) {
        switch(header.bpp) {
            case 24:
                format = PixelFormat::RGB8Unorm;
                break;
            case 32:
                format = PixelFormat::RGBA8Unorm;
                break;
            default:
                Error() << "Trade::TgaImporter::image2D(): unsupported color bits-per-pixel:" << header.bpp;
                return {};
        }

    /* Grayscale */
    } else if((header.imageType & ~8) == 3) {
        format = PixelFormat::R8Unorm;
        if(header.bpp != 8) {
            Error() << "Trade::TgaImporter::image2D(): unsupported grayscale bits-per-pixel:" << header.bpp;
            return {};
        }

    /* Other? */
    } else {
        Error() << "Trade::TgaImporter::image2D(): unsupported image type:" << header.imageType;
        return {};
    }

    const std::size_t pixelSize = header.bpp/8;
    const std::size_t outputSize = std::size_t(size.product())*pixelSize;

    /* The source pixel data is implicitly the rest of the file. If there's a
       TGA 2 header at the end, ignore the extension and developer areas.
        https://en.wikipedia.org/wiki/Truevision_TGA#File_footer_(optional) */
    Containers::ArrayView<const char> srcPixels = _in.exceptPrefix(sizeof(Implementation::TgaHeader));
    if(Containers::StringView{_in}.hasSuffix("TRUEVISION-XFILE.\0"_s)) {
        if(srcPixels.size() < sizeof(Implementation::TgaFooter)) {
            Error{} << "Trade::TgaImporter::image2D(): TGA 2 file too short, expected at least" << sizeof(Implementation::TgaHeader) + sizeof(Implementation::TgaFooter) << "bytes but got" << _in.size();
            return {};
        }

        const auto& footer = *reinterpret_cast<const Implementation::TgaFooter*>(srcPixels.end() - sizeof(Implementation::TgaFooter));
        const UnsignedInt extensionOffset = Utility::Endianness::littleEndian(footer.extensionOffset);
        const UnsignedInt developerAreaOffset = Utility::Endianness::littleEndian(footer.developerAreaOffset);

        srcPixels = srcPixels.exceptSuffix(sizeof(Implementation::TgaFooter));

        /* If the extension area is present, cut it from the pixel data */
        if(extensionOffset) {
            if(extensionOffset < sizeof(Implementation::TgaHeader)) {
                Error{} << "Trade::TgaImporter::image2D(): TGA 2 extension offset" << extensionOffset << "overlaps with file header";
                return {};
            }
            if(extensionOffset > _in.size() - sizeof(Implementation::TgaFooter)) {
                Error{} << "Trade::TgaImporter::image2D(): TGA 2 extension offset" << extensionOffset << "out of range for" << _in.size() << "bytes and a" << sizeof(Implementation::TgaFooter) << Debug::nospace << "-byte file footer";
                return {};
            }

            srcPixels = srcPixels.prefix(_in.data() + extensionOffset);
        }

        /* If the developer area is present, cut it from the pixel data */
        if(developerAreaOffset) {
            if(developerAreaOffset < sizeof(Implementation::TgaHeader)) {
                Error{} << "Trade::TgaImporter::image2D(): TGA 2 developer area offset" << developerAreaOffset << "overlaps with file header";
                return {};
            }
            if(developerAreaOffset > _in.size() - sizeof(Implementation::TgaFooter)) {
                Error{} << "Trade::TgaImporter::image2D(): TGA 2 developer area offset" << developerAreaOffset << "out of range for" << _in.size() << "bytes and a" << sizeof(Implementation::TgaFooter) << Debug::nospace << "-byte file footer";
                return {};
            }

            if(!extensionOffset)
                srcPixels = srcPixels.prefix(_in.data() + developerAreaOffset);
            else if(developerAreaOffset < extensionOffset) {
                Error{} << "Trade::TgaImporter::image2D(): TGA 2 developer area offset" << developerAreaOffset << "overlaps with extensions at" << extensionOffset << "bytes";
                return {};
            }
        }
    }

    /* Copy data directly if not RLE */
    Containers::Array<char> data{outputSize};
    if(!rle) {
        if(srcPixels.size() < outputSize) {
            Error{} << "Trade::TgaImporter::image2D(): file too short, expected" << outputSize + sizeof(Implementation::TgaHeader) << "bytes but got" << _in.size();
            return {};
        }

        /* Image data that are larger are allowed in this case (even if there's
           a TGA 2 footer after), as we get garbage back in the worst case. In
           case of RLE this would be a failure. */
        if(srcPixels.size() > outputSize && !(flags() & ImporterFlag::Quiet)) {
            Warning{} << "Trade::TgaImporter::image2D(): ignoring" << srcPixels.size() - outputSize << "extra bytes at the end of image data";
        }

        Utility::copy(srcPixels.prefix(data.size()), data);

    /* Otherwise decode */
    } else {
        Containers::ArrayView<char> dstPixels = data;
        while(!srcPixels.isEmpty()) {
            /* Reference: http://www.paulbourke.net/dataformats/tga/ */

            /* 8-bit RLE header. First bit denotes the operation, last 7 bits
               denotes operation count minus 1. */
            const UnsignedByte rleHeader = srcPixels[0];
            const std::size_t count = (rleHeader & ~0x80) + 1;

            /* First bit set to 1 means copying the following pixel given
               number of times, 0 means copying the following number of
               pixels once. We represent that operation with a stride. */
            const std::size_t dataSize = (rleHeader & 0x80 ? 1 : count)*pixelSize;
            const std::ptrdiff_t stride = rleHeader & 0x80 ? 0 : pixelSize;

            /* Check bounds */
            if(1 + dataSize > srcPixels.size()) {
                Error{} << "Trade::TgaImporter::image2D(): RLE file too short at pixel" << (dstPixels.begin() - data.begin())/pixelSize;
                return {};
            }
            if(count*pixelSize > dstPixels.size()) {
                Error{} << "Trade::TgaImporter::image2D(): RLE data at byte" << (srcPixels.data() - _in.data()) << "contains" << count << "pixels but only" << dstPixels.size()/pixelSize << "left to decode";
                return {};
            }

            /* Copy the data */
            Containers::StridedArrayView2D<const char> src{
                srcPixels.slice(1, 1 + dataSize),
                {count, pixelSize}, {stride, 1}};
            Containers::StridedArrayView2D<char> dst{
                dstPixels.prefix(count*pixelSize),
                {count, pixelSize}};
            Utility::copy(src, dst);

            /* Update views for the next round */
            srcPixels = srcPixels.exceptPrefix(1 + dataSize);
            dstPixels = dstPixels.exceptPrefix(count*pixelSize);
        }
    }

    /* Adjust pixel storage if row size is not four byte aligned */
    PixelStorage storage;
    if((size.x()*header.bpp/8)%4 != 0)
        storage.setAlignment(1);

    if(format == PixelFormat::RGB8Unorm) {
        if(flags() & ImporterFlag::Verbose)
            Debug{} << "Trade::TgaImporter::image2D(): converting from BGR to RGB";
        for(Vector3ub& pixel: Containers::arrayCast<Vector3ub>(data))
            pixel = Math::gather<'b', 'g', 'r'>(pixel);
    } else if(format == PixelFormat::RGBA8Unorm) {
        if(flags() & ImporterFlag::Verbose)
            Debug{} << "Trade::TgaImporter::image2D(): converting from BGRA to RGBA";
        for(Vector4ub& pixel: Containers::arrayCast<Vector4ub>(data))
            pixel = Math::gather<'b', 'g', 'r', 'a'>(pixel);
    }

    return ImageData2D{storage, format, size, Utility::move(data)};
}

}}

CORRADE_PLUGIN_REGISTER(TgaImporter, Magnum::Trade::TgaImporter,
    MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE)
