#ifndef Magnum_Math_Geometry_Intersection_h
#define Magnum_Math_Geometry_Intersection_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016, 2018 Jonathan Hale <squareys@googlemail.com>

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
 * @m_deprecated_since{201,10} Use @ref Magnum/Math/Intersection.h instead.
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/Math/Intersection.h"

CORRADE_DEPRECATED_FILE("use Magnum/Math/Intersection.h instead")

/** @namespace Magnum::Math::Geometry::Intersection
 * @m_deprecated_since{2018,10} Use @ref Magnum::Math::Intersection instead.
 */

namespace Magnum { namespace Math { namespace Geometry { namespace Intersection {

/** @brief @copybrief Math::Intersection::lineSegmentLineSegment(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Intersection::lineSegmentLineSegment(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Intersection::lineSegmentLineSegment() instead") std::pair<T, T> lineSegmentLineSegment(const Vector2<T>& p, const Vector2<T>& r, const Vector2<T>& q, const Vector2<T>& s) {
    return Math::Intersection::lineSegmentLineSegment(p, r, q, s);
}

/** @brief @copybrief Math::Intersection::lineSegmentLine(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Intersection::lineSegmentLine(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Intersection::lineSegmentLine() instead") T lineSegmentLine(const Vector2<T>& p, const Vector2<T>& r, const Vector2<T>& q, const Vector2<T>& s) {
    return Math::Intersection::lineSegmentLine(p, r, q, s);
}

/** @brief @copybrief Math::Intersection::planeLine(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Intersection::planeLine(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Intersection::planeLine() instead") T planeLine(const Vector3<T>& planePosition, const Vector3<T>& planeNormal, const Vector3<T>& p, const Vector3<T>& r) {
    return Math::Intersection::planeLine(planePosition, planeNormal, p, r);
}

/** @brief @copybrief Math::Intersection::pointFrustum(const Vector3<T>&, const Frustum<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Intersection::pointFrustum(const Vector3<T>&, const Frustum<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Intersection::pointFrustum() instead") bool pointFrustum(const Vector3<T>& point, const Frustum<T>& frustum) {
    return Math::Intersection::pointFrustum(point, frustum);
}

/**
 * @brief @copybrief Math::Intersection::rangeFrustum(const Range3D<T>&, const Frustum<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Intersection::rangeFrustum(const Range3D<T>&, const Frustum<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Intersection::rangeFrustum() instead") bool boxFrustum(const Range3D<T>& box, const Frustum<T>& frustum) {
    return Math::Intersection::rangeFrustum(box, frustum);
}

}}}}
#else
#error use Magnum/Math/Intesection.h instead
#endif

#endif
