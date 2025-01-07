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

#include "PackingBatch.h"

#include <Corrade/Containers/StridedArrayView.h>
#ifndef MAGNUM_SINGLES_NO_UTILITY_ALGORITHMS_DEPENDENCY
#include <Corrade/Utility/Algorithms.h>
#endif
#include <Corrade/Utility/Assert.h>

#include "Magnum/Math/Packing.h"
#include "Magnum/Math/Implementation/halfTables.hpp"

namespace Magnum { namespace Math {

namespace {

template<class T> inline void unpackUnsignedIntoImplementation(const Containers::StridedArrayView2D<const T>& src, const Containers::StridedArrayView2D<Float>& dst) {
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::unpackInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.template isContiguous<1>(),
        "Math::unpackInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.isContiguous<1>(),
        "Math::unpackInto(): second destination view dimension is not contiguous", );

    /** @todo SSE/NEON implementations exploiting second dimension size/stride,
        contiguity etc */

    /* Caching values to avoid inline function calls in ebug builds */
    constexpr Float bitMax = Implementation::bitMax<T>();
    const char* srcPtr = reinterpret_cast<const char*>(src.data());
    char* dstPtr = reinterpret_cast<char*>(dst.data());
    const std::ptrdiff_t srcStride = src.stride()[0];
    const std::ptrdiff_t dstStride = dst.stride()[0];
    const std::size_t maxJ = src.size()[1];
    for(std::size_t i = 0, maxI = src.size()[0]; i != maxI; ++i) {
        const T* srcPtrI = reinterpret_cast<const T*>(srcPtr);
        Float* dstPtrI = reinterpret_cast<Float*>(dstPtr);
        for(std::size_t j = 0; j != maxJ; ++j)
            *dstPtrI++ = *srcPtrI++/bitMax;

        srcPtr += srcStride;
        dstPtr += dstStride;
    }
}

}

void unpackInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<Float>& dst) {
    unpackUnsignedIntoImplementation(src, dst);
}

void unpackInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<Float>& dst) {
    unpackUnsignedIntoImplementation(src, dst);
}

namespace {

template<class T> inline void unpackSignedIntoImplementation(const Containers::StridedArrayView2D<const T>& src, const Containers::StridedArrayView2D<Float>& dst) {
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::unpackInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.template isContiguous<1>(),
        "Math::unpackInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.isContiguous<1>(),
        "Math::unpackInto(): second destination view dimension is not contiguous", );

    /** @todo SSE/NEON implementations exploiting second dimension size/stride,
        contiguity etc */

    /* Caching values to avoid inline function calls in debug builds */
    constexpr Float bitMax = Implementation::bitMax<T>();
    const char* srcPtr = reinterpret_cast<const char*>(src.data());
    char* dstPtr = reinterpret_cast<char*>(dst.data());
    const std::ptrdiff_t srcStride = src.stride()[0];
    const std::ptrdiff_t dstStride = dst.stride()[0];
    const std::size_t maxJ = src.size()[1];
    for(std::size_t i = 0, maxI = src.size()[0]; i != maxI; ++i) {
        const T* srcPtrI = reinterpret_cast<const T*>(srcPtr);
        Float* dstPtrI = reinterpret_cast<Float*>(dstPtr);
        for(std::size_t j = 0; j != maxJ; ++j) {
            const Float value = *srcPtrI++/bitMax;
            /* Avoiding a max() call in Debug */
            *dstPtrI++ = value < -1.0f ? -1.0f : value;
        }

        srcPtr += srcStride;
        dstPtr += dstStride;
    }
}

}

void unpackInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Float>& dst) {
    unpackSignedIntoImplementation(src, dst);
}

void unpackInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Float>& dst) {
    unpackSignedIntoImplementation(src, dst);
}

namespace {

template<class T> inline void packIntoImplementation(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<T>& dst) {
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::packInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.isContiguous<1>(),
        "Math::packInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.template isContiguous<1>(),
        "Math::packInto(): second destination view dimension is not contiguous", );

    /** @todo SSE/NEON implementations exploiting second dimension size/stride,
        contiguity etc */

    /* Caching values to avoid inline function calls in debug builds */
    constexpr Float bitMax = Implementation::bitMax<T>();
    const char* srcPtr = reinterpret_cast<const char*>(src.data());
    char* dstPtr = reinterpret_cast<char*>(dst.data());
    const std::ptrdiff_t srcStride = src.stride()[0];
    const std::ptrdiff_t dstStride = dst.stride()[0];
    const std::size_t maxJ = src.size()[1];
    for(std::size_t i = 0, maxI = src.size()[0]; i != maxI; ++i) {
        const Float* srcPtrI = reinterpret_cast<const Float*>(srcPtr);
        T* dstPtrI = reinterpret_cast<T*>(dstPtr);
        for(std::size_t j = 0; j != maxJ; ++j)
            /** @todo provide a version that doesn't do rounding */
            *dstPtrI++ = std::round(*srcPtrI++*bitMax);

        srcPtr += srcStride;
        dstPtr += dstStride;
    }
}

}

void packInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    packIntoImplementation(src, dst);
}

void packInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    packIntoImplementation(src, dst);
}

void packInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    packIntoImplementation(src, dst);
}

void packInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Short>& dst) {
    packIntoImplementation(src, dst);
}

namespace {

template<class T, class U> inline void castIntoImplementation(const Containers::StridedArrayView2D<const T>& src, const Containers::StridedArrayView2D<U>& dst) {
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::castInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.template isContiguous<1>(),
        "Math::castInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.template isContiguous<1>(),
        "Math::castInto(): second destination view dimension is not contiguous", );

    /** @todo SSE/NEON implementations exploiting second dimension size/stride,
        contiguity etc */

    /* Caching values to avoid inline function calls in debug buílds */
    const char* srcPtr = reinterpret_cast<const char*>(src.data());
    char* dstPtr = reinterpret_cast<char*>(dst.data());
    const std::ptrdiff_t srcStride = src.stride()[0];
    const std::ptrdiff_t dstStride = dst.stride()[0];
    const std::size_t maxJ = src.size()[1];
    for(std::size_t i = 0, maxI = src.size()[0]; i != maxI; ++i) {
        const T* srcPtrI = reinterpret_cast<const T*>(srcPtr);
        U* dstPtrI = reinterpret_cast<U*>(dstPtr);
        for(std::size_t j = 0; j != maxJ; ++j)
            *dstPtrI++ = U(*srcPtrI++);

        srcPtr += srcStride;
        dstPtr += dstStride;
    }
}

}

void castInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedInt>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Int>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedInt>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Int>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Short>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<UnsignedInt>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Int>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<Short>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<UnsignedInt>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<Int>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Short>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<UnsignedInt>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Int>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<UnsignedInt>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Int>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<UnsignedLong>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Long>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<UnsignedLong>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Long>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedInt>& src, const Containers::StridedArrayView2D<UnsignedLong>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Int>& src, const Containers::StridedArrayView2D<Long>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedLong>& src, const Containers::StridedArrayView2D<UnsignedInt>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Long>& src, const Containers::StridedArrayView2D<Int>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedLong>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Long>& src, const Containers::StridedArrayView2D<Short>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedLong>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Long>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedInt>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Int>& src, const Containers::StridedArrayView2D<Short>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedInt>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Int>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Double>& dst) {
    castIntoImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<Float>& dst) {
    castIntoImplementation(src, dst);
}

namespace {

#ifndef MAGNUM_SINGLES_NO_UTILITY_ALGORITHMS_DEPENDENCY
template<class T> inline void copyImplementation(const Containers::StridedArrayView2D<const T>& src, const Containers::StridedArrayView2D<T>& dst) {
    /* Utility::copy() has its own assertions, but those are debug-only for
       perf reasons and don't require the second dimension to be contiguous.
       Here the use case is different and the behavior should be consistent
       with other castInto() overloads to avoid surprises. */
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::castInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.template isContiguous<1>(),
        "Math::castInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.template isContiguous<1>(),
        "Math::castInto(): second destination view dimension is not contiguous", );

    Utility::copy(src, dst);
}
#else
/* Not as great because it doesn't have a fast memcpy codepath but more
   acceptable than excluding the APIs altogether */
template<class T> inline void copyImplementation(const Containers::StridedArrayView2D<const T>& src, const Containers::StridedArrayView2D<T>& dst) {
    castIntoImplementation(src, dst);
}
#endif

}

void castInto(const Containers::StridedArrayView2D<const UnsignedByte>& src, const Containers::StridedArrayView2D<UnsignedByte>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Byte>& src, const Containers::StridedArrayView2D<Byte>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Short>& src, const Containers::StridedArrayView2D<Short>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedInt>& src, const Containers::StridedArrayView2D<UnsignedInt>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Int>& src, const Containers::StridedArrayView2D<Int>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const UnsignedLong>& src, const Containers::StridedArrayView2D<UnsignedLong>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Long>& src, const Containers::StridedArrayView2D<Long>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<Float>& dst) {
    copyImplementation(src, dst);
}

void castInto(const Containers::StridedArrayView2D<const Double>& src, const Containers::StridedArrayView2D<Double>& dst) {
    copyImplementation(src, dst);
}

static_assert(sizeof(HalfMantissaTable) + sizeof(HalfOffsetTable) + sizeof(HalfExponentTable) == 8576,
    "improper size of half->float conversion tables");

static_assert(sizeof(HalfBaseTable) + sizeof(HalfShiftTable) == 1536,
    "improper size of float->half conversion tables");

void unpackHalfInto(const Containers::StridedArrayView2D<const UnsignedShort>& src, const Containers::StridedArrayView2D<Float>& dst) {
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::unpackHalfInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.isContiguous<1>(),
        "Math::unpackHalfInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.isContiguous<1>(),
        "Math::unpackHalfInto(): second destination view dimension is not contiguous", );

    /* Caching values to avoid inline function calls in debug builds */
    const char* srcPtr = reinterpret_cast<const char*>(src.data());
    char* dstPtr = reinterpret_cast<char*>(dst.data());
    const std::ptrdiff_t srcStride = src.stride()[0];
    const std::ptrdiff_t dstStride = dst.stride()[0];
    const std::size_t maxJ = src.size()[1];
    for(std::size_t i = 0, maxI = src.size()[0]; i != maxI; ++i) {
        const UnsignedShort* srcPtrI = reinterpret_cast<const UnsignedShort*>(srcPtr);
        UnsignedInt* dstPtrI = reinterpret_cast<UnsignedInt*>(dstPtr);
        for(std::size_t j = 0; j != maxJ; ++j) {
            const UnsignedShort h = *srcPtrI++;
            *dstPtrI++ = HalfMantissaTable[HalfOffsetTable[h >> 10] + (h & 0x3ff)] + HalfExponentTable[h >> 10];
        }

        srcPtr += srcStride;
        dstPtr += dstStride;
    }
}

void packHalfInto(const Containers::StridedArrayView2D<const Float>& src, const Containers::StridedArrayView2D<UnsignedShort>& dst) {
    CORRADE_ASSERT(src.size() == dst.size(),
        "Math::packHalfInto(): wrong destination size, got" << dst.size() << "but expected" << src.size(), );
    CORRADE_ASSERT(src.isContiguous<1>(),
        "Math::packHalfInto(): second source view dimension is not contiguous", );
    CORRADE_ASSERT(dst.isContiguous<1>(),
        "Math::packHalfInto(): second destination view dimension is not contiguous", );

    /* Caching values to avoid inline function calls in debug builds */
    const char* srcPtr = reinterpret_cast<const char*>(src.data());
    char* dstPtr = reinterpret_cast<char*>(dst.data());
    const std::ptrdiff_t srcStride = src.stride()[0];
    const std::ptrdiff_t dstStride = dst.stride()[0];
    const std::size_t maxJ = src.size()[1];
    for(std::size_t i = 0, maxI = src.size()[0]; i != maxI; ++i) {
        const UnsignedInt* srcPtrI = reinterpret_cast<const UnsignedInt*>(srcPtr);
        UnsignedShort* dstPtrI = reinterpret_cast<UnsignedShort*>(dstPtr);
        for(std::size_t j = 0; j != maxJ; ++j) {
            const UnsignedInt f = *srcPtrI++;
            *dstPtrI++ = HalfBaseTable[(f >> 23) & 0x1ff] + ((f & 0x007fffff) >> HalfShiftTable[(f >> 23) & 0x1ff]);
        }

        srcPtr += srcStride;
        dstPtr += dstStride;
    }
}

}}
