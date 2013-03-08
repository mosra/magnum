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

#include <TestSuite/Tester.h>

#include "MeshTools/Clean.h"

namespace Magnum { namespace MeshTools { namespace Test {

class CleanTest: public Corrade::TestSuite::Tester {
    public:
        CleanTest();

        void cleanMesh();
};

class Vector1 {
    public:
        static const std::size_t Size = 1;
        typedef Int Type;

        Vector1(): data(0) {}
        Vector1(Type i): data(i) {}
        Type operator[](std::size_t) const { return data; }
        Type& operator[](std::size_t) { return data; }
        bool operator==(Vector1 i) const { return i.data == data; }
        Vector1 operator-(Vector1 i) const { return data-i.data; }

    private:
        Type data;
};

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
