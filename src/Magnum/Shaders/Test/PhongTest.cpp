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

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Phong.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct PhongTest: TestSuite::Tester {
    explicit PhongTest();

    template<class T> void uniformSizeAlignment();

    void drawUniformConstructDefault();
    void drawUniformConstructNoInit();
    void drawUniformSetters();
    void drawUniformMaterialIdPacking();

    void materialUniformConstructDefault();
    void materialUniformConstructNoInit();
    void materialUniformSetters();

    void lightUniformConstructDefault();
    void lightUniformConstructNoInit();
    void lightUniformSetters();
};

PhongTest::PhongTest() {
    addTests({&PhongTest::uniformSizeAlignment<PhongDrawUniform>,
              &PhongTest::uniformSizeAlignment<PhongMaterialUniform>,
              &PhongTest::uniformSizeAlignment<PhongLightUniform>,

              &PhongTest::drawUniformConstructDefault,
              &PhongTest::drawUniformConstructNoInit,
              &PhongTest::drawUniformSetters,
              &PhongTest::drawUniformMaterialIdPacking,

              &PhongTest::materialUniformConstructDefault,
              &PhongTest::materialUniformConstructNoInit,
              &PhongTest::materialUniformSetters,

              &PhongTest::lightUniformConstructDefault,
              &PhongTest::lightUniformConstructNoInit,
              &PhongTest::lightUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<PhongDrawUniform> {
    static const char* name() { return "PhongDrawUniform"; }
};
template<> struct UniformTraits<PhongMaterialUniform> {
    static const char* name() { return "PhongMaterialUniform"; }
};
template<> struct UniformTraits<PhongLightUniform> {
    static const char* name() { return "PhongLightUniform"; }
};

template<class T> void PhongTest::uniformSizeAlignment() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment.");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment.");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768.");

    CORRADE_COMPARE(alignof(T), 4);
}

void PhongTest::drawUniformConstructDefault() {
    PhongDrawUniform a;
    PhongDrawUniform b{DefaultInit};
    CORRADE_COMPARE(a.normalMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(b.normalMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(a.materialId, 0);
    CORRADE_COMPARE(b.materialId, 0);
    CORRADE_COMPARE(a.objectId, 0);
    CORRADE_COMPARE(b.objectId, 0);
    CORRADE_COMPARE(a.lightOffset, 0);
    CORRADE_COMPARE(b.lightOffset, 0);
    CORRADE_COMPARE(a.lightCount, 0xffffffffu);
    CORRADE_COMPARE(b.lightCount, 0xffffffffu);

    constexpr PhongDrawUniform ca;
    constexpr PhongDrawUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.normalMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(cb.normalMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(ca.materialId, 0);
    CORRADE_COMPARE(cb.materialId, 0);
    CORRADE_COMPARE(ca.objectId, 0);
    CORRADE_COMPARE(cb.objectId, 0);
    CORRADE_COMPARE(ca.lightOffset, 0);
    CORRADE_COMPARE(cb.lightOffset, 0);
    CORRADE_COMPARE(ca.lightCount, 0xffffffffu);
    CORRADE_COMPARE(cb.lightCount, 0xffffffffu);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<PhongDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<PhongDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, PhongDrawUniform>::value);
}

void PhongTest::drawUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    PhongDrawUniform a;
    a.normalMatrix[2] = {1.5f, 0.3f, 3.1f, 0.5f};
    a.materialId = 5;
    a.lightCount = 7;

    new(&a) PhongDrawUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.normalMatrix[2], (Vector4{1.5f, 0.3f, 3.1f, 0.5f}));
        CORRADE_COMPARE(a.materialId, 5);
        CORRADE_COMPARE(a.lightCount, 7);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<PhongDrawUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, PhongDrawUniform>::value);
}

void PhongTest::drawUniformSetters() {
    PhongDrawUniform a;
    a.setNormalMatrix(Matrix4::rotationX(90.0_degf).normalMatrix())
     .setMaterialId(5)
     .setObjectId(7)
     .setLightOffsetCount(9, 13);
    CORRADE_COMPARE(a.normalMatrix, (Matrix3x4{
        Vector4{1.0f,  0.0f, 0.0f, 0.0f},
        Vector4{0.0f,  0.0f, 1.0f, 0.0f},
        Vector4{0.0f, -1.0f, 0.0f, 0.0f}
    }));
    CORRADE_COMPARE(a.materialId, 5);
    CORRADE_COMPARE(a.objectId, 7);
    CORRADE_COMPARE(a.lightOffset, 9);
    CORRADE_COMPARE(a.lightCount, 13);
}

void PhongTest::drawUniformMaterialIdPacking() {
    PhongDrawUniform a;
    a.setMaterialId(13765);
    /* The normalMatrix field is 3x4 floats, materialId should be right after
       in the low 16 bits on both LE and BE */
    CORRADE_COMPARE(reinterpret_cast<UnsignedInt*>(&a)[12] & 0xffff, 13765);
}

void PhongTest::materialUniformConstructDefault() {
    PhongMaterialUniform a;
    PhongMaterialUniform b{DefaultInit};
    CORRADE_COMPARE(a.ambientColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(b.ambientColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(a.diffuseColor, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.diffuseColor, 0xffffffff_rgbaf);
    CORRADE_COMPARE(a.specularColor, 0xffffff00_rgbaf);
    CORRADE_COMPARE(b.specularColor, 0xffffff00_rgbaf);
    CORRADE_COMPARE(a.normalTextureScale, 1.0f);
    CORRADE_COMPARE(b.normalTextureScale, 1.0f);
    CORRADE_COMPARE(a.shininess, 80.0f);
    CORRADE_COMPARE(b.shininess, 80.0f);
    CORRADE_COMPARE(a.alphaMask, 0.5f);
    CORRADE_COMPARE(b.alphaMask, 0.5f);

    constexpr PhongMaterialUniform ca;
    constexpr PhongMaterialUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.ambientColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(cb.ambientColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(ca.diffuseColor, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.diffuseColor, 0xffffffff_rgbaf);
    CORRADE_COMPARE(ca.specularColor, 0xffffff00_rgbaf);
    CORRADE_COMPARE(cb.specularColor, 0xffffff00_rgbaf);
    CORRADE_COMPARE(ca.normalTextureScale, 1.0f);
    CORRADE_COMPARE(cb.normalTextureScale, 1.0f);
    CORRADE_COMPARE(ca.shininess, 80.0f);
    CORRADE_COMPARE(cb.shininess, 80.0f);
    CORRADE_COMPARE(ca.alphaMask, 0.5f);
    CORRADE_COMPARE(cb.alphaMask, 0.5f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<PhongMaterialUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<PhongMaterialUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, PhongMaterialUniform>::value);
}

void PhongTest::materialUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    PhongMaterialUniform a;
    a.diffuseColor = 0x354565fc_rgbaf;
    a.normalTextureScale = 0.4f;
    a.alphaMask = 7.0f;

    new(&a) PhongMaterialUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.diffuseColor, 0x354565fc_rgbaf);
        CORRADE_COMPARE(a.normalTextureScale, 0.4f);
        CORRADE_COMPARE(a.alphaMask, 7.0f);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<PhongMaterialUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, PhongMaterialUniform>::value);
}

void PhongTest::materialUniformSetters() {
    PhongMaterialUniform a;
    a.setAmbientColor(0xff3366cc_rgbaf)
     .setDiffuseColor(0x996600aa_rgbaf)
     .setSpecularColor(0x0044ffdd_rgbaf)
     .setNormalTextureScale(0.4f)
     .setShininess(37.0f)
     .setAlphaMask(2.5f);
    CORRADE_COMPARE(a.ambientColor, 0xff3366cc_rgbaf);
    CORRADE_COMPARE(a.diffuseColor, 0x996600aa_rgbaf);
    CORRADE_COMPARE(a.specularColor, 0x0044ffdd_rgbaf);
    CORRADE_COMPARE(a.normalTextureScale, 0.4f);
    CORRADE_COMPARE(a.shininess, 37.0f);
    CORRADE_COMPARE(a.alphaMask, 2.5f);
}

void PhongTest::lightUniformConstructDefault() {
    PhongLightUniform a;
    PhongLightUniform b{DefaultInit};
    CORRADE_COMPARE(a.position, (Vector4{0.0f, 0.0f, 1.0f, 0.0f}));
    CORRADE_COMPARE(b.position, (Vector4{0.0f, 0.0f, 1.0f, 0.0f}));
    CORRADE_COMPARE(a.color, 0xffffff_rgbf);
    CORRADE_COMPARE(b.color, 0xffffff_rgbf);
    CORRADE_COMPARE(a.specularColor, 0xffffff_rgbf);
    CORRADE_COMPARE(b.specularColor, 0xffffff_rgbf);
    CORRADE_COMPARE(a.range, Constants::inf());
    CORRADE_COMPARE(b.range, Constants::inf());

    constexpr PhongLightUniform ca;
    constexpr PhongLightUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.position, (Vector4{0.0f, 0.0f, 1.0f, 0.0f}));
    CORRADE_COMPARE(cb.position, (Vector4{0.0f, 0.0f, 1.0f, 0.0f}));
    CORRADE_COMPARE(ca.color, 0xffffff_rgbf);
    CORRADE_COMPARE(cb.color, 0xffffff_rgbf);
    CORRADE_COMPARE(ca.specularColor, 0xffffff_rgbf);
    CORRADE_COMPARE(cb.specularColor, 0xffffff_rgbf);
    CORRADE_COMPARE(ca.range, Constants::inf());
    CORRADE_COMPARE(cb.range, Constants::inf());

    CORRADE_VERIFY(std::is_nothrow_default_constructible<PhongLightUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<PhongLightUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, PhongLightUniform>::value);
}

void PhongTest::lightUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    PhongLightUniform a;
    a.color = 0x354565_rgbf;
    a.range = 7.0f;

    new(&a) PhongLightUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.color, 0x354565_rgbf);
        CORRADE_COMPARE(a.range, 7.0f);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<PhongLightUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, PhongLightUniform>::value);
}

void PhongTest::lightUniformSetters() {
    PhongLightUniform a;
    a.setPosition({2.5f, 3.6f, 0.7f, 1.1f})
     .setColor(0x354565_rgbf)
     .setSpecularColor(0x996600_rgbf)
     .setRange(7.0f);
    CORRADE_COMPARE(a.position, (Vector4{2.5f, 3.6f, 0.7f, 1.1f}));
    CORRADE_COMPARE(a.color, 0x354565_rgbf);
    CORRADE_COMPARE(a.specularColor, 0x996600_rgbf);
    CORRADE_COMPARE(a.range, 7.0f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongTest)
