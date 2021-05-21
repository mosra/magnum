/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include <new>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Shaders/DistanceFieldVector.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct DistanceFieldVectorTest: TestSuite::Tester {
    explicit DistanceFieldVectorTest();

    template<class T> void uniformSizeAlignment();

    void drawUniformConstructDefault();
    void drawUniformConstructNoInit();
    void drawUniformSetters();
    void drawUniformMaterialIdPacking();

    void materialUniformConstructDefault();
    void materialUniformConstructNoInit();
    void materialUniformSetters();
};

DistanceFieldVectorTest::DistanceFieldVectorTest() {
    addTests({&DistanceFieldVectorTest::uniformSizeAlignment<DistanceFieldVectorDrawUniform>,
              &DistanceFieldVectorTest::uniformSizeAlignment<DistanceFieldVectorMaterialUniform>,

              &DistanceFieldVectorTest::drawUniformConstructDefault,
              &DistanceFieldVectorTest::drawUniformConstructNoInit,
              &DistanceFieldVectorTest::drawUniformSetters,
              &DistanceFieldVectorTest::drawUniformMaterialIdPacking,

              &DistanceFieldVectorTest::materialUniformConstructDefault,
              &DistanceFieldVectorTest::materialUniformConstructNoInit,
              &DistanceFieldVectorTest::materialUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<DistanceFieldVectorDrawUniform> {
    static const char* name() { return "DistanceFieldVectorDrawUniform"; }
};
template<> struct UniformTraits<DistanceFieldVectorMaterialUniform> {
    static const char* name() { return "DistanceFieldVectorMaterialUniform"; }
};

template<class T> void DistanceFieldVectorTest::uniformSizeAlignment() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment.");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment.");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768.");

    CORRADE_COMPARE(alignof(T), 4);
}

void DistanceFieldVectorTest::drawUniformConstructDefault() {
    DistanceFieldVectorDrawUniform a;
    DistanceFieldVectorDrawUniform b{DefaultInit};
    CORRADE_COMPARE(a.materialId, 0);
    CORRADE_COMPARE(b.materialId, 0);

    constexpr DistanceFieldVectorDrawUniform ca;
    constexpr DistanceFieldVectorDrawUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.materialId, 0);
    CORRADE_COMPARE(cb.materialId, 0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<DistanceFieldVectorDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<DistanceFieldVectorDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, DistanceFieldVectorDrawUniform>::value);
}

void DistanceFieldVectorTest::drawUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    DistanceFieldVectorDrawUniform a;
    a.materialId = 76;

    new(&a) DistanceFieldVectorDrawUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.materialId, 76);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<DistanceFieldVectorDrawUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, DistanceFieldVectorDrawUniform>::value);
}

void DistanceFieldVectorTest::drawUniformSetters() {
    DistanceFieldVectorDrawUniform a;
    a.setMaterialId(76);
    CORRADE_COMPARE(a.materialId, 76);
}

void DistanceFieldVectorTest::drawUniformMaterialIdPacking() {
    DistanceFieldVectorDrawUniform a;
    a.setMaterialId(13765);
    /* materialId should be right at the beginning, in the low 16 bits on both
       LE and BE */
    CORRADE_COMPARE(reinterpret_cast<UnsignedInt*>(&a)[0] & 0xffff, 13765);
}

void DistanceFieldVectorTest::materialUniformConstructDefault() {
    DistanceFieldVectorMaterialUniform a;
    DistanceFieldVectorMaterialUniform b{DefaultInit};
    CORRADE_COMPARE(a.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(a.outlineColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(b.outlineColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(a.outlineStart, 0.5f);
    CORRADE_COMPARE(b.outlineStart, 0.5f);
    CORRADE_COMPARE(a.outlineEnd, 1.0f);
    CORRADE_COMPARE(b.outlineEnd, 1.0f);
    CORRADE_COMPARE(a.smoothness, 0.04f);
    CORRADE_COMPARE(b.smoothness, 0.04f);

    constexpr DistanceFieldVectorMaterialUniform ca;
    constexpr DistanceFieldVectorMaterialUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(ca.outlineColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(cb.outlineColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(ca.outlineStart, 0.5f);
    CORRADE_COMPARE(cb.outlineStart, 0.5f);
    CORRADE_COMPARE(ca.outlineEnd, 1.0f);
    CORRADE_COMPARE(cb.outlineEnd, 1.0f);
    CORRADE_COMPARE(ca.smoothness, 0.04f);
    CORRADE_COMPARE(cb.smoothness, 0.04f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<DistanceFieldVectorMaterialUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<DistanceFieldVectorMaterialUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, DistanceFieldVectorMaterialUniform>::value);
}

void DistanceFieldVectorTest::materialUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    DistanceFieldVectorMaterialUniform a;
    a.color = 0x354565fc_rgbaf;
    a.outlineEnd = 0.37f;

    new(&a) DistanceFieldVectorMaterialUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
        CORRADE_COMPARE(a.outlineEnd, 0.37f);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<DistanceFieldVectorMaterialUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, DistanceFieldVectorMaterialUniform>::value);
}

void DistanceFieldVectorTest::materialUniformSetters() {
    DistanceFieldVectorMaterialUniform a;
    a.setColor(0x354565fc_rgbaf)
     .setOutlineColor(0x9876facd_rgbaf)
     .setOutlineRange(0.6f, 0.1f)
     .setSmoothness(0.37f);
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    CORRADE_COMPARE(a.outlineColor, 0x9876facd_rgbaf);
    CORRADE_COMPARE(a.outlineStart, 0.6f);
    CORRADE_COMPARE(a.outlineEnd, 0.1f);
    CORRADE_COMPARE(a.smoothness, 0.37f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::DistanceFieldVectorTest)
