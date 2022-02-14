#ifndef Magnum_MeshTools_Transform_h
#define Magnum_MeshTools_Transform_h
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
 * @brief Function @ref Magnum::MeshTools::transformVectorsInPlace(), @ref Magnum::MeshTools::transformVectors(), @ref Magnum::MeshTools::transformPointsInPlace(), @ref Magnum::MeshTools::transformPoints(), @ref Magnum::MeshTools::transform2D(), @ref Magnum::MeshTools::transform2DInPlace(), @ref Magnum::MeshTools::transform3D(), @ref Magnum::MeshTools::transform3DInPlace(), @ref Magnum::MeshTools::transformTextureCoordinates2D(), @ref Magnum::MeshTools::transformTextureCoordinates2DInPlace()
 */

#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/MeshTools/InterleaveFlags.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Transform vectors in-place using given transformation

Usable for one-time mesh transformations that would otherwise negatively affect
dependent objects, such as (uneven) scaling. Accepts any forward-iterable type
with compatible vector type as @p vectors. Expects that @ref Math::Quaternion "Quaternion"
is normalized, no further requirements are for other transformation
representations.

Unlike in @ref transformPointsInPlace(), the transformation does not involve
translation.

Example usage:

@snippet MagnumMeshTools.cpp transformVectors

@see @ref transformVectors(), @ref Matrix3::transformVector(),
    @ref Matrix4::transformVector(), @ref Complex::transformVector(),
    @ref Quaternion::transformVectorNormalized()
@todo GPU transform feedback implementation (otherwise this is only bad joke)
*/
template<class T, class U> void transformVectorsInPlace(const Math::Matrix4<T>& matrix, U&& vectors) {
    for(auto& vector: vectors) vector = matrix.transformVector(vector);
}

/** @overload */
template<class T, class U> void transformVectorsInPlace(const Math::Matrix3<T>& matrix, U&& vectors) {
    for(auto& vector: vectors) vector = matrix.transformVector(vector);
}

/** @overload */
template<class T, class U> void transformVectorsInPlace(const Math::Complex<T>& complex, U&& vectors) {
    for(auto& vector: vectors) vector = complex.transformVector(vector);
}

/** @overload */
template<class T, class U> void transformVectorsInPlace(const Math::Quaternion<T>& normalizedQuaternion, U&& vectors) {
    for(auto& vector: vectors) vector = normalizedQuaternion.transformVectorNormalized(vector);
}

/**
@brief Transform vectors using given transformation

Returns transformed vectors instead of modifying them in-place. See
@ref transformVectorsInPlace() for more information.
*/
template<class T, class U> U transformVectors(const T& transformation, U vectors) {
    U result(std::move(vectors));
    transformVectorsInPlace(transformation, result);
    return result;
}

/**
@brief Transform points in-place using given transformation

Usable for one-time mesh transformations that would otherwise negatively affect
dependent objects, such as (uneven) scaling. Accepts any forward-iterable type
with compatible vector type as @p vectors. Expects that
@ref Math::DualQuaternion "DualQuaternion" is normalized, no further
requirements are for other transformation representations.

Unlike in @ref transformVectorsInPlace(), the transformation also involves
translation.

Example usage:

@snippet MagnumMeshTools.cpp transformPoints

@see @ref transformPoints(), @ref Matrix3::transformPoint(),
    @ref Matrix4::transformPoint(),
    @ref DualQuaternion::transformPointNormalized()
*/
template<class T, class U> void transformPointsInPlace(const Math::Matrix4<T>& matrix, U&& points) {
    for(auto& point: points) point = matrix.transformPoint(point);
}

/** @overload */
template<class T, class U> void transformPointsInPlace(const Math::Matrix3<T>& matrix, U&& points) {
    for(auto& point: points) point = matrix.transformPoint(point);
}

/** @overload */
template<class T, class U> void transformPointsInPlace(const Math::DualComplex<T>& dualComplex, U&& points) {
    for(auto& point: points) point = dualComplex.transformPoint(point);
}

/** @overload */
template<class T, class U> void transformPointsInPlace(const Math::DualQuaternion<T>& normalizedDualQuaternion, U&& points) {
    for(auto& point: points) point = normalizedDualQuaternion.transformPointNormalized(point);
}

/**
@brief Transform points using given transformation

Returns transformed points instead of modifying them in-place. See
@ref transformPointsInPlace() for more information.
*/
template<class T, class U> U transformPoints(const T& transformation, U vectors) {
    U result(std::move(vectors));
    transformPointsInPlace(transformation, result);
    return result;
}

/**
@brief Transform 2D positions in a mesh data
@m_since_latest

Expects that the mesh contains a two-dimensional
@ref Trade::MeshAttribute::Position with index @p id and that the attribute
does not have an implementation-specific format. To avoid data loss with packed
types, the positions are converted to @ref VertexFormat::Vector2 if not
already. In that case the data layouting is done by @ref interleavedLayout()
with the @p flags parameter propagated to it, see its documentation for
detailed behavior description. Other attributes, position attributes other than
@p id, and indices (if any) are passed through untouched.

See also @ref transform2D(Trade::MeshData&&, const Matrix3&, UnsignedInt, InterleaveFlags)
for a potentially more efficient operation instead of always performing a full
copy, you can also do an in-place transformation using @ref transform2DInPlace().
@see @ref transform3D(), @ref transformTextureCoordinates2D(),
    @ref Trade::MeshData::attributeCount(MeshAttribute) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData transform2D(const Trade::MeshData& data, const Matrix3& transformation, UnsignedInt id = 0, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Transform 2D positions in a mesh data
@m_since_latest

Compared to @ref transform2D(const Trade::MeshData&, const Matrix3&, UnsignedInt, InterleaveFlags)
this function can can perform the transformation in-place, transferring the
data ownership to the returned instance, if both vertex and index data is
owned, vertex data is mutable and the positions with index @p id are
@ref VertexFormat::Vector2.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData transform2D(Trade::MeshData&& data, const Matrix3& transformation, UnsignedInt id = 0, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Transform 2D positions in a mesh data in-place
@m_since_latest

Expects that the mesh has mutable vertex data and contains a two-dimensional
@ref Trade::MeshAttribute::Position with index @p id. To avoid data loss with
packed types, the in-place operation requires the position type to be
@ref VertexFormat::Vector2 --- if you can't guarantee that, use
@ref transform2D() instead. Other attributes, position attributes other than
@p id, and indices (if any) are left untouched.
@see @ref transform3DInPlace(), @ref transformTextureCoordinates2DInPlace(),
    @ref Trade::MeshData::vertexDataFlags(),
    @ref Trade::MeshData::attributeCount(MeshAttribute) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt) const
*/
MAGNUM_MESHTOOLS_EXPORT void transform2DInPlace(Trade::MeshData& data, const Matrix3& transformation, UnsignedInt id = 0);

/**
@brief Transform 3D positions, normals, tangents and bitangents in a mesh data
@m_since_latest

Expects that the mesh contains a three-dimensional
@ref Trade::MeshAttribute::Position with index @p id. If
@ref Trade::MeshAttribute::Normal, @ref Trade::MeshAttribute::Tangent or
@ref Trade::MeshAttribute::Bitangent with index @p id are present as well,
those get transformed with @ref Matrix4::normalMatrix() extracted out of
@p transformation. All these attributes are expected to not have an
implementation-specific format. To avoid data loss with packed types, the
positions, normals and bitangents are converted to @ref VertexFormat::Vector3
if not already, tangents to either @ref VertexFormat::Vector3 or
@ref VertexFormat::Vector4 if not already. In that case the data layouting is
done by @ref interleavedLayout() with the @p flags parameter propagated to it,
see its documentation for detailed behavior description. Other attributes,
additional position/TBN attributes other than @p id, and indices (if any) are
passed through untouched.

See also @ref transform3D(Trade::MeshData&&, const Matrix4&, UnsignedInt, InterleaveFlags)
for a potentially more efficient operation instead of always performing a full
copy, you can also do an in-place transformation using @ref transform3DInPlace().
@see @ref transform2D(), @ref transformTextureCoordinates2D(),
    @ref Trade::MeshData::attributeCount(MeshAttribute) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData transform3D(const Trade::MeshData& data, const Matrix4& transformation, UnsignedInt id = 0, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Transform 3D positions, normals, tangenta and bitangents in a mesh data
@m_since_latest

Compared to @ref transform3D(const Trade::MeshData&, const Matrix4&, UnsignedInt, InterleaveFlags)
this function can can perform the transformation in-place, transferring the
data ownership to the returned instance, if both vertex and index data is
owned, vertex data is mutable, positions, normals and bitangents (if present)
are @ref VertexFormat::Vector3 and tangents (if present) either
@ref VertexFormat::Vector3 or @ref VertexFormat::Vector4.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData transform3D(Trade::MeshData&& data, const Matrix4& transformation, UnsignedInt id = 0, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Transform 3D positions, normals, tangents and bitangents in a mesh data in-place
@m_since_latest

Expects that the mesh has mutable vertex data and contains at least a
three-dimensional @ref Trade::MeshAttribute::Position with index @p id;
optionally also @ref Trade::MeshAttribute::Normal,
@ref Trade::MeshAttribute::Tangent or @ref Trade::MeshAttribute::Bitangent with
index @p id, those get transformed with @ref Matrix4::normalMatrix() extracted
out of @p transformation. To avoid data loss with packed types, the in-place
operation requires the position, normal and bitangent types to be
@ref VertexFormat::Vector3 and tangent either @ref VertexFormat::Vector3 or
@ref VertexFormat::Vector4 --- if you can't guarantee that, use
@ref transform3D() instead. Other attributes, position/TBN attributes other
than @p id, and indices (if any) are left untouched.
@see @ref transform2DInPlace(), @ref transformTextureCoordinates2DInPlace(),
    @ref Trade::MeshData::vertexDataFlags(),
    @ref Trade::MeshData::attributeCount(MeshAttribute) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt) const
*/
MAGNUM_MESHTOOLS_EXPORT void transform3DInPlace(Trade::MeshData& data, const Matrix4& transformation, UnsignedInt id = 0);

/**
@brief Transform 2D texture coordinates in a mesh data
@m_since_latest

Expects that the mesh contains a @ref Trade::MeshAttribute::TextureCoordinates
with index id. To avoid data loss with packed types, the coordinattes are
converted to @ref VertexFormat::Vector2 if not already. In that case the data
layouting is done by @ref interleavedLayout() with the @p flags parameter
propagated to it, see its documentation for detailed behavior description.
Other attributes, texture coordinate attributes other than @p id, and indices
(if any) are passed through untouched.

See also @ref transformTextureCoordinates2D(Trade::MeshData&&, const Matrix3&, UnsignedInt, InterleaveFlags)
for a potentially more efficient operation instead of always performing a full
copy, you can also do an in-place transformation using
@ref transformTextureCoordinates2DInPlace().
@see @ref transform2D(), @ref transform3D(),
    @ref Trade::MeshData::attributeCount(MeshAttribute) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData transformTextureCoordinates2D(const Trade::MeshData& data, const Matrix3& transformation, UnsignedInt id = 0, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Transform 2D texture coordinates in a mesh data
@m_since_latest

Compared to @ref transformTextureCoordinates2D(const Trade::MeshData&, const Matrix3&, UnsignedInt, InterleaveFlags)
this function can can perform the transformation in-place, transferring the
data ownership to the returned instance, if both vertex and index data is
owned, vertex data is mutable and the coordinates with index @p id are
@ref VertexFormat::Vector2.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData transformTextureCoordinates2D(Trade::MeshData&& data, const Matrix3& transformation, UnsignedInt id = 0, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Transform 2D texture coordinates in a mesh data in-place
@m_since_latest

Expects that the mesh has mutable vertex data and contains a
@ref Trade::MeshAttribute::TextureCoordinates with index @p id and that the
attribute does not have an implementation-specific format. To avoid data loss
with packed types, the in-place operation requires the coordinate type to be
@ref VertexFormat::Vector2 --- if you can't guarantee that, use
@ref transformTextureCoordinates2D() instead. Other attributes, texture
coordinate attributes other than @p id, and indices (if any) are passed through
untouched.
@see @ref transform2DInPlace(), @ref transform3DInPlace(),
    @ref Trade::MeshData::vertexDataFlags(),
    @ref Trade::MeshData::attributeCount(MeshAttribute) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt) const
*/
MAGNUM_MESHTOOLS_EXPORT void transformTextureCoordinates2DInPlace(Trade::MeshData& data, const Matrix3& transformation, UnsignedInt id = 0);

}}

#endif
