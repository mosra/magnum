#ifndef Magnum_Text_Alignment_h
#define Magnum_Text_Alignment_h
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

/** @file
 * @brief Enum @ref Magnum::Text::Alignment
 */

#include "Magnum/Types.h"

namespace Magnum { namespace Text {

namespace Implementation {
    enum: UnsignedByte {
        AlignmentLeft = 1,
        AlignmentCenter = 2,
        AlignmentRight = 3,

        AlignmentLine = 1 << 3,
        AlignmentMiddle = 2 << 3,
        AlignmentTop = 3 << 3,

        AlignmentHorizontal = 3,
        AlignmentVertical = 3 << 3,
        AlignmentIntegral = 1 << 6
    };
}

/**
@brief Text rendering alignment

@see @ref Renderer::render(), @ref Renderer::Renderer()
*/
enum class Alignment: UnsignedByte {
    /** Text start and line is at origin */
    LineLeft = Implementation::AlignmentLine|Implementation::AlignmentLeft,

    /**
     * Text center and line is at origin
     *
     * @see @ref Alignment::LineCenterIntegral
     */
    LineCenter = Implementation::AlignmentLine|Implementation::AlignmentCenter,

    /** Text end and line is at origin */
    LineRight = Implementation::AlignmentLine|Implementation::AlignmentRight,

    /**
     * Text start and vertical middle is at origin
     *
     * @see @ref Alignment::MiddleLeftIntegral
     */
    MiddleLeft = Implementation::AlignmentMiddle|Implementation::AlignmentLeft,

    /**
     * Text center and vertical middle is at origin
     *
     * @see @ref Alignment::MiddleRightIntegral
     */
    MiddleCenter = Implementation::AlignmentMiddle|Implementation::AlignmentCenter,

    /**
     * Text end and vertical middle is at origin
     *
     * @see @ref Alignment::MiddleRightIntegral
     */
    MiddleRight = Implementation::AlignmentMiddle|Implementation::AlignmentRight,

    /** Text start and top is at origin */
    TopLeft = Implementation::AlignmentTop|Implementation::AlignmentLeft,

    /** Text center and top is at origin */
    TopCenter = Implementation::AlignmentTop|Implementation::AlignmentCenter,

    /** Text end and top is at origin */
    TopRight = Implementation::AlignmentTop|Implementation::AlignmentRight,

    /**
     * Text center and line is at origin and alignment offset is integral
     *
     * @see @ref Alignment::LineCenter
     */
    LineCenterIntegral = Implementation::AlignmentLine|Implementation::AlignmentCenter|Implementation::AlignmentIntegral,

    /**
     * Text start and vertical middle is at origin and alignment offset is integral
     *
     * @see @ref Alignment::MiddleLeft
     */
    MiddleLeftIntegral = Implementation::AlignmentMiddle|Implementation::AlignmentLeft|Implementation::AlignmentIntegral,

    /**
     * Text center and vertical middle is at origin and alignment offset is integral
     *
     * @see @ref Alignment::MiddleCenter
     */
    MiddleCenterIntegral = Implementation::AlignmentMiddle|Implementation::AlignmentCenter|Implementation::AlignmentIntegral,

    /**
     * Text end and vertical middle is at origin and alignment offset is integral
     *
     * @see @ref Alignment::MiddleRight
     */
    MiddleRightIntegral = Implementation::AlignmentMiddle|Implementation::AlignmentRight|Implementation::AlignmentIntegral
};

}}

#endif
