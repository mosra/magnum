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

#include "RectangularMatrix.h"

namespace Magnum { namespace Math {

template<size_t size, class T> class Vector;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    /* Implementation for Vector<size, T>::from(const Vector<size, U>&) */
    template<class T, class U, size_t ...sequence> inline constexpr Math::Vector<sizeof...(sequence), T> vectorFrom(Sequence<sequence...>, const Math::Vector<sizeof...(sequence), U>& vector) {
        return {T(vector[sequence])...};
    }
}
#endif

/**
@brief %Vector

@configurationvalueref{Magnum::Math::Vector}
@todo Constexprize all for loops
*/
template<size_t s, class T> class Vector: public RectangularMatrix<1, s, T> {
    public:
        const static size_t size = s;    /**< @brief %Vector size */

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
        inline constexpr Vector() {}

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Vector(T first, U... next): RectangularMatrix<1, size, T>(first, next...) {}
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
                (*this)[i] = value;
        }

        /** @brief Copy constructor */
        inline constexpr Vector(const RectangularMatrix<1, size, T>& other): RectangularMatrix<1, size, T>(other) {}

        /** @brief Value at given position */
        inline T& operator[](size_t pos) { return RectangularMatrix<1, size, T>::_data[pos]; }
        inline constexpr T operator[](size_t pos) const { return RectangularMatrix<1, size, T>::_data[pos]; } /**< @overload */

        /**
         * @brief Multiply vector
         *
         * @see operator*=(U), operator*(U, const Vector<size, T>&)
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator*(U number) const {
        #else
        template<class U> inline Vector<size, T> operator*(U number) const {
        #endif
            return Vector<size, T>(*this)*=number;
        }

        /**
         * @brief Multiply vector and assign
         *
         * More efficient than operator*(U) const, because it does the
         * computation in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>&>::type operator*=(U number) {
        #else
        template<class U> Vector<size, T>& operator*=(U number) {
        #endif
            for(size_t i = 0; i != size; ++i)
                (*this)[i] *= number;

            return *this;
        }

        /**
         * @brief Multiply vector component-wise
         *
         * @see operator*=(const Vector<size, T>&)
         */
        Vector<size, T> operator*(const Vector<size, T>& other) const {
            return Vector<size, T>(*this)*=other;
        }

        /**
         * @brief Multiply vector component-wise and assign
         *
         * More efficient than operator*(const Vector<size, T>&) const,
         * because it does the computation in-place.
         */
        Vector<size, T>& operator*=(const Vector<size, T>& other) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] *= other[i];

            return *this;
        }

        /**
         * @brief Divide vector
         *
         * @see operator/=(U), operator/(U, const Vector<size, T>&)
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator/(U number) const {
        #else
        template<class U> inline Vector<size, T> operator/(U number) const {
        #endif
            return Vector<size, T>(*this)/=number;
        }

        /**
         * @brief Divide vector and assign
         *
         * More efficient than operator/(U) const, because it does the
         * computation in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>&>::type operator/=(U number) {
        #else
        template<class U> Vector<size, T>& operator/=(U number) {
        #endif
            for(size_t i = 0; i != size; ++i)
                (*this)[i] /= number;

            return *this;
        }

        /**
         * @brief Divide vector component-wise
         *
         * @see operator/=(const Vector<size, T>&)
         */
        Vector<size, T> operator/(const Vector<size, T>& other) const {
            return Vector<size, T>(*this)/=other;
        }

        /**
         * @brief Divide vector component-wise and assign
         *
         * More efficient than operator/(const Vector<size, T>&) const,
         * because it does the computation in-place.
         */
        Vector<size, T>& operator/=(const Vector<size, T>& other) {
            for(size_t i = 0; i != size; ++i)
                (*this)[i] /= other[i];

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

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(1, size, Vector<size, T>)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, size, Vector<size, T>)

    private:
        /* Hiding unused things from RectangularMatrix */
        using RectangularMatrix<1, size, T>::cols;
        using RectangularMatrix<1, size, T>::rows;
        using RectangularMatrix<1, size, T>::operator[];
        using RectangularMatrix<1, size, T>::operator();
};

/** @relates Vector
@brief Multiply number with vector

@see Vector::operator*(U) const
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<size_t size, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator*(U number, const Vector<size, T>& vector) {
#else
template<size_t size, class T, class U> inline Vector<size, T> operator*(U number, const Vector<size, T>& vector) {
#endif
    return vector*number;
}

/** @relates Vector
@brief Divide vector with number and invert

Example:
@code
Vector<4, float> vec(1.0f, 2.0f, -4.0f, 8.0f);
Vector<4, float> another = 1.0f/vec; // {1.0f, 0.5f, -0.25f, 0.128f}
@endcode
@see Vector::operator/(U) const
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<size_t size, class T, class U> typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator/(U number, const Vector<size, T>& vector) {
#else
template<size_t size, class T, class U> Vector<size, T> operator/(U number, const Vector<size, T>& vector) {
#endif
    Vector<size, T> out;

    for(size_t i = 0; i != size; ++i)
        out[i] = number/vector[i];

    return out;
}

/** @debugoperator{Magnum::Math::Vector} */
template<size_t size, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector<size, T>& value) {
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
    inline Type<T> operator*(const Vector<size, T>& other) const {          \
        return Vector<size, T>::operator*(other);                           \
    }                                                                       \
    inline Type<T>& operator*=(const Vector<size, T>& other) {              \
        Vector<size, T>::operator*=(other);                                 \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline Type<T> operator/(U number) const {            \
        return Vector<size, T>::operator/(number);                          \
    }                                                                       \
    template<class U> inline Type<T>& operator/=(U number) {                \
        Vector<size, T>::operator/=(number);                                \
        return *this;                                                       \
    }                                                                       \
    inline Type<T> operator/(const Vector<size, T>& other) const {          \
        return Vector<size, T>::operator/(other);                           \
    }                                                                       \
    inline Type<T>& operator/=(const Vector<size, T>& other) {              \
        Vector<size, T>::operator/=(other);                                 \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline Type<T> operator-() const { return Vector<size, T>::operator-(); } \
    inline Type<T> normalized() const { return Vector<size, T>::normalized(); }

#define MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Type, size)          \
    template<class T, class U> inline Type<T> operator*(U number, const Type<T>& vector) { \
        return number*Vector<size, T>(vector);                              \
    }                                                                       \
    template<class T, class U> inline Type<T> operator/(U number, const Type<T>& vector) { \
        return number/Vector<size, T>(vector);                              \
    }
#endif

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Vector} */
template<size_t size, class T> struct ConfigurationValue<Magnum::Math::Vector<size, T>> {
    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Vector<size, T>& value, int flags = 0) {
        std::string output;

        for(size_t pos = 0; pos != size; ++pos) {
            if(!output.empty()) output += ' ';
            output += ConfigurationValue<T>::toString(value[pos], flags);
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Vector<size, T> fromString(const std::string& stringValue, int flags = 0) {
        Magnum::Math::Vector<size, T> result;
        std::istringstream in(stringValue);

        std::string num;
        for(size_t pos = 0; pos != size; ++pos) {
            in >> num;
            result[pos] = ConfigurationValue<T>::fromString(num, flags);
        }

        return result;
    }
};

}}

#endif
