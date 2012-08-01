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

#include "CompressIndicesTest.h"

#include <QtTest/QTest>
#include <Utility/Endianness.h>

#include "MeshTools/CompressIndices.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::CompressIndicesTest)

using namespace std;
using Corrade::Utility::Endianness;

namespace Magnum { namespace MeshTools { namespace Test {

void CompressIndicesTest::compressChar() {
    size_t indexCount;
    Type indexType;
    char* data;
    tie(indexCount, indexType, data) = MeshTools::compressIndices(
        vector<unsigned int>{1, 2, 3, 0, 4});

    QVERIFY(indexCount == 5);
    QVERIFY(indexType == Type::UnsignedByte);
    QVERIFY((vector<char>(data, data+indexCount*TypeInfo::sizeOf(indexType)) ==
        vector<char>{ 0x01, 0x02, 0x03, 0x00, 0x04 }));

    delete[] data;
}

void CompressIndicesTest::compressShort() {
    size_t indexCount;
    Type indexType;
    char* data;
    tie(indexCount, indexType, data) = MeshTools::compressIndices(
        vector<unsigned int>{1, 256, 0, 5});

    QVERIFY(indexCount == 4);
    QVERIFY(indexType == Type::UnsignedShort);
    if(!Endianness::isBigEndian()) {
        QVERIFY((vector<char>(data, data+indexCount*TypeInfo::sizeOf(indexType)) ==
            vector<char>{ 0x01, 0x00,
                          0x00, 0x01,
                          0x00, 0x00,
                          0x05, 0x00 }));
    } else {
        QVERIFY((vector<char>(data, data+indexCount*TypeInfo::sizeOf(indexType)) ==
            vector<char>{ 0x00, 0x01,
                          0x01, 0x00,
                          0x00, 0x00,
                          0x00, 0x05 }));
    }

    delete[] data;
}

void CompressIndicesTest::compressInt() {
    size_t indexCount;
    Type indexType;
    char* data;
    tie(indexCount, indexType, data) = MeshTools::compressIndices(
        vector<unsigned int>{65536, 3, 2});

    QVERIFY(indexCount == 3);
    QVERIFY(indexType == Type::UnsignedInt);

    if(!Endianness::isBigEndian()) {
        QVERIFY((vector<char>(data, data+indexCount*TypeInfo::sizeOf(indexType)) ==
            vector<char>{ 0x00, 0x00, 0x01, 0x00,
                          0x03, 0x00, 0x00, 0x00,
                          0x02, 0x00, 0x00, 0x00
            }));
    } else {
        QVERIFY((vector<char>(data, data+indexCount*TypeInfo::sizeOf(indexType)) ==
            vector<char>{ 0x00, 0x01, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x03,
                          0x00, 0x00, 0x00, 0x02
            }));
    }

    delete[] data;
}

}}}
