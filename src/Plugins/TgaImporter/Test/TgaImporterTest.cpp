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

#include "TgaImporterTest.h"

#include <Utility/Debug.h>
#include <Math/Vector2.h>
#include <Trade/ImageData.h>

#include "../TgaImporter.h"

using namespace std;
using namespace Corrade::Utility;

CORRADE_TEST_MAIN(Magnum::Trade::TgaImporter::Test::TgaImporterTest)

namespace Magnum { namespace Trade { namespace TgaImporter { namespace Test {

TgaImporterTest::TgaImporterTest() {
    addTests(&TgaImporterTest::openInexistent,
             &TgaImporterTest::openShort,
             &TgaImporterTest::paletted,
             &TgaImporterTest::nonRgb,
             &TgaImporterTest::bits16,
             &TgaImporterTest::bits24,
             &TgaImporterTest::bits32);
}

void TgaImporterTest::openInexistent() {
    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    CORRADE_VERIFY(!importer.open("inexistent.file"));
    CORRADE_COMPARE(debug.str(), "TgaImporter: cannot open file inexistent.file\n");
}

void TgaImporterTest::openShort() {
    const char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    CORRADE_VERIFY(!importer.open(in));
    CORRADE_COMPARE(debug.str(), "TgaImporter: the file is too short: 17 bytes\n");
}

void TgaImporterTest::paletted() {
    const char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    CORRADE_VERIFY(!importer.open(in));
    CORRADE_COMPARE(debug.str(), "TgaImporter: paletted files are not supported\n");
}

void TgaImporterTest::nonRgb() {
    const char data[] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    CORRADE_VERIFY(!importer.open(in));
    CORRADE_COMPARE(debug.str(), "TgaImporter: non-RGB files are not supported\n");
}

void TgaImporterTest::bits16() {
    const char data[] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    CORRADE_VERIFY(!importer.open(in));
    CORRADE_COMPARE(debug.str(), "TgaImporter: unsupported bits-per-pixel: 16\n");
}

void TgaImporterTest::bits24() {
    const char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8
    };
    #ifndef MAGNUM_TARGET_GLES
    const char* pixels = data + 18;
    #else
    const char pixels[] = {
        3, 2, 1, 4, 3, 2, 5, 4, 3, 6, 5, 4, 7, 6, 5, 8, 7, 6
    };
    #endif
    std::istringstream in(string(data, sizeof(data)));

    TgaImporter importer;
    CORRADE_VERIFY(importer.open(in));
    auto image = importer.image2D(0);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_VERIFY(image->components() == AbstractImage::Components::BGR);
    #else
    CORRADE_VERIFY(image->components() == AbstractImage::Components::RGB);
    #endif
    CORRADE_COMPARE(image->size(), Math::Vector2<GLsizei>(2, 3));
    CORRADE_VERIFY(image->type() == TypeTraits<GLubyte>::imageType());
    CORRADE_COMPARE(string(static_cast<const char*>(image->data()), 2*3*3), string(pixels, 2*3*3));
}

void TgaImporterTest::bits32() {
    const char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        1, 2, 3, 1, 2, 3, 4, 1, 3, 4, 5, 1, 4, 5, 6, 1, 5, 6, 7, 1, 6, 7, 8, 1
    };
    #ifndef MAGNUM_TARGET_GLES
    const char* pixels = data + 18;
    #else
    const char pixels[] = {
        3, 2, 1, 1, 4, 3, 2, 1, 5, 4, 3, 1, 6, 5, 4, 1, 7, 6, 5, 1, 8, 7, 6, 1
    };
    #endif
    std::istringstream in(string(data, sizeof(data)));

    TgaImporter importer;
    CORRADE_VERIFY(importer.open(in));
    auto image = importer.image2D(0);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_VERIFY(image->components() == AbstractImage::Components::BGRA);
    #else
    CORRADE_VERIFY(image->components() == AbstractImage::Components::RGBA);
    #endif
    CORRADE_COMPARE(image->size(), Math::Vector2<GLsizei>(2, 3));
    CORRADE_VERIFY(image->type() == TypeTraits<GLubyte>::imageType());
    CORRADE_COMPARE(string(static_cast<const char*>(image->data()), 2*3*3), string(pixels, 2*3*3));
}

}}}}
