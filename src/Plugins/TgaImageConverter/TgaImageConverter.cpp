/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <fstream>
#include <tuple>
#include <Containers/Array.h>
#include <Utility/Endianness.h>
#include <Image.h>
#include <ImageFormat.h>

#ifdef MAGNUM_TARGET_GLES
#include <algorithm>
#include <Swizzle.h>
#endif

#include "TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade { namespace TgaImageConverter {

TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(PluginManager::AbstractManager* manager, std::string plugin): AbstractImageConverter(manager, std::move(plugin)) {}

auto TgaImageConverter::doFeatures() const -> Features { return Feature::ConvertData; }

Containers::Array<unsigned char> TgaImageConverter::doExportToData(const Image2D* const image) const {
    #ifndef MAGNUM_TARGET_GLES
    if(image->format() != ImageFormat::BGR &&
       image->format() != ImageFormat::BGRA &&
       image->format() != ImageFormat::Red)
    #else
    if(image->format() != ImageFormat::RGB &&
       image->format() != ImageFormat::RGBA &&
       image->format() != ImageFormat::Red)
    #endif
    {
        Error() << "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image format" << image->format();
        return nullptr;
    }

    if(image->type() != ImageType::UnsignedByte) {
        Error() << "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image type" << image->type();
        return nullptr;
    }

    /* Initialize data buffer */
    const UnsignedByte pixelSize = image->pixelSize();
    auto data = Containers::Array<unsigned char>::zeroInitialized(sizeof(TgaImporter::TgaHeader) + pixelSize*image->size().product());

    /* Fill header */
    auto header = reinterpret_cast<TgaImporter::TgaHeader*>(data.begin());
    header->imageType = image->format() == ImageFormat::Red ? 3 : 2;
    header->bpp = pixelSize*8;
    header->width = Utility::Endianness::littleEndian(image->size().x());
    header->height = Utility::Endianness::littleEndian(image->size().y());

    /* Fill data */
    std::copy(image->data(), image->data()+pixelSize*image->size().product(), data.begin()+sizeof(TgaImporter::TgaHeader));

    #ifdef MAGNUM_TARGET_GLES
    if(image->format() == ImageFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data.begin()+sizeof(TgaImporter::TgaHeader));
        std::transform(pixels, pixels + image->size().product(), pixels,
            [](Math::Vector3<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r'>(pixel); });
    } else if(image->format() == ImageFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data.begin()+sizeof(TgaImporter::TgaHeader));
        std::transform(pixels, pixels + image->size().product(), pixels,
            [](Math::Vector4<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }
    #endif

    return std::move(data);
}

}}}
