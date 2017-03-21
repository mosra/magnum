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

#include "TgaImporter.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <Corrade/Utility/Endianness.h>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

#ifdef MAGNUM_TARGET_GLES2
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#endif

namespace Magnum { namespace Trade {

TgaImporter::TgaImporter() = default;

TgaImporter::TgaImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

TgaImporter::~TgaImporter() = default;

auto TgaImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool TgaImporter::doIsOpened() const { return _in; }

void TgaImporter::doClose() { _in = nullptr; }

void TgaImporter::doOpenData(const Containers::ArrayView<const char> data) {
    _in = Containers::Array<char>{data.size()};
    std::copy(data.begin(), data.end(), _in.begin());
}

UnsignedInt TgaImporter::doImage2DCount() const { return 1; }

std::optional<ImageData2D> TgaImporter::doImage2D(UnsignedInt) {
    /* Check if the file is long enough */
    if(_in.size() < std::streamoff(sizeof(TgaHeader))) {
        Error() << "Trade::TgaImporter::image2D(): the file is too short:" << _in.size() << "bytes";
        return std::nullopt;
    }

    const TgaHeader& header = *reinterpret_cast<const TgaHeader*>(_in.data());

    /* Size in machine endian */
    const Vector2i size{Utility::Endianness::littleEndian(header.width),
                        Utility::Endianness::littleEndian(header.height)};

    /* Image format */
    PixelFormat format;
    if(header.colorMapType != 0) {
        Error() << "Trade::TgaImporter::image2D(): paletted files are not supported";
        return std::nullopt;
    }

    /* Color */
    if(header.imageType == 2) {
        switch(header.bpp) {
            case 24:
                format = PixelFormat::RGB;
                break;
            case 32:
                format = PixelFormat::RGBA;
                break;
            default:
                Error() << "Trade::TgaImporter::image2D(): unsupported color bits-per-pixel:" << header.bpp;
                return std::nullopt;
        }

    /* Grayscale */
    } else if(header.imageType == 3) {
        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        format = Context::hasCurrent() && Context::current().isExtensionSupported<Extensions::GL::EXT::texture_rg>() ?
            PixelFormat::Red : PixelFormat::Luminance;
        #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        format = PixelFormat::Red;
        #else
        format = PixelFormat::Luminance;
        #endif
        if(header.bpp != 8) {
            Error() << "Trade::TgaImporter::image2D(): unsupported grayscale bits-per-pixel:" << header.bpp;
            return std::nullopt;
        }

    /* Compressed files */
    } else {
        Error() << "Trade::TgaImporter::image2D(): unsupported (compressed?) image type:" << header.imageType;
        return std::nullopt;
    }

    Containers::Array<char> data{std::size_t(size.product())*header.bpp/8};
    std::copy_n(_in + sizeof(TgaHeader), data.size(), data.begin());

    /* Adjust pixel storage if row size is not four byte aligned */
    PixelStorage storage;
    if((size.x()*header.bpp/8)%4 != 0)
        storage.setAlignment(1);

    if(format == PixelFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data.data());
        std::transform(pixels, pixels + size.product(), pixels,
            [](Math::Vector3<UnsignedByte> pixel) { return Math::swizzle<'b', 'g', 'r'>(pixel); });
    } else if(format == PixelFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data.data());
        std::transform(pixels, pixels + size.product(), pixels,
            [](Math::Vector4<UnsignedByte> pixel) { return Math::swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }

    return ImageData2D{storage, format, PixelType::UnsignedByte, size, std::move(data)};
}

}}
