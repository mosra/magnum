#ifndef Magnum_DimensionTraits_h
#define Magnum_DimensionTraits_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Magnum.h"

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
template<> struct DimensionTraits<2, Double> {
    DimensionTraits() = delete;

    typedef Math::Vector2<Double> VectorType;
    typedef Math::Matrix3<Double> MatrixType;
};

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
template<> struct DimensionTraits<3, Double> {
    DimensionTraits() = delete;

    typedef Math::Vector3<Double> VectorType;
    typedef Math::Matrix4<Double> MatrixType;
};
#endif

}

#endif
