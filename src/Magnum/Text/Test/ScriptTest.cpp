/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Text/Script.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct ScriptTest: TestSuite::Tester {
    explicit ScriptTest();

    void debug();

    void fromFourCC();
    void fromString();
    void fromStringInvalid();
};

ScriptTest::ScriptTest() {
    addTests({&ScriptTest::debug,

              &ScriptTest::fromFourCC,
              &ScriptTest::fromString,
              &ScriptTest::fromStringInvalid});
}

void ScriptTest::debug() {
    Containers::String out;
    Debug{&out} << Script::Latin << Text::script('M', 'a', '\xab', 'g');
    CORRADE_COMPARE(out, "Text::Script::Latin Text::Script('M', 'a', 0xab, 'g')\n");
}

void ScriptTest::fromFourCC() {
    Script s = Text::script('H', 'a', 'n', 'i');
    CORRADE_COMPARE(s, Script::Han);

    constexpr Script cs = Text::script('H', 'a', 'n', 'i');
    CORRADE_COMPARE(cs, Script::Han);
}

void ScriptTest::fromString() {
    Script s = Text::script("Hani");
    CORRADE_COMPARE(s, Script::Han);
}

void ScriptTest::fromStringInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    Text::script("");
    Text::script("hahah");
    /* Non-ASCII values are allowed, as the constexpr script() allows them
       too */
    CORRADE_COMPARE(out,
        "Text::script(): expected a four-character code, got \n"
        "Text::script(): expected a four-character code, got hahah\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::ScriptTest)
