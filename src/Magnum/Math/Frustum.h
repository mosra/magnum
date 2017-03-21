#ifndef Magnum_Math_Frustum_h
#define Magnum_Math_Frustum_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<class, class> struct FrustumConverter;
}

/**
@brief Camera frustum

Stores camera frustum planes in order left (index `0`), right (index `1`),
bottom (index `2`), top (index `3`), near (index `4`) and far (index `5`).
@see @ref Magnum::Frustum, @ref Magnum::Frustumd,
    @ref Geometry::Intersection::pointFrustum(),
    @ref Geometry::Intersection::boxFrustum()
*/
template<class T> class Frustum {
    public:
        /** @brief Create a frustum from projection matrix */
        static Frustum<T> fromMatrix(const Matrix4<T>& m) {
            return {m.row(3) + m.row(0),
                    m.row(3) - m.row(0),
                    m.row(3) + m.row(1),
                    m.row(3) - m.row(1),
                    m.row(3) + m.row(2),
                    m.row(3) - m.row(2)};
        }

        /**
         * @brief Identity constructor
         *
         * Equivalent to creating a frustum from an identity matrix.
         * @see @ref fromMatrix()
         */
        constexpr /*implicit*/ Frustum(IdentityInitT = IdentityInit) noexcept;

        /** @brief Construct a frustum without initializing the contents */
        explicit Frustum(NoInitT) noexcept: _data{Vector4<T>{NoInit}, Vector4<T>{NoInit}, Vector4<T>{NoInit}, Vector4<T>{NoInit}, Vector4<T>{NoInit}, Vector4<T>{NoInit}} {}

        /** @brief Construct a frustum from plane equations */
        constexpr /*implicit*/ Frustum(const Vector4<T>& left, const Vector4<T>& right, const Vector4<T>& bottom, const Vector4<T>& top, const Vector4<T>& near, const Vector4<T>& far) noexcept: _data{left, right, bottom, top, near, far} {}

        /**
         * @brief Construct frustum from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else.
         */
        template<class U> constexpr explicit Frustum(const Frustum<U>& other) noexcept;

        /** @brief Construct frustum from external representation */
        template<class U, class V = decltype(Implementation::FrustumConverter<T, U>::from(std::declval<U>()))> constexpr explicit Frustum(const U& other) noexcept: Frustum<T>{Implementation::FrustumConverter<T, U>::from(other)} {}

        /** @brief Convert frustum to external representation */
        template<class U, class V = decltype(Implementation::FrustumConverter<T, U>::to(std::declval<Frustum<T>>()))> constexpr explicit operator U() const {
            return Implementation::FrustumConverter<T, U>::to(*this);
        }

        /** @brief Equality comparison */
        bool operator==(const Frustum<T>& other) const {
            for(std::size_t i = 0; i != 6; ++i)
                if(_data[i] != other._data[i]) return false;

            return true;
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Frustum<T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of length `24`.
         */
        T* data() { return _data[0].data(); }
        constexpr const T* data() const { return _data[0].data(); } /**< @overload */

        /** @brief Frustum planes */
        constexpr Corrade::Containers::StaticArrayView<6, const Vector4<T>> planes() const {
            /* GCC 4.7 needs explicit construction */
            return Corrade::Containers::StaticArrayView<6, const Vector4<T>>{_data};
        }

        /** @brief Plane at given index */
        /* returns const& so [][] operations are also constexpr */
        constexpr const Vector4<T>& operator[](std::size_t i) const { return _data[i]; }

        /** @brief Left plane */
        constexpr Vector4<T> left() const { return _data[0]; }

        /** @brief Right plane */
        constexpr Vector4<T> right() const { return _data[1]; }

        /** @brief Bottom plane */
        constexpr Vector4<T> bottom() const { return _data[2]; }

        /** @brief Top plane */
        constexpr Vector4<T> top() const { return _data[3]; }

        /** @brief Near plane */
        constexpr Vector4<T> near() const { return _data[4]; }

        /** @brief Far plane */
        constexpr Vector4<T> far() const { return _data[5]; }

    private:
        Vector4<T> _data[6];
};

/** @debugoperator{Magnum::Math::Frustum} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Frustum<T>& value) {
    debug << "Frustum({" << Corrade::Utility::Debug::nospace;
    for(std::size_t i = 0; i != 6; ++i) {
        if(i != 0) debug << Corrade::Utility::Debug::nospace << "},\n        {" << Corrade::Utility::Debug::nospace;
        for(std::size_t j = 0; j != 4; ++j) {
            if(j != 0) debug << Corrade::Utility::Debug::nospace << ",";
            debug << value[i][j];
        }
    }
    return debug << Corrade::Utility::Debug::nospace << "})";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Frustum<Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Frustum<Float>&);
#endif

template<class T> constexpr Frustum<T>::Frustum(IdentityInitT) noexcept: _data{
    { 1.0f,  0.0f,  0.0f, 1.0f},
    {-1.0f,  0.0f,  0.0f, 1.0f},
    { 0.0f,  1.0f,  0.0f, 1.0f},
    { 0.0f, -1.0f,  0.0f, 1.0f},
    { 0.0f,  0.0f,  1.0f, 1.0f},
    { 0.0f,  0.0f, -1.0f, 1.0f}} {}

template<class T> template<class U> constexpr Frustum<T>::Frustum(const Frustum<U>& other) noexcept: _data{
    Vector4<T>{other[0]},
    Vector4<T>{other[1]},
    Vector4<T>{other[2]},
    Vector4<T>{other[3]},
    Vector4<T>{other[4]},
    Vector4<T>{other[5]}} {}

}}

#endif
