#ifndef Magnum_Math_Geometry_Distance_h
#define Magnum_Math_Geometry_Distance_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @m_deprecated_since{2018,10} Use @ref Magnum/Math/Distance.h instead.
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/Math/Distance.h"

CORRADE_DEPRECATED_FILE("use Magnum/Math/Distance.h instead")

/** @namespace Magnum::Math::Geometry::Distance
 * @m_deprecated_since{2018,10} Use @ref Magnum::Math::Distance instead.
 */

namespace Magnum { namespace Math { namespace Geometry { namespace Distance {

/** @brief @copybrief Math::Distance::linePointSquared(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::linePointSquared(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::linePointSquared() instead") T linePointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    return Math::Distance::linePointSquared(a, b, point);
}

/** @brief @copybrief Math::Distance::linePoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::linePoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::linePoint() instead") T linePoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    return Math::Distance::linePoint(a, b, point);
}

/** @brief @copybrief Math::Distance::linePointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::linePointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::linePointSquared() instead") T linePointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return Math::Distance::linePointSquared(a, b, point);
}

/** @brief @copybrief Math::Distance::linePoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::linePoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::linePoint() instead") T linePoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return Math::Distance::linePoint(a, b, point);
}

/** @brief @copybrief Math::Distance::lineSegmentPointSquared(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::lineSegmentPointSquared(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::lineSegmentPointSquared() instead") T lineSegmentPointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    return Math::Distance::lineSegmentPointSquared(a, b, point);
}

/** @brief @copybrief Math::Distance::lineSegmentPoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::lineSegmentPoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::lineSegmentPoint() instead") T lineSegmentPoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    return Math::Distance::lineSegmentPoint(a, b, point);
}

/** @brief @copybrief Math::Distance::lineSegmentPointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::lineSegmentPointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::lineSegmentPointSquared() instead") T lineSegmentPointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return Math::Distance::lineSegmentPointSquared(a, b, point);
}

/** @brief @copybrief Math::Distance::lineSegmentPoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::lineSegmentPoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::lineSegmentPointSquared() instead") T lineSegmentPoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return Math::Distance::lineSegmentPoint(a, b, point);
}

/** @brief @copybrief Math::Distance::pointPlaneScaled(const Vector3<T>&, const Vector4<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::pointPlaneScaled(const Vector3<T>&, const Vector4<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::pointPlaneScaled() instead") T pointPlaneScaled(const Vector3<T>& point, const Vector4<T>& plane) {
    return Math::Distance::pointPlaneScaled(point, plane);
}

/** @brief @copybrief Math::Distance::pointPlane(const Vector3<T>&, const Vector4<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::pointPlane(const Vector3<T>&, const Vector4<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::pointPlane() instead") T pointPlane(const Vector3<T>& point, const Vector4<T>& plane) {
    return Math::Distance::pointPlane(point, plane);
}

/** @brief @copybrief Math::Distance::pointPlaneNormalized(const Vector3<T>&, const Vector4<T>&)
 * @m_deprecated_since{2018,10} Use @ref Math::Distance::pointPlaneNormalized(const Vector3<T>&, const Vector4<T>&)
 *      instead.
 */
template<class T> inline CORRADE_DEPRECATED("use Math::Distance::pointPlaneNormalized() instead") T pointPlaneNormalized(const Vector3<T>& point, const Vector4<T>& plane) {
    return Math::Distance::pointPlaneNormalized(point, plane);
}

}}}}
#else
#error use Magnum/Math/Distance.h instead
#endif

#endif
