#ifndef Magnum_Text_Direction_h
#define Magnum_Text_Direction_h
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
 * @brief Enum @ref Magnum::Text::ShapeDirection, @ref Magnum::Text::LayoutDirection
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief Direction a text is shaped in
@m_since_latest

@see @ref AbstractShaper::setDirection(), @ref AbstractShaper::direction(),
    @ref LayoutDirection
*/
enum class ShapeDirection: UnsignedByte {
    /**
     * Unspecified. When set in @ref AbstractShaper::setDirection(), makes the
     * shaping rely on direction autodetection implemented in a particular
     * @ref AbstractFont plugin (if any). When returned from
     * @ref AbstractShaper::direction() after a successful
     * @ref AbstractShaper::shape() call, it means a particular
     * @ref AbstractFont plugin doesn't implement any script-specific behavior.
     */
    Unspecified = 0,

    /**
     * Left to right. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphOffsetsAdvancesInto()
     * have their Y components @cpp 0.0f @ce, and clusters filled by
     * @relativeref{AbstractShaper,glyphClustersInto()} are by default
     * monotonically non-decreasing.
     */
    LeftToRight = 1,

    /**
     * Right to left. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphOffsetsAdvancesInto()
     * have their Y components @cpp 0.0f @ce, and clusters filled by
     * @relativeref{AbstractShaper,glyphClustersInto()} are by default
     * monotonically non-increasing.
     */
    RightToLeft,

    /**
     * Top to bottom. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphOffsetsAdvancesInto()
     * have their X components @cpp 0.0f @ce, and clusters filled by
     * @relativeref{AbstractShaper,glyphClustersInto()} are by default
     * monotonically non-decreasing.
     */
    TopToBottom,

    /**
     * Bottom to top. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphOffsetsAdvancesInto()
     * have their X components @cpp 0.0f @ce, and clusters filled by
     * @relativeref{AbstractShaper,glyphClustersInto()} are by default
     * monotonically non-increasing.
     */
    BottomToTop
};

/** @debugoperatorenum{ShapeDirection} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, ShapeDirection value);

/**
@brief Direction a text is laid out in
@m_since_latest

@see @ref ShapeDirection
*/
enum class LayoutDirection: UnsignedByte {
    Unspecified = 0,

    /** Horizontal text with lines advancing from top to bottom */
    HorizontalTopToBottom = 1,

    /* There's no HorizontalBottomToTop, as apparently no scripts use that. Not
       even the CSS writing-mode property has it, so why should I. */

    /** Vertical text with lines advancing from left to right */
    VerticalLeftToRight,

    /** Vertical text with lines advancing from right to left */
    VerticalRightToLeft
};

/** @debugoperatorenum{LayoutDirection} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, LayoutDirection value);

}}

#endif
