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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct PackingBatchTest: Corrade::TestSuite::Tester {
    explicit PackingBatchTest();

    void unpackUnsignedByte();
    void unpackUnsignedShort();
    void unpackSignedByte();
    void unpackSignedShort();
    void packUnsignedByte();
    void packUnsignedShort();
    void packSignedByte();
    void packSignedShort();

    void unpackHalf();
    void packHalf();

    template<class T> void castUnsignedFloat();
    template<class T> void castSignedFloat();

    template<class T> void castUnsignedInteger();
    template<class T> void castSignedInteger();

    template<class T> void assertionsPackUnpack();
    void assertionsPackUnpackHalf();
    template<class U, class T> void assertionsCast();
};

PackingBatchTest::PackingBatchTest() {
    addTests({&PackingBatchTest::unpackUnsignedByte,
              &PackingBatchTest::unpackUnsignedShort,
              &PackingBatchTest::unpackSignedByte,
              &PackingBatchTest::unpackSignedShort,
              &PackingBatchTest::packUnsignedByte,
              &PackingBatchTest::packUnsignedShort,
              &PackingBatchTest::packSignedByte,
              &PackingBatchTest::packSignedShort,

              &PackingBatchTest::unpackHalf,
              &PackingBatchTest::packHalf,

              &PackingBatchTest::castUnsignedFloat<UnsignedByte>,
              &PackingBatchTest::castUnsignedFloat<UnsignedShort>,
              &PackingBatchTest::castUnsignedFloat<UnsignedInt>,
              &PackingBatchTest::castSignedFloat<Byte>,
              &PackingBatchTest::castSignedFloat<Short>,
              &PackingBatchTest::castSignedFloat<Int>,

              &PackingBatchTest::castUnsignedInteger<UnsignedByte>,
              &PackingBatchTest::castUnsignedInteger<UnsignedShort>,
              &PackingBatchTest::castSignedInteger<Byte>,
              &PackingBatchTest::castSignedInteger<Short>,

              &PackingBatchTest::assertionsPackUnpack<UnsignedByte>,
              &PackingBatchTest::assertionsPackUnpack<Byte>,
              &PackingBatchTest::assertionsPackUnpack<UnsignedShort>,
              &PackingBatchTest::assertionsPackUnpack<Short>,
              &PackingBatchTest::assertionsPackUnpackHalf,
              &PackingBatchTest::assertionsCast<Float, UnsignedByte>,
              &PackingBatchTest::assertionsCast<Float, Byte>,
              &PackingBatchTest::assertionsCast<Float, UnsignedShort>,
              &PackingBatchTest::assertionsCast<Float, Short>,
              &PackingBatchTest::assertionsCast<Float, UnsignedInt>,
              &PackingBatchTest::assertionsCast<Float, Int>,
              &PackingBatchTest::assertionsCast<UnsignedInt, UnsignedByte>,
              &PackingBatchTest::assertionsCast<UnsignedInt, UnsignedShort>,
              &PackingBatchTest::assertionsCast<Int, Byte>,
              &PackingBatchTest::assertionsCast<Int, Short>});
}

typedef Math::Constants<Float> Constants;
typedef Math::Vector2<UnsignedByte> Vector2ub;
typedef Math::Vector2<UnsignedShort> Vector2us;
typedef Math::Vector2<Byte> Vector2b;
typedef Math::Vector2<Short> Vector2s;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector2<UnsignedInt> Vector2ui;
typedef Math::Vector2<Int> Vector2i;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;

void PackingBatchTest::unpackUnsignedByte() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2ub src;
        Vector2 dst;
    } data[]{
        {{0, 89}, {}},
        {{149, 255}, {}},
        {{0, 255}, {}}
    };

    constexpr Vector2 expected[] {
        {0.0f, 0.34902f},
        {0.584314f, 1.0f},
        {0.0f, 1.0f}
    };

    Corrade::Containers::StridedArrayView1D<Vector2ub> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst, 3, sizeof(Data)};
    unpackInto(Corrade::Containers::arrayCast<2, UnsignedByte>(src),
               Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::unpack<Vector2>(data[i].src), data[i].dst);
}

void PackingBatchTest::unpackUnsignedShort() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2us src;
        Vector2 dst;
    } data[]{
        {{0, 8192}, {}},
        {{49152, 65535}, {}},
        {{0, 65535}, {}}
    };

    constexpr Vector2 expected[] {
        {0.0f, 0.125002f},
        {0.750011f, 1.0f},
        {0.0f, 1.0f}
    };

    Corrade::Containers::StridedArrayView1D<Vector2us> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst, 3, sizeof(Data)};
    unpackInto(Corrade::Containers::arrayCast<2, UnsignedShort>(src),
               Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::unpack<Vector2>(data[i].src), data[i].dst);
}

void PackingBatchTest::unpackSignedByte() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2b src;
        Vector2 dst;
    } data[]{
        {{0, 127}, {}},
        {{37, -72}, {}},
        {{-127, -128}, {}}
    };

    constexpr Vector2 expected[] {
        {0.0f, 1.0f},
        {0.291339f, -0.566929f},
        {-1.0f, -1.0f}
    };

    Corrade::Containers::StridedArrayView1D<Vector2b> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst, 3, sizeof(Data)};
    unpackInto(Corrade::Containers::arrayCast<2, Byte>(src),
               Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::unpack<Vector2>(data[i].src), data[i].dst);
}

void PackingBatchTest::unpackSignedShort() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2s src;
        Vector2 dst;
    } data[]{
        {{0, 16384}, {}},
        {{-16384, 32767}, {}},
        {{-32767, -32768}, {}}
    };

    constexpr Vector2 expected[] {
        {0.0f, 0.500015f},
        {-0.500015f, 1.0f},
        {-1.0f, -1.0f}
    };

    Corrade::Containers::StridedArrayView1D<Vector2s> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst, 3, sizeof(Data)};
    unpackInto(Corrade::Containers::arrayCast<2, Short>(src),
               Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::unpack<Vector2>(data[i].src), data[i].dst);
}

void PackingBatchTest::packUnsignedByte() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2 src;
        Vector2ub dst;
    } data[]{
        {{0.0f, 0.0000001f}, {}},
        {{0.4357f, 0.5f}, {}},
        {{1.0f, 0.9999999f}, {}}
    };

    constexpr Vector2ub expected[] {
        {0, 0},
        {111, 128},
        {255, 255}
    };

    Corrade::Containers::StridedArrayView1D<Vector2> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2ub> dst{data, &data[0].dst, 3, sizeof(Data)};
    packInto(Corrade::Containers::arrayCast<2, Float>(src),
             Corrade::Containers::arrayCast<2, UnsignedByte>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::pack<Vector2ub>(data[i].src), data[i].dst);
}

void PackingBatchTest::packUnsignedShort() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2 src;
        Vector2us dst;
    } data[]{
        {{0.0f, 0.0000001f}, {}},
        {{0.4357f, 0.5f}, {}},
        {{1.0f, 0.9999999f}, {}}
    };

    constexpr Vector2us expected[] {
        {0, 0},
        {28554, 32768},
        {65535, 65535}
    };

    Corrade::Containers::StridedArrayView1D<Vector2> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2us> dst{data, &data[0].dst, 3, sizeof(Data)};
    packInto(Corrade::Containers::arrayCast<2, Float>(src),
             Corrade::Containers::arrayCast<2, UnsignedShort>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::pack<Vector2us>(data[i].src), data[i].dst);
}

void PackingBatchTest::packSignedByte() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2 src;
        Vector2b dst;
    } data[]{
        {{-1.0f, -0.732f}, {}},
        {{0.0f, 0.1357f}, {}},
        {{1.0f, 0.9999999f}, {}}
    };

    constexpr Vector2b expected[] {
        {-127, -93},
        {0, 17},
        {127, 127}
    };

    Corrade::Containers::StridedArrayView1D<Vector2> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2b> dst{data, &data[0].dst, 3, sizeof(Data)};
    packInto(Corrade::Containers::arrayCast<2, Float>(src),
             Corrade::Containers::arrayCast<2, Byte>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::pack<Vector2b>(data[i].src), data[i].dst);
}

void PackingBatchTest::packSignedShort() {
    /* Test data adapted from PackingTest */
    struct Data {
        Vector2 src;
        Vector2s dst;
    } data[]{
        {{-1.0f, -0.33f}, {}},
        {{0.0f, 0.66f}, {}},
        {{1.0f, 0.9999999f}, {}}
    };

    constexpr Vector2s expected[] {
        {-32767, -10813},
        {0, 21626},
        {32767, 32767}
    };

    Corrade::Containers::StridedArrayView1D<Vector2> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2s> dst{data, &data[0].dst, 3, sizeof(Data)};
    packInto(Corrade::Containers::arrayCast<2, Float>(src),
             Corrade::Containers::arrayCast<2, Short>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::pack<Vector2s>(data[i].src), data[i].dst);
}

void PackingBatchTest::unpackHalf() {
    /* Test data adapted from HalfTest */
    struct Data {
        Vector2us src;
        Vector2 dst;
    } data[]{
        {{0, 0x3c00}, {}},
        {{0x4000, 0x4200}, {}},
        {{0x8dc2, 0x57bc}, {}},
        {{0xfc00, 0x7c00}, {}}
    };

    constexpr Vector2 expected[] {
        {0.0f, 1.0f},
        {2.0f, 3.0f},
        {-0.000351f, 123.75f},
        {-Constants::inf(), +Constants::inf()}
    };

    Corrade::Containers::StridedArrayView1D<Vector2us> src{data, &data[0].src,
        Corrade::Containers::arraySize(data), sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst,
        Corrade::Containers::arraySize(data), sizeof(Data)};
    unpackHalfInto(Corrade::Containers::arrayCast<2, UnsignedShort>(src),
                   Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::unpackHalf(data[i].src), data[i].dst);
}

void PackingBatchTest::packHalf() {
    /* Test data adapted from HalfTest */
    struct Data {
        Vector2 src;
        Vector2us dst;
    } data[]{
        {{0.0f, 1.0f}, {}},
        {{2.0f, 3.0f}, {}},
        {{-0.000351512f, 123.75f}, {}},
        {{-Constants::inf(), +Constants::inf()}, {}}
    };

    constexpr Vector2us expected[] {
        {0, 0x3c00},
        {0x4000, 0x4200},
        {0x8dc2, 0x57bc},
        {0xfc00, 0x7c00}
    };

    Corrade::Containers::StridedArrayView1D<Vector2> src{data, &data[0].src,
        Corrade::Containers::arraySize(data), sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2us> dst{data, &data[0].dst,
        Corrade::Containers::arraySize(data), sizeof(Data)};
    packHalfInto(Corrade::Containers::arrayCast<2, Float>(src),
                 Corrade::Containers::arrayCast<2, UnsignedShort>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expected),
        Corrade::TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Corrade::Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::packHalf(data[i].src), data[i].dst);
}

template<class T> void PackingBatchTest::castUnsignedFloat() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    struct Data {
        Math::Vector2<T> src;
        Vector2 dst;
    } data[]{
        {{0, 89}, {}},
        {{149, 22}, {}},
        {{13, 255}, {}}
    };

    /* GCC 4.8 doesn't like constexpr here (cannot initialize aggregate of type
       ‘const Vector2 [3]’ with a compound literal), wtf */
    const Vector2 expectedFloat[] {
        {0.0f, 89.0f},
        {149.0f, 22.0f},
        {13.0f, 255.0f}
    };

    constexpr Math::Vector2<T> expectedInteger[] {
        {0, 89},
        {149, 22},
        {13, 255}
    };

    Corrade::Containers::StridedArrayView1D<Math::Vector2<T>> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst, 3, sizeof(Data)};
    castInto(Corrade::Containers::arrayCast<2, T>(src),
             Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expectedFloat),
        Corrade::TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(Corrade::Containers::arrayCast<2, Float>(dst),
             Corrade::Containers::arrayCast<2, T>(src));
    CORRADE_COMPARE_AS(src, Corrade::Containers::stridedArrayView(expectedInteger),
        Corrade::TestSuite::Compare::Container);
}

template<class T> void PackingBatchTest::castSignedFloat() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    struct Data {
        Math::Vector2<T> src;
        Vector2 dst;
    } data[]{
        {{0, -89}, {}},
        {{-119, 22}, {}},
        {{13, 127}, {}}
    };

    /* GCC 4.8 doesn't like constexpr here (cannot initialize aggregate of type
       ‘const Vector2 [3]’ with a compound literal), wtf */
    const Vector2 expectedFloat[] {
        {0.0f, -89.0f},
        {-119.0f, 22.0f},
        {13.0f, 127.0f}
    };

    constexpr Math::Vector2<T> expectedInteger[] {
        {0, -89},
        {-119, 22},
        {13, 127}
    };

    Corrade::Containers::StridedArrayView1D<Math::Vector2<T>> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2> dst{data, &data[0].dst, 3, sizeof(Data)};
    castInto(Corrade::Containers::arrayCast<2, T>(src),
             Corrade::Containers::arrayCast<2, Float>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expectedFloat),
        Corrade::TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(Corrade::Containers::arrayCast<2, Float>(dst),
             Corrade::Containers::arrayCast<2, T>(src));
    CORRADE_COMPARE_AS(src, Corrade::Containers::stridedArrayView(expectedInteger),
        Corrade::TestSuite::Compare::Container);
}

template<class T> void PackingBatchTest::castUnsignedInteger() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    struct Data {
        Math::Vector2<T> src;
        Vector2ui dst;
    } data[]{
        {{0, 89}, {}},
        {{149, 22}, {}},
        {{13, 255}, {}}
    };

    /* GCC 4.8 doesn't like constexpr here (cannot initialize aggregate of type
       ‘const Vector2 [3]’ with a compound literal), wtf */
    const Vector2ui expectedTargetType[] {
        {0, 89},
        {149, 22},
        {13, 255}
    };

    constexpr Math::Vector2<T> expectedOriginalType[] {
        {0, 89},
        {149, 22},
        {13, 255}
    };

    Corrade::Containers::StridedArrayView1D<Math::Vector2<T>> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2ui> dst{data, &data[0].dst, 3, sizeof(Data)};
    castInto(Corrade::Containers::arrayCast<2, T>(src),
             Corrade::Containers::arrayCast<2, UnsignedInt>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expectedTargetType),
        Corrade::TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(Corrade::Containers::arrayCast<2, UnsignedInt>(dst),
             Corrade::Containers::arrayCast<2, T>(src));
    CORRADE_COMPARE_AS(src, Corrade::Containers::stridedArrayView(expectedOriginalType),
        Corrade::TestSuite::Compare::Container);
}

template<class T> void PackingBatchTest::castSignedInteger() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    struct Data {
        Math::Vector2<T> src;
        Vector2i dst;
    } data[]{
        {{0, -89}, {}},
        {{-119, 22}, {}},
        {{13, 127}, {}}
    };

    /* GCC 4.8 doesn't like constexpr here (cannot initialize aggregate of type
       ‘const Vector2 [3]’ with a compound literal), wtf */
    const Vector2i expectedTargetType[] {
        {0, -89},
        {-119, 22},
        {13, 127}
    };

    constexpr Math::Vector2<T> expectedOriginalType[] {
        {0, -89},
        {-119, 22},
        {13, 127}
    };

    Corrade::Containers::StridedArrayView1D<Math::Vector2<T>> src{data, &data[0].src, 3, sizeof(Data)};
    Corrade::Containers::StridedArrayView1D<Vector2i> dst{data, &data[0].dst, 3, sizeof(Data)};
    castInto(Corrade::Containers::arrayCast<2, T>(src),
             Corrade::Containers::arrayCast<2, Int>(dst));
    CORRADE_COMPARE_AS(dst, Corrade::Containers::stridedArrayView(expectedTargetType),
        Corrade::TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(Corrade::Containers::arrayCast<2, Int>(dst),
             Corrade::Containers::arrayCast<2, T>(src));
    CORRADE_COMPARE_AS(src, Corrade::Containers::stridedArrayView(expectedOriginalType),
        Corrade::TestSuite::Compare::Container);
}

template<class T> void PackingBatchTest::assertionsPackUnpack() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Math::Vector2<T> data[2]{};
    Vector2 resultWrongCount[1]{};
    Vector3 resultWrongVectorSize[2]{};
    Vector4 resultNonContiguous[2]{};

    auto src = Corrade::Containers::arrayCast<2, T>(
        Corrade::Containers::arrayView(data));
    auto dstWrongCount = Corrade::Containers::arrayCast<2, Float>(
        Corrade::Containers::arrayView(resultWrongCount));
    auto dstWrongVectorSize = Corrade::Containers::arrayCast<2, Float>(
        Corrade::Containers::arrayView(resultWrongVectorSize));
    auto dstNotContiguous = Corrade::Containers::arrayCast<2, Float>(
        Corrade::Containers::arrayView(resultNonContiguous)).every({1, 2});

    std::ostringstream out;
    Error redirectError{&out};
    unpackInto(src, dstWrongCount);
    unpackInto(src, dstWrongVectorSize);
    unpackInto(src, dstNotContiguous);
    packInto(dstWrongCount, src);
    packInto(dstWrongVectorSize, src);
    packInto(dstNotContiguous, src);
    CORRADE_COMPARE(out.str(),
        "Math::unpackInto(): wrong destination size, got {1, 2} but expected {2, 2}\n"
        "Math::unpackInto(): wrong destination size, got {2, 3} but expected {2, 2}\n"
        "Math::unpackInto(): second view dimension is not contiguous\n"
        "Math::packInto(): wrong destination size, got {2, 2} but expected {1, 2}\n"
        "Math::packInto(): wrong destination size, got {2, 2} but expected {2, 3}\n"
        "Math::packInto(): second view dimension is not contiguous\n");
}

void PackingBatchTest::assertionsPackUnpackHalf() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2us data[2]{};
    Vector2 resultWrongCount[1]{};
    Vector3 resultWrongVectorSize[2]{};
    Vector4 resultNonContiguous[2]{};

    auto src = Corrade::Containers::arrayCast<2, UnsignedShort>(
        Corrade::Containers::arrayView(data));
    auto dstWrongCount = Corrade::Containers::arrayCast<2, Float>(
        Corrade::Containers::arrayView(resultWrongCount));
    auto dstWrongVectorSize = Corrade::Containers::arrayCast<2, Float>(
        Corrade::Containers::arrayView(resultWrongVectorSize));
    auto dstNotContiguous = Corrade::Containers::arrayCast<2, Float>(
        Corrade::Containers::arrayView(resultNonContiguous)).every({1, 2});

    std::ostringstream out;
    Error redirectError{&out};
    unpackHalfInto(src, dstWrongCount);
    unpackHalfInto(src, dstWrongVectorSize);
    unpackHalfInto(src, dstNotContiguous);
    packHalfInto(dstWrongCount, src);
    packHalfInto(dstWrongVectorSize, src);
    packHalfInto(dstNotContiguous, src);
    CORRADE_COMPARE(out.str(),
        "Math::unpackHalfInto(): wrong destination size, got {1, 2} but expected {2, 2}\n"
        "Math::unpackHalfInto(): wrong destination size, got {2, 3} but expected {2, 2}\n"
        "Math::unpackHalfInto(): second view dimension is not contiguous\n"
        "Math::packHalfInto(): wrong destination size, got {2, 2} but expected {1, 2}\n"
        "Math::packHalfInto(): wrong destination size, got {2, 2} but expected {2, 3}\n"
        "Math::packHalfInto(): second view dimension is not contiguous\n");
}

template<class U, class T> void PackingBatchTest::assertionsCast() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    setTestCaseTemplateName(Corrade::Utility::formatString("{}, {}", TypeTraits<U>::name(), TypeTraits<T>::name()));

    Math::Vector2<T> data[2]{};
    Math::Vector2<U> resultWrongCount[1]{};
    Math::Vector3<U> resultWrongVectorSize[2]{};
    Math::Vector4<U> resultNonContiguous[2]{};

    auto src = Corrade::Containers::arrayCast<2, T>(
        Corrade::Containers::arrayView(data));
    auto dstWrongCount = Corrade::Containers::arrayCast<2, U>(
        Corrade::Containers::arrayView(resultWrongCount));
    auto dstWrongVectorSize = Corrade::Containers::arrayCast<2, U>(
        Corrade::Containers::arrayView(resultWrongVectorSize));
    auto dstNotContiguous = Corrade::Containers::arrayCast<2, U>(
        Corrade::Containers::arrayView(resultNonContiguous)).every({1, 2});

    std::ostringstream out;
    Error redirectError{&out};
    castInto(src, dstWrongCount);
    castInto(src, dstWrongVectorSize);
    castInto(src, dstNotContiguous);
    castInto(dstWrongCount, src);
    castInto(dstWrongVectorSize, src);
    castInto(dstNotContiguous, src);
    CORRADE_COMPARE(out.str(),
        "Math::castInto(): wrong destination size, got {1, 2} but expected {2, 2}\n"
        "Math::castInto(): wrong destination size, got {2, 3} but expected {2, 2}\n"
        "Math::castInto(): second view dimension is not contiguous\n"
        "Math::castInto(): wrong destination size, got {2, 2} but expected {1, 2}\n"
        "Math::castInto(): wrong destination size, got {2, 2} but expected {2, 3}\n"
        "Math::castInto(): second view dimension is not contiguous\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::PackingBatchTest)
