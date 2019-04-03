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

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/GenerateFlatNormals.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateFlatNormalsTest: TestSuite::Tester {
    explicit GenerateFlatNormalsTest();

    void wrongIndexCount();
    void generate();
};

GenerateFlatNormalsTest::GenerateFlatNormalsTest() {
    addTests({&GenerateFlatNormalsTest::wrongIndexCount,
              &GenerateFlatNormalsTest::generate});
}

void GenerateFlatNormalsTest::wrongIndexCount() {
    std::stringstream ss;
    Error redirectError{&ss};
    std::vector<UnsignedInt> indices;
    std::vector<Vector3> normals;
    std::tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1
    }, {});

    CORRADE_COMPARE(indices.size(), 0);
    CORRADE_COMPARE(normals.size(), 0);
    CORRADE_COMPARE(ss.str(), "MeshTools::generateFlatNormals(): index count is not divisible by 3!\n");
}

void GenerateFlatNormalsTest::generate() {
    /* Two vertices connected by one edge, each winded in another direction */
    std::vector<UnsignedInt> indices;
    std::vector<Vector3> normals;
    std::tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1, 2,
        1, 2, 3
    }, {
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}
    });

    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{
        0, 0, 0,
        1, 1, 1
    }));
    CORRADE_COMPARE(normals, (std::vector<Vector3>{
        Vector3::zAxis(),
        -Vector3::zAxis()
    }));
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateFlatNormalsTest)
