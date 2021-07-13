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
#include "Magnum/Shaders/MeshVisualizer.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct MeshVisualizerTest: TestSuite::Tester {
    explicit MeshVisualizerTest();

    template<class T> void uniformSizeAlignment();

    void drawUniform2DConstructDefault();
    void drawUniform2DConstructNoInit();
    void drawUniform2DSetters();
    void drawUniform2DMaterialIdPacking();

    void drawUniform3DConstructDefault();
    void drawUniform3DConstructNoInit();
    void drawUniform3DSetters();
    void drawUniform3DMaterialIdPacking();

    void materialUniformConstructDefault();
    void materialUniformConstructNoInit();
    void materialUniformSetters();
};

MeshVisualizerTest::MeshVisualizerTest() {
    addTests({&MeshVisualizerTest::uniformSizeAlignment<MeshVisualizerDrawUniform2D>,
              &MeshVisualizerTest::uniformSizeAlignment<MeshVisualizerDrawUniform3D>,
              &MeshVisualizerTest::uniformSizeAlignment<MeshVisualizerMaterialUniform>,

              &MeshVisualizerTest::drawUniform2DConstructDefault,
              &MeshVisualizerTest::drawUniform2DConstructNoInit,
              &MeshVisualizerTest::drawUniform2DSetters,
              &MeshVisualizerTest::drawUniform2DMaterialIdPacking,

              &MeshVisualizerTest::drawUniform3DConstructDefault,
              &MeshVisualizerTest::drawUniform3DConstructNoInit,
              &MeshVisualizerTest::drawUniform3DSetters,
              &MeshVisualizerTest::drawUniform3DMaterialIdPacking,

              &MeshVisualizerTest::materialUniformConstructDefault,
              &MeshVisualizerTest::materialUniformConstructNoInit,
              &MeshVisualizerTest::materialUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<MeshVisualizerDrawUniform2D> {
    static const char* name() { return "MeshVisualizerDrawUniform2D"; }
};
template<> struct UniformTraits<MeshVisualizerDrawUniform3D> {
    static const char* name() { return "MeshVisualizerDrawUniform3D"; }
};
template<> struct UniformTraits<MeshVisualizerMaterialUniform> {
    static const char* name() { return "MeshVisualizerMaterialUniform"; }
};

template<class T> void MeshVisualizerTest::uniformSizeAlignment() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768");

    CORRADE_COMPARE(alignof(T), 4);
}

void MeshVisualizerTest::drawUniform2DConstructDefault() {
    MeshVisualizerDrawUniform2D a;
    MeshVisualizerDrawUniform2D b{DefaultInit};
    CORRADE_COMPARE(a.materialId, 0);
    CORRADE_COMPARE(b.materialId, 0);

    constexpr MeshVisualizerDrawUniform2D ca;
    constexpr MeshVisualizerDrawUniform2D cb{DefaultInit};
    CORRADE_COMPARE(ca.materialId, 0);
    CORRADE_COMPARE(cb.materialId, 0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<MeshVisualizerDrawUniform2D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<MeshVisualizerDrawUniform2D, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, MeshVisualizerDrawUniform2D>::value);
}

void MeshVisualizerTest::drawUniform2DConstructNoInit() {
    /* Testing only some fields, should be enough */
    MeshVisualizerDrawUniform2D a;
    a.materialId = 73;

    new(&a) MeshVisualizerDrawUniform2D{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.materialId, 73);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<MeshVisualizerDrawUniform2D, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, MeshVisualizerDrawUniform2D>::value);
}

void MeshVisualizerTest::drawUniform2DSetters() {
    MeshVisualizerDrawUniform2D a;
    a.setMaterialId(73);
    CORRADE_COMPARE(a.materialId, 73);
}

void MeshVisualizerTest::drawUniform2DMaterialIdPacking() {
    MeshVisualizerDrawUniform2D a;
    a.setMaterialId(13765);
    /* materialId should be right at the beginning, in the low 16 bits on both
       LE and BE */
    CORRADE_COMPARE(reinterpret_cast<UnsignedInt*>(&a)[0] & 0xffff, 13765);
}

void MeshVisualizerTest::drawUniform3DConstructDefault() {
    MeshVisualizerDrawUniform3D a;
    MeshVisualizerDrawUniform3D b{DefaultInit};
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

    constexpr MeshVisualizerDrawUniform3D ca;
    constexpr MeshVisualizerDrawUniform3D cb{DefaultInit};
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

    CORRADE_VERIFY(std::is_nothrow_default_constructible<MeshVisualizerDrawUniform3D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<MeshVisualizerDrawUniform3D, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, MeshVisualizerDrawUniform2D>::value);
}

void MeshVisualizerTest::drawUniform3DConstructNoInit() {
    /* Testing only some fields, should be enough */
    MeshVisualizerDrawUniform3D a;
    a.normalMatrix[2] = {1.5f, 0.3f, 3.1f, 0.5f};
    a.materialId = 5;

    new(&a) MeshVisualizerDrawUniform3D{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.normalMatrix[2], (Vector4{1.5f, 0.3f, 3.1f, 0.5f}));
        CORRADE_COMPARE(a.materialId, 5);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<MeshVisualizerDrawUniform3D, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, MeshVisualizerDrawUniform3D>::value);
}

void MeshVisualizerTest::drawUniform3DSetters() {
    MeshVisualizerDrawUniform3D a;
    a.setNormalMatrix(Matrix4::rotationX(90.0_degf).normalMatrix())
     .setMaterialId(5);
    CORRADE_COMPARE(a.normalMatrix, (Matrix3x4{
        Vector4{1.0f,  0.0f, 0.0f, 0.0f},
        Vector4{0.0f,  0.0f, 1.0f, 0.0f},
        Vector4{0.0f, -1.0f, 0.0f, 0.0f}
    }));
    CORRADE_COMPARE(a.materialId, 5);
}

void MeshVisualizerTest::drawUniform3DMaterialIdPacking() {
    MeshVisualizerDrawUniform3D a;
    a.setMaterialId(13765);
    /* The normalMatrix field is 3x4 floats, materialId should be right after
       in the low 16 bits on both LE and BE */
    CORRADE_COMPARE(reinterpret_cast<UnsignedInt*>(&a)[12] & 0xffff, 13765);
}

void MeshVisualizerTest::materialUniformConstructDefault() {
    MeshVisualizerMaterialUniform a;
    MeshVisualizerMaterialUniform b{DefaultInit};
    CORRADE_COMPARE(a.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(a.wireframeColor, 0x000000ff_rgbaf);
    CORRADE_COMPARE(b.wireframeColor, 0x000000ff_rgbaf);
    CORRADE_COMPARE(a.wireframeWidth, 1.0f);
    CORRADE_COMPARE(b.wireframeWidth, 1.0f);
    CORRADE_COMPARE(a.colorMapOffset, 1.0f/512.0f);
    CORRADE_COMPARE(b.colorMapOffset, 1.0f/512.0f);
    CORRADE_COMPARE(a.colorMapScale, 1.0f/256.0f);
    CORRADE_COMPARE(b.colorMapScale, 1.0f/256.0f);
    CORRADE_COMPARE(a.lineWidth, 1.0f);
    CORRADE_COMPARE(b.lineWidth, 1.0f);
    CORRADE_COMPARE(a.lineLength, 1.0f);
    CORRADE_COMPARE(b.lineLength, 1.0f);
    CORRADE_COMPARE(a.smoothness, 2.0f);
    CORRADE_COMPARE(b.smoothness, 2.0f);

    constexpr MeshVisualizerMaterialUniform ca;
    constexpr MeshVisualizerMaterialUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(ca.wireframeColor, 0x000000ff_rgbaf);
    CORRADE_COMPARE(cb.wireframeColor, 0x000000ff_rgbaf);
    CORRADE_COMPARE(ca.wireframeWidth, 1.0f);
    CORRADE_COMPARE(cb.wireframeWidth, 1.0f);
    CORRADE_COMPARE(ca.colorMapOffset, 1.0f/512.0f);
    CORRADE_COMPARE(cb.colorMapOffset, 1.0f/512.0f);
    CORRADE_COMPARE(ca.colorMapScale, 1.0f/256.0f);
    CORRADE_COMPARE(cb.colorMapScale, 1.0f/256.0f);
    CORRADE_COMPARE(ca.lineWidth, 1.0f);
    CORRADE_COMPARE(cb.lineWidth, 1.0f);
    CORRADE_COMPARE(ca.lineLength, 1.0f);
    CORRADE_COMPARE(cb.lineLength, 1.0f);
    CORRADE_COMPARE(ca.smoothness, 2.0f);
    CORRADE_COMPARE(cb.smoothness, 2.0f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<MeshVisualizerMaterialUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<MeshVisualizerMaterialUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, MeshVisualizerMaterialUniform>::value);
}

void MeshVisualizerTest::materialUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    MeshVisualizerMaterialUniform a;
    a.color = 0x354565fc_rgbaf;
    a.lineWidth = 0.765f;

    new(&a) MeshVisualizerMaterialUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
        CORRADE_COMPARE(a.lineWidth, 0.765f);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<MeshVisualizerMaterialUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, MeshVisualizerMaterialUniform>::value);
}

void MeshVisualizerTest::materialUniformSetters() {
    MeshVisualizerMaterialUniform a;
    a.setColor(0x354565fc_rgbaf)
     .setWireframeColor(0x9876fadc_rgbaf)
     .setWireframeWidth(3.5f)
     .setColorMapTransformation(35.5f, 0.5f)
     .setLineWidth(3.0f)
     .setLineLength(4.0f)
     .setSmoothness(5.0f);
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    CORRADE_COMPARE(a.wireframeColor, 0x9876fadc_rgbaf);
    CORRADE_COMPARE(a.wireframeWidth, 3.5f);
    CORRADE_COMPARE(a.colorMapOffset, 35.5f);
    CORRADE_COMPARE(a.colorMapScale, 0.5f);
    CORRADE_COMPARE(a.lineWidth, 3.0f);
    CORRADE_COMPARE(a.lineLength, 4.0f);
    CORRADE_COMPARE(a.smoothness, 5.0f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerTest)
