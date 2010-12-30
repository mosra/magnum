#ifndef Magnum_Math_Matrix3_h
#define Magnum_Math_Matrix3_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Math::Matrix3
 */

#include "Matrix.h"

namespace Magnum { namespace Math {

/** @brief Matrix 3x3 */
template<class T> class Matrix3: public Matrix<T, 3> {
    public:
        /** @copydoc Matrix::Matrix(bool) */
        inline Matrix3(bool identity = true): Matrix<T, 3>(identity) {}

        /** @copydoc Matrix::Matrix(const T*) */
        inline Matrix3(const T* data): Matrix<T, 3>(data) {}

        /** @copydoc Matrix::Matrix(const Matrix<T, size>&) */
        inline Matrix3(const Matrix<T, 3>& other): Matrix<T, 3>(other) {}
};

}}

#endif
