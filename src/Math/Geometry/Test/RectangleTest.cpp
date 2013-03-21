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

#include "Math/Geometry/Rectangle.h"

namespace Magnum { namespace Math { namespace Geometry { namespace Test {

class RectangleTest: public Corrade::TestSuite::Tester {
    public:
        RectangleTest();

        void construct();
        void constructDefault();
        void constructFromSize();
        void constructConversion();
        void constructCopy();

        void access();
        void compare();
        void size();

        void debug();
};

typedef Geometry::Rectangle<Float> Rectangle;
typedef Geometry::Rectangle<Int> Rectanglei;
typedef Vector2<Int> Vector2i;

RectangleTest::RectangleTest() {
    addTests({&RectangleTest::construct,
              &RectangleTest::constructDefault,
              &RectangleTest::constructFromSize,
              &RectangleTest::constructConversion,
              &RectangleTest::constructCopy,

              &RectangleTest::access,
              &RectangleTest::compare,
              &RectangleTest::size,

              &RectangleTest::debug});
}

void RectangleTest::construct() {
    constexpr Rectanglei a({3, 5}, {23, 78});
    CORRADE_COMPARE(a, Rectanglei({3, 5}, {23, 78}));
}

void RectangleTest::constructDefault() {
    constexpr Rectanglei a;
    CORRADE_COMPARE(a, Rectanglei({0, 0}, {0, 0}));
}

void RectangleTest::constructFromSize() {
    CORRADE_COMPARE(Rectanglei::fromSize({3, 5}, {23, 78}), Rectanglei({3, 5}, {26, 83}));
}

void RectangleTest::constructConversion() {
    constexpr Rectangle a({1.3f, 2.7f}, {-15.0f, 7.0f});
    constexpr Rectanglei b(a);
    CORRADE_COMPARE(b, Rectanglei({1, 2}, {-15, 7}));
}

void RectangleTest::constructCopy() {
    constexpr Rectanglei a({3, 5}, {23, 78});
    constexpr Rectanglei b(a);
    CORRADE_COMPARE(b, Rectanglei({3, 5}, {23, 78}));
}

void RectangleTest::access() {
    Rectanglei rect({34, 23}, {47, 30});
    constexpr Rectanglei crect({34, 23}, {47, 30});

    CORRADE_COMPARE(rect.bottomLeft(), Vector2i(34, 23));
    CORRADE_COMPARE(rect.topRight(), Vector2i(47, 30));
    CORRADE_COMPARE(rect.bottom(), 23);
    CORRADE_COMPARE(rect.top(), 30);
    CORRADE_COMPARE(rect.left(), 34);
    CORRADE_COMPARE(rect.right(), 47);
    CORRADE_COMPARE(rect.bottomLeft(), Vector2i(34, 23));
    CORRADE_COMPARE(rect.topRight(), Vector2i(47, 30));

    constexpr Int bottom = crect.bottom();
    constexpr Int top = crect.top();
    constexpr Int left = crect.left();
    constexpr Int right = crect.right();
    CORRADE_COMPARE(bottom, 23);
    CORRADE_COMPARE(top, 30);
    CORRADE_COMPARE(left, 34);
    CORRADE_COMPARE(right, 47);

    constexpr Vector2i bottomLeft = crect.bottomLeft();
    constexpr Vector2i topRight = crect.topRight();
    CORRADE_COMPARE(bottomLeft, Vector2i(34, 23));
    CORRADE_COMPARE(topRight, Vector2i(47, 30));

    CORRADE_COMPARE(rect.topLeft(), Vector2i(34, 30));
    CORRADE_COMPARE(rect.bottomRight(), Vector2i(47, 23));
}

void RectangleTest::compare() {
    CORRADE_VERIFY(Rectanglei({34, 23}, {47, 30}) == Rectanglei({34, 23}, {47, 30}));
    CORRADE_VERIFY(Rectanglei({34, 23}, {47, 30}) != Rectanglei({34, 23}, {48, 30}));
    CORRADE_VERIFY(Rectanglei({34, 23}, {47, 30}) != Rectanglei({35, 23}, {47, 30}));
}

void RectangleTest::size() {
    Rectanglei rect({34, 23}, {47, 30});

    CORRADE_COMPARE(rect.size(), Vector2i(13, 7));
    CORRADE_COMPARE(rect.width(), 13);
    CORRADE_COMPARE(rect.height(), 7);
}

void RectangleTest::debug() {
    std::ostringstream o;
    Debug(&o) << Rectanglei({34, 23}, {47, 30});

    CORRADE_COMPARE(o.str(), "Rectangle({34, 23}, {47, 30})\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Geometry::Test::RectangleTest)
