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

#include <TestSuite/Tester.h>
#include <Utility/Endianness.h>

#include "MeshTools/CompressIndices.h"

namespace Magnum { namespace MeshTools { namespace Test {

class CompressIndicesTest: public TestSuite::Tester {
    public:
        CompressIndicesTest();

        void compressChar();
        void compressShort();
        void compressInt();
};

CompressIndicesTest::CompressIndicesTest() {
    addTests({&CompressIndicesTest::compressChar,
              &CompressIndicesTest::compressShort,
              &CompressIndicesTest::compressInt});
}

void CompressIndicesTest::compressChar() {
    std::size_t indexCount;
    Mesh::IndexType indexType;
    char* data;
    std::tie(indexCount, indexType, data) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{1, 2, 3, 0, 4});

    CORRADE_COMPARE(indexCount, 5);
    CORRADE_VERIFY(indexType == Mesh::IndexType::UnsignedByte);
    CORRADE_COMPARE(std::vector<char>(data, data+indexCount*Mesh::indexSize(indexType)),
        (std::vector<char>{ 0x01, 0x02, 0x03, 0x00, 0x04 }));

    delete[] data;
}

void CompressIndicesTest::compressShort() {
    std::size_t indexCount;
    Mesh::IndexType indexType;
    char* data;
    std::tie(indexCount, indexType, data) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{1, 256, 0, 5});

    CORRADE_COMPARE(indexCount, 4);
    CORRADE_VERIFY(indexType == Mesh::IndexType::UnsignedShort);
    if(!Utility::Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data, data+indexCount*Mesh::indexSize(indexType)),
            (std::vector<char>{ 0x01, 0x00,
                           0x00, 0x01,
                           0x00, 0x00,
                           0x05, 0x00 }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data, data+indexCount*Mesh::indexSize(indexType)),
            (std::vector<char>{ 0x00, 0x01,
                           0x01, 0x00,
                           0x00, 0x00,
                           0x00, 0x05 }));
    }

    delete[] data;
}

void CompressIndicesTest::compressInt() {
    std::size_t indexCount;
    Mesh::IndexType indexType;
    char* data;
    std::tie(indexCount, indexType, data) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{65536, 3, 2});

    CORRADE_COMPARE(indexCount, 3);
    CORRADE_VERIFY(indexType == Mesh::IndexType::UnsignedInt);

    if(!Utility::Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data, data+indexCount*Mesh::indexSize(indexType)),
            (std::vector<char>{ 0x00, 0x00, 0x01, 0x00,
                           0x03, 0x00, 0x00, 0x00,
                           0x02, 0x00, 0x00, 0x00 }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data, data+indexCount*Mesh::indexSize(indexType)),
            (std::vector<char>{ 0x00, 0x01, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x03,
                           0x00, 0x00, 0x00, 0x02 }));
    }

    delete[] data;
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompressIndicesTest)
