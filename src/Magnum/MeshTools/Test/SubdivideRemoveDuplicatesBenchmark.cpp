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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Subdivide.h"
#include "Magnum/Primitives/Icosphere.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct SubdivideRemoveDuplicatesBenchmark: TestSuite::Tester {
    explicit SubdivideRemoveDuplicatesBenchmark();

    void subdivide();
    void subdivideAndRemoveDuplicatesAfter();
    void subdivideAndRemoveDuplicatesAfterInPlace();
    void subdivideAndRemoveDuplicatesInBetween();
};

SubdivideRemoveDuplicatesBenchmark::SubdivideRemoveDuplicatesBenchmark() {
    addBenchmarks({&SubdivideRemoveDuplicatesBenchmark::subdivide,
                   &SubdivideRemoveDuplicatesBenchmark::subdivideAndRemoveDuplicatesAfter,
                   &SubdivideRemoveDuplicatesBenchmark::subdivideAndRemoveDuplicatesAfterInPlace,
                   &SubdivideRemoveDuplicatesBenchmark::subdivideAndRemoveDuplicatesInBetween}, 4);
}

namespace {
    static Vector3 interpolator(const Vector3& a, const Vector3& b) {
        return (a+b).normalized();
    }
}

void SubdivideRemoveDuplicatesBenchmark::subdivide() {
    CORRADE_BENCHMARK(3) {
        Trade::MeshData3D icosphere = Primitives::icosphereSolid(0);

        /* Subdivide 5 times */
        for(std::size_t i = 0; i != 5; ++i)
            MeshTools::subdivide(icosphere.indices(), icosphere.positions(0), interpolator);
    }
}

void SubdivideRemoveDuplicatesBenchmark::subdivideAndRemoveDuplicatesAfter() {
    CORRADE_BENCHMARK(3) {
        Trade::MeshData3D icosphere = Primitives::icosphereSolid(0);

        /* Subdivide 5 times */
        for(std::size_t i = 0; i != 5; ++i)
            MeshTools::subdivide(icosphere.indices(), icosphere.positions(0), interpolator);

        /* Remove duplicates after */
        icosphere.indices() = MeshTools::duplicate(icosphere.indices(), MeshTools::removeDuplicates(icosphere.positions(0)));
    }
}

void SubdivideRemoveDuplicatesBenchmark::subdivideAndRemoveDuplicatesAfterInPlace() {
    CORRADE_BENCHMARK(3) {
        /* Because that's what this thing does */
        Trade::MeshData3D icosphere = Primitives::icosphereSolid(5);
    }
}

void SubdivideRemoveDuplicatesBenchmark::subdivideAndRemoveDuplicatesInBetween() {
    CORRADE_BENCHMARK(3) {
        Trade::MeshData3D icosphere = Primitives::icosphereSolid(0);

        /* Subdivide 5 times and remove duplicates during the operation */
        for(std::size_t i = 0; i != 5; ++i) {
            MeshTools::subdivide(icosphere.indices(), icosphere.positions(0), interpolator);
            icosphere.indices() = MeshTools::duplicate(icosphere.indices(), MeshTools::removeDuplicates(icosphere.positions(0)));
        }
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::SubdivideRemoveDuplicatesBenchmark)

