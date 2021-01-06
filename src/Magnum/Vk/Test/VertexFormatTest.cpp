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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/VertexFormat.h"
#include "Magnum/Vk/VertexFormat.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct VertexFormatTest: TestSuite::Tester {
    explicit VertexFormatTest();

    void map();
    void mapImplementationSpecific();
    void mapUnsupported();
    void mapInvalid();

    void debug();
};

VertexFormatTest::VertexFormatTest() {
    addTests({&VertexFormatTest::map,
              &VertexFormatTest::mapImplementationSpecific,
              &VertexFormatTest::mapUnsupported,
              &VertexFormatTest::mapInvalid,

              &VertexFormatTest::debug});
}

void VertexFormatTest::map() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasVertexFormat(Magnum::VertexFormat::Vector3us));
    CORRADE_COMPARE(vertexFormat(Magnum::VertexFormat::Vector3us), VertexFormat::Vector3us);
    CORRADE_COMPARE(vertexFormat(Magnum::VertexFormat::Matrix2x3bNormalizedAligned), VertexFormat::Vector3bNormalized);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::VertexFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular generic format maps to a particular format
           - that the debug output matches what was converted
        */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(input, output) \
                case Magnum::VertexFormat::input: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasVertexFormat(Magnum::VertexFormat::input)); \
                    CORRADE_COMPARE(vertexFormat(Magnum::VertexFormat::input), VertexFormat::output); \
                    { \
                        std::ostringstream out; \
                        Debug{&out} << vertexFormat(Magnum::VertexFormat::input); \
                        CORRADE_COMPARE(out.str(), "Vk::VertexFormat::" #output "\n"); \
                    } \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::VertexFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasVertexFormat(Magnum::VertexFormat::format)); \
                    std::ostringstream out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        vertexFormat(Magnum::VertexFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out.str(); \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/vertexFormatMapping.hpp"
            #undef _s
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

void VertexFormatTest::mapImplementationSpecific() {
    CORRADE_VERIFY(hasVertexFormat(Magnum::vertexFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)));
    CORRADE_COMPARE(vertexFormat(Magnum::vertexFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)),
        VertexFormat(VK_FORMAT_A8B8G8R8_SINT_PACK32));
}

void VertexFormatTest::mapUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #if 1
    CORRADE_SKIP("All vertex formats are supported.");
    #else
    std::ostringstream out;
    Error redirectError{&out};

    vertexFormat(Magnum::VertexFormat::Vector3d);
    CORRADE_COMPARE(out.str(), "Vk::vertexFormat(): unsupported format VertexFormat::Vector3d\n");
    #endif
}

void VertexFormatTest::mapInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasVertexFormat(Magnum::VertexFormat{});
    hasVertexFormat(Magnum::VertexFormat(0x123));
    vertexFormat(Magnum::VertexFormat{});
    vertexFormat(Magnum::VertexFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVertexFormat(): invalid format VertexFormat(0x0)\n"
        "Vk::hasVertexFormat(): invalid format VertexFormat(0x123)\n"
        "Vk::vertexFormat(): invalid format VertexFormat(0x0)\n"
        "Vk::vertexFormat(): invalid format VertexFormat(0x123)\n");
}

void VertexFormatTest::debug() {
    std::ostringstream out;
    Debug{&out} << VertexFormat::Vector2usNormalized << VertexFormat(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::VertexFormat::Vector2usNormalized Vk::VertexFormat(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::VertexFormatTest)
