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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/VertexFormat.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Test { namespace {

struct VertexFormatTest: TestSuite::Tester {
    explicit VertexFormatTest();

    void mapping();

    void isImplementationSpecific();
    void wrap();
    void wrapInvalid();
    void unwrap();
    void unwrapInvalid();
    void size();
    void sizeInvalid();
    void sizeImplementationSpecific();
    void componentCount();
    void componentCountInvalid();
    void componentCountImplementationSpecific();
    void componentFormat();
    void componentFormatInvalid();
    void componentFormatImplementationSpecific();
    void vectorCount();
    void vectorCountInvalid();
    void vectorCountImplementationSpecific();
    void vectorStride();
    void vectorStrideInvalid();
    void vectorStrideImplementationSpecific();
    void isNormalized();
    void isNormalizedInvalid();
    void isNormalizedImplementationSpecific();

    void assemble();
    void assembleRoundtrip();
    void assembleCantNormalize();
    void assembleInvalidComponentCount();
    void assembleImplementationSpecific();

    void assembleMatrix();
    void assembleMatrixRoundtrip();
    void assembleMatrixInvalidType();
    void assembleMatrixInvalidCount();
    void assembleMatrixImplementationSpecific();

    void debug();
    void debugImplementationSpecific();
    void configuration();
};

constexpr struct {
    VertexFormat componentType;
    bool normalized;
} AssembleRoundtripData[] {
    {VertexFormat::Float, false},
    {VertexFormat::Double, false},
    {VertexFormat::UnsignedByte, false},
    {VertexFormat::UnsignedByte, true},
    {VertexFormat::Byte, false},
    {VertexFormat::Byte, true},
    {VertexFormat::UnsignedShort, false},
    {VertexFormat::UnsignedShort, true},
    {VertexFormat::Short, false},
    {VertexFormat::Short, true},
    {VertexFormat::UnsignedInt, false},
    {VertexFormat::Int, false}
};

constexpr struct {
    VertexFormat componentType;
    UnsignedInt componentCount;
    bool aligned;
} AssembleMatrixRoundtripData[]{
    {VertexFormat::Float, 2, true},
    {VertexFormat::Float, 3, true},
    {VertexFormat::Float, 4, true},
    {VertexFormat::Half, 2, false},
    {VertexFormat::Half, 3, false},
    {VertexFormat::Half, 3, true},
    {VertexFormat::Half, 4, false},
    {VertexFormat::Double, 2, true},
    {VertexFormat::Double, 3, true},
    {VertexFormat::Double, 4, true},
    {VertexFormat::Byte, 2, false},
    {VertexFormat::Byte, 2, true},
    {VertexFormat::Byte, 3, false},
    {VertexFormat::Byte, 3, true},
    {VertexFormat::Byte, 4, false},
    {VertexFormat::Short, 2, false},
    {VertexFormat::Short, 3, false},
    {VertexFormat::Short, 3, true},
    {VertexFormat::Short, 4, false}
};

VertexFormatTest::VertexFormatTest() {
    addTests({&VertexFormatTest::mapping,

              &VertexFormatTest::isImplementationSpecific,
              &VertexFormatTest::wrap,
              &VertexFormatTest::wrapInvalid,
              &VertexFormatTest::unwrap,
              &VertexFormatTest::unwrapInvalid,
              &VertexFormatTest::size,
              &VertexFormatTest::sizeInvalid,
              &VertexFormatTest::sizeImplementationSpecific,
              &VertexFormatTest::componentCount,
              &VertexFormatTest::componentCountInvalid,
              &VertexFormatTest::componentCountImplementationSpecific,
              &VertexFormatTest::componentFormat,
              &VertexFormatTest::componentFormatInvalid,
              &VertexFormatTest::componentFormatImplementationSpecific,
              &VertexFormatTest::vectorCount,
              &VertexFormatTest::vectorCountInvalid,
              &VertexFormatTest::vectorCountImplementationSpecific,
              &VertexFormatTest::vectorStride,
              &VertexFormatTest::vectorStrideInvalid,
              &VertexFormatTest::vectorStrideImplementationSpecific,
              &VertexFormatTest::isNormalized,
              &VertexFormatTest::isNormalizedInvalid,
              &VertexFormatTest::isNormalizedImplementationSpecific,

              &VertexFormatTest::assemble});

    addRepeatedInstancedTests({&VertexFormatTest::assembleRoundtrip}, 4,
        Containers::arraySize(AssembleRoundtripData));

    addTests({&VertexFormatTest::assembleCantNormalize,
              &VertexFormatTest::assembleInvalidComponentCount,
              &VertexFormatTest::assembleImplementationSpecific,

              &VertexFormatTest::assembleMatrix});

    addRepeatedInstancedTests({&VertexFormatTest::assembleMatrixRoundtrip}, 3,
        Containers::arraySize(AssembleMatrixRoundtripData));

    addTests({&VertexFormatTest::assembleMatrixInvalidType,
              &VertexFormatTest::assembleMatrixInvalidCount,
              &VertexFormatTest::assembleMatrixImplementationSpecific,

              &VertexFormatTest::debug,
              &VertexFormatTest::debugImplementationSpecific,
              &VertexFormatTest::configuration});
}

void VertexFormatTest::mapping() {
    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid type */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto type = VertexFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range) */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(type) {
            #define _c(type) \
                case VertexFormat::type: \
                    CORRADE_COMPARE(Utility::ConfigurationValue<VertexFormat>::toString(VertexFormat::type, {}), #type); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Implementation/vertexFormatMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void VertexFormatTest::isImplementationSpecific() {
    constexpr bool a = isVertexFormatImplementationSpecific(VertexFormat::Vector2sNormalized);
    constexpr bool b = isVertexFormatImplementationSpecific(VertexFormat(0x8000dead));
    CORRADE_VERIFY(!a);
    CORRADE_VERIFY(b);
}

void VertexFormatTest::wrap() {
    constexpr VertexFormat a = vertexFormatWrap(0xdead);
    CORRADE_COMPARE(UnsignedInt(a), 0x8000dead);
}

void VertexFormatTest::wrapInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatWrap(0xdeadbeef);

    CORRADE_COMPARE(out.str(), "vertexFormatWrap(): implementation-specific value 0xdeadbeef already wrapped or too large\n");
}

void VertexFormatTest::unwrap() {
    constexpr UnsignedInt a = vertexFormatUnwrap(VertexFormat(0x8000dead));
    CORRADE_COMPARE(a, 0xdead);
}

void VertexFormatTest::unwrapInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatUnwrap(VertexFormat::Float);

    CORRADE_COMPARE(out.str(), "vertexFormatUnwrap(): VertexFormat::Float isn't a wrapped implementation-specific value\n");
}

void VertexFormatTest::size() {
    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Vector2), sizeof(Vector2));
    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Vector3), sizeof(Vector3));
    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Vector4), sizeof(Vector4));

    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Matrix2x3), sizeof(Matrix2x3));
    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Matrix4x3h), sizeof(Matrix4x3h));

    /* Aligned types */
    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Matrix2x2bNormalized), sizeof(Matrix2x2b));
    CORRADE_COMPARE(vertexFormatSize(VertexFormat::Matrix2x2bNormalizedAligned), sizeof(Matrix2x4b));
}

void VertexFormatTest::sizeInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatSize(VertexFormat{});
    vertexFormatSize(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatSize(): invalid format VertexFormat(0x0)\n"
        "vertexFormatSize(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::sizeImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormatSize(vertexFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(), "vertexFormatSize(): can't determine size of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::componentCount() {
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::UnsignedByteNormalized), 1);
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Vector2us), 2);
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Vector3bNormalized), 3);
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Vector4), 4);

    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Matrix4x3), 3);
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Matrix2x4sNormalized), 4);

    /* Aligned types return used component count, w/o padding */
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Matrix2x3sNormalized), 3);
    CORRADE_COMPARE(vertexFormatComponentCount(VertexFormat::Matrix2x3sNormalizedAligned), 3);
}

void VertexFormatTest::componentCountInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatComponentCount(VertexFormat{});
    vertexFormatComponentCount(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatComponentCount(): invalid format VertexFormat(0x0)\n"
        "vertexFormatComponentCount(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::componentCountImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormatComponentCount(vertexFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "vertexFormatComponentCount(): can't determine component count of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::componentFormat() {
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector4), VertexFormat::Float);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector3h), VertexFormat::Half);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector2d), VertexFormat::Double);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::UnsignedByte), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::UnsignedByteNormalized), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector3bNormalized), VertexFormat::Byte);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector2us), VertexFormat::UnsignedShort);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector2sNormalized), VertexFormat::Short);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector2ui), VertexFormat::UnsignedInt);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Vector3i), VertexFormat::Int);

    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix3x4), VertexFormat::Float);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix2x3h), VertexFormat::Half);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix2x2d), VertexFormat::Double);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix4x2bNormalized), VertexFormat::Byte);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix4x2bNormalizedAligned), VertexFormat::Byte);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix2x3sNormalized), VertexFormat::Short);
    CORRADE_COMPARE(vertexFormatComponentFormat(VertexFormat::Matrix2x3sNormalizedAligned), VertexFormat::Short);
}

void VertexFormatTest::componentFormatInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatComponentFormat(VertexFormat{});
    vertexFormatComponentFormat(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatComponentType(): invalid format VertexFormat(0x0)\n"
        "vertexFormatComponentType(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::componentFormatImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormatComponentFormat(vertexFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "vertexFormatComponentFormat(): can't determine component format of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::vectorCount() {
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::UnsignedByteNormalized), 1);
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Vector2us), 1);
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Vector3bNormalized), 1);
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Vector4), 1);

    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Matrix2x4sNormalized), 2);
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Matrix3x2bNormalized), 3);
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Matrix3x2bNormalizedAligned), 3);
    CORRADE_COMPARE(vertexFormatVectorCount(VertexFormat::Matrix4x3), 4);
}

void VertexFormatTest::vectorCountInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatVectorCount(VertexFormat{});
    vertexFormatVectorCount(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatVectorCount(): invalid format VertexFormat(0x0)\n"
        "vertexFormatVectorCount(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::vectorCountImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormatVectorCount(vertexFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "vertexFormatVectorCount(): can't determine vector count of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::vectorStride() {
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::UnsignedByteNormalized), 1);
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Vector3bNormalized), 3);
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Vector2us), 4);
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Vector4), 16);

    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Matrix2x4sNormalized), 8);
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Matrix4x3), 12);

    /* Aligned formats */
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Matrix3x2bNormalized), 2);
    CORRADE_COMPARE(vertexFormatVectorStride(VertexFormat::Matrix3x2bNormalizedAligned), 4);
}

void VertexFormatTest::vectorStrideInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vertexFormatVectorStride(VertexFormat{});
    vertexFormatVectorStride(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatVectorStride(): invalid format VertexFormat(0x0)\n"
        "vertexFormatVectorStride(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::vectorStrideImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormatVectorStride(vertexFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "vertexFormatVectorStride(): can't determine vector count of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::isNormalized() {
    CORRADE_VERIFY(isVertexFormatNormalized(VertexFormat::UnsignedByteNormalized));
    CORRADE_VERIFY(!isVertexFormatNormalized(VertexFormat::Vector2us));
    CORRADE_VERIFY(isVertexFormatNormalized(VertexFormat::Vector3bNormalized));
    CORRADE_VERIFY(!isVertexFormatNormalized(VertexFormat::Vector4));

    CORRADE_VERIFY(!isVertexFormatNormalized(VertexFormat::Matrix2x2h));
    CORRADE_VERIFY(isVertexFormatNormalized(VertexFormat::Matrix2x3bNormalized));
}

void VertexFormatTest::isNormalizedInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    isVertexFormatNormalized(VertexFormat{});
    isVertexFormatNormalized(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "isVertexFormatNormalized(): invalid format VertexFormat(0x0)\n"
        "isVertexFormatNormalized(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::isNormalizedImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    isVertexFormatNormalized(vertexFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "isVertexFormatNormalized(): can't determine normalization of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::assemble() {
    CORRADE_COMPARE(vertexFormat(VertexFormat::UnsignedShort, 3, true),
        VertexFormat::Vector3usNormalized);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Int, 4, false),
        VertexFormat::Vector4i);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Double, 1, false),
        VertexFormat::Double);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Byte, 1, true),
        VertexFormat::ByteNormalized);

    /* Non-scalar types allowed too, as that makes the internal checking
       much simpler than when requiring the type to be scalar non-normalized */
    CORRADE_COMPARE(vertexFormat(VertexFormat::Vector4bNormalized, 2, false),
        VertexFormat::Vector2b);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Vector3h, 2, false),
        VertexFormat::Vector2h);
}

void VertexFormatTest::assembleRoundtrip() {
    auto&& data = AssembleRoundtripData[testCaseInstanceId()];

    std::ostringstream out;
    {
        Debug d{&out, Debug::Flag::NoNewlineAtTheEnd};
        d << data.componentType;
        if(data.normalized) d << Debug::nospace << ", normalized";
    }
    setTestCaseDescription(out.str());

    VertexFormat result = vertexFormat(data.componentType, testCaseRepeatId() + 1, data.normalized);
    CORRADE_COMPARE(vertexFormat(result, testCaseRepeatId() + 1, data.normalized), result);
    CORRADE_COMPARE(vertexFormatComponentFormat(result), data.componentType);
    CORRADE_COMPARE(vertexFormatComponentCount(result), testCaseRepeatId() + 1);
    CORRADE_COMPARE(vertexFormatVectorCount(result), 1);
    CORRADE_COMPARE(vertexFormatVectorStride(result), vertexFormatSize(result));
    CORRADE_COMPARE(isVertexFormatNormalized(result), data.normalized);
}

void VertexFormatTest::assembleCantNormalize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(VertexFormat::Vector2, 1, true);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): VertexFormat::Vector2 can't be made normalized\n");
}

void VertexFormatTest::assembleInvalidComponentCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(VertexFormat::Vector3, 5, false);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): invalid component count 5\n");
}

void VertexFormatTest::assembleImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(vertexFormatWrap(0xdead), 1, true);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): can't assemble a format out of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::assembleMatrix() {
    CORRADE_COMPARE(vertexFormat(VertexFormat::Byte, 3, 2, false),
        VertexFormat::Matrix3x2bNormalized);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Short, 2, 3, true),
        VertexFormat::Matrix2x3sNormalizedAligned);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Float, 4, 2, true),
        VertexFormat::Matrix4x2);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Half, 2, 4, false),
        VertexFormat::Matrix2x4h);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Double, 4, 4, true),
        VertexFormat::Matrix4x4d);

    /* Non-scalar types allowed too, as that makes the internal checking
       much simpler than when requiring the type to be scalar non-normalized */
    CORRADE_COMPARE(vertexFormat(VertexFormat::Vector4bNormalized, 2, 2, false),
        VertexFormat::Matrix2x2bNormalized);
    CORRADE_COMPARE(vertexFormat(VertexFormat::Vector3h, 3, 3, true),
        VertexFormat::Matrix3x3hAligned);
}

void VertexFormatTest::assembleMatrixRoundtrip() {
    auto&& data = AssembleMatrixRoundtripData[testCaseInstanceId()];

    std::ostringstream out;
    {
        Debug d{&out, Debug::Flag::NoNewlineAtTheEnd};
        d << data.componentType << Debug::nospace << "," << data.componentCount;
        if(data.aligned) d << Debug::nospace << ", aligned";
    }
    setTestCaseDescription(out.str());

    VertexFormat result = vertexFormat(data.componentType, testCaseRepeatId() + 2, data.componentCount, data.aligned);
    CORRADE_COMPARE(vertexFormat(result, testCaseRepeatId() + 2, data.componentCount, data.aligned), result);
    CORRADE_COMPARE(vertexFormatComponentFormat(result), data.componentType);
    CORRADE_COMPARE(vertexFormatComponentCount(result), data.componentCount);
    CORRADE_COMPARE(vertexFormatVectorCount(result), testCaseRepeatId() + 2);
    CORRADE_COMPARE(vertexFormatVectorStride(result), vertexFormatSize(result)/(testCaseRepeatId() + 2));
    if(data.aligned)
        CORRADE_COMPARE_AS(vertexFormatVectorStride(result), 4, TestSuite::Compare::Divisible);
}

void VertexFormatTest::assembleMatrixInvalidType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(VertexFormat::UnsignedByte, 3, 2, false);
    vertexFormat(VertexFormat::UnsignedShort, 3, 2, false);
    vertexFormat(VertexFormat::UnsignedInt, 2, 3, false);
    vertexFormat(VertexFormat::Int, 2, 3, false);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): invalid matrix component type VertexFormat::UnsignedByte, only floating-point or 8-/16-bit signed integer types are supported\n"
        "vertexFormat(): invalid matrix component type VertexFormat::UnsignedShort, only floating-point or 8-/16-bit signed integer types are supported\n"
        "vertexFormat(): invalid matrix component type VertexFormat::UnsignedInt, only floating-point or 8-/16-bit signed integer types are supported\n"
        "vertexFormat(): invalid matrix component type VertexFormat::Int, only floating-point or 8-/16-bit signed integer types are supported\n");
}

void VertexFormatTest::assembleMatrixInvalidCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(VertexFormat::Vector3, 2, 1, false);
    vertexFormat(VertexFormat::Vector3, 2, 5, false);
    vertexFormat(VertexFormat::Vector3, 5, 2, false);
    vertexFormat(VertexFormat::Vector3, 1, 2, false);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): invalid component count 1\n"
        "vertexFormat(): invalid component count 5\n"
        "vertexFormat(): invalid vector count 5\n"
        "vertexFormat(): invalid vector count 1\n");
}

void VertexFormatTest::assembleMatrixImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(vertexFormatWrap(0xdead), 2, 2, true);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): can't assemble a format out of an implementation-specific format 0xdead\n");
}

void VertexFormatTest::debug() {
    std::ostringstream o;
    Debug(&o) << VertexFormat::Vector4 << VertexFormat(0xdead);
    CORRADE_COMPARE(o.str(), "VertexFormat::Vector4 VertexFormat(0xdead)\n");
}

void VertexFormatTest::debugImplementationSpecific() {
    std::ostringstream o;
    Debug(&o) << Magnum::vertexFormatWrap(0xdead);
    CORRADE_COMPARE(o.str(), "VertexFormat::ImplementationSpecific(0xdead)\n");
}

void VertexFormatTest::configuration() {
    Utility::Configuration c;

    c.setValue("format", VertexFormat::Vector3);
    CORRADE_COMPARE(c.value("format"), "Vector3");
    CORRADE_COMPARE(c.value<VertexFormat>("format"), VertexFormat::Vector3);

    c.setValue("zero", VertexFormat{});
    CORRADE_COMPARE(c.value("zero"), "");
    CORRADE_COMPARE(c.value<VertexFormat>("zero"), VertexFormat{});

    c.setValue("invalid", VertexFormat(0xdead));
    CORRADE_COMPARE(c.value("invalid"), "");
    CORRADE_COMPARE(c.value<VertexFormat>("invalid"), VertexFormat{});
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::VertexFormatTest)
