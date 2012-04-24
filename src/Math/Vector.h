#ifndef Magnum_Math_Vector_h
#define Magnum_Math_Vector_h
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

/** @file
 * @brief Class Magnum::Math::Vector
 */

#include <cmath>

#include "Utility/Debug.h"
#include "TypeTraits.h"

namespace Magnum { namespace Math {

/**
@brief %Vector

@todo Swizzling
*/
template<class T, size_t size> class Vector {
    public:
        typedef T Type;                     /**< @brief %Vector data type */
        const static size_t Size = size;    /**< @brief %Vector size */

        /**
         * @brief %Vector from array
         * @return Reference to the data as if it was Vector, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        inline constexpr static Vector<T, size>& from(T* data) {
            return *reinterpret_cast<Vector<T, size>*>(data);
        }

        /** @copydoc from(T*) */
        inline constexpr static const Vector<T, size>& from(const T* data) {
            return *reinterpret_cast<const Vector<T, size>*>(data);
        }

        /**
         * @brief Dot product
         *
         * @f[
         * a \cdot b = \sum_{i=1}^n a_ib_i
         * @f]
         */
        static T dot(const Vector<T, size>& a, const Vector<T, size>& b) {
            T out(0);

            for(size_t i = 0; i != size; ++i)
                out += a[i]*b[i];

            return out;
        }

        /**
         * @brief Angle between vectors
         *
         * @f[
         * \phi = \frac{a \cdot b}{|a| \cdot |b|}
         * @f]
         *
         * @todo optimize - Assume the vectors are normalized?
         */
        inline static T angle(const Vector<T, size>& a, const Vector<T, size>& b) {
            return acos(dot(a, b)/(a.length()*b.length()));
        }

        /** @brief Default constructor */
        inline constexpr Vector(): _data() {}

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Vector(T first, U... next): _data{first, next...} {}
        #else
        template<class ...U> inline constexpr Vector(T first, U... next);
        #endif

        /**
         * @brief Constructor
         * @param value Value for all fields
         */
        inline explicit Vector(T value) {
            /** @todo constexprize */
            for(size_t i = 0; i != size; ++i)
                _data[i] = value;
        }

        /** @brief Copy constructor */
        inline constexpr Vector(const Vector<T, size>& other) = default;

        /** @brief Assignment operator */
        inline Vector<T, size>& operator=(const Vector<T, size>& other) = default;

        /**
         * @brief Raw data
         * @return Array with the same size as the vector
         */
        inline T* data() { return _data; }
        inline constexpr const T* data() const { return _data; } /**< @copydoc data() */

        /** @brief Value at given position */
        inline T& operator[](size_t pos) { return _data[pos]; }
        inline constexpr T operator[](size_t pos) const { return _data[pos]; } /**< @copydoc operator[]() */

        /** @brief Equality operator */
        inline bool operator==(const Vector<T, size>& other) const {
            for(size_t pos = 0; pos != size; ++pos)
                if(!TypeTraits<T>::equals((*this)[pos], other[pos])) return false;

            return true;
        }

        /** @brief Non-equality operator */
        inline bool operator!=(const Vector<T, size>& other) const {
            return !operator==(other);
        }

        /** @brief Multiply vector */
        inline Vector<T, size> operator*(T number) const {
            return Vector<T, size>(*this)*=number;
        }

        /**
         * @brief Multiply and assign vector
         *
         * More efficient than operator*(), because it does the computation
         * in-place.
         */
        Vector<T, size>& operator*=(T number) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] *= number;

            return *this;
        }

        /** @brief Divide vector */
        inline Vector<T, size> operator/(T number) const {
            return Vector<T, size>(*this)/=number;
        }

        /**
         * @brief Divide and assign vector
         *
         * More efficient than operator/(), because it does the computation
         * in-place.
         */
        Vector<T, size>& operator/=(T number) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] /= number;

            return *this;
        }

        /** @brief Add two vectors */
        inline Vector<T, size> operator+(const Vector<T, size>& other) const {
            return Vector<T, size>(*this)+=other;
        }

        /**
         * @brief Add and assign vector
         *
         * More efficient than operator+(), because it does the computation
         * in-place.
         */
        Vector<T, size>& operator+=(const Vector<T, size>& other) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] += other[i];

            return *this;
        }

        /** @brief Substract two vectors */
        inline Vector<T, size> operator-(const Vector<T, size>& other) const {
            return Vector<T, size>(*this)-=other;
        }

        /**
         * @brief Substract and assign vector
         *
         * More efficient than operator-(), because it does the computation
         * in-place.
         */
        Vector<T, size>& operator-=(const Vector<T, size>& other) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] -= other[i];

            return *this;
        }

        /** @brief Negative vector */
        Vector<T, size> operator-() const {
            Vector<T, size> out;

            for(size_t i = 0; i != size; ++i)
                out[i] = -(*this)[i];

            return out;
        }

        /** @brief %Vector length */
        inline T length() const {
            return sqrt(dot(*this, *this));
        }

        /** @brief Normalized vector (of length 1) */
        inline Vector<T, size> normalized() const {
            return *this/length();
        }

        /** @brief Product of values in the vector */
        T product() const {
            T out = 1;

            for(size_t i = 0; i != size; ++i)
                out *= (*this)[i];

            return out;
        }

    private:
        T _data[size];
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T, size_t size> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector<T, size>& value) {
    debug << "Vector(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(size_t i = 0; i != size; ++i) {
        if(i != 0) debug << ", ";
        debug << value[i];
    }
    debug << ')';
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}
#endif

}}

#endif
