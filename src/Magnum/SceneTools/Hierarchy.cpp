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

#include "Hierarchy.h"

#include <Corrade/Cpu.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>
#ifdef CORRADE_ENABLE_AVX
#include <Corrade/Utility/IntrinsicsAvx.h>
#elif defined(CORRADE_ENABLE_SSE2)
#include <Corrade/Utility/IntrinsicsSse2.h>
#endif
#ifdef CORRADE_ENABLE_SIMD128
#include <wasm_simd128.h>
#endif

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/Implementation/cpu.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

Containers::Array<Containers::Pair<UnsignedInt, Int>> parentsBreadthFirst(const Trade::SceneData& scene) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::parentsBreadthFirst(): the scene has no hierarchy", {});
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out{NoInit, scene.fieldSize(*parentFieldId)};
    parentsBreadthFirstInto(scene,
        stridedArrayView(out).slice(&decltype(out)::Type::first),
        stridedArrayView(out).slice(&decltype(out)::Type::second));
    return out;
}

void parentsBreadthFirstInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& parentDestination) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::parentsBreadthFirstInto(): the scene has no hierarchy", );
    const std::size_t parentFieldSize = scene.fieldSize(*parentFieldId);
    CORRADE_ASSERT(mappingDestination.size() == parentFieldSize,
        "SceneTools::parentsBreadthFirstInto(): expected mapping destination view with" << parentFieldSize << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(parentDestination.size() == parentFieldSize,
        "SceneTools::parentsBreadthFirstInto(): expected parent destination view with" << parentFieldSize << "elements but got" << parentDestination.size(), );

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> parents;
    Containers::ArrayView<UnsignedInt> childrenOffsets;
    Containers::ArrayView<UnsignedInt> children;
    Containers::ArrayView<Int> parentsToProcess;
    Containers::ArrayTuple storage{
        /* Output of scene.parentsInto() */
        {NoInit, parentFieldSize, parents},
        /* Running children offset (+1) for each node including root (+1), plus
           one more element when we shift the array by one below */
        {ValueInit, std::size_t(scene.mappingBound() + 3), childrenOffsets},
        {NoInit, parentFieldSize, children},
        /* Queue of parents to process. Can't reuse mappingDestination because
           this includes one more element for root objects. */
        {NoInit, parentFieldSize + 1, parentsToProcess}
    };

    /* Convert the parent list to a child list to sort them toplogically */
    scene.parentsInto(
        stridedArrayView(parents).slice(&decltype(parents)::Type::first),
        stridedArrayView(parents).slice(&decltype(parents)::Type::second)
    );

    /* Children offset for each node including root. First calculate the count
       of children for each, skipping the first element (parent.second() can be
       -1, accounting for that as well)... */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents) {
        CORRADE_INTERNAL_ASSERT(parent.first() < scene.mappingBound() && (parent.second() == -1 || UnsignedInt(parent.second()) < scene.mappingBound()));
        ++childrenOffsets[parent.second() + 2];
    }

    /* ... then convert the counts to a running offset. Now
       `[childrenOffsets[i + 2], childrenOffsets[i + 3])` contains a range in
       which the `children` array below contains a list of children for `i`. */
    UnsignedInt offset = 0;
    for(UnsignedInt& i: childrenOffsets) {
        UnsignedInt nextOffset = offset + i;
        i = offset;
        offset = nextOffset;
    }
    CORRADE_INTERNAL_ASSERT(offset == parents.size());

    /* Go through the parent list again, convert that to child ranges. The
       childrenOffsets array gets shifted by one element by the process, thus
       now `[childrenOffsets[i + 1], childrenOffsets[i + 2])` contains a range
       in which the `children` array below contains a list of children for
       `i`. */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents)
        children[childrenOffsets[parent.second() + 2]++] = parent.first();

    /* Go breadth-first (so we have nodes sharing the same parent next to each
       other) and build a list of (id, parent id) where a parent is always
       before its children */
    std::size_t outputOffset = 0;
    parentsToProcess[0] = -1;
    for(std::size_t i = 0; i != outputOffset + 1; ++i) {
        const Int objectId = parentsToProcess[i];
        for(std::size_t j = childrenOffsets[objectId + 1], jMax = childrenOffsets[objectId + 2]; j != jMax; ++j) {
            /** @todo better diagnostic with BitArray to detect which nodes are
                parented more than once (OTOH maybe that's unnecessary extra
                work which isn't desired to be done here but should be instead
                in a dedicated cycle/sparse checker utility?) */
            CORRADE_ASSERT(outputOffset < parents.size(),
                "SceneTools::parentsBreadthFirst(): hierarchy is cyclic", );
            parentsToProcess[outputOffset + 1] = children[j];
            mappingDestination[outputOffset] = children[j];
            parentDestination[outputOffset] = objectId;
            ++outputOffset;
        }
    }

    /** @todo better diagnostic with BitArray to detect which nodes are
        unreachable from root (OTOH again maybe that's undesirable extra work
        that doesn't belong here?) */
    CORRADE_ASSERT(outputOffset == parents.size(),
        "SceneTools::parentsBreadthFirst(): hierarchy is sparse", );
}

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> childrenDepthFirst(const Trade::SceneData& scene) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::childrenDepthFirst(): the scene has no hierarchy", {});
    Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> out{NoInit, scene.fieldSize(*parentFieldId)};
    childrenDepthFirstInto(scene,
        stridedArrayView(out).slice(&decltype(out)::Type::first),
        stridedArrayView(out).slice(&decltype(out)::Type::second));
    return out;
}

void childrenDepthFirstInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& childCountDestination) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::childrenDepthFirstInto(): the scene has no hierarchy", );
    const std::size_t parentFieldSize = scene.fieldSize(*parentFieldId);
    CORRADE_ASSERT(mappingDestination.size() == parentFieldSize,
        "SceneTools::childrenDepthFirstInto(): expected mapping destination view with" << parentFieldSize << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(childCountDestination.size() == parentFieldSize,
        "SceneTools::childrenDepthFirstInto(): expected child count destination view with" << parentFieldSize << "elements but got" << childCountDestination.size(), );

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> parents;
    Containers::ArrayView<UnsignedInt> childrenOffsets;
    Containers::ArrayView<UnsignedInt> children;
    /* Parent ID, offset of the first child in `childCountDestination`, offset
       of next child in `children` to process */
    Containers::ArrayView<Containers::Triple<Int, UnsignedInt, UnsignedInt>> parentsToProcess;
    Containers::ArrayTuple storage{
        /* Output of scene.parentsInto() */
        {NoInit, parentFieldSize, parents},
        /* Running children offset (+1) for each node including root (+1), plus
           one more element when we shift the array by one below */
        {ValueInit, std::size_t(scene.mappingBound() + 3), childrenOffsets},
        {NoInit, parentFieldSize, children},
        /* A stack of parents to process. It only reaches `parentFieldSize + 1`
           if the hierarchy is a single branch, usually it's shorter. */
        {NoInit, parentFieldSize + 1, parentsToProcess}
    };

    /* Convert the parent list to a child list to sort them toplogically */
    scene.parentsInto(
        stridedArrayView(parents).slice(&decltype(parents)::Type::first),
        stridedArrayView(parents).slice(&decltype(parents)::Type::second)
    );

    /* Children offset for each node including root. First calculate the count
       of children for each, skipping the first element (parent.second() can be
       -1, accounting for that as well)... */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents) {
        CORRADE_INTERNAL_ASSERT(parent.first() < scene.mappingBound() && (parent.second() == -1 || UnsignedInt(parent.second()) < scene.mappingBound()));
        ++childrenOffsets[parent.second() + 2];
    }

    /* ... then convert the counts to a running offset. Now
       `[childrenOffsets[i + 2], childrenOffsets[i + 3])` contains a range in
       which the `children` array below contains a list of children for `i`. */
    UnsignedInt offset = 0;
    for(UnsignedInt& i: childrenOffsets) {
        UnsignedInt nextOffset = offset + i;
        i = offset;
        offset = nextOffset;
    }
    CORRADE_INTERNAL_ASSERT(offset == parents.size());

    /* Go through the parent list again, convert that to child ranges. The
       childrenOffsets array gets shifted by one element by the process, thus
       now `[childrenOffsets[i + 1], childrenOffsets[i + 2])` contains a range
       in which the `children` array below contains a list of children for
       `i`. */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents)
        children[childrenOffsets[parent.second() + 2]++] = parent.first();

    UnsignedInt outputOffset = 0;
    std::size_t parentsToProcessOffset = 0;
    parentsToProcess[parentsToProcessOffset++] = {-1, outputOffset, childrenOffsets[-1 + 1]};
    while(parentsToProcessOffset) {
        const Int objectId = parentsToProcess[parentsToProcessOffset - 1].first();
        UnsignedInt& childrenOffset = parentsToProcess[parentsToProcessOffset - 1].third();

        /* If all children were processed, we're done with this object */
        if(childrenOffset == childrenOffsets[objectId + 2]) {
            /* Save the total size. Only if it's not the root objects, for them
               the total size is implicitly the whole output size. */
            if(objectId != -1) {
                const UnsignedInt firstChildOutputOffset = parentsToProcess[parentsToProcessOffset - 1].second();
                childCountDestination[firstChildOutputOffset - 1] = outputOffset - firstChildOutputOffset;
            }

            /* Remove from the processing stack and continue with next */
            --parentsToProcessOffset;
            continue;
        }

        CORRADE_INTERNAL_DEBUG_ASSERT(childrenOffset < childrenOffsets[objectId + 2]);
        CORRADE_INTERNAL_DEBUG_ASSERT(parentsToProcessOffset < parentFieldSize + 1);
        /** @todo better diagnostic with BitArray to detect which nodes are
            parented more than once (OTOH maybe that's undesirable extra work
            that would be duplicated here and in parentsBreadthFirst()?) */
        CORRADE_ASSERT(outputOffset < parents.size(),
            "SceneTools::childrenDepthFirst(): hierarchy is cyclic", );

        /* Add the current child to the mapping output and to the list of
           parents to process next. Increment all offsets for the next
           round. */
        const UnsignedInt childObjectId = children[childrenOffset++];
        mappingDestination[outputOffset] = childObjectId;
        parentsToProcess[parentsToProcessOffset++] = {Int(childObjectId), ++outputOffset, childrenOffsets[childObjectId + 1]};
    }

    CORRADE_INTERNAL_ASSERT(parentsToProcessOffset == 0);

    /** @todo better diagnostic with BitArray to detect which nodes are
        unreachable from root (OTOH again maybe that's undesirable extra work
        that would be duplicated here and in parentsBreadthFirst()?) */
    CORRADE_ASSERT(outputOffset == parents.size(),
        "SceneTools::childrenDepthFirst(): hierarchy is sparse", );
}

namespace Implementation {

namespace {

MAGNUM_SCENETOOLS_CPU_MAYBE_UNUSED typename std::decay<decltype(absoluteTransformations3D)>::type absoluteTransformations3DImplementation(CORRADE_CPU_DECLARE(Cpu::Scalar)) {
  return [](const std::size_t size, const char* const transformations, const std::ptrdiff_t transformationsStride, const char* const parents, const std::ptrdiff_t parentsStride, char* const output, const std::ptrdiff_t outputStride) {
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);

        /* `c = a*b`; `a` is stored in the output array which contains the
           global transform (for `parent == -1`) at index 0, which is why
           `parent + 1` is used, similarly `c` is stored at `i + 1` */
        const Matrix4& a = *reinterpret_cast<const Matrix4*>(output + (parent + 1)*outputStride);
        const Matrix4& b = *reinterpret_cast<const Matrix4*>(transformations + i*transformationsStride);
        Matrix4& c = *reinterpret_cast<Matrix4*>(output + (i + 1)*outputStride);

        c = a*b;
    }
  };
}

#ifdef CORRADE_ENABLE_SSE2
/* Originally from https://stackoverflow.com/a/18508113, with a single load and
   four (integer) shuffles instead of four loads, subsequently changed to
   calculate only an affine transformation, i.e., ignoring all `b[j][3]`
   elements and assuming they're (0, 0, 0, 1), and with caching of the
   left-side matrix. See Test/HierarchyTest.h for the other (slower) variants
   that led to this. */
MAGNUM_SCENETOOLS_CPU_MAYBE_UNUSED CORRADE_ENABLE_SSE2 typename std::decay<decltype(absoluteTransformations3D)>::type absoluteTransformations3DImplementation(CORRADE_CPU_DECLARE(Cpu::Sse2)) {
  return [](const std::size_t size, const char* const transformations, const std::ptrdiff_t transformationsStride, const char* const parents, const std::ptrdiff_t parentsStride, char* const output, const std::ptrdiff_t outputStride) CORRADE_ENABLE_SSE2 {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m128 a0{};
    __m128 a1{};
    __m128 a2{};
    __m128 a3{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        /* Same as in the scalar implementation, the only difference is that
           we're casting only to the primitive float type */
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        /* Optimistically assume the parent index stays the same across
           multiple transformations, which allows us to cache the loaded
           left-side matrix. There isn't as many SSE2 registers as is with AVX+
           so on older HW or on 32-bit targets this could be worse than no
           caching at all due to having to offload the loaded registers back to
           memory. But in my tests on a 64-bit machine it's a clear win. */
        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);

            /* Columns `a[0]` to `a[3]` */
            a0 = _mm_load_ps(a + 0);
            a1 = _mm_load_ps(a + 4);
            a2 = _mm_load_ps(a + 8);
            a3 = _mm_load_ps(a + 12);
        }

        /* Rows `b[j][0]` to `b[j][2]`, `b[j][3]` omitted */
        for(std::size_t j = 0; j != 3; ++j) {
            /* Elements `b[j][0]` to `b[j][2]`, four times in each register */
            const __m128 bj = _mm_load_ps(b + j*4);
            /* Using SSE2 _mm_shuffle_epi32() (and two extra casts to satisfy
               the type checker, but with no impact on the generated code) is
               faster than SSE _mm_shuffle_ps() that takes two input
               registers */
            const __m128 bj0x4 = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(bj), _MM_SHUFFLE(0, 0, 0, 0)));
            const __m128 bj1x4 = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(bj), _MM_SHUFFLE(1, 1, 1, 1)));
            const __m128 bj2x4 = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(bj), _MM_SHUFFLE(2, 2, 2, 2)));

            /* Calculate the whole output row at once. As `b[j][3]` is 0 the
               part of the expression that's `a[3]*b[j][3]` cancels out:
                c[j] = a[0]*b[j][0] + a[1]*b[j][1] +
                       a[2]*b[j][2]
               In other words, for k being 0 to 3:
                c[j][k] = a[0][k]*b[j][0] + a[1][k]*b[j][1] +
                          a[2][k]*b[j][2] */
            _mm_store_ps(c + j*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, bj0x4),
                    _mm_mul_ps(a1, bj1x4)),
                _mm_mul_ps(a2, bj2x4)));
        }

        /* Row `b[3][0]` to `b[3][2]`, `b[3][3]` omitted */
        {
            /* Elements `b[3][0]` to `b[3][2]`, four times in each register */
            const __m128 b3 = _mm_load_ps(b + 3*4);
            /* Like above, using SSE2 _mm_shuffle_epi32() is faster than SSE
               _mm_shuffle_ps() that takes two input registers */
            const __m128 b30x4 = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(b3), _MM_SHUFFLE(0, 0, 0, 0)));
            const __m128 b31x4 = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(b3), _MM_SHUFFLE(1, 1, 1, 1)));
            const __m128 b32x4 = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(b3), _MM_SHUFFLE(2, 2, 2, 2)));

            /* Calculate the whole output row at once. As `b[3][3]` is 1 the
               part of the expression that's `a[3]*b[3][3]` reduces to `a[3]`:
                c[j] = a[0]*b[3][0] + a[1]*b[3][1] +
                       a[2]*b[3][2] + a[3]
               In other words, for k being 0 to 3:
                c[j][k] = a[0][k]*b[3][0] + a[1][k]*b[3][1] +
                          a[2][k]*b[3][2] + a[3][k] */
            _mm_store_ps(c + 3*4, _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(a0, b30x4),
                    _mm_mul_ps(a1, b31x4)),
                _mm_add_ps(
                    _mm_mul_ps(a2, b32x4),
                    a3)));
        }
    }
  };
}
#endif

#ifdef CORRADE_ENABLE_AVX
/* Basically a direct translation of the affine SSE2 code above, processing two
   columns with two scalars at once. The _mm256_permute2f128_ps() is chosen
   because that's what GCC 10's autovectorizer uses for the scalar 4x4 version
   with -mavx. */
MAGNUM_SCENETOOLS_CPU_MAYBE_UNUSED CORRADE_ENABLE_AVX typename std::decay<decltype(absoluteTransformations3D)>::type absoluteTransformations3DImplementation(CORRADE_CPU_DECLARE(Cpu::Avx)) {
  return [](const std::size_t size, const char* const transformations, const std::ptrdiff_t transformationsStride, const char* const parents, const std::ptrdiff_t parentsStride, char* const output, const std::ptrdiff_t outputStride) CORRADE_ENABLE_AVX {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m256 a0x2{};
    __m256 a1x2{};
    __m256 a2x2{};
    __m256 aZero3{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        /* Optimistically assume the parent index stays the same across
           multiple transformations, which allows us to cache the loaded
           left-side matrix */
        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);

            /* Columns `a[0]` to `a[3]`, twice in each register */
            const __m256 a01 = _mm256_load_ps(a + 0);
            const __m256 a23 = _mm256_load_ps(a + 8);
            a0x2 = _mm256_permute2f128_ps(a01, a01, 0x00);
            a1x2 = _mm256_permute2f128_ps(a01, a01, 0x11);
            a2x2 = _mm256_permute2f128_ps(a23, a23, 0x00);
            /* Here the first part of the register is zeros, second is `a[3]`.
               Setting bit 3 causes the first part to be zeroed. Could also
               pass _mm256_setzero_ps() as one argument but this is more
               consistent. */
            aZero3 = _mm256_permute2f128_ps(a23, a23, 0x18);
        }

        /* Elements `b[0][i]` four times in a register followed by `b[1][i]`
           four times, then `b[2][i]` four times in a register followed by
           `b[3][i] four times. The `b[j][3]` elements are omitted as the
           bottom row is assumed to be (0, 0, 0, 1). */
        const __m256 b01 = _mm256_load_ps(b + 0);
        const __m256 b23 = _mm256_load_ps(b + 8);
        const __m256 b00x4_10x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b01x4_11x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b02x4_12x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(2, 2, 2, 2));
        const __m256 b20x4_30x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b21x4_31x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b22x4_32x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(2, 2, 2, 2));

        /* As `b[j][3]` are both 0, the `a[3]*b[j][3]` part of the expression
           cancels out:
            c[0] = a[0]*b[0][0] + a[1]*b[0][1] + a[2]*b[0][2]
            c[1] = a[0]*b[1][0] + a[1]*b[1][1] + a[2]*b[1][2] */
        _mm256_store_ps(c + 0, _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(a0x2, b00x4_10x4),
                _mm256_mul_ps(a1x2, b01x4_11x4)),
            _mm256_mul_ps(a2x2, b02x4_12x4)));

        /* As `b[2][3]` is 0 and `b[3][3]` is 1, the `a[3]*b[2][3]` part of the
           expression cancels out and `a[3]*b[3][3]` reduces to `a[3]`:
            c[2] = a[0]*b[2][0] + a[1]*b[2][1] + a[2]*b[2][2]
            c[3] = a[0]*b[3][0] + a[1]*b[3][1] + a[2]*b[3][2] + a[3] */
        _mm256_store_ps(c + 8, _mm256_add_ps(
            _mm256_add_ps(
                _mm256_mul_ps(a0x2, b20x4_30x4),
                _mm256_mul_ps(a1x2, b21x4_31x4)),
            _mm256_add_ps(
                _mm256_mul_ps(a2x2, b22x4_32x4),
                aZero3)));
    }
  };
}
#endif

#ifdef CORRADE_ENABLE_AVX_FMA
/* Like the AVX implementation, just folding the _mm256_mul_ps() and
   _mm256_add_ps() together. Matches GCC 10's autovectorizer for the 4x4
   version with -mavx -mfma. Comments omitted as there's nothing new to say. */
MAGNUM_SCENETOOLS_CPU_MAYBE_UNUSED CORRADE_ENABLE_AVX_FMA typename std::decay<decltype(absoluteTransformations3D)>::type absoluteTransformations3DImplementation(CORRADE_CPU_DECLARE(Cpu::Avx|Cpu::AvxFma)) {
  return [](const std::size_t size, const char* const transformations, const std::ptrdiff_t transformationsStride,  const char* const parents, const std::ptrdiff_t parentsStride, char* const output, const std::ptrdiff_t outputStride) CORRADE_ENABLE_AVX_FMA {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m256 a0x2{};
    __m256 a1x2{};
    __m256 a2x2{};
    __m256 aZero3{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const Float* const b = reinterpret_cast<const Float*>(transformations + i*transformationsStride);
        Float* const c = reinterpret_cast<Float*>(output + (i + 1)*outputStride);

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const Float* const a = reinterpret_cast<const Float*>(output + (parent + 1)*outputStride);

            const __m256 a01 = _mm256_load_ps(a + 0);
            const __m256 a23 = _mm256_load_ps(a + 8);
            a0x2 = _mm256_permute2f128_ps(a01, a01, 0x00);
            a1x2 = _mm256_permute2f128_ps(a01, a01, 0x11);
            a2x2 = _mm256_permute2f128_ps(a23, a23, 0x00);
            aZero3 = _mm256_permute2f128_ps(a23, a23, 0x18);
        }

        const __m256 b01 = _mm256_load_ps(b + 0);
        const __m256 b23 = _mm256_load_ps(b + 8);
        const __m256 b00x4_10x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b01x4_11x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b02x4_12x4 = _mm256_permute_ps(b01, _MM_SHUFFLE(2, 2, 2, 2));
        const __m256 b20x4_30x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(0, 0, 0, 0));
        const __m256 b21x4_31x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(1, 1, 1, 1));
        const __m256 b22x4_32x4 = _mm256_permute_ps(b23, _MM_SHUFFLE(2, 2, 2, 2));

        _mm256_store_ps(c + 0,
            _mm256_fmadd_ps(a0x2, b00x4_10x4,
                _mm256_fmadd_ps(a1x2, b01x4_11x4,
                    _mm256_mul_ps(a2x2, b02x4_12x4))));
        _mm256_store_ps(c + 8,
            _mm256_fmadd_ps(a0x2, b20x4_30x4,
                _mm256_fmadd_ps(a1x2, b21x4_31x4,
                    _mm256_fmadd_ps(a2x2, b22x4_32x4, aZero3))));
    }
  };
}
#endif

#ifdef CORRADE_ENABLE_AVX512F
/* Translation of the above AVX2 FMA code to a 512-bit width, processing four
   columns and four scalars at once instead of two. */
MAGNUM_SCENETOOLS_CPU_MAYBE_UNUSED CORRADE_ENABLE_AVX512F typename std::decay<decltype(absoluteTransformations3D)>::type absoluteTransformations3DImplementation(CORRADE_CPU_DECLARE(Cpu::Avx512f)) {
  return [](const std::size_t size, const char* const transformations, const std::ptrdiff_t transformationsStride, const char* const parents, const std::ptrdiff_t parentsStride, char* const output, const std::ptrdiff_t outputStride) CORRADE_ENABLE_AVX512F {
    /* GCC stupidly warns that these may be uninitialized, so zero-init */
    __m512 a0x4{};
    __m512 a1x4{};
    __m512 a2x4{};
    __m512 aZero3{};

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        /* _mm512_load_ps() take an untyped pointer, finally, so we don't need
           to mess with reinterpret_cast anymore */
        const void* const b = transformations + i*transformationsStride;
        void* const c = output + (i + 1)*outputStride;

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const void* const a = output + (parent + 1)*outputStride;

            /* Columns `a[0]` to `a[2]`, four times in each register. For the
               _mm512_permutexvar_ps() arguments, elements inside each 128-bit
               vector are ordered in reverse, so in order to put element 3 at
               index 3, it has to be first of the four-argument group, not
               last. */
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
  };
}
#endif

#ifdef CORRADE_ENABLE_SIMD128
/* Direct translation of the SSE2 variant. There aren't really many options how
   to do things in WASM SIMD, so this is basically the only possible result. */
MAGNUM_SCENETOOLS_CPU_MAYBE_UNUSED CORRADE_ENABLE_SIMD128 typename std::decay<decltype(absoluteTransformations3D)>::type absoluteTransformations3DImplementation(CORRADE_CPU_DECLARE(Cpu::Simd128)) {
  return [](const std::size_t size, const char* const transformations, const std::ptrdiff_t transformationsStride, const char* const parents, const std::ptrdiff_t parentsStride, char* const output, const std::ptrdiff_t outputStride) CORRADE_ENABLE_SIMD128 {
    v128_t a0;
    v128_t a1;
    v128_t a2;
    v128_t a3;

    Int prevParent = INT32_MIN;
    for(std::size_t i = 0; i != size; ++i) {
        const Int parent = *reinterpret_cast<const Int*>(parents + i*parentsStride);
        const v128_t* const b = reinterpret_cast<const v128_t*>(transformations + i*transformationsStride);
        v128_t* const c = reinterpret_cast<v128_t*>(output + (i + 1)*outputStride);

        if CORRADE_UNLIKELY(prevParent != parent) {
            prevParent = parent;
            const v128_t* const a = reinterpret_cast<const v128_t*>(output + (parent + 1)*outputStride);

            a0 = wasm_v128_load(a + 0);
            a1 = wasm_v128_load(a + 1);
            a2 = wasm_v128_load(a + 2);
            a3 = wasm_v128_load(a + 3);
        }

        for(std::size_t j = 0; j != 3; ++j) {
            const v128_t bj = wasm_v128_load(b + j);
            const v128_t bj0x4 = wasm_i32x4_shuffle(bj, bj, 0, 0, 0, 0);
            const v128_t bj1x4 = wasm_i32x4_shuffle(bj, bj, 1, 1, 1, 1);
            const v128_t bj2x4 = wasm_i32x4_shuffle(bj, bj, 2, 2, 2, 2);

            wasm_v128_store(c + j, wasm_f32x4_add(
                wasm_f32x4_add(
                    wasm_f32x4_mul(a0, bj0x4),
                    wasm_f32x4_mul(a1, bj1x4)),
                wasm_f32x4_mul(a2, bj2x4)));
        }

        {
            const v128_t b3 = wasm_v128_load(b + 3);
            const v128_t b30x4 = wasm_i32x4_shuffle(b3, b3, 0, 0, 0, 0);
            const v128_t b31x4 = wasm_i32x4_shuffle(b3, b3, 1, 1, 1, 1);
            const v128_t b32x4 = wasm_i32x4_shuffle(b3, b3, 2, 2, 2, 2);

            wasm_v128_store(c + 3, wasm_f32x4_add(
                wasm_f32x4_add(
                    wasm_f32x4_mul(a0, b30x4),
                    wasm_f32x4_mul(a1, b31x4)),
                wasm_f32x4_add(
                    wasm_f32x4_mul(a2, b32x4),
                    a3)));
        }
    }
  };
}
#endif

}

#ifdef CORRADE_TARGET_X86
MAGNUM_SCENETOOLS_CPU_DISPATCHER(absoluteTransformations3DImplementation, Cpu::AvxFma)
#else
MAGNUM_SCENETOOLS_CPU_DISPATCHER(absoluteTransformations3DImplementation)
#endif
MAGNUM_SCENETOOLS_CPU_DISPATCHED(absoluteTransformations3DImplementation, void MAGNUM_SCENETOOLS_CPU_DISPATCHED_DECLARATION(absoluteTransformations3D)(std::size_t size, const char* transformations, std::ptrdiff_t transformationsStride, const char* parents, std::ptrdiff_t parentsStride, char* output, std::ptrdiff_t outputStride))({
    return absoluteTransformations3DImplementation(CORRADE_CPU_SELECT(Cpu::Default))(size, transformations, transformationsStride, parents, parentsStride, output, outputStride);
})

}

void absoluteTransformations3DInto(const Containers::StridedArrayView1D<const Matrix4>& transformations, const Containers::StridedArrayView1D<const Int>& parents, const Containers::StridedArrayView1D<Matrix4>& output, const Matrix4& globalTransformation) {
    const std::size_t size = transformations.size();
    const char* const transformationsData = static_cast<const char*>(transformations.data());
    const std::ptrdiff_t transformationsStride = transformations.stride();
    const char* const parentsData = static_cast<const char*>(parents.data());
    const std::ptrdiff_t parentsStride = parents.stride();
    char* const outputData = static_cast<char*>(output.data());
    const std::ptrdiff_t outputStride = output.stride();
    CORRADE_ASSERT(parents.size() == size && output.size() == size + 1,
        "SceneTools::absoluteTransformations3DInto(): expected parents with" << size << "elements and output with" << size + 1 << "elements but got" << parents.size() << "and" << output.size(), );
    /* Stride is checked before alignment as with the opposite order the stride
       assert couldn't be easily tested for, as struct sizes are always a
       multiple of member alignment. This also seems to handle negative strides
       correctly without having to do anything extra. */
    CORRADE_ASSERT(transformationsStride % 64 == 0,
        "SceneTools::absoluteTransformations3DInto(): transformations stride" << transformationsStride << "is not a multiple of 64 bytes", );
    CORRADE_ASSERT(reinterpret_cast<std::uintptr_t>(transformationsData) % 64 == 0,
        "SceneTools::absoluteTransformations3DInto(): transformations are not 64-byte aligned", );
    CORRADE_ASSERT(outputStride % 64 == 0,
        "SceneTools::absoluteTransformations3DInto(): output stride" << outputStride << "is not a multiple of 64 bytes", );
    CORRADE_ASSERT(reinterpret_cast<std::uintptr_t>(outputData) % 64 == 0,
        "SceneTools::absoluteTransformations3DInto(): output is not 64-byte aligned", );
    #ifndef CORRADE_NO_DEBUG_ASSERT
    /* This is important for verifying that the input data make sense, but is
       too expensive to run in hot code, so it's just a debug assert */
    for(std::size_t i = 0; i != size; ++i)
        CORRADE_DEBUG_ASSERT(parents[i] >= -1 && parents[i] < Int(i),
            "SceneTools::absoluteTransformations3DInto(): expected parents to be -1 or back-referencing but got parent" << parents[i] << "at index" << i, );
    #endif

    /* Put the global transformation as the first item in the output, to be
       picked by parents that are -1. There's nothing to optimize about this
       assignment so it's done outside of the dispatched arch-specific code. */
    output[0] = globalTransformation;
    Implementation::absoluteTransformations3D(size, transformationsData, transformationsStride, parentsData, parentsStride, outputData, outputStride);
}

void absoluteTransformations3DInto(const Containers::StridedArrayView1D<const Matrix4>& transformations, const Containers::StridedArrayView1D<const Int>& parents, const Containers::StridedArrayView1D<Matrix4>& output) {
    absoluteTransformations3DInto(transformations, parents, output, {});
}

namespace {

template<UnsignedInt> struct SceneDataDimensionTraits;
template<> struct SceneDataDimensionTraits<2> {
    static bool isDimensions(const Trade::SceneData& scene) {
        return scene.is2D();
    }
    static void transformationsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix3>& transformationDestination) {
        return scene.transformations2DInto(mappingDestination, transformationDestination);
    }
};
template<> struct SceneDataDimensionTraits<3> {
    static bool isDimensions(const Trade::SceneData& scene) {
        return scene.is3D();
    }
    static void transformationsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix4>& transformationDestination) {
        return scene.transformations3DInto(mappingDestination, transformationDestination);
    }
};

template<UnsignedInt dimensions> void absoluteFieldTransformationsIntoImplementation(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>>& outputTransformations, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    CORRADE_ASSERT(SceneDataDimensionTraits<dimensions>::isDimensions(scene),
        "SceneTools::absoluteFieldTransformations(): the scene is not" << dimensions << Debug::nospace << "D", );
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::absoluteFieldTransformations(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields", );
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::absoluteFieldTransformations(): the scene has no hierarchy", );
    CORRADE_ASSERT(outputTransformations.size() == scene.fieldSize(fieldId),
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected" << scene.fieldSize(fieldId) << "but got" << outputTransformations.size(), );

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> orderedClusteredParents;
    Containers::ArrayView<Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>> transformations;
    Containers::ArrayView<MatrixTypeFor<dimensions, Float>> absoluteTransformations;
    Containers::ArrayTuple storage{
        /* Output of parentsBreadthFirstInto() */
        {NoInit, scene.fieldSize(*parentFieldId), orderedClusteredParents},
        /* Output of scene.transformationsXDInto() */
        {NoInit, scene.transformationFieldSize(), transformations},
        /* Above transformations but indexed by object ID */
        {ValueInit, std::size_t(scene.mappingBound() + 1), absoluteTransformations}
    };
    parentsBreadthFirstInto(scene,
        stridedArrayView(orderedClusteredParents).slice(&decltype(orderedClusteredParents)::Type::first),
        stridedArrayView(orderedClusteredParents).slice(&decltype(orderedClusteredParents)::Type::second));
    SceneDataDimensionTraits<dimensions>::transformationsInto(scene,
        stridedArrayView(transformations).slice(&decltype(transformations)::Type::first),
        stridedArrayView(transformations).slice(&decltype(transformations)::Type::second));

    /* Retrieve transformations of all objects, indexed by object ID. Since not
       all nodes in the hierarchy may have a transformation assigned, the whole
       array got initialized to identity first. */
    /** @todo switch to a hashmap eventually? */
    absoluteTransformations[0] = globalTransformation;
    for(const Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>& transformation: transformations) {
        CORRADE_INTERNAL_ASSERT(transformation.first() < scene.mappingBound());
        absoluteTransformations[transformation.first() + 1] = transformation.second();
    }

    /* Turn the transformations into absolute */
    for(const Containers::Pair<UnsignedInt, Int>& parentOffset: orderedClusteredParents) {
        absoluteTransformations[parentOffset.first() + 1] =
            absoluteTransformations[parentOffset.second() + 1]*
            absoluteTransformations[parentOffset.first() + 1];
    }

    /* Allocate the output array, retrieve mesh & material IDs and assign
       absolute transformations to each. The matrix location is abused for
       object mapping, which is subsequently replaced by the absolute object
       transformation for given mesh. */
    const auto mapping = Containers::arrayCast<UnsignedInt>(outputTransformations);
    scene.mappingInto(fieldId, mapping);
    for(std::size_t i = 0; i != mapping.size(); ++i) {
        CORRADE_INTERNAL_ASSERT(mapping[i] < scene.mappingBound());
        outputTransformations[i] = absoluteTransformations[mapping[i] + 1];
    }
}

template<UnsignedInt dimensions> void absoluteFieldTransformationsIntoImplementation(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>>& outputTransformations, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::absoluteFieldTransformationsInto(): field" << field << "not found", );

    absoluteFieldTransformationsIntoImplementation<dimensions>(scene, *fieldId, outputTransformations, globalTransformation);
}

template<UnsignedInt dimensions> Containers::Array<MatrixTypeFor<dimensions, Float>> absoluteFieldTransformationsImplementation(const Trade::SceneData& scene, const UnsignedInt fieldId, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::absoluteFieldTransformations(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields", {});

    Containers::Array<MatrixTypeFor<dimensions, Float>> out{NoInit, scene.fieldSize(fieldId)};
    absoluteFieldTransformationsIntoImplementation<dimensions>(scene, fieldId, out, globalTransformation);
    return out;
}

template<UnsignedInt dimensions> Containers::Array<MatrixTypeFor<dimensions, Float>> absoluteFieldTransformationsImplementation(const Trade::SceneData& scene, const Trade::SceneField field, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::absoluteFieldTransformations(): field" << field << "not found", {});

    Containers::Array<MatrixTypeFor<dimensions, Float>> out{NoInit, scene.fieldSize(*fieldId)};
    absoluteFieldTransformationsIntoImplementation<dimensions>(scene, *fieldId, out, globalTransformation);
    return out;
}

}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const Trade::SceneField field, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsImplementation<2>(scene, field, globalTransformation);
}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const Trade::SceneField field) {
    return absoluteFieldTransformationsImplementation<2>(scene, field, {});
}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const UnsignedInt fieldId, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsImplementation<2>(scene, fieldId, globalTransformation);
}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const UnsignedInt fieldId) {
    return absoluteFieldTransformationsImplementation<2>(scene, fieldId, {});
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, field, transformations, globalTransformation);
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, field, transformations, {});
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, fieldId, transformations, globalTransformation);
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, fieldId, transformations, {});
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const Trade::SceneField field, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsImplementation<3>(scene, field, globalTransformation);
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const Trade::SceneField field) {
    return absoluteFieldTransformationsImplementation<3>(scene, field, {});
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const UnsignedInt fieldId, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsImplementation<3>(scene, fieldId, globalTransformation);
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const UnsignedInt fieldId) {
    return absoluteFieldTransformationsImplementation<3>(scene, fieldId, {});
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, field, transformations, globalTransformation);
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, field, transformations, {});
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, fieldId, transformations, globalTransformation);
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, fieldId, transformations, {});
}

}}
