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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Vector2.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"

namespace Magnum { namespace MeshTools { namespace Test {

struct RemoveDuplicatesTest: TestSuite::Tester {
    explicit RemoveDuplicatesTest();

    void removeDuplicates();
};

RemoveDuplicatesTest::RemoveDuplicatesTest() {
    addTests({&RemoveDuplicatesTest::removeDuplicates});
}

void RemoveDuplicatesTest::removeDuplicates() {
    /* Numbers with distance 1 should be merged, numbers with distance 2 should
       be kept. Testing both even-odd and odd-even sequence to verify that
       half-epsilon translations are applied properly. */
    std::vector<Vector2i> data{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    const std::vector<UnsignedInt> indices = MeshTools::removeDuplicates(data, 2);
    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{0, 0, 1, 1}));
    CORRADE_COMPARE(data, (std::vector<Vector2i>{
        {1, 0},
        {0, 4}
    }));
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::RemoveDuplicatesTest)
