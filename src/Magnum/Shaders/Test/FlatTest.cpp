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

#include "Magnum/Shaders/Flat.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct FlatTest: TestSuite::Tester {
    explicit FlatTest();

    template<class T> void uniformSizeAlignment();

    void drawUniformConstructDefault();
    void drawUniformConstructNoInit();
    void drawUniformSetters();
    void drawUniformMaterialIdPacking();

    void materialUniformConstructDefault();
    void materialUniformConstructNoInit();
    void materialUniformSetters();
};

FlatTest::FlatTest() {
    addTests({&FlatTest::uniformSizeAlignment<FlatDrawUniform>,
              &FlatTest::uniformSizeAlignment<FlatMaterialUniform>,

              &FlatTest::drawUniformConstructDefault,
              &FlatTest::drawUniformConstructNoInit,
              &FlatTest::drawUniformSetters,
              &FlatTest::drawUniformMaterialIdPacking,

              &FlatTest::materialUniformConstructDefault,
              &FlatTest::materialUniformConstructNoInit,
              &FlatTest::materialUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<FlatDrawUniform> {
    static const char* name() { return "FlatDrawUniform"; }
};
template<> struct UniformTraits<FlatMaterialUniform> {
    static const char* name() { return "FlatMaterialUniform"; }
};

template<class T> void FlatTest::uniformSizeAlignment() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment.");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment.");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768.");

    CORRADE_COMPARE(alignof(T), 4);
}

void FlatTest::drawUniformConstructDefault() {
    FlatDrawUniform a;
    FlatDrawUniform b{DefaultInit};
    CORRADE_COMPARE(a.materialId, 0);
    CORRADE_COMPARE(b.materialId, 0);
    CORRADE_COMPARE(a.objectId, 0);
    CORRADE_COMPARE(b.objectId, 0);

    constexpr FlatDrawUniform ca;
    constexpr FlatDrawUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.materialId, 0);
    CORRADE_COMPARE(cb.materialId, 0);
    CORRADE_COMPARE(ca.objectId, 0);
    CORRADE_COMPARE(cb.objectId, 0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<FlatDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<FlatDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, FlatDrawUniform>::value);
}

void FlatTest::drawUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    FlatDrawUniform a;
    a.materialId = 5;
    a.objectId = 7;

    new(&a) FlatDrawUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.materialId, 5);
        CORRADE_COMPARE(a.objectId, 7);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<FlatDrawUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, FlatDrawUniform>::value);
}

void FlatTest::drawUniformSetters() {
    FlatDrawUniform a;
    a.setMaterialId(5)
     .setObjectId(7);
    CORRADE_COMPARE(a.materialId, 5);
    CORRADE_COMPARE(a.objectId, 7);
}

void FlatTest::drawUniformMaterialIdPacking() {
    FlatDrawUniform a;
    a.setMaterialId(13765);
    /* materialId should be right at the beginning, in the low 16 bits on both
       LE and BE */
    CORRADE_COMPARE(reinterpret_cast<UnsignedInt*>(&a)[0] & 0xffff, 13765);
}

void FlatTest::materialUniformConstructDefault() {
    FlatMaterialUniform a;
    FlatMaterialUniform b{DefaultInit};
    CORRADE_COMPARE(a.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(a.alphaMask, 0.5f);
    CORRADE_COMPARE(b.alphaMask, 0.5f);

    constexpr FlatMaterialUniform ca;
    constexpr FlatMaterialUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(ca.alphaMask, 0.5f);
    CORRADE_COMPARE(cb.alphaMask, 0.5f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<FlatDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<FlatDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, FlatDrawUniform>::value);
}

void FlatTest::materialUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    FlatMaterialUniform a;
    a.color = 0x354565fc_rgbaf;
    a.alphaMask = 0.7f;

    new(&a) FlatMaterialUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
        CORRADE_COMPARE(a.alphaMask, 0.7f);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<FlatMaterialUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, FlatMaterialUniform>::value);
}

void FlatTest::materialUniformSetters() {
    FlatMaterialUniform a;
    a.setColor(0x354565fc_rgbaf)
     .setAlphaMask(0.7f);
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    CORRADE_COMPARE(a.alphaMask, 0.7f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatTest)
