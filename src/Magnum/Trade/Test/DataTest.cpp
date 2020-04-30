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
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Trade/Data.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct DataTest: TestSuite::Tester {
    explicit DataTest();

    void dataChunkHeaderDeserialize();
    void dataChunkHeaderDeserializeInvalid();

    void dataChunkHeaderSerialize();
    void dataChunkHeaderSerializeTooShort();

    void debugDataFlag();
    void debugDataFlags();

    void debugDataChunkType();
    void debugDataChunkSignature();
};

constexpr char Data32[]{
    '\x80', '\x0a', '\x0d', '\x0a', 'B',
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        'l', 'O',
        #else
        'O', 'l',
        #endif
    'B', 0, 0,
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        42, 0,
        #else
        0, 42,
        #endif
    'W', 'a', 'v', 'e',
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    20 + 5, 0, 0, 0,
    #else
    0, 0, 0, 20 + 5,
    #endif

    'h', 'e', 'l', 'l', 'o'
};

constexpr char Data64[]{
    '\x80', '\x0a', '\x0d', '\x0a', 'B',
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        'L', 'O',
        #else
        'O', 'L',
        #endif
    'B', 0, 0,
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        42, 0,
        #else
        0, 42,
        #endif
    'W', 'a', 'v', 'e',
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    24 + 5, 0, 0, 0, 0, 0, 0, 0,
    #else
    0, 0, 0, 0, 0, 0, 0, 24 + 5,
    #endif

    'h', 'e', 'l', 'l', 'o'
};

constexpr Containers::ArrayView<const char> Data = sizeof(void*) == 4 ?
    Containers::arrayView(Data32) : Containers::arrayView(Data64);

const struct {
    const char* name;
    std::size_t size;
    std::size_t offset;
    Containers::Array<char> replace;
    const char* message;
} DataChunkDeserializeInvalidData[] {
    {"too short header",
        sizeof(void*) == 4 ? 19 : 23, 0, {},
        sizeof(void*) == 4 ?
            "expected at least 20 bytes for a header but got 19" :
            "expected at least 24 bytes for a header but got 23"},
    {"too short chunk",
        sizeof(void*) == 4 ? 24 : 28, 0, {},
        sizeof(void*) == 4 ?
            "expected at least 25 bytes but got 24" :
            "expected at least 29 bytes but got 28"},
    {"wrong version",
        0, 0, Containers::array<char>({'\x7f'}),
        "expected version 128 but got 127"},
    {"invalid signature",
        0, 4,
        /* Using the 32-bit signature on 64-bit and vice versa */
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        sizeof(void*) == 4 ?
            Containers::array<char>({'B', 'L', 'O', 'B'}) :
            Containers::array<char>({'B', 'l', 'O', 'B'}),
        sizeof(void*) == 4 ?
            "expected signature Trade::DataChunkSignature('B', 'l', 'O', 'B') but got Trade::DataChunkSignature('B', 'L', 'O', 'B')" :
            "expected signature Trade::DataChunkSignature('B', 'L', 'O', 'B') but got Trade::DataChunkSignature('B', 'l', 'O', 'B')"
        #else
        sizeof(void*) == 4 ?
            Containers::array<char>({'B', 'O', 'L', 'B'}) :
            Containers::array<char>({'B', 'O', 'l', 'B'}),
        sizeof(void*) == 4 ?
            "expected signature Trade::DataChunkSignature('B', 'O', 'l', 'B') but got Trade::DataChunkSignature('B', 'O', 'L', 'B')" :
            "expected signature Trade::DataChunkSignature('B', 'O', 'L', 'B') but got Trade::DataChunkSignature('B', 'O', 'l', 'B')"
        #endif
    },
    {"invalid check bytes",
        0, 8, Containers::array<char>({1, 0}),
        "invalid header check bytes"},
};

constexpr struct {
    const char* name;
    std::size_t size;
} DataChunkSerializeData[] {
    {"no extra data", sizeof(DataChunkHeader)},
    {"1735 bytes extra data", sizeof(DataChunkHeader) + 1735}
};

DataTest::DataTest() {
    addTests({&DataTest::dataChunkHeaderDeserialize});

    addInstancedTests({&DataTest::dataChunkHeaderDeserializeInvalid},
        Containers::arraySize(DataChunkDeserializeInvalidData));

    addInstancedTests({&DataTest::dataChunkHeaderSerialize},
        Containers::arraySize(DataChunkSerializeData));

    addTests({&DataTest::dataChunkHeaderSerializeTooShort});

    addTests({&DataTest::debugDataFlag,
              &DataTest::debugDataFlags,

              &DataTest::debugDataChunkType,
              &DataTest::debugDataChunkSignature});
}

void DataTest::dataChunkHeaderDeserialize() {
    CORRADE_VERIFY(isDataChunk(Data));
    const DataChunkHeader* chunk = Trade::dataChunkHeaderDeserialize(Data);
    CORRADE_VERIFY(chunk);
}

void DataTest::dataChunkHeaderDeserializeInvalid() {
    auto&& data = DataChunkDeserializeInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> blob{Containers::NoInit, Data.size()};
    Utility::copy(Data, blob);

    Containers::ArrayView<char> view = blob;
    if(data.size) view = view.prefix(data.size);
    if(data.replace) Utility::copy(data.replace, view.slice(data.offset, data.offset + data.replace.size()));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!isDataChunk(view));
    CORRADE_VERIFY(!Trade::dataChunkHeaderDeserialize(view));
    CORRADE_COMPARE(out.str(),
        Utility::formatString("Trade::dataChunkHeaderDeserialize(): {}\n", data.message));
}

void DataTest::dataChunkHeaderSerialize() {
    auto&& data = DataChunkSerializeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> out{Containers::NoInit, data.size};
    std::size_t size = dataChunkHeaderSerializeInto(out, DataChunkType(Utility::Endianness::fourCC('r', 't', 'F', 'm')), 0xfeed);
    CORRADE_COMPARE(size, sizeof(DataChunkHeader));
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    if(sizeof(void*) == 4) CORRADE_COMPARE_AS(out.prefix(size),
        Containers::arrayView<char>({
            '\x80', '\x0a', '\x0d', '\x0a', 'B', 'l', 'O', 'B', 0, 0,
            '\xed', '\xfe', 'r', 't', 'F', 'm',
            char(data.size & 0xff), char(data.size >> 8 & 0xff), 0, 0,
        }), TestSuite::Compare::Container);
    else CORRADE_COMPARE_AS(out.prefix(size),
        Containers::arrayView<char>({
            '\x80', '\x0a', '\x0d', '\x0a', 'B', 'L', 'O', 'B', 0, 0,
            '\xed', '\xfe', 'r', 't', 'F', 'm',
            char(data.size & 0xff), char(data.size >> 8 & 0xff), 0, 0, 0, 0, 0, 0
        }), TestSuite::Compare::Container);
    #else
    if(sizeof(void*) == 4) CORRADE_COMPARE_AS(out.prefix(size),
        Containers::arrayView<char>({
            '\x80', '\x0a', '\x0d', '\x0a', 'B', 'O', 'l', 'B', 0, 0,
            '\xed', '\xfe', 'r', 't', 'F', 'm',
            0, 0, char(data.size >> 8 & 0xff), char(data.size & 0xff)
        }), TestSuite::Compare::Container);
    else CORRADE_COMPARE_AS(out.prefix(size),
        Containers::arrayView<char>({
            '\x80', '\x0a', '\x0d', '\x0a', 'B', 'O', 'L', 'B', 0, 0,
            '\xed', '\xfe', 'r', 't', 'F', 'm',
            0, 0, 0, 0, 0, 0, char(data.size >> 8 & 0xff), char(data.size & 0xff)
        }), TestSuite::Compare::Container);
    #endif
}

void DataTest::dataChunkHeaderSerializeTooShort() {
    std::ostringstream out;
    Error redirectError{&out};
    char data[sizeof(DataChunkHeader) - 1];
    dataChunkHeaderSerializeInto(data, DataChunkType{}, 0);
    CORRADE_COMPARE(out.str(), sizeof(void*) == 4 ?
        "Trade::dataChunkHeaderSerializeInto(): data too small, expected at least 20 bytes but got 19\n" :
        "Trade::dataChunkHeaderSerializeInto(): data too small, expected at least 24 bytes but got 23\n");
}

void DataTest::debugDataFlag() {
    std::ostringstream out;

    Debug{&out} << DataFlag::Owned << DataFlag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::DataFlag::Owned Trade::DataFlag(0xf0)\n");
}

void DataTest::debugDataFlags() {
    std::ostringstream out;

    Debug{&out} << (DataFlag::Owned|DataFlag::Mutable) << DataFlags{};
    CORRADE_COMPARE(out.str(), "Trade::DataFlag::Owned|Trade::DataFlag::Mutable Trade::DataFlags{}\n");
}

void DataTest::debugDataChunkType() {
    std::ostringstream out;

    Debug{&out} << DataChunkType(Utility::Endianness::fourCC('M', 's', 'h', '\xab')) << DataChunkType{};
    CORRADE_COMPARE(out.str(), "Trade::DataChunkType('M', 's', 'h', 0xab) Trade::DataChunkType(0x0, 0x0, 0x0, 0x0)\n");
}

void DataTest::debugDataChunkSignature() {
    std::ostringstream out;

    Debug{&out} << DataChunkSignature::Little64 << DataChunkSignature{};
    CORRADE_COMPARE(out.str(), "Trade::DataChunkSignature('B', 'L', 'O', 'B') Trade::DataChunkSignature(0x0, 0x0, 0x0, 0x0)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::DataTest)
