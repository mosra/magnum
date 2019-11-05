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

#include "Magnum/Mesh.h"

namespace Magnum { namespace Test { namespace {

struct MeshTest: TestSuite::Tester {
    explicit MeshTest();

    void primitiveMapping();
    void indexTypeMapping();

    void indexTypeSize();
    void indexTypeSizeInvalid();

    void debugPrimitive();
    void debugIndexType();
    void configurationPrimitive();
    void configurationIndexType();
};

MeshTest::MeshTest() {
    addTests({&MeshTest::primitiveMapping,
              &MeshTest::indexTypeMapping,

              &MeshTest::indexTypeSize,
              &MeshTest::indexTypeSizeInvalid,

              &MeshTest::debugPrimitive,
              &MeshTest::debugIndexType,
              &MeshTest::configurationPrimitive,
              &MeshTest::configurationIndexType});
}

void MeshTest::primitiveMapping() {
    /* This goes through the first 8 bits, which should be enough. */
    UnsignedInt firstUnhandled = 0xff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid primitive */
    for(UnsignedInt i = 1; i <= 0xff; ++i) {
        const auto primitive = MeshPrimitive(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range) */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(primitive) {
            #define _c(primitive) \
                case MeshPrimitive::primitive: \
                    CORRADE_COMPARE(Utility::ConfigurationValue<MeshPrimitive>::toString(MeshPrimitive::primitive, {}), #primitive); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xff); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Implementation/meshPrimitiveMapping.hpp"
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

    CORRADE_COMPARE(firstUnhandled, 0xff);
}

void MeshTest::indexTypeMapping() {
    /* This goes through the first 8 bits, which should be enough. */
    UnsignedInt firstUnhandled = 0xff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid type */
    for(UnsignedInt i = 1; i <= 0xff; ++i) {
        const auto type = MeshIndexType(i);
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
                case MeshIndexType::type: \
                    CORRADE_COMPARE(Utility::ConfigurationValue<MeshIndexType>::toString(MeshIndexType::type, {}), #type); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xff); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Implementation/meshIndexTypeMapping.hpp"
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

    CORRADE_COMPARE(firstUnhandled, 0xff);
}

void MeshTest::indexTypeSize() {
    CORRADE_COMPARE(meshIndexTypeSize(MeshIndexType::UnsignedByte), 1);
    CORRADE_COMPARE(meshIndexTypeSize(MeshIndexType::UnsignedShort), 2);
    CORRADE_COMPARE(meshIndexTypeSize(MeshIndexType::UnsignedInt), 4);
}

void MeshTest::indexTypeSizeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    meshIndexTypeSize(MeshIndexType{});
    meshIndexTypeSize(MeshIndexType(0xdead));

    CORRADE_COMPARE(out.str(),
        "meshIndexTypeSize(): invalid type MeshIndexType(0x0)\n"
        "meshIndexTypeSize(): invalid type MeshIndexType(0xdead)\n");
}

void MeshTest::debugPrimitive() {
    std::ostringstream o;
    Debug(&o) << MeshPrimitive::TriangleFan << MeshPrimitive(0xdead);
    CORRADE_COMPARE(o.str(), "MeshPrimitive::TriangleFan MeshPrimitive(0xdead)\n");
}

void MeshTest::debugIndexType() {
    std::ostringstream o;
    Debug(&o) << MeshIndexType::UnsignedShort << MeshIndexType(0xdead);
    CORRADE_COMPARE(o.str(), "MeshIndexType::UnsignedShort MeshIndexType(0xdead)\n");
}

void MeshTest::configurationPrimitive() {
    Utility::Configuration c;

    c.setValue("primitive", MeshPrimitive::LineStrip);
    CORRADE_COMPARE(c.value("primitive"), "LineStrip");
    CORRADE_COMPARE(c.value<MeshPrimitive>("primitive"), MeshPrimitive::LineStrip);

    c.setValue("zero", MeshPrimitive{});
    CORRADE_COMPARE(c.value("zero"), "");
    CORRADE_COMPARE(c.value<MeshPrimitive>("zero"), MeshPrimitive{});

    c.setValue("invalid", MeshPrimitive(0xdead));
    CORRADE_COMPARE(c.value("invalid"), "");
    CORRADE_COMPARE(c.value<MeshPrimitive>("invalid"), MeshPrimitive{});
}

void MeshTest::configurationIndexType() {
    Utility::Configuration c;

    c.setValue("type", MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(c.value("type"), "UnsignedShort");
    CORRADE_COMPARE(c.value<MeshIndexType>("type"), MeshIndexType::UnsignedShort);

    c.setValue("zero", MeshIndexType{});
    CORRADE_COMPARE(c.value("zero"), "");
    CORRADE_COMPARE(c.value<MeshIndexType>("zero"), MeshIndexType{});

    c.setValue("invalid", MeshIndexType(0xdead));
    CORRADE_COMPARE(c.value("invalid"), "");
    CORRADE_COMPARE(c.value<MeshIndexType>("invalid"), MeshIndexType{});
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::MeshTest)
