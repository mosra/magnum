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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Shaders/MeshVisualizer.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct MeshVisualizerTest: TestSuite::Tester {
    explicit MeshVisualizerTest();

    void constructNoCreate();
    void constructCopy();

    void debugFlag();
    void debugFlags();
};

MeshVisualizerTest::MeshVisualizerTest() {
    addTests({&MeshVisualizerTest::constructNoCreate,
              &MeshVisualizerTest::constructCopy,

              &MeshVisualizerTest::debugFlag,
              &MeshVisualizerTest::debugFlags});
}

void MeshVisualizerTest::constructNoCreate() {
    {
        MeshVisualizer shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void MeshVisualizerTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<MeshVisualizer, const MeshVisualizer&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshVisualizer, const MeshVisualizer&>{}));
}

void MeshVisualizerTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << MeshVisualizer::Flag::Wireframe << MeshVisualizer::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer::Flag::Wireframe Shaders::MeshVisualizer::Flag(0xf0)\n");
}

void MeshVisualizerTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (MeshVisualizer::Flag::Wireframe|MeshVisualizer::Flag::NoGeometryShader) << MeshVisualizer::Flags{};
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer::Flag::Wireframe|Shaders::MeshVisualizer::Flag::NoGeometryShader Shaders::MeshVisualizer::Flags{}\n");
    #else
    CORRADE_COMPARE(out.str(), "Shaders::MeshVisualizer::Flag::Wireframe Shaders::MeshVisualizer::Flags{}\n");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerTest)
