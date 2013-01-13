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

#include <sstream>
#include <TestSuite/Tester.h>

#include "Utility/Endianness.h"
#include "Utility/Debug.h"
#include "MeshTools/Interleave.h"

using Corrade::Utility::Endianness;

namespace Magnum { namespace MeshTools { namespace Test {

class InterleaveTest: public Corrade::TestSuite::Tester {
    public:
        InterleaveTest();

        void attributeCount();
        void attributeCountGaps();
        void stride();
        void strideGaps();
        void write();
        void writeGaps();
};

InterleaveTest::InterleaveTest() {
    addTests(&InterleaveTest::attributeCount,
             &InterleaveTest::attributeCountGaps,
             &InterleaveTest::stride,
             &InterleaveTest::strideGaps,
             &InterleaveTest::write,
             &InterleaveTest::writeGaps);
}

void InterleaveTest::attributeCount() {
    std::stringstream ss;
    Error::setOutput(&ss);
    CORRADE_COMPARE((Implementation::Interleave::attributeCount(std::vector<std::int8_t>{0, 1, 2},
        std::vector<std::int8_t>{0, 1, 2, 3, 4, 5})), std::size_t(0));
    CORRADE_COMPARE(ss.str(), "MeshTools::interleave(): attribute arrays don't have the same length, nothing done.\n");

    CORRADE_COMPARE((Implementation::Interleave::attributeCount(std::vector<std::int8_t>{0, 1, 2},
        std::vector<std::int8_t>{3, 4, 5})), std::size_t(3));
}

void InterleaveTest::attributeCountGaps() {
    CORRADE_COMPARE((Implementation::Interleave::attributeCount(std::vector<std::int8_t>{0, 1, 2}, 3,
        std::vector<std::int8_t>{3, 4, 5}, 5)), std::size_t(3));

    /* No arrays from which to get size */
    CORRADE_COMPARE(Implementation::Interleave::attributeCount(3, 5), ~std::size_t(0));
}

void InterleaveTest::stride() {
    CORRADE_COMPARE(Implementation::Interleave::stride(std::vector<std::int8_t>()), std::size_t(1));
    CORRADE_COMPARE(Implementation::Interleave::stride(std::vector<std::int32_t>()), std::size_t(4));
    CORRADE_COMPARE((Implementation::Interleave::stride(std::vector<std::int8_t>(), std::vector<std::int32_t>())), std::size_t(5));
}

void InterleaveTest::strideGaps() {
    CORRADE_COMPARE((Implementation::Interleave::stride(2, std::vector<std::int8_t>(), 1, std::vector<std::int32_t>(), 12)), std::size_t(20));
}

void InterleaveTest::write() {
    std::size_t attributeCount;
    std::size_t stride;
    char* data;
    std::tie(attributeCount, stride, data) = MeshTools::interleave(
        std::vector<std::int8_t>{0, 1, 2},
        std::vector<std::int32_t>{3, 4, 5},
        std::vector<std::int16_t>{6, 7, 8});

    CORRADE_COMPARE(attributeCount, std::size_t(3));
    CORRADE_COMPARE(stride, std::size_t(7));
    std::size_t size = attributeCount*stride;
    if(!Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data, data+size), (std::vector<char>{
            0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00,
            0x01, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00,
            0x02, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00
        }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data, data+size), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08
        }));
    }

    delete[] data;
}

void InterleaveTest::writeGaps() {
    std::size_t attributeCount;
    std::size_t stride;
    char* data;
    std::tie(attributeCount, stride, data) = MeshTools::interleave(
        std::vector<std::int8_t>{0, 1, 2}, 3,
        std::vector<std::int32_t>{3, 4, 5},
        std::vector<std::int16_t>{6, 7, 8}, 2);

    CORRADE_COMPARE(attributeCount, std::size_t(3));
    CORRADE_COMPARE(stride, std::size_t(12));
    std::size_t size = attributeCount*stride;
    if(!Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data, data+size), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
        }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data, data+size), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00
        }));
    }

    delete[] data;
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::InterleaveTest)
