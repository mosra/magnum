#ifndef Magnum_Math_Geometry_Intersection_h
#define Magnum_Math_Geometry_Intersection_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Math::Geometry::Intersection
 */

#include "Math/Vector3.h"

namespace Magnum { namespace Math { namespace Geometry {

/** @brief Functions for computing intersections */
class Intersection {
    public:
        Intersection() = delete;

        /**
         * @brief %Intersection of a plane and line
         * @param planePosition Plane position
         * @param planeNormal   Plane normal
         * @param a             Starting point of the line
         * @param b             Ending point of the line
         * @return %Intersection point position, NaN if the line lies on the
         * plane or infinity if the intersection doesn't exist. %Intersection
         * point can be computed from the position with `a+intersection(...)*b`.
         * If returned value is in range @f$ [ 0 ; 1 ] @f$, the intersection
         * is inside the line segment defined by `a` and `b`.
         *
         * First the parameter *f* of parametric equation of the plane
         * is computed from plane normal **n** and plane position: @f[
         *      \begin{pmatrix} n_0 \\ n_1 \\ n_2 \end{pmatrix} \cdot
         *      \begin{pmatrix} x \\ y \\ z \end{pmatrix} - f = 0
         * @f]
         * Using plane normal **n**, parameter *f* and points **a** and **b**,
         * value of *t* is computed and returned. @f[
         *      \begin{array}{rcl}
         *          \Delta \boldsymbol b & = & \boldsymbol b - \boldsymbol a \\
         *          f & = & \boldsymbol n \cdot (\boldsymbol a + \Delta \boldsymbol b \cdot t) \\
         *          \Rightarrow t & = & \cfrac{f - \boldsymbol n \cdot \boldsymbol a}{\boldsymbol n \cdot \Delta \boldsymbol b}
         *      \end{array}
         * @f]
         */
        template<class T> static T planeLine(const Vector3<T>& planePosition, const Vector3<T>& planeNormal, const Vector3<T>& a, const Vector3<T>& b) {
            /* Compute f from normal and plane position */
            T f = Vector3<T>::dot(planePosition, planeNormal);

            /* Compute t */
            return (f-Vector3<T>::dot(planeNormal, a))/Vector3<T>::dot(planeNormal, b-a);
        }
};

}}}

#endif
