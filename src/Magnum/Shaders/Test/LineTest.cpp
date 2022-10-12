/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Shaders/Line.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct LineTest: TestSuite::Tester {
    explicit LineTest();

    template<class T> void uniformSizeAlignment();

    void drawUniformConstructDefault();
    void drawUniformConstructNoInit();
    void drawUniformSetters();
    void drawUniformMaterialIdPacking();

    void materialUniformConstructDefault();
    void materialUniformConstructNoInit();
    void materialUniformSetters();
};

LineTest::LineTest() {
    addTests({&LineTest::uniformSizeAlignment<LineDrawUniform>,
              &LineTest::uniformSizeAlignment<LineMaterialUniform>,

              &LineTest::drawUniformConstructDefault,
              &LineTest::drawUniformConstructNoInit,
              &LineTest::drawUniformSetters,
              &LineTest::drawUniformMaterialIdPacking,

              &LineTest::materialUniformConstructDefault,
              &LineTest::materialUniformConstructNoInit,
              &LineTest::materialUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<LineDrawUniform> {
    static const char* name() { return "LineDrawUniform"; }
};
template<> struct UniformTraits<LineMaterialUniform> {
    static const char* name() { return "LineMaterialUniform"; }
};

template<class T> void LineTest::uniformSizeAlignment() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment.");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment.");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768.");

    CORRADE_COMPARE(alignof(T), 4);
}

void LineTest::drawUniformConstructDefault() {
    LineDrawUniform a;
    LineDrawUniform b{DefaultInit};
    CORRADE_COMPARE(a.materialId, 0);
    CORRADE_COMPARE(b.materialId, 0);
    CORRADE_COMPARE(a.objectId, 0);
    CORRADE_COMPARE(b.objectId, 0);

    constexpr LineDrawUniform ca;
    constexpr LineDrawUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.materialId, 0);
    CORRADE_COMPARE(cb.materialId, 0);
    CORRADE_COMPARE(ca.objectId, 0);
    CORRADE_COMPARE(cb.objectId, 0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<LineDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<LineDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, LineDrawUniform>::value);
}

void LineTest::drawUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    LineDrawUniform a;
    a.materialId = 5;
    a.objectId = 7;

    new(&a) LineDrawUniform{NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.materialId, 5);
        CORRADE_COMPARE(a.objectId, 7);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<LineDrawUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, LineDrawUniform>::value);
}

void LineTest::drawUniformSetters() {
    LineDrawUniform a;
    a.setMaterialId(5)
     .setObjectId(7);
    CORRADE_COMPARE(a.materialId, 5);
    CORRADE_COMPARE(a.objectId, 7);
}

void LineTest::drawUniformMaterialIdPacking() {
    LineDrawUniform a;
    a.setMaterialId(13765);
    /* materialId should be right at the beginning, in the low 16 bits on both
       LE and BE */
    CORRADE_COMPARE(reinterpret_cast<UnsignedInt*>(&a)[0] & 0xffff, 13765);
}

void LineTest::materialUniformConstructDefault() {
    LineMaterialUniform a;
    LineMaterialUniform b{DefaultInit};
    CORRADE_COMPARE(a.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.color, 0xffffffff_rgbaf);

    constexpr LineMaterialUniform ca;
    constexpr LineMaterialUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.color, 0xffffffff_rgbaf);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<LineDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<LineDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, LineDrawUniform>::value);
}

void LineTest::materialUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    LineMaterialUniform a;
    a.color = 0x354565fc_rgbaf;

    new(&a) LineMaterialUniform{NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<LineMaterialUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, LineMaterialUniform>::value);
}

void LineTest::materialUniformSetters() {
    LineMaterialUniform a;
    a.setColor(0x354565fc_rgbaf);
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::LineTest)
