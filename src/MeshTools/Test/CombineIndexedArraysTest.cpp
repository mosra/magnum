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

#include "Types.h"
#include "MeshTools/CombineIndexedArrays.h"

namespace Magnum { namespace MeshTools { namespace Test {

class CombineIndexedArraysTest: public Corrade::TestSuite::Tester {
    public:
        CombineIndexedArraysTest();

        void wrongIndexCount();
        void combine();
};

CombineIndexedArraysTest::CombineIndexedArraysTest() {
    addTests({&CombineIndexedArraysTest::wrongIndexCount,
              &CombineIndexedArraysTest::combine});
}

void CombineIndexedArraysTest::wrongIndexCount() {
    std::stringstream ss;
    Error::setOutput(&ss);
    std::vector<UnsignedInt> array;
    std::vector<UnsignedInt> result = MeshTools::combineIndexedArrays(
        std::tuple<const std::vector<UnsignedInt>&, std::vector<UnsignedInt>&>(std::vector<UnsignedInt>{0, 1, 0}, array),
        std::tuple<const std::vector<UnsignedInt>&, std::vector<UnsignedInt>&>(std::vector<UnsignedInt>{3, 4}, array));

    CORRADE_COMPARE(result.size(), 0);
    CORRADE_COMPARE(ss.str(), "MeshTools::combineIndexedArrays(): index arrays don't have the same length, nothing done.\n");
}

void CombineIndexedArraysTest::combine() {
    std::vector<UnsignedInt> array1{ 0, 1 };
    std::vector<UnsignedInt> array2{ 0, 1, 2, 3, 4 };
    std::vector<UnsignedInt> array3{ 0, 1, 2, 3, 4, 5, 6, 7 };

    std::vector<UnsignedInt> result = MeshTools::combineIndexedArrays(
        std::tuple<const std::vector<UnsignedInt>&, std::vector<UnsignedInt>&>(std::vector<UnsignedInt>{0, 1, 0}, array1),
        std::tuple<const std::vector<UnsignedInt>&, std::vector<UnsignedInt>&>(std::vector<UnsignedInt>{3, 4, 3}, array2),
        std::tuple<const std::vector<UnsignedInt>&, std::vector<UnsignedInt>&>(std::vector<UnsignedInt>{6, 7, 6}, array3));

    CORRADE_COMPARE(result, (std::vector<UnsignedInt>{0, 1, 0}));
    CORRADE_COMPARE(array1, (std::vector<UnsignedInt>{0, 1}));
    CORRADE_COMPARE(array2, (std::vector<UnsignedInt>{3, 4}));
    CORRADE_COMPARE(array3, (std::vector<UnsignedInt>{6, 7}));
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CombineIndexedArraysTest)
