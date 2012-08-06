#ifndef Magnum_Math_Test_VectorTest_h
#define Magnum_Math_Test_VectorTest_h
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

class VectorTest: public Corrade::TestSuite::Tester<VectorTest> {
    public:
        VectorTest();

        void construct();
        void data();
        void copy();
        void dot();
        void multiplyDivide();
        void addSubstract();
        void dotSelf();
        void length();
        void normalized();
        void sum();
        void product();
        void angle();
        void negative();

        void debug();
};

}}}

#endif
