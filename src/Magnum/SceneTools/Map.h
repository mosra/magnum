#ifndef Magnum_SceneTools_Map_h
#define Magnum_SceneTools_Map_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function @ref Magnum::SceneTools::mapIndexField(), @ref Magnum::SceneTools::mapIndexFieldInPlace()
 * @m_since_latest
 */

#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Map an index field in a scene
@m_since_latest

Maps a field containing data indices, such as @ref Trade::SceneField::Mesh or
@ref Trade::SceneField::Camera, to different indices by iterating through the
field at index @p fieldId and replacing a particular @cpp value @ce with
@cpp mapping[value] @ce. If the field has a signed type (such as is the case
with @ref Trade::SceneField::MeshMaterial), @cpp -1 @ce is treated as an
"unset" value and preserved verbatim.

Expects that @p fieldId is less than @ref Trade::SceneData::fieldCount(), the
field is @ref Trade::SceneFieldType::UnsignedInt,
@relativeref{Trade::SceneFieldType,Int}, @relativeref{Trade::SceneFieldType,UnsignedShort},
@relativeref{Trade::SceneFieldType,Short},
@relativeref{Trade::SceneFieldType,UnsignedByte} or
@relativeref{Trade::SceneFieldType,Byte} and is not an array, and that the
@p mapping array is large enough to cover all field values.

The output field is always a @ref Trade::SceneFieldType::UnsignedInt if the
input type is unsigned and @ref Trade::SceneFieldType::Int if it's signed. See
also @ref mapIndexField(Trade::SceneData&&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&)
for a potentially more efficient operation instead of always performing a full
copy, you can also do an in-place mapping using
@ref mapIndexFieldInPlace(Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&)
which doesn't change the field type but additionally expects that the
@p mapping values don't overflow given type.
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData mapIndexField(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<const UnsignedInt>& mapping);

/**
@brief Map a named index field in a scene
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref mapIndexField(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&).
The @p field is expected to exist in @p scene.
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData mapIndexField(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping);

/**
@brief Map an index field in a scene
@m_since_latest

Compared to @ref mapIndexField(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&)
this function can perform the mapping in-place, transferring the data ownership
to the returned instance if the data is owned and mutable and the field at
index @p fieldId is @ref Trade::SceneFieldType::UnsignedInt or
@ref Trade::SceneFieldType::Int.
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData mapIndexField(Trade::SceneData&& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<const UnsignedInt>& mapping);

/**
@brief Map a named index field in a scene
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref mapIndexField(Trade::SceneData&&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&).
The @p field is expected to exist in @p scene.
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData mapIndexField(Trade::SceneData&& scene, Trade::SceneField field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping);

/**
@brief Map an index field in a scene in-place
@param[in,out]  scene       Scene
@param[in]      fieldId     Field to map
@param[in]      mapping     Index value mapping
@m_since_latest

Maps a field containing data indices, such as @ref Trade::SceneField::Mesh or
@ref Trade::SceneField::Camera, to different indices by iterating through the
field at index @p fieldId and replacing a particular @cpp value @ce with
@cpp mapping[value] @ce. If the field has a signed type (such as is the case
with @ref Trade::SceneField::MeshMaterial), @cpp -1 @ce is treated as an
"unset" value and preserved verbatim.

Expects that the @p scene has mutable data, @p fieldId is less than
@ref Trade::SceneData::fieldCount(), the field is
@ref Trade::SceneFieldType::UnsignedInt,
@relativeref{Trade::SceneFieldType,Int}, @relativeref{Trade::SceneFieldType,UnsignedShort},
@relativeref{Trade::SceneFieldType,Short},
@relativeref{Trade::SceneFieldType,UnsignedByte} or
@relativeref{Trade::SceneFieldType,Byte} and is not an array, the @p mapping
array is large enough to cover all field values and the @p mapping values don't
overflow given type.

If you need to map to a larger index range that doesn't fit into the original
field type, use @ref mapIndexField(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&) instead.
@see @ref Trade::SceneData::dataFlags()
*/
MAGNUM_SCENETOOLS_EXPORT void mapIndexFieldInPlace(Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<const UnsignedInt>& mapping);

/**
@brief Map a named index field in a scene in-place
@param[in,out]  scene       Scene
@param[in]      field       Field to map
@param[in]      mapping     Index value mapping
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref mapIndexFieldInPlace(Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<const UnsignedInt>&).
The @p field is expected to exist in @p scene.
*/
MAGNUM_SCENETOOLS_EXPORT void mapIndexFieldInPlace(Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping);

}}

#endif
