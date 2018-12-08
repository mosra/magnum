/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include <map>
#include <set>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/StrictWeakOrdering.h"

namespace std
{
    template <> struct less<Magnum::Math::Vector2<float>>
        : public Magnum::Math::Implementation::StrictWeakOrdering<Magnum::Math::Vector2<float>>
    {};
}

namespace Magnum {
namespace Math {

namespace Test {

using Vector2 = Magnum::Math::Vector2<float>;

struct StrictWeakOrderingTest : Corrade::TestSuite::Tester {
    explicit StrictWeakOrderingTest();

    void base();

    void set();
    void setShort();
    void setLess();

    void map();
    void mapShort();
    void mapLess();

};

StrictWeakOrderingTest::StrictWeakOrderingTest() {
    addTests({ &StrictWeakOrderingTest::base,

              &StrictWeakOrderingTest::set,
              &StrictWeakOrderingTest::setShort,
              &StrictWeakOrderingTest::setLess,

              &StrictWeakOrderingTest::map,
              &StrictWeakOrderingTest::mapShort,
              &StrictWeakOrderingTest::mapLess});
}

void StrictWeakOrderingTest::base() {
    Implementation::StrictWeakOrdering<int> o;
    CORRADE_VERIFY(o(1, 2));
    CORRADE_VERIFY(!o(2, 2));
    CORRADE_VERIFY(!o(3, 2));

    StrictWeakOrdering of;
    CORRADE_VERIFY(of(1, 2));
    CORRADE_VERIFY(!of(2.5, 2.5));
    CORRADE_VERIFY(!of('z', 'h'));
}

void StrictWeakOrderingTest::set() {
    std::set<Vector2, Implementation::StrictWeakOrdering<Vector2>> s;

    s.insert({1, 2});
    s.insert({2, 3});

    CORRADE_VERIFY(s.size() == 2);
    CORRADE_VERIFY(*s.begin() == Vector2(1, 2));
    CORRADE_VERIFY(*s.rbegin() == Vector2(2, 3));

    s.insert({1, 2});
    CORRADE_VERIFY(s.size() == 2);
}

void StrictWeakOrderingTest::setShort() {
    std::set<Vector2, StrictWeakOrdering> s;

    s.insert({1, 2});
    s.insert({2, 3});

    CORRADE_VERIFY(s.size() == 2);
    CORRADE_VERIFY(*s.begin() == Vector2(1, 2));
    CORRADE_VERIFY(*s.rbegin() == Vector2(2, 3));

    s.insert({1, 2});
    CORRADE_VERIFY(s.size() == 2);
}

void StrictWeakOrderingTest::setLess() {
    std::set<Vector2> s;

    s.insert({1, 2});
    s.insert({2, 3});

    CORRADE_VERIFY(s.size() == 2);
    CORRADE_VERIFY(*s.begin() == Vector2(1, 2));
    CORRADE_VERIFY(*s.rbegin() == Vector2(2, 3));

    s.insert({1, 2});
    CORRADE_VERIFY(s.size() == 2);
}

void StrictWeakOrderingTest::map() {
    std::map<Vector2, int, Implementation::StrictWeakOrdering<Vector2>> m;

    m[Vector2{1, 2}] = 23;
    m[Vector2{4, 5}] = 55;

    CORRADE_VERIFY(m.size() == 2);
    CORRADE_VERIFY(m.begin()->second == 23);
    CORRADE_VERIFY(m.rbegin()->second == 55);

    m[Vector2{1, 2}] = 99;
    CORRADE_VERIFY(m.size() == 2);
    CORRADE_VERIFY(m.begin()->second == 99);
}

void StrictWeakOrderingTest::mapShort() {
    std::map<Vector2, int, StrictWeakOrdering> m;

    m[Vector2{1, 2}] = 23;
    m[Vector2{4, 5}] = 55;

    CORRADE_VERIFY(m.size() == 2);
    CORRADE_VERIFY(m.begin()->second == 23);
    CORRADE_VERIFY(m.rbegin()->second == 55);

    m[Vector2{1, 2}] = 99;
    CORRADE_VERIFY(m.size() == 2);
    CORRADE_VERIFY(m.begin()->second == 99);
}

void StrictWeakOrderingTest::mapLess() {
    std::map<Vector2, int> m;

    m[Vector2{1, 2}] = 23;
    m[Vector2{4, 5}] = 55;

    CORRADE_VERIFY(m.size() == 2);
    CORRADE_VERIFY(m.begin()->second == 23);
    CORRADE_VERIFY(m.rbegin()->second == 55);

    m[Vector2{1, 2}] = 99;
    CORRADE_VERIFY(m.size() == 2);
    CORRADE_VERIFY(m.begin()->second == 99);
}

}
}
}

CORRADE_TEST_MAIN(Magnum::Math::Test::StrictWeakOrderingTest)
