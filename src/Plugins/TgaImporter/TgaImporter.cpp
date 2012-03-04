/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "TgaImporter.h"

#include "Utility/Endianness.h"
#include "Math/Vector2.h"
#include "Trade/ImageData.h"

using namespace std;
using namespace Corrade::Utility;

PLUGIN_REGISTER(TgaImporter, Magnum::Trade::TgaImporter::TgaImporter,
                "cz.mosra.magnum.Trade.AbstractImporter/0.1")

namespace Magnum { namespace Trade { namespace TgaImporter {

static_assert(sizeof(TgaImporter::Header) == 18, "TgaImporter: header size is not 18 bytes");

bool TgaImporter::TgaImporter::open(const string& filename) {
    ifstream in(filename.c_str());
    bool status = open(in);
    in.close();
    return status;
}

bool TgaImporter::open(std::istream& in) {
    if(_image) close();
    if(!in.good()) return false;

    Header header;
    in.read(reinterpret_cast<char*>(&header), sizeof(Header));

    /* Convert to machine endian */
    header.width = Endianness::littleEndian<unsigned short>(header.width);
    header.height = Endianness::littleEndian<unsigned short>(header.height);

    if(header.colorMapType != 0) {
        Error() << "TgaImporter: paletted files are not supported";
        return false;
    }

    if(header.imageType != 2) {
        Error() << "TgaImporter: non-RGB files are not supported";
        return false;
    }

    AbstractTexture::ColorFormat colorFormat;
    switch(header.bpp) {
        case 24:
            colorFormat = AbstractTexture::ColorFormat::BGR;
            break;
        case 32:
            colorFormat = AbstractTexture::ColorFormat::BGRA;
            break;
        default:
            Error() << "TgaImporter: unsupported bits-per-pixel:" << (int) header.bpp;
            return false;
    }

    size_t size = header.width*header.height*header.bpp/8;
    GLubyte* buffer = new GLubyte[size];
    in.read(reinterpret_cast<char*>(buffer), size);

    Math::Vector2<GLsizei> dimensions(header.width, header.height);

    _image = new ImageData2D(colorFormat, dimensions, buffer);
    return true;
}

void TgaImporter::close() {
    delete _image;
    _image = nullptr;
}

ImageData2D* TgaImporter::image2D(size_t id) {
    return _image;
}

}}}
