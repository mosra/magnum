/*
    Magnum::MeshTools
        — algorithms for dealing with mesh data

    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1MeshTools.html

    Depends on CorradeArray.h and MagnumMathBatch.hpp.

    This is a single-header library generated from the Magnum project. With the
    goal being easy integration, it's deliberately free of all comments to keep
    the file size small. More info, changelogs and full docs here:

    -   Project homepage — https://magnum.graphics/magnum/
    -   Documentation — https://doc.magnum.graphics/
    -   GitHub project page — https://github.com/mosra/magnum
    -   GitHub Singles repository — https://github.com/mosra/magnum-singles

    The library has a separate non-inline implementation part, enable it *just
    once* like this:

        #define MAGNUM_MESHTOOLS_IMPLEMENTATION
        #include <MagnumMeshTools.hpp>

    If you need the deinlined symbols to be exported from a shared library,
    `#define MAGNUM_MESHTOOLS_EXPORT` as appropriate.

    v2020.06-3128-g47b22 (2025-01-07)
    -   Initial release

    Generated from Corrade {{revision:corrade/src}} and
        Magnum {{revision:magnum/src}}, {{stats:loc}} / {{stats:preprocessed}} LoC
*/

#include "base.h"
// {{includes}}
#include <cstddef> /* std::size_t, std::ptrdiff_t */

/* Exclude everything that relies on Trade, or also the MeshPrimitive etc enums
   from Magnum core namespace */
#pragma ACME enable MAGNUM_SINGLES_NO_TRADE_DEPENDENCY

/* Our own subset of visibility macros */
#pragma ACME enable Magnum_MeshTools_visibility_h
#pragma ACME enable Corrade_Utility_VisibilityMacros_h
#ifndef MAGNUM_MESHTOOLS_EXPORT
#define MAGNUM_MESHTOOLS_EXPORT
#endif

/* We don't need anything from configure.h for the declarations, only the
   implementation does */
#pragma ACME enable Corrade_configure_h
#pragma ACME enable Magnum_configure_h

/* This is just a tiny subset of the full forward declaration header in
   MagnumMath. We don't need that whole header, especially given the user may
   not need to use any of the Math types. */
#include "Magnum/Types.h"
#ifndef MagnumMeshTools_hpp
#define MagnumMeshTools_hpp
namespace Corrade { namespace Containers {

template<class, class> class Pair;
template<unsigned, class> class StridedArrayView;
template<class T> using StridedArrayView1D = StridedArrayView<1, T>;
template<class T> using StridedArrayView2D = StridedArrayView<2, T>;

}}
namespace Magnum {

using namespace Corrade;

namespace Math {
    template<class> class Vector3;
    template<class> class Range3D;
}

typedef Math::Vector3<Float> Vector3;
typedef Math::Range3D<Float> Range3D;

}
#endif
/* This is a verbatim copy from CorradeArray.h. In case it gets included, only
   one of the two occurences gets picked, and thus it has to be present in
   full, including StaticArray and other types we don't strictly need, in order
   to make sure the default template argument is listed exactly once. */
#ifndef CorradeArray_h
#define CorradeArray_h
namespace Corrade { namespace Containers {

/* In case Corrade/Containers/Containers.h is included too, these two would
   conflict */
#ifndef Corrade_Containers_Containers_h
template<class T, class = void(*)(T*, std::size_t)> class Array;
#endif
/* Needs to be defined here because it's referenced before its definition */
template<std::size_t, class> class StaticArray;
/* These need to be defined here because the other occurence is guarded with
   #ifndef CORRADE_MSVC2015_COMPATIBILITY */
template<class T> using Array2 = StaticArray<2, T>;
template<class T> using Array3 = StaticArray<3, T>;
template<class T> using Array4 = StaticArray<4, T>;

}}
#endif
#pragma ACME enable Magnum_Magnum_h
#include "Magnum/MeshTools/BoundingVolume.h"
#include "Magnum/MeshTools/GenerateIndices.h"
// TODO CompressIndices once it's renamed to Pack
// TODO FlipNormals?
// TODO GenerateNormals?
// TODO RemoveDuplicates?
#ifdef MAGNUM_MESHTOOLS_IMPLEMENTATION
// {{includes}}
#if !defined(CORRADE_ASSERT_UNREACHABLE) && !defined(NDEBUG)
#include <cassert>
#endif

/* CorradeArray and MagnumMathBatch is an implementation dependency.
   CorradeStridedArrayView is pulled in by MagnumMathBatch already. */
#pragma ACME noexpand CorradeArray.h
#pragma ACME noexpand MagnumMathBatch.hpp
#include "CorradeArray.h"
#include "MagnumMathBatch.hpp"

/* We need CORRADE_TARGET_BIG_ENDIAN from configure.h */
#ifdef __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define CORRADE_TARGET_BIG_ENDIAN
#elif __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error what kind of endianness is this?
#endif
#elif defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MIPSEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define CORRADE_TARGET_BIG_ENDIAN
#endif

/* All asserts we use are present in dependencies already, except for
   CORRADE_ASSERT_UNREACHABLE */
#include "singles/assert.h"
#pragma ACME forget CORRADE_ASSERT_UNREACHABLE
#pragma ACME enable Corrade_Utility_Move_h
#include <Corrade/Utility/Assert.h>

#pragma ACME enable Corrade_Containers_Array_h
#pragma ACME enable Corrade_Containers_Pair_h
#pragma ACME enable Corrade_Containers_StridedArrayView_h
#pragma ACME enable Magnum_Math_Vector3_h
#pragma ACME enable Magnum_Math_Range_h
#pragma ACME enable Magnum_Math_FunctionsBatch_h
/* This guard is there only for tests I think, not needed for anything else */
#pragma ACME disable Magnum_Math_halfTables_hpp
#include "Magnum/MeshTools/BoundingVolume.cpp"
#include "Magnum/MeshTools/GenerateIndices.cpp"
#endif
