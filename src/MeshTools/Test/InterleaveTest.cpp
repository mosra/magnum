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

#include "InterleaveTest.h"

#include <sstream>
#include <QtTest/QTest>

#include "Utility/Endianness.h"
#include "Utility/Debug.h"
#include "MeshTools/Interleave.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::InterleaveTest)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace MeshTools { namespace Test {

void InterleaveTest::attributeCount() {
    stringstream ss;
    Error::setOutput(&ss);
    QCOMPARE((Implementation::Interleave::attributeCount(vector<char>{0, 1, 2},
        vector<char>{0, 1, 2, 3, 4, 5})), size_t(0));
    QVERIFY(ss.str() == "MeshTools::interleave(): attribute arrays don't have the same length, nothing done.\n");

    QCOMPARE((Implementation::Interleave::attributeCount(vector<char>{0, 1, 2},
        vector<char>{3, 4, 5})), size_t(3));
}

void InterleaveTest::stride() {
    QCOMPARE(Implementation::Interleave::stride(vector<char>()), size_t(1));
    QCOMPARE(Implementation::Interleave::stride(vector<int>()), size_t(4));
    QCOMPARE((Implementation::Interleave::stride(vector<char>(), vector<int>())), size_t(5));
}

void InterleaveTest::write() {
    size_t attributeCount;
    size_t stride;
    char* data;
    tie(attributeCount, stride, data) = MeshTools::interleave(
        vector<char>{0, 1, 2},
        vector<int>{3, 4, 5},
        vector<short>{6, 7, 8});

    QCOMPARE(attributeCount, size_t(3));
    QCOMPARE(stride, size_t(7));
    size_t size = attributeCount*stride;
    if(!Endianness::isBigEndian()) {
        QVERIFY((vector<char>(data, data+size) == vector<char>{
            0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00,
            0x01, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00,
            0x02, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00
        }));
    } else {
        QVERIFY((vector<char>(data, data+size) == vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08
        }));
    }

    delete[] data;
}

}}}
