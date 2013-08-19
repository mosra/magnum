#ifndef Magnum_DebugTools_Implementation_ForceRendererTransformation_h
#define Magnum_DebugTools_Implementation_ForceRendererTransformation_h
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

#include "Math/Functions.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Magnum.h"
#include "DimensionTraits.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

template<UnsignedInt dimensions> typename DimensionTraits<dimensions, Float>::MatrixType forceRendererTransformation(const typename DimensionTraits<dimensions, Float>::VectorType& forcePosition, const typename DimensionTraits<dimensions, Float>::VectorType& force);

template<> inline Matrix3 forceRendererTransformation<2>(const Vector2& forcePosition, const Vector2& force) {
    return Matrix3::from({force, Vector2(-force.y(), force.x())}, forcePosition);
}

template<> Matrix4 forceRendererTransformation<3>(const Vector3& forcePosition, const Vector3& force) {
    const Matrix4 translation = Matrix4::translation(forcePosition);
    const Float forceLength = force.length();

    /* Zero length, zero scaling */
    if(forceLength < Math::TypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling(Vector3(0.0f));

    const Float dot = Vector3::dot(force/forceLength, Vector3::xAxis());

    /* Force is parallel to X axis, just scaling */
    if(Math::abs(dot) > 1.0f - Math::TypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling({Math::sign(dot)*forceLength, forceLength, forceLength});

    /* Normal of plane going through force vector and X axis vector */
    const Vector3 normal = Vector3::cross(Vector3::xAxis(), force).normalized();

    /* Third base vector, orthogonal to force and normal */
    const Vector3 binormal = Vector3::cross(normal, force/forceLength);
    CORRADE_INTERNAL_ASSERT(binormal.isNormalized());

    /* Transformation matrix from scaled base vectors and translation vector */
    return Matrix4::from({force, normal*forceLength, binormal*forceLength}, forcePosition);
}

}}}

#endif
