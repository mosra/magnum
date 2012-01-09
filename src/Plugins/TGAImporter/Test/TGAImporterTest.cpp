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

#include "TGAImporterTest.h"

#include <QtTest/QTest>

#include "Utility/Debug.h"
#include "../TGAImporter.h"

using namespace std;
using namespace Corrade::Utility;

QTEST_APPLESS_MAIN(Magnum::Plugins::TGAImporter::Test::TGAImporterTest)

namespace Magnum { namespace Plugins { namespace TGAImporter { namespace Test {

void TGAImporterTest::paletted() {
    const char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TGAImporter importer;
    QVERIFY(!importer.open(in));
    QCOMPARE(debug.str().c_str(), "TGAImporter: paletted files are not supported\n");
}

void TGAImporterTest::nonRgb() {
    const char data[] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TGAImporter importer;
    QVERIFY(!importer.open(in));
    QCOMPARE(debug.str().c_str(), "TGAImporter: non-RGB files are not supported\n");
}

void TGAImporterTest::bits16() {
    const char data[] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0 };
    istringstream in(string(data, sizeof(data)));

    ostringstream debug;
    Error::setOutput(&debug);

    TGAImporter importer;
    QVERIFY(!importer.open(in));
    QCOMPARE(debug.str().c_str(), "TGAImporter: unsupported bits-per-pixel: 16\n");
}

void TGAImporterTest::bits24() {
    const char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8
    };
    std::istringstream in(string(data, sizeof(data)));

    TGAImporter importer;
    QVERIFY(importer.open(in));
    auto image = importer.image2D(0);
    QVERIFY(image->colorFormat() == ColorFormat::BGR);
    QVERIFY(image->dimensions() == Math::Vector2<GLsizei>(2, 3));
    QVERIFY(image->type() == TypeTraits<GLubyte>::glType());
    QVERIFY(string(static_cast<const char*>(image->data())) == string(data + 18, 2*3*3));
}

void TGAImporterTest::bits32() {
    const char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        1, 2, 3, 1, 2, 3, 4, 1, 3, 4, 5, 1, 4, 5, 6, 1, 5, 6, 7, 1, 6, 7, 8, 1
    };
    std::istringstream in(string(data, sizeof(data)));

    TGAImporter importer;
    QVERIFY(importer.open(in));
    auto image = importer.image2D(0);
    QVERIFY(image->colorFormat() == ColorFormat::BGRA);
    QVERIFY(image->dimensions() == Math::Vector2<GLsizei>(2, 3));
    QVERIFY(image->type() == TypeTraits<GLubyte>::glType());
    QVERIFY(string(static_cast<const char*>(image->data()), 2*3*3) == string(data + 18, 2*3*3));
}

}}}}
