/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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

#include <set>
#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Audio/Context.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct ContextTest: TestSuite::Tester {
    explicit ContextTest();

    void constructNoCreate();
    void constructCopyMove();

    void extensions();

    void debugHrtfStatus();
};

ContextTest::ContextTest() {
    addTests({&ContextTest::constructNoCreate,
              &ContextTest::constructCopyMove,

              &ContextTest::extensions,

              &ContextTest::debugHrtfStatus});
}

void ContextTest::constructNoCreate() {
    Context context{NoCreate};

    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextTest::constructCopyMove() {
    /* Only move-construction allowed */
    CORRADE_VERIFY(!(std::is_constructible<Context, const Context&>{}));
    CORRADE_VERIFY((std::is_constructible<Context, Context&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, const Context&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, Context&&>{}));
}

void ContextTest::extensions() {
    const char* used[Implementation::ExtensionCount]{};

    std::set<std::string> unique;

    /* Check that all extension indices are unique, are listed just once etc. */
    std::string previous;
    for(const Extension& e: Extension::extensions()) {
        CORRADE_ITERATION(e.string());

        /** @todo convert to CORRADE_ERROR() when that's done */

        if(!previous.empty() && previous >= e.string()) {
            Error{} << "Extension not sorted after" << previous;
            CORRADE_VERIFY(false);
        }

        if(e.index() >= Implementation::ExtensionCount) {
            Error{} << "Index" << e.index() << "larger than" << Implementation::ExtensionCount;
            CORRADE_VERIFY(false);
        }

        if(used[e.index()]) {
            Error{} << "Index" << e.index() << "already used by" << used[e.index()];
            CORRADE_VERIFY(false);
        }

        used[e.index()] = e.string();
        if(!unique.insert(e.string()).second) {
            Error{} << "Extension listed more than once";
            CORRADE_VERIFY(false);
        }

        previous = e.string();
    }

    CORRADE_VERIFY(true);
}

void ContextTest::debugHrtfStatus() {
    std::ostringstream out;
    Debug(&out) << Context::HrtfStatus::Denied << Context::HrtfStatus(0xdead);
    CORRADE_COMPARE(out.str(), "Audio::Context::HrtfStatus::Denied Audio::Context::HrtfStatus(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::ContextTest)
