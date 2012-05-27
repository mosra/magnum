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

#include "CombineIndexedArraysTest.h"

#include <sstream>
#include <QtTest/QTest>

#include "MeshTools/CombineIndexedArrays.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::CombineIndexedArraysTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

void CombineIndexedArraysTest::wrongIndexCount() {
    stringstream ss;
    Error::setOutput(&ss);
    vector<unsigned int> array;
    vector<unsigned int> result = MeshTools::combineIndexedArrays(
        tuple<const vector<unsigned int>&, vector<unsigned int>&>(vector<unsigned int>{0, 1, 0}, array),
        tuple<const vector<unsigned int>&, vector<unsigned int>&>(vector<unsigned int>{3, 4}, array));

    QVERIFY(result.size() == 0);
    QVERIFY(ss.str() == "MeshTools::combineIndexedArrays(): index arrays don't have the same length, nothing done.\n");
}

void CombineIndexedArraysTest::combine() {
    vector<unsigned int> array1{ 0, 1 };
    vector<unsigned int> array2{ 0, 1, 2, 3, 4 };
    vector<unsigned int> array3{ 0, 1, 2, 3, 4, 5, 6, 7 };

    vector<unsigned int> result = MeshTools::combineIndexedArrays(
        tuple<const vector<unsigned int>&, vector<unsigned int>&>(vector<unsigned int>{0, 1, 0}, array1),
        tuple<const vector<unsigned int>&, vector<unsigned int>&>(vector<unsigned int>{3, 4, 3}, array2),
        tuple<const vector<unsigned int>&, vector<unsigned int>&>(vector<unsigned int>{6, 7, 6}, array3));

    QVERIFY((result == vector<unsigned int>{0, 1, 0}));
    QVERIFY((array1 == vector<unsigned int>{0, 1}));
    QVERIFY((array2 == vector<unsigned int>{3, 4}));
    QVERIFY((array3 == vector<unsigned int>{6, 7}));
}


}}}
