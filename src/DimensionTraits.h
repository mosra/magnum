#ifndef Magnum_DimensionTraits_h
#define Magnum_DimensionTraits_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Math/Math.h"
#include "Types.h"

/** @file
 * @brief Class Magnum::DimensionTraits
 */

namespace Magnum {

/** @brief Matrix, point and vector specializations for given dimension count */
template<UnsignedInt dimensions, class T = Float> struct DimensionTraits {
    DimensionTraits() = delete;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Vector type
     *
     * Math::Vector, Math::Vector2 or Math::Vector3 based on dimension count.
     */
    typedef U VectorType;

    /**
     * @brief Matrix type
     *
     * Floating-point Math::Matrix3 or Math::Matrix4 for 2D or 3D. No matrix
     * type defined for one dimension and integral types.
     */
    typedef U MatrixType;
    #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* One dimension */
template<class T> struct DimensionTraits<1, T> {
    DimensionTraits() = delete;

    typedef Math::Vector<1, T> VectorType;
};

/* Two dimensions - integral */
template<class T> struct DimensionTraits<2, T> {
    DimensionTraits() = delete;

    typedef Math::Vector2<T> VectorType;
};

/* Two dimensions - floating-point */
template<> struct DimensionTraits<2, Float> {
    DimensionTraits() = delete;

    typedef Math::Vector2<Float> VectorType;
    typedef Math::Matrix3<Float> MatrixType;
};
#ifndef MAGNUM_TARGET_GLES
template<> struct DimensionTraits<2, Double> {
    DimensionTraits() = delete;

    typedef Math::Vector2<Double> VectorType;
    typedef Math::Matrix3<Double> MatrixType;
};
#endif

/* Three dimensions - integral */
template<class T> struct DimensionTraits<3, T> {
    DimensionTraits() = delete;

    typedef Math::Vector3<T> VectorType;
};

/* Three dimensions - floating-point */
template<> struct DimensionTraits<3, Float> {
    DimensionTraits() = delete;

    typedef Math::Vector3<Float> VectorType;
    typedef Math::Matrix4<Float> MatrixType;
};
#ifndef MAGNUM_TARGET_GLES
template<> struct DimensionTraits<3, Double> {
    DimensionTraits() = delete;

    typedef Math::Vector3<Double> VectorType;
    typedef Math::Matrix4<Double> MatrixType;
};
#endif
#endif

}

#endif
