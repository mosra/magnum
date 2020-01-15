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
#include <vector>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Endianness.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct InterleaveTest: Corrade::TestSuite::Tester {
    explicit InterleaveTest();

    void attributeCount();
    void attributeCountGaps();
    void stride();
    void strideGaps();
    void write();
    void writeGaps();

    void interleaveInto();

    void isInterleaved();
    void isInterleavedEmpty();
    void isInterleavedSingleAttribute();
    void isInterleavedGaps();
    void isInterleavedAliased();
    void isInterleavedUnordered();
    void isInterleavedAttributeAcrossStride();
};

InterleaveTest::InterleaveTest() {
    addTests({&InterleaveTest::attributeCount,
              &InterleaveTest::attributeCountGaps,
              &InterleaveTest::stride,
              &InterleaveTest::strideGaps,
              &InterleaveTest::write,
              &InterleaveTest::writeGaps,

              &InterleaveTest::interleaveInto,

              &InterleaveTest::isInterleaved,
              &InterleaveTest::isInterleavedEmpty,
              &InterleaveTest::isInterleavedSingleAttribute,
              &InterleaveTest::isInterleavedGaps,
              &InterleaveTest::isInterleavedAliased,
              &InterleaveTest::isInterleavedUnordered,
              &InterleaveTest::isInterleavedAttributeAcrossStride});
}

void InterleaveTest::attributeCount() {
    std::stringstream ss;
    Error redirectError{&ss};
    CORRADE_COMPARE((Implementation::AttributeCount{}(std::vector<Byte>{0, 1, 2},
        std::vector<Byte>{0, 1, 2, 3, 4, 5})), std::size_t(0));
    CORRADE_COMPARE(ss.str(), "MeshTools::interleave(): attribute arrays don't have the same length, expected 3 but got 6\n");

    CORRADE_COMPARE((Implementation::AttributeCount{}(std::vector<Byte>{0, 1, 2},
        std::vector<Byte>{3, 4, 5})), std::size_t(3));
}

void InterleaveTest::attributeCountGaps() {
    CORRADE_COMPARE((Implementation::AttributeCount{}(std::vector<Byte>{0, 1, 2}, 3,
        std::vector<Byte>{3, 4, 5}, 5)), std::size_t(3));

    /* No arrays from which to get size */
    CORRADE_COMPARE(Implementation::AttributeCount{}(3, 5), ~std::size_t(0));
}

void InterleaveTest::stride() {
    CORRADE_COMPARE(Implementation::Stride{}(std::vector<Byte>()), std::size_t(1));
    CORRADE_COMPARE(Implementation::Stride{}(std::vector<Int>()), std::size_t(4));
    CORRADE_COMPARE((Implementation::Stride{}(std::vector<Byte>(), std::vector<Int>())), std::size_t(5));
}

void InterleaveTest::strideGaps() {
    CORRADE_COMPARE((Implementation::Stride{}(2, std::vector<Byte>(), 1, std::vector<Int>(), 12)), std::size_t(20));
}

void InterleaveTest::write() {
    const Containers::Array<char> data = MeshTools::interleave(
        std::vector<Byte>{0, 1, 2},
        std::vector<Int>{3, 4, 5},
        std::vector<Short>{6, 7, 8});

    if(!Utility::Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00,
            0x01, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00,
            0x02, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00
        }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08
        }));
    }
}

void InterleaveTest::writeGaps() {
    const Containers::Array<char> data = MeshTools::interleave(
        std::vector<Byte>{0, 1, 2}, 3,
        std::vector<Int>{3, 4, 5},
        std::vector<Short>{6, 7, 8}, 2);

    if(!Utility::Endianness::isBigEndian()) {
        /*  byte, _____________gap, int___________________, short_____, _______gap */
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
        }));
    } else {
        /*  byte, _____________gap, ___________________int, _____short, _______gap */
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00
        }));
    }
}

void InterleaveTest::interleaveInto() {
    Containers::Array<char> data{Containers::InPlaceInit, {
        0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77,
        0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77,
        0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77,
        0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77, 0x11, 0x33, 0x55, 0x77}};

    MeshTools::interleaveInto(data, 2, std::vector<Int>{4, 5, 6, 7}, 1, std::vector<Short>{0, 1, 2, 3}, 3);

    if(!Utility::Endianness::isBigEndian()) {
        /*  _______gap, int___________________, _gap, short_____, _____________gap */
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x11, 0x33, 0x04, 0x00, 0x00, 0x00, 0x55, 0x00, 0x00, 0x33, 0x55, 0x77,
            0x11, 0x33, 0x05, 0x00, 0x00, 0x00, 0x55, 0x01, 0x00, 0x33, 0x55, 0x77,
            0x11, 0x33, 0x06, 0x00, 0x00, 0x00, 0x55, 0x02, 0x00, 0x33, 0x55, 0x77,
            0x11, 0x33, 0x07, 0x00, 0x00, 0x00, 0x55, 0x03, 0x00, 0x33, 0x55, 0x77
        }));
    } else {
        /*  _______gap, ___________________int, _gap, _____short, _____________gap */
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x11, 0x33, 0x00, 0x00, 0x00, 0x04, 0x55, 0x00, 0x00, 0x33, 0x55, 0x77,
            0x11, 0x33, 0x00, 0x00, 0x00, 0x05, 0x55, 0x00, 0x01, 0x33, 0x55, 0x77,
            0x11, 0x33, 0x00, 0x00, 0x00, 0x06, 0x55, 0x00, 0x02, 0x33, 0x55, 0x77,
            0x11, 0x33, 0x00, 0x00, 0x00, 0x07, 0x55, 0x00, 0x03, 0x33, 0x55, 0x77
        }));
    }
}

void InterleaveTest::isInterleaved() {
    /* Interleaved; testing also initial offset */
    {
        Containers::Array<char> vertexData{100 + 3*20};
        Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
            Containers::StridedArrayView1D<Vector2>{vertexData,
                reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 20}};
        Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
            Containers::StridedArrayView1D<Vector3>{vertexData,
                reinterpret_cast<Vector3*>(vertexData.data() + 100 + 8), 3, 20}};

        Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions, normals}};
        CORRADE_VERIFY(MeshTools::isInterleaved(data));
    }

    /* One after another */
    {
        Containers::Array<char> vertexData{100 + 3*20};
        Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
            Containers::arrayCast<Vector2>(vertexData.suffix(100).prefix(3*8))};
        Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
            Containers::arrayCast<Vector3>(vertexData.suffix(100).suffix(3*8))};

        Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions, normals}};
        CORRADE_VERIFY(!MeshTools::isInterleaved(data));
    }
}

void InterleaveTest::isInterleavedEmpty() {
    Trade::MeshData data{MeshPrimitive::Triangles, 5};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
}

void InterleaveTest::isInterleavedSingleAttribute() {
    Containers::Array<char> vertexData{3*8};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::arrayCast<Vector2>(vertexData.prefix(3*8))};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
}

void InterleaveTest::isInterleavedGaps() {
    Containers::Array<char> vertexData{3*40};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + 5), 3, 40}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            reinterpret_cast<Vector3*>(vertexData.data() + 24), 3, 40}};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
}

void InterleaveTest::isInterleavedAliased() {
    /* Normals share first two components with positions */
    Containers::Array<char> vertexData{3*12};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data()), 3, 12}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            reinterpret_cast<Vector3*>(vertexData.data()), 3, 12}};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
}

void InterleaveTest::isInterleavedUnordered() {
    Containers::Array<char> vertexData{3*12};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data()), 3, 12}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            reinterpret_cast<Vector3*>(vertexData.data()), 3, 12}};

    /* Normals specified first even though they're ordered after positions */
    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {normals, positions}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
}

void InterleaveTest::isInterleavedAttributeAcrossStride() {
    /* Data slightly larger */
    Containers::Array<char> vertexData{5 + 3*30 + 3};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + 5), 3, 30}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            /* 23 + 12 is 35, which still fits into the stride after
               subtracting the initial offset; 24 not */
            reinterpret_cast<Vector3*>(vertexData.data() + 23), 3, 30}};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData),
        {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    vertexData = data.releaseVertexData();
    Trade::MeshAttributeData normals2{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            reinterpret_cast<Vector3*>(vertexData.data() + 24), 3, 30}};
    Trade::MeshData data2{MeshPrimitive::Triangles,
        std::move(vertexData), {positions, normals2}};
    CORRADE_VERIFY(!MeshTools::isInterleaved(data2));
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::InterleaveTest)
