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
#include <Image.h>

#include "TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade { namespace TgaImageConverter {

TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(Corrade::PluginManager::AbstractPluginManager* manager, std::string plugin): AbstractImageConverter(manager, std::move(plugin)) {}

TgaImageConverter::Features TgaImageConverter::features() const {
    return Feature::ConvertToData|Feature::ConvertToFile;
}

std::pair<const unsigned char*, std::size_t> TgaImageConverter::convertToData(const Image2D* const image) const {
    if(image->format() != AbstractImage::Format::BGR &&
       image->format() != AbstractImage::Format::BGRA &&
       image->format() != AbstractImage::Format::Red) {
        Error() << "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image format" << image->format();
        return {nullptr, 0};
    }

    if(image->type() != AbstractImage::Type::UnsignedByte) {
        Error() << "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image type" << image->type();
        return {nullptr, 0};
    }

    /* Initialize data buffer */
    const UnsignedByte pixelSize = Image2D::pixelSize(image->format(), image->type());
    const std::size_t size = sizeof(TgaImporter::TgaHeader) + pixelSize*image->size().product();
    unsigned char* data = new unsigned char[size]();

    /* Fill header */
    auto header = reinterpret_cast<TgaImporter::TgaHeader*>(data);
    header->imageType = image->format() == AbstractImage::Format::Red ? 3 : 2;
    header->bpp = pixelSize*8;
    header->width = image->size().x();
    header->height = image->size().y();

    /* Fill data */
    std::copy(image->data(), image->data()+pixelSize*image->size().product(), data+sizeof(TgaImporter::TgaHeader));

    return {data, size};
}

bool TgaImageConverter::convertToFile(const Image2D* const image, const std::string& filename) const {
    /* Convert the image */
    const unsigned char* data;
    std::size_t size;
    std::tie(data, size) = convertToData(image);
    if(!data) return false;

    /* Open file */
    std::ofstream out(filename.c_str());
    if(!out.good()) {
        delete[] data;
        return false;
    }

    /* Write contents */
    out.write(reinterpret_cast<const char*>(data), size);
    delete[] data;
    return true;
}

}}}
