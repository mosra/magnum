/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Borislav Stanimirov <b.stanimirov@abv.bg>

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

#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/StrictWeakOrdering.h"

namespace std {
    template<> struct less<Magnum::Math::Vector2<int>>: Magnum::Math::Implementation::StrictWeakOrdering<Magnum::Math::Vector2<int>> {};
}

namespace Magnum { namespace Math { namespace Test { namespace {

struct StrictWeakOrderingTest: Corrade::TestSuite::Tester {
    explicit StrictWeakOrderingTest();

    void base();

    template<class Set> void set();
    template<class Map> void map();
};

using Vector2i = Magnum::Math::Vector2<Int>;

StrictWeakOrderingTest::StrictWeakOrderingTest() {
    addTests({&StrictWeakOrderingTest::base,

              &StrictWeakOrderingTest::set<std::set<Vector2i, Implementation::StrictWeakOrdering<Vector2i>>>,
              &StrictWeakOrderingTest::set<std::set<Vector2i, StrictWeakOrdering>>,
              &StrictWeakOrderingTest::set<std::set<Vector2i>>,

              &StrictWeakOrderingTest::map<std::map<Vector2i, Int, Implementation::StrictWeakOrdering<Vector2i>>>,
              &StrictWeakOrderingTest::map<std::map<Vector2i, Int, StrictWeakOrdering>>,
              &StrictWeakOrderingTest::map<std::map<Vector2i, Int>>});
}

void StrictWeakOrderingTest::base() {
    Implementation::StrictWeakOrdering<Int> o;
    CORRADE_VERIFY(o(1, 2));
    CORRADE_VERIFY(!o(2, 2));
    CORRADE_VERIFY(!o(3, 2));

    StrictWeakOrdering of;
    CORRADE_VERIFY(of(1, 2));
    CORRADE_VERIFY(!of(2.5, 2.5));
    CORRADE_VERIFY(!of('z', 'h'));
}

template<class> struct Compare;
template<> struct Compare<Implementation::StrictWeakOrdering<Vector2i>> {
    static const char* name() { return "Implementation::StrictWeakOrdering<Vector2i>"; }
};
template<> struct Compare<StrictWeakOrdering> {
    static const char* name() { return "StrictWeakOrdering"; }
};
template<> struct Compare<std::less<Vector2i>> {
    static const char* name() { return "std::less<Vector2i>"; }
};

template<class Set> void StrictWeakOrderingTest::set() {
    setTestCaseTemplateName(Compare<typename Set::key_compare>::name());
    Set s;

    s.insert({1, 2});
    s.insert({2, 3});

    CORRADE_COMPARE(s.size(), 2);
    CORRADE_COMPARE(*s.begin(), (Vector2i{1, 2}));
    CORRADE_COMPARE(*s.rbegin(), (Vector2i{2, 3}));

    s.insert({1, 2});
    CORRADE_COMPARE(s.size(), 2);
}

template<class Map> void StrictWeakOrderingTest::map() {
    setTestCaseTemplateName(Compare<typename Map::key_compare>::name());
    Map m;

    m[{1, 2}] = 23;
    m[{4, 5}] = 55;

    CORRADE_COMPARE(m.size(), 2);
    CORRADE_COMPARE(m.begin()->second, 23);
    CORRADE_COMPARE(m.rbegin()->second, 55);

    m[{1, 2}] = 99;
    CORRADE_COMPARE(m.size(), 2);
    CORRADE_COMPARE(m.begin()->second, 99);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::StrictWeakOrderingTest)
