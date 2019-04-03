/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/MeshTools/CompressIndices.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CompressIndicesTest: TestSuite::Tester {
    explicit CompressIndicesTest();

    void compressChar();
    void compressShort();
    void compressInt();

    void compressAsShort();
};

CompressIndicesTest::CompressIndicesTest() {
    addTests({&CompressIndicesTest::compressChar,
              &CompressIndicesTest::compressShort,
              &CompressIndicesTest::compressInt,

              &CompressIndicesTest::compressAsShort});
}

void CompressIndicesTest::compressChar() {
    Containers::Array<char> data;
    MeshIndexType type;
    UnsignedInt start, end;
    std::tie(data, type, start, end) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{1, 2, 3, 0, 4});

    CORRADE_COMPARE(start, 0);
    CORRADE_COMPARE(end, 4);
    CORRADE_COMPARE(type, MeshIndexType::UnsignedByte);
    CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()),
        (std::vector<char>{ 0x01, 0x02, 0x03, 0x00, 0x04 }));
}

void CompressIndicesTest::compressShort() {
    Containers::Array<char> data;
    MeshIndexType type;
    UnsignedInt start, end;
    std::tie(data, type, start, end) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{1, 256, 0, 5});

    CORRADE_COMPARE(start, 0);
    CORRADE_COMPARE(end, 256);
    CORRADE_COMPARE(type, MeshIndexType::UnsignedShort);
    if(!Utility::Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()),
            (std::vector<char>{ 0x01, 0x00,
                           0x00, 0x01,
                           0x00, 0x00,
                           0x05, 0x00 }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()),
            (std::vector<char>{ 0x00, 0x01,
                           0x01, 0x00,
                           0x00, 0x00,
                           0x00, 0x05 }));
    }
}

void CompressIndicesTest::compressInt() {
    Containers::Array<char> data;
    MeshIndexType type;
    UnsignedInt start, end;
    std::tie(data, type, start, end) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{65536, 3, 2});

    CORRADE_COMPARE(start, 2);
    CORRADE_COMPARE(end, 65536);
    CORRADE_COMPARE(type, MeshIndexType::UnsignedInt);

    if(!Utility::Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()),
            (std::vector<char>{ 0x00, 0x00, 0x01, 0x00,
                           0x03, 0x00, 0x00, 0x00,
                           0x02, 0x00, 0x00, 0x00 }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()),
            (std::vector<char>{ 0x00, 0x01, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x03,
                           0x00, 0x00, 0x00, 0x02 }));
    }
}

void CompressIndicesTest::compressAsShort() {
    CORRADE_COMPARE_AS(MeshTools::compressIndicesAs<UnsignedShort>({123, 456}),
        (Containers::Array<UnsignedShort>{Containers::InPlaceInit, {123, 456}}),
        TestSuite::Compare::Container);

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compressIndicesAs<UnsignedShort>({65536});
    CORRADE_COMPARE(out.str(), "MeshTools::compressIndicesAs(): type too small to represent value 65536\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompressIndicesTest)
