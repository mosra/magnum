/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include <fstream>
#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Image.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

#ifdef MAGNUM_TARGET_GLES
#include <algorithm>
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#endif

namespace Magnum { namespace Trade {

#ifdef MAGNUM_TARGET_GLES
namespace {
    constexpr Math::Vector3<UnsignedByte> bgr(const Math::Vector3<UnsignedByte>& vec) {
        return Math::swizzle<'b', 'g', 'r'>(vec);
    }

    constexpr Math::Vector4<UnsignedByte> bgra(const Math::Vector4<UnsignedByte>& vec) {
        return Math::swizzle<'b', 'g', 'r', 'a'>(vec);
    }
}
#endif

TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(PluginManager::AbstractManager& manager, std::string plugin): AbstractImageConverter(manager, std::move(plugin)) {}

auto TgaImageConverter::doFeatures() const -> Features { return Feature::ConvertData; }

Containers::Array<unsigned char> TgaImageConverter::doExportToData(const ImageReference2D& image) const {
    #ifndef MAGNUM_TARGET_GLES
    if(image.format() != ColorFormat::BGR &&
       image.format() != ColorFormat::BGRA &&
       image.format() != ColorFormat::Red)
    #else
    if(image.format() != ColorFormat::RGB &&
       image.format() != ColorFormat::RGBA &&
       image.format() != ColorFormat::Red)
    #endif
    {
        Error() << "Trade::TgaImageConverter::convertToData(): unsupported image format" << image.format();
        #ifndef CORRADE_GCC45_COMPATIBILITY
        return nullptr;
        #else
        return {};
        #endif
    }

    if(image.type() != ColorType::UnsignedByte) {
        Error() << "Trade::TgaImageConverter::convertToData(): unsupported image type" << image.type();
        #ifndef CORRADE_GCC45_COMPATIBILITY
        return nullptr;
        #else
        return {};
        #endif
    }

    /* Initialize data buffer */
    const auto pixelSize = UnsignedByte(image.pixelSize());
    auto data = Containers::Array<unsigned char>::zeroInitialized(sizeof(TgaHeader) + pixelSize*image.size().product());

    /* Fill header */
    auto header = reinterpret_cast<TgaHeader*>(data.begin());
    header->imageType = image.format() == ColorFormat::Red ? 3 : 2;
    header->bpp = pixelSize*8;
    header->width = UnsignedShort(Utility::Endianness::littleEndian(image.size().x()));
    header->height = UnsignedShort(Utility::Endianness::littleEndian(image.size().y()));

    /* Fill data */
    std::copy(image.data(), image.data()+pixelSize*image.size().product(), data.begin()+sizeof(TgaHeader));

    #ifdef MAGNUM_TARGET_GLES
    if(image.format() == ColorFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data.begin()+sizeof(TgaHeader));
        std::transform(pixels, pixels + image.size().product(), pixels, bgr);
    } else if(image.format() == ColorFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data.begin()+sizeof(TgaHeader));
        std::transform(pixels, pixels + image.size().product(), pixels, bgra);
    }
    #endif

    return std::move(data);
}

}}
