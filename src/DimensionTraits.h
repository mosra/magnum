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

#include <cstdint>

/** @file
 * @brief Class Magnum::DimensionTraits
 */

namespace Magnum {

namespace Math {
    template<size_t, class> class Vector;
    template<class> class Vector2;
    template<class> class Vector3;

    template<class> class Point2D;
    template<class> class Point3D;

    template<class> class Matrix3;
    template<class> class Matrix4;
}

/** @brief Matrix, point and vector specializations for given dimension count */
template<std::uint8_t dimensions, class T> struct DimensionTraits {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Vector type
     *
     * Math::Vector, Math::Vector2 or Math::Vector3 based on dimension count.
     */
    typedef U VectorType;

    /**
     * @brief Point type
     *
     * Floating-point Math::Point2D or Math::Point3D for 2D or 3D. No point
     * type defined for one dimension and integral types.
     */
    typedef U PointType;

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
    typedef Math::Vector<1, T> VectorType;
};

/* Two dimensions - integral */
template<class T> struct DimensionTraits<2, T> {
    typedef Math::Vector2<T> VectorType;
};

/* Two dimensions - floating-point */
template<> struct DimensionTraits<2, float> {
    typedef Math::Vector2<float> VectorType;
    typedef Math::Point2D<float> PointType;
    typedef Math::Matrix3<float> MatrixType;
};
template<> struct DimensionTraits<2, double> {
    typedef Math::Vector2<double> VectorType;
    typedef Math::Point2D<double> PointType;
    typedef Math::Matrix3<double> MatrixType;
};

/* Three dimensions - integral */
template<class T> struct DimensionTraits<3, T> {
    typedef Math::Vector3<T> VectorType;
};

/* Three dimensions - floating-point */
template<> struct DimensionTraits<3, float> {
    typedef Math::Vector3<float> VectorType;
    typedef Math::Point3D<float> PointType;
    typedef Math::Matrix4<float> MatrixType;
};
template<> struct DimensionTraits<3, double> {
    typedef Math::Vector3<double> VectorType;
    typedef Math::Point3D<double> PointType;
    typedef Math::Matrix4<double> MatrixType;
};
#endif

}

#endif
