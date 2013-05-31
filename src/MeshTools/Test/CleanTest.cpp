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

#include <TestSuite/Tester.h>

#include "MeshTools/Clean.h"

namespace Magnum { namespace MeshTools { namespace Test {

class CleanTest: public TestSuite::Tester {
    public:
        CleanTest();

        void cleanMesh();
};

typedef Math::Vector<1, int> Vector1;

CleanTest::CleanTest() {
    addTests({&CleanTest::cleanMesh});
}

void CleanTest::cleanMesh() {
    std::vector<Vector1> positions{1, 2, 1, 4};
    std::vector<UnsignedInt> indices{0, 1, 2, 1, 2, 3};
    MeshTools::clean(indices, positions);

    /* Verify cleanup */
    CORRADE_VERIFY(positions == (std::vector<Vector1>{1, 2, 4}));
    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{0, 1, 0, 1, 0, 2}));
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CleanTest)
