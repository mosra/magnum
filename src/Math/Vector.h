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
#include "MathTypeTraits.h"

namespace Magnum { namespace Math {

/**
@brief %Vector

@todo Swizzling
*/
template<size_t size, class T> class Vector {
    public:
        const static size_t Size = size;    /**< @brief %Vector size */
        typedef T Type;                     /**< @brief %Vector data type */

        /**
         * @brief %Vector from array
         * @return Reference to the data as if it was Vector, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        inline constexpr static Vector<size, T>& from(T* data) {
            return *reinterpret_cast<Vector<size, T>*>(data);
        }
        /** @overload */
        inline constexpr static const Vector<size, T>& from(const T* data) {
            return *reinterpret_cast<const Vector<size, T>*>(data);
        }

        /**
         * @brief Dot product
         *
         * @f[
         * a \cdot b = \sum_{i=0}^{n-1} a_ib_i
         * @f]
         */
        static T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
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
        inline static T angle(const Vector<size, T>& a, const Vector<size, T>& b) {
            return acos(dot(a, b)/(a.length()*b.length()));
        }

        /** @brief Default constructor */
        inline constexpr Vector(): _data() {}

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         *
         * @todoc Remove workaround when Doxygen supports uniform initialization
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Vector(T first, U... next): _data{first, next...} {
            static_assert(sizeof...(next)+1 == size, "Improper number of arguments passed to Vector constructor");
        }
        #else
        template<class ...U> inline constexpr Vector(T first, U... next);
        #endif

        /**
         * @brief Constructor
         * @param value Value for all fields
         */
        inline explicit Vector(T value) {
            for(size_t i = 0; i != size; ++i)
                _data[i] = value;
        }

        /** @brief Copy constructor */
        inline constexpr Vector(const Vector<size, T>& other) = default;

        /** @brief Assignment operator */
        inline Vector<size, T>& operator=(const Vector<size, T>& other) = default;

        /**
         * @brief Raw data
         * @return Array with the same size as the vector
         */
        inline T* data() { return _data; }
        inline constexpr const T* data() const { return _data; } /**< @overload */

        /** @brief Value at given position */
        inline T& operator[](size_t pos) { return _data[pos]; }
        inline constexpr T operator[](size_t pos) const { return _data[pos]; } /**< @overload */

        /** @brief Equality operator */
        inline bool operator==(const Vector<size, T>& other) const {
            for(size_t pos = 0; pos != size; ++pos)
                if(!MathTypeTraits<T>::equals((*this)[pos], other[pos])) return false;

            return true;
        }

        /** @brief Non-equality operator */
        inline bool operator!=(const Vector<size, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Multiply vector
         *
         * Note that corresponding operator with swapped type order
         * (multiplying number with vector) is not available, because it would
         * cause ambiguity in some cases.
         */
        template<class U> inline Vector<size, T> operator*(U number) const {
            return Vector<size, T>(*this)*=number;
        }

        /**
         * @brief Multiply and assign vector
         *
         * More efficient than operator*(), because it does the computation
         * in-place.
         */
        template<class U> Vector<size, T>& operator*=(U number) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] *= number;

            return *this;
        }

        /** @brief Divide vector */
        template<class U> inline Vector<size, T> operator/(U number) const {
            return Vector<size, T>(*this)/=number;
        }

        /**
         * @brief Divide and assign vector
         *
         * More efficient than operator/(), because it does the computation
         * in-place.
         */
        template<class U> Vector<size, T>& operator/=(U number) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] /= number;

            return *this;
        }

        /** @brief Add two vectors */
        inline Vector<size, T> operator+(const Vector<size, T>& other) const {
            return Vector<size, T>(*this)+=other;
        }

        /**
         * @brief Add and assign vector
         *
         * More efficient than operator+(), because it does the computation
         * in-place.
         */
        Vector<size, T>& operator+=(const Vector<size, T>& other) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] += other[i];

            return *this;
        }

        /** @brief Substract two vectors */
        inline Vector<size, T> operator-(const Vector<size, T>& other) const {
            return Vector<size, T>(*this)-=other;
        }

        /**
         * @brief Substract and assign vector
         *
         * More efficient than operator-(), because it does the computation
         * in-place.
         */
        Vector<size, T>& operator-=(const Vector<size, T>& other) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] -= other[i];

            return *this;
        }

        /** @brief Negative vector */
        Vector<size, T> operator-() const {
            Vector<size, T> out;

            for(size_t i = 0; i != size; ++i)
                out[i] = -(*this)[i];

            return out;
        }

        /**
         * @brief %Vector length
         *
         * @see lengthSquared()
         */
        inline T length() const {
            return sqrt(dot(*this, *this));
        }

        /**
         * @brief %Vector length squared
         *
         * More efficient than length() for comparing vector length with
         * other values, because it doesn't compute the square root, just the
         * dot product: @f$ a \cdot a < length \cdot length @f$ is faster
         * than @f$ \sqrt{a \cdot a} < length @f$.
         */
        inline T lengthSquared() const {
            return dot(*this, *this);
        }

        /** @brief Normalized vector (of length 1) */
        inline Vector<size, T> normalized() const {
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

/** @debugoperator{Vector} */
template<class T, size_t size> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector<size, T>& value) {
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

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Type, size)                   \
    inline constexpr static Type<T>& from(T* data) {                        \
        return *reinterpret_cast<Type<T>*>(data);                           \
    }                                                                       \
    inline constexpr static const Type<T>& from(const T* data) {            \
        return *reinterpret_cast<const Type<T>*>(data);                     \
    }                                                                       \
                                                                            \
    inline Type<T>& operator=(const Type<T>& other) {                       \
        Vector<size, T>::operator=(other);                                  \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    template<class U> inline Type<T> operator*(U number) const {            \
        return Vector<size, T>::operator*(number);                          \
    }                                                                       \
    template<class U> inline Type<T>& operator*=(U number) {                \
        Vector<size, T>::operator*=(number);                                \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline Type<T> operator/(U number) const {            \
        return Vector<size, T>::operator/(number);                          \
    }                                                                       \
    template<class U> inline Type<T>& operator/=(U number) {                \
        Vector<size, T>::operator/=(number);                                \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline Type<T> operator+(const Vector<size, T>& other) const {          \
        return Vector<size, T>::operator+(other);                           \
    }                                                                       \
    inline Type<T>& operator+=(const Vector<size, T>& other) {              \
        Vector<size, T>::operator+=(other);                                 \
        return *this;                                                       \
    }                                                                       \
    inline Type<T> operator-(const Vector<size, T>& other) const {          \
        return Vector<size, T>::operator-(other);                           \
    }                                                                       \
    inline Type<T>& operator-=(const Vector<size, T>& other) {              \
        Vector<size, T>::operator-=(other);                                 \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline Type<T> operator-() const { return Vector<size, T>::operator-(); } \
    inline Type<T> normalized() const { return Vector<size, T>::normalized(); }
#endif

}}

#endif
