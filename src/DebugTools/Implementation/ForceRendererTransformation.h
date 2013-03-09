#ifndef Magnum_DebugTools_Implementation_ForceRendererTransformation_h
#define Magnum_DebugTools_Implementation_ForceRendererTransformation_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Magnum.h"
#include "DimensionTraits.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

template<UnsignedInt dimensions> typename DimensionTraits<dimensions>::MatrixType forceRendererTransformation(const typename DimensionTraits<dimensions>::VectorType& forcePosition, const typename DimensionTraits<dimensions>::VectorType& force);

template<> inline Matrix3 forceRendererTransformation<2>(const Vector2& forcePosition, const Vector2& force) {
    return Matrix3::from({force, Vector2(-force.y(), force.x())}, forcePosition);
}

template<> inline Matrix4 forceRendererTransformation<3>(const Vector3& forcePosition, const Vector3& force) {
    const Matrix4 translation = Matrix4::translation(forcePosition);
    const Float forceLength = force.length();

    /* Zero length, zero scaling */
    if(forceLength < Math::MathTypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling(Vector3(0.0f));

    const Float dot = Vector3::dot(force/forceLength, Vector3::xAxis());

    /* Force is parallel to X axis, just scaling */
    if(dot > 1.0f - Math::MathTypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling(Vector3(forceLength));

    /* Force is antiparallel to X axis, scaling inverted on X */
    if(-dot > 1.0f - Math::MathTypeTraits<Float>::epsilon())
        return translation*Matrix4::scaling({-forceLength, forceLength, forceLength});

    /* Normal of plane going through force vector and X axis vector */
    const Vector3 normal = Vector3::cross(Vector3::xAxis(), force).normalized();

    /* Third base vector, orthogonal to force and normal */
    const Vector3 binormal = Vector3::cross(normal, force).normalized();

    /* Transformation matrix from scaled base vectors and translation vector */
    return Matrix4::from({force, normal*forceLength, binormal*forceLength}, forcePosition);
}

}}}

#endif
