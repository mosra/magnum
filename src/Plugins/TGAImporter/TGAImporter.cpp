/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "TGAImporter.h"

#include "Utility/Endianness.h"
#include "Math/Vector2.h"
#include "Image.h"

using namespace std;
using namespace Corrade::Utility;

PLUGIN_REGISTER(Magnum::Plugins::TGAImporter::TGAImporter, "cz.mosra.magnum.AbstractImporter/0.1")

namespace Magnum { namespace Plugins { namespace TGAImporter {

static_assert(sizeof(TGAImporter::Header) == 18, "TGAImporter: header size is not 18 bytes");

bool TGAImporter::open(std::istream& in) {
    if(_image) close();
    if(!in.good()) return false;

    Header header;
    in.read(reinterpret_cast<char*>(&header), sizeof(Header));

    /* Convert to machine endian */
    header.width = Endianness::littleEndian<unsigned short>(header.width);
    header.height = Endianness::littleEndian<unsigned short>(header.height);

    ColorFormat colorFormat;
    switch(header.bpp) {
        case 24:
            colorFormat = ColorFormat::BGR;
            break;
        case 32:
            colorFormat = ColorFormat::BGRA;
            break;
        default:
            Error() << "TGAImporter: unsupported bits-per-pixel:" << (int) header.bpp;
            return false;
    }

    size_t size = header.width*header.height*header.bpp/8;
    GLubyte* buffer = new GLubyte[size];
    in.read(reinterpret_cast<char*>(buffer), size);

    Math::Vector2<GLsizei> dimensions(header.width, header.height);

    _image = shared_ptr<Image2D>(new Image2D(dimensions, colorFormat, buffer));
    return true;
}

void TGAImporter::close() {
    _image = shared_ptr<Image2D>();
}

shared_ptr<Image2D> TGAImporter::image2D(size_t id) {
    return _image;
}

}}}
