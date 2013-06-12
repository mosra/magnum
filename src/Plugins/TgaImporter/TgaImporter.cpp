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
#include <algorithm>
#include <Utility/Endianness.h>
#include <Math/Vector2.h>
#include <ImageFormat.h>
#include <Swizzle.h>
#include <Trade/ImageData.h>

#include "TgaHeader.h"

namespace Magnum { namespace Trade { namespace TgaImporter {

TgaImporter::TgaImporter(): in(nullptr) {}

TgaImporter::TgaImporter(PluginManager::AbstractManager* manager, std::string plugin): AbstractImporter(manager, std::move(plugin)), in(nullptr) {}

TgaImporter::~TgaImporter() { close(); }

TgaImporter::Features TgaImporter::features() const {
    return Feature::OpenData|Feature::OpenFile;
}

bool TgaImporter::TgaImporter::openData(const void* const data, const std::size_t size) {
    close();

    in = new std::istringstream(std::string(reinterpret_cast<const char*>(data), size));
    return true;
}

bool TgaImporter::TgaImporter::openFile(const std::string& filename) {
    close();

    in = new std::ifstream(filename.c_str());
    if(in->good()) return true;

    Error() << "Trade::TgaImporter::TgaImporter::openFile(): cannot open file" << filename;
    close();
    return false;
}

void TgaImporter::close() {
    delete in;
    in = nullptr;
}

UnsignedInt TgaImporter::TgaImporter::image2DCount() const {
    return in ? 1 : 0;
}

ImageData2D* TgaImporter::image2D(UnsignedInt id) {
    CORRADE_ASSERT(in, "Trade::TgaImporter::TgaImporter::image2D(): no file opened", nullptr);
    CORRADE_ASSERT(id == 0, "Trade::TgaImporter::TgaImporter::image2D(): wrong image ID", nullptr);

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
        format = ImageFormat::Red;
        if(header.bpp != 8) {
            Error() << "Trade::TgaImporter::TgaImporter::image2D(): unsupported grayscale bits-per-pixel:" << header.bpp;
            return nullptr;
        }

    /* Compressed files */
    } else {
        Error() << "Trade::TgaImporter::TgaImporter::image2D(): unsupported (compressed?) image type:" << header.imageType;
        return nullptr;
    }

    std::size_t size = header.width*header.height*header.bpp/8;
    char* buffer = new char[size];
    in->read(buffer, size);

    Vector2i dimensions(header.width, header.height);

    #ifdef MAGNUM_TARGET_GLES
    if(format == ImageFormat::RGB) {
        auto data = reinterpret_cast<Math::Vector3<UnsignedByte>*>(buffer);
        std::transform(data, data + dimensions.product(), data,
            [](Math::Vector3<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r'>(pixel); });
    } else if(format == ImageFormat::RGBA) {
        auto data = reinterpret_cast<Math::Vector4<UnsignedByte>*>(buffer);
        std::transform(data, data + dimensions.product(), data,
            [](Math::Vector4<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }
    #endif

    return new ImageData2D(dimensions, format, ImageType::UnsignedByte, buffer);
}

}}}
