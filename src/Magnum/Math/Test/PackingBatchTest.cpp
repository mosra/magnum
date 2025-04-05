/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct PackingBatchTest: TestSuite::Tester {
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

    template<class FloatingPoint, class Integral> void castUnsignedFloatingPoint();
    template<class FloatingPoint, class Integral> void castSignedFloatingPoint();

    template<class T, class U> void castUnsignedInteger();
    template<class T, class U> void castSignedInteger();
    template<class T, class U> void castFloatDouble();

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

              &PackingBatchTest::castUnsignedFloatingPoint<Float, UnsignedByte>,
              &PackingBatchTest::castUnsignedFloatingPoint<Float, UnsignedShort>,
              &PackingBatchTest::castUnsignedFloatingPoint<Float, UnsignedInt>,
              &PackingBatchTest::castUnsignedFloatingPoint<Double, UnsignedByte>,
              &PackingBatchTest::castUnsignedFloatingPoint<Double, UnsignedShort>,
              &PackingBatchTest::castUnsignedFloatingPoint<Double, UnsignedInt>,
              &PackingBatchTest::castSignedFloatingPoint<Float, Byte>,
              &PackingBatchTest::castSignedFloatingPoint<Float, Short>,
              &PackingBatchTest::castSignedFloatingPoint<Float, Int>,
              &PackingBatchTest::castSignedFloatingPoint<Double, Byte>,
              &PackingBatchTest::castSignedFloatingPoint<Double, Short>,
              &PackingBatchTest::castSignedFloatingPoint<Double, Int>,

              &PackingBatchTest::castUnsignedInteger<UnsignedByte, UnsignedByte>,
              &PackingBatchTest::castUnsignedInteger<UnsignedByte, UnsignedShort>,
              &PackingBatchTest::castUnsignedInteger<UnsignedByte, UnsignedInt>,
              &PackingBatchTest::castUnsignedInteger<UnsignedShort, UnsignedShort>,
              &PackingBatchTest::castUnsignedInteger<UnsignedShort, UnsignedInt>,
              &PackingBatchTest::castUnsignedInteger<UnsignedInt, UnsignedInt>,
              &PackingBatchTest::castUnsignedInteger<UnsignedByte, UnsignedLong>,
              &PackingBatchTest::castUnsignedInteger<UnsignedShort, UnsignedLong>,
              &PackingBatchTest::castUnsignedInteger<UnsignedInt, UnsignedLong>,
              &PackingBatchTest::castUnsignedInteger<UnsignedLong, UnsignedLong>,
              &PackingBatchTest::castSignedInteger<Byte, Byte>,
              &PackingBatchTest::castSignedInteger<Byte, Short>,
              &PackingBatchTest::castSignedInteger<Byte, Int>,
              &PackingBatchTest::castSignedInteger<Short, Short>,
              &PackingBatchTest::castSignedInteger<Short, Int>,
              &PackingBatchTest::castSignedInteger<Int, Int>,
              &PackingBatchTest::castSignedInteger<Byte, Long>,
              &PackingBatchTest::castSignedInteger<Short, Long>,
              &PackingBatchTest::castSignedInteger<Int, Long>,
              &PackingBatchTest::castSignedInteger<Long, Long>,
              &PackingBatchTest::castFloatDouble<Float, Double>,
              &PackingBatchTest::castFloatDouble<Float, Float>,
              &PackingBatchTest::castFloatDouble<Double, Double>,

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
              &PackingBatchTest::assertionsCast<Double, UnsignedByte>,
              &PackingBatchTest::assertionsCast<Double, Byte>,
              &PackingBatchTest::assertionsCast<Double, UnsignedShort>,
              &PackingBatchTest::assertionsCast<Double, Short>,
              &PackingBatchTest::assertionsCast<Double, UnsignedInt>,
              &PackingBatchTest::assertionsCast<Double, Int>,
              &PackingBatchTest::assertionsCast<UnsignedByte, UnsignedByte>,
              &PackingBatchTest::assertionsCast<UnsignedShort, UnsignedByte>,
              &PackingBatchTest::assertionsCast<UnsignedShort, UnsignedShort>,
              &PackingBatchTest::assertionsCast<UnsignedInt, UnsignedByte>,
              &PackingBatchTest::assertionsCast<UnsignedInt, UnsignedShort>,
              &PackingBatchTest::assertionsCast<UnsignedInt, UnsignedInt>,
              &PackingBatchTest::assertionsCast<UnsignedLong, UnsignedByte>,
              &PackingBatchTest::assertionsCast<UnsignedLong, UnsignedShort>,
              &PackingBatchTest::assertionsCast<UnsignedLong, UnsignedInt>,
              &PackingBatchTest::assertionsCast<UnsignedLong, UnsignedLong>,
              &PackingBatchTest::assertionsCast<Byte, Byte>,
              &PackingBatchTest::assertionsCast<Short, Byte>,
              &PackingBatchTest::assertionsCast<Short, Short>,
              &PackingBatchTest::assertionsCast<Int, Byte>,
              &PackingBatchTest::assertionsCast<Int, Short>,
              &PackingBatchTest::assertionsCast<Int, Int>,
              &PackingBatchTest::assertionsCast<Long, Byte>,
              &PackingBatchTest::assertionsCast<Long, Short>,
              &PackingBatchTest::assertionsCast<Long, Int>,
              &PackingBatchTest::assertionsCast<Long, Long>,
              &PackingBatchTest::assertionsCast<Float, Float>,
              &PackingBatchTest::assertionsCast<Double, Float>,
              &PackingBatchTest::assertionsCast<Double, Double>});
}

using Magnum::Constants;
using Magnum::Vector2;
using Magnum::Vector3;
using Magnum::Vector4;

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

    unpackInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2ub::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    unpackInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2us::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    unpackInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2b::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    unpackInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2s::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    packInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2ub::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    packInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2us::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    packInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2b::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    packInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2s::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    unpackHalfInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2us::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
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

    packHalfInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Vector2::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Vector2us::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expected),
        TestSuite::Compare::Container);

    /* Ensure the results are consistent with non-batch APIs */
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        CORRADE_COMPARE(Math::packHalf(data[i].src), data[i].dst);
}

template<class FloatingPoint, class Integral> void PackingBatchTest::castUnsignedFloatingPoint() {
    setTestCaseTemplateName({TypeTraits<FloatingPoint>::name(), TypeTraits<Integral>::name()});

    struct Data {
        Math::Vector2<Integral> src;
        Math::Vector2<FloatingPoint> dst;
    } data[]{
        {{0, 89}, {}},
        {{149, 22}, {}},
        {{13, 255}, {}}
    };

    /* GCC 4.8 doesn't like constexpr here (cannot initialize aggregate of type
       ‘const Vector2 [3]’ with a compound literal), wtf */
    const Math::Vector2<FloatingPoint> expectedFloatingPoint[] {
        {FloatingPoint(0.0), FloatingPoint(89.0)},
        {FloatingPoint(149.0), FloatingPoint(22.0)},
        {FloatingPoint(13.0), FloatingPoint(255.0)}
    };

    constexpr Math::Vector2<Integral> expectedIntegral[] {
        {0, 89},
        {149, 22},
        {13, 255}
    };

    castInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<Integral>::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<FloatingPoint>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expectedFloatingPoint),
        TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<FloatingPoint>::data),
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<Integral>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::src),
        Containers::stridedArrayView(expectedIntegral),
        TestSuite::Compare::Container);
}

template<class FloatingPoint, class Integral> void PackingBatchTest::castSignedFloatingPoint() {
    setTestCaseTemplateName({TypeTraits<FloatingPoint>::name(), TypeTraits<Integral>::name()});

    struct Data {
        Math::Vector2<Integral> src;
        Math::Vector2<FloatingPoint> dst;
    } data[]{
        {{0, -89}, {}},
        {{-119, 22}, {}},
        {{13, 127}, {}}
    };

    /* GCC 4.8 doesn't like constexpr here (cannot initialize aggregate of type
       ‘const Vector2 [3]’ with a compound literal), wtf */
    const Math::Vector2<FloatingPoint> expectedFloatingPoint[] {
        {FloatingPoint(0.0), FloatingPoint(-89.0)},
        {FloatingPoint(-119.0), FloatingPoint(22.0)},
        {FloatingPoint(13.0), FloatingPoint(127.0)}
    };

    constexpr Math::Vector2<Integral> expectedIntegral[] {
        {0, -89},
        {-119, 22},
        {13, 127}
    };

    castInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<Integral>::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<FloatingPoint>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expectedFloatingPoint),
        TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<FloatingPoint>::data),
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<Integral>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::src),
        Containers::stridedArrayView(expectedIntegral),
        TestSuite::Compare::Container);
}

template<class T, class U> void PackingBatchTest::castUnsignedInteger() {
    setTestCaseTemplateName({TypeTraits<T>::name(), TypeTraits<U>::name()});

    struct Data {
        Math::Vector2<T> src;
        Math::Vector2<U> dst;
    } data[]{
        {{0, 89}, {}},
        {{149, 22}, {}},
        {{13, 255}, {}}
    };

    constexpr Math::Vector2<U> expectedTargetType[] {
        {0, 89},
        {149, 22},
        {13, 255}
    };

    constexpr Math::Vector2<T> expectedOriginalType[] {
        {0, 89},
        {149, 22},
        {13, 255}
    };

    castInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<T>::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<U>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expectedTargetType),
        TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<U>::data),
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<T>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::src),
        Containers::stridedArrayView(expectedOriginalType),
        TestSuite::Compare::Container);
}

template<class T, class U> void PackingBatchTest::castSignedInteger() {
    setTestCaseTemplateName({TypeTraits<T>::name(), TypeTraits<U>::name()});

    struct Data {
        Math::Vector2<T> src;
        Math::Vector2<U> dst;
    } data[]{
        {{0, -89}, {}},
        {{-119, 22}, {}},
        {{13, 127}, {}}
    };

    constexpr Math::Vector2<U> expectedTargetType[] {
        {0, -89},
        {-119, 22},
        {13, 127}
    };

    constexpr Math::Vector2<T> expectedOriginalType[] {
        {0, -89},
        {-119, 22},
        {13, 127}
    };

    castInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<T>::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<U>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expectedTargetType),
        TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<U>::data),
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<T>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::src),
        Containers::stridedArrayView(expectedOriginalType),
        TestSuite::Compare::Container);
}

template<class T, class U> void PackingBatchTest::castFloatDouble() {
    struct Data {
        Math::Vector2<T> src;
        Math::Vector2<U> dst;
    } data[]{
        {{T(0.25), T(-89.5)}, {}},
        {{T(-119.0), T(22.75)}, {}},
        {{T(13.0), T(127.5)}, {}}
    };

    constexpr Math::Vector2<U> expectedTargetType[] {
        {U(0.25), U(-89.5)},
        {U(-119.0), U(22.75)},
        {U(13.0), U(127.5)}
    };

    constexpr Math::Vector2<T> expectedOriginalType[] {
        {T(0.25), T(-89.5)},
        {T(-119.0), T(22.75)},
        {T(13.0), T(127.5)}
    };
    castInto(
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<T>::data),
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<U>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::dst),
        Containers::stridedArrayView(expectedTargetType),
        TestSuite::Compare::Container);

    /* Test the other way around as well */
    castInto(
        Containers::stridedArrayView(data).slice(&Data::dst)
            .slice(&Math::Vector2<U>::data),
        Containers::stridedArrayView(data).slice(&Data::src)
            .slice(&Math::Vector2<T>::data));
    CORRADE_COMPARE_AS(
        Containers::stridedArrayView(data).slice(&Data::src),
        Containers::stridedArrayView(expectedOriginalType),
        TestSuite::Compare::Container);
}

template<class T> void PackingBatchTest::assertionsPackUnpack() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Math::Vector2<T> data[2]{};
    Math::Vector4<T> dataNonContiguous[2]{};
    Vector2 resultWrongCount[1]{};
    Vector2 result[2]{};
    Vector3 resultWrongVectorSize[2]{};
    Vector4 resultNonContiguous[2]{};

    auto src = Containers::stridedArrayView(data)
        .slice(&Math::Vector2<T>::data);
    auto srcNonContiguous = Containers::StridedArrayView2D<T>{
            Containers::stridedArrayView(dataNonContiguous)
                .slice(&Math::Vector4<T>::data)
        }.every({1, 2});
    auto dst = Containers::stridedArrayView(result)
        .slice(&Vector2::data);
    auto dstWrongCount = Containers::stridedArrayView(resultWrongCount)
        .slice(&Vector2::data);
    auto dstWrongVectorSize = Containers::stridedArrayView(resultWrongVectorSize)
        .slice(&Vector3::data);
    auto dstNotContiguous = Containers::StridedArrayView2D<Float>{
            Containers::stridedArrayView(resultNonContiguous)
                .slice(&Vector4::data)
        }.every({1, 2});

    Containers::String out;
    Error redirectError{&out};
    unpackInto(src, dstWrongCount);
    unpackInto(src, dstWrongVectorSize);
    unpackInto(src, dstNotContiguous);
    unpackInto(srcNonContiguous, dst);
    packInto(dstWrongCount, src);
    packInto(dstWrongVectorSize, src);
    packInto(dstNotContiguous, src);
    packInto(dst, srcNonContiguous);
    CORRADE_COMPARE(out,
        "Math::unpackInto(): wrong destination size, got {1, 2} but expected {2, 2}\n"
        "Math::unpackInto(): wrong destination size, got {2, 3} but expected {2, 2}\n"
        "Math::unpackInto(): second destination view dimension is not contiguous\n"
        "Math::unpackInto(): second source view dimension is not contiguous\n"
        "Math::packInto(): wrong destination size, got {2, 2} but expected {1, 2}\n"
        "Math::packInto(): wrong destination size, got {2, 2} but expected {2, 3}\n"
        "Math::packInto(): second source view dimension is not contiguous\n"
        "Math::packInto(): second destination view dimension is not contiguous\n");
}

void PackingBatchTest::assertionsPackUnpackHalf() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2us data[2]{};
    Vector4us dataNonContiguous[2]{};
    Vector2 result[2]{};
    Vector2 resultWrongCount[1]{};
    Vector3 resultWrongVectorSize[2]{};
    Vector4 resultNonContiguous[2]{};

    auto src = Containers::stridedArrayView(data)
        .slice(&Vector2us::data);
    auto srcNonContiguous = Containers::StridedArrayView2D<UnsignedShort>{
            Containers::stridedArrayView(dataNonContiguous)
                .slice(&Vector4us::data)
        }.every({1, 2});
    auto dst = Containers::stridedArrayView(result)
        .slice(&Vector2::data);
    auto dstWrongCount = Containers::stridedArrayView(resultWrongCount)
        .slice(&Vector2::data);
    auto dstWrongVectorSize = Containers::stridedArrayView(resultWrongVectorSize)
        .slice(&Vector3::data);
    auto dstNotContiguous = Containers::StridedArrayView2D<Float>{
            Containers::stridedArrayView(resultNonContiguous)
                .slice(&Vector4::data)
        }.every({1, 2});

    Containers::String out;
    Error redirectError{&out};
    unpackHalfInto(src, dstWrongCount);
    unpackHalfInto(src, dstWrongVectorSize);
    unpackHalfInto(src, dstNotContiguous);
    unpackHalfInto(srcNonContiguous, dst);
    packHalfInto(dstWrongCount, src);
    packHalfInto(dstWrongVectorSize, src);
    packHalfInto(dstNotContiguous, src);
    packHalfInto(dst, srcNonContiguous);
    CORRADE_COMPARE(out,
        "Math::unpackHalfInto(): wrong destination size, got {1, 2} but expected {2, 2}\n"
        "Math::unpackHalfInto(): wrong destination size, got {2, 3} but expected {2, 2}\n"
        "Math::unpackHalfInto(): second destination view dimension is not contiguous\n"
        "Math::unpackHalfInto(): second source view dimension is not contiguous\n"
        "Math::packHalfInto(): wrong destination size, got {2, 2} but expected {1, 2}\n"
        "Math::packHalfInto(): wrong destination size, got {2, 2} but expected {2, 3}\n"
        "Math::packHalfInto(): second source view dimension is not contiguous\n"
        "Math::packHalfInto(): second destination view dimension is not contiguous\n");
}

template<class U, class T> void PackingBatchTest::assertionsCast() {
    CORRADE_SKIP_IF_NO_ASSERT();

    setTestCaseTemplateName({TypeTraits<U>::name(), TypeTraits<T>::name()});

    Math::Vector2<T> data[2]{};
    Math::Vector2<U> resultWrongCount[1]{};
    Math::Vector3<U> resultWrongVectorSize[2]{};
    Math::Vector4<U> resultNonContiguous[2]{};

    auto src = Containers::stridedArrayView(data)
        .slice(&Math::Vector2<T>::data);
    auto dstWrongCount = Containers::stridedArrayView(resultWrongCount)
        .slice(&Math::Vector2<U>::data);
    auto dstWrongVectorSize = Containers::stridedArrayView(resultWrongVectorSize)
        .slice(&Math::Vector3<U>::data);
    auto dstNotContiguous = Containers::StridedArrayView2D<U>{
            Containers::stridedArrayView(resultNonContiguous)
                .slice(&Math::Vector4<U>::data)
        }.every({1, 2});

    Containers::String out;
    Error redirectError{&out};
    castInto(src, dstWrongCount);
    castInto(src, dstWrongVectorSize);
    castInto(src, dstNotContiguous);
    castInto(dstWrongCount, src);
    castInto(dstWrongVectorSize, src);
    castInto(dstNotContiguous, src);
    CORRADE_COMPARE(out,
        "Math::castInto(): wrong destination size, got {1, 2} but expected {2, 2}\n"
        "Math::castInto(): wrong destination size, got {2, 3} but expected {2, 2}\n"
        "Math::castInto(): second destination view dimension is not contiguous\n"
        "Math::castInto(): wrong destination size, got {2, 2} but expected {1, 2}\n"
        "Math::castInto(): wrong destination size, got {2, 2} but expected {2, 3}\n"
        "Math::castInto(): second source view dimension is not contiguous\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::PackingBatchTest)
