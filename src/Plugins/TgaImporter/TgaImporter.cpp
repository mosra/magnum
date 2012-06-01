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

namespace Magnum { namespace Trade { namespace TgaImporter {

#ifndef DOXYGEN_GENERATING_OUTPUT
static_assert(sizeof(TgaImporter::Header) == 18, "TgaImporter: header size is not 18 bytes");
#endif

bool TgaImporter::TgaImporter::open(const string& filename) {
    ifstream in(filename.c_str());
    if(!in.good()) {
        Error() << "TgaImporter: cannot open file" << filename;
        return false;
    }
    bool status = open(in);
    in.close();
    return status;
}

bool TgaImporter::open(istream& in) {
    if(_image) close();
    if(!in.good()) {
        Error() << "TgaImporter: cannot read input stream";
        return false;
    }

    /* Check if the file is long enough */
    in.seekg(0, istream::end);
    streampos filesize = in.tellg();
    in.seekg(0, istream::beg);
    if(filesize < streampos(sizeof(Header))) {
        Error() << "TgaImporter: the file is too short:" << filesize << "bytes";
        return false;
    }

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

    AbstractImage::Components components;
    switch(header.bpp) {
        case 24:
            components = AbstractImage::Components::BGR;
            break;
        case 32:
            components = AbstractImage::Components::BGRA;
            break;
        default:
            Error() << "TgaImporter: unsupported bits-per-pixel:" << (int) header.bpp;
            return false;
    }

    size_t size = header.width*header.height*header.bpp/8;
    GLubyte* buffer = new GLubyte[size];
    in.read(reinterpret_cast<char*>(buffer), size);

    Math::Vector2<GLsizei> dimensions(header.width, header.height);

    _image = new ImageData2D(dimensions, components, buffer);
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
