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

#include "Magnum/Shaders/MeshVisualizer.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct MeshVisualizerTest: TestSuite::Tester {
    explicit MeshVisualizerTest();

    void constructNoCreate2D();
    void constructNoCreate3D();

    void constructCopy2D();
    void constructCopy3D();

    void vertexIndexSameAsObjectId();

    void debugFlag2D();
    void debugFlag3D();
    void debugFlags2D();
    void debugFlags3D();
};

MeshVisualizerTest::MeshVisualizerTest() {
    addTests({&MeshVisualizerTest::constructNoCreate2D,
              &MeshVisualizerTest::constructNoCreate3D,

              &MeshVisualizerTest::constructCopy2D,
              &MeshVisualizerTest::constructCopy3D,

              &MeshVisualizerTest::vertexIndexSameAsObjectId,

              &MeshVisualizerTest::debugFlag2D,
              &MeshVisualizerTest::debugFlag3D,
              &MeshVisualizerTest::debugFlags2D,
              &MeshVisualizerTest::debugFlags3D});
}

void MeshVisualizerTest::constructNoCreate2D() {
    {
        MeshVisualizer2D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void MeshVisualizerTest::constructNoCreate3D() {
    {
        MeshVisualizer3D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void MeshVisualizerTest::constructCopy2D() {
    CORRADE_VERIFY((std::is_constructible<MeshVisualizer2D, MeshVisualizer2D&&>{}));
    CORRADE_VERIFY(!(std::is_constructible<MeshVisualizer2D, const MeshVisualizer2D&>{}));

    CORRADE_VERIFY((std::is_assignable<MeshVisualizer2D, MeshVisualizer2D&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshVisualizer2D, const MeshVisualizer2D&>{}));
}

void MeshVisualizerTest::constructCopy3D() {
    CORRADE_VERIFY((std::is_constructible<MeshVisualizer3D, MeshVisualizer3D&&>{}));
    CORRADE_VERIFY(!(std::is_constructible<MeshVisualizer3D, const MeshVisualizer3D&>{}));

    CORRADE_VERIFY((std::is_assignable<MeshVisualizer3D, MeshVisualizer3D&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshVisualizer3D, const MeshVisualizer3D&>{}));
}

void MeshVisualizerTest::vertexIndexSameAsObjectId() {
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Object ID is not available on ES2.");
    #else
    CORRADE_COMPARE(MeshVisualizer2D::VertexIndex::Location, Generic2D::ObjectId::Location);
    CORRADE_COMPARE(MeshVisualizer3D::VertexIndex::Location, Generic3D::ObjectId::Location);
    #endif
}

void MeshVisualizerTest::debugFlag2D() {
    std::ostringstream out;

    Debug{&out} << MeshVisualizer2D::Flag::Wireframe << MeshVisualizer2D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer2D::Flag::Wireframe Shaders::MeshVisualizer2D::Flag(0xf0)\n");
}

void MeshVisualizerTest::debugFlag3D() {
    std::ostringstream out;

    Debug{&out} << MeshVisualizer3D::Flag::Wireframe << MeshVisualizer3D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer3D::Flag::Wireframe Shaders::MeshVisualizer3D::Flag(0xf0)\n");
}

void MeshVisualizerTest::debugFlags2D() {
    std::ostringstream out;

    Debug{&out} << (MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader) << MeshVisualizer2D::Flags{};
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer2D::Flag::Wireframe|Shaders::MeshVisualizer2D::Flag::NoGeometryShader Shaders::MeshVisualizer2D::Flags{}\n");
    #else
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer2D::Flag::Wireframe Shaders::MeshVisualizer2D::Flags{}\n");
    #endif
}

void MeshVisualizerTest::debugFlags3D() {
    std::ostringstream out;

    Debug{&out} << (MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader) << MeshVisualizer3D::Flags{};
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer3D::Flag::Wireframe|Shaders::MeshVisualizer3D::Flag::NoGeometryShader Shaders::MeshVisualizer3D::Flags{}\n");
    #else
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer3D::Flag::Wireframe Shaders::MeshVisualizer3D::Flags{}\n");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerTest)
