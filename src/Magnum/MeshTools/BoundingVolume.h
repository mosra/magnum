#ifndef Magnum_MeshTools_BoundingVolume_h
#define Magnum_MeshTools_BoundingVolume_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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
 * @brief Function @ref Magnum::MeshTools::boundingRange(), @ref Magnum::MeshTools::boundingSphereBouncingBubble()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"

namespace Magnum { namespace MeshTools {

/**
@brief Calculate a bounding range
@param positions    Vertex positions
@return Bounding range
@m_since_latest

Same as @ref Math::minmax(const Containers::StridedArrayView1D<const T>&).
@see @ref Math::Intersection::rayRange(),
    @ref Math::Intersection::rangeFrustum(),
    @ref Math::Intersection::rangeCone(), @ref meshtools-bounding-volume
*/
MAGNUM_MESHTOOLS_EXPORT Range3D boundingRange(const Containers::StridedArrayView1D<const Vector3>& positions);

/**
@brief Calculate an approximate bounding sphere using the Bouncing Bubble
    algorithm
@param positions    Vertex positions
@return Sphere center and radius
@m_since_latest

The resulting bounding sphere is not usually minimal. According to the author,
a 1% to 2% error can be expected. Due to the nature of the algorithm, the
radius is never below @ref Math::TypeTraits::epsilon(), even for empty or
entirely overlapping lists of points. <em>NaN</em>s are ignored, unless the
first position is <em>NaN</em> in which case it is propagated. Algorithm used:
* *Bo Tian --- Bouncing Bubble: A fast algorithm for Minimal Enclosing Ball
problem, 2012, https://www.grin.com/document/204869*.
@see @ref Math::Intersection::pointSphere(),
    @ref Math::Intersection::sphereFrustum(),
    @ref Math::Intersection::sphereCone(),
    @ref Math::Intersection::sphereConeView(), @ref meshtools-bounding-volume
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Pair<Vector3, Float> boundingSphereBouncingBubble(const Containers::StridedArrayView1D<const Vector3>& positions);

}}

#endif
