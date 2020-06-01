#ifndef Magnum_DebugTools_Implementation_ForceRendererTransformation_h
#define Magnum_DebugTools_Implementation_ForceRendererTransformation_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/DimensionTraits.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

template<UnsignedInt dimensions> MatrixTypeFor<dimensions, Float> forceRendererTransformation(const VectorTypeFor<dimensions, Float>& forcePosition, const VectorTypeFor<dimensions, Float>& force);

template<> inline Matrix3 forceRendererTransformation<2>(const Vector2& forcePosition, const Vector2& force) {
    return Matrix3::from({force, force.perpendicular()}, forcePosition);
}

template<> Matrix4 forceRendererTransformation<3>(const Vector3& forcePosition, const Vector3& force) {
    const Matrix4 translation = Matrix4::translation(forcePosition);
    const Float forceLength = force.length();

    /* Zero length, zero scaling */
    if(forceLength < Math::TypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling(Vector3(0.0f));

    const Float dot = Math::dot(force/forceLength, Vector3::xAxis());

    /* Force is parallel to X axis, just scaling */
    if(Math::abs(dot) > 1.0f - Math::TypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling({Math::sign(dot)*forceLength, forceLength, forceLength});

    /* Normal of plane going through force vector and X axis vector */
    const Vector3 normal = Math::cross(Vector3::xAxis(), force).normalized();

    /* Third base vector, orthogonal to force and normal */
    const Vector3 bitangent = Math::cross(normal, force/forceLength);
    CORRADE_INTERNAL_ASSERT(bitangent.isNormalized());

    /* Transformation matrix from scaled base vectors and translation vector */
    return Matrix4::from({force, normal*forceLength, bitangent*forceLength}, forcePosition);
}

}}}

#endif
