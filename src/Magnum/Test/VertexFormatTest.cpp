/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/VertexFormat.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Test { namespace {

struct VertexFormatTest: TestSuite::Tester {
    explicit VertexFormatTest();

    void mapping();

    void size();
    void sizeInvalid();
    void componentCount();
    void componentCountInvalid();
    void componentFormat();
    void componentFormatInvalid();
    void isNormalized();
    void isNormalizedInvalid();

    void assemble();
    void assembleRoundtrip();
    void assembleCantNormalize();
    void assembleInvalidComponentCount();

    void debug();
    void configuration();
};

constexpr struct {
    VertexFormat componentType;
    bool normalized;
} CombineRoundtripData[] {
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

VertexFormatTest::VertexFormatTest() {
    addTests({&VertexFormatTest::mapping,

              &VertexFormatTest::size,
              &VertexFormatTest::sizeInvalid,
              &VertexFormatTest::componentCount,
              &VertexFormatTest::componentCountInvalid,
              &VertexFormatTest::componentFormat,
              &VertexFormatTest::componentFormatInvalid,
              &VertexFormatTest::isNormalized,
              &VertexFormatTest::isNormalizedInvalid,

              &VertexFormatTest::assemble});

    addRepeatedInstancedTests({&VertexFormatTest::assembleRoundtrip}, 4,
        Containers::arraySize(CombineRoundtripData));

    addTests({&VertexFormatTest::assembleCantNormalize,
              &VertexFormatTest::assembleInvalidComponentCount,

              &VertexFormatTest::debug,
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

void VertexFormatTest::size() {
    CORRADE_COMPARE(Magnum::vertexFormatSize(VertexFormat::Vector2), sizeof(Vector2));
    CORRADE_COMPARE(Magnum::vertexFormatSize(VertexFormat::Vector3), sizeof(Vector3));
    CORRADE_COMPARE(Magnum::vertexFormatSize(VertexFormat::Vector4), sizeof(Vector4));
}

void VertexFormatTest::sizeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    Magnum::vertexFormatSize(VertexFormat{});
    Magnum::vertexFormatSize(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatSize(): invalid format VertexFormat(0x0)\n"
        "vertexFormatSize(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::componentCount() {
    CORRADE_COMPARE(Magnum::vertexFormatComponentCount(VertexFormat::UnsignedByteNormalized), 1);
    CORRADE_COMPARE(Magnum::vertexFormatComponentCount(VertexFormat::Vector2us), 2);
    CORRADE_COMPARE(Magnum::vertexFormatComponentCount(VertexFormat::Vector3bNormalized), 3);
    CORRADE_COMPARE(Magnum::vertexFormatComponentCount(VertexFormat::Vector4), 4);
}

void VertexFormatTest::componentCountInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    Magnum::vertexFormatComponentCount(VertexFormat{});
    Magnum::vertexFormatComponentCount(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatComponentCount(): invalid format VertexFormat(0x0)\n"
        "vertexFormatComponentCount(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::componentFormat() {
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector4), VertexFormat::Float);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector3h), VertexFormat::Half);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector2d), VertexFormat::Double);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::UnsignedByte), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::UnsignedByteNormalized), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector3bNormalized), VertexFormat::Byte);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector2us), VertexFormat::UnsignedShort);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector2sNormalized), VertexFormat::Short);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector2ui), VertexFormat::UnsignedInt);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(VertexFormat::Vector3i), VertexFormat::Int);
}

void VertexFormatTest::componentFormatInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    Magnum::vertexFormatComponentFormat(VertexFormat{});
    Magnum::vertexFormatComponentFormat(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "vertexFormatComponentType(): invalid format VertexFormat(0x0)\n"
        "vertexFormatComponentType(): invalid format VertexFormat(0xdead)\n");
}

void VertexFormatTest::isNormalized() {
    CORRADE_VERIFY(isVertexFormatNormalized(VertexFormat::UnsignedByteNormalized));
    CORRADE_VERIFY(!isVertexFormatNormalized(VertexFormat::Vector2us));
    CORRADE_VERIFY(isVertexFormatNormalized(VertexFormat::Vector3bNormalized));
    CORRADE_VERIFY(!isVertexFormatNormalized(VertexFormat::Vector4));
}

void VertexFormatTest::isNormalizedInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    isVertexFormatNormalized(VertexFormat{});
    isVertexFormatNormalized(VertexFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "isVertexFormatNormalized(): invalid format VertexFormat(0x0)\n"
        "isVertexFormatNormalized(): invalid format VertexFormat(0xdead)\n");
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
    auto&& data = CombineRoundtripData[testCaseInstanceId()];

    std::ostringstream out;
    {
        Debug d{&out, Debug::Flag::NoNewlineAtTheEnd};
        d << data.componentType;
        if(data.normalized) d << Debug::nospace << ", normalized";
    }
    setTestCaseDescription(out.str());

    VertexFormat result = vertexFormat(data.componentType, testCaseRepeatId() + 1, data.normalized);
    CORRADE_COMPARE(Magnum::vertexFormatComponentFormat(result), data.componentType);
    CORRADE_COMPARE(Magnum::vertexFormatComponentCount(result), testCaseRepeatId() + 1);
    CORRADE_COMPARE(isVertexFormatNormalized(result), data.normalized);
}

void VertexFormatTest::assembleCantNormalize() {
    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(VertexFormat::Vector2, 1, true);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): VertexFormat::Vector2 can't be made normalized\n");
}

void VertexFormatTest::assembleInvalidComponentCount() {
    std::ostringstream out;
    Error redirectError{&out};
    vertexFormat(VertexFormat::Vector3, 5, false);
    CORRADE_COMPARE(out.str(),
        "vertexFormat(): invalid component count 5\n");
}

void VertexFormatTest::debug() {
    std::ostringstream o;
    Debug(&o) << VertexFormat::Vector4 << VertexFormat(0xdead);
    CORRADE_COMPARE(o.str(), "VertexFormat::Vector4 VertexFormat(0xdead)\n");
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
