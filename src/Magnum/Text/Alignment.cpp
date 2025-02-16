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

#include "Alignment.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Text/Direction.h"

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const Alignment value) {
    debug << "Text::Alignment" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Alignment::v: return debug << "::" #v;
        _c(LineLeft)
        _c(LineLeftGlyphBounds)
        _c(LineCenter)
        _c(LineCenterIntegral)
        _c(LineCenterGlyphBounds)
        _c(LineCenterGlyphBoundsIntegral)
        _c(LineRight)
        _c(LineRightGlyphBounds)
        _c(LineBegin)
        _c(LineBeginGlyphBounds)
        _c(LineEnd)
        _c(LineEndGlyphBounds)
        _c(BottomLeft)
        _c(BottomLeftGlyphBounds)
        _c(BottomCenter)
        _c(BottomCenterIntegral)
        _c(BottomCenterGlyphBounds)
        _c(BottomCenterGlyphBoundsIntegral)
        _c(BottomRight)
        _c(BottomRightGlyphBounds)
        _c(BottomBegin)
        _c(BottomBeginGlyphBounds)
        _c(BottomEnd)
        _c(BottomEndGlyphBounds)
        _c(MiddleLeft)
        _c(MiddleLeftIntegral)
        _c(MiddleLeftGlyphBounds)
        _c(MiddleLeftGlyphBoundsIntegral)
        _c(MiddleCenter)
        _c(MiddleCenterIntegral)
        _c(MiddleCenterGlyphBounds)
        _c(MiddleCenterGlyphBoundsIntegral)
        _c(MiddleRight)
        _c(MiddleRightIntegral)
        _c(MiddleRightGlyphBounds)
        _c(MiddleRightGlyphBoundsIntegral)
        _c(MiddleBegin)
        _c(MiddleBeginIntegral)
        _c(MiddleBeginGlyphBounds)
        _c(MiddleBeginGlyphBoundsIntegral)
        _c(MiddleEnd)
        _c(MiddleEndIntegral)
        _c(MiddleEndGlyphBounds)
        _c(MiddleEndGlyphBoundsIntegral)
        _c(TopLeft)
        _c(TopLeftGlyphBounds)
        _c(TopCenter)
        _c(TopCenterIntegral)
        _c(TopCenterGlyphBounds)
        _c(TopCenterGlyphBoundsIntegral)
        _c(TopRight)
        _c(TopRightGlyphBounds)
        _c(TopBegin)
        _c(TopBeginGlyphBounds)
        _c(TopEnd)
        _c(TopEndGlyphBounds)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Alignment alignmentForDirection(const Alignment alignment, const LayoutDirection layoutDirection, const ShapeDirection shapeDirection) {
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(layoutDirection);
    #endif
    CORRADE_ASSERT(layoutDirection == LayoutDirection::HorizontalTopToBottom,
        "Text::alignmentForDirection(): only" << LayoutDirection::HorizontalTopToBottom << "is supported right now, got" << layoutDirection, {});
    CORRADE_ASSERT(shapeDirection != ShapeDirection::TopToBottom &&
                   shapeDirection != ShapeDirection::BottomToTop,
        "Text::alignmentForDirection():" << shapeDirection << "is not supported yet, sorry", {});

    const UnsignedByte horizontal = UnsignedByte(alignment) & Implementation::AlignmentHorizontal;
    const UnsignedByte exceptHorizontal = UnsignedByte(alignment) & ~Implementation::AlignmentHorizontal;
    if(horizontal == Implementation::AlignmentBegin)
        return Alignment((shapeDirection == ShapeDirection::RightToLeft ?
            Implementation::AlignmentRight :
            Implementation::AlignmentLeft)|exceptHorizontal);
    if(horizontal == Implementation::AlignmentEnd)
        return Alignment((shapeDirection == ShapeDirection::RightToLeft ?
            Implementation::AlignmentLeft :
            Implementation::AlignmentRight)|exceptHorizontal);

    return alignment;
}

}}
