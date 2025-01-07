/*
    Magnum::Math
        — a graphics-focused vector math library, batch APIs

    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1Math.html

    Depends on ContainersStridedArrayView.h and MagnumMath.hpp.

    This is a single-header library generated from the Magnum project. With the
    goal being easy integration, it's deliberately free of all comments to keep
    the file size small. More info, changelogs and full docs here:

    -   Project homepage — https://magnum.graphics/magnum/
    -   Documentation — https://doc.magnum.graphics/
    -   GitHub project page — https://github.com/mosra/magnum
    -   GitHub Singles repository — https://github.com/mosra/magnum-singles

    The library has a separate non-inline implementation part, enable it *just
    once* like this:

        #define MAGNUM_MATH_BATCH_IMPLEMENTATION
        #include <MagnumMathBatch.hpp>

    If you need the deinlined symbols to be exported from a shared library,
    `#define MAGNUM_EXPORT` as appropriate.

    v2020.06-3128-g47b22 (2025-01-07)
    -   Initial release

    Generated from Corrade {{revision:corrade/src}} and
        Magnum {{revision:magnum/src}}, {{stats:loc}} / {{stats:preprocessed}} LoC
*/

#include "base.h"
// {{includes}}

/* All asserts we use are present in dependencies already */
#include "singles/assert.h"

/* Some castInto() overloads are convenience wrappers over Utility::copy(). Use
   a (slower) fallback instead. */
#pragma ACME enable MAGNUM_SINGLES_NO_UTILITY_ALGORITHMS_DEPENDENCY

/* We don't need anything from configure.h here that isn't pulled in by
   MagnumMath already */
#pragma ACME enable Corrade_configure_h
#pragma ACME enable Magnum_configure_h

/* CorradeStridedArrayView and MagnumMath is a dependency */
#pragma ACME noexpand CorradeStridedArrayView.h
#pragma ACME enable Corrade_Containers_Containers_h
#pragma ACME enable Corrade_Containers_Pair_h
#pragma ACME enable Corrade_Containers_StridedArrayView_h
#pragma ACME enable Corrade_Utility_Move_h
#pragma ACME enable Corrade_Utility_VisibilityMacros_h
#include "CorradeStridedArrayView.h"
#pragma ACME noexpand MagnumMath.hpp
#pragma ACME enable Magnum_visibility_h
#pragma ACME enable Magnum_Magnum_h
#pragma ACME enable Magnum_Types_h
#pragma ACME enable Magnum_Math_Functions_h
#pragma ACME enable Magnum_Math_Packing_h
/* This guard is there only for tests I think, not needed for anything else */
#pragma ACME disable Magnum_Math_halfTables_hpp
#include "MagnumMath.hpp"

// TODO ColorBatch once it's more useful
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/PackingBatch.h"
#ifdef MAGNUM_MATH_BATCH_IMPLEMENTATION
#include "Magnum/Math/PackingBatch.cpp"
#endif
