#ifndef Magnum_Math_Frustum_h
#define Magnum_Math_Frustum_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Math::Frustum
 */

#include <Corrade/configure.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Math {

/**
@brief Camera frustum

*/
template<class T> class Frustum {
    public:

        /**
         * @brief Create a frustum from projection matrix
         */
        static Frustum<T> fromMatrix(const Matrix4<T>& m) {
            return Frustum{
                    mvp.row(3) + mvp.row(0),
                    mvp.row(3) - mvp.row(0),
                    mvp.row(3) + mvp.row(1),
                    mvp.row(3) - mvp.row(1),
                    mvp.row(3) + mvp.row(2),
                    mvp.row(3) - mvp.row(2)
                };
        }

        /**
         * @brief Construct frustum from frustum planes
         */
        constexpr /*implicit*/ Frustum(const Vector4<T>& left, const Vector4<T>& right, const Vector4<T>& bottom, const Vector4<T>& top, const Vector4<T>& near, const Vector4& far): _data{left, right, bottom, top, near, far} {}

        /**
         * @brief Raw data
         * @return One-dimensional array of length `24`.
         */
        T* data() { return _data[0].data(); }

        /** @overload */
        constexpr const T* data() const { return _data[0].data(); }

        /**
         * @brief The frustum planes
         *
         * In order left (index `0`), right (index `1`), bottom (index `1`),
         * top (index `3`), near (index `4`), far (index `5`).
         */
        constexpr Corrade::Containers::StaticArrayView<6, Vector4<T>> planes() const { return _data; }

        /**
         * @brief Plane at given index
         *
         * In order left (index `0`), right (index `1`), bottom (index `1`),
         * top (index `3`), near (index `4`), far (index `5`).
         */
        constexpr Vector4<T> operator[](std::size_t i) const { return _data[i]; }

    private:
        Vector4<T> _data[6];
};

}}

#endif
