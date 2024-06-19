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
#include <Corrade/Containers/StringStl.h> /** @todo remove once Debug is stream-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Debug is stream-free */

#include "Magnum/Text/Alignment.h"
#include "Magnum/Text/Direction.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AlignmentTest: TestSuite::Tester {
    explicit AlignmentTest();

    void debug();

    void forDirection();
    void forDirectionInvalid();
};

AlignmentTest::AlignmentTest() {
    addTests({&AlignmentTest::debug,

              &AlignmentTest::forDirection,
              &AlignmentTest::forDirectionInvalid});
}

void AlignmentTest::debug() {
    std::ostringstream out;
    Debug{&out} << Alignment::MiddleRightGlyphBounds << Alignment(0xab);
    CORRADE_COMPARE(out.str(), "Text::Alignment::MiddleRightGlyphBounds Text::Alignment(0xab)\n");
}

void AlignmentTest::forDirection() {
    /* For alignment that's neither Start nor End it's just a passthrough */
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::BottomRightGlyphBounds,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::RightToLeft
    ), Alignment::BottomRightGlyphBounds);
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::MiddleLeftIntegral,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::RightToLeft
    ), Alignment::MiddleLeftIntegral);
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::TopCenter,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::LeftToRight
    ), Alignment::TopCenter);

    /* For Start / End it resolves based on ShapeDirection, keeping all extra
       bits as well */
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::TopStart,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::LeftToRight
    ), Alignment::TopLeft);
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::MiddleEndIntegral,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::LeftToRight
    ), Alignment::MiddleRightIntegral);
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::MiddleStartGlyphBoundsIntegral,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::RightToLeft
    ), Alignment::MiddleRightGlyphBoundsIntegral);
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::LineEndGlyphBounds,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::RightToLeft
    ), Alignment::LineLeftGlyphBounds);

    /* Unspecified ShapeDirection behaves same as LeftToRight */
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::BottomStart,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::Unspecified
    ), Alignment::BottomLeft);
    CORRADE_COMPARE(alignmentForDirection(
        Alignment::TopEndGlyphBounds,
        LayoutDirection::HorizontalTopToBottom,
        ShapeDirection::Unspecified
    ), Alignment::TopRightGlyphBounds);
}

void AlignmentTest::forDirectionInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    /* It should blow up also for alignments that don't use Start or End, for
       consistency */
    alignmentForDirection(Alignment::BottomCenter, LayoutDirection::VerticalRightToLeft, ShapeDirection::Unspecified);
    alignmentForDirection(Alignment::MiddleCenterIntegral, LayoutDirection::HorizontalTopToBottom, ShapeDirection::TopToBottom);
    alignmentForDirection(Alignment::MiddleCenterIntegral, LayoutDirection::HorizontalTopToBottom, ShapeDirection::BottomToTop);
    CORRADE_COMPARE_AS(out.str(),
        "Text::alignmentForDirection(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalRightToLeft\n"
        "Text::alignmentForDirection(): Text::ShapeDirection::TopToBottom is not supported yet, sorry\n"
        "Text::alignmentForDirection(): Text::ShapeDirection::BottomToTop is not supported yet, sorry\n",
        TestSuite::Compare::String);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AlignmentTest)
