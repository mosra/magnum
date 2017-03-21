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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Math/Range.h"

struct Dim {
    float offset, size;
};

struct Rect {
    float x, y, w, h;
};

struct Box {
    float x, y, z, w, h, d;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RangeConverter<1, Float, Dim> {
    constexpr static Range<1, Float> from(const Dim& other) {
        /* Doing it this way to preserve constexpr */
        return {other.offset, other.offset + other.size};
    }

    constexpr static Dim to(const Range<1, Float>& other) {
        return {other.min()[0],
                /* Doing it this way to preserve constexpr */
                other.max()[0] - other.min()[0]};
    }
};

template<> struct RangeConverter<2, Float, Rect> {
    constexpr static Range<2, Float> from(const Rect& other) {
        /* Doing it this way to preserve constexpr */
        return {{other.x, other.y}, {other.x + other.w, other.y + other.h}};
    }

    constexpr static Rect to(const Range<2, Float>& other) {
        return {other.min().x(), other.min().y(),
                /* Doing it this way to preserve constexpr */
                other.max().x() - other.min().x(),
                other.max().y() - other.min().y()};
    }
};

template<> struct RangeConverter<3, Float, Box> {
    constexpr static Range<3, Float> from(const Box& other) {
        return {{other.x, other.y, other.z},
                /* Doing it this way to preserve constexpr */
                {other.x + other.w,
                 other.y + other.h,
                 other.z + other.d}};
    }

    constexpr static Box to(const Range<3, Float>& other) {
        return {other.min().x(), other.min().y(), other.min().z(),
                /* Doing it this way to preserve constexpr */
                other.max().x() - other.min().x(),
                other.max().y() - other.min().y(),
                other.max().z() - other.min().z()};
    }
};

}

namespace Test {

struct RangeTest: Corrade::TestSuite::Tester {
    explicit RangeTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructFromSize();
    void constructConversion();
    void constructCopy();
    void convert();

    void access();
    void compare();
    void size();
    void center();

    void translated();
    void padded();
    void scaled();

    void contains();
    void join();

    void subclassTypes();
    void subclass();

    void debug();
    void configuration();
};

typedef Math::Range1D<Float> Range1D;
typedef Math::Range2D<Float> Range2D;
typedef Math::Range3D<Float> Range3D;
typedef Math::Range1D<Int> Range1Di;
typedef Math::Range2D<Int> Range2Di;
typedef Math::Range3D<Int> Range3Di;
typedef Vector2<Int> Vector2i;
typedef Vector3<Int> Vector3i;

RangeTest::RangeTest() {
    addTests({&RangeTest::construct,
              &RangeTest::constructDefault,
              &RangeTest::constructNoInit,
              &RangeTest::constructFromSize,
              &RangeTest::constructConversion,
              &RangeTest::constructCopy,
              &RangeTest::convert,

              &RangeTest::access,
              &RangeTest::compare,
              &RangeTest::size,
              &RangeTest::center,

              &RangeTest::translated,
              &RangeTest::padded,
              &RangeTest::scaled,

              &RangeTest::contains,
              &RangeTest::join,

              &RangeTest::subclassTypes,
              &RangeTest::subclass,

              &RangeTest::debug,
              &RangeTest::configuration});
}

void RangeTest::construct() {
    constexpr Range1Di a = {3, 23};
    constexpr Range2Di b = {{3, 5}, {23, 78}};
    constexpr Range3Di c = {{3, 5, -7}, {23, 78, 2}};

    CORRADE_COMPARE(a, (Range<1, Int>(3, 23)));
    CORRADE_COMPARE(b, (Range<2, Int>({3, 5}, {23, 78})));
    CORRADE_COMPARE(c, (Range<3, Int>({3, 5, -7}, {23, 78, 2})));

    CORRADE_VERIFY((std::is_nothrow_constructible<Range1Di, Int, Int>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range2Di, Vector2i, Vector2i>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range3Di, Vector3i, Vector3i>::value));
}

void RangeTest::constructDefault() {
    constexpr Range1Di a1;
    constexpr Range2Di b1;
    constexpr Range3Di c1;
    constexpr Range1Di a2{ZeroInit};
    constexpr Range2Di b2{ZeroInit};
    constexpr Range3Di c2{ZeroInit};

    CORRADE_COMPARE(a1, Range1Di(0, 0));
    CORRADE_COMPARE(a2, Range1Di(0, 0));
    CORRADE_COMPARE(b1, Range2Di({0, 0}, {0, 0}));
    CORRADE_COMPARE(b2, Range2Di({0, 0}, {0, 0}));
    CORRADE_COMPARE(c1, Range3Di({0, 0, 0}, {0, 0, 0}));
    CORRADE_COMPARE(c2, Range3Di({0, 0, 0}, {0, 0, 0}));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Range1Di>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Range2Di>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Range3Di>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Range1Di, ZeroInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range2Di, ZeroInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range3Di, ZeroInitT>::value));
}

void RangeTest::constructNoInit() {
    Range1Di a{3, 23};
    Range2Di b{{3, 5}, {23, 78}};
    Range3Di c{{3, 5, -7}, {23, 78, 2}};

    new(&a) Range1Di{NoInit};
    new(&b) Range2Di{NoInit};
    new(&c) Range3Di{NoInit};

    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Range1Di{3, 23}));
        CORRADE_COMPARE(b, (Range2Di{{3, 5}, {23, 78}}));
        CORRADE_COMPARE(c, (Range3Di{{3, 5, -7}, {23, 78, 2}}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Range1Di, NoInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range2Di, NoInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range3Di, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Range1Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Range2Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Range3Di>::value));
}

void RangeTest::constructFromSize() {
    CORRADE_COMPARE(Range1Di::fromSize(3, 23), Range1Di(3, 26));
    CORRADE_COMPARE(Range2Di::fromSize({3, 5}, {23, 78}), Range2Di({3, 5}, {26, 83}));
    CORRADE_COMPARE(Range3Di::fromSize({3, 5, -7}, {23, 78, 9}), Range3Di({3, 5, -7}, {26, 83, 2}));
}

void RangeTest::constructConversion() {
    constexpr Range1D a(1.3f, -15.0f);
    constexpr Range2D b({1.3f, 2.7f}, {-15.0f, 7.0f});
    constexpr Range3D c({1.3f, 2.7f, -1.5f}, {-15.0f, 7.0f, 0.3f});

    constexpr Range1Di d(a);
    CORRADE_COMPARE(d, Range1Di(1, -15));

    constexpr Range2Di e(b);
    CORRADE_COMPARE(e, Range2Di({1, 2}, {-15, 7}));

    constexpr Range3Di f(c);
    CORRADE_COMPARE(f, Range3Di({1, 2, -1}, {-15, 7, 0}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Range<2, Float>, Range<2, Int>>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range1D, Range1Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range2D, Range2Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range3D, Range3Di>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Range1D, Range1Di>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range2D, Range2Di>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range3D, Range3Di>::value));
}

void RangeTest::constructCopy() {
    constexpr Range1Di a(3, 23);
    constexpr Range2Di b({3, 5}, {23, 78});
    constexpr Range3Di c({3, 5, -7}, {23, 78, 2});

    constexpr Range1Di d(a);
    constexpr Range2Di e(b);
    constexpr Range3Di f(c);

    CORRADE_COMPARE(d, Range1Di(3, 23));
    CORRADE_COMPARE(e, Range2Di({3, 5}, {23, 78}));
    CORRADE_COMPARE(f, Range3Di({3, 5, -7}, {23, 78, 2}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Range1Di>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Range2Di>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Range3Di>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Range1Di>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Range2Di>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Range3Di>::value);
}

void RangeTest::convert() {
    /* It's position/size, not min/max */
    constexpr Dim a{1.5f, 3.5f};
    constexpr Rect b{1.5f, -2.0f, 3.5f, 0.5f};
    constexpr Box c{1.5f, -2.0f, -0.5f, 3.5f, 0.5f, 9.5f};
    constexpr Range1D d{1.5f, 5.0f};
    constexpr Range2D e{{1.5f, -2.0f}, {5.0f, -1.5f}};
    constexpr Range3D f{{1.5f, -2.0f, -0.5f}, {5.0f, -1.5f, 9.0f}};

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr Range<2, Float> g{b};
    constexpr Range1D h{a};
    constexpr Range2D i{b};
    constexpr Range3D j{c};
    CORRADE_COMPARE(g, e);
    CORRADE_COMPARE(h, d);
    CORRADE_COMPARE(i, e);
    CORRADE_COMPARE(j, f);

    constexpr Dim k(d);
    CORRADE_COMPARE(k.offset, a.offset);
    CORRADE_COMPARE(k.size, a.size);

    constexpr Rect l(e);
    CORRADE_COMPARE(l.x, b.x);
    CORRADE_COMPARE(l.y, b.y);
    CORRADE_COMPARE(l.w, b.w);
    CORRADE_COMPARE(l.h, b.h);

    constexpr Box m(f);
    CORRADE_COMPARE(m.x, c.x);
    CORRADE_COMPARE(m.y, c.y);
    CORRADE_COMPARE(m.z, c.z);
    CORRADE_COMPARE(m.w, c.w);
    CORRADE_COMPARE(m.h, c.h);
    CORRADE_COMPARE(m.d, c.d);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Rect, Range<2, Float>>::value));
    CORRADE_VERIFY(!(std::is_convertible<Dim, Range1D>::value));
    CORRADE_VERIFY(!(std::is_convertible<Rect, Range2D>::value));
    CORRADE_VERIFY(!(std::is_convertible<Box, Range3D>::value));

    CORRADE_VERIFY(!(std::is_convertible<Range1D, Dim>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range2D, Rect>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range3D, Box>::value));
}

void RangeTest::access() {
    Range1Di line(34, 47);
    Range2Di rect({34, 23}, {47, 30});
    Range3Di cube({34, 23, -17}, {47, 30, 12});

    constexpr Range1Di cline(34, 47);
    constexpr Range2Di crect({34, 23}, {47, 30});
    constexpr Range3Di ccube({34, 23, -17}, {47, 30, 12});

    CORRADE_COMPARE(line.min(), 34);
    CORRADE_COMPARE(cline.min(), 34);
    CORRADE_COMPARE(line.max(), 47);
    CORRADE_COMPARE(cline.max(), 47);

    CORRADE_COMPARE(rect.bottomLeft(), Vector2i(34, 23));
    CORRADE_COMPARE(rect.topRight(), Vector2i(47, 30));
    constexpr Vector2i bottomLeft = crect.bottomLeft();
    constexpr Vector2i topRight = crect.topRight();
    CORRADE_COMPARE(bottomLeft, Vector2i(34, 23));
    CORRADE_COMPARE(topRight, Vector2i(47, 30));

    CORRADE_COMPARE(rect.left(), 34);
    CORRADE_COMPARE(rect.right(), 47);
    CORRADE_COMPARE(rect.bottom(), 23);
    CORRADE_COMPARE(rect.top(), 30);
    constexpr Int left2 = crect.left();
    constexpr Int right2 = crect.right();
    constexpr Int bottom2 = crect.bottom();
    constexpr Int top2 = crect.top();
    CORRADE_COMPARE(left2, 34);
    CORRADE_COMPARE(right2, 47);
    CORRADE_COMPARE(bottom2, 23);
    CORRADE_COMPARE(top2, 30);

    CORRADE_COMPARE(cube.backBottomLeft(), Vector3i(34, 23, -17));
    CORRADE_COMPARE(cube.frontTopRight(), Vector3i(47, 30, 12));
    constexpr Vector3i backBottomLeft = ccube.backBottomLeft();
    constexpr Vector3i frontTopRight = ccube.frontTopRight();
    CORRADE_COMPARE(backBottomLeft, Vector3i(34, 23, -17));
    CORRADE_COMPARE(frontTopRight, Vector3i(47, 30, 12));

    CORRADE_COMPARE(cube.left(), 34);
    CORRADE_COMPARE(cube.right(), 47);
    CORRADE_COMPARE(cube.bottom(), 23);
    CORRADE_COMPARE(cube.top(), 30);
    CORRADE_COMPARE(cube.back(), -17);
    CORRADE_COMPARE(cube.front(), 12);
    constexpr Int left3 = ccube.left();
    constexpr Int right3 = ccube.right();
    constexpr Int bottom3 = ccube.bottom();
    constexpr Int top3 = ccube.top();
    constexpr Int back3 = ccube.back();
    constexpr Int front3 = ccube.front();
    CORRADE_COMPARE(left3, 34);
    CORRADE_COMPARE(right3, 47);
    CORRADE_COMPARE(bottom3, 23);
    CORRADE_COMPARE(top3, 30);
    CORRADE_COMPARE(back3, -17);
    CORRADE_COMPARE(front3, 12);

    CORRADE_COMPARE(rect.bottomRight(), Vector2i(47, 23));
    CORRADE_COMPARE(rect.topLeft(), Vector2i(34, 30));

    CORRADE_COMPARE(cube.backBottomRight(), Vector3i(47, 23, -17));
    CORRADE_COMPARE(cube.backTopLeft(), Vector3i(34, 30, -17));
    CORRADE_COMPARE(cube.backTopRight(), Vector3i(47, 30, -17));
    CORRADE_COMPARE(cube.frontBottomLeft(), Vector3i(34, 23, 12));
    CORRADE_COMPARE(cube.frontBottomRight(), Vector3i(47, 23, 12));
    CORRADE_COMPARE(cube.frontTopLeft(), Vector3i(34, 30, 12));
}

void RangeTest::compare() {
    CORRADE_VERIFY(Range2Di({34, 23}, {47, 30}) == Range2Di({34, 23}, {47, 30}));
    CORRADE_VERIFY(Range2Di({34, 23}, {47, 30}) != Range2Di({34, 23}, {48, 30}));
    CORRADE_VERIFY(Range2Di({34, 23}, {47, 30}) != Range2Di({35, 23}, {47, 30}));

    CORRADE_VERIFY(Range1D(1.0f, 1.0f) != Range1D(1.0f + TypeTraits<Float>::epsilon()*2, 1.0f));
    CORRADE_VERIFY(Range1D(1.0f, 1.0f) != Range1D(1.0f, 1.0f + TypeTraits<Float>::epsilon()*2));
    CORRADE_VERIFY(Range1D(1.0f, 1.0f) == Range1D(1.0f + TypeTraits<Float>::epsilon()/2.0f,
                                                  1.0f + TypeTraits<Float>::epsilon()/2.0f));
}

void RangeTest::size() {
    const Range1Di line(34, 47);
    const Range2Di rect({34, 23}, {47, 30});
    const Range3Di cube({34, 23, -17}, {47, 30, 12});

    CORRADE_COMPARE(line.size(), 13);
    CORRADE_COMPARE(rect.size(), Vector2i(13, 7));
    CORRADE_COMPARE(cube.size(), Vector3i(13, 7, 29));

    CORRADE_COMPARE(rect.sizeX(), 13);
    CORRADE_COMPARE(rect.sizeY(), 7);

    CORRADE_COMPARE(cube.sizeX(), 13);
    CORRADE_COMPARE(cube.sizeY(), 7);
    CORRADE_COMPARE(cube.sizeZ(), 29);
}

void RangeTest::center() {
    const Range1Di line(34, 47);
    const Range2Di rect({34, 23}, {47, 30});
    const Range3Di cube({34, 23, -17}, {47, 30, 12});

    CORRADE_COMPARE(line.center(), 40);
    CORRADE_COMPARE(rect.center(), Vector2i(40, 26));
    CORRADE_COMPARE(cube.center(), Vector3i(40, 26, -2));

    CORRADE_COMPARE(rect.centerX(), 40);
    CORRADE_COMPARE(rect.centerY(), 26);

    CORRADE_COMPARE(cube.centerX(), 40);
    CORRADE_COMPARE(cube.centerY(), 26);
    CORRADE_COMPARE(cube.centerZ(), -2);
}

void RangeTest::translated() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({17, 63}, {30, 70});

    CORRADE_COMPARE(a.translated({-17, 40}), b);
    CORRADE_COMPARE(a.size(), b.size());
}

void RangeTest::padded() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({31, 28}, {50, 25});

    CORRADE_COMPARE(a.padded({3, -5}), b);
    CORRADE_COMPARE(a.center(), b.center());
}

void RangeTest::scaled() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({68, -69}, {94, -90});

    CORRADE_COMPARE(a.scaled({2, -3}), b);
}

void RangeTest::contains() {
    Range2Di a({34, 23}, {47, 30});

    CORRADE_VERIFY(a.contains({40, 23}));
    CORRADE_VERIFY(!a.contains({33, 23}));
    CORRADE_VERIFY(!a.contains({40, 30}));
}

void RangeTest::join() {
    Range2Di a{{12, 20}, {15, 35}};
    Range2Di b{{10, 25}, {17, 105}};
    Range2Di c{{130, -15}, {130, -15}};
    Range2Di d{{10, 20}, {17, 105}};

    CORRADE_COMPARE(Math::join(a, b), d);
    CORRADE_COMPARE(Math::join(b, a), d);
    CORRADE_COMPARE(Math::join(a, c), a);
    CORRADE_COMPARE(Math::join(c, a), a);
}

template<class T> class BasicRect: public Math::Range<2, T> {
    public:
        template<class ...U> constexpr BasicRect(U&&... args): Math::Range<2, T>{args...} {}

        MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(2, BasicRect, Vector2)
};

typedef BasicRect<Int> Recti;

void RangeTest::subclassTypes() {
    const Vector2i a;
    CORRADE_VERIFY((std::is_same<decltype(Recti::fromSize(a, a)), Recti>::value));

    const Recti r;
    CORRADE_VERIFY((std::is_same<decltype(r.translated(a)), Recti>::value));
    CORRADE_VERIFY((std::is_same<decltype(r.padded(a)), Recti>::value));
    CORRADE_VERIFY((std::is_same<decltype(r.scaled(a)), Recti>::value));
}

void RangeTest::subclass() {
    /* Constexpr constructor */
    constexpr Recti a{Vector2i{34, 23}, Vector2i{47, 30}};
    CORRADE_COMPARE(a.min(), (Vector2i{34, 23}));

    CORRADE_COMPARE(Recti::fromSize({3, 5}, {23, 78}),
                    Recti(Vector2i{3, 5}, Vector2i{26, 83}));

    CORRADE_COMPARE(Recti(Vector2i{34, 23}, Vector2i{47, 30}).translated({-17, 40}),
                    Recti(Vector2i{17, 63}, Vector2i{30, 70}));
    CORRADE_COMPARE(Recti(Vector2i{34, 23}, Vector2i{47, 30}).padded({3, -5}),
                    Recti(Vector2i{31, 28}, Vector2i{50, 25}));
    CORRADE_COMPARE(Recti(Vector2i{34, 23}, Vector2i{47, 30}).scaled({2, -3}),
                    Recti(Vector2i{68, -69}, Vector2i{94, -90}));
}

void RangeTest::debug() {
    std::ostringstream o;
    Debug(&o) << Range2Di({34, 23}, {47, 30});

    CORRADE_COMPARE(o.str(), "Range({34, 23}, {47, 30})\n");
}

void RangeTest::configuration() {
    Corrade::Utility::Configuration c;

    Range2D rect({3.0f, 3.125f}, {9.0f, 9.55f});
    std::string value("3 3.125 9 9.55");

    c.setValue("rectangle", rect);
    CORRADE_COMPARE(c.value("rectangle"), value);
    CORRADE_COMPARE(c.value<Range2D>("rectangle"), rect);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RangeTest)
