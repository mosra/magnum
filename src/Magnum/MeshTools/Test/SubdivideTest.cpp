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
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Subdivide.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct SubdivideTest: TestSuite::Tester {
    explicit SubdivideTest();

    void subdivide();
    void subdivideWrongIndexCount();
    template<class T> void subdivideInPlace();
    void subdivideInPlaceWrongIndexCount();
    void subdivideInPlaceSmallIndexType();

    /* this is additionally regression-tested in PrimitivesIcosphereTest */
};

typedef Math::Vector<1, Int> Vector1;

inline Vector1 interpolator(Vector1 a, Vector1 b) { return (a[0]+b[0])/2; }

SubdivideTest::SubdivideTest() {
    addTests({&SubdivideTest::subdivide,
              &SubdivideTest::subdivideWrongIndexCount,
              &SubdivideTest::subdivideInPlace<UnsignedByte>,
              &SubdivideTest::subdivideInPlace<UnsignedShort>,
              &SubdivideTest::subdivideInPlace<UnsignedInt>,
              &SubdivideTest::subdivideInPlaceWrongIndexCount,
              &SubdivideTest::subdivideInPlaceSmallIndexType});
}

void SubdivideTest::subdivide() {
    std::vector<Vector1> positions{0, 2, 6, 8};
    std::vector<UnsignedInt> indices{0, 1, 2, 1, 2, 3};
    MeshTools::subdivide(indices, positions, interpolator);

    CORRADE_COMPARE_AS(indices,
        (std::vector<UnsignedInt>{4, 5, 6, 7, 8, 9, 0, 4, 6, 4, 1, 5, 6, 5, 2, 1, 7, 9, 7, 2, 8, 9, 8, 3}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(positions,
        (std::vector<Vector1>{0, 2, 6, 8, 1, 4, 3, 4, 7, 5}),
        TestSuite::Compare::Container);
}

void SubdivideTest::subdivideWrongIndexCount() {
    std::stringstream out;
    Error redirectError{&out};

    std::vector<Vector1> positions;
    std::vector<UnsignedInt> indices{0, 1};
    MeshTools::subdivide(indices, positions, interpolator);
    CORRADE_COMPARE(out.str(), "MeshTools::subdivide(): index count is not divisible by 3\n");
}

template<class T> void SubdivideTest::subdivideInPlace() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[6*4]{0, 1, 2, 1, 2, 3, /* and 18 more */};
    Vector1 positions[4 + 6]{0, 2, 6, 8, /* and 6 more */};
    MeshTools::subdivideInPlace(Containers::stridedArrayView(indices),
        Containers::stridedArrayView(positions), interpolator);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({4, 5, 6, 7, 8, 9, 0, 4, 6, 4, 1, 5, 6, 5, 2, 1, 7, 9, 7, 2, 8, 9, 8, 3}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(positions),
        Containers::arrayView<Vector1>({0, 2, 6, 8, 1, 4, 3, 4, 7, 5}),
        TestSuite::Compare::Container);
}

void SubdivideTest::subdivideInPlaceWrongIndexCount() {
    std::stringstream out;
    Error redirectError{&out};

    UnsignedInt indices[6*4 + 1]{0, 1, 2, 1, 2, 3, /* and 18+1 more */};
    Vector1 positions[]{0};
    MeshTools::subdivideInPlace(Containers::stridedArrayView(indices),
        Containers::stridedArrayView(positions), interpolator);
    CORRADE_COMPARE(out.str(), "MeshTools::subdivideInPlace(): can't divide 25 indices to four parts with each having triangle faces\n");
}

void SubdivideTest::subdivideInPlaceSmallIndexType() {
    std::stringstream out;
    Error redirectError{&out};

    UnsignedByte indices[6*4]{0, 1, 2, 1, 2, 3, /* and 18 more */};
    Vector1 positions[256]{};
    MeshTools::subdivideInPlace(Containers::stridedArrayView(indices),
        Containers::stridedArrayView(positions), interpolator);
    CORRADE_COMPARE(out.str(), "MeshTools::subdivideInPlace(): a 1-byte index type is too small for 256 vertices\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::SubdivideTest)
