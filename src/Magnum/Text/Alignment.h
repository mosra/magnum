#ifndef Magnum_Text_Alignment_h
#define Magnum_Text_Alignment_h
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

/** @file
 * @brief Enum @ref Magnum::Text::Alignment
 */

#include "Magnum/Types.h"

namespace Magnum { namespace Text {

namespace Implementation {
    enum: UnsignedByte {
        /* Line/Left, which causes no shift of the shaped text whatsoever,
           is deliberately 0 to signify a default */

        AlignmentLeft = 0,
        AlignmentCenter = 1 << 0,
        AlignmentRight = 2 << 0,
        AlignmentHorizontal = AlignmentLeft|AlignmentCenter|AlignmentRight,

        AlignmentLine = 0,
        AlignmentBottom = 1 << 2,
        AlignmentMiddle = 2 << 2,
        AlignmentTop = 3 << 2,
        AlignmentVertical = AlignmentLine|AlignmentBottom|AlignmentMiddle|AlignmentTop,

        AlignmentIntegral = 1 << 4,
        AlignmentGlyphBounds = 1 << 5
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
@see @ref Renderer::render(), @ref Renderer::Renderer()
*/
enum class Alignment: UnsignedByte {
    /**
     * Leftmost cursor position and vertical line position is at origin.
     * @see @ref Alignment::LineLeftGlyphBounds
     */
    LineLeft = Implementation::AlignmentLine|Implementation::AlignmentLeft,

    /**
     * Left side of the glyph bounding rectangle and vertical line position is
     * at origin.
     * @see @ref Alignment::LineLeft
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
     * @see @ref Alignment::LineRightGlyphBounds
     */
    LineRight = Implementation::AlignmentLine|Implementation::AlignmentRight,

    /**
     * Right side of the glyph bounding rectangle and vertical line position is
     * at origin.
     * @see @ref Alignment::LineRight
     * @m_since_latest
     */
    LineRightGlyphBounds = LineRight|Implementation::AlignmentGlyphBounds,

    /**
     * Leftmost cursor position and bottommost line descent is at origin.
     * @see @ref Alignment::BottomLeftGlyphBounds
     * @m_since_latest
     */
    BottomLeft = Implementation::AlignmentBottom|Implementation::AlignmentLeft,

    /**
     * Bottom left corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::BottomLeft
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
     * @see @ref Alignment::BottomRightGlyphBounds
     * @m_since_latest
     */
    BottomRight = Implementation::AlignmentBottom|Implementation::AlignmentRight,

    /**
     * Bottom right corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::BottomRight
     * @m_since_latest
     */
    BottomRightGlyphBounds = BottomRight|Implementation::AlignmentGlyphBounds,

    /**
     * Leftmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin.
     * @see @ref Alignment::MiddleLeftGlyphBounds,
     *      @ref Alignment::MiddleLeftIntegral
     */
    MiddleLeft = Implementation::AlignmentMiddle|Implementation::AlignmentLeft,

    /**
     * Leftmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin, with the vertical offset rounded
     * to whole units.
     * @see @ref Alignment::MiddleLeft,
     *      @ref Alignment::MiddleLeftGlyphBoundsIntegral
     */
    MiddleLeftIntegral = MiddleLeft|Implementation::AlignmentIntegral,

    /**
     * Left side and vertical center of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::MiddleLeft,
     *      @ref Alignment::MiddleLeftGlyphBoundsIntegral
     * @m_since_latest
     */
    MiddleLeftGlyphBounds = MiddleLeft|Implementation::AlignmentGlyphBounds,

    /**
     * Left side and vertical center of the glyph bounding rectangle is at
     * origin, with the vertical offset rounded to whole units.
     * @see @ref Alignment::MiddleLeftGlyphBounds,
     *      @ref Alignment::MiddleLeftIntegral
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
     *      @ref Alignment::MiddleRightIntegral
     */
    MiddleRight = Implementation::AlignmentMiddle|Implementation::AlignmentRight,

    /**
     * Rightmost cursor position and a midpoint between topmost line ascent and
     * bottommost line descent is at origin, with the vertical offset rounded
     * to whole units.
     * @see @ref Alignment::MiddleRight,
     *      @ref Alignment::MiddleRightGlyphBoundsIntegral
     */
    MiddleRightIntegral = MiddleRight|Implementation::AlignmentIntegral,

    /**
     * Right side and vertical center of the glyph bounding rectangle is at
     * origin.
     * @see @ref Alignment::MiddleRight,
     *      @ref Alignment::MiddleRightGlyphBoundsIntegral
     * @m_since_latest
     */
    MiddleRightGlyphBounds = MiddleRight|Implementation::AlignmentGlyphBounds,

    /**
     * Right side and vertical center of the glyph bounding rectangle is at
     * origin, with the vertical offset rounded to whole units.
     * @see @ref Alignment::MiddleRightGlyphBounds,
     *      @ref Alignment::MiddleRightIntegral
     * @m_since_latest
     */
    MiddleRightGlyphBoundsIntegral = MiddleRightGlyphBounds|Implementation::AlignmentIntegral,

    /**
     * Leftmost cursor position and topmost line ascent is at origin.
     * @see @ref Alignment::TopLeftGlyphBounds
     */
    TopLeft = Implementation::AlignmentTop|Implementation::AlignmentLeft,

    /**
     * Top left corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::TopLeft
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
     * @see @ref Alignment::TopRightGlyphBounds
     */
    TopRight = Implementation::AlignmentTop|Implementation::AlignmentRight,

    /**
     * Top right corner of the glyph bounding rectangle is at origin.
     * @see @ref Alignment::TopRight
     * @m_since_latest
     */
    TopRightGlyphBounds = TopRight|Implementation::AlignmentGlyphBounds,
};

}}

#endif
