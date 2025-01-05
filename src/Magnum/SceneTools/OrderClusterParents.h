#ifndef Magnum_SceneTools_OrderClusterParents_h
#define Magnum_SceneTools_OrderClusterParents_h
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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Function @ref Magnum::SceneTools::orderClusterParents(), @ref Magnum::SceneTools::orderClusterParentsInto()
 * @m_deprecated_since_latest Use @ref Magnum/SceneTools/Hierarchy.h and the
 *      @relativeref{Magnum,SceneTools::parentsBreadthFirst()} function
 *      instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/SceneTools/Hierarchy.h"

CORRADE_DEPRECATED_FILE("use Magnum/SceneTools/Hierarchy.h and the SceneTools::parentsBreadthFirst() function instead")

namespace Magnum { namespace SceneTools {

/* Made header-only to not have to maintain a deprecated source file */

/**
@brief Calculate ordered and clustered parents
@m_deprecated_since_latest Use @ref parentsBreadthFirst(const Trade::SceneData&)
    instead.
*/
inline CORRADE_DEPRECATED("use parentsBreadthFirst() instead") Containers::Array<Containers::Pair<UnsignedInt, Int>> orderClusterParents(const Trade::SceneData& scene) {
    return parentsBreadthFirst(scene);
}

/**
@brief Calculate ordered and clustered parents
@m_deprecated_since_latest Use @ref parentsBreadthFirstInto(const Trade::SceneData&, const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Int>&)
    instead.
*/
inline CORRADE_DEPRECATED("use parentsBreadthFirstInto() instead") void orderClusterParentsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& parentDestination) {
    return parentsBreadthFirstInto(scene, mappingDestination, parentDestination);
}

}}
#else
#error use Magnum/SceneTools/Hierarchy.h and the SceneTools::parentsBreadthFirst() function instead
#endif

#endif
