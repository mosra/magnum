#ifndef Magnum_SceneTools_Filter_h
#define Magnum_SceneTools_Filter_h
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
 * @brief Function @ref Magnum::SceneTools::filterFields(), @ref Magnum::SceneTools::filterOnlyFields(), @ref Magnum::SceneTools::filterExceptFields(), @ref Magnum::SceneTools::filterFieldEntries()
 * @m_since_latest
 */

#include <initializer_list>

#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Filter a scene to contain only the selected subset of fields
@m_since_latest

Returns a non-owning reference to the data from @p scene with only the fields
for which the corresponding bit in @p fieldsToKeep was set. The size of
@p fieldsToKeep is expected to be equal to @ref Trade::SceneData::fieldCount().

This function only operates on the field metadata --- if you'd like to have
the data repacked to contain just the remaining fields as well, pass
the output to @ref combineFields(const Trade::SceneData&).
@see @ref reference(), @ref filterOnlyFields(), @ref filterExceptFields()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFields(const Trade::SceneData& scene, Containers::BitArrayView fieldsToKeep);

/**
@brief Filter a scene to contain only the selected subset of fields
@m_since_latest

Compared to @ref filterFields(const Trade::SceneData&, Containers::BitArrayView),
if the @p scene data is owned, this function transfers the data ownership to
the returned instance instead of returning a non-owning reference. If the data
is not owned, the two overloads behave the same.
@see @ref Trade::SceneData::dataFlags()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFields(Trade::SceneData&& scene, Containers::BitArrayView fieldsToKeep);

/**
@brief Filter a scene to contain only the selected subset of named fields
@m_since_latest

Returns a non-owning reference to the data from @p scene with only the fields
that are listed in @p fields. Fields from the list that are not present in
@p scene are skipped, duplicates in the list are treated the same as if given
field was listed just once.

This function only operates on the field metadata --- if you'd like to have
the data repacked to contain just the remaining fields as well, pass
the output to @ref combineFields(const Trade::SceneData&).
@see @ref reference(), @ref filterFields(), @ref filterExceptFields()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, Containers::ArrayView<const Trade::SceneField> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields);

/**
@brief Filter a scene to contain only the selected subset of named fields
@m_since_latest

Compared to @ref filterOnlyFields(const Trade::SceneData&, Containers::ArrayView<const Trade::SceneField>),
if the @p scene data is owned, this function transfers the data ownership to
the returned instance instead of returning a non-owning reference. If the data
is not owned, the two overloads behave the same.
@see @ref Trade::SceneData::dataFlags()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterOnlyFields(Trade::SceneData&& scene, Containers::ArrayView<const Trade::SceneField> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterOnlyFields(Trade::SceneData&& scene, std::initializer_list<Trade::SceneField> fields);

/**
@brief Filter a scene to contain everything except the selected subset of named fields
@m_since_latest

Returns a non-owning reference to the data from @p scene with only the fields
that are not listed in @p fields. Fields from the list that are not present in
@p scene are skipped, duplicates in the list are treated the same as if given
fields was listed just once.

This function only operates on the field metadata --- if you'd like to have
the data repacked to contain just the remaining fields as well, pass
the output to @ref combineFields(const Trade::SceneData&).
@see @ref reference(), @ref filterFields(), @ref filterOnlyFields()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterExceptFields(const Trade::SceneData& scene, Containers::ArrayView<const Trade::SceneField> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterExceptFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields);

/**
@brief Filter a scene to contain everything except the selected subset of named fields
@m_since_latest

Compared to @ref filterExceptFields(const Trade::SceneData&, Containers::ArrayView<const Trade::SceneField>),
if the @p scene data is owned, this function transfers the data ownership to
the returned instance instead of returning a non-owning reference. If the data
is not owned, the two overloads behave the same.
@see @ref Trade::SceneData::dataFlags()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterExceptFields(Trade::SceneData&& scene, Containers::ArrayView<const Trade::SceneField> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterExceptFields(Trade::SceneData&& scene, std::initializer_list<Trade::SceneField> fields);

/**
@brief Filter individual entries of fields in a scene
@m_since_latest

Returns a copy of @p scene containing the same fields but only with entries for
which the corresponding bit in @p entriesToKeep is set. Each item in
@p entriesToKeep is a pair of a field ID and a mask of entries to keep in that
field. The field ID is expected to be unique in the list and less than
@ref Trade::SceneData::fieldCount(), size of the mask then equal to
@ref Trade::SceneData::fieldSize() for that field. Fields not listed in the
@p entriesToKeep array are passed through unchanged, use @ref filterFields(),
@ref filterExceptFields() or @ref filterOnlyFields() to deal with them as a
whole if needed.

Fields that fully share their mapping views (such as @ref Trade::SceneField::Mesh
and @relativeref{Trade::SceneField,MeshMaterial}, including fields for which
this isn't enforced) either need to be listed all in @p entriesToKeep with the
same mask view, or all omitted so they're passed through. Fields that share the
mapping only partially don't have any special handling. The data repacking is
performed using @ref combineFields(), see its documentation for more
information.

Field flags are preserved except for @ref Trade::SceneFieldFlag::ImplicitMapping
--- if a field with this flag is present in the list, the field is downgraded
to @ref Trade::SceneFieldFlag::OrderedMapping, as removing field entries makes
the mapping to no longer be an implicit sequence (but the order is still
preserved). For simplicity this downgrade happens always, even if all bits for
given field may be set. Fields with
@ref Trade::SceneFieldFlag::ImplicitMapping that aren't present in the list
have the flag preserved.

As an example, let's assume in the following snippet the scene contains
@ref Trade::SceneField::Translation,
@relativeref{Trade::SceneField,Rotation}, @relativeref{Trade::SceneField,Mesh},
@relativeref{Trade::SceneField,MeshMaterial} and
@relativeref{Trade::SceneField,Light}, with the intent to filter some
translations and lights away. Filtering translations means the rotations have
to be filtered as well, however neither meshes nor materials (which share the
mapping as well) need to be listed if they're not filtered:

@snippet SceneTools.cpp filterFieldEntries-shared-mapping

At the moment, @ref Trade::SceneFieldType::Bit and string fields can't be
filtered, only passed through.
@experimental
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, Containers::ArrayView<const Containers::Pair<UnsignedInt, Containers::BitArrayView>> entriesToKeep);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, std::initializer_list<Containers::Pair<UnsignedInt, Containers::BitArrayView>> entriesToKeep);

/**
@brief Filter individual entries of named fields in a scene
@m_since_latest

Translates field names in @p entriesToKeep to field IDs using
@ref Trade::SceneData::fieldId() and delegates to
@ref filterFieldEntries(const Trade::SceneData&, Containers::ArrayView<const Containers::Pair<UnsignedInt, Containers::BitArrayView>>).
Expects that all listed fields exist in @p scene, see the referenced function
documentation for other expectations.
@experimental
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, Containers::ArrayView<const Containers::Pair<Trade::SceneField, Containers::BitArrayView>> entriesToKeep);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, std::initializer_list<Containers::Pair<Trade::SceneField, Containers::BitArrayView>> entriesToKeep);

/**
@brief Filter objects in a scene
@m_since_latest

Returns a copy of @p scene containing the same fields but only with entries
mapped to objects for which the corresponding bit in @p objectsToKeep is set.
The size of @p objectsToKeep is expected to be equal to
@ref Trade::SceneData::mappingBound().

Fields that don't contain any entries mapped to filtered-out objects are passed
through unchanged. The data filtering is performed using
@ref filterFieldEntries() which then delegates to @ref combineFields() for
repacking the data, see their documentation for more information.

Note that this function performs only filtering of the data, it doesn't change
the data in any other way. If there are references to the removed objects from
other fields such as @ref Trade::SceneField::Parent, it's the responsibility of
the caller to deal with them either before or after calling this API, otherwise
the returned data may end up being unusable.
@experimental
@see @ref childrenDepthFirst()
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterObjects(const Trade::SceneData& scene, Containers::BitArrayView objectsToKeep);

}}

#endif
