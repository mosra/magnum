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
#include <Utility/Assert.h>
#include <Utility/Debug.h>
#include <Utility/ConfigurationValue.h>

#include "Math/Angle.h"
#include "Math/BoolVector.h"
#include "Math/MathTypeTraits.h"

#include "magnumVisibility.h"

namespace Magnum { namespace Math {

/**
@brief %Vector
@tparam size    %Vector size
@tparam T       Underlying data type

See @ref matrix-vector for brief introduction.
@configurationvalueref{Magnum::Math::Vector}
*/
template<std::size_t size, class T> class Vector {
    static_assert(size != 0, "Vector cannot have zero elements");

    template<std::size_t, class> friend class Vector;

    public:
        typedef T Type;                         /**< @brief Underlying data type */
        const static std::size_t Size = size;   /**< @brief %Vector size */

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
         *      \boldsymbol a \cdot \boldsymbol b = \sum_{i=0}^{n-1} \boldsymbol a_i \boldsymbol b_i
         * @f]
         * @see dot() const
         */
        inline static T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
            return (a*b).sum();
        }

        /**
         * @brief Angle between normalized vectors
         *
         * Expects that both vectors are normalized. @f[
         *      \theta = acos \left( \frac{\boldsymbol a \cdot \boldsymbol b}{|\boldsymbol a| \cdot |\boldsymbol b|} \right) = acos (\boldsymbol a \cdot \boldsymbol b)
         * @f]
         */
        inline static Rad<T> angle(const Vector<size, T>& normalizedA, const Vector<size, T>& normalizedB) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normalizedA.dot(), T(1)) && MathTypeTraits<T>::equals(normalizedB.dot(), T(1)),
                           "Math::Vector::angle(): vectors must be normalized", Rad<T>(std::numeric_limits<T>::quiet_NaN()));
            return Rad<T>(std::acos(dot(normalizedA, normalizedB)));
        }

        /**
         * @brief Default constructor
         *
         * @f[
         *      \boldsymbol v = \boldsymbol 0
         * @f]
         */
        inline constexpr /*implicit*/ Vector(): _data() {}

        /** @todo Creating Vector from combination of vector and scalar types */

        /**
         * @brief Construct vector from values
         * @param first First value
         * @param next  Next values
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr /*implicit*/ Vector(T first, U... next);
        #else
        template<class ...U, class V = typename std::enable_if<sizeof...(U)+1 == size, T>::type> inline constexpr /*implicit*/ Vector(T first, U... next): _data{first, next...} {}
        #endif

        /** @brief Construct vector with one value for all fields */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        inline explicit Vector(T value);
        #else
        template<class U, class V = typename std::enable_if<std::is_same<T, U>::value && size != 1, T>::type> inline constexpr explicit Vector(U value): Vector(typename Implementation::GenerateSequence<size>::Type(), value) {}
        #endif

        /**
         * @brief Construct vector from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Vector<4, float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * Vector<4, std::int8_t> integral(floatingPoint);
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        template<class U> inline constexpr explicit Vector(const Vector<size, U>& other): Vector(typename Implementation::GenerateSequence<size>::Type(), other) {}

        /** @brief Copy constructor */
        inline constexpr Vector(const Vector<size, T>&) = default;

        /** @brief Assignment operator */
        inline Vector<size, T>& operator=(const Vector<size, T>&) = default;

        /**
         * @brief Raw data
         * @return One-dimensional array of `size*size` length in column-major
         *      order.
         *
         * @see operator[]
         */
        inline T* data() { return _data; }
        inline constexpr const T* data() const { return _data; } /**< @overload */

        /**
         * @brief Value at given position
         *
         * @see data()
         */
        inline T& operator[](std::size_t pos) { return _data[pos]; }
        inline constexpr T operator[](std::size_t pos) const { return _data[pos]; } /**< @overload */

        /** @brief Equality comparison */
        inline bool operator==(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if(!MathTypeTraits<T>::equals(_data[i], other._data[i])) return false;

            return true;
        }

        /** @brief Non-equality comparison */
        inline bool operator!=(const Vector<size, T>& other) const {
            return !operator==(other);
        }

        /** @brief Component-wise less than */
        inline BoolVector<size> operator<(const Vector<size, T>& other) const {
            BoolVector<size> out;

            for(std::size_t i = 0; i != size; ++i)
                out.set(i, _data[i] < other._data[i]);

            return out;
        }

        /** @brief Component-wise less than or equal */
        inline BoolVector<size> operator<=(const Vector<size, T>& other) const {
            BoolVector<size> out;

            for(std::size_t i = 0; i != size; ++i)
                out.set(i, _data[i] <= other._data[i]);

            return out;
        }

        /** @brief Component-wise greater than or equal */
        inline BoolVector<size> operator>=(const Vector<size, T>& other) const {
            BoolVector<size> out;

            for(std::size_t i = 0; i != size; ++i)
                out.set(i, _data[i] >= other._data[i]);

            return out;
        }

        /** @brief Component-wise greater than */
        inline BoolVector<size> operator>(const Vector<size, T>& other) const {
            BoolVector<size> out;

            for(std::size_t i = 0; i != size; ++i)
                out.set(i, _data[i] > other._data[i]);

            return out;
        }

        /**
         * @brief Negated vector
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = -\boldsymbol a_i
         * @f]
         */
        Vector<size, T> operator-() const {
            Vector<size, T> out;

            for(std::size_t i = 0; i != size; ++i)
                out._data[i] = -_data[i];

            return out;
        }

        /**
         * @brief Add and assign vector
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i + \boldsymbol b_i
         * @f]
         */
        Vector<size, T>& operator+=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] += other._data[i];

            return *this;
        }

        /**
         * @brief Add vector
         *
         * @see operator+=()
         */
        inline Vector<size, T> operator+(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) += other;
        }

        /**
         * @brief Subtract and assign vector
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i - \boldsymbol b_i
         * @f]
         */
        Vector<size, T>& operator-=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] -= other._data[i];

            return *this;
        }

        /**
         * @brief Subtract vector
         *
         * @see operator-=()
         */
        inline Vector<size, T> operator-(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) -= other;
        }

        /**
         * @brief Multiply vector with number and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = b \boldsymbol a_i
         * @f]
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class U> Vector<size, T>& operator*=(U number) {
        #else
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>&>::type operator*=(U number) {
        #endif
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= number;

            return *this;
        }

        /**
         * @brief Multiply vector with number
         *
         * @see operator*=(U), operator*(U, const Vector<size, T>&)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline Vector<size, T> operator*(U number) const {
        #else
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator*(U number) const {
        #endif
            return Vector<size, T>(*this) *= number;
        }

        /**
         * @brief Divide vector with number and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i} b
         * @f]
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class U> Vector<size, T>& operator/=(U number) {
        #else
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>&>::type operator/=(U number) {
        #endif
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= number;

            return *this;
        }

        /**
         * @brief Divide vector with number
         *
         * @see operator/=(), operator/(U, const Vector<size, T>&)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline Vector<size, T> operator/(U number) const {
        #else
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator/(U number) const {
        #endif
            return Vector<size, T>(*this) /= number;
        }

        /**
         * @brief Multiply vector component-wise and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i \boldsymbol b_i
         * @f]
         */
        template<class U> Vector<size, T>& operator*=(const Vector<size, U>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= other._data[i];

            return *this;
        }

        /**
         * @brief Multiply vector component-wise
         *
         * @see operator*=(const Vector<size, U>&)
         */
        template<class U> inline Vector<size, T> operator*(const Vector<size, U>& other) const {
            return Vector<size, T>(*this) *= other;
        }

        /**
         * @brief Divide vector component-wise and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i}{\boldsymbol b_i}
         * @f]
         */
        template<class U> Vector<size, T>& operator/=(const Vector<size, U>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= other._data[i];

            return *this;
        }

        /**
         * @brief Divide vector component-wise
         *
         * @see operator/=(const Vector<size, U>&)
         */
        template<class U> inline Vector<size, T> operator/(const Vector<size, U>& other) const {
            return Vector<size, T>(*this) /= other;
        }

        /**
         * @brief Dot product of the vector
         *
         * Should be used instead of length() for comparing vector length with
         * other values, because it doesn't compute the square root. @f[
         *      \boldsymbol a \cdot \boldsymbol a = \sum_{i=0}^{n-1} \boldsymbol a_i^2
         * @f]
         * @see dot(const Vector<size, T>&, const Vector<size, T>&)
         */
        inline T dot() const {
            return dot(*this, *this);
        }

        /**
         * @brief %Vector length
         *
         * See also dot() const which is faster for comparing length with other
         * values. @f[
         *      |\boldsymbol a| = \sqrt{\boldsymbol a \cdot \boldsymbol a}
         * @f]
         */
        inline T length() const {
            return std::sqrt(dot());
        }

        /** @brief Normalized vector (of length 1) */
        inline Vector<size, T> normalized() const {
            return *this/length();
        }

        /**
         * @brief %Vector projected onto line
         *
         * Returns vector projected onto @p line. @f[
         *      \boldsymbol a_1 = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b
         * @f]
         * @see projectedOntoNormalized()
         */
        inline Vector<size, T> projected(const Vector<size, T>& line) const {
            return line*dot(*this, line)/line.dot();
        }

        /**
         * @brief %Vector projected onto normalized line
         *
         * Slightly faster alternative to projected(), expects @p line to be
         * normalized. @f[
         *      \boldsymbol a_1 = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b =
         *          (\boldsymbol a \cdot \boldsymbol b) \boldsymbol b
         * @f]
         */
        inline Vector<size, T> projectedOntoNormalized(const Vector<size, T>& line) const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(line.dot(), T(1)), "Math::Vector::projectedOntoNormalized(): line must be normalized", (Vector<size, T>(std::numeric_limits<T>::quiet_NaN())));
            return line*dot(*this, line);
        }

        /** @brief Sum of values in the vector */
        T sum() const {
            T out(_data[0]);

            for(std::size_t i = 1; i != size; ++i)
                out += _data[i];

            return out;
        }

        /** @brief Product of values in the vector */
        T product() const {
            T out(_data[0]);

            for(std::size_t i = 1; i != size; ++i)
                out *= _data[i];

            return out;
        }

        /** @brief Minimal value in the vector */
        T min() const {
            T out(_data[0]);

            for(std::size_t i = 1; i != size; ++i)
                out = std::min(out, _data[i]);

            return out;
        }

        /** @brief Minimal absolute value in the vector */
        T minAbs() const {
            T out(std::abs(_data[0]));

            for(std::size_t i = 1; i != size; ++i)
                out = std::min(out, std::abs(_data[i]));

            return out;
        }

        /** @brief Maximal value in the vector */
        T max() const {
            T out(_data[0]);

            for(std::size_t i = 1; i != size; ++i)
                out = std::max(out, _data[i]);

            return out;
        }

        /** @brief Maximal absolute value in the vector */
        T maxAbs() const {
            T out(std::abs(_data[0]));

            for(std::size_t i = 1; i != size; ++i)
                out = std::max(out, std::abs(_data[i]));

            return out;
        }

    private:
        /* Implementation for Vector<size, T>::Vector(const Vector<size, U>&) */
        template<class U, std::size_t ...sequence> inline constexpr explicit Vector(Implementation::Sequence<sequence...>, const Vector<sizeof...(sequence), U>& vector): _data{T(vector._data[sequence])...} {}

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> inline constexpr explicit Vector(Implementation::Sequence<sequence...>, T value): _data{Implementation::repeat(value, sequence)...} {}

        T _data[size];
};

/** @relates Vector
@brief Multiply number with vector

Same as Vector::operator*(U) const.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t size, class T, class U> inline Vector<size, T> operator*(U number, const Vector<size, T>& vector) {
#else
template<std::size_t size, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator*(U number, const Vector<size, T>& vector) {
#endif
    return vector*number;
}

/** @relates Vector
@brief Divide vector with number and invert

@f[
    \boldsymbol c_i = \frac b {\boldsymbol a_i}
@f]
@see Vector::operator/()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t size, class T, class U> inline Vector<size, T> operator/(U number, const Vector<size, T>& vector) {
#else
template<std::size_t size, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Vector<size, T>>::type operator/(U number, const Vector<size, T>& vector) {
#endif
    Vector<size, T> out;

    for(std::size_t i = 0; i != size; ++i)
        out[i] = number/vector[i];

    return out;
}

/** @debugoperator{Magnum::Math::Vector} */
template<std::size_t size, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector<size, T>& value) {
    debug << "Vector(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(std::size_t i = 0; i != size; ++i) {
        if(i != 0) debug << ", ";
        debug << value[i];
    }
    debug << ")";
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
                                                                            \
    inline Type<T>& operator=(const Type<T>& other) {                       \
        Math::Vector<size, T>::operator=(other);                            \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline Type<T> operator-() const {                                      \
        return Math::Vector<size, T>::operator-();                          \
    }                                                                       \
    inline Type<T>& operator+=(const Math::Vector<size, T>& other) {        \
        Math::Vector<size, T>::operator+=(other);                           \
        return *this;                                                       \
    }                                                                       \
    inline Type<T> operator+(const Math::Vector<size, T>& other) const {    \
        return Math::Vector<size, T>::operator+(other);                     \
    }                                                                       \
    inline Type<T>& operator-=(const Math::Vector<size, T>& other) {        \
        Math::Vector<size, T>::operator-=(other);                           \
        return *this;                                                       \
    }                                                                       \
    inline Type<T> operator-(const Math::Vector<size, T>& other) const {    \
        return Math::Vector<size, T>::operator-(other);                     \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Type<T>&>::type operator*=(U number) { \
        Math::Vector<size, T>::operator*=(number);                          \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Type<T>>::type operator*(U number) const { \
        return Math::Vector<size, T>::operator*(number);                    \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Type<T>&>::type operator/=(U number) { \
        Math::Vector<size, T>::operator/=(number);                          \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, Type<T>>::type operator/(U number) const { \
        return Math::Vector<size, T>::operator/(number);                    \
    }                                                                       \
    template<class U> inline Type<T>& operator*=(const Math::Vector<size, U>& other) { \
        Math::Vector<size, T>::operator*=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline Type<T> operator*(const Math::Vector<size, U>& other) const { \
        return Math::Vector<size, T>::operator*(other);                     \
    }                                                                       \
    template<class U> inline Type<T>& operator/=(const Math::Vector<size, U>& other) { \
        Math::Vector<size, T>::operator/=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline Type<T> operator/(const Math::Vector<size, U>& other) const { \
        return Math::Vector<size, T>::operator/(other);                     \
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

/** @configurationvalue{Magnum::Math::RectangularMatrix} */
template<std::size_t size, class T> struct ConfigurationValue<Magnum::Math::Vector<size, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Vector<size, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t i = 0; i != size; ++i) {
            if(!output.empty()) output += ' ';
            output += ConfigurationValue<T>::toString(value[i], flags);
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Vector<size, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::Vector<size, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos);

        return result;
    }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Vectors */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, unsigned int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, unsigned int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, unsigned int>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, double>>;
#endif
#endif

}}

#endif
