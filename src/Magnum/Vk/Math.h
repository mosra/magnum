#ifndef Magnum_Vk_Math_h
#define Magnum_Vk_Math_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief
 */

#include <Magnum/Magnum.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/Math/Matrix4.h>

#include "vulkan.h"

namespace Magnum { namespace Math {

namespace Implementation {

/* VkExtent3D */
template<> struct VectorConverter<3, UnsignedInt, VkExtent3D> {
    static Vector<3, UnsignedInt> from(const VkExtent3D& other) {
        return {other.width, other.height, other.depth};
    }

    static VkExtent3D to(const Vector<3, UnsignedInt>& other) {
        return {other[0], other[1], other[2]};
    }
};

}


/**
 * @brief 3D perspective projection matrix with clipping range [0.0, 1.0]
 * @param size      Size of near clipping plane
 * @param near      Near clipping plane
 * @param far       Far clipping plane
 *
 * @see @ref orthographicProjection(), @ref Matrix3::projection()
 */
template<class T> Matrix4<T> perspectiveProjectionZeroToOne(const Vector2<T>& size, const T near, const T far) {
    Vector2<T> xyScale = 2*near/size;
    T zScale = T(1.0)/(near-far);

    return {{xyScale.x(),        T(0),            T(0),  T(0)},
            {       T(0),-xyScale.y(),            T(0),  T(0)},
            {       T(0),        T(0),      far*zScale, T(-1)}, // difference in [2][2]
            {       T(0),        T(0), far*near*zScale,  T(0)}};// difference in [3][2]
}

/**
 * @brief 3D perspective projection matrix with clipping range [0.0, 1.0]
 * @param fov           Field of view angle (horizontal)
 * @param aspectRatio   Aspect ratio
 * @param near          Near clipping plane
 * @param far           Far clipping plane
 *
 * @see @ref orthographicProjection(), @ref Matrix3::projection()
 */
template<class T> Matrix4<T> perspectiveProjectionZeroToOne(Rad<T> fov, T aspectRatio, T near, T far) {
    const T xyScale = 2*std::tan(T(fov)/2)*near;
    return perspectiveProjectionZeroToOne(Vector2<T>(xyScale, xyScale/aspectRatio), near, far);
}


}}

#endif
