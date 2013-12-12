/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
#include <TestSuite/Tester.h>
#include <Utility/Configuration.h>

#include "Mesh.h"

namespace Magnum { namespace Test {

class MeshTest: public TestSuite::Tester {
    public:
        MeshTest();

        void debugPrimitive();
        void debugIndexType();
        void configurationPrimitive();
        void configurationIndexType();
};

MeshTest::MeshTest() {
    addTests<MeshTest>({&MeshTest::debugPrimitive,
              &MeshTest::debugIndexType,
              &MeshTest::configurationPrimitive,
              &MeshTest::configurationIndexType});
}

void MeshTest::debugPrimitive() {
    std::ostringstream o;
    Debug(&o) << MeshPrimitive::TriangleFan;
    CORRADE_COMPARE(o.str(), "MeshPrimitive::TriangleFan\n");
}

void MeshTest::debugIndexType() {
    std::ostringstream o;
    Debug(&o) << Mesh::IndexType::UnsignedShort;
    CORRADE_COMPARE(o.str(), "Mesh::IndexType::UnsignedShort\n");
}

void MeshTest::configurationPrimitive() {
    Utility::Configuration c;

    c.setValue("primitive", MeshPrimitive::LineStrip);
    CORRADE_COMPARE(c.value("primitive"), "LineStrip");
    CORRADE_COMPARE(c.value<MeshPrimitive>("primitive"), MeshPrimitive::LineStrip);
}

void MeshTest::configurationIndexType() {
    Utility::Configuration c;

    c.setValue("type", Mesh::IndexType::UnsignedByte);
    CORRADE_COMPARE(c.value("type"), "UnsignedByte");
    CORRADE_COMPARE(c.value<Mesh::IndexType>("type"), Mesh::IndexType::UnsignedByte);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::MeshTest)
