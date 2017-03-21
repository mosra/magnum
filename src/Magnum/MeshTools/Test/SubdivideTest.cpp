/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Subdivide.h"

namespace Magnum { namespace MeshTools { namespace Test {

struct SubdivideTest: TestSuite::Tester {
    explicit SubdivideTest();

    void wrongIndexCount();
    void subdivide();
};

namespace {

typedef Math::Vector<1, Int> Vector1;

inline Vector1 interpolator(Vector1 a, Vector1 b) { return (a[0]+b[0])/2; }

}

SubdivideTest::SubdivideTest() {
    addTests({&SubdivideTest::wrongIndexCount,
              &SubdivideTest::subdivide});
}

void SubdivideTest::wrongIndexCount() {
    std::stringstream ss;
    Error redirectError{&ss};

    std::vector<Vector1> positions;
    std::vector<UnsignedInt> indices{0, 1};
    MeshTools::subdivide(indices, positions, interpolator);
    CORRADE_COMPARE(ss.str(), "MeshTools::subdivide(): index count is not divisible by 3!\n");
}

void SubdivideTest::subdivide() {
    std::vector<Vector1> positions{0, 2, 6, 8};
    std::vector<UnsignedInt> indices{0, 1, 2, 1, 2, 3};
    MeshTools::subdivide(indices, positions, interpolator);

    CORRADE_COMPARE(indices.size(), 24);

    CORRADE_VERIFY(positions == (std::vector<Vector1>{0, 2, 6, 8, 1, 4, 3, 4, 7, 5}));
    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{4, 5, 6, 7, 8, 9, 0, 4, 6, 4, 1, 5, 6, 5, 2, 1, 7, 9, 7, 2, 8, 9, 8, 3}));
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::SubdivideTest)
