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

/* On GCC 4.8, the <random> header apparently messes up with AVX includes
   somehow, causing <Corrade/Utility/IntrinsicsAvx.h> to not work if it's
   included after, resulting in errors about _m256i not defined. */
#include "Magnum/SceneTools/Test/HierarchyTest.h"

#include <random>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Memory.h>

#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/Hierarchy.h"
#include "Magnum/SceneTools/Test/cpuVariantHelpers.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct HierarchyBenchmark: TestSuite::Tester {
    explicit HierarchyBenchmark();

    void captureImplementations();
    void restoreImplementations();

    void absoluteTransformations3D();
    void absoluteTransformations3DClustered();
    void absoluteTransformations3DClusteredInPlace();
    void absoluteTransformations3DClusteredInterleaved();
    void absoluteTransformations3DChained();

    Containers::Array<Matrix4> _transformations;
    Containers::Array<Int> _parents, _parentsClustered, _parentsChained;

    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    decltype(Implementation::absoluteTransformations3D) _absoluteTransformations3DImplementation;
    #endif
};

using namespace Math::Literals;

const struct {
    Cpu::Features features;
    const char* extra;
    /* Cases that define a function pointer are not present in the library, see
       the pointed-to function documentation for more info */
    void(*function)(std::size_t, const char*, std::ptrdiff_t, const char*, const std::ptrdiff_t, char*, std::ptrdiff_t);
} AbsoluteTransformationsData[]{
    /* Order of the tests in increasing speed, for consistency then doing the
       same in HierarchyTest */
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    {Cpu::Scalar, "affine transform",
        absoluteTransformations3DScalarAffine},
    #endif
    {Cpu::Scalar, nullptr, nullptr},
    #ifdef CORRADE_ENABLE_SSE2
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    {Cpu::Sse2, "no shuffle, no cache, no affine transform",
        absoluteTransformations3DSse2NoShuffleNoCacheNoAffine},
    {Cpu::Sse2, "two-register shuffle, no cache, no affine transform",
        absoluteTransformations3DSse2TwoRegisterShuffleNoCacheNoAffine},
    {Cpu::Sse2, "two-register shuffle, no cache",
        absoluteTransformations3DSse2TwoRegisterShuffleNoCache},
    {Cpu::Sse2, "two-register shuffle",
        absoluteTransformations3DSse2TwoRegisterShuffle},
    #endif
    {Cpu::Sse2, nullptr, nullptr},
    #endif
    #ifdef CORRADE_ENABLE_AVX
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    {Cpu::Avx, "no cache, no affine transform",
        absoluteTransformations3DAvxNoCacheNoAffine},
    {Cpu::Avx, "no cache",
        absoluteTransformations3DAvxNoCache},
    #endif
    {Cpu::Avx, nullptr, nullptr},
    #endif
    #ifdef CORRADE_ENABLE_AVX_FMA
    {Cpu::Avx|Cpu::AvxFma, nullptr, nullptr},
    #endif
    #ifdef CORRADE_ENABLE_AVX512F
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    {Cpu::Avx512f, "no affine transform, broadcast",
        absoluteTransformations3DAvx512fNoAffineBroadcast},
    {Cpu::Avx512f, "no affine transform",
        absoluteTransformations3DAvx512fNoAffine},
    #endif
    {Cpu::Avx512f, nullptr, nullptr},
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    {Cpu::Avx512f, "hardcoded stride",
        absoluteTransformations3DAvx512fHardcodedStride},
    #endif
    #endif
    #ifdef CORRADE_ENABLE_SIMD128
    {Cpu::Simd128, nullptr, nullptr},
    #endif
};

/* 10k is just enough to get stable results in release also for AVX-512 and
   such but it takes just too much in Debug builds. Take this into account
   because it might make Debug *look* run faster than Release. */
constexpr std::size_t Many =
    #ifdef CORRADE_IS_DEBUG_BUILD
    1000
    #else
    10000
    #endif
    ;
constexpr std::size_t SlightlyLess = 100;

HierarchyBenchmark::HierarchyBenchmark() {
    addInstancedBenchmarks({&HierarchyBenchmark::absoluteTransformations3D,
                            &HierarchyBenchmark::absoluteTransformations3DClustered,
                            &HierarchyBenchmark::absoluteTransformations3DClusteredInPlace,
                            &HierarchyBenchmark::absoluteTransformations3DClusteredInterleaved,
                            &HierarchyBenchmark::absoluteTransformations3DChained}, 10,
        cpuVariantCount(AbsoluteTransformationsData),
        &HierarchyBenchmark::captureImplementations,
        &HierarchyBenchmark::restoreImplementations);

    _transformations = Utility::allocateAligned<Matrix4, 64>(ValueInit, Many);
    _parents = Containers::Array<Int>{ValueInit, Many};

    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<Int> distrib{0, Int(SlightlyLess + 1)};

    /* Parenting randomly to the first 100 objects */
    for(std::size_t i = 0; i != Many; ++i) {
        /* Restrict the parent to be in the [-1, i - 1] range */
        _parents[i] = (distrib(gen) % (i + 1)) - 1;
    }

    /* Basic verification that the parent indices are always back-referencing
       and never forward */
    for(std::size_t i = 0; i != _parents.size(); ++i)
        CORRADE_INTERNAL_ASSERT(_parents[i] < Int(i));

    /* Create a clustered parent array */
    {
        /* Count the number of occurences for each parent. One more element for
           parent being -1, one extra element at the front so the first offset
           stays 0, and then need one extra element at the end because there's
           SlightlyLess + 1 parents in total */
        UnsignedInt parentOffsets[SlightlyLess + 3]{};
        for(Int parent: _parents)
            ++parentOffsets[parent + 2];

        /* Turn the counts into an offset array. `[parentOffsets[i + 1],
           parentOffsets[i + 2])` then is a range containing parent `i`. */
        for(std::size_t i = 1; i != Containers::arraySize(parentOffsets); ++i)
            parentOffsets[i] += parentOffsets[i - 1];

        /* Fill the clustered array. After this, the parentOffsets array is
           shifted one element to the left compared to before, losing the
           offset for parent -1 but that's fine as we don't need it for
           anything after anymore. */
        _parentsClustered = Containers::Array<Int>{NoInit, _parents.size()};
        for(std::size_t i = 0; i != _parents.size(); ++i)
            _parentsClustered[parentOffsets[_parents[i] + 1]++] = _parents[i];

        /* Verification that the parent indices are always back-referencing
           and never forward, and that they're monotonic (and thus
           clustered) */
        for(std::size_t i = 0; i != _parentsClustered.size(); ++i) {
            CORRADE_INTERNAL_ASSERT(_parentsClustered[i] < Int(i));
            CORRADE_INTERNAL_ASSERT(i == 0 || _parentsClustered[i] >= _parentsClustered[i - 1]);
        }
    }

    /* Finally, chained parents just reference the immediately previous, with
       the first one being -1 */
    _parentsChained = Containers::Array<Int>{NoInit, _parents.size()};
    for(std::size_t i = 0; i != _parentsChained.size(); ++i)
        _parentsChained[i] = Int(i) - 1;
}

void HierarchyBenchmark::captureImplementations() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    _absoluteTransformations3DImplementation = Implementation::absoluteTransformations3D;
    #endif
}

void HierarchyBenchmark::restoreImplementations() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    Implementation::absoluteTransformations3D = _absoluteTransformations3DImplementation;
    #endif
}

void HierarchyBenchmark::absoluteTransformations3D() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    auto&& data = AbsoluteTransformationsData[testCaseInstanceId()];
    Implementation::absoluteTransformations3D = data.function ?
        data.function :
        Implementation::absoluteTransformations3DImplementation(data.features);
    #else
    auto&& data = cpuVariantCompiled(AbsoluteTransformationsData);
    #endif
    setTestCaseDescription(Utility::format(
        data.extra ? "{}, {}" : "{}",
        cpuVariantName(data), data.extra));

    if(!isCpuVariantSupported(data))
        CORRADE_SKIP("CPU features not supported");

    /* Checks the general case where parents point to some earlier object in a
       random order */

    Containers::Array<Matrix4> out = Utility::allocateAligned<Matrix4, 64>(NoInit, _transformations.size() + 1);

    Matrix4 globalTransformation =
        Matrix4::rotationY(37.0_degf)*
        Matrix4::translation({1.0f, 2.0f, 3.0f});
    CORRADE_BENCHMARK(100)
        absoluteTransformations3DInto(_transformations, _parents, out, globalTransformation);

    /* All _transformations are identity, so this should result in the global
       transform simply copied everywhere */
    CORRADE_COMPARE(out.back(), globalTransformation);
}

void HierarchyBenchmark::absoluteTransformations3DClustered() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    auto&& data = AbsoluteTransformationsData[testCaseInstanceId()];
    Implementation::absoluteTransformations3D = data.function ?
        data.function :
        Implementation::absoluteTransformations3DImplementation(data.features);
    #else
    auto&& data = cpuVariantCompiled(AbsoluteTransformationsData);
    #endif
    setTestCaseDescription(Utility::format(
        data.extra ? "{}, {}" : "{}",
        cpuVariantName(data), data.extra));

    if(!isCpuVariantSupported(data))
        CORRADE_SKIP("CPU features not supported");

    /* Like absoluteTransformations3D(), just using _parentsOrdered instead of
       _parents, checking the case where the same parent matrix is reused
       several times in a row */

    Containers::Array<Matrix4> out = Utility::allocateAligned<Matrix4, 64>(NoInit, _transformations.size() + 1);

    Matrix4 globalTransformation =
        Matrix4::rotationY(37.0_degf)*
        Matrix4::translation({1.0f, 2.0f, 3.0f});
    CORRADE_BENCHMARK(100)
        absoluteTransformations3DInto(_transformations, _parentsClustered, out, globalTransformation);

    /* All _transformations are identity, so this should result in the global
       transform simply copied everywhere */
    CORRADE_COMPARE(out.back(), globalTransformation);
}

void HierarchyBenchmark::absoluteTransformations3DClusteredInPlace() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    auto&& data = AbsoluteTransformationsData[testCaseInstanceId()];
    Implementation::absoluteTransformations3D = data.function ?
        data.function :
        Implementation::absoluteTransformations3DImplementation(data.features);
    #else
    auto&& data = cpuVariantCompiled(AbsoluteTransformationsData);
    #endif
    setTestCaseDescription(Utility::format(
        data.extra ? "{}, {}" : "{}",
        cpuVariantName(data), data.extra));

    if(!isCpuVariantSupported(data))
        CORRADE_SKIP("CPU features not supported");

    /* Like absoluteTransformations3DClustered(), but operating in-place which
       should be better for memory access as the same location is both being
       read from and written to. The use case is mainly for when the transforms
       are regemerated every time, here due to reusing the same memory 100
       times we cannot really test with anything else than all identity
       transforms. */

    Containers::Array<Matrix4> out = Utility::allocateAligned<Matrix4, 64>(NoInit, _transformations.size() + 1);
    Utility::copy(_transformations, out.exceptPrefix(1));

    CORRADE_BENCHMARK(100)
        absoluteTransformations3DInto(out.exceptPrefix(1), _parentsClustered, out);

    /* All _transformations are identity, so this should result in the global
       (identity) transform simply copied everywhere */
    CORRADE_COMPARE(out.back(), Matrix4{Math::IdentityInit});
}

void HierarchyBenchmark::absoluteTransformations3DClusteredInterleaved() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    auto&& data = AbsoluteTransformationsData[testCaseInstanceId()];
    Implementation::absoluteTransformations3D = data.function ?
        data.function :
        Implementation::absoluteTransformations3DImplementation(data.features);
    #else
    auto&& data = cpuVariantCompiled(AbsoluteTransformationsData);
    #endif
    setTestCaseDescription(Utility::format(
        data.extra ? "{}, {}" : "{}",
        cpuVariantName(data), data.extra));

    if(!isCpuVariantSupported(data))
        CORRADE_SKIP("CPU features not supported");
    if(Containers::StringView{data.extra}.contains("hardcoded stride"))
        CORRADE_SKIP("Variant doesn't support non-contiguous data");

    /* Like absoluteTransformations3DClustered() but having the input and
       output matrices interleaved for hopefully(?) better memory access
       patterns. The parent array is still separate because the alignment
       requirements would force 60 bytes of padding between each element which
       is I think worse than fetching a separate memory location. Compared to
       absoluteTransformations3DClusteredInPlace() this is likely to be still
       slower but has the advantage of not destroying the input data. */

    struct Data {
        Matrix4 in;
        Matrix4 out;
    };
    Containers::Array<Data> interleaved = Utility::allocateAligned<Data, 64>(NoInit, _transformations.size() + 1);
    Utility::copy(_transformations,
        stridedArrayView(interleaved).slice(&Data::in).exceptPrefix(1));

    Matrix4 globalTransformation =
        Matrix4::rotationY(37.0_degf)*
        Matrix4::translation({1.0f, 2.0f, 3.0f});
    CORRADE_BENCHMARK(100)
        absoluteTransformations3DInto(
            stridedArrayView(interleaved).slice(&Data::in).exceptPrefix(1),
            _parentsClustered,
            stridedArrayView(interleaved).slice(&Data::out), globalTransformation);

    /* All _transformations are identity, so this should result in the global
       transform simply copied everywhere */
    CORRADE_COMPARE(interleaved.back().out, globalTransformation);
}

void HierarchyBenchmark::absoluteTransformations3DChained() {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    auto&& data = AbsoluteTransformationsData[testCaseInstanceId()];
    Implementation::absoluteTransformations3D = data.function ?
        data.function :
        Implementation::absoluteTransformations3DImplementation(data.features);
    #else
    auto&& data = cpuVariantCompiled(AbsoluteTransformationsData);
    #endif
    setTestCaseDescription(Utility::format(
        data.extra ? "{}, {}" : "{}",
        cpuVariantName(data), data.extra));

    if(!isCpuVariantSupported(data))
        CORRADE_SKIP("CPU features not supported");

    /* Same as absoluteTransformations3D(), just using _parentsChained instead
       of _parents, checking the case where all matrices depend on the previous
       one, i.e. with no ability to parallelize the calculation across
       iterations */

    Containers::Array<Matrix4> out = Utility::allocateAligned<Matrix4, 64>(NoInit, _transformations.size() + 1);

    Matrix4 globalTransformation =
        Matrix4::rotationY(37.0_degf)*
        Matrix4::translation({1.0f, 2.0f, 3.0f});
    CORRADE_BENCHMARK(100)
        absoluteTransformations3DInto(_transformations, _parentsChained, out, globalTransformation);

    /* All _transformations are identity, so this should result in the global
       transform simply copied everywhere */
    CORRADE_COMPARE(out.back(), globalTransformation);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::HierarchyBenchmark)
