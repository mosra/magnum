#ifndef Magnum_Math_RectangularMatrix_h
#define Magnum_Math_RectangularMatrix_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Math::RectangularMatrix
 */

#include "Math/Vector.h"

namespace Magnum { namespace Math {

/**
@brief Rectangular matrix
@tparam cols    Column count
@tparam rows    Row count
@tparam T       Underlying data type

See @ref matrix-vector for brief introduction. See also Matrix (square) and
Vector.

The data are stored in column-major order, to reflect that, all indices in
math formulas are in reverse order (i.e. @f$ \boldsymbol A_{ji} @f$ instead
of @f$ \boldsymbol A_{ij} @f$).
@see Magnum::Matrix2x3, Magnum::Matrix3x2, Magnum::Matrix2x4, Magnum::Matrix4x2,
    Magnum::Matrix3x4, Magnum::Matrix4x3, Magnum::Matrix2x3d, Magnum::Matrix3x2d,
    Magnum::Matrix2x4d, Magnum::Matrix4x2d, Magnum::Matrix3x4d, Magnum::Matrix4x3d
*/
template<std::size_t cols, std::size_t rows, class T> class RectangularMatrix {
    static_assert(cols != 0 && rows != 0, "RectangularMatrix cannot have zero elements");

    template<std::size_t, std::size_t, class> friend class RectangularMatrix;

    public:
        typedef T Type;                         /**< @brief Underlying data type */
        const static std::size_t Cols = cols;   /**< @brief %Matrix column count */
        const static std::size_t Rows = rows;   /**< @brief %Matrix row count */

        /**
         * @brief Size of matrix diagonal
         *
         * @see fromDiagonal(), diagonal()
         */
        const static std::size_t DiagonalSize = (cols < rows ? cols : rows);

        /**
         * @brief %Matrix from array
         * @return Reference to the data as if it was Matrix, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        inline constexpr static RectangularMatrix<cols, rows, T>& from(T* data) {
            return *reinterpret_cast<RectangularMatrix<cols, rows, T>*>(data);
        }
        /** @overload */
        inline constexpr static const RectangularMatrix<cols, rows, T>& from(const T* data) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(data);
        }

        /**
         * @brief Construct diagonal matrix
         *
         * @see diagonal()
         * @todo make this constexpr
         */
        inline static RectangularMatrix<cols, rows, T> fromDiagonal(const Vector<DiagonalSize, T>& diagonal) {
            RectangularMatrix<cols, rows, T> out;

            for(std::size_t i = 0; i != DiagonalSize; ++i)
                out[i][i] = diagonal[i];

            return out;
        }

        /** @brief Construct zero-filled matrix */
        inline constexpr /*implicit*/ RectangularMatrix() {}

        /**
         * @brief Construct matrix from column vectors
         * @param first First column vector
         * @param next  Next column vectors
         *
         * @todo Creating matrix from arbitrary combination of matrices with n rows
         */
        template<class ...U> inline constexpr /*implicit*/ RectangularMatrix(const Vector<rows, T>& first, const U&... next): _data{first, next...} {
            static_assert(sizeof...(next)+1 == cols, "Improper number of arguments passed to RectangularMatrix constructor");
        }

        /**
         * @brief Construct matrix from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * RectangularMatrix<4, 1, Float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * RectangularMatrix<4, 1, Byte> integral(floatingPoint);
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        #ifndef CORRADE_GCC46_COMPATIBILITY
        template<class U> inline constexpr explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other): RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), other) {}
        #else
        template<class U> inline explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other) {
            *this = RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), other);
        }
        #endif

        /** @brief Copy constructor */
        inline constexpr RectangularMatrix(const RectangularMatrix<cols, rows, T>&) = default;

        /** @brief Assignment operator */
        inline RectangularMatrix<cols, rows, T>& operator=(const RectangularMatrix<cols, rows, T>&) = default;

        /**
         * @brief Raw data
         * @return One-dimensional array of `size*size` length in column-major
         *      order.
         *
         * @see operator[]
         */
        inline T* data() { return _data[0].data(); }
        inline constexpr const T* data() const { return _data[0].data(); } /**< @overload */

        /**
         * @brief %Matrix column
         *
         * Particular elements can be accessed using Vector::operator[], e.g.:
         * @code
         * RectangularMatrix<4, 3, Float> m;
         * Float a = m[2][1];
         * @endcode
         *
         * @see row(), data()
         */
        inline Vector<rows, T>& operator[](std::size_t col) {
            return _data[col];
        }
        /** @overload */
        inline constexpr const Vector<rows, T>& operator[](std::size_t col) const {
            return _data[col];
        }

        /**
         * @brief %Matrix row
         *
         * Consider using transposed() when accessing rows frequently, as this
         * is slower than accessing columns due to the way the matrix is stored.
         * @see operator[]()
         */
        inline Vector<cols, T> row(std::size_t row) const {
            Vector<cols, T> out;

            for(std::size_t i = 0; i != cols; ++i)
                out[i] = _data[i][row];

            return out;
        }

        /** @brief Equality comparison */
        inline bool operator==(const RectangularMatrix<cols, rows, T>& other) const {
            for(std::size_t i = 0; i != cols; ++i)
                if(_data[i] != other._data[i]) return false;

            return true;
        }

        /**
         * @brief Non-equality operator
         *
         * @see Vector::operator<(), Vector::operator<=(), Vector::operator>=(),
         *      Vector::operator>()
         */
        inline bool operator!=(const RectangularMatrix<cols, rows, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Add and assign matrix
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \boldsymbol A_j + \boldsymbol B_j
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator+=(const RectangularMatrix<cols, rows, T>& other) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] += other._data[i];

            return *this;
        }

        /**
         * @brief Add matrix
         *
         * @see operator+=()
         */
        inline RectangularMatrix<cols, rows, T> operator+(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)+=other;
        }

        /**
         * @brief Negated matrix
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = -\boldsymbol A_j
         * @f]
         */
        RectangularMatrix<cols, rows, T> operator-() const {
            RectangularMatrix<cols, rows, T> out;

            for(std::size_t i = 0; i != cols; ++i)
                out._data[i] = -_data[i];

            return out;
        }

        /**
         * @brief Subtract and assign matrix
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \boldsymbol A_j - \boldsymbol B_j
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator-=(const RectangularMatrix<cols, rows, T>& other) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] -= other._data[i];

            return *this;
        }

        /**
         * @brief Subtract matrix
         *
         * @see operator-=()
         */
        inline RectangularMatrix<cols, rows, T> operator-(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)-=other;
        }

        /**
         * @brief Multiply matrix with number and assign
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = a \boldsymbol A_j
         * @f]
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>&>::type operator*=(U number) {
        #else
        template<class U> RectangularMatrix<cols, rows, T>& operator*=(U number) {
        #endif
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] *= number;

            return *this;
        }

        /**
         * @brief Multiply matrix with number
         *
         * @see operator*=(U), operator*(U, const RectangularMatrix<cols, rows, T>&)
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator*(U number) const {
        #else
        template<class U> inline RectangularMatrix<cols, rows, T> operator*(U number) const {
        #endif
            return RectangularMatrix<cols, rows, T>(*this)*=number;
        }

        /**
         * @brief Divide matrix with number and assign
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \frac{\boldsymbol A_j} a
         * @f]
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>&>::type operator/=(U number) {
        #else
        template<class U> RectangularMatrix<cols, rows, T>& operator/=(U number) {
        #endif
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] /= number;

            return *this;
        }

        /**
         * @brief Divide matrix with number
         *
         * @see operator/=(), operator/(U, const RectangularMatrix<cols, rows, T>&)
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator/(U number) const {
        #else
        template<class U> inline RectangularMatrix<cols, rows, T> operator/(U number) const {
        #endif
            return RectangularMatrix<cols, rows, T>(*this)/=number;
        }

        /**
         * @brief Multiply matrix
         *
         * @f[
         *      (\boldsymbol {AB})_{ji} = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol B_{jk}
         * @f]
         */
        template<std::size_t size> RectangularMatrix<size, rows, T> operator*(const RectangularMatrix<size, cols, T>& other) const {
            RectangularMatrix<size, rows, T> out;

            for(std::size_t col = 0; col != size; ++col)
                for(std::size_t row = 0; row != rows; ++row)
                    for(std::size_t pos = 0; pos != cols; ++pos)
                        out[col][row] += _data[pos][row]*other._data[col][pos];

            return out;
        }

        /**
         * @brief Multiply vector
         *
         * Internally the same as multiplying with one-column matrix, but
         * returns vector. @f[
         *      (\boldsymbol {Aa})_i = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol a_k
         * @f]
         */
        Vector<rows, T> operator*(const Vector<rows, T>& other) const {
            return operator*(RectangularMatrix<1, rows, T>(other))[0];
        }

        /**
         * @brief Transposed matrix
         *
         * @see row()
         */
        RectangularMatrix<rows, cols, T> transposed() const {
            RectangularMatrix<rows, cols, T> out;

            for(std::size_t col = 0; col != cols; ++col)
                for(std::size_t row = 0; row != rows; ++row)
                    out[row][col] = _data[col][row];

            return out;
        }

        /**
         * @brief Values on diagonal
         *
         * @see fromDiagonal()
         */
        Vector<DiagonalSize, T> diagonal() const {
            Vector<DiagonalSize, T> out;

            for(std::size_t i = 0; i != DiagonalSize; ++i)
                out[i] = _data[i][i];

            return out;
        }

        /** @brief Sum of values in the matrix */
        T sum() const {
            T out(_data[0].sum());

            for(std::size_t i = 1; i != cols; ++i)
                out += _data[i].sum();

            return out;
        }

        /** @brief Product of values in the matrix */
        T product() const {
            T out(_data[0].product());

            for(std::size_t i = 1; i != cols; ++i)
                out *= _data[i].product();

            return out;
        }

        /** @brief Minimal value in the matrix */
        T min() const {
            T out(_data[0].min());

            for(std::size_t i = 1; i != cols; ++i)
                out = std::min(out, _data[i].min());

            return out;
        }

        /** @brief Minimal absolute value in the matrix */
        T minAbs() const {
            T out(_data[0].minAbs());

            for(std::size_t i = 1; i != cols; ++i)
                out = std::min(out, _data[i].minAbs());

            return out;
        }

        /** @brief Maximal value in the matrix */
        T max() const {
            T out(_data[0].max());

            for(std::size_t i = 1; i != cols; ++i)
                out = std::max(out, _data[i].max());

            return out;
        }

        /** @brief Maximal absolute value in the matrix */
        T maxAbs() const {
            T out(_data[0].maxAbs());

            for(std::size_t i = 1; i != cols; ++i)
                out = std::max(out, _data[i].maxAbs());

            return out;
        }

    private:
        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(const RectangularMatrix<cols, rows, U>&) */
        template<class U, std::size_t ...sequence> inline constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, const RectangularMatrix<cols, rows, U>& matrix): _data{Vector<rows, T>(matrix[sequence])...} {}

        Vector<rows, T> _data[cols];
};

/** @relates RectangularMatrix
@brief Multiply number with matrix

Same as RectangularMatrix::operator*(U) const.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t cols, std::size_t rows, class T, class U> inline RectangularMatrix<cols, rows, T> operator*(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#else
template<std::size_t cols, std::size_t rows, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator*(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#endif
    return matrix*number;
}

/** @relates RectangularMatrix
@brief Divide matrix with number and invert

The computation is done column-wise. @f[
    \boldsymbol B_j = \frac a {\boldsymbol A_j}
@f]
@see RectangularMatrix::operator/(U) const
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t cols, std::size_t rows, class T, class U> RectangularMatrix<cols, rows, T> operator/(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#else
template<std::size_t cols, std::size_t rows, class T, class U> typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator/(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#endif
    RectangularMatrix<cols, rows, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out[i] = number/matrix[i];

    return out;
}

/** @relates RectangularMatrix
@brief Multiply vector with rectangular matrix

Internally the same as multiplying one-column matrix with one-row matrix. @f[
    (\boldsymbol {aA})_{ji} = \boldsymbol a_i \boldsymbol A_j
@f]
@see RectangularMatrix::operator*(const RectangularMatrix<size, cols, T>&) const
*/
template<std::size_t size, std::size_t cols, class T> inline RectangularMatrix<cols, size, T> operator*(const Vector<size, T>& vector, const RectangularMatrix<cols, 1, T>& matrix) {
    return RectangularMatrix<1, size, T>(vector)*matrix;
}

/** @debugoperator{Magnum::Math::RectangularMatrix} */
template<std::size_t cols, std::size_t rows, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::RectangularMatrix<cols, rows, T>& value) {
    debug << "Matrix(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(std::size_t row = 0; row != rows; ++row) {
        if(row != 0) debug << ",\n       ";
        for(std::size_t col = 0; col != cols; ++col) {
            if(col != 0) debug << ", ";
            debug << value[col][row];
        }
    }
    debug << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Explicit instantiation for types used in OpenGL */
/* Square matrices */
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 3, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 4, Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 3, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 4, Double>&);
#endif

/* Rectangular matrices */
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 3, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 4, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 4, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 3, Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 3, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 4, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 4, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 3, Double>&);
#endif

#define MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(cols, rows, ...)   \
    inline constexpr static __VA_ARGS__& from(T* data) {                    \
        return *reinterpret_cast<__VA_ARGS__*>(data);                       \
    }                                                                       \
    inline constexpr static const __VA_ARGS__& from(const T* data) {        \
        return *reinterpret_cast<const __VA_ARGS__*>(data);                 \
    }                                                                       \
                                                                            \
    inline __VA_ARGS__& operator=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator=(other);           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline __VA_ARGS__ operator-() const {                                  \
        return Math::RectangularMatrix<cols, rows, T>::operator-();         \
    }                                                                       \
    inline __VA_ARGS__& operator+=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator+=(other);          \
        return *this;                                                       \
    }                                                                       \
    inline __VA_ARGS__ operator+(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator+(other);    \
    }                                                                       \
    inline __VA_ARGS__& operator-=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator-=(other);          \
        return *this;                                                       \
    }                                                                       \
    inline __VA_ARGS__ operator-(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator-(other);    \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__&>::type operator*=(U number) { \
        Math::RectangularMatrix<cols, rows, T>::operator*=(number);         \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__>::type operator*(U number) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator*(number);   \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__&>::type operator/=(U number) { \
        Math::RectangularMatrix<cols, rows, T>::operator/=(number);         \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__>::type operator/(U number) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator/(number);   \
    }
#endif

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::RectangularMatrix} */
template<std::size_t cols, std::size_t rows, class T> struct ConfigurationValue<Magnum::Math::RectangularMatrix<cols, rows, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::RectangularMatrix<cols, rows, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t row = 0; row != rows; ++row) {
            for(std::size_t col = 0; col != cols; ++col) {
                if(!output.empty()) output += ' ';
                output += ConfigurationValue<T>::toString(value[col][row], flags);
            }
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::RectangularMatrix<cols, rows, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::RectangularMatrix<cols, rows, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i%cols][i/cols] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos);

        return result;
    }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Square matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Float>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Double>>;
#endif

/* Rectangular matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Float>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Double>>;
#endif
#endif

}}

#endif
