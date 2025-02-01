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

#include <new>
#include <Corrade/Containers/ArrayView.h> /* arraySize() */
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Shaders/Line.h"
#include "Magnum/Shaders/Implementation/lineMiterLimit.h"

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

    /* Miter limit tests are copied verbatim between the Shaders and Ui
       libraries */
    void materialUniformMiterLimit();
    void materialUniformMiterLengthLimitInvalid();
    void materialUniformMiterAngleLimitInvalid();

    void debugCapStyle();
    void debugJoinStyle();
    void debugVertexAnnotation();
    void debugVertexAnnotationPacked();
    void debugVertexAnnotations();
    void debugVertexAnnotationsPacked();
};

using namespace Math::Literals;

const struct {
    const char* name;
    Float limit;
    const char* message;
} MaterialUniformMiterLengthLimitInvalidData[]{
    {"too short", 0.9997f,
        "expected a finite value greater than or equal to 1, got 0.9997"},
    {"too long", Constants::inf(),
        "expected a finite value greater than or equal to 1, got inf"},
};

const struct {
    const char* name;
    Rad limit;
    const char* message;
} MaterialUniformMiterAngleLimitInvalidData[]{
    {"too small", 0.0_degf,
        "expected a value greater than 0° and less than or equal to 180°, got 0°"},
    {"too large", 180.1_degf,
        "expected a value greater than 0° and less than or equal to 180°, got 180.1°"}
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
              &LineTest::materialUniformSetters,

              &LineTest::materialUniformMiterLimit});

    addInstancedTests({&LineTest::materialUniformMiterLengthLimitInvalid},
        Containers::arraySize(MaterialUniformMiterLengthLimitInvalidData));

    addInstancedTests({&LineTest::materialUniformMiterAngleLimitInvalid},
        Containers::arraySize(MaterialUniformMiterAngleLimitInvalidData));

    addTests({&LineTest::debugCapStyle,
              &LineTest::debugJoinStyle,
              &LineTest::debugVertexAnnotation,
              &LineTest::debugVertexAnnotationPacked,
              &LineTest::debugVertexAnnotations,
              &LineTest::debugVertexAnnotationsPacked});
}

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
    CORRADE_COMPARE(a.backgroundColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(b.backgroundColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(a.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(a.width, 1.0f);
    CORRADE_COMPARE(b.width, 1.0f);
    CORRADE_COMPARE(a.smoothness, 0.0f);
    CORRADE_COMPARE(b.smoothness, 0.0f);
    CORRADE_COMPARE(a.miterLimit, Implementation::lineMiterLengthLimit("", 4.0f));
    CORRADE_COMPARE(b.miterLimit, Implementation::lineMiterLengthLimit("", 4.0f));

    constexpr LineMaterialUniform ca;
    constexpr LineMaterialUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.backgroundColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(cb.backgroundColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(ca.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(ca.width, 1.0f);
    CORRADE_COMPARE(cb.width, 1.0f);
    CORRADE_COMPARE(ca.smoothness, 0.0f);
    CORRADE_COMPARE(cb.smoothness, 0.0f);
    CORRADE_COMPARE(ca.miterLimit, Implementation::lineMiterLengthLimit("", 4.0f));
    CORRADE_COMPARE(cb.miterLimit, Implementation::lineMiterLengthLimit("", 4.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<LineDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<LineDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, LineDrawUniform>::value);
}

void LineTest::materialUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    LineMaterialUniform a;
    a.color = 0x354565fc_rgbaf;
    a.smoothness = 7.0f;

    new(&a) LineMaterialUniform{NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
        CORRADE_COMPARE(a.smoothness, 7.0f);
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<LineMaterialUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, LineMaterialUniform>::value);
}

void LineTest::materialUniformSetters() {
    LineMaterialUniform a;
    a.setBackgroundColor(0x01020304_rgbaf)
     .setColor(0x354565fc_rgbaf)
     .setWidth(2.5f)
     .setSmoothness(7.0f)
     .setMiterLimit(3.4567f);

    CORRADE_COMPARE(a.backgroundColor, 0x01020304_rgbaf);
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    CORRADE_COMPARE(a.width, 2.5f);
    CORRADE_COMPARE(a.smoothness, 7.0f);
    CORRADE_COMPARE(a.miterLimit, 3.4567f);
}

void LineTest::materialUniformMiterLimit() {
    LineMaterialUniform a;

    /* Verifying documented relation of the default to angle/length */
    CORRADE_COMPARE(a.miterLimit, 0.875f);
    a.setMiterLengthLimit(4.0f);
    CORRADE_COMPARE(a.miterLimit, 0.875f);
    a.setMiterAngleLimit(28.955_degf);
    CORRADE_COMPARE(a.miterLimit, 0.875f);

    a.setMiterLengthLimit(25.0f);
    CORRADE_COMPARE(a.miterLimit, 0.9968f);

    a.setMiterAngleLimit(35.0_degf);
    CORRADE_COMPARE(a.miterLimit, 0.819152f);
}

void LineTest::materialUniformMiterLengthLimitInvalid() {
    auto&& data = MaterialUniformMiterLengthLimitInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    LineMaterialUniform a;

    Containers::String out;
    Error redirectError{&out};
    a.setMiterLengthLimit(data.limit);
    CORRADE_COMPARE(out, Utility::format("Shaders::LineMaterialUniform::setMiterLengthLimit(): {}\n", data.message));
}

void LineTest::materialUniformMiterAngleLimitInvalid() {
    auto&& data = MaterialUniformMiterAngleLimitInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    LineMaterialUniform a;

    Containers::String out;
    Error redirectError{&out};
    a.setMiterAngleLimit(data.limit);
    CORRADE_COMPARE(out, Utility::format("Shaders::LineMaterialUniform::setMiterAngleLimit(): {}\n", data.message));
}

void LineTest::debugCapStyle() {
    Containers::String out;
    Debug{&out} << LineCapStyle::Square << LineCapStyle(0xb0);
    CORRADE_COMPARE(out, "Shaders::LineCapStyle::Square Shaders::LineCapStyle(0xb0)\n");
}

void LineTest::debugJoinStyle() {
    Containers::String out;
    Debug{&out} << LineJoinStyle::Bevel << LineJoinStyle(0xb0);
    CORRADE_COMPARE(out, "Shaders::LineJoinStyle::Bevel Shaders::LineJoinStyle(0xb0)\n");
}

void LineTest::debugVertexAnnotation() {
    /* The values are guaranteed to fit into 8 bytes but the type itself is
       32bit to avoid surprises when passing it to the default-constructed
       LineGL::Annotation attribute (which defaults to 32bit), so it should
       also print the whole 32bit value. */
    Containers::String out;
    Debug{&out} << LineVertexAnnotation::Join << LineVertexAnnotation(0xcafecafe);
    CORRADE_COMPARE(out, "Shaders::LineVertexAnnotation::Join Shaders::LineVertexAnnotation(0xcafecafe)\n");
}

void LineTest::debugVertexAnnotationPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << LineVertexAnnotation::Join << Debug::packed << LineVertexAnnotation(0xcafecafe) << LineVertexAnnotation::Begin;
    CORRADE_COMPARE(out, "Join 0xcafecafe Shaders::LineVertexAnnotation::Begin\n");
}

void LineTest::debugVertexAnnotations() {
    Containers::String out;
    Debug{&out} << (LineVertexAnnotation::Up|LineVertexAnnotation::Join|LineVertexAnnotation(0xb00)) << LineVertexAnnotations{};
    CORRADE_COMPARE(out, "Shaders::LineVertexAnnotation::Up|Shaders::LineVertexAnnotation::Join|Shaders::LineVertexAnnotation(0xb00) Shaders::LineVertexAnnotations{}\n");
}

void LineTest::debugVertexAnnotationsPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (LineVertexAnnotation::Up|LineVertexAnnotation::Join|LineVertexAnnotation(0xb00)) << Debug::packed << LineVertexAnnotations{} << LineVertexAnnotation::Begin;
    CORRADE_COMPARE(out, "Up|Join|0xb00 {} Shaders::LineVertexAnnotation::Begin\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::LineTest)
