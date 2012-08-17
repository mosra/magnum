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
#include <limits>
#include <Utility/Debug.h>

#include "MathTypeTraits.h"

namespace Magnum { namespace Math {

template<size_t size, class T> class Vector;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<size_t ...> struct Sequence {};

    /* E.g. GenerateSequence<3>::Type is Sequence<0, 1, 2> */
    template<size_t N, size_t ...sequence> struct GenerateSequence:
        GenerateSequence<N-1, N-1, sequence...> {};

    template<size_t ...sequence> struct GenerateSequence<0, sequence...> {
        typedef Sequence<sequence...> Type;
    };

    /* Implementation for Vector<size, T>::from(const Vector<size, U>&) */
    template<class T, class U, size_t ...sequence> inline constexpr Math::Vector<sizeof...(sequence), T> vectorFrom(Sequence<sequence...>, const Math::Vector<sizeof...(sequence), U>& vector) {
        return {T(vector[sequence])...};
    }
}
#endif

/** @brief %Vector */
template<size_t size, class T> class Vector {
    static_assert(size != 0, "Vector cannot have zero elements");

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
         * @brief %Vector from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Vector<4, float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * Vector<4, int> integral(Vector<4, int>::from(floatingPoint));
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        template<class U> inline constexpr static Vector<size, T> from(const Vector<size, U>& other) {
            return Implementation::vectorFrom<T, U>(typename Implementation::GenerateSequence<size>::Type(), other);
        }

        /**
         * @brief Dot product
         *
         * @f[
         * a \cdot b = \sum_{i=0}^{n-1} a_ib_i
         * @f]
         * @see dot() const
         */
        static T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
            T out(0);

            for(size_t i = 0; i != size; ++i)
                out += a[i]*b[i];

            return out;
        }

        /**
         * @brief Angle between normalized vectors
         *
         * @f[
         * \phi = \frac{a \cdot b}{|a| \cdot |b|}
         * @f]
         * @attention If any of the parameters is not normalized (and
         * assertions are enabled), returns NaN.
         */
        inline static T angle(const Vector<size, T>& a, const Vector<size, T>& b) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(a.dot(), T(1)) && MathTypeTraits<T>::equals(b.dot(), T(1)),
                           "Math::Vector::angle(): vectors must be normalized!", std::numeric_limits<T>::quiet_NaN());
            return std::acos(dot(a, b));
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
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline explicit Vector(typename std::enable_if<std::is_same<T, U>::value && size != 1, U>::type value) {
        #else
        inline explicit Vector(T value) {
        #endif
            for(size_t i = 0; i != size; ++i)
                _data[i] = value;
        }

        /** @brief Copy constructor */
        inline constexpr Vector(const Vector<size, T>&) = default;

        /** @brief Assignment operator */
        inline Vector<size, T>& operator=(const Vector<size, T>&) = default;

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
         * @see operator*=(U)
         */
        template<class U> inline Vector<size, T> operator*(U number) const {
            return Vector<size, T>(*this)*=number;
        }

        /**
         * @brief Multiply vector and assign
         *
         * More efficient than operator*(U) const, because it does the
         * computation in-place.
         */
        template<class U> Vector<size, T>& operator*=(U number) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] *= number;

            return *this;
        }

        /**
         * @brief Divide vector
         *
         * @see operator/=(U)
         */
        template<class U> inline Vector<size, T> operator/(U number) const {
            return Vector<size, T>(*this)/=number;
        }

        /**
         * @brief Divide vector and assign
         *
         * More efficient than operator/(U) const, because it does the
         * computation in-place.
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

        /** @brief Subtract two vectors */
        inline Vector<size, T> operator-(const Vector<size, T>& other) const {
            return Vector<size, T>(*this)-=other;
        }

        /**
         * @brief Subtract and assign vector
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
         * @brief Dot product of the vector
         *
         * Should be used instead of length() for comparing vector length with
         * other values, because it doesn't compute the square root, just the
         * dot product: @f$ a \cdot a < length \cdot length @f$ is faster
         * than @f$ \sqrt{a \cdot a} < length @f$.
         *
         * @see dot(const Vector<size, T>&, const Vector<size, T>&)
         */
        inline T dot() const {
            return dot(*this, *this);
        }

        /**
         * @brief %Vector length
         *
         * @see dot() const
         */
        inline T length() const {
            return std::sqrt(dot());
        }

        /** @brief Normalized vector (of length 1) */
        inline Vector<size, T> normalized() const {
            return *this/length();
        }

        /** @brief Sum of values in the vector */
        T sum() const {
            T out(0);

            for(size_t i = 0; i != size; ++i)
                out += (*this)[i];

            return out;
        }

        /** @brief Product of values in the vector */
        T product() const {
            T out(1);

            for(size_t i = 0; i != size; ++i)
                out *= (*this)[i];

            return out;
        }

        /** @brief Minimal value in the vector */
        T min() const {
            T out((*this)[0]);

            for(size_t i = 1; i != size; ++i)
                out = std::min(out, (*this)[i]);

            return out;
        }

        /** @brief Maximal value in the vector */
        T max() const {
            T out((*this)[0]);

            for(size_t i = 1; i != size; ++i)
                out = std::max(out, (*this)[i]);

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
        debug << typename MathTypeTraits<T>::NumericType(value[i]);
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
    template<class U> inline constexpr static Type<T> from(const Vector<size, U>& other) { \
        return Vector<size, T>::from(other);                                \
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
