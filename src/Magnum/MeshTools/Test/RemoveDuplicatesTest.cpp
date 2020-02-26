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

#include "Magnum/Math/Vector2.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct RemoveDuplicatesTest: TestSuite::Tester {
    explicit RemoveDuplicatesTest();

    void removeDuplicatesInPlace();
    void removeDuplicatesStl();
    template<class T> void removeDuplicatesIndexedInPlace();
    void removeDuplicatesIndexedInPlaceSmallType();
    void removeDuplicatesIndexedInPlaceEmptyIndices();
    void removeDuplicatesIndexedInPlaceEmptyIndicesVertices();

    /* this is additionally regression-tested in PrimitivesIcosphereTest */
};

RemoveDuplicatesTest::RemoveDuplicatesTest() {
    addTests({&RemoveDuplicatesTest::removeDuplicatesInPlace,
              &RemoveDuplicatesTest::removeDuplicatesStl,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlace<UnsignedByte>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlace<UnsignedShort>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlace<UnsignedInt>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceSmallType,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndices,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndicesVertices});
}

void RemoveDuplicatesTest::removeDuplicatesInPlace() {
    /* Numbers with distance 1 should be merged, numbers with distance 2 should
       be kept. Testing both even-odd and odd-even sequence to verify that
       half-epsilon translations are applied properly. */
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::pair<Containers::Array<UnsignedInt>, std::size_t> result = MeshTools::removeDuplicatesInPlace(Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(result.first),
        Containers::arrayView<UnsignedInt>({0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result.second),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesStl() {
    /* Same but with implicit bloat. HEH HEH */
    std::vector<Vector2i> data{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    const std::vector<UnsignedInt> indices = MeshTools::removeDuplicates(data, 2);
    CORRADE_COMPARE_AS(indices,
        (std::vector<UnsignedInt>{0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data,
        (std::vector<Vector2i>{{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

template<class T> void RemoveDuplicatesTest::removeDuplicatesIndexedInPlace() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Same as above, but with an explicit index buffer */
    T indices[]{3, 2, 0, 1, 2, 3};
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::size_t count = MeshTools::removeDuplicatesIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({1, 1, 0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceSmallType() {
    std::stringstream out;
    Error redirectError{&out};

    UnsignedByte indices[1];
    Vector2i data[256]{};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE(out.str(), "MeshTools::removeDuplicatesIndexedInPlace(): a 1-byte index type is too small for 256 vertices\n");
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndices() {
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::size_t count = MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView1D<UnsignedInt>{},
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndicesVertices() {
    CORRADE_COMPARE((MeshTools::removeDuplicatesIndexedInPlace<UnsignedInt, Vector2i>({}, {}, 2)), 0);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::RemoveDuplicatesTest)
