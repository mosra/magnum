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

#include "TgaImageConverter.h"

#include <fstream>
#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade {

TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImageConverter{manager, plugin} {}

ImageConverterFeatures TgaImageConverter::doFeatures() const { return ImageConverterFeature::ConvertData; }

Containers::Array<char> TgaImageConverter::doExportToData(const ImageView2D& image) {
    /* Initialize data buffer */
    const auto pixelSize = UnsignedByte(image.pixelSize());
    Containers::Array<char> data{Containers::ValueInit, sizeof(Implementation::TgaHeader) + pixelSize*image.size().product()};

    /* Fill header */
    auto header = reinterpret_cast<Implementation::TgaHeader*>(data.begin());
    switch(image.format()) {
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
            header->imageType = 2;
            break;
        case PixelFormat::R8Unorm:
            header->imageType = 3;
            break;
        default:
            Error() << "Trade::TgaImageConverter::exportToData(): unsupported pixel format" << image.format();
            return nullptr;
    }
    header->bpp = pixelSize*8;
    header->width = UnsignedShort(Utility::Endianness::littleEndian(image.size().x()));
    header->height = UnsignedShort(Utility::Endianness::littleEndian(image.size().y()));

    /* Copy the pixels into output, dropping padding (if any) */
    const Containers::ArrayView<char> pixels = data.suffix(sizeof(Implementation::TgaHeader));
    Utility::copy(image.pixels(), Containers::StridedArrayView3D<char>{pixels,
        {std::size_t(image.size().y()), std::size_t(image.size().x()), pixelSize}});

    if(image.format() == PixelFormat::RGB8Unorm) {
        if(flags() & ImageConverterFlag::Verbose)
            Debug{} << "Trade::TgaImageConverter::exportToData(): converting from RGB to BGR";
        for(Vector3ub& pixel: Containers::arrayCast<Vector3ub>(pixels))
            pixel = Math::gather<'b', 'g', 'r'>(pixel);
    } else if(image.format() == PixelFormat::RGBA8Unorm) {
        if(flags() & ImageConverterFlag::Verbose)
            Debug{} << "Trade::TgaImageConverter::exportToData(): converting from RGBA to BGRA";
        for(Vector4ub& pixel: Containers::arrayCast<Vector4ub>(pixels))
            pixel = Math::gather<'b', 'g', 'r', 'a'>(pixel);
    }

    return data;
}

}}

CORRADE_PLUGIN_REGISTER(TgaImageConverter, Magnum::Trade::TgaImageConverter,
    "cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1")
