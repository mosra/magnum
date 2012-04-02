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

#include <QtTest/QTest>

#include "Utility/Debug.h"
#include "../TgaImporter.h"

using namespace std;
using namespace Corrade::Utility;

QTEST_APPLESS_MAIN(Magnum::Trade::TgaImporter::Test::TgaImporterTest)

namespace Magnum { namespace Trade { namespace TgaImporter { namespace Test {

void TgaImporterTest::paletted() {
    const char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    QVERIFY(!importer.open(in));
    QCOMPARE(debug.str().c_str(), "TgaImporter: paletted files are not supported\n");
}

void TgaImporterTest::nonRgb() {
    const char data[] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    QVERIFY(!importer.open(in));
    QCOMPARE(debug.str().c_str(), "TgaImporter: non-RGB files are not supported\n");
}

void TgaImporterTest::bits16() {
    const char data[] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    QVERIFY(!importer.open(in));
    QCOMPARE(debug.str().c_str(), "TgaImporter: unsupported bits-per-pixel: 16\n");
}

void TgaImporterTest::bits24() {
    const char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8
    };
    std::istringstream in(string(data, sizeof(data)));

    TgaImporter importer;
    QVERIFY(importer.open(in));
    auto image = importer.image2D(0);
    QVERIFY(image->components() == AbstractImage::Components::BGR);
    QVERIFY(image->dimensions() == Math::Vector2<GLsizei>(2, 3));
    QVERIFY(image->type() == TypeTraits<GLubyte>::imageType());
    QVERIFY(string(static_cast<const char*>(image->data())) == string(data + 18, 2*3*3));
}

void TgaImporterTest::bits32() {
    const char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        1, 2, 3, 1, 2, 3, 4, 1, 3, 4, 5, 1, 4, 5, 6, 1, 5, 6, 7, 1, 6, 7, 8, 1
    };
    std::istringstream in(string(data, sizeof(data)));

    TgaImporter importer;
    QVERIFY(importer.open(in));
    auto image = importer.image2D(0);
    QVERIFY(image->components() == AbstractImage::Components::BGRA);
    QVERIFY(image->dimensions() == Math::Vector2<GLsizei>(2, 3));
    QVERIFY(image->type() == TypeTraits<GLubyte>::imageType());
    QVERIFY(string(static_cast<const char*>(image->data()), 2*3*3) == string(data + 18, 2*3*3));
}

}}}}
