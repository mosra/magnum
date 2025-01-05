#ifndef Magnum_ImageFlags_h
#define Magnum_ImageFlags_h
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
 * @brief Enum @ref Magnum::ImageFlag1D, @ref Magnum::ImageFlag2D, @ref Magnum::ImageFlag3D, enum set @ref Magnum::ImageFlags1D, @ref Magnum::ImageFlags2D, @ref Magnum::ImageFlags3D, alias @ref Magnum::ImageFlag, @ref Magnum::ImageFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief 1D image layout flag
@m_since_latest

Used by @ref Image1D, @ref ImageView1D and @ref Trade::ImageData1D. Currently
no flags specific to 1D images are defined.
@see @ref ImageFlags1D
*/
enum class ImageFlag1D: UnsignedShort {};

/**
@brief 2D image layout flag
@m_since_latest

Used by @ref Image2D, @ref ImageView2D and @ref Trade::ImageData2D.
@see @ref ImageFlags2D
*/
enum class ImageFlag2D: UnsignedShort {
    /**
     * The image is a 1D array instead of 2D. I.e., no filtering is done along
     * the Y axis and mip levels don't shorten along the Y axis.
     *
     * Guaranteed to have the same value as @ref ImageFlag3D::Array.
     */
    Array = 1 << 0,
};

/**
@brief 3D image layout flag
@m_since_latest

Used by @ref Image3D, @ref ImageView3D and @ref Trade::ImageData3D.
@see @ref ImageFlags3D
*/
enum class ImageFlag3D: UnsignedShort {
    /**
     * The image is a 2D array instead of 3D. I.e., no filtering is done along
     * the Z axis and mip levels don't shorten along the Z axis.
     *
     * Guaranteed to have the same value as @ref ImageFlag2D::Array.
     */
    Array = 1 << 0,

    /**
     * The image is a cube map instead of 3D. I.e., there's exactly six square
     * 2D faces in order (+X, -X, +Y, -Y, +Z, -Z), filtering is done on face
     * edges, and mip levels don't shorten along the Z axis. If combined with
     * @ref ImageFlag3D::Array, the image is a cube map array, consisting of an
     * exact multiple of six square 2D faces, with each six layers being one
     * cube map.
     */
    CubeMap = 1 << 1
};

/** @debugoperatorenum{ImageFlag1D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ImageFlag1D value);

/** @debugoperatorenum{ImageFlag2D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ImageFlag2D value);

/** @debugoperatorenum{ImageFlag3D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ImageFlag3D value);

/**
@brief 1D image layout flags
@m_since_latest

Used by @ref Image1D, @ref ImageView1D, @ref Trade::ImageData1D and
@ref GL::BufferImage1D.
*/
typedef Containers::EnumSet<ImageFlag1D> ImageFlags1D;

/**
@brief 2D image layout flags
@m_since_latest

Used by @ref Image2D, @ref ImageView2D, @ref Trade::ImageData2D and
@ref GL::BufferImage2D.
*/
typedef Containers::EnumSet<ImageFlag2D> ImageFlags2D;

/**
@brief 3D image layout flags
@m_since_latest

Used by @ref Image3D, @ref ImageView3D, @ref Trade::ImageData3D and
@ref GL::BufferImage3D.
*/
typedef Containers::EnumSet<ImageFlag3D> ImageFlags3D;

CORRADE_ENUMSET_OPERATORS(ImageFlags1D)
CORRADE_ENUMSET_OPERATORS(ImageFlags2D)
CORRADE_ENUMSET_OPERATORS(ImageFlags3D)

/** @debugoperatorenum{ImageFlags1D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ImageFlags1D value);

/** @debugoperatorenum{ImageFlags2D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ImageFlags2D value);

/** @debugoperatorenum{ImageFlags3D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ImageFlags3D value);

namespace Implementation {

template<UnsignedInt> struct ImageFlagTraits;
template<> struct ImageFlagTraits<1> {
    typedef ImageFlag1D Type;
    typedef ImageFlags1D SetType;
};
template<> struct ImageFlagTraits<2> {
    typedef ImageFlag2D Type;
    typedef ImageFlags2D SetType;
};
template<> struct ImageFlagTraits<3> {
    typedef ImageFlag3D Type;
    typedef ImageFlags3D SetType;
};

}

/**
@brief Image layout flag
@m_since_latest

Expands to @ref ImageFlag1D, @ref ImageFlag2D or @ref ImageFlag3D based on
dimension count.
@see @ref ImageFlags
*/
template<UnsignedInt dimensions> using ImageFlag = typename Implementation::ImageFlagTraits<dimensions>::Type;

/**
@brief Image layout flags
@m_since_latest

Expands to @ref ImageFlags1D, @ref ImageFlags2D or @ref ImageFlags3D based on
dimension count.
@see @ref ImageFlag
*/
template<UnsignedInt dimensions> using ImageFlags = typename Implementation::ImageFlagTraits<dimensions>::SetType;

}

#endif
