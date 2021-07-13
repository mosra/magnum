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

#include "Magnum/Shaders/Generic.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct GenericTest: TestSuite::Tester {
    explicit GenericTest();

    template<class T> void uniformSizeAlignment();

    void projectionUniform2DConstructDefault();
    void projectionUniform2DConstructNoInit();
    void projectionUniform2DSetters();

    void projectionUniform3DConstructDefault();
    void projectionUniform3DConstructNoInit();
    void projectionUniform3DSetters();

    void transformationUniform2DConstructDefault();
    void transformationUniform2DConstructNoInit();
    void transformationUniform2DSetters();

    void transformationUniform3DConstructDefault();
    void transformationUniform3DConstructNoInit();
    void transformationUniform3DSetters();

    void textureTransformationUniformConstructDefault();
    void textureTransformationUniformConstructNoInit();
    void textureTransformationUniformSetters();
};

GenericTest::GenericTest() {
    addTests({&GenericTest::uniformSizeAlignment<ProjectionUniform2D>,
              &GenericTest::uniformSizeAlignment<ProjectionUniform3D>,
              &GenericTest::uniformSizeAlignment<TransformationUniform2D>,
              &GenericTest::uniformSizeAlignment<TransformationUniform3D>,
              &GenericTest::uniformSizeAlignment<TextureTransformationUniform>,

              &GenericTest::projectionUniform2DConstructDefault,
              &GenericTest::projectionUniform2DConstructNoInit,
              &GenericTest::projectionUniform2DSetters,

              &GenericTest::projectionUniform3DConstructDefault,
              &GenericTest::projectionUniform3DConstructNoInit,
              &GenericTest::projectionUniform3DSetters,

              &GenericTest::transformationUniform2DConstructDefault,
              &GenericTest::transformationUniform2DConstructNoInit,
              &GenericTest::transformationUniform2DSetters,

              &GenericTest::transformationUniform3DConstructDefault,
              &GenericTest::transformationUniform3DConstructNoInit,
              &GenericTest::transformationUniform3DSetters,

              &GenericTest::textureTransformationUniformConstructDefault,
              &GenericTest::textureTransformationUniformConstructNoInit,
              &GenericTest::textureTransformationUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<ProjectionUniform2D> {
    static const char* name() { return "ProjectionUniform2D"; }
};
template<> struct UniformTraits<ProjectionUniform3D> {
    static const char* name() { return "ProjectionUniform3D"; }
};
template<> struct UniformTraits<TransformationUniform2D> {
    static const char* name() { return "TransformationUniform2D"; }
};
template<> struct UniformTraits<TransformationUniform3D> {
    static const char* name() { return "TransformationUniform3D"; }
};
template<> struct UniformTraits<TextureTransformationUniform> {
    static const char* name() { return "TextureTransformationUniform"; }
};

template<class T> void GenericTest::uniformSizeAlignment() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment.");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment.");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768.");

    CORRADE_COMPARE(alignof(T), 4);
}

void GenericTest::projectionUniform2DConstructDefault() {
    ProjectionUniform2D a;
    ProjectionUniform2D b{DefaultInit};
    CORRADE_COMPARE(a.projectionMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(b.projectionMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));

    constexpr ProjectionUniform2D ca;
    constexpr ProjectionUniform2D cb{DefaultInit};
    CORRADE_COMPARE(ca.projectionMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(cb.projectionMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<ProjectionUniform2D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<ProjectionUniform2D, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, ProjectionUniform2D>::value);
}

void GenericTest::projectionUniform2DConstructNoInit() {
    ProjectionUniform2D a;
    a.projectionMatrix[2] = {1.5f, 0.3f, 3.1f, 0.5f};

    new(&a) ProjectionUniform2D{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.projectionMatrix[2], (Vector4{1.5f, 0.3f, 3.1f, 0.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<ProjectionUniform2D, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, ProjectionUniform2D>::value);
}

void GenericTest::projectionUniform2DSetters() {
    ProjectionUniform2D a;
    a.setProjectionMatrix(Matrix3::projection({2.5f, 3.0f}));
    CORRADE_COMPARE(a.projectionMatrix, (Matrix3x4{
        Vector4{0.8f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.666667f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
}

void GenericTest::projectionUniform3DConstructDefault() {
    ProjectionUniform3D a;
    ProjectionUniform3D b{DefaultInit};
    CORRADE_COMPARE(a.projectionMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));
    CORRADE_COMPARE(b.projectionMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));

    constexpr ProjectionUniform3D ca;
    constexpr ProjectionUniform3D cb{DefaultInit};
    CORRADE_COMPARE(ca.projectionMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));
    CORRADE_COMPARE(cb.projectionMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<ProjectionUniform3D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<ProjectionUniform3D, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, ProjectionUniform3D>::value);
}

void GenericTest::projectionUniform3DConstructNoInit() {
    ProjectionUniform3D a;
    a.projectionMatrix[2] = {1.5f, 0.3f, 3.1f, 0.5f};

    new(&a) ProjectionUniform3D{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.projectionMatrix[2], (Vector4{1.5f, 0.3f, 3.1f, 0.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<ProjectionUniform3D, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, ProjectionUniform3D>::value);
}

void GenericTest::projectionUniform3DSetters() {
    ProjectionUniform3D a;
    a.setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.5f, 0.1f, 100.0f));
    CORRADE_COMPARE(a.projectionMatrix, Matrix4::perspectiveProjection(35.0_degf, 1.5f, 0.1f, 100.0f));
}

void GenericTest::transformationUniform2DConstructDefault() {
    TransformationUniform2D a;
    TransformationUniform2D b{DefaultInit};
    CORRADE_COMPARE(a.transformationMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(b.transformationMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));

    constexpr TransformationUniform2D ca;
    constexpr TransformationUniform2D cb{DefaultInit};
    CORRADE_COMPARE(ca.transformationMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));
    CORRADE_COMPARE(cb.transformationMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f}
    }));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<TransformationUniform2D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<TransformationUniform2D, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, TransformationUniform2D>::value);
}

void GenericTest::transformationUniform2DConstructNoInit() {
    TransformationUniform2D a;
    a.transformationMatrix[2] = {1.5f, 0.3f, 3.1f, 0.5f};

    new(&a) TransformationUniform2D{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.transformationMatrix[2], (Vector4{1.5f, 0.3f, 3.1f, 0.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<TransformationUniform2D, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, TransformationUniform2D>::value);
}

void GenericTest::transformationUniform2DSetters() {
    TransformationUniform2D a;
    a.setTransformationMatrix(Matrix3::translation({2.5f, 3.0f}));
    CORRADE_COMPARE(a.transformationMatrix, (Matrix3x4{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{2.5f, 3.0f, 1.0f, 0.0f}
    }));
}

void GenericTest::transformationUniform3DConstructDefault() {
    TransformationUniform3D a;
    TransformationUniform3D b{DefaultInit};
    CORRADE_COMPARE(a.transformationMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));
    CORRADE_COMPARE(b.transformationMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));

    constexpr TransformationUniform3D ca;
    constexpr TransformationUniform3D cb{DefaultInit};
    CORRADE_COMPARE(ca.transformationMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));
    CORRADE_COMPARE(cb.transformationMatrix, (Matrix4{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<TransformationUniform3D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<TransformationUniform3D, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, TransformationUniform3D>::value);
}

void GenericTest::transformationUniform3DConstructNoInit() {
    TransformationUniform3D a;
    a.transformationMatrix[2] = {1.5f, 0.3f, 3.1f, 0.5f};

    new(&a) TransformationUniform3D{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.transformationMatrix[2], (Vector4{1.5f, 0.3f, 3.1f, 0.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<TransformationUniform3D, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, TransformationUniform3D>::value);
}

void GenericTest::transformationUniform3DSetters() {
    TransformationUniform3D a;
    a.setTransformationMatrix(Matrix4::translation({3.5f, 2.0f, -1.0f}));
    CORRADE_COMPARE(a.transformationMatrix, Matrix4::translation({3.5f, 2.0f, -1.0f}));
}

void GenericTest::textureTransformationUniformConstructDefault() {
    TextureTransformationUniform a;
    TextureTransformationUniform b{DefaultInit};
    CORRADE_COMPARE(a.rotationScaling, (Matrix2x2{
        Vector2{1.0f, 0.0f},
        Vector2{0.0f, 1.0f}
    }));
    CORRADE_COMPARE(b.rotationScaling, (Matrix2x2{
        Vector2{1.0f, 0.0f},
        Vector2{0.0f, 1.0f}
    }));
    CORRADE_COMPARE(a.offset, (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(b.offset, (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(a.layer, 0);
    CORRADE_COMPARE(b.layer, 0);

    constexpr TextureTransformationUniform ca;
    constexpr TextureTransformationUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.rotationScaling, (Matrix2x2{
        Vector2{1.0f, 0.0f},
        Vector2{0.0f, 1.0f}
    }));
    CORRADE_COMPARE(cb.rotationScaling, (Matrix2x2{
        Vector2{1.0f, 0.0f},
        Vector2{0.0f, 1.0f}
    }));
    CORRADE_COMPARE(ca.offset, (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(cb.offset, (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(ca.layer, 0);
    CORRADE_COMPARE(cb.layer, 0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<TextureTransformationUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<TextureTransformationUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, TextureTransformationUniform>::value);
}

void GenericTest::textureTransformationUniformConstructNoInit() {
    TextureTransformationUniform a;
    a.rotationScaling[1] = {2.5f, -3.0f};
    a.offset = {2.7f, 0.3f};
    a.layer = 37;

    new(&a) TextureTransformationUniform{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.rotationScaling[1], (Vector2{2.5f, -3.0f}));
        CORRADE_COMPARE(a.offset, (Vector2{2.7f, 0.3f}));
        CORRADE_COMPARE(a.layer, 37);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<TextureTransformationUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, TextureTransformationUniform>::value);
}

void GenericTest::textureTransformationUniformSetters() {
    TextureTransformationUniform a;
    a.setTextureMatrix(Matrix3::translation({2.6f, 0.3f})*
                       Matrix3::rotation(90.0_degf))
     .setLayer(37);
    CORRADE_COMPARE(a.rotationScaling, (Matrix2x2{
        Vector2{ 0.0f, 1.0f},
        Vector2{-1.0f, 0.0f}
    }));
    CORRADE_COMPARE(a.offset, (Vector2{2.6f, 0.3f}));
    CORRADE_COMPARE(a.layer, 37);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::GenericTest)
