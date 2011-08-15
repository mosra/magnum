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

#include "TGATexture.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace Magnum::Math;

namespace Magnum { namespace Examples {

TGATexture::TGATexture(const std::string& filename) {
    ifstream image(filename.c_str(), ifstream::binary);
    if(!image.good()) {
        image.close();
        return;
    }

    Header header;
    image.read(reinterpret_cast<char*>(&header), sizeof(Header));

    ColorFormat colorFormat;
    int internalFormat;

    switch(header.bpp) {
        case 24:
            colorFormat = BGR;
            internalFormat = RGB;
            break;
        case 32:
            colorFormat = BGRA;
            internalFormat = RGBA;
            break;
        default:
            cerr << (int) header.bpp << endl;
            return;
    }

    size_t size = header.width*header.height*header.bpp/8;
    GLubyte* buffer = new GLubyte[size];
    image.read(reinterpret_cast<char*>(buffer), size);
    image.close();

    GLsizei dimensions[] = {header.width, header.height};

    setData(0, internalFormat, dimensions, colorFormat, buffer);
    delete[] buffer;
}

}}
