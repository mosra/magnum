/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Debug is stream-free */

#include "Magnum/Math/Range.h"
#include "Magnum/Text/AbstractFont.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractLayouterTest: TestSuite::Tester {
    explicit AbstractLayouterTest();

    void renderGlyph();
    void renderGlyphOutOfRange();
};

AbstractLayouterTest::AbstractLayouterTest() {
    addTests({&AbstractLayouterTest::renderGlyph,
              &AbstractLayouterTest::renderGlyphOutOfRange});
}

void AbstractLayouterTest::renderGlyph() {
    struct Layouter: AbstractLayouter {
        explicit Layouter(): AbstractLayouter{3} {}

        Containers::Triple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt) override {
            return {{{1.0f, 0.5f}, {1.1f, 1.0f}},
                    {{0.3f, 1.1f}, {-0.5f, 0.7f}},
                    {2.0f, -1.0f}};
        }
    };

    /* Rectangle of zero size shouldn't be merged, but replaced */
    Range2D rectangle({-1.0f, -1.0f}, {-1.0f, -1.0f});
    Vector2 cursorPosition(1.0f, 2.0f);

    Layouter l;
    CORRADE_COMPARE(l.renderGlyph(0, cursorPosition, rectangle),
        Containers::pair(Range2D{{2.0f, 2.5f}, {2.1f, 3.0f}},
                         Range2D{{0.3f, 1.1f}, {-0.5f, 0.7f}}));
    CORRADE_COMPARE(cursorPosition, Vector2(3.0f, 1.0f));
    CORRADE_COMPARE(rectangle, Range2D({2.0f, 2.5f}, {2.1f, 3.0f}));

    CORRADE_COMPARE(l.renderGlyph(1, cursorPosition, rectangle),
        Containers::pair(Range2D{{4.0f, 1.5f}, {4.1f, 2.0f}},
                         Range2D{{0.3f, 1.1f}, {-0.5f, 0.7f}}));
    CORRADE_COMPARE(cursorPosition, Vector2(5.0f, 0.0f));
    CORRADE_COMPARE(rectangle, Range2D({2.0f, 1.5f}, {4.1f, 3.0f}));

    CORRADE_COMPARE(l.renderGlyph(2, cursorPosition, rectangle),
        Containers::pair(Range2D{{6.0f, 0.5f}, {6.1f, 1.0f}},
                         Range2D{{0.3f, 1.1f}, {-0.5f, 0.7f}}));
    CORRADE_COMPARE(cursorPosition, Vector2(7.0f, -1.0f));
    CORRADE_COMPARE(rectangle, Range2D({2.0f, 0.5f}, {6.1f, 3.0f}));
}

void AbstractLayouterTest::renderGlyphOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Layouter: AbstractLayouter {
        explicit Layouter(): AbstractLayouter{3} {}

        Containers::Triple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt) override { return {}; }
    } layouter;

    Range2D rectangle;
    Vector2 cursorPosition;

    std::ostringstream out;
    Error redirectError{&out};
    layouter.renderGlyph(3, cursorPosition, rectangle);
    CORRADE_COMPARE(out.str(), "Text::AbstractLayouter::renderGlyph(): index 3 out of range for 3 glyphs\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractLayouterTest)
