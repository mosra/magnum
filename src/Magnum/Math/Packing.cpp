/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "Packing.h"

#include <emmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

namespace Magnum { namespace Math {

namespace {

union FloatBits {
    UnsignedInt u;
    Float f;
};

}

/* half_to_float_fast4() from https://gist.github.com/rygorous/2144712 */
Float unpackHalf(const UnsignedShort value) {
    constexpr const FloatBits Magic{113 << 23};
    /* Exponent mask after shift */
    constexpr const UnsignedInt ShiftedExp = 0x7c00 << 13;

    const UnsignedShort h{value};
    FloatBits o;

    o.u = (h & 0x7fff) << 13;                   /* exponent/mantissa bits */
    const UnsignedInt exp = ShiftedExp & o.u;   /* just the exponent */
    o.u += (127 - 15) << 23;                    /* exponent adjust */

    /* handle exponent special cases */
    if(exp == ShiftedExp) {                     /* Inf/NaN? */
        o.u += (128 - 16) << 23;                /* Extra exp adjust */
    } else if(exp == 0) {                       /* Zero/Denormal */
        o.u += 1 << 23;                         /* Extra exp adjust */
        o.f -= Magic.f;                         /* Renormalize */
    }

    o.u |= (h & 0x8000) << 16;                  /* sign bit */
    return o.f;
}

/* float_to_half_fast3() from https://gist.github.com/rygorous/2156668 */
UnsignedShort packHalf(const Float value) {
    constexpr const FloatBits FloatInfinity{255 << 23};
    constexpr const FloatBits HalfInfinity{31 << 23};
    constexpr const FloatBits Magic{15 << 23};
    constexpr const UnsignedInt SignMask = 0x80000000u;
    constexpr const UnsignedInt RoundMask = ~0xfffu;

    FloatBits f;
    f.f = value;
    UnsignedShort h;

    const UnsignedInt sign = f.u & SignMask;
    f.u ^= sign;

    /* Note: all the integer compares in this function can be safely compiled
       into signed compares since all operands are below 0x80000000. Important
       if you want fast straight SSE2 code (since there's no unsigned PCMPGTD). */

    /* Inf or NaN (all exponent bits set): NaN->qNaN and Inf->Inf */
    if(f.u >= FloatInfinity.u) {
        h = (f.u > FloatInfinity.u) ? 0x7e00 : 0x7c00;

    /* (De)normalized number or zero */
    } else {
        f.u &= RoundMask;
        f.f *= Magic.f;
        f.u -= RoundMask;

        /* Clamp to signed infinity if overflowed */
        if (f.u > HalfInfinity.u) f.u = HalfInfinity.u;

        /* Take the bits! */
        h = f.u >> 13;
    }

    h |= sign >> 16;
    return h;
}

namespace Implementation {

void unpackUnsignedByteToShort(Simd::NoneT, const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    for(std::size_t i = 0; i < in.size(); ++i) out[i] = in[i];
}

void unpackUnsignedByteToShort(Simd::Sse2T, const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    const __m128i* in128 = reinterpret_cast<const __m128i*>(in.data());
    __m128i* out128 = reinterpret_cast<__m128i*>(out.data());
    for(std::size_t i = 0; i < in.size()/16; ++i) {
        __m128i a = _mm_loadu_si128(in128 + i);
        _mm_storeu_si128(out128 + i*2 + 0, _mm_unpacklo_epi8(a, _mm_setzero_si128()));
        _mm_storeu_si128(out128 + i*2 + 1, _mm_unpacklo_epi8(a, _mm_setzero_si128()));
    }
}

void unpackUnsignedByteToShort(Simd::Sse41T, const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    const __m128i* in128 = reinterpret_cast<const __m128i*>(in.data());
    __m128i* out128 = reinterpret_cast<__m128i*>(out.data());
    for(std::size_t i = 0; i < in.size()/16; ++i) {
        __m128i a = _mm_loadu_si128(in128 + i);
        _mm_storeu_si128(out128 + i*2 + 0, _mm_cvtepu8_epi16(a));
        _mm_storeu_si128(out128 + i*2 + 1, _mm_cvtepu8_epi16(_mm_srli_si128(a, 8)));
    }
}

void unpackUnsignedByteToShort(Simd::Avx2T, const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    const __m128i* in128 = reinterpret_cast<const __m128i*>(in.data());
    __m256i* out256 = reinterpret_cast<__m256i*>(out.data());
    for(std::size_t i = 0; i < in.size()/16; ++i) {
        __m128i a = _mm_load_si128(in128 + i);
        _mm256_store_si256(out256 + i, _mm256_cvtepu8_epi16(a));
    }
}

}

namespace {

__attribute__ ((target ("default"))) void unpackUnsignedByteToShortDispatch(const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    Implementation::unpackUnsignedByteToShort(Simd::Sse2, in, out);
}

// TODO: why gcc complains about unused functions here?!
__attribute__ ((target ("sse2"))) void unpackUnsignedByteToShortDispatch(const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    Implementation::unpackUnsignedByteToShort(Simd::Sse2, in, out);
}

__attribute__ ((target ("sse4.1"))) void unpackUnsignedByteToShortDispatch(const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    Implementation::unpackUnsignedByteToShort(Simd::Sse41, in, out);
}

__attribute__ ((target ("avx2"))) void unpackUnsignedByteToShortDispatch(const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    Implementation::unpackUnsignedByteToShort(Simd::Sse41, in, out);
}

}

void unpackUnsignedByteToShort(const Corrade::Containers::ArrayView<const UnsignedByte> in, const Corrade::Containers::ArrayView<UnsignedShort> out) {
    CORRADE_ASSERT(in.size() == out.size(), "Math::unpackUnsignedByteToShort(): input has" << in.size() << "elements while output has" << out.size(), );
    CORRADE_ASSERT(!(reinterpret_cast<std::uintptr_t>(in.data())%16) && !(reinterpret_cast<std::uintptr_t>(in.data())%16), "Math::unpackUnsignedByteToShort(): the data are not 16-byte aligned", );

    /** @todo run only for a multiple of 16, do the rest scalar */
    CORRADE_INTERNAL_ASSERT(!(in.size()%16));
    unpackUnsignedByteToShortDispatch(in, out);
}

}}
