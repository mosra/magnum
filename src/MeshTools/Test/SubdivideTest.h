#ifndef Magnum_MeshTools_Test_SubdivideTest_h
#define Magnum_MeshTools_Test_SubdivideTest_h
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

}}}

#endif
