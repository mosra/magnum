#ifndef Magnum_SceneTools_Combine_h
#define Magnum_SceneTools_Combine_h
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
 * @brief Function @ref Magnum::SceneTools::combineFields()
 * @m_since_latest
 */

#include <initializer_list>

#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Combine scene fields together
@m_since_latest

Combines fields of varying @ref Trade::SceneMappingType together into a
@ref Trade::SceneData of a single @p mappingType. If any fields fully share
their mapping views (such as @ref Trade::SceneField::Mesh and
@relativeref{Trade::SceneField,MeshMaterial}, including fields for which this
isn't enforced), the sharing gets preserved. Partial sharing or sharing of data
views (as opposed to mapping views) isn't recognized and the data will get
duplicated.

Fields pointing to existing memory are copied to the output, fields with
(sized) @cpp nullptr @ce mapping or data views are treated as placeholders for
copying the data later, with memory left uninitialized. If you however want to
have placeholder mapping data shared among multiple fields you have to allocate
them upfront. Fields with a string @ref Trade::SceneFieldType can't have
placeholder data views or @cpp nullptr @ce string data pointers, as they're
used to calculate the total string data size. Note that offset-only
@ref Trade::SceneFieldData instances are not supported in the @p fields array.

The resulting fields are always tightly packed (not interleaved). Returned data
flags have both @ref Trade::DataFlag::Mutable and @ref Trade::DataFlag::Owned,
so mutable attribute access is guaranteed.
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData combineFields(Trade::SceneMappingType mappingType, UnsignedLong mappingBound, Containers::ArrayView<const Trade::SceneFieldData> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData combineFields(Trade::SceneMappingType mappingType, UnsignedLong mappingBound, std::initializer_list<Trade::SceneFieldData> fields);

/**
@brief Combine scene fields from scratch
@m_since_latest

Calls @ref combineFields(Trade::SceneMappingType, UnsignedLong, Containers::ArrayView<const Trade::SceneFieldData>)
with mapping type, bound and fields coming from @p scene. Useful for
conveniently repacking an existing scene and throwing away data not referenced
by any field.
@see @ref filterFields(), @ref filterOnlyFields(), @ref filterExceptFields(),
    @ref copy(const Trade::SceneData&)
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData combineFields(const Trade::SceneData& scene);

}}

#endif
