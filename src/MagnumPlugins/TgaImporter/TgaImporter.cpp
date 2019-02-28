/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade {

TgaImporter::TgaImporter() = default;

TgaImporter::TgaImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

TgaImporter::~TgaImporter() = default;

auto TgaImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool TgaImporter::doIsOpened() const { return _in; }

void TgaImporter::doClose() { _in = nullptr; }

void TgaImporter::doOpenData(const Containers::ArrayView<const char> data) {
    /* Because here we're copying the data and using the _in to check if file
       is opened, having them nullptr would mean openData() would fail without
       any error message. It's not possible to do this check on the importer
       side, because empty file is valid in some formats (OBJ or glTF). We also
       can't do the full import here because then doImage2D() would need to
       copy the imported data instead anyway. This way it'll also work nicely
       with a future openMemory(). */
    if(data.empty()) {
        Error{} << "Trade::TgaImporter::openData(): the file is empty";
        return;
    }

    _in = Containers::Array<char>{data.size()};
    std::copy(data.begin(), data.end(), _in.begin());
}

UnsignedInt TgaImporter::doImage2DCount() const { return 1; }

Containers::Optional<ImageData2D> TgaImporter::doImage2D(UnsignedInt) {
    /* Check if the file is long enough */
    if(_in.size() < std::streamoff(sizeof(Implementation::TgaHeader))) {
        Error() << "Trade::TgaImporter::image2D(): the file is too short:" << _in.size() << "bytes";
        return Containers::NullOpt;
    }

    const Implementation::TgaHeader& header = *reinterpret_cast<const Implementation::TgaHeader*>(_in.data());

    /* Size in machine endian */
    const Vector2i size{Utility::Endianness::littleEndian(header.width),
                        Utility::Endianness::littleEndian(header.height)};

    /* Image format */
    PixelFormat format;
    if(header.colorMapType != 0) {
        Error() << "Trade::TgaImporter::image2D(): paletted files are not supported";
        return Containers::NullOpt;
    }

    /* Color */
    if(header.imageType == 2) {
        switch(header.bpp) {
            case 24:
                format = PixelFormat::RGB8Unorm;
                break;
            case 32:
                format = PixelFormat::RGBA8Unorm;
                break;
            default:
                Error() << "Trade::TgaImporter::image2D(): unsupported color bits-per-pixel:" << header.bpp;
                return Containers::NullOpt;
        }

    /* Grayscale */
    } else if(header.imageType == 3) {
        format = PixelFormat::R8Unorm;
        if(header.bpp != 8) {
            Error() << "Trade::TgaImporter::image2D(): unsupported grayscale bits-per-pixel:" << header.bpp;
            return Containers::NullOpt;
        }

    /* Compressed files */
    } else {
        Error() << "Trade::TgaImporter::image2D(): unsupported (compressed?) image type:" << header.imageType;
        return Containers::NullOpt;
    }

    Containers::Array<char> data{std::size_t(size.product())*header.bpp/8};
    std::copy_n(_in + sizeof(Implementation::TgaHeader), data.size(), data.begin());

    /* Adjust pixel storage if row size is not four byte aligned */
    PixelStorage storage;
    if((size.x()*header.bpp/8)%4 != 0)
        storage.setAlignment(1);

    if(format == PixelFormat::RGB8Unorm) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data.data());
        std::transform(pixels, pixels + size.product(), pixels,
            [](Math::Vector3<UnsignedByte> pixel) { return Math::swizzle<'b', 'g', 'r'>(pixel); });
    } else if(format == PixelFormat::RGBA8Unorm) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data.data());
        std::transform(pixels, pixels + size.product(), pixels,
            [](Math::Vector4<UnsignedByte> pixel) { return Math::swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }

    return ImageData2D{storage, format, size, std::move(data)};
}

}}

CORRADE_PLUGIN_REGISTER(TgaImporter, Magnum::Trade::TgaImporter,
    "cz.mosra.magnum.Trade.AbstractImporter/0.3")
