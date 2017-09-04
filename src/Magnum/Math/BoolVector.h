#ifndef Magnum_Math_BoolVector_h
#define Magnum_Math_BoolVector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::BoolVector
 */

#include <Corrade/configure.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Types.h"
#include "Magnum/Math/Tags.h"

namespace Magnum { namespace Math {

namespace Implementation {
    /** @todo C++14: use std::make_index_sequence and std::integer_sequence */
    template<std::size_t ...> struct Sequence {};

    #ifndef DOXYGEN_GENERATING_OUTPUT
    /* E.g. GenerateSequence<3>::Type is Sequence<0, 1, 2> */
    template<std::size_t N, std::size_t ...sequence> struct GenerateSequence:
        GenerateSequence<N-1, N-1, sequence...> {};

    template<std::size_t ...sequence> struct GenerateSequence<0, sequence...> {
        typedef Sequence<sequence...> Type;
    };
    #endif

    template<class T> constexpr T repeat(T value, std::size_t) { return value; }
}

/**
@brief Vector storing boolean values
@tparam size    Bit count

Result of component-wise comparison from Vector. The boolean values are stored
as bits in array of unsigned bytes, unused bits have undefined value which
doesn't affect comparison or @ref all() / @ref none() / @ref any() functions.
See also @ref matrix-vector for brief introduction.
*/
template<std::size_t size> class BoolVector {
    static_assert(size != 0, "BoolVector cannot have zero elements");

    public:
        enum: std::size_t {
            Size = size,                /**< Vector size */
            DataSize = (size-1)/8+1     /**< Vector storage size */
        };

        /** @brief Construct zero-filled boolean vector */
        constexpr /*implicit*/ BoolVector(ZeroInitT = ZeroInit) noexcept: _data{} {}

        /** @brief Construct without initializing the contents */
        explicit BoolVector(NoInitT) noexcept {}

        /**
         * @brief Construct boolean vector from segment values
         * @param first Value for first 8bit segment
         * @param next  Values for next Bbit segments
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...T> constexpr /*implicit*/ BoolVector(UnsignedByte first, T... next) noexcept;
        #else
        template<class ...T, class U = typename std::enable_if<sizeof...(T)+1 == DataSize, bool>::type> constexpr /*implicit*/ BoolVector(UnsignedByte first, T... next) noexcept: _data{first, UnsignedByte(next)...} {}
        #endif

        /** @brief Construct boolean vector with one value for all fields */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit BoolVector(T value) noexcept;
        #else
        template<class T, class U = typename std::enable_if<std::is_same<bool, T>::value && size != 1, bool>::type> constexpr explicit BoolVector(T value) noexcept: BoolVector(typename Implementation::GenerateSequence<DataSize>::Type(), value ? FullSegmentMask : 0) {}
        #endif

        /** @brief Copy constructor */
        constexpr /*implicit*/ BoolVector(const BoolVector<size>&) noexcept = default;

        /**
         * @brief Raw data
         * @return Array of DataSize length
         *
         * @see @ref operator[](), @ref set()
         */
        UnsignedByte* data() { return _data; }
        constexpr const UnsignedByte* data() const { return _data; } /**< @overload */

        /** @brief Bit at given position */
        constexpr bool operator[](std::size_t i) const {
            return (_data[i/8] >> i%8) & 0x01;
        }

        /** @brief Set bit at given position */
        BoolVector<size>& set(std::size_t i, bool value) {
            value ? _data[i/8] |=  (1 << i%8) :
                    _data[i/8] &= ~(1 << i%8);
            return *this;
        }

        /** @brief Equality comparison */
        bool operator==(const BoolVector<size>& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const BoolVector<size>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Boolean conversion
         *
         * Equivalent to @ref all().
         * @see @ref any(), @ref none()
         */
        explicit operator bool() const { return all(); }

        /**
         * @brief Whether all bits are set
         *
         * @see @ref none(), @ref any(), @ref operator bool()
         */
        bool all() const;

        /**
         * @brief Whether no bits are set
         *
         * @see @ref all(), @ref any(), @ref operator bool()
         */
        bool none() const;

        /**
         * @brief Whether any bit is set
         *
         * @see @ref all(), @ref none(), @ref operator bool()
         */
        bool any() const { return !none(); }

        /** @brief Bitwise inversion */
        BoolVector<size> operator~() const;

        /**
         * @brief Bitwise AND and assign
         *
         * The computation is done in-place.
         */
        BoolVector<size>& operator&=(const BoolVector<size>& other) {
            for(std::size_t i = 0; i != DataSize; ++i)
                _data[i] &= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise AND
         *
         * @see @ref operator&=()
         */
        BoolVector<size> operator&(const BoolVector<size>& other) const {
            return BoolVector<size>(*this) &= other;
        }

        /**
         * @brief Bitwise OR and assign
         *
         * The computation is done in-place.
         */
        BoolVector<size>& operator|=(const BoolVector<size>& other) {
            for(std::size_t i = 0; i != DataSize; ++i)
                _data[i] |= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise OR
         *
         * @see @ref operator|=()
         */
        BoolVector<size> operator|(const BoolVector<size>& other) const {
            return BoolVector<size>(*this) |= other;
        }

        /**
         * @brief Bitwise XOR and assign
         *
         * The computation is done in-place.
         */
        BoolVector<size>& operator^=(const BoolVector<size>& other) {
            for(std::size_t i = 0; i != DataSize; ++i)
                _data[i] ^= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise XOR
         *
         * @see @ref operator^=()
         */
        BoolVector<size> operator^(const BoolVector<size>& other) const {
            return BoolVector<size>(*this) ^= other;
        }

    private:
        enum: UnsignedByte {
            FullSegmentMask = 0xFF,
            LastSegmentMask = (1 << size%8) - 1
        };

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> constexpr explicit BoolVector(Implementation::Sequence<sequence...>, UnsignedByte value): _data{Implementation::repeat(value, sequence)...} {}

        UnsignedByte _data[(size-1)/8+1];
};

/** @debugoperator{Magnum::Math::BoolVector} */
template<std::size_t size> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const BoolVector<size>& value) {
    debug << "BoolVector(" << Corrade::Utility::Debug::nospace;
    for(std::size_t i = 0; i != size; ++i) {
        if(!i || (i%8)) debug << Corrade::Utility::Debug::nospace;
        debug << (value[i] ? "1" : "0");
    }
    return debug << Corrade::Utility::Debug::nospace << ")";
}

template<std::size_t size> inline bool BoolVector<size>::operator==(const BoolVector<size>& other) const {
    for(std::size_t i = 0; i != size/8; ++i)
        if(_data[i] != other._data[i]) return false;

    /* Check last segment */
    if(size%8 && (_data[DataSize-1] & LastSegmentMask) != (other._data[DataSize-1] & LastSegmentMask))
        return false;

    return true;
}

template<std::size_t size> inline bool BoolVector<size>::all() const {
    /* Check all full segments */
    for(std::size_t i = 0; i != size/8; ++i)
        if(_data[i] != FullSegmentMask) return false;

    /* Check last segment */
    if(size%8 && (_data[DataSize-1] & LastSegmentMask) != LastSegmentMask)
        return false;

    return true;
}

template<std::size_t size> inline bool BoolVector<size>::none() const {
    /* Check all full segments */
    for(std::size_t i = 0; i != size/8; ++i)
        if(_data[i]) return false;

    /* Check last segment */
    if(size%8 && (_data[DataSize-1] & LastSegmentMask))
        return false;

    return true;
}

template<std::size_t size> inline BoolVector<size> BoolVector<size>::operator~() const {
    BoolVector<size> out;

    for(std::size_t i = 0; i != DataSize; ++i)
        out._data[i] = ~_data[i];

    return out;
}

}}

#endif
