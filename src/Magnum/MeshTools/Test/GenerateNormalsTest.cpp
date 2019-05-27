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
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/GenerateNormals.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateNormalsTest: TestSuite::Tester {
    explicit GenerateNormalsTest();

    void flat();
    void flatWrongCount();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void flatDeprecated();
    #endif
    void flatIntoWrongSize();
};

GenerateNormalsTest::GenerateNormalsTest() {
    addTests({&GenerateNormalsTest::flat,
              &GenerateNormalsTest::flatWrongCount,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &GenerateNormalsTest::flatDeprecated,
              #endif
              &GenerateNormalsTest::flatIntoWrongSize});
}

/* Two vertices connected by one edge, each wound in another direction */
constexpr Vector3 TwoTriangles[]{
    {-1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},

    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}
};

void GenerateNormalsTest::flat() {
    CORRADE_COMPARE_AS(generateFlatNormals(TwoTriangles),
        (Containers::Array<Vector3>{Containers::InPlaceInit, {
            Vector3::zAxis(),
            Vector3::zAxis(),
            Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis()
        }}), TestSuite::Compare::Container);
}

void GenerateNormalsTest::flatWrongCount() {
    std::stringstream out;
    Error redirectError{&out};

    const Vector3 positions[7];
    generateFlatNormals(positions);
    CORRADE_COMPARE(out.str(), "MeshTools::generateFlatNormalsInto(): position count not divisible by 3\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void GenerateNormalsTest::flatDeprecated() {
    /* Two vertices connected by one edge, each wound in another direction */
    std::vector<UnsignedInt> indices;
    std::vector<Vector3> normals;
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1, 2,
        1, 2, 3
    }, {
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}
    });
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{
        0, 0, 0,
        1, 1, 1
    }));
    CORRADE_COMPARE(normals, (std::vector<Vector3>{
        Vector3::zAxis(),
        -Vector3::zAxis()
    }));
}
#endif

void GenerateNormalsTest::flatIntoWrongSize() {
    std::stringstream out;
    Error redirectError{&out};

    const Vector3 positions[6];
    Vector3 normals[7];
    generateFlatNormalsInto(positions, normals);
    CORRADE_COMPARE(out.str(), "MeshTools::generateFlatNormalsInto(): bad output size, expected 6 but got 7\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateNormalsTest)
