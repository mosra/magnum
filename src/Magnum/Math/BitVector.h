#ifndef Magnum_Math_BitVector_h
#define Magnum_Math_BitVector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Math::BitVector
 * @m_since_latest
 */

/* std::declval() is said to be in <utility> but libstdc++, libc++ and MSVC STL
   all have it directly in <type_traits> because it just makes sense */
#include <type_traits>
#include <Corrade/Containers/sequenceHelpers.h>
#ifndef CORRADE_SINGLES_NO_DEBUG
#include <Corrade/Utility/Debug.h>
#endif

#include "Magnum/Magnum.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Math/Tags.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t, class> struct BitVectorConverter;

    template<class T> constexpr T repeat(T value, std::size_t) { return value; }
}

/**
@brief Vector of bits
@tparam size    Bit count
@m_since_latest

Result of component-wise comparison from @ref Vector. The boolean values are
stored as bits in array of unsigned bytes, unused bits have undefined value
which doesn't affect comparison or @ref all() / @ref none() / @ref any()
functions. See also @ref matrix-vector for brief introduction.

@section Math-BitVector-indexing Bit indexing

Value at position 0 is the lowest bit of the first byte passed in constructor.
Value at position 8 is the lowest bit of the second byte passed in constructor.
For example:

@snippet Math-cpp14.cpp BitVector-indexing

@section Math-BitVector-boolean Boolean operations

The class implements @cpp && @ce, @cpp || @ce and @cpp ! @ce operators
component-wise, in other words equivalently to @cpp & @ce, @cpp | @ce and
@cpp ~ @ce. This is done in order to have consistent behavior with boolean
operations on scalar types --- in the following example, it causes the final
conversion to @cpp bool @ce to be done at the end (instead of it happening
already in the boolean subexpressions). Combined with @ref operator bool()
returning @cpp true @ce only if all bits are set, this means the condition will
be passed only if @cpp b @ce is around @cpp a @ce in *all dimensions*, and work
the same way as if the variables were just scalars:

@snippet Math.cpp BitVector-boolean

@see @ref Magnum::BitVector2, @ref Magnum::BitVector3, @ref Magnum::BitVector4
*/
template<std::size_t size> class BitVector {
    static_assert(size != 0, "BitVector cannot have zero elements");

    public:
        enum: std::size_t {
            Size = size,                /**< Vector size */
            DataSize = (size + 7)/8     /**< Vector storage size */
        };

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref BitVector(ZeroInitT).
         */
        constexpr /*implicit*/ BitVector() noexcept: _data{} {}

        /** @brief Construct a zero-filled bit vector */
        constexpr explicit BitVector(ZeroInitT) noexcept: _data{} {}

        /** @brief Construct without initializing the contents */
        explicit BitVector(Magnum::NoInitT) noexcept {}

        /**
         * @brief Construct a bit vector from segment values
         * @param first Value for first 8bit segment
         * @param next  Values for next Bbit segments
         */
        template<class ...T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* For some reason MSVC 2015 and 2017 doesn't understand the value
               parameter here, only a type parameter. Don't need to prevent any
               ambiguity with this constructor so it's fine. */
            #ifndef CORRADE_MSVC2017_COMPATIBILITY
            , typename std::enable_if<sizeof...(T)+1 == DataSize, int>::type = 0
            #else
            , class = typename std::enable_if<sizeof...(T)+1 == DataSize>::type
            #endif
            #endif
        > constexpr /*implicit*/ BitVector(UnsignedByte first, T... next) noexcept: _data{first, UnsignedByte(next)...} {}

        /** @brief Construct a bit vector with one value for all fields */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_same<bool, T>::value && size != 1, int>::type = 0
            #endif
        > constexpr explicit BitVector(T value) noexcept: BitVector(typename Containers::Implementation::GenerateSequence<DataSize>::Type{}, value ? FullSegmentMask : 0) {}

        /** @brief Construct a bit vector from external representation */
        template<class U, class = decltype(Implementation::BitVectorConverter<size, U>::from(std::declval<U>()))> constexpr explicit BitVector(const U& other) noexcept: BitVector{Implementation::BitVectorConverter<size, U>::from(other)} {}

        /** @brief Convert the bit vector to external representation */
        template<class U, class = decltype(Implementation::BitVectorConverter<size, U>::to(std::declval<BitVector<size>>()))> constexpr explicit operator U() const {
            return Implementation::BitVectorConverter<size, U>::to(*this);
        }

        /**
         * @brief Raw data
         *
         * Contrary to what Doxygen shows, returns reference to an
         * one-dimensional fixed-size array of @ref DataSize elements, i.e.
         * @cpp UnsignedByte(&)[DataSize] @ce.
         * @see @ref operator[](), @ref set()
         * @todoc Fix once there's a possibility to patch the signature in a
         *      post-processing step (https://github.com/mosra/m.css/issues/56)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        UnsignedByte* data();
        constexpr const UnsignedByte* data() const; /**< @overload */
        #else
        auto data() -> UnsignedByte(&)[DataSize] { return _data; }
        constexpr auto data() const -> const UnsignedByte(&)[DataSize] { return _data; }
        #endif

        /** @brief Bit at given position */
        constexpr bool operator[](std::size_t i) const {
            return (_data[i/8] >> i%8) & 0x01;
        }

        /**
         * @brief Set a bit at given position
         * @m_since_latest
         */
        BitVector<size>& set(std::size_t i) {
            _data[i/8] |= 1 << i%8;
            return *this;
        }

        /**
         * @brief Reset a bit at given position
         * @m_since_latest
         */
        BitVector<size>& reset(std::size_t i) {
            _data[i/8] &= ~(1 << i%8);
            return *this;
        }

        /**
         * @brief Set or reset a bit at given position
         *
         * Prefer to use @ref set(std::size_t) and @ref reset(std::size_t)
         * where possible as that's a simpler operation.
         */
        BitVector<size>& set(std::size_t i, bool value) {
            value ? _data[i/8] |=  (1 << i%8) :
                    _data[i/8] &= ~(1 << i%8);
            return *this;
        }

        /** @brief Equality comparison */
        bool operator==(const BitVector<size>& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const BitVector<size>& other) const {
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
        BitVector<size> operator~() const;

        /**
         * @brief Component-wise boolean negation
         * @m_since{2019,10}
         *
         * Equivalent to @ref operator~(). See @ref Math-BitVector-boolean for
         * more information.
         */
        BitVector<size> operator!() const { return operator~(); }

        /**
         * @brief Bitwise AND and assign
         *
         * The computation is done in-place.
         */
        BitVector<size>& operator&=(const BitVector<size>& other) {
            for(std::size_t i = 0; i != DataSize; ++i)
                _data[i] &= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise AND
         *
         * @see @ref operator&=()
         */
        BitVector<size> operator&(const BitVector<size>& other) const {
            return BitVector<size>(*this) &= other;
        }

        /**
         * @brief Component-wise boolean AND
         * @m_since{2019,10}
         *
         * Equivalent to @ref operator&(). See @ref Math-BitVector-boolean for
         * more information.
         */
        BitVector<size> operator&&(const BitVector<size>& other) const {
            return BitVector<size>(*this) &= other;
        }

        /**
         * @brief Bitwise OR and assign
         *
         * The computation is done in-place.
         */
        BitVector<size>& operator|=(const BitVector<size>& other) {
            for(std::size_t i = 0; i != DataSize; ++i)
                _data[i] |= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise OR
         *
         * @see @ref operator|=()
         */
        BitVector<size> operator|(const BitVector<size>& other) const {
            return BitVector<size>(*this) |= other;
        }

        /**
         * @brief Component-wise boolean OR
         * @m_since{2019,10}
         *
         * Equivalent to @ref operator|(). See @ref Math-BitVector-boolean for
         * more information.
         */
        BitVector<size> operator||(const BitVector<size>& other) const {
            return BitVector<size>(*this) |= other;
        }

        /**
         * @brief Bitwise XOR and assign
         *
         * The computation is done in-place.
         */
        BitVector<size>& operator^=(const BitVector<size>& other) {
            for(std::size_t i = 0; i != DataSize; ++i)
                _data[i] ^= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise XOR
         *
         * @see @ref operator^=()
         */
        BitVector<size> operator^(const BitVector<size>& other) const {
            return BitVector<size>(*this) ^= other;
        }

    private:
        enum: UnsignedByte {
            FullSegmentMask = 0xFF,
            LastSegmentMask = (1 << size%8) - 1
        };

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> constexpr explicit BitVector(Containers::Implementation::Sequence<sequence...>, UnsignedByte value): _data{Implementation::repeat(value, sequence)...} {}

        UnsignedByte _data[DataSize];
};

#ifndef CORRADE_SINGLES_NO_DEBUG
/**
@debugoperator{BitVector}
@m_since_latest

In order to avoid potential confusion, prints the value as a comma-separated sequence of binary literals, so the output corresponds to how the value would
be constructed. For example,

@snippet Math-cpp14.cpp BitVector-debug

<b></b>

@m_class{m-noindent}

prints as

@code{.shell-session}
BitVector(0b1010) BitVector(0b00001000, 0b00000011, 0b100)
@endcode

Note that this, on the other hand, makes mapping to bit indices less obvious
--- see @ref Math-BitVector-indexing for more information.
*/
template<std::size_t size> Debug& operator<<(Debug& debug, const BitVector<size>& value) {
    debug << "BitVector(0b" << Debug::nospace;

    /* Print the full bytes comma-separated */
    for(std::size_t byte = 0; byte != BitVector<size>::DataSize - 1; ++byte) {
        for(std::size_t i = 0; i != 8; ++i)
            debug << (((value.data()[byte] >> (8 - i - 1)) & 1) ? "1" : "0")
                  << Debug::nospace;
        debug << ", 0b" << Debug::nospace;
    }

    /* Print the last (potentially) partial byte */
    constexpr std::size_t suffixSize = size%8 ? size%8 : 8;
    for(std::size_t i = 0; i != suffixSize; ++i)
        debug << (((value.data()[size/8] >> (suffixSize - i - 1)) & 1) ? "1" : "0")
              << Debug::nospace;

    return debug << ")";
}
#endif

template<std::size_t size> inline bool BitVector<size>::operator==(const BitVector<size>& other) const {
    for(std::size_t i = 0; i != size/8; ++i)
        if(_data[i] != other._data[i]) return false;

    /* Check last segment */
    if(size%8 && (_data[DataSize-1] & LastSegmentMask) != (other._data[DataSize-1] & LastSegmentMask))
        return false;

    return true;
}

template<std::size_t size> inline bool BitVector<size>::all() const {
    /* Check all full segments */
    for(std::size_t i = 0; i != size/8; ++i)
        if(_data[i] != FullSegmentMask) return false;

    /* Check last segment */
    if(size%8 && (_data[DataSize-1] & LastSegmentMask) != LastSegmentMask)
        return false;

    return true;
}

template<std::size_t size> inline bool BitVector<size>::none() const {
    /* Check all full segments */
    for(std::size_t i = 0; i != size/8; ++i)
        if(_data[i]) return false;

    /* Check last segment */
    if(size%8 && (_data[DataSize-1] & LastSegmentMask))
        return false;

    return true;
}

template<std::size_t size> inline BitVector<size> BitVector<size>::operator~() const {
    BitVector<size> out{Magnum::NoInit};

    for(std::size_t i = 0; i != DataSize; ++i)
        out._data[i] = ~_data[i];

    return out;
}

#ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
namespace Implementation {

template<std::size_t size> struct StrictWeakOrdering<BitVector<size>> {
    bool operator()(const BitVector<size>& a, const BitVector<size>& b) const {
        auto ad = a.data();
        auto bd = b.data();
        for(std::size_t i = 0; i < BitVector<size>::DataSize - 1; ++i) {
            if(ad[i] < bd[i])
                return true;
            if(ad[i] > bd[i])
                return false;
        }

        /* Mask last element with to hide padding bits */
        constexpr UnsignedByte mask = UnsignedByte(0xFF) >> (BitVector<size>::DataSize * 8 - size);
        constexpr std::size_t i = BitVector<size>::DataSize - 1;
        return (ad[i] & mask) < (bd[i] & mask);
    }
};

}
#endif

}}

#endif
