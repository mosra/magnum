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

#include "TgaImporter.h"

#include <fstream>
#include <sstream>
#include <Utility/Endianness.h>
#include <Containers/Array.h>
#include <ImageFormat.h>
#include <Trade/ImageData.h>

#ifdef MAGNUM_TARGET_GLES
#include <algorithm>
#include <Context.h>
#include <Extensions.h>
#include <Swizzle.h>
#endif

#include "TgaHeader.h"

namespace Magnum { namespace Trade {

TgaImporter::TgaImporter(): in(nullptr) {}

TgaImporter::TgaImporter(PluginManager::AbstractManager* manager, std::string plugin): AbstractImporter(manager, std::move(plugin)), in(nullptr) {}

TgaImporter::~TgaImporter() { close(); }

auto TgaImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool TgaImporter::doIsOpened() const { return in; }

void TgaImporter::doOpenData(const Containers::ArrayReference<const unsigned char> data) {
    in = new std::istringstream(std::string(reinterpret_cast<const char*>(data.begin()), data.size()));
}

void TgaImporter::doOpenFile(const std::string& filename) {
    in = new std::ifstream(filename.c_str());
    if(in->good()) return;

    Error() << "Trade::TgaImporter::TgaImporter::openFile(): cannot open file" << filename;
    close();
}

void TgaImporter::doClose() {
    delete in;
    in = nullptr;
}

UnsignedInt TgaImporter::doImage2DCount() const { return 1; }

ImageData2D* TgaImporter::doImage2D(UnsignedInt) {
    /* Check if the file is long enough */
    in->seekg(0, std::istream::end);
    std::streampos filesize = in->tellg();
    in->seekg(0, std::istream::beg);
    if(filesize < std::streampos(sizeof(TgaHeader))) {
        Error() << "Trade::TgaImporter::TgaImporter::image2D(): the file is too short:" << filesize << "bytes";
        return nullptr;
    }

    TgaHeader header;
    in->read(reinterpret_cast<char*>(&header), sizeof(TgaHeader));

    /* Convert to machine endian */
    header.width = Utility::Endianness::littleEndian(header.width);
    header.height = Utility::Endianness::littleEndian(header.height);

    /* Image format */
    ImageFormat format;
    if(header.colorMapType != 0) {
        Error() << "Trade::TgaImporter::TgaImporter::image2D(): paletted files are not supported";
        return nullptr;
    }

    /* Color */
    if(header.imageType == 2) {
        switch(header.bpp) {
            case 24:
                #ifndef MAGNUM_TARGET_GLES
                format = ImageFormat::BGR;
                #else
                format = ImageFormat::RGB;
                #endif
                break;
            case 32:
                #ifndef MAGNUM_TARGET_GLES
                format = ImageFormat::BGRA;
                #else
                format = ImageFormat::RGBA;
                #endif
                break;
            default:
                Error() << "Trade::TgaImporter::TgaImporter::image2D(): unsupported color bits-per-pixel:" << header.bpp;
                return nullptr;
        }

    /* Grayscale */
    } else if(header.imageType == 3) {
        #ifdef MAGNUM_TARGET_GLES
        format = Context::current() && Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>() ?
            ImageFormat::Red : ImageFormat::Luminance;
        #else
        format = ImageFormat::Red;
        #endif
        if(header.bpp != 8) {
            Error() << "Trade::TgaImporter::TgaImporter::image2D(): unsupported grayscale bits-per-pixel:" << header.bpp;
            return nullptr;
        }

    /* Compressed files */
    } else {
        Error() << "Trade::TgaImporter::TgaImporter::image2D(): unsupported (compressed?) image type:" << header.imageType;
        return nullptr;
    }

    const std::size_t dataSize = header.width*header.height*header.bpp/8;
    char* const data = new char[dataSize];
    in->read(data, dataSize);

    Vector2i size(header.width, header.height);

    #ifdef MAGNUM_TARGET_GLES
    if(format == ImageFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data);
        std::transform(pixels, pixels + size.product(), pixels,
            [](Math::Vector3<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r'>(pixel); });
    } else if(format == ImageFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data);
        std::transform(pixels, pixels + size.product(), pixels,
            [](Math::Vector4<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }
    #endif

    return new ImageData2D(format, ImageType::UnsignedByte, size, data);
}

}}
