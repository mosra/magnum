#ifndef Magnum_SceneTools_Test_HierarchyTest_h
#define Magnum_SceneTools_Test_HierarchyTest_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
/* Contains additional variants of SceneTools hierarchy algorithms that are
   included just for historical / testing / benchmark comparison purposes;
   referenced explicitly via function pointers from the respective test case
   and benchmark instances. Used by HierarchyTest.cpp and
   HierarchyBenchmark.cpp. */

#include <Corrade/Cpu.h>
#ifdef CORRADE_ENABLE_AVX
#include <Corrade/Utility/IntrinsicsAvx.h>
#elif defined(CORRADE_ENABLE_SSE2)
#include <Corrade/Utility/IntrinsicsSse2.h>
#endif

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

/* Variant of the scalar implementation that doesn't do the full 4x4 matrix
   multiply but just an affine transformation. Theoretically should be faster
   as it does less multiplications, but in practice is slower, likely because
   the compiler isn't able to autovectorize this. */
void absoluteTransformations3DScalarAffine(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Matrix4& a = *reinterpret_cast<const Matrix4*>(output + (parent + 1)*outputStride);
        const Matrix4& b = *reinterpret_cast<const Matrix4*>(transformations + i*transformationsStride);
        Matrix4& c = *reinterpret_cast<Matrix4*>(output + (i + 1)*outputStride);

        const Matrix3x3 aRotationScaling = a.rotationScaling();
        c = Matrix4::from(aRotationScaling*b.rotationScaling(),
                          a.translation() + aRotationScaling*b.translation());
    }
}

#ifdef CORRADE_ENABLE_SSE2
/* Basic algorithm from https://stackoverflow.com/a/18508113 (slowest) */
CORRADE_ENABLE_SSE2 void absoluteTransformations3DSse2NoShuffleNoCacheNoAffine(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        const __m128 a0 = _mm_load_ps(a + 0);
        const __m128 a1 = _mm_load_ps(a + 4);
        const __m128 a2 = _mm_load_ps(a + 8);
        const __m128 a3 = _mm_load_ps(a + 12);
        for(std::size_t j = 0; j != 4; ++j) {
            /* Elements b[j][0] to b[j][3], four separate scalar loads instead
               of _mm_load_ps() + four _mm_shuffle_ps() */
            const __m128 bj0x4 = _mm_set1_ps(b[j*4 + 0]);
            const __m128 bj1x4 = _mm_set1_ps(b[j*4 + 1]);
            const __m128 bj2x4 = _mm_set1_ps(b[j*4 + 2]);
            const __m128 bj3x4 = _mm_set1_ps(b[j*4 + 3]);

            _mm_store_ps(c + j*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, bj0x4),
                    _mm_mul_ps(a1, bj1x4)),
                _mm_add_ps(
                    _mm_mul_ps(a2, bj2x4),
                    _mm_mul_ps(a3, bj3x4))));
        }
    }
}

/* Like above, with a single load and four shuffles instead of four loads.
   Single vector load plus four shuffles is faster than four separate scalar
   broadcasts with _mm_set1_ps(). Note that the suffles can be still beaten
   with something faster, which is used in the builtin variant. */
CORRADE_ENABLE_SSE2 void absoluteTransformations3DSse2TwoRegisterShuffleNoCacheNoAffine(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        /* Columns a[0] to a[3] */
        const __m128 a0 = _mm_load_ps(a + 0);
        const __m128 a1 = _mm_load_ps(a + 4);
        const __m128 a2 = _mm_load_ps(a + 8);
        const __m128 a3 = _mm_load_ps(a + 12);
        for(std::size_t j = 0; j != 4; ++j) {
            /* Elements b[j][0] to b[j][3], repeated four times each */
            const __m128 bj = _mm_load_ps(b + j*4);
            const __m128 bj0x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(0, 0, 0, 0));
            const __m128 bj1x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(1, 1, 1, 1));
            const __m128 bj2x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(2, 2, 2, 2));
            const __m128 bj3x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(3, 3, 3, 3));

            /* Calculate the whole output row at once:
                c[j] = a[0]*b[j][0] + a[1]*b[j][1] +
                       a[2]*b[j][2] + a[3]*b[j][3]
               In other words, for k being 0 to 3:
                c[j][k] = a[0][k]*b[j][0] + a[1][k]*b[j][1] +
                          a[2][k]*b[j][2] + a[3][k]*b[j][3] */
            _mm_store_ps(c + j*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, bj0x4),
                    _mm_mul_ps(a1, bj1x4)),
                _mm_add_ps(
                    _mm_mul_ps(a2, bj2x4),
                    _mm_mul_ps(a3, bj3x4))));
        }
    }
}

/* Affine variant of the above */
CORRADE_ENABLE_SSE2 void absoluteTransformations3DSse2TwoRegisterShuffleNoCache(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        const __m128 a0 = _mm_load_ps(a + 0);
        const __m128 a1 = _mm_load_ps(a + 4);
        const __m128 a2 = _mm_load_ps(a + 8);
        const __m128 a3 = _mm_load_ps(a + 12);

        for(std::size_t j = 0; j != 3; ++j) {
            const __m128 bj = _mm_load_ps(b + j*4);
            const __m128 bj0x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(0, 0, 0, 0));
            const __m128 bj1x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(1, 1, 1, 1));
            const __m128 bj2x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(2, 2, 2, 2));

            _mm_store_ps(c + j*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, bj0x4),
                    _mm_mul_ps(a1, bj1x4)),
                _mm_mul_ps(a2, bj2x4)));
        }

        {
            const __m128 b3 = _mm_load_ps(b + 3*4);
            const __m128 b30x4 = _mm_shuffle_ps(b3, b3, _MM_SHUFFLE(0, 0, 0, 0));
            const __m128 b31x4 = _mm_shuffle_ps(b3, b3, _MM_SHUFFLE(1, 1, 1, 1));
            const __m128 b32x4 = _mm_shuffle_ps(b3, b3, _MM_SHUFFLE(2, 2, 2, 2));

            _mm_store_ps(c + 3*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, b30x4),
                    _mm_mul_ps(a1, b31x4)),
                _mm_add_ps(
                    _mm_mul_ps(a2, b32x4),
                    a3)));
        }
    }
}

/* The above but with caching the left side on the top. Compared to the builtin
   version uses two-register _mm_shuffle_ps() (SSE) instead of single-register
   SSE2 _mm_shuffle_epi32(). Is slightly slower. */
CORRADE_ENABLE_SSE2 void absoluteTransformations3DSse2TwoRegisterShuffle(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    __m128 a0{};
    __m128 a1{};
    __m128 a2{};
    __m128 a3{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);

            /* Columns `a[0]` to `a[3]` */
            a0 = _mm_load_ps(a + 0);
            a1 = _mm_load_ps(a + 4);
            a2 = _mm_load_ps(a + 8);
            a3 = _mm_load_ps(a + 12);
        }

        for(std::size_t j = 0; j != 3; ++j) {
            const __m128 bj = _mm_load_ps(b + j*4);
            const __m128 bj0x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(0, 0, 0, 0));
            const __m128 bj1x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(1, 1, 1, 1));
            const __m128 bj2x4 = _mm_shuffle_ps(bj, bj, _MM_SHUFFLE(2, 2, 2, 2));

            _mm_store_ps(c + j*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, bj0x4),
                    _mm_mul_ps(a1, bj1x4)),
                _mm_mul_ps(a2, bj2x4)));
        }

        {
            const __m128 b3 = _mm_load_ps(b + 3*4);
            const __m128 b30x4 = _mm_shuffle_ps(b3, b3, _MM_SHUFFLE(0, 0, 0, 0));
            const __m128 b31x4 = _mm_shuffle_ps(b3, b3, _MM_SHUFFLE(1, 1, 1, 1));
            const __m128 b32x4 = _mm_shuffle_ps(b3, b3, _MM_SHUFFLE(2, 2, 2, 2));

            _mm_store_ps(c + 3*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, b30x4),
                    _mm_mul_ps(a1, b31x4)),
                _mm_add_ps(
                    _mm_mul_ps(a2, b32x4),
                    a3)));
        }
    }
}
#endif

#ifdef CORRADE_ENABLE_AVX
/* Basically a direct translation of the no-affine SSE2 code above, processing
   two columns at once */
CORRADE_ENABLE_AVX void absoluteTransformations3DAvxNoCacheNoAffine(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        /* Columns `a[0]` to `a[3]`, twice in each register */
        const __m256 a01 = _mm256_load_ps(a + 0);
        const __m256 a23 = _mm256_load_ps(a + 8);
        const __m256 a0x2 = _mm256_permute2f128_ps(a01, a01, 0x00);
        const __m256 a1x2 = _mm256_permute2f128_ps(a01, a01, 0x11);
        const __m256 a2x2 = _mm256_permute2f128_ps(a23, a23, 0x00);
        const __m256 a3x2 = _mm256_permute2f128_ps(a23, a23, 0x11);

        /* Elements `b[0][i]` four times in a register followed by `b[1][i]`
           four times, then `b[2][i]` four times in a register followed by
           `b[3][i] four times */
        const __m256 b01 = _mm256_load_ps(b + 0);
        const __m256 b23 = _mm256_load_ps(b + 8);
        const __m256 b00x4_10x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b01x4_11x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b02x4_12x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(2, 2, 2, 2));
        const __m256 b03x4_13x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(3, 3, 3, 3));
        const __m256 b20x4_30x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b21x4_31x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b22x4_32x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(2, 2, 2, 2));
        const __m256 b23x4_33x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(3, 3, 3, 3));

        /* c[0] = a[0]*b[0][0] + a[1]*b[0][1] + a[2]*b[0][2] + a[3]*b[0][3]
           c[1] = a[0]*b[1][0] + a[1]*b[1][1] + a[2]*b[1][2] + a[3]*b[1][3] */
        _mm256_store_ps(c + 0, _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(a0x2, b00x4_10x4),
                _mm256_mul_ps(a1x2, b01x4_11x4)),
            _mm256_add_ps(
                _mm256_mul_ps(a2x2, b02x4_12x4),
                _mm256_mul_ps(a3x2, b03x4_13x4))));

        /* c[2] = a[0]*b[2][0] + a[1]*b[2][1] + a[2]*b[2][2] + a[3]*b[2][3]
           c[3] = a[0]*b[3][0] + a[1]*b[3][1] + a[2]*b[3][2] + a[3]*b[3][3] */
        _mm256_store_ps(c + 8, _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(a0x2, b20x4_30x4),
                _mm256_mul_ps(a1x2, b21x4_31x4)),
            _mm256_add_ps(
                _mm256_mul_ps(a2x2, b22x4_32x4),
                _mm256_mul_ps(a3x2, b23x4_33x4))));
    }
}

/* Variant of the AVX codepath without caching the left-side matrix for
   subsequent calculations */
CORRADE_ENABLE_AVX void absoluteTransformations3DAvxNoCache(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        const __m256 a01 = _mm256_load_ps(a + 0);
        const __m256 a23 = _mm256_load_ps(a + 8);
        const __m256 a0x2 = _mm256_permute2f128_ps(a01, a01, 0x00);
        const __m256 a1x2 = _mm256_permute2f128_ps(a01, a01, 0x11);
        const __m256 a2x2 = _mm256_permute2f128_ps(a23, a23, 0x00);
        const __m256 aZero3 = _mm256_permute2f128_ps(a23, a23, 0x18);

        const __m256 b01 = _mm256_load_ps(b + 0);
        const __m256 b23 = _mm256_load_ps(b + 8);
        const __m256 b00x4_10x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b01x4_11x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b02x4_12x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(2, 2, 2, 2));
        const __m256 b20x4_30x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b21x4_31x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b22x4_32x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(2, 2, 2, 2));

        _mm256_store_ps(c + 0, _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(a0x2, b00x4_10x4),
                _mm256_mul_ps(a1x2, b01x4_11x4)),
            _mm256_mul_ps(a2x2, b02x4_12x4)));

        _mm256_store_ps(c + 8, _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(a0x2, b20x4_30x4),
                _mm256_mul_ps(a1x2, b21x4_31x4)),
            _mm256_add_ps(
                _mm256_mul_ps(a2x2, b22x4_32x4),
                aZero3)));
    }
}
#endif

#ifdef CORRADE_ENABLE_AVX512F
/* Variant of the builtin AVX512 affine version with hardcoded strides to
   verify that the stride calculation isn't affecting performance too much.
   Done just for the highest-bit-width variant because there any extra
   instructions have the biggest impact. */
CORRADE_ENABLE_AVX512F void absoluteTransformations3DAvx512fHardcodedStride(std::size_t size, const char* transformations, std::ptrdiff_t, const char* parents, std::ptrdiff_t, char* output, std::ptrdiff_t) {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m512 a0x4{};
    __m512 a1x4{};
    __m512 a2x4{};
    __m512 aZero3{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*sizeof(Int));
        const void* const b = transformations + i*sizeof(Matrix4);
        void* const c = output + (i + 1)*sizeof(Matrix4);

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const void* const a = output + (parent + 1)*sizeof(Matrix4);

            const __m512 a0123 = _mm512_load_ps(a);
            a0x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0), a0123);
            a1x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                7, 6, 5, 4, 7, 6, 5, 4, 7, 6, 5, 4, 7, 6, 5, 4), a0123);
            a2x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                11, 10, 9, 8, 11, 10, 9, 8, 11, 10, 9, 8, 11, 10, 9, 8), a0123);
            /* Three columns of zeros, followed by `a[3]`. Assuming a[0][3] is
               zero, use it to zero-fill the rest. Similarly to above, the
               128-bit vectors are *also* ordered in reverse so the first
               four-argument group is actually the last column of A. */
            aZero3 = _mm512_permutexvar_ps(_mm512_set_epi32(
                15, 14, 13, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3), a0123);
        }

        /* Elements `b[0][i]` four times in a register followed by `b[1][i]`
           four times, `b[2][i]` four times and `b[3][i] four times.
           The `b[j][3]` elements are omitted as the bottom row is assumed to
           be (0, 0, 0, 1). */
        const __m512 b0123 = _mm512_load_ps(b);
        const __m512 b00x4_10x4_20x4_30x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(0, 0, 0, 0));
        const __m512 b01x4_11x4_21x4_31x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(1, 1, 1, 1));
        const __m512 b02x4_12x4_22x4_32x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(2, 2, 2, 2));

        /* As `b[0][3]` to `b[2][3]` are all 0, the `a[3]*b[j][3]` part of the
           expression cancels out. Then, `b[3][3]` is 1, so the `a[3]*b[3][3]`
           part of the expression reduces to `a[3]`:
            c[0] = a[0]*b[0][0] + a[1]*b[0][1] + a[2]*b[0][2]
            c[1] = a[0]*b[1][0] + a[1]*b[1][1] + a[2]*b[1][2]
            c[2] = a[0]*b[2][0] + a[1]*b[2][1] + a[2]*b[2][2]
            c[3] = a[0]*b[3][0] + a[1]*b[3][1] + a[2]*b[3][2] + a[3] */
        _mm512_store_ps(c,
            _mm512_fmadd_ps(a0x4, b00x4_10x4_20x4_30x4,
                _mm512_fmadd_ps(a1x4, b01x4_11x4_21x4_31x4,
                    _mm512_fmadd_ps(a2x4, b02x4_12x4_22x4_32x4, aZero3))));
    }
}

/* Variant of the builtin AVX512 affine version that's doing a full 4x4
   multiply for easier iteration and debugging */
CORRADE_ENABLE_AVX512F void absoluteTransformations3DAvx512fNoAffine(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m512 a0x4{};
    __m512 a1x4{};
    __m512 a2x4{};
    __m512 a3x4{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const void* const b = transformations + i*transformationsStride;
        void* const c = output + (i + 1)*outputStride;

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const void* const a = output + (parent + 1)*outputStride;

            const __m512 a0123 = _mm512_load_ps(a);
            a0x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0), a0123);
            a1x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                7, 6, 5, 4, 7, 6, 5, 4, 7, 6, 5, 4, 7, 6, 5, 4), a0123);
            a2x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                11, 10, 9, 8, 11, 10, 9, 8, 11, 10, 9, 8, 11, 10, 9, 8), a0123);
            a3x4 = _mm512_permutexvar_ps(_mm512_set_epi32(
                15, 14, 13, 12, 15, 14, 13, 12, 15, 14, 13, 12, 15, 14, 13, 12), a0123);
        }

        const __m512 b0123 = _mm512_load_ps(b);
        const __m512 b00x4_10x4_20x4_30x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(0, 0, 0, 0));
        const __m512 b01x4_11x4_21x4_31x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(1, 1, 1, 1));
        const __m512 b02x4_12x4_22x4_32x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(2, 2, 2, 2));
        const __m512 b03x4_13x4_23x4_33x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(3, 3, 3, 3));

        _mm512_store_ps(c,
            _mm512_fmadd_ps(a0x4, b00x4_10x4_20x4_30x4,
                _mm512_fmadd_ps(a1x4, b01x4_11x4_21x4_31x4,
                    _mm512_fmadd_ps(a2x4, b02x4_12x4_22x4_32x4,
                        _mm512_mul_ps(a3x4, b03x4_13x4_23x4_33x4)))));
    }
}

/* Like above, with extract + broadcast instead of _mm512_permutexvar_ps().
   Doesn't need to set the extra registers for the permutation mask but isn't
   actually faster. */
CORRADE_ENABLE_AVX512F void absoluteTransformations3DAvx512fNoAffineBroadcast(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride) {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m512 a0x4{};
    __m512 a1x4{};
    __m512 a2x4{};
    __m512 a3x4{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const void* const b = transformations + i*transformationsStride;
        void* const c = output + (i + 1)*outputStride;

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const void* const a = output + (parent + 1)*outputStride;

            const __m512 a0123 = _mm512_load_ps(a);
            a0x4 = _mm512_broadcast_f32x4(_mm512_extractf32x4_ps(a0123, 0));
            a1x4 = _mm512_broadcast_f32x4(_mm512_extractf32x4_ps(a0123, 1));
            a2x4 = _mm512_broadcast_f32x4(_mm512_extractf32x4_ps(a0123, 2));
            a3x4 = _mm512_broadcast_f32x4(_mm512_extractf32x4_ps(a0123, 3));
        }

        const __m512 b0123 = _mm512_load_ps(b);
        const __m512 b00x4_10x4_20x4_30x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(0, 0, 0, 0));
        const __m512 b01x4_11x4_21x4_31x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(1, 1, 1, 1));
        const __m512 b02x4_12x4_22x4_32x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(2, 2, 2, 2));
        const __m512 b03x4_13x4_23x4_33x4 = _mm512_permute_ps(b0123, _MM_SHUFFLE(3, 3, 3, 3));

        _mm512_store_ps(c,
            _mm512_fmadd_ps(a0x4, b00x4_10x4_20x4_30x4,
                _mm512_fmadd_ps(a1x4, b01x4_11x4_21x4_31x4,
                    _mm512_fmadd_ps(a2x4, b02x4_12x4_22x4_32x4,
                        _mm512_mul_ps(a3x4, b03x4_13x4_23x4_33x4)))));
    }
}
#endif

}}}}
#endif

#endif
