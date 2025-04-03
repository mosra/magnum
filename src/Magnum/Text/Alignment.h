#ifndef Magnum_Text_Alignment_h
#define Magnum_Text_Alignment_h
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

/** @file
 * @brief Enum @ref Magnum::Text::Alignment, function @ref Magnum::Text::alignmentForDirection()
 */

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

namespace Implementation {
    enum: UnsignedByte {
        /* Line/Left, which causes no shift of the shaped text whatsoever,
           is deliberately 0 to signify a default */

        AlignmentLeft = 0,
        AlignmentCenter = 1 << 0,
        AlignmentRight = 2 << 0,
        /* Begin and End is Left or Right based on ShapeDirection, and possibly
           also Top / Bottom eventually for vertical text */
        AlignmentBegin = 3 << 0,
        AlignmentEnd = 4 << 0,
        AlignmentHorizontal = AlignmentLeft|AlignmentCenter|AlignmentRight|AlignmentBegin|AlignmentEnd,

        AlignmentLine = 0,
        AlignmentBottom = 1 << 4,
        AlignmentMiddle = 2 << 4,
        AlignmentTop = 3 << 4,
        AlignmentVertical = AlignmentLine|AlignmentBottom|AlignmentMiddle|AlignmentTop,

        AlignmentIntegral = 1 << 6,
        AlignmentGlyphBounds = 1 << 7
    };
}

/**
@brief Text rendering alignment

By default, the alignment is performed based on cursor position and font metric
alone, without taking actual glyph offsets and rectangles into account. This
allows the alignment to be performed even before actual glyph bounds are known
and avoids the position changing based on what concrete glyphs are present.
Aligning to actual glyph rectangle bounds can be done with the `*GlyphBounds`
variants.

The `*Integer` values are meant to be used for pixel-perfect fonts that always
have glyph sizes, advances and other metrics whole pixels, and need to be
positioned on whole pixels as well in order to avoid making them blurry. These
are only needed for `*Middle` and `*Center` alignments as they may result in
the bounding rectangle to have odd dimensions, which would then result in
half-pixel shifts when divided by 2.

The `*Begin` and `*End` values behave the same as `*Left` and `*Right`,
respectively, if @ref ShapeDirection::LeftToRight is passed to
@ref AbstractShaper::setDirection(), or if it's
@ref ShapeDirection::Unspecified and the particular font plugin doesn't detect
@ref ShapeDirection::RightToLeft when shaping. If
@ref ShapeDirection::RightToLeft is set (or detected for
@ref ShapeDirection::Unspecified), they're swapped, i.e. `*Begin` becomes
`*Right` and `*End` becomes `*Left`.
@see @ref Renderer::render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "Renderer::render()",
    @ref Renderer::add(), @ref alignmentForDirection()
*/
enum class Alignment: UnsignedByte {
    /**
     * Leftmost cursor position and vertical line position is at origin.
     * @see @ref Alignment::LineLeftGlyphBounds, @ref Alignment::LineBegin,
     *      @ref Alignment::LineEnd
     */
    LineLeft = Implementation::AlignmentLine|Implementation::AlignmentLeft,

    /**
     * Left side of the glyph bounding rectangle and vertical line position is
     * at origin.
     * @see @ref Alignment::LineLeft, @ref Alignment::LineBeginGlyphBounds,
     *      @ref Alignment::LineEndGlyphBounds
     * @m_since_latest
     */
    LineLeftGlyphBounds = LineLeft|Implementation::AlignmentGlyphBounds,

    /**
     * Midpoint between leftmost and rightmost cursor position and vertical
     * line position is at origin.
     * @see @ref Alignment::LineCenterGlyphBounds,
     *      @ref Alignment::LineCenterIntegral
     */
    LineCenter = Implementation::AlignmentLine|Implementation::AlignmentCenter,

    /**
     * Midpoint between leftmost and rightmost cursor position and vertical
     * line position is at origin, with the horizontal offset rounded to whole
     * units.
     * @see @ref Alignment::LineCenter,
     *      @ref Alignment::LineCenterGlyphBoundsIntegral
     */
    LineCenterIntegral = LineCenter|Implementation::AlignmentIntegral,

    /**
     * Horizontal center of the glyph bounding rectangle and vertical line
     * position is at origin.
     * @see @ref Alignment::LineCenter,
     *      @ref Alignment::LineCenterGlyphBoundsIntegral
     * @m_since_latest
     */
    LineCenterGlyphBounds = LineCenter|Implementation::AlignmentGlyphBounds,

    /**
     * Horizontal center of the glyph bounding rectangle and vertical line
     * position is at origin, with the horizontal offset rounded to whole
     * units.
     * @see @ref Alignment::LineCenterGlyphBounds,
     *      @ref Alignment::LineCenterIntegral
     * @m_since_latest
     */
    LineCenterGlyphBoundsIntegral = LineCenterGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Rightmost cursor position and vertical line position is at origin.
     * @see @ref Alignment::LineRightGlyphBounds, @ref Alignment::LineEnd,
     *      @ref Alignment::LineBegin
     * @see @ref alignmentForDirection()
     */
    LineRight = Implementation::AlignmentLine|Implementation::AlignmentRight,

    /**
     * Right side of the glyph bounding rectangle and vertical line position is
     * at origin.
     * @see @ref Alignment::LineRight, @ref Alignment::LineEndGlyphBounds,
     *      @ref Alignment::LineBeginGlyphBounds
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    LineRightGlyphBounds = LineRight|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::LineRight for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::LineLeft otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    LineBegin = Implementation::AlignmentLine|Implementation::AlignmentBegin,

    /**
     * @ref Alignment::LineRightGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::LineLeftGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    LineBeginGlyphBounds = LineBegin|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::LineLeft for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::LineRight otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    LineEnd = Implementation::AlignmentLine|Implementation::AlignmentEnd,

    /**
     * @ref Alignment::LineLeftGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::LineRightGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    LineEndGlyphBounds = LineEnd|Implementation::AlignmentGlyphBounds,

    /**
     * Leftmost cursor position and bottommost line descent is at origin.
     * @see @ref Alignment::BottomLeftGlyphBounds, @ref Alignment::BottomBegin,
     *      @ref Alignment::BottomEnd
     * @m_since_latest
     */
    BottomLeft = Implementation::AlignmentBottom|Implementation::AlignmentLeft,

    /**
     * Bottom left corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::BottomLeft, @ref Alignment::BottomBeginGlyphBounds,
     *      @ref Alignment::BottomEndGlyphBounds
     * @m_since_latest
     */
    BottomLeftGlyphBounds = BottomLeft|Implementation::AlignmentGlyphBounds,

    /**
     * Midpoint between leftmost and rightmost cursor position and bottommost
     * line decent is at origin.
     * @see @ref Alignment::BottomCenterGlyphBounds,
     *      @ref Alignment::BottomCenterIntegral
     * @m_since_latest
     */
    BottomCenter = Implementation::AlignmentBottom|Implementation::AlignmentCenter,

    /**
     * Midpoint between leftmost and rightmost cursor position and bottommost
     * line descent is at origin, with the horizontal offset rounded to whole
     * units.
     * @see @ref Alignment::BottomCenter,
     *      @ref Alignment::BottomCenterGlyphBoundsIntegral
     * @m_since_latest
     */
    BottomCenterIntegral = BottomCenter|Implementation::AlignmentIntegral,

    /**
     * Horizontal center and bottom side of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::BottomCenter,
     *      @ref Alignment::BottomCenterGlyphBoundsIntegral
     * @m_since_latest
     */
    BottomCenterGlyphBounds = BottomCenter|Implementation::AlignmentGlyphBounds,

    /**
     * Horizontal center and bottom side of the glyph bounding rectangle is at
     * origin, with the horizontal offset rounded to whole units.
     * @see @ref Alignment::BottomCenterGlyphBounds,
     *      @ref Alignment::BottomCenterIntegral
     * @m_since_latest
     */
    BottomCenterGlyphBoundsIntegral = BottomCenterGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Rightmost cursor position and bottommost line descent is at origin.
     * @see @ref Alignment::BottomRightGlyphBounds, @ref Alignment::BottomEnd,
     *      @ref Alignment::BottomBegin
     * @m_since_latest
     */
    BottomRight = Implementation::AlignmentBottom|Implementation::AlignmentRight,

    /**
     * Bottom right corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::BottomRight, @ref Alignment::BottomEndGlyphBounds,
     *      @ref Alignment::BottomBeginGlyphBounds
     * @m_since_latest
     */
    BottomRightGlyphBounds = BottomRight|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::BottomRight for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::BottomLeft otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    BottomBegin = Implementation::AlignmentBottom|Implementation::AlignmentBegin,

    /**
     * @ref Alignment::BottomRightGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::BottomLeftGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    BottomBeginGlyphBounds = BottomBegin|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::BottomLeft for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::BottomRight otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    BottomEnd = Implementation::AlignmentBottom|Implementation::AlignmentEnd,

    /**
     * @ref Alignment::BottomLeftGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::BottomRightGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    BottomEndGlyphBounds = BottomEnd|Implementation::AlignmentGlyphBounds,

    /**
     * Leftmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin.
     * @see @ref Alignment::MiddleLeftGlyphBounds,
     *      @ref Alignment::MiddleLeftIntegral, @ref Alignment::MiddleBegin,
     *      @ref Alignment::MiddleEnd
     */
    MiddleLeft = Implementation::AlignmentMiddle|Implementation::AlignmentLeft,

    /**
     * Leftmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin, with the vertical offset rounded
     * to whole units.
     * @see @ref Alignment::MiddleLeft,
     *      @ref Alignment::MiddleLeftGlyphBoundsIntegral,
     *      @ref Alignment::MiddleBeginIntegral,
     *      @ref Alignment::MiddleEndIntegral
     */
    MiddleLeftIntegral = MiddleLeft|Implementation::AlignmentIntegral,

    /**
     * Left side and vertical center of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::MiddleLeft,
     *      @ref Alignment::MiddleLeftGlyphBoundsIntegral,
     *      @ref Alignment::MiddleBeginGlyphBounds,
     *      @ref Alignment::MiddleEndGlyphBounds
     * @m_since_latest
     */
    MiddleLeftGlyphBounds = MiddleLeft|Implementation::AlignmentGlyphBounds,

    /**
     * Left side and vertical center of the glyph bounding rectangle is at
     * origin, with the vertical offset rounded to whole units.
     * @see @ref Alignment::MiddleLeftGlyphBounds,
     *      @ref Alignment::MiddleLeftIntegral,
     *      @ref Alignment::MiddleBeginGlyphBoundsIntegral,
     *      @ref Alignment::MiddleEndGlyphBoundsIntegral
     * @m_since_latest
     */
    MiddleLeftGlyphBoundsIntegral = MiddleLeftGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Midpoint between leftmost and rightmost cursor position and a midpoint
     * between topmost line ascent and bottommost line descent is at origin.
     * @see @ref Alignment::MiddleCenterGlyphBounds,
     *      @ref Alignment::MiddleRightIntegral
     */
    MiddleCenter = Implementation::AlignmentMiddle|Implementation::AlignmentCenter,

    /**
     * Midpoint between leftmost and rightmost cursor position and a midpoint
     * between topmost line ascent and bottommost line descent is at origin,
     * with both the horizontal and vertical offset rounded to whole units.
     * @see @ref Alignment::MiddleCenter,
     *      @ref Alignment::MiddleCenterGlyphBoundsIntegral
     */
    MiddleCenterIntegral = MiddleCenter|Implementation::AlignmentIntegral,

    /**
     * Horizontal and vertical center of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::MiddleCenter,
     *      @ref Alignment::MiddleCenterGlyphBoundsIntegral
     * @m_since_latest
     */
    MiddleCenterGlyphBounds = MiddleCenter|Implementation::AlignmentGlyphBounds,

    /**
     * Horizontal and vertical center of the glyph bounding rectangle is at
     * origin, with both the horizontal and vertical offset rounded to whole
     * units.
     * @see @ref Alignment::MiddleCenterGlyphBounds,
     *      @ref Alignment::MiddleCenterIntegral
     * @m_since_latest
     */
    MiddleCenterGlyphBoundsIntegral = MiddleCenterGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Rightmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin.
     * @see @ref Alignment::MiddleRightGlyphBounds,
     *      @ref Alignment::MiddleRightIntegral, @ref Alignment::MiddleEnd,
     *      @ref Alignment::MiddleBegin
     */
    MiddleRight = Implementation::AlignmentMiddle|Implementation::AlignmentRight,

    /**
     * Rightmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin, with the vertical offset rounded
     * to whole units.
     * @see @ref Alignment::MiddleRight,
     *      @ref Alignment::MiddleRightGlyphBoundsIntegral,
     *      @ref Alignment::MiddleEndIntegral,
     *      @ref Alignment::MiddleBeginIntegral
     */
    MiddleRightIntegral = MiddleRight|Implementation::AlignmentIntegral,

    /**
     * Right side and vertical center of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::MiddleRight,
     *      @ref Alignment::MiddleRightGlyphBoundsIntegral,
     *      @ref Alignment::MiddleEndGlyphBounds,
     *      @ref Alignment::MiddleBeginGlyphBounds
     * @m_since_latest
     */
    MiddleRightGlyphBounds = MiddleRight|Implementation::AlignmentGlyphBounds,

    /**
     * Right side and vertical center of the glyph bounding rectangle is at
     * origin, with the vertical offset rounded to whole units.
     * @see @ref Alignment::MiddleRightGlyphBounds,
     *      @ref Alignment::MiddleRightIntegral,
     *      @ref Alignment::MiddleEndGlyphBoundsIntegral,
     *      @ref Alignment::MiddleBeginGlyphBoundsIntegral
     * @m_since_latest
     */
    MiddleRightGlyphBoundsIntegral = MiddleRightGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * @ref Alignment::MiddleRight for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::MiddleLeft otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleBegin = Implementation::AlignmentMiddle|Implementation::AlignmentBegin,

    /**
     * @ref Alignment::MiddleRightIntegral for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::MiddleLeftIntegral
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleBeginIntegral = MiddleBegin|Implementation::AlignmentIntegral,

    /**
     * @ref Alignment::MiddleRightGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::MiddleLeftGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleBeginGlyphBounds = MiddleBegin|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::MiddleRightGlyphBoundsIntegral for
     * @ref ShapeDirection::RightToLeft,
     * @ref Alignment::MiddleLeftGlyphBoundsIntegral otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleBeginGlyphBoundsIntegral = MiddleBeginGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * @ref Alignment::MiddleLeft for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::MiddleRight otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleEnd = Implementation::AlignmentMiddle|Implementation::AlignmentEnd,

    /**
     * @ref Alignment::MiddleLeftIntegral for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::MiddleRightIntegral
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleEndIntegral = MiddleEnd|Implementation::AlignmentIntegral,

    /**
     * @ref Alignment::MiddleLeftGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::MiddleRightGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleEndGlyphBounds = MiddleEnd|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::MiddleLeftGlyphBoundsIntegral for
     * @ref ShapeDirection::RightToLeft,
     * @ref Alignment::MiddleRightGlyphBoundsIntegral otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    MiddleEndGlyphBoundsIntegral = MiddleEndGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Leftmost cursor position and topmost line ascent is at origin.
     * @see @ref Alignment::TopLeftGlyphBounds, @ref Alignment::TopBegin,
     *      @ref Alignment::TopEnd
     */
    TopLeft = Implementation::AlignmentTop|Implementation::AlignmentLeft,

    /**
     * Top left corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::TopLeft, @ref Alignment::TopBeginGlyphBounds,
     *      @ref Alignment::TopEndGlyphBounds
     * @m_since_latest
     */
    TopLeftGlyphBounds = TopLeft|Implementation::AlignmentGlyphBounds,

    /**
     * Midpoint between leftmost and rightmost cursor position and topmost line
     * ascent is at origin.
     * @see @ref Alignment::TopCenterGlyphBounds,
     *      @ref Alignment::TopCenterIntegral
     */
    TopCenter = Implementation::AlignmentTop|Implementation::AlignmentCenter,

    /**
     * Midpoint between leftmost and rightmost cursor position and topmost line
     * ascent is at origin, with the horizontal offset rounded to whole units.
     * @see @ref Alignment::TopCenter,
     *      @ref Alignment::TopCenterGlyphBoundsIntegral
     * @m_since_latest
     */
    TopCenterIntegral = TopCenter|Implementation::AlignmentIntegral,

    /**
     * Horizontal center and top side of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::TopCenter,
     *      @ref Alignment::TopCenterGlyphBoundsIntegral
     * @m_since_latest
     */
    TopCenterGlyphBounds = TopCenter|Implementation::AlignmentGlyphBounds,

    /**
     * Horizontal center and top side of the glyph bounding rectangle is at
     * origin, with the horizontal offset rounded to whole units.
     * @see @ref Alignment::TopCenterGlyphBounds,
     *      @ref Alignment::TopCenterIntegral
     * @m_since_latest
     */
    TopCenterGlyphBoundsIntegral = TopCenterGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Rightmost cursor position and topmost line ascent is at origin.
     * @see @ref Alignment::TopRightGlyphBounds, @ref Alignment::TopEnd,
     *      @ref Alignment::TopBegin
     */
    TopRight = Implementation::AlignmentTop|Implementation::AlignmentRight,

    /**
     * Top right corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::TopRight, @ref Alignment::TopEndGlyphBounds,
     *      @ref Alignment::TopBeginGlyphBounds
     * @m_since_latest
     */
    TopRightGlyphBounds = TopRight|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::TopRight for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::TopLeft otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    TopBegin = Implementation::AlignmentTop|Implementation::AlignmentBegin,

    /**
     * @ref Alignment::TopRightGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::TopLeftGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    TopBeginGlyphBounds = TopBegin|Implementation::AlignmentGlyphBounds,

    /**
     * @ref Alignment::TopLeft for @ref ShapeDirection::RightToLeft,
     * @ref Alignment::TopRight otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    TopEnd = Implementation::AlignmentTop|Implementation::AlignmentEnd,

    /**
     * @ref Alignment::TopLeftGlyphBounds for
     * @ref ShapeDirection::RightToLeft, @ref Alignment::TopRightGlyphBounds
     * otherwise.
     * @see @ref alignmentForDirection()
     * @m_since_latest
     */
    TopEndGlyphBounds = TopEnd|Implementation::AlignmentGlyphBounds,
};

/** @debugoperatorenum{Alignment} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, Alignment value);

/**
@brief Alignment for layout and shape direction
@m_since_latest

The @p layoutDirection is currently expected to always be
@ref LayoutDirection::HorizontalTopToBottom and @p shapeDirection never
@ref ShapeDirection::TopToBottom or @ref ShapeDirection::BottomToTop. Then, if
@p alignment is `*Begin` or `*End`, it's converted to `*Left` or `*Right`,
respectively, if @p shapeDirection is @ref ShapeDirection::LeftToRight or
@ref ShapeDirection::Unspecified, and `*Right` or `*Left`, respectively, if
@p shapeDirection is @ref ShapeDirection::RightToLeft.
*/
MAGNUM_TEXT_EXPORT Alignment alignmentForDirection(Alignment alignment, LayoutDirection layoutDirection, ShapeDirection shapeDirection);


}}

#endif
