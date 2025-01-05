/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

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
    CORRADE_VERIFY(!std::is_copy_constructible<Context>{});
    CORRADE_VERIFY(std::is_move_constructible<Context>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Context>{});
    CORRADE_VERIFY(!std::is_move_assignable<Context>{});
}

void ContextTest::extensions() {
    const char* used[Implementation::ExtensionCount]{};

    std::set<Containers::StringView> unique;

    /* Check that all extension indices are unique, are listed just once etc. */
    Containers::StringView previous;
    for(const Extension& e: Extension::extensions()) {
        CORRADE_ITERATION(e.string());

        CORRADE_FAIL_IF(previous && previous >= e.string(),
            "Extension not sorted after" << previous);

        CORRADE_FAIL_IF(e.index() >= Implementation::ExtensionCount,
            "Index" << e.index() << "larger than" << Implementation::ExtensionCount);

        /* Have to use Containers::StringView to avoid passing null char* to
           std::cout (which crashes on Mac due to strlen being called on it)
           because the message is actually printed sooner than the condition
           gets tested! */
        /** @todo clean up once fixed in Corrade */
        CORRADE_FAIL_IF(used[e.index()],
            "Index" << e.index() << "already used by" << Containers::StringView{used[e.index()]});

        used[e.index()] = e.string();
        CORRADE_FAIL_IF(!unique.insert(e.string()).second,
            "Extension listed more than once");

        previous = e.string();
    }
}

void ContextTest::debugHrtfStatus() {
    Containers::String out;
    Debug(&out) << Context::HrtfStatus::Denied << Context::HrtfStatus(0xdead);
    CORRADE_COMPARE(out, "Audio::Context::HrtfStatus::Denied Audio::Context::HrtfStatus(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::ContextTest)
