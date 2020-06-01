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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Range.h"
#include "Magnum/Text/AbstractFont.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractLayouterTest: TestSuite::Tester {
    explicit AbstractLayouterTest();

    void renderGlyph();
};

AbstractLayouterTest::AbstractLayouterTest() {
    addTests({&AbstractLayouterTest::renderGlyph});
}

void AbstractLayouterTest::renderGlyph() {
    class Layouter: public AbstractLayouter {
        public:
            explicit Layouter(): AbstractLayouter(3) {}

        private:
            std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt) override {
                return std::make_tuple(Range2D({1.0f, 0.5f}, {1.1f, 1.0f}),
                                       Range2D({0.3f, 1.1f}, {-0.5f, 0.7f}),
                                       Vector2(2.0f, -1.0f));
            }
    };

    /* Rectangle of zero size shouldn't be merged, but replaced */
    Range2D rectangle({-1.0f, -1.0f}, {-1.0f, -1.0f});
    Vector2 cursorPosition(1.0f, 2.0f);

    Layouter l;
    Range2D quadPosition;
    Range2D textureCoords;

    std::tie(quadPosition, textureCoords) = l.renderGlyph(0, cursorPosition, rectangle);
    CORRADE_COMPARE(quadPosition, Range2D({2.0f, 2.5f}, {2.1f, 3.0f}));
    CORRADE_COMPARE(textureCoords, Range2D({0.3f, 1.1f}, {-0.5f, 0.7f}));
    CORRADE_COMPARE(cursorPosition, Vector2(3.0f, 1.0f));
    CORRADE_COMPARE(rectangle, Range2D({2.0f, 2.5f}, {2.1f, 3.0f}));

    std::tie(quadPosition, textureCoords) = l.renderGlyph(1, cursorPosition, rectangle);
    CORRADE_COMPARE(quadPosition, Range2D({4.0f, 1.5f}, {4.1f, 2.0f}));
    CORRADE_COMPARE(textureCoords, Range2D({0.3f, 1.1f}, {-0.5f, 0.7f}));
    CORRADE_COMPARE(cursorPosition, Vector2(5.0f, 0.0f));
    CORRADE_COMPARE(rectangle, Range2D({2.0f, 1.5f}, {4.1f, 3.0f}));

    std::tie(quadPosition, textureCoords) = l.renderGlyph(2, cursorPosition, rectangle);
    CORRADE_COMPARE(quadPosition, Range2D({6.0f, 0.5f}, {6.1f, 1.0f}));
    CORRADE_COMPARE(textureCoords, Range2D({0.3f, 1.1f}, {-0.5f, 0.7f}));
    CORRADE_COMPARE(cursorPosition, Vector2(7.0f, -1.0f));
    CORRADE_COMPARE(rectangle, Range2D({2.0f, 0.5f}, {6.1f, 3.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractLayouterTest)
