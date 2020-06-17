/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/TestSuite/Compare/Container.h>
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
    void attributeCountInvalid();

    void stride();
    void strideGaps();
    void interleave();
    void interleaveGaps();
    void interleaveEmpty();

    void interleaveInto();

    void isInterleaved();
    void isInterleavedEmpty();
    void isInterleavedSingleAttribute();
    void isInterleavedGaps();
    void isInterleavedAliased();
    void isInterleavedUnordered();
    void isInterleavedAttributeAcrossStride();
    void isInterleavedVertexDataWholeMemory();
    void isInterleavedImplementationSpecificVertexFormat();

    void interleavedData();
    void interleavedDataArrayAttributes();
    void interleavedDataNoAttributes();
    void interleavedDataNoVertices();
    void interleavedDataNotInterleaved();
    void interleavedDataVertexDataWholeMemory();
    void interleavedMutableDataNotMutable();
    void interleavedDataImplementationSpecificVertexFormat();

    void interleavedLayout();
    void interleavedLayoutExtra();
    void interleavedLayoutExtraAliased();
    void interleavedLayoutExtraTooNegativePadding();
    void interleavedLayoutExtraOnly();
    void interleavedLayoutAlreadyInterleaved();
    void interleavedLayoutAlreadyInterleavedAliased();
    void interleavedLayoutAlreadyInterleavedExtra();
    void interleavedLayoutNothing();
    void interleavedLayoutRvalue();

    void interleaveMeshData();
    void interleaveMeshDataIndexed();
    void interleaveMeshDataExtra();
    void interleaveMeshDataExtraEmpty();
    void interleaveMeshDataExtraOriginalEmpty();
    void interleaveMeshDataExtraWrongCount();
    void interleaveMeshDataExtraOffsetOnly();
    void interleaveMeshDataAlreadyInterleavedMove();
    void interleaveMeshDataAlreadyInterleavedMoveNonOwned();
    void interleaveMeshDataNothing();
};

InterleaveTest::InterleaveTest() {
    addTests({&InterleaveTest::attributeCount,
              &InterleaveTest::attributeCountGaps,
              &InterleaveTest::attributeCountInvalid,
              &InterleaveTest::stride,
              &InterleaveTest::strideGaps,
              &InterleaveTest::interleave,
              &InterleaveTest::interleaveGaps,
              &InterleaveTest::interleaveEmpty,

              &InterleaveTest::interleaveInto,

              &InterleaveTest::isInterleaved,
              &InterleaveTest::isInterleavedEmpty,
              &InterleaveTest::isInterleavedSingleAttribute,
              &InterleaveTest::isInterleavedGaps,
              &InterleaveTest::isInterleavedAliased,
              &InterleaveTest::isInterleavedUnordered,
              &InterleaveTest::isInterleavedAttributeAcrossStride,
              &InterleaveTest::isInterleavedVertexDataWholeMemory,
              &InterleaveTest::isInterleavedImplementationSpecificVertexFormat,

              &InterleaveTest::interleavedData,
              &InterleaveTest::interleavedDataArrayAttributes,
              &InterleaveTest::interleavedDataNoAttributes,
              &InterleaveTest::interleavedDataNoVertices,
              &InterleaveTest::interleavedDataNotInterleaved,
              &InterleaveTest::interleavedDataVertexDataWholeMemory,
              &InterleaveTest::interleavedMutableDataNotMutable,
              &InterleaveTest::interleavedDataImplementationSpecificVertexFormat,

              &InterleaveTest::interleavedLayout,
              &InterleaveTest::interleavedLayoutExtra,
              &InterleaveTest::interleavedLayoutExtraAliased,
              &InterleaveTest::interleavedLayoutExtraTooNegativePadding,
              &InterleaveTest::interleavedLayoutExtraOnly,
              &InterleaveTest::interleavedLayoutAlreadyInterleaved,
              &InterleaveTest::interleavedLayoutAlreadyInterleavedAliased,
              &InterleaveTest::interleavedLayoutAlreadyInterleavedExtra,
              &InterleaveTest::interleavedLayoutNothing,
              &InterleaveTest::interleavedLayoutRvalue,

              &InterleaveTest::interleaveMeshData,
              &InterleaveTest::interleaveMeshDataIndexed,
              &InterleaveTest::interleaveMeshDataExtra,
              &InterleaveTest::interleaveMeshDataExtraEmpty,
              &InterleaveTest::interleaveMeshDataExtraOriginalEmpty,
              &InterleaveTest::interleaveMeshDataExtraWrongCount,
              &InterleaveTest::interleaveMeshDataExtraOffsetOnly,
              &InterleaveTest::interleaveMeshDataAlreadyInterleavedMove,
              &InterleaveTest::interleaveMeshDataAlreadyInterleavedMoveNonOwned,
              &InterleaveTest::interleaveMeshDataNothing});
}

void InterleaveTest::attributeCount() {
    CORRADE_COMPARE((Implementation::AttributeCount{}(std::vector<Byte>{0, 1, 2},
        std::vector<Byte>{3, 4, 5})), std::size_t(3));
}

void InterleaveTest::attributeCountGaps() {
    CORRADE_COMPARE((Implementation::AttributeCount{}(std::vector<Byte>{0, 1, 2}, 3,
        std::vector<Byte>{3, 4, 5}, 5)), std::size_t(3));

    /* No arrays from which to get size */
    CORRADE_COMPARE(Implementation::AttributeCount{}(3, 5), ~std::size_t(0));
}

void InterleaveTest::attributeCountInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream ss;
    Error redirectError{&ss};
    CORRADE_COMPARE((Implementation::AttributeCount{}(std::vector<Byte>{0, 1, 2},
        std::vector<Byte>{0, 1, 2, 3, 4, 5})), std::size_t(0));
    CORRADE_COMPARE(ss.str(), "MeshTools::interleave(): attribute arrays don't have the same length, expected 3 but got 6\n");
}

void InterleaveTest::stride() {
    CORRADE_COMPARE(Implementation::Stride{}(std::vector<Byte>()), std::size_t(1));
    CORRADE_COMPARE(Implementation::Stride{}(std::vector<Int>()), std::size_t(4));
    CORRADE_COMPARE((Implementation::Stride{}(std::vector<Byte>(), std::vector<Int>())), std::size_t(5));
}

void InterleaveTest::strideGaps() {
    CORRADE_COMPARE((Implementation::Stride{}(2, std::vector<Byte>(), 1, std::vector<Int>(), 12)), std::size_t(20));
}

void InterleaveTest::interleave() {
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

void InterleaveTest::interleaveGaps() {
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

void InterleaveTest::interleaveEmpty() {
    const Containers::Array<char> data = MeshTools::interleave(std::vector<Byte>{}, 2);
    CORRADE_COMPARE(data.size(), 0);
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

void InterleaveTest::isInterleavedVertexDataWholeMemory() {
    struct Vertex {
        Vector2 position;
        Vector3 normal;
    } vertexData[3];
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            &vertexData[0].position, 3, sizeof(Vertex)}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            &vertexData[0].normal, 3, sizeof(Vertex)}};

    /* This is used internally by combineFaceAttributes(), as long as the
       vertex data array isn't accessed directly it's okay */
    Trade::MeshData data{MeshPrimitive::Triangles,
        {}, {nullptr, ~std::size_t{}}, {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
}

void InterleaveTest::isInterleavedImplementationSpecificVertexFormat() {
    /* Interleaved; fits into one byte at the end of stride */
    {
        Containers::Array<char> vertexData{100 + 3*9};
        Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
            Containers::StridedArrayView1D<Vector2>{vertexData,
                reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 9}};
        Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
            vertexFormatWrap(0x1234),
            Containers::StridedArrayView1D<char>{vertexData,
                vertexData.data() + 100 + 8, 3, 9}};

        /* The result should be independent on the order of calculations */
        Trade::MeshData data{MeshPrimitive::Triangles, {}, vertexData,
            {positions, normals}};
        Trade::MeshData dataDifferentOrder{MeshPrimitive::Triangles, {}, vertexData,
            {normals, positions}};
        CORRADE_VERIFY(MeshTools::isInterleaved(data));
        CORRADE_VERIFY(MeshTools::isInterleaved(dataDifferentOrder));
    }

    /* Doesn't have even one byte of space in the stride, invalid */
    {
        Containers::Array<char> vertexData{100 + 3*8};
        Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
            Containers::StridedArrayView1D<Vector2>{vertexData,
                reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 8}};
        Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
            vertexFormatWrap(0x1234),
            Containers::StridedArrayView1D<char>{vertexData,
                vertexData.data() + 100 + 8, 3, 8}};

        Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions, normals}};
        CORRADE_VERIFY(!MeshTools::isInterleaved(data));
    }

    /* A non-interleaved (or not?) attribute with a implementation-specific
       format after interleaved ones is also invalid */
    {
        Containers::Array<char> vertexData{100 + 3*20 + 3};
        Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
            Containers::StridedArrayView1D<Vector2>{vertexData,
                reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 20}};
        Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
            Containers::StridedArrayView1D<Vector3>{vertexData,
                reinterpret_cast<Vector3*>(vertexData.data() + 100 + 8), 3, 20}};
        Trade::MeshAttributeData extra{Trade::meshAttributeCustom(1234),
            vertexFormatWrap(0x1234),
            Containers::StridedArrayView1D<char>{vertexData,
                vertexData.data() + 100 + 3*20, 3, 1}};

        Trade::MeshData data{MeshPrimitive::Triangles, {}, vertexData,
            {positions, normals, extra}};
        CORRADE_VERIFY(!MeshTools::isInterleaved(data));
    }
}

void InterleaveTest::interleavedData() {
    Containers::Array<char> vertexData{100 + 3*40};
    Containers::StridedArrayView1D<Vector3> normals{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + 100 + 24), 3, 40};
    Containers::StridedArrayView1D<Vector2> positions{vertexData,
        reinterpret_cast<Vector2*>(vertexData.data() + 100 + 5), 3, 40};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normals},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions}
    }};

    CORRADE_VERIFY(MeshTools::isInterleaved(data));
    Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
    CORRADE_COMPARE(interleaved.data(), positions.data());
    CORRADE_COMPARE(interleaved.size()[0], 3);
    CORRADE_COMPARE(interleaved.size()[1], 31);
    CORRADE_COMPARE(interleaved.stride()[0], 40);
    CORRADE_COMPARE(interleaved.stride()[1], 1);

    Containers::StridedArrayView2D<char> interleavedMutable = MeshTools::interleavedMutableData(data);
    CORRADE_COMPARE(interleavedMutable.data(), positions.data());
    CORRADE_COMPARE(interleavedMutable.size()[0], 3);
    CORRADE_COMPARE(interleavedMutable.size()[1], 31);
    CORRADE_COMPARE(interleavedMutable.stride()[0], 40);
    CORRADE_COMPARE(interleavedMutable.stride()[1], 1);
}

void InterleaveTest::interleavedDataArrayAttributes() {
    /* Same as above, except that the MeshData get those as custom Float array
       attribs of size 3 and 2 instead of Vector3 and Vector2. Output should be
       the same for both. */
    Containers::Array<char> vertexData{100 + 3*40};
    Containers::StridedArrayView1D<Vector3> normals{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + 100 + 24), 3, 40};
    Containers::StridedArrayView1D<Vector2> positions{vertexData,
        reinterpret_cast<Vector2*>(vertexData.data() + 100 + 5), 3, 40};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Float, normals, 3},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(43),
            VertexFormat::Float, positions, 2}
    }};

    CORRADE_VERIFY(MeshTools::isInterleaved(data));
    Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
    CORRADE_COMPARE(interleaved.data(), positions.data());
    CORRADE_COMPARE(interleaved.size()[0], 3);
    CORRADE_COMPARE(interleaved.size()[1], 31);
    CORRADE_COMPARE(interleaved.stride()[0], 40);
    CORRADE_COMPARE(interleaved.stride()[1], 1);

    Containers::StridedArrayView2D<char> interleavedMutable = MeshTools::interleavedMutableData(data);
    CORRADE_COMPARE(interleavedMutable.data(), positions.data());
    CORRADE_COMPARE(interleavedMutable.size()[0], 3);
    CORRADE_COMPARE(interleavedMutable.size()[1], 31);
    CORRADE_COMPARE(interleavedMutable.stride()[0], 40);
    CORRADE_COMPARE(interleavedMutable.stride()[1], 1);
}

void InterleaveTest::interleavedDataNoAttributes() {
    char a[1];
    Trade::MeshData data{MeshPrimitive::Lines, {}, a, {}, 15};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));
    Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
    CORRADE_COMPARE(interleaved.data(), static_cast<void*>(a));
    CORRADE_COMPARE(interleaved.size()[0], 15);
    CORRADE_COMPARE(interleaved.size()[1], 0);
    CORRADE_COMPARE(interleaved.stride()[0], 0);
    CORRADE_COMPARE(interleaved.stride()[1], 1);
}

void InterleaveTest::interleavedDataNoVertices() {
    struct Vertex {
        Vector3 normal;
        Vector3 position;
    };
    Vertex a[1];
    Trade::MeshData data{MeshPrimitive::Triangles, {}, a, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            Containers::stridedArrayView(a, &a[0].normal, 0, sizeof(Vertex))},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::stridedArrayView(a, &a[0].position, 0, sizeof(Vertex))}
    }};

    CORRADE_VERIFY(MeshTools::isInterleaved(data));
    Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
    CORRADE_COMPARE(interleaved.data(), static_cast<void*>(a));
    CORRADE_COMPARE(interleaved.size()[0], 0);
    CORRADE_COMPARE(interleaved.size()[1], sizeof(Vertex));
    CORRADE_COMPARE(interleaved.stride()[0], sizeof(Vertex));
    CORRADE_COMPARE(interleaved.stride()[1], 1);
}

void InterleaveTest::interleavedDataNotInterleaved() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{100 + 3*20};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::arrayCast<Vector2>(vertexData.suffix(100).prefix(3*8))};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::arrayCast<Vector3>(vertexData.suffix(100).suffix(3*8))};

    Trade::MeshData data{MeshPrimitive::Triangles, std::move(vertexData), {positions, normals}};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::interleavedData(data);
    CORRADE_COMPARE(out.str(), "MeshTools::interleavedData(): the mesh is not interleaved\n");
}

void InterleaveTest::interleavedDataVertexDataWholeMemory() {
    struct Vertex {
        int:32;
        Vector2 position;
        int:32;
        int:32;
        Vector3 normal;
        int:32;
        int:32;
    } vertexData[3];
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            &vertexData[0].position, 3, sizeof(Vertex)}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData,
            &vertexData[0].normal, 3, sizeof(Vertex)}};

    /* This is used internally by combineFaceAttributes(), as long as the
       vertex data array isn't accessed directly it's okay */
    Trade::MeshData data{MeshPrimitive::Triangles,
        {}, {nullptr, ~std::size_t{}}, {normals, positions}};

    CORRADE_VERIFY(MeshTools::isInterleaved(data));
    Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
    CORRADE_COMPARE(interleaved.data(), positions.data().data());
    CORRADE_COMPARE(interleaved.size()[0], 3);
    CORRADE_COMPARE(interleaved.size()[1], 28);
    CORRADE_COMPARE(interleaved.stride()[0], 40);
    CORRADE_COMPARE(interleaved.stride()[1], 1);
}

void InterleaveTest::interleavedMutableDataNotMutable() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char a[1];
    Trade::MeshData data{MeshPrimitive::Lines, {}, a, {}, 15};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::interleavedMutableData(data);
    CORRADE_COMPARE(out.str(), "MeshTools::interleavedMutableData(): vertex data is not mutable\n");
}

void InterleaveTest::interleavedDataImplementationSpecificVertexFormat() {
    Containers::Array<char> vertexData{100 + 3*50};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 50}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        vertexFormatWrap(0x1234),
        Containers::StridedArrayView1D<char>{vertexData,
            vertexData.data() + 100 + 8, 3, 50}};

    {
        Trade::MeshData data{MeshPrimitive::Triangles, {}, vertexData,
            {positions, normals}};
        CORRADE_VERIFY(MeshTools::isInterleaved(data));
        Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
        CORRADE_COMPARE(interleaved.data(), positions.data().data());
        CORRADE_COMPARE(interleaved.size()[0], 3);
        /* The implementation-specific format is conservatively assumed to
           occupy the whole stride (even if may be is excessive) */
        CORRADE_COMPARE(interleaved.size()[1], 50);
        CORRADE_COMPARE(interleaved.stride()[0], 50);
        CORRADE_COMPARE(interleaved.stride()[1], 1);

    /* The result should be the same independent on the order of attributes */
    } {
        Trade::MeshData data{MeshPrimitive::Triangles, {}, vertexData,
            {normals, positions}};
        CORRADE_VERIFY(MeshTools::isInterleaved(data));
        Containers::StridedArrayView2D<const char> interleaved = MeshTools::interleavedData(data);
        CORRADE_COMPARE(interleaved.data(), positions.data().data());
        CORRADE_COMPARE(interleaved.size()[0], 3);
        CORRADE_COMPARE(interleaved.size()[1], 50);
        CORRADE_COMPARE(interleaved.stride()[0], 50);
        CORRADE_COMPARE(interleaved.stride()[1], 1);
    }
}

void InterleaveTest::interleavedLayout() {
    Containers::Array<char> indexData{6};
    Containers::Array<char> vertexData{3*24};

    const Trade::MeshAttributeData attributeData[]{
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayCast<Vector2>(vertexData.prefix(3*8))},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            Containers::arrayCast<Vector3>(vertexData.slice(3*8, 3*20))},
        /* Array attribute to verify it's correctly propagated */
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Short, Containers::StridedArrayView2D<char>{vertexData.suffix(3*20), {3, 4}}, 2}
    };

    Trade::MeshIndexData indices{Containers::arrayCast<UnsignedShort>(indexData)};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        std::move(indexData), indices, std::move(vertexData),
        /* Verify that interleavedLayout() won't attempt to modify the const
           array (see interleavedLayoutRvalue()) */
        Trade::meshAttributeDataNonOwningArray(attributeData)};
    CORRADE_VERIFY(!MeshTools::isInterleaved(data));

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 10);
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!layout.isIndexed()); /* Indices are not preserved */
    CORRADE_COMPARE(layout.attributeCount(), 3);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeName(2), Trade::meshAttributeCustom(42));
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeFormat(2), VertexFormat::Short);
    CORRADE_COMPARE(layout.attributeStride(0), 24);
    CORRADE_COMPARE(layout.attributeStride(1), 24);
    CORRADE_COMPARE(layout.attributeStride(2), 24);
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 8);
    CORRADE_COMPARE(layout.attributeOffset(2), 20);
    CORRADE_COMPARE(layout.vertexCount(), 10);
    /* Needs to be like this so we can modify the data */
    CORRADE_COMPARE(layout.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_VERIFY(layout.vertexData());
    CORRADE_COMPARE(layout.vertexData().size(), 10*24);
}

void InterleaveTest::interleavedLayoutExtra() {
    Containers::Array<char> vertexData{3*20};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::arrayCast<Vector2>(vertexData.prefix(3*8))};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::arrayCast<Vector3>(vertexData.suffix(3*8))};

    Trade::MeshData data{MeshPrimitive::Triangles,
        std::move(vertexData), {positions, normals}};
    CORRADE_VERIFY(!MeshTools::isInterleaved(data));

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 7, {
        Trade::MeshAttributeData{1},
        /* Array attribute to verify it's correctly propagated */
        Trade::MeshAttributeData{Trade::meshAttributeCustom(15),
            VertexFormat::UnsignedByte, nullptr, 6},
        Trade::MeshAttributeData{1},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{4}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.attributeCount(), 4);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeName(2), Trade::meshAttributeCustom(15));
    CORRADE_COMPARE(layout.attributeName(3), Trade::MeshAttribute::Color);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeFormat(2), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(layout.attributeFormat(3), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeStride(0), 44);
    CORRADE_COMPARE(layout.attributeStride(1), 44);
    CORRADE_COMPARE(layout.attributeStride(2), 44);
    CORRADE_COMPARE(layout.attributeStride(3), 44);
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 8);
    CORRADE_COMPARE(layout.attributeOffset(2), 21);
    CORRADE_COMPARE(layout.attributeOffset(3), 28);
    CORRADE_COMPARE(layout.attributeArraySize(0), 0);
    CORRADE_COMPARE(layout.attributeArraySize(1), 0);
    CORRADE_COMPARE(layout.attributeArraySize(2), 6);
    CORRADE_COMPARE(layout.attributeArraySize(3), 0);
    CORRADE_COMPARE(layout.vertexCount(), 7);
    CORRADE_COMPARE(layout.vertexData().size(), 7*44);
}

void InterleaveTest::interleavedLayoutExtraAliased() {
    Containers::Array<char> vertexData{3*12};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData, reinterpret_cast<Vector2*>(vertexData.data()), 3, 12}};
    Trade::MeshData data{MeshPrimitive::Triangles,
        std::move(vertexData), {positions}};

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 100, {
        /* Normals at the same place as positions */
        Trade::MeshAttributeData{-12},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, positions.data()}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.attributeCount(), 2);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeStride(0), 12);
    CORRADE_COMPARE(layout.attributeStride(1), 12);
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 0); /* aliases */
    CORRADE_COMPARE(layout.vertexCount(), 100);
    CORRADE_COMPARE(layout.vertexData().size(), 100*12);
}

void InterleaveTest::interleavedLayoutExtraTooNegativePadding() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*12};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData, reinterpret_cast<Vector2*>(vertexData.data()), 3, 12}};
    Trade::MeshData data{MeshPrimitive::Triangles,
        std::move(vertexData), {positions}};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::interleavedLayout(data, 100, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, positions.data()},
        Trade::MeshAttributeData{-25}
    });
    CORRADE_COMPARE(out.str(), "MeshTools::interleavedLayout(): negative padding -25 in extra attribute 1 too large for stride 24\n");
}

void InterleaveTest::interleavedLayoutExtraOnly() {
    Trade::MeshData data{MeshPrimitive::Triangles, 0};

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 10, {
        Trade::MeshAttributeData{4},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, nullptr}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.attributeCount(), 2);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeStride(0), 24);
    CORRADE_COMPARE(layout.attributeStride(1), 24);
    CORRADE_COMPARE(layout.attributeOffset(0), 4);
    CORRADE_COMPARE(layout.attributeOffset(1), 12);
    CORRADE_COMPARE(layout.vertexCount(), 10);
    CORRADE_COMPARE(layout.vertexData().size(), 10*24);
}

void InterleaveTest::interleavedLayoutAlreadyInterleaved() {
    Containers::Array<char> indexData{6};
    /* Test also removing the initial offset */
    Containers::Array<char> vertexData{100 + 3*24};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData, reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 24}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData, reinterpret_cast<Vector3*>(vertexData.data() + 100 + 10), 3, 24}};

    Trade::MeshIndexData indices{Containers::arrayCast<UnsignedShort>(indexData)};
    Trade::MeshData data{MeshPrimitive::Triangles,
        std::move(indexData), indices,
        std::move(vertexData), {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 10);
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_VERIFY(!layout.isIndexed()); /* Indices are not preserved */
    CORRADE_COMPARE(layout.attributeCount(), 2);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    /* Original stride should be preserved */
    CORRADE_COMPARE(layout.attributeStride(0), 24);
    CORRADE_COMPARE(layout.attributeStride(1), 24);
    /* Relative offsets should be preserved, but the initial one removed */
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 10);
    CORRADE_COMPARE(layout.vertexCount(), 10);
    CORRADE_COMPARE(layout.vertexData().size(), 10*24);
}

void InterleaveTest::interleavedLayoutAlreadyInterleavedAliased() {
    Containers::Array<char> indexData{6};
    Containers::Array<char> vertexData{3*12};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData, reinterpret_cast<Vector2*>(vertexData.data()), 3, 12}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData, reinterpret_cast<Vector3*>(vertexData.data()), 3, 12}};

    Trade::MeshIndexData indices{Containers::arrayCast<UnsignedShort>(indexData)};
    Trade::MeshData data{MeshPrimitive::Triangles,
        std::move(indexData), indices,
        std::move(vertexData), {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 10);
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_VERIFY(!layout.isIndexed()); /* Indices are not preserved */
    CORRADE_COMPARE(layout.attributeCount(), 2);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeStride(0), 12);
    CORRADE_COMPARE(layout.attributeStride(1), 12);
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 0); /* aliases */
    CORRADE_COMPARE(layout.vertexCount(), 10);
    CORRADE_COMPARE(layout.vertexData().size(), 10*12);
}

void InterleaveTest::interleavedLayoutAlreadyInterleavedExtra() {
    Containers::Array<char> vertexData{100 + 3*24};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector2>{vertexData, reinterpret_cast<Vector2*>(vertexData.data() + 100), 3, 24}};
    Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
        Containers::StridedArrayView1D<Vector3>{vertexData, reinterpret_cast<Vector3*>(vertexData.data() + 100 + 10), 3, 24}};

    Trade::MeshData data{MeshPrimitive::Triangles,
        std::move(vertexData), {positions, normals}};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    Trade::MeshData layout = MeshTools::interleavedLayout(data, 10, {
        Trade::MeshAttributeData{1},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(15),
            VertexFormat::UnsignedShort, nullptr},
        Trade::MeshAttributeData{1},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{4}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.attributeCount(), 4);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeName(2), Trade::meshAttributeCustom(15));
    CORRADE_COMPARE(layout.attributeName(3), Trade::MeshAttribute::Color);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeFormat(2), VertexFormat::UnsignedShort);
    CORRADE_COMPARE(layout.attributeFormat(3), VertexFormat::Vector3);
    /* Original stride should be preserved, with stride from extra attribs
       added */
    CORRADE_COMPARE(layout.attributeStride(0), 24 + 20);
    CORRADE_COMPARE(layout.attributeStride(1), 24 + 20);
    CORRADE_COMPARE(layout.attributeStride(2), 24 + 20);
    CORRADE_COMPARE(layout.attributeStride(3), 24 + 20);
    /* Relative offsets should be preserved, but the initial one removed */
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 10);
    CORRADE_COMPARE(layout.attributeOffset(2), 25);
    CORRADE_COMPARE(layout.attributeOffset(3), 28);
    CORRADE_COMPARE(layout.vertexCount(), 10);
    CORRADE_COMPARE(layout.vertexData().size(), 10*44);
}

void InterleaveTest::interleavedLayoutNothing() {
    Trade::MeshData layout = MeshTools::interleavedLayout(Trade::MeshData{MeshPrimitive::Points, 25}, 10);
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.attributeCount(), 0);
    CORRADE_COMPARE(layout.vertexCount(), 10);
    CORRADE_VERIFY(!layout.vertexData());
    CORRADE_COMPARE(layout.vertexData().size(), 0);
}

void InterleaveTest::interleavedLayoutRvalue() {
    Containers::Array<char> indexData{6};
    Containers::Array<char> vertexData{3*20};
    Containers::Array<Trade::MeshAttributeData> attributeData{2};
    attributeData[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::arrayCast<Vector2>(vertexData.prefix(3*8))};
    attributeData[1] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
        Containers::arrayCast<Vector3>(vertexData.suffix(3*8))};
    const void* originalAttributeData = attributeData.data();

    Trade::MeshIndexData indices{Containers::arrayCast<UnsignedShort>(indexData)};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData)};
    CORRADE_VERIFY(!MeshTools::isInterleaved(data));

    /* Check that the attribute data array gets reused when moving a rvalue.
       Explicitly passing an empty init list to verify the rvalue gets
       propagated correctly through all functions. */
    Trade::MeshData layout = MeshTools::interleavedLayout(std::move(data), 10,
        std::initializer_list<Trade::MeshAttributeData>{});
    CORRADE_VERIFY(layout.attributeData().data() == originalAttributeData);

    /* The rest is same as in interleavedLayout() */
    CORRADE_VERIFY(MeshTools::isInterleaved(layout));
    CORRADE_COMPARE(layout.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!layout.isIndexed()); /* Indices are not preserved */
    CORRADE_COMPARE(layout.attributeCount(), 2);
    CORRADE_COMPARE(layout.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(layout.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(layout.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(layout.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(layout.attributeStride(0), 20);
    CORRADE_COMPARE(layout.attributeStride(1), 20);
    CORRADE_COMPARE(layout.attributeOffset(0), 0);
    CORRADE_COMPARE(layout.attributeOffset(1), 8);
    CORRADE_COMPARE(layout.vertexCount(), 10);
    /* Needs to be like this so we can modify the data */
    CORRADE_COMPARE(layout.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_VERIFY(layout.vertexData());
    CORRADE_COMPARE(layout.vertexData().size(), 10*20);
}

void InterleaveTest::interleaveMeshData() {
    struct {
        Vector2 positions[3];
        Vector3 normals[3];
    } vertexData{
        {{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}},
        {Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()}
    };
    Trade::MeshData data{MeshPrimitive::TriangleFan, {},
        Containers::arrayView(&vertexData, sizeof(vertexData)), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertexData.positions)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertexData.normals)}
        }};

    Trade::MeshData interleaved = MeshTools::interleave(data);
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!interleaved.isIndexed());
    /* No reason to not be like this */
    CORRADE_COMPARE(interleaved.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(interleaved.attributeCount(), 2);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::stridedArrayView(vertexData.positions),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector3>(Trade::MeshAttribute::Normal),
        Containers::stridedArrayView(vertexData.normals),
        TestSuite::Compare::Container);
}

void InterleaveTest::interleaveMeshDataIndexed() {
    /* Testing also offset */
    UnsignedShort indexData[50 + 3];
    indexData[50] = 0;
    indexData[51] = 2;
    indexData[52] = 1;
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, Containers::arrayView(indexData), Trade::MeshIndexData{Containers::arrayView(indexData).suffix(50)},
        {}, Containers::arrayView(positions), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};

    Trade::MeshData interleaved = MeshTools::interleave(data);
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(interleaved.isIndexed());
    CORRADE_COMPARE(interleaved.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(interleaved.indexData().size(), 106);
    CORRADE_COMPARE_AS(interleaved.indices<UnsignedShort>(),
        Containers::arrayView(indexData).suffix(50),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(interleaved.attributeCount(), 1);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::stridedArrayView(positions),
        TestSuite::Compare::Container);
}

void InterleaveTest::interleaveMeshDataExtra() {
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, Containers::arrayView(positions), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};

    const Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    Trade::MeshData interleaved = MeshTools::interleave(data, {
        Trade::MeshAttributeData{10},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(normals)}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!interleaved.isIndexed());
    /* No reason to not be like this */
    CORRADE_COMPARE(interleaved.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(interleaved.attributeCount(), 2);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::stridedArrayView(positions),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector3>(Trade::MeshAttribute::Normal),
        Containers::stridedArrayView(normals),
        TestSuite::Compare::Container);
}

void InterleaveTest::interleaveMeshDataExtraEmpty() {
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, Containers::arrayView(positions), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};

    Trade::MeshData interleaved = MeshTools::interleave(data, {
        Trade::MeshAttributeData{4},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!interleaved.isIndexed());
    /* No reason to not be like this */
    CORRADE_COMPARE(interleaved.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(interleaved.attributeCount(), 2);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::stridedArrayView(positions),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(interleaved.attributeStride(Trade::MeshAttribute::Normal), 24);
    CORRADE_COMPARE(interleaved.attributeOffset(Trade::MeshAttribute::Normal), 12);
}

void InterleaveTest::interleaveMeshDataExtraOriginalEmpty() {
    Trade::MeshData data{MeshPrimitive::TriangleFan, 3};

    /* Verify the original vertex count gets passed through */
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData interleaved = MeshTools::interleave(data, {
        Trade::MeshAttributeData{4},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    });

    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!interleaved.isIndexed());
    /* No reason to not be like this */
    CORRADE_COMPARE(interleaved.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(interleaved.attributeCount(), 1);
    CORRADE_COMPARE_AS(interleaved.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::stridedArrayView(positions),
        TestSuite::Compare::Container);
}

void InterleaveTest::interleaveMeshDataExtraWrongCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, Containers::arrayView(positions), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};
    const Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis()};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::interleave(data, {
        Trade::MeshAttributeData{10},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, Containers::arrayView(normals)}
    });
    CORRADE_COMPARE(out.str(), "MeshTools::interleave(): extra attribute 1 expected to have 3 items but got 2\n");
}

void InterleaveTest::interleaveMeshDataExtraOffsetOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData data{MeshPrimitive::TriangleFan, 5};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::interleave(data, {
        Trade::MeshAttributeData{10},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, 3, 5, 14}
    });
    CORRADE_COMPARE(out.str(), "MeshTools::interleave(): extra attribute 1 is offset-only, which is not supported\n");
}

void InterleaveTest::interleaveMeshDataAlreadyInterleavedMove() {
    Containers::Array<char> indexData{4};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*24};
    Containers::StridedArrayView1D<Vector2> positionView{vertexData,
        reinterpret_cast<Vector2*>(vertexData.data()), 3, 24};
    Containers::StridedArrayView1D<Vector3> normalView{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + 10), 3, 24};
    auto attributeData = Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positionView},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normalView}
    });
    const Trade::MeshAttributeData* attributePointer = attributeData;

    Trade::MeshData data{MeshPrimitive::TriangleFan,
        std::move(indexData), Trade::MeshIndexData{indexView},
        std::move(vertexData), std::move(attributeData)};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    /* {} just to cover the initializer_list overload :P */
    Trade::MeshData interleaved = MeshTools::interleave(std::move(data), {});
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.indexCount(), 2);
    CORRADE_COMPARE(interleaved.attributeCount(), 2);
    CORRADE_COMPARE(interleaved.vertexCount(), 3);
    /* Things got just moved without copying */
    CORRADE_VERIFY(interleaved.indexData().data() == static_cast<const void*>(indexView.data()));
    CORRADE_VERIFY(interleaved.attributeData().data() == attributePointer);
    CORRADE_VERIFY(interleaved.vertexData().data() == positionView.data());
}

void InterleaveTest::interleaveMeshDataAlreadyInterleavedMoveNonOwned() {
    Containers::Array<char> indexData{4};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*24};
    Containers::StridedArrayView1D<Vector2> positionView{vertexData,
        reinterpret_cast<Vector2*>(vertexData.data()), 3, 24};
    Containers::StridedArrayView1D<Vector3> normalView{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + 10), 3, 24};
    auto attributeData = Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positionView},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normalView}
    });
    const Trade::MeshAttributeData* attributePointer = attributeData;

    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, indexData, Trade::MeshIndexData{indexView},
        {}, vertexData, std::move(attributeData)};
    CORRADE_VERIFY(MeshTools::isInterleaved(data));

    Trade::MeshData interleaved = MeshTools::interleave(std::move(data));
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.indexCount(), 2);
    CORRADE_COMPARE(interleaved.attributeCount(), 2);
    CORRADE_COMPARE(interleaved.vertexCount(), 3);
    /* The moved data array doesn't own these so things got copied */
    CORRADE_VERIFY(interleaved.indexData().data() != static_cast<const void*>(indexView.data()));
    CORRADE_VERIFY(interleaved.attributeData().data() != attributePointer);
    CORRADE_VERIFY(interleaved.vertexData().data() != positionView.data());
}

void InterleaveTest::interleaveMeshDataNothing() {
    Trade::MeshData interleaved = MeshTools::interleave(Trade::MeshData{MeshPrimitive::Points, 2});
    CORRADE_VERIFY(MeshTools::isInterleaved(interleaved));
    CORRADE_COMPARE(interleaved.attributeCount(), 0);
    CORRADE_COMPARE(interleaved.vertexCount(), 2);
    CORRADE_VERIFY(!interleaved.vertexData());
    CORRADE_COMPARE(interleaved.vertexData().size(), 0);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::InterleaveTest)
