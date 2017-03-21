/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <algorithm>
#include <fstream>
#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade {

TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImageConverter{manager, plugin} {}

auto TgaImageConverter::doFeatures() const -> Features { return Feature::ConvertData; }

Containers::Array<char> TgaImageConverter::doExportToData(const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES
    if(image.storage().swapBytes()) {
        Error() << "Trade::TgaImageConverter::exportToData(): pixel byte swap is not supported";
        return nullptr;
    }
    #endif

    if(image.format() != PixelFormat::RGB &&
       image.format() != PixelFormat::RGBA
       #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
       && image.format() != PixelFormat::Red
       #endif
       #ifdef MAGNUM_TARGET_GLES2
       && image.format() != PixelFormat::Luminance
       #endif
       )
    {
        Error() << "Trade::TgaImageConverter::exportToData(): unsupported color format" << image.format();
        return nullptr;
    }

    if(image.type() != PixelType::UnsignedByte) {
        Error() << "Trade::TgaImageConverter::exportToData(): unsupported color type" << image.type();
        return nullptr;
    }

    /* Initialize data buffer */
    const auto pixelSize = UnsignedByte(image.pixelSize());
    Containers::Array<char> data{Containers::ValueInit, sizeof(TgaHeader) + pixelSize*image.size().product()};

    /* Fill header */
    auto header = reinterpret_cast<TgaHeader*>(data.begin());
    switch(image.format()) {
        case PixelFormat::RGB:
        case PixelFormat::RGBA:
            header->imageType = 2;
            break;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::Red:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::Luminance:
        #endif
            header->imageType = 3;
            break;
        default: CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    header->bpp = pixelSize*8;
    header->width = UnsignedShort(Utility::Endianness::littleEndian(image.size().x()));
    header->height = UnsignedShort(Utility::Endianness::littleEndian(image.size().y()));

    /* Image data pointer including skip */
    const char* imageData = image.data() + std::get<0>(image.dataProperties()).sum();

    /* Fill data or copy them row by row if we need to drop the padding */
    const std::size_t rowSize = image.size().x()*pixelSize;
    const std::size_t rowStride = std::get<1>(image.dataProperties()).x();
    if(rowStride != rowSize) {
        for(std::int_fast32_t y = 0; y != image.size().y(); ++y)
            std::copy_n(imageData + y*rowStride, rowSize, data.begin() + sizeof(TgaHeader) + y*rowSize);
    } else std::copy_n(imageData, pixelSize*image.size().product(), data.begin() + sizeof(TgaHeader));

    if(image.format() == PixelFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data.begin()+sizeof(TgaHeader));
        std::transform(pixels, pixels + image.size().product(), pixels,
            [](Math::Vector3<UnsignedByte> pixel) { return Math::swizzle<'b', 'g', 'r'>(pixel); });
    } else if(image.format() == PixelFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data.begin()+sizeof(TgaHeader));
        std::transform(pixels, pixels + image.size().product(), pixels,
            [](Math::Vector4<UnsignedByte> pixel) { return Math::swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }

    return data;
}

}}
