#ifndef Magnum_Math_Test_RectangularMatrixTest_h
#define Magnum_Math_Test_RectangularMatrixTest_h
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

namespace Magnum { namespace Math { namespace Test {

class RectangularMatrixTest: public Corrade::TestSuite::Tester<RectangularMatrixTest> {
    public:
        RectangularMatrixTest();

        void construct();
        void constructFromVectors();
        void constructZero();
        void data();

        void negative();
        void addSubtract();
        void multiplyDivide();
        void multiply();

        void transposed();

        void debug();
        void configuration();
};

}}}

#endif
