#ifndef Magnum_SceneTools_OrderClusterParents_h
#define Magnum_SceneTools_OrderClusterParents_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function @ref Magnum::SceneTools::orderClusterParents(), @ref Magnum::SceneTools::orderClusterParentsInto()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Calculate ordered and clustered parents
@m_since_latest

Extracts the @ref Trade::SceneField::Parent field from @p scene and converts it
to match the following rules:

-   a parent object reference appears always before any of its children
-   the array is clustered so children sharing the same parent are together

This form is useful primarily for calculating absolute object transformations,
for example:

@snippet MagnumSceneTools.cpp orderClusterParents-transformations

The operation is done in an @f$ \mathcal{O}(n) @f$ execution time and memory
complexity, with @f$ n @f$ being @ref Trade::SceneData::mappingBound(). The
@ref Trade::SceneField::Parent field is expected to be contained in the scene,
having no cycles (i.e., every node listed just once) and not being sparse
(i.e., every node listed in the field reachable from the root).
@see @ref Trade::SceneData::hasField()
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Pair<UnsignedInt, Int>> orderClusterParents(const Trade::SceneData& scene);

/**
@brief Calculate ordered and clustered parents into a pre-allocated view
@m_since_latest

Like @ref orderClusterParents(), but puts the result into
@p mappingDestination and @p parentDestination instead of allocating a new
array. Expect that both views have a size equal to size of the
@ref Trade::SceneField::Parent view in @p scene.
@see @ref Trade::SceneData::fieldSize(SceneField) const
*/
MAGNUM_SCENETOOLS_EXPORT void orderClusterParentsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& parentDestination);

}}

#endif
