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

#include "BoundingVolume.h"

#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Range.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/FunctionsBatch.h"

namespace Magnum { namespace MeshTools {

Range3D boundingRange(const Containers::StridedArrayView1D<const Vector3>& points) {
    return Math::minmax(points);
}

Containers::Pair<Vector3, Float> boundingSphereBouncingBubble(const Containers::StridedArrayView1D<const Vector3>& points) {
    /* See comment about radius below, this is done for consistency */
    if(points.isEmpty()) return {{}, Math::TypeTraits<Float>::epsilon()};

    /** @todo Skip NaNs here? To match behaviour of boundingBoxAxisAligned()
        which uses minmax(). */
    Vector3 center = points[0];
    /* Radius ends up in the denominator in the first loop so we can't
       initialize it to 0. Unfortunately this also means the returned radius is
       always above or equal to epsilon. */
    Float radius = Math::TypeTraits<Float>::epsilon();
    Float radiusSquared = radius*radius;

    for(Int i = 0; i < 2; ++i) {
        for(const Vector3& p : points) {
            const Float ds = (p - center).dot();
            if(ds > radiusSquared) {
                const Float alphaInv = radius/Math::sqrt(ds);
                /* Not using alphaInv*alphaInv since that may lose precision */
                const Float alphaSqInv = radiusSquared/ds;
                radius = (1.0f/alphaInv + alphaInv)*0.5f*radius;
                center = ((1.0f + alphaSqInv)*center + (1.0f - alphaSqInv)*p)*0.5f;
                radiusSquared = radius*radius;
            }
        }
    }

    for(const Vector3& p : points) {
        const Vector3 diff = p - center;
        const Float ds = diff.dot();
        if(ds > radiusSquared) {
            const Float d = Math::sqrt(ds);
            radius = (radius + d)*0.5f;
            center = center + ((d - radius)/d*diff);
            radiusSquared = radius*radius;
        }
    }

    return {center, radius};
}

}}
