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

    void debug();
    void configuration();
};

VertexFormatTest::VertexFormatTest() {
    addTests({&VertexFormatTest::mapping,

              &VertexFormatTest::size,
              &VertexFormatTest::sizeInvalid,

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
