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

#include "Magnum/Text/Feature.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct FeatureTest: TestSuite::Tester {
    explicit FeatureTest();

    void rangeConstruct();
    void rangeConstructBeginEnd();

    void debug();

    void fromFourCC();
    void fromString();
    void fromStringInvalid();
};

FeatureTest::FeatureTest() {
    addTests({&FeatureTest::rangeConstruct,
              &FeatureTest::rangeConstructBeginEnd,

              &FeatureTest::debug,

              &FeatureTest::fromFourCC,
              &FeatureTest::fromString,
              &FeatureTest::fromStringInvalid});
}

void FeatureTest::rangeConstruct() {
    FeatureRange a{Feature::Kerning};
    FeatureRange b{Feature::StandardLigatures, false};
    FeatureRange c{Feature::AccessAllAlternates, 13};
    CORRADE_COMPARE(a.feature(), Feature::Kerning);
    CORRADE_COMPARE(b.feature(), Feature::StandardLigatures);
    CORRADE_COMPARE(c.feature(), Feature::AccessAllAlternates);
    CORRADE_VERIFY(a.isEnabled());
    CORRADE_VERIFY(!b.isEnabled());
    CORRADE_COMPARE(a.value(), 1);
    CORRADE_COMPARE(b.value(), 0);
    CORRADE_COMPARE(c.value(), 13);
    CORRADE_COMPARE(a.begin(), 0);
    CORRADE_COMPARE(b.begin(), 0);
    CORRADE_COMPARE(c.begin(), 0);
    CORRADE_COMPARE(a.end(), ~UnsignedInt{});
    CORRADE_COMPARE(b.end(), ~UnsignedInt{});
    CORRADE_COMPARE(c.end(), ~UnsignedInt{});

    constexpr FeatureRange ca{Feature::Kerning};
    constexpr FeatureRange cb{Feature::StandardLigatures, false};
    constexpr FeatureRange cc{Feature::AccessAllAlternates, 13};
    CORRADE_COMPARE(ca.feature(), Feature::Kerning);
    CORRADE_COMPARE(cb.feature(), Feature::StandardLigatures);
    CORRADE_COMPARE(cc.feature(), Feature::AccessAllAlternates);
    CORRADE_VERIFY(ca.isEnabled());
    CORRADE_VERIFY(!cb.isEnabled());
    CORRADE_COMPARE(ca.value(), 1);
    CORRADE_COMPARE(cb.value(), 0);
    CORRADE_COMPARE(cc.value(), 13);
    CORRADE_COMPARE(ca.begin(), 0);
    CORRADE_COMPARE(cb.begin(), 0);
    CORRADE_COMPARE(cc.begin(), 0);
    CORRADE_COMPARE(ca.end(), ~UnsignedInt{});
    CORRADE_COMPARE(cb.end(), ~UnsignedInt{});
    CORRADE_COMPARE(cc.end(), ~UnsignedInt{});
}

void FeatureTest::rangeConstructBeginEnd() {
    FeatureRange a{Feature::Kerning, 7, 26};
    FeatureRange b{Feature::StandardLigatures, 7, 26, false};
    FeatureRange c{Feature::AccessAllAlternates, 7, 26, 13};
    CORRADE_COMPARE(a.feature(), Feature::Kerning);
    CORRADE_COMPARE(b.feature(), Feature::StandardLigatures);
    CORRADE_COMPARE(c.feature(), Feature::AccessAllAlternates);
    CORRADE_VERIFY(a.isEnabled());
    CORRADE_VERIFY(!b.isEnabled());
    CORRADE_COMPARE(a.value(), 1);
    CORRADE_COMPARE(b.value(), 0);
    CORRADE_COMPARE(c.value(), 13);
    CORRADE_COMPARE(a.begin(), 7);
    CORRADE_COMPARE(b.begin(), 7);
    CORRADE_COMPARE(c.begin(), 7);
    CORRADE_COMPARE(a.end(), 26);
    CORRADE_COMPARE(b.end(), 26);
    CORRADE_COMPARE(c.end(), 26);

    constexpr FeatureRange ca{Feature::Kerning, 7, 26};
    constexpr FeatureRange cb{Feature::StandardLigatures, 7, 26, false};
    constexpr FeatureRange cc{Feature::AccessAllAlternates, 7, 26, 13};
    CORRADE_COMPARE(ca.feature(), Feature::Kerning);
    CORRADE_COMPARE(cb.feature(), Feature::StandardLigatures);
    CORRADE_COMPARE(cc.feature(), Feature::AccessAllAlternates);
    CORRADE_VERIFY(ca.isEnabled());
    CORRADE_VERIFY(!cb.isEnabled());
    CORRADE_COMPARE(ca.value(), 1);
    CORRADE_COMPARE(cb.value(), 0);
    CORRADE_COMPARE(cc.value(), 13);
    CORRADE_COMPARE(ca.begin(), 7);
    CORRADE_COMPARE(cb.begin(), 7);
    CORRADE_COMPARE(cc.begin(), 7);
    CORRADE_COMPARE(ca.end(), 26);
    CORRADE_COMPARE(cb.end(), 26);
    CORRADE_COMPARE(cc.end(), 26);
}

void FeatureTest::debug() {
    Containers::String out;
    Debug{&out} << Feature::StandardLigatures << Text::feature('m', 'a', '\xab', 'g');
    CORRADE_COMPARE(out, "Text::Feature::StandardLigatures Text::Feature('m', 'a', 0xab, 'g')\n");
}

void FeatureTest::fromFourCC() {
    Feature s = Text::feature('z', 'e', 'r', 'o');
    CORRADE_COMPARE(s, Feature::SlashedZero);

    constexpr Feature cs = Text::feature('z', 'e', 'r', 'o');
    CORRADE_COMPARE(cs, Feature::SlashedZero);
}

void FeatureTest::fromString() {
    Feature s = Text::feature("zero");
    CORRADE_COMPARE(s, Feature::SlashedZero);
}

void FeatureTest::fromStringInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    Text::feature("");
    Text::feature("hahah");
    /* Non-ASCII values are allowed, as the constexpr feature() allows them
       too */
    CORRADE_COMPARE(out,
        "Text::feature(): expected a four-character code, got \n"
        "Text::feature(): expected a four-character code, got hahah\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::FeatureTest)
