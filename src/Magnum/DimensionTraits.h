#ifndef Magnum_DimensionTraits_h
#define Magnum_DimensionTraits_h
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
 * @brief Class @ref Magnum::DimensionTraits, alias @ref Magnum::VectorTypeFor, @ref Magnum::RangeTypeFor, @ref Magnum::MatrixTypeFor
 */

#include "Magnum/Math/Math.h"
#include "Magnum/Types.h"

namespace Magnum {

/**
@brief Matrix and vector specializations for given dimension count

@see @ref VectorTypeFor, @ref RangeTypeFor, @ref MatrixTypeFor
*/
template<UnsignedInt dimensions, class T> struct DimensionTraits {
    DimensionTraits() = delete;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Vector type
     *
     * @ref Math::Vector, @ref Math::Vector2 or @ref Math::Vector3 based on
     * dimension count.
     */
    typedef U VectorType;

    /**
     * @brief Range type
     *
     * @ref Math::Range1D, @ref Math::Range2D or @ref Math::Range3D based on
     * dimension count.
     */
    typedef U RangeType;

    /**
     * @brief Matrix type
     *
     * Floating-point @ref Math::Matrix3 or @ref Math::Matrix4 for 2D or 3D. No
     * matrix type defined for one dimension and integral types.
     */
    typedef U MatrixType;
    #endif
};

/**
@brief Vector type for given dimension count and type

Convenience alternative to @cpp typename DimensionTraits<dimensions, T>::VectorType @ce.
See @ref DimensionTraits::VectorType for more information.
*/
template<UnsignedInt dimensions, class T> using VectorTypeFor = typename DimensionTraits<dimensions, T>::VectorType;

/**
@brief Range type for given dimension count and type

Convenience alternative to @cpp typename DimensionTraits<dimensions, T>::RangeType @ce.
See @ref DimensionTraits::RangeType for more information.
*/
template<UnsignedInt dimensions, class T> using RangeTypeFor = typename DimensionTraits<dimensions, T>::RangeType;

/**
@brief Matrix type for given dimension count and type

Convenience alternative to @cpp typename DimensionTraits<dimensions, T>::MatrixType @ce.
See @ref DimensionTraits::MatrixType for more information.
*/
template<UnsignedInt dimensions, class T> using MatrixTypeFor = typename DimensionTraits<dimensions, T>::MatrixType;

#ifndef DOXYGEN_GENERATING_OUTPUT
/* One dimension */
template<class T> struct DimensionTraits<1, T> {
    DimensionTraits() = delete;

    typedef Math::Vector<1, T> VectorType;
    typedef Math::Range1D<T> RangeType;
};

/* Two dimensions - integral */
template<class T> struct DimensionTraits<2, T> {
    DimensionTraits() = delete;

    typedef Math::Vector2<T> VectorType;
    typedef Math::Range2D<T> RangeType;
};

/* Two dimensions - floating-point */
template<> struct DimensionTraits<2, Float> {
    DimensionTraits() = delete;

    typedef Math::Vector2<Float> VectorType;
    typedef Math::Range2D<Float> RangeType;
    typedef Math::Matrix3<Float> MatrixType;
};
template<> struct DimensionTraits<2, Double> {
    DimensionTraits() = delete;

    typedef Math::Vector2<Double> VectorType;
    typedef Math::Range2D<Double> RangeType;
    typedef Math::Matrix3<Double> MatrixType;
};

/* Three dimensions - integral */
template<class T> struct DimensionTraits<3, T> {
    DimensionTraits() = delete;

    typedef Math::Vector3<T> VectorType;
    typedef Math::Range3D<T> RangeType;
};

/* Three dimensions - floating-point */
template<> struct DimensionTraits<3, Float> {
    DimensionTraits() = delete;

    typedef Math::Vector3<Float> VectorType;
    typedef Math::Range3D<Float> RangeType;
    typedef Math::Matrix4<Float> MatrixType;
};
template<> struct DimensionTraits<3, Double> {
    DimensionTraits() = delete;

    typedef Math::Vector3<Double> VectorType;
    typedef Math::Range3D<Double> RangeType;
    typedef Math::Matrix4<Double> MatrixType;
};
#endif

}

#endif
