#ifndef Magnum_SceneTools_Filter_h
#define Magnum_SceneTools_Filter_h
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
 * @brief Function @ref Magnum::SceneTools::filterFields(), @ref Magnum::SceneTools::filterOnlyFields(), @ref Magnum::SceneTools::filterExceptFields()
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
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterFields(const Trade::SceneData& scene, Containers::BitArrayView fieldsToKeep);

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
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, Containers::ArrayView<const Trade::SceneField> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields);

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
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterExceptFields(const Trade::SceneData& scene, Containers::ArrayView<const Trade::SceneField> fields);

/**
@overload
@m_since_latest
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData filterExceptFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields);

}}

#endif
