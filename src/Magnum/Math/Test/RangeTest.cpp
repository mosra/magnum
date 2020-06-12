/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/StrictWeakOrdering.h"

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
        return {other.min(),
                /* Doing it this way to preserve constexpr */
                other.max() - other.min()};
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

namespace Test { namespace {

struct RangeTest: Corrade::TestSuite::Tester {
    explicit RangeTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructFromSize();
    void constructFromCenter();
    void constructPair();
    void constructConversion();
    void constructCopy();
    void convert();

    void access();
    void compare();
    void dimensionSlice();
    void size();
    void center();

    /* Testing 1D separately because it's a scalar, not vector. The above
       functions test all dimensions explicitly. */

    void translated();
    void translated1D();
    void padded();
    void padded1D();
    void scaled();
    void scaled1D();
    void scaledFromCenter();
    void scaledFromCenter1D();

    void containsPoint();
    void containsPoint1D();
    void containsRange();
    void containsRange1D();
    void intersectIntersects();
    void intersectIntersects1D();
    void join();
    void join1D();

    void strictWeakOrdering();

    void subclassTypes();
    void subclass();

    void debug();
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
              &RangeTest::constructFromCenter,
              &RangeTest::constructPair,
              &RangeTest::constructConversion,
              &RangeTest::constructCopy,
              &RangeTest::convert,

              &RangeTest::access,
              &RangeTest::compare,
              &RangeTest::dimensionSlice,
              &RangeTest::size,
              &RangeTest::center,

              &RangeTest::translated,
              &RangeTest::translated1D,
              &RangeTest::padded,
              &RangeTest::padded1D,
              &RangeTest::scaled,
              &RangeTest::scaled1D,
              &RangeTest::scaledFromCenter,
              &RangeTest::scaledFromCenter1D,

              &RangeTest::containsPoint,
              &RangeTest::containsPoint1D,
              &RangeTest::containsRange,
              &RangeTest::containsRange1D,
              &RangeTest::intersectIntersects,
              &RangeTest::intersectIntersects1D,
              &RangeTest::join,
              &RangeTest::join1D,

              &RangeTest::strictWeakOrdering,

              &RangeTest::subclassTypes,
              &RangeTest::subclass,

              &RangeTest::debug});
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

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Range1Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Range2Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Range3Di>::value));
}

void RangeTest::constructNoInit() {
    Range1Di a{3, 23};
    Range2Di b{{3, 5}, {23, 78}};
    Range3Di c{{3, 5, -7}, {23, 78, 2}};

    new(&a) Range1Di{Magnum::NoInit};
    new(&b) Range2Di{Magnum::NoInit};
    new(&c) Range3Di{Magnum::NoInit};

    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Range1Di{3, 23}));
        CORRADE_COMPARE(b, (Range2Di{{3, 5}, {23, 78}}));
        CORRADE_COMPARE(c, (Range3Di{{3, 5, -7}, {23, 78, 2}}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Range1Di, Magnum::NoInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range2Di, Magnum::NoInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Range3Di, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Range1Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Range2Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Range3Di>::value));
}

void RangeTest::constructFromSize() {
    CORRADE_COMPARE(Range1Di::fromSize(3, 23), Range1Di(3, 26));
    CORRADE_COMPARE(Range2Di::fromSize({3, 5}, {23, 78}), Range2Di({3, 5}, {26, 83}));
    CORRADE_COMPARE(Range3Di::fromSize({3, 5, -7}, {23, 78, 9}), Range3Di({3, 5, -7}, {26, 83, 2}));
}

void RangeTest::constructFromCenter() {
    CORRADE_COMPARE(Range1Di::fromCenter(15, 3), (Range1Di{12, 18}));
    CORRADE_COMPARE(Range2Di::fromCenter({15, 5}, {3, 10}), (Range2Di{{12, -5}, {18, 15}}));
    CORRADE_COMPARE(Range3Di::fromCenter({15, 5, -7}, {3, 10, 9}), (Range3Di{{12, -5, -16}, {18, 15, 2}}));
}

void RangeTest::constructPair() {
    Vector2i a{10, 22};
    Vector2i b{30, 18};
    Vector2i c{20, 25};

    /* Conversion should be implicit, so not using {} */
    Range1Di bounds1a = Math::minmax({a.x(), b.x(), c.x()});
    Range1Di bounds1c{10, 30};
    CORRADE_COMPARE(bounds1a, bounds1c);

    Range2Di bounds2a = Math::minmax({a, b, c});
    Range2Di bounds2b = std::pair<Math::Vector<2, Int>, Math::Vector<2, Int>>{{10, 18}, {30, 25}};
    Range2Di bounds2c{{10, 18}, {30, 25}};
    CORRADE_COMPARE(bounds2a, bounds2c);
    CORRADE_COMPARE(bounds2b, bounds2c);

    Vector3i a3{a, 122};
    Vector3i b3{b, 122};
    Vector3i c3{c, 123};

    Range3Di bounds3a = Math::minmax({a3, b3, c3});
    Range3Di bounds3b = std::pair<Math::Vector<3, Int>, Math::Vector<3, Int>>{{10, 18, 122}, {30, 25, 123}};
    Range3Di bounds3c{{10, 18, 122}, {30, 25, 123}};
    CORRADE_COMPARE(bounds3a, bounds3c);
    CORRADE_COMPARE(bounds3b, bounds3c);
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

    CORRADE_COMPARE(line.data(), static_cast<void*>(&line));
    CORRADE_COMPARE(rect.data(), static_cast<void*>(&rect));
    CORRADE_COMPARE(cube.data(), static_cast<void*>(&cube));
    constexpr Int lineData = *cline.data();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing a pointer is verboten */
    constexpr
    #endif
    Int rectData = *crect.data();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing a pointer is verboten */
    constexpr
    #endif
    Int cubeData = *ccube.data();
    CORRADE_COMPARE(lineData, 34);
    CORRADE_COMPARE(rectData, 34);
    CORRADE_COMPARE(cubeData, 34);

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

void RangeTest::dimensionSlice() {
    constexpr Range1Di lineX{34, 47};
    constexpr Range1Di lineY{23, 30};
    constexpr Range1Di lineZ{-17, 12};
    constexpr Range2Di rect{{34, 23}, {47, 30}};
    constexpr Range3Di cube{{34, 23, -17}, {47, 30, 12}};

    constexpr Range1Di x2 = rect.x();
    constexpr Range1Di x3 = cube.x();
    CORRADE_COMPARE(x2, lineX);
    CORRADE_COMPARE(x3, lineX);
    CORRADE_COMPARE(rect.x(), lineX);
    CORRADE_COMPARE(cube.x(), lineX);

    constexpr Range1Di y2 = rect.y();
    constexpr Range1Di y3 = cube.y();
    CORRADE_COMPARE(y2, lineY);
    CORRADE_COMPARE(y3, lineY);
    CORRADE_COMPARE(rect.y(), lineY);
    CORRADE_COMPARE(cube.y(), lineY);

    constexpr Range1Di z = cube.z();
    CORRADE_COMPARE(z, lineZ);
    CORRADE_COMPARE(cube.z(), lineZ);

    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No clue. Also, IDGAF. */
    #endif
    Range2Di xy = cube.xy();
    CORRADE_COMPARE(xy, rect);
    CORRADE_COMPARE(cube.xy(), rect);
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

void RangeTest::translated1D() {
    Range1Di a(34, 47);
    Range1Di b(17, 30);

    CORRADE_COMPARE(a.translated(-17), b);
    CORRADE_COMPARE(a.size(), b.size());
}

void RangeTest::padded() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({31, 28}, {50, 25});

    CORRADE_COMPARE(a.padded({3, -5}), b);
    CORRADE_COMPARE(a.center(), b.center());
}

void RangeTest::padded1D() {
    Range1Di a{34, 47};
    Range1Di b{31, 50};

    CORRADE_COMPARE(a.padded(3), b);
    CORRADE_COMPARE(a.center(), b.center());
}

void RangeTest::scaled() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({68, -69}, {94, -90});

    CORRADE_COMPARE(a.scaled({2, -3}), b);
    CORRADE_COMPARE((a.size()*Vector2i{2, -3}), b.size());
}

void RangeTest::scaled1D() {
    Range1Di a{34, 47};
    Range1Di b{68, 94};

    CORRADE_COMPARE(a.scaled(2), b);
    CORRADE_COMPARE(a.size()*2, b.size());
}

void RangeTest::scaledFromCenter() {
    Range2Di a{{34, 22}, {48, 30}};
    Range2Di b{{27, 38}, {55, 14}};

    CORRADE_COMPARE(a.scaledFromCenter({2, -3}), b);
    CORRADE_COMPARE(a.center(), b.center());
    CORRADE_COMPARE((a.size()*Vector2i{2, -3}), b.size());
}

void RangeTest::scaledFromCenter1D() {
    Range1Di a{34, 48};
    Range1Di b{27, 55};

    CORRADE_COMPARE(a.scaledFromCenter(2), b);
    CORRADE_COMPARE(a.center(), b.center());
    CORRADE_COMPARE(a.size()*2, b.size());
}

void RangeTest::containsPoint() {
    Range2Di a({34, 23}, {47, 30});

    CORRADE_VERIFY(a.contains({40, 23}));
    CORRADE_VERIFY(!a.contains({33, 23}));
    CORRADE_VERIFY(!a.contains({40, 30}));

    /* Contains a point at min, but not at max */
    CORRADE_VERIFY(a.contains({34, 23}));
    CORRADE_VERIFY(!a.contains({47, 30}));
}

void RangeTest::containsPoint1D() {
    Range1Di a{34, 47};

    CORRADE_VERIFY(a.contains(40));
    CORRADE_VERIFY(!a.contains(33));

    /* Contains a point at min, but not at max */
    CORRADE_VERIFY(a.contains(34));
    CORRADE_VERIFY(!a.contains(47));
}

void RangeTest::containsRange() {
    Range2Di a({34, 23}, {47, 30});

    /* Contains whole range with a gap, not the other way around */
    Range2Di b{{35, 25}, {40, 28}};
    CORRADE_VERIFY(a.contains(b));
    CORRADE_VERIFY(!b.contains(a));

    /* Contains itself, empty range contains itself as well */
    Range2Di c;
    CORRADE_VERIFY(a.contains(a));
    CORRADE_VERIFY(b.contains(b));
    CORRADE_VERIFY(c.contains(c));

    /* Contains zero-sized range inside but not outside */
    Range2Di d{{34, 23}, {34, 23}};
    Range2Di e{{33, 23}, {33, 23}};
    Range2Di f{{47, 30}, {47, 30}};
    Range2Di g{{47, 31}, {47, 31}};
    CORRADE_VERIFY(a.contains(d));
    CORRADE_VERIFY(!a.contains(e));
    CORRADE_VERIFY(a.contains(f));
    CORRADE_VERIFY(!a.contains(g));

    /* Doesn't contain a range that overlaps */
    Range2Di h{{30, 25}, {35, 105}};
    CORRADE_VERIFY(!a.contains(h));
    CORRADE_VERIFY(!h.contains(a));

    /* Doesn't contain a range touching the edges from the outside */
    Range2Di i{{20, 30}, {34, 40}};
    Range2Di j{{47, 20}, {60, 23}};
    CORRADE_VERIFY(!a.contains(i));
    CORRADE_VERIFY(!a.contains(j));
}

void RangeTest::containsRange1D() {
    Range1Di a{34, 47};

    /* Contains whole range with a gap, not the other way around */
    Range1Di b{35, 40};
    CORRADE_VERIFY(a.contains(b));
    CORRADE_VERIFY(!b.contains(a));

    /* Contains itself, empty range contains itself as well */
    Range1Di c;
    CORRADE_VERIFY(a.contains(a));
    CORRADE_VERIFY(b.contains(b));
    CORRADE_VERIFY(c.contains(c));

    /* Contains zero-sized range inside but not outside */
    Range1Di d{34, 34};
    Range1Di e{33, 33};
    Range1Di f{47, 47};
    Range1Di g{48, 48};
    CORRADE_VERIFY(a.contains(d));
    CORRADE_VERIFY(!a.contains(e));
    CORRADE_VERIFY(a.contains(f));
    CORRADE_VERIFY(!a.contains(g));

    /* Doesn't contain a range that overlaps */
    Range1Di h{30, 35};
    CORRADE_VERIFY(!a.contains(h));
    CORRADE_VERIFY(!h.contains(a));

    /* Doesn't contain a range touching the edges from the outside */
    Range1Di i{20, 34};
    Range1Di j{47, 60};
    CORRADE_VERIFY(!a.contains(i));
    CORRADE_VERIFY(!a.contains(j));
}

void RangeTest::intersectIntersects() {
    Range2Di a({34, 23}, {47, 30});

    /* Intersects itself */
    CORRADE_VERIFY(Math::intersects(a, a));
    CORRADE_COMPARE(Math::intersect(a, a), a);

    /* Non-empty intersection */
    Range2Di b{{30, 25}, {35, 105}};
    Range2Di c{{34, 25}, {35, 30}};
    CORRADE_VERIFY(Math::intersects(a, b));
    CORRADE_VERIFY(Math::intersects(b, a));
    CORRADE_COMPARE(Math::intersect(a, b), c);
    CORRADE_COMPARE(Math::intersect(b, a), c);

    /* Intersecting with an empty range outside produces a default-constructed range */
    Range2Di d{{130, -15}, {130, -15}};
    CORRADE_VERIFY(!Math::intersects(a, d));
    CORRADE_VERIFY(!Math::intersects(d, a));
    CORRADE_COMPARE(Math::intersect(a, d), Range2Di{});
    CORRADE_COMPARE(Math::intersect(d, a), Range2Di{});

    /* Intersecting with an empty range inside produces an empty range */
    Range2Di e{{40, 25}, {40, 25}};
    CORRADE_VERIFY(Math::intersects(a, e));
    CORRADE_VERIFY(Math::intersects(e, a));
    CORRADE_COMPARE(Math::intersect(a, e), e);
    CORRADE_COMPARE(Math::intersect(e, a), e);

    /* Doesn't intersect a range touching the edges from the outside */
    Range2Di i{{20, 30}, {34, 40}};
    Range2Di j{{47, 20}, {60, 23}};
    Range2Di k{{20, 20}, {34, 23}};
    Range2Di l{{47, 30}, {60, 40}};
    CORRADE_VERIFY(!Math::intersects(a, i));
    CORRADE_VERIFY(!Math::intersects(a, j));
    CORRADE_VERIFY(!Math::intersects(a, k));
    CORRADE_VERIFY(!Math::intersects(a, l));
    CORRADE_VERIFY(!Math::intersects(i, a));
    CORRADE_VERIFY(!Math::intersects(j, a));
    CORRADE_VERIFY(!Math::intersects(k, a));
    CORRADE_VERIFY(!Math::intersects(l, a));
    CORRADE_COMPARE(Math::intersect(a, i), Range2Di{});
    CORRADE_COMPARE(Math::intersect(a, j), Range2Di{});
    CORRADE_COMPARE(Math::intersect(a, k), Range2Di{});
    CORRADE_COMPARE(Math::intersect(a, l), Range2Di{});
    CORRADE_COMPARE(Math::intersect(i, a), Range2Di{});
    CORRADE_COMPARE(Math::intersect(j, a), Range2Di{});
    CORRADE_COMPARE(Math::intersect(k, a), Range2Di{});
    CORRADE_COMPARE(Math::intersect(l, a), Range2Di{});
}

void RangeTest::intersectIntersects1D() {
    Range1Di a(34, 47);

    /* Intersects itself */
    CORRADE_VERIFY(Math::intersects(a, a));
    CORRADE_COMPARE(Math::intersect(a, a), a);

    /* Non-empty intersection */
    Range1Di b{30, 35};
    Range1Di c{34, 35};
    CORRADE_VERIFY(Math::intersects(a, b));
    CORRADE_VERIFY(Math::intersects(b, a));
    CORRADE_COMPARE(Math::intersect(a, b), c);
    CORRADE_COMPARE(Math::intersect(b, a), c);

    /* Intersecting with an empty range outside produces a default-constructed range */
    Range1Di d{130, 130};
    CORRADE_VERIFY(!Math::intersects(a, d));
    CORRADE_VERIFY(!Math::intersects(d, a));
    CORRADE_COMPARE(Math::intersect(a, d), Range1Di{});
    CORRADE_COMPARE(Math::intersect(d, a), Range1Di{});

    /* Intersecting with an empty range inside produces an empty range */
    Range1Di e{40, 40};
    CORRADE_VERIFY(Math::intersects(a, e));
    CORRADE_VERIFY(Math::intersects(e, a));
    CORRADE_COMPARE(Math::intersect(a, e), e);
    CORRADE_COMPARE(Math::intersect(e, a), e);

    /* Doesn't intersect a range touching the edges from the outside */
    Range1Di i{20, 34};
    Range1Di j{47, 60};
    CORRADE_VERIFY(!Math::intersects(a, i));
    CORRADE_VERIFY(!Math::intersects(a, j));
    CORRADE_VERIFY(!Math::intersects(i, a));
    CORRADE_VERIFY(!Math::intersects(j, a));
    CORRADE_COMPARE(Math::intersect(a, i), Range1Di{});
    CORRADE_COMPARE(Math::intersect(a, j), Range1Di{});
    CORRADE_COMPARE(Math::intersect(i, a), Range1Di{});
    CORRADE_COMPARE(Math::intersect(j, a), Range1Di{});
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

void RangeTest::join1D() {
    Range1Di a{12, 15};
    Range1Di b{10, 17};
    Range1Di c{130, 130};
    Range1Di d{10, 17};

    CORRADE_COMPARE(Math::join(a, b), d);
    CORRADE_COMPARE(Math::join(b, a), d);
    CORRADE_COMPARE(Math::join(a, c), a);
    CORRADE_COMPARE(Math::join(c, a), a);
}

void RangeTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Range1D a{1.0f, 2.0f};
    const Range1D b{2.0f, 3.0f};
    const Range1D c{1.0f, 3.0f};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));

    CORRADE_VERIFY(!o(a, a));
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

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RangeTest)
