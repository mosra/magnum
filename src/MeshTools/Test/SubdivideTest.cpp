/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <sstream>
#include <TestSuite/Tester.h>

#include "Magnum.h"
#include "MeshTools/Clean.h"
#include "MeshTools/Subdivide.h"

namespace Magnum { namespace MeshTools { namespace Test {

class SubdivideTest: public Corrade::TestSuite::Tester {
    public:
        SubdivideTest();

        void wrongIndexCount();
        void subdivide();

    private:
        class Vector1 {
            public:
                static const std::size_t Size = 1;
                typedef std::int32_t Type;

                Vector1(): data(0) {}
                Vector1(Type i): data(i) {}
                Type operator[](std::size_t) const { return data; }
                Type& operator[](std::size_t) { return data; }
                bool operator==(Vector1 i) const { return i.data == data; }
                Vector1 operator-(Vector1 i) const { return data-i.data; }

            private:
                Type data;
        };

        inline static Vector1 interpolator(Vector1 a, Vector1 b) { return (a[0]+b[0])/2; }
};

SubdivideTest::SubdivideTest() {
    addTests(&SubdivideTest::wrongIndexCount,
             &SubdivideTest::subdivide);
}

void SubdivideTest::wrongIndexCount() {
    std::stringstream ss;
    Error::setOutput(&ss);

    std::vector<Vector1> positions;
    std::vector<std::uint32_t> indices{0, 1};
    MeshTools::subdivide(indices, positions, interpolator);
    CORRADE_COMPARE(ss.str(), "MeshTools::subdivide(): index count is not divisible by 3!\n");
}

void SubdivideTest::subdivide() {
    std::vector<Vector1> positions{0, 2, 6, 8};
    std::vector<std::uint32_t> indices{0, 1, 2, 1, 2, 3};
    MeshTools::subdivide(indices, positions, interpolator);

    CORRADE_COMPARE(indices.size(), 24);

    CORRADE_VERIFY(positions == (std::vector<Vector1>{0, 2, 6, 8, 1, 4, 3, 4, 7, 5}));
    CORRADE_COMPARE(indices, (std::vector<std::uint32_t>{4, 5, 6, 7, 8, 9, 0, 4, 6, 4, 1, 5, 6, 5, 2, 1, 7, 9, 7, 2, 8, 9, 8, 3}));

    MeshTools::clean(indices, positions);

    /* Positions 0, 1, 2, 3, 4, 5, 6, 7, 8 */
    CORRADE_COMPARE(positions.size(), 9);
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::SubdivideTest)
