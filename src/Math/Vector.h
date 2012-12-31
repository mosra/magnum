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

#include <Utility/Assert.h>

#include "RectangularMatrix.h"

namespace Magnum { namespace Math {

/**
@brief %Vector
@tparam size    %Vector size
@tparam T       Data type

See @ref matrix-vector for brief introduction.
@configurationvalueref{Magnum::Math::Vector}
*/
template<std::size_t size, class T> class Vector: public RectangularMatrix<1, size, T> {
    public:
        const static std::size_t Size = size; /**< @brief %Vector size */

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

            for(std::size_t i = 0; i != size; ++i)
                out += a[i]*b[i];

            return out;
        }

        /**
         * @brief Angle between normalized vectors (in radians)
         *
         * @f[
         * \phi = acos \left(\frac{a \cdot b}{|a| \cdot |b|} \right)
         * @f]
         * @attention Both vectors must be normalized.
         */
        inline static T angle(const Vector<size, T>& normalizedA, const Vector<size, T>& normalizedB) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normalizedA.dot(), T(1)) && MathTypeTraits<T>::equals(normalizedB.dot(), T(1)),
                           "Math::Vector::angle(): vectors must be normalized", std::numeric_limits<T>::quiet_NaN());
            return std::acos(dot(normalizedA, normalizedB));
        }

        /** @brief Default constructor */
        inline constexpr /*implicit*/ Vector() {}

        /** @todo Creating Vector from combination of vector and scalar types */

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr /*implicit*/ Vector(T first, U... next): RectangularMatrix<1, size, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr /*implicit*/ Vector(T first, U... next);
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
            for(std::size_t i = 0; i != size; ++i)
                (*this)[i] = value;
        }

        /** @brief Copy constructor */
        inline constexpr Vector(const RectangularMatrix<1, size, T>& other): RectangularMatrix<1, size, T>(other) {}

        /** @brief Value at given position */
        inline T& operator[](std::size_t pos) { return RectangularMatrix<1, size, T>::_data[pos]; }
        inline constexpr T operator[](std::size_t pos) const { return RectangularMatrix<1, size, T>::_data[pos]; } /**< @overload */

        /**
         * @brief Component-wise less than
         * @return `True` if all components are smaller than their
         *      counterparts in @p other, `false` otherwise
         */
        inline bool operator<(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if((*this)[i] >= other[i]) return false;

            return true;
        }

        /**
         * @brief Component-wise less than or equal
         * @return `True` if all components are smaller than or equal to their
         *      counterparts in @p other, `false` otherwise
         */
        inline bool operator<=(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if((*this)[i] > other[i]) return false;

            return true;
        }

        /**
         * @brief Component-wise greater than or equal
         * @return `True` if all components are larger than or equal to their
         *      counterparts in @p other, `false` otherwise
         */
        inline bool operator>=(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if((*this)[i] < other[i]) return false;

            return true;
        }

        /**
         * @brief Component-wise greater than
         * @return `True` if all components are larger than their
         *      counterparts in @p other, `false` otherwise
         */
        inline bool operator>(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if((*this)[i] <= other[i]) return false;

            return true;
        }

        /**
         * @brief Multiply vector component-wise
         *
         * @see operator*=(const Vector<size, U>&)
         */
        template<class U> inline Vector<size, T> operator*(const Vector<size, U>& other) const {
            return Vector<size, T>(*this)*=other;
        }

        /**
         * @brief Multiply vector component-wise and assign
         *
         * More efficient than operator*(const Vector<size, U>&) const,
         * because it does the computation in-place.
         */
        template<class U> Vector<size, T>& operator*=(const Vector<size, U>& other) {
            for(std::size_t i = 0; i != size; ++i)
                (*this)[i] *= other[i];

            return *this;
        }

        /**
         * @brief Divide vector component-wise
         *
         * @see operator/=(const Vector<size, U>&)
         */
        template<class U> inline Vector<size, T> operator/(const Vector<size, U>& other) const {
            return Vector<size, T>(*this)/=other;
        }

        /**
         * @brief Divide vector component-wise and assign
         *
         * More efficient than operator/(const Vector<size, U>&) const,
         * because it does the computation in-place.
         */
        template<class U> Vector<size, T>& operator/=(const Vector<size, U>& other) {
            for(std::size_t i = 0; i != size; ++i)
                (*this)[i] /= other[i];

            return *this;
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

        /**
         * @brief Vector projected onto another
         *
         * Returns vector projected onto line defined by @p other.
         */
        inline Vector<size, T> projected(const Vector<size, T>& other) const {
            return other*dot(*this, other)/other.dot();
        }

        /** @brief Sum of values in the vector */
        T sum() const {
            T out(0);

            for(std::size_t i = 0; i != size; ++i)
                out += (*this)[i];

            return out;
        }

        /** @brief Product of values in the vector */
        T product() const {
            T out(1);

            for(std::size_t i = 0; i != size; ++i)
                out *= (*this)[i];

            return out;
        }

        /** @brief Minimal value in the vector */
        T min() const {
            T out((*this)[0]);

            for(std::size_t i = 1; i != size; ++i)
                out = std::min(out, (*this)[i]);

            return out;
        }

        /** @brief Maximal value in the vector */
        T max() const {
            T out((*this)[0]);

            for(std::size_t i = 1; i != size; ++i)
                out = std::max(out, (*this)[i]);

            return out;
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Reimplementation of functions to return correct type */
        template<std::size_t otherCols> inline RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, 1, T>& other) const {
            return RectangularMatrix<1, size, T>::operator*(other);
        }
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(1, size, Vector<size, T>)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, size, Vector<size, T>)
        #endif

    private:
        /* Hiding unused things from RectangularMatrix */
        using RectangularMatrix<1, size, T>::Cols;
        using RectangularMatrix<1, size, T>::Rows;
        using RectangularMatrix<1, size, T>::operator[];
        using RectangularMatrix<1, size, T>::operator();
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::size_t size, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator*(U number, const Vector<size, T>& vector) {
    return number*RectangularMatrix<1, size, T>(vector);
}
template<std::size_t size, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator/(U number, const Vector<size, T>& vector) {
    return number/RectangularMatrix<1, size, T>(vector);
}
#endif

/** @debugoperator{Magnum::Math::Vector} */
template<std::size_t size, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector<size, T>& value) {
    debug << "Vector(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(std::size_t i = 0; i != size; ++i) {
        if(i != 0) debug << ", ";
        debug << typename MathTypeTraits<T>::NumericType(value[i]);
    }
    debug << ')';
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for types used in OpenGL */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, unsigned int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, unsigned int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, unsigned int>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, double>&);
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Type, size)                   \
    inline constexpr static Type<T>& from(T* data) {                        \
        return *reinterpret_cast<Type<T>*>(data);                           \
    }                                                                       \
    inline constexpr static const Type<T>& from(const T* data) {            \
        return *reinterpret_cast<const Type<T>*>(data);                     \
    }                                                                       \
    template<class U> inline constexpr static Type<T> from(const Math::Vector<size, U>& other) { \
        return Math::Vector<size, T>::from(other);                          \
    }                                                                       \
                                                                            \
    inline Type<T>& operator=(const Type<T>& other) {                       \
        Math::Vector<size, T>::operator=(other);                            \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    template<std::size_t otherCols> inline Math::RectangularMatrix<otherCols, size, T> operator*(const Math::RectangularMatrix<otherCols, 1, T>& other) const { \
        return Math::Vector<size, T>::operator*(other);    \
    }                                                                       \
    template<class U> inline Type<T> operator*(const Math::Vector<size, U>& other) const { \
        return Math::Vector<size, T>::operator*(other);                     \
    }                                                                       \
    template<class U> inline Type<T>& operator*=(const Math::Vector<size, U>& other) { \
        Math::Vector<size, T>::operator*=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline Type<T> operator/(const Math::Vector<size, U>& other) const { \
        return Math::Vector<size, T>::operator/(other);                     \
    }                                                                       \
    template<class U> inline Type<T>& operator/=(const Math::Vector<size, U>& other) { \
        Math::Vector<size, T>::operator/=(other);                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline Type<T> normalized() const { return Math::Vector<size, T>::normalized(); } \
    inline Type<T> projected(const Math::Vector<size, T>& other) const {    \
        return Math::Vector<size, T>::projected(other);                     \
    }

#define MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Type, size)          \
    template<class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Type<T>>::type operator*(U number, const Type<T>& vector) { \
        return number*Math::Vector<size, T>(vector);                        \
    }                                                                       \
    template<class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Type<T>>::type operator/(U number, const Type<T>& vector) { \
        return number/Math::Vector<size, T>(vector);                        \
    }
#endif

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Vector} */
template<std::size_t size, class T> struct ConfigurationValue<Magnum::Math::Vector<size, T>>: public ConfigurationValue<Magnum::Math::RectangularMatrix<1, size, T>> {};

}}

#endif
