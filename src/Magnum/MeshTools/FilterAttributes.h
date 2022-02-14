#ifndef Magnum_MeshTools_FilterAttributes_h
#define Magnum_MeshTools_FilterAttributes_h
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
 * @brief Function @ref Magnum::MeshTools::filterOnlyAttributes(), @ref Magnum::MeshTools::filterExceptAttributes()
 * @m_since_latest
 */

#include <initializer_list>

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Filter a mesh to contain only the selected subset of named attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p data with
only the attributes that are listed in @p attributes. The index buffer, if
present, is left untouched. Attributes from the list that are not present in
@p data are skipped. All duplicates of a listed attribute are kept --- if you
want a different behavior, use the @ref filterOnlyAttributes(const Trade::MeshData&, Containers::ArrayView<const UnsignedInt>)
overload and pick attributes by their IDs instead.

This function only operates on the attribute metadata --- if you'd like to have
the vertex data repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
@see @ref reference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttribute> attributes);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, std::initializer_list<Trade::MeshAttribute> attributes);

/**
@brief Filter a mesh to contain only the selected subset of attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p data with
only the attribute IDs listed in @p attributes. IDs specified more than once
don't result in given attribute being added multiple times. The index buffer,
if present, is left untouched. All attribute IDs are expected to be smaller
than @ref Trade::MeshData::attributeCount() const.

This function only operates on the attribute metadata --- if you'd like to have
the vertex data repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
@see @ref reference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, Containers::ArrayView<const UnsignedInt> attributes);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, std::initializer_list<UnsignedInt> attributes);

/**
@brief Filter a mesh to contain everything except the selected subset of named attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p data with
only the attributes that are not listed in @p attributes. The index buffer, if
present, is left untouched. Attributes from the list that are not present in
@p data are skipped. All duplicates of a listed attribute are removed --- if
you want a different behavior, use the @ref filterExceptAttributes(const Trade::MeshData&, Containers::ArrayView<const UnsignedInt>)
overload and pick attributes by their IDs instead. If @p attributes is empty,
the behavior is equivalent to @ref reference().

This function only operates on the attribute metadata --- if you'd like to have
the vertex data repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttribute> attributes);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, std::initializer_list<Trade::MeshAttribute> attributes);

/**
@brief Filter a mesh to contain everything except the selected subset of attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p data with
only the attribute IDs that are not listed in @p attributes. IDs specified
multiple times behave like if specified just once. The index buffer, if
present, is left untouched. All attribute IDs are expected to be smaller than
@ref Trade::MeshData::attributeCount() const. If @p attributes is empty, the
behavior is equivalent to @ref reference().

This function only operates on the attribute metadata --- if you'd like to have
the vertex data repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, Containers::ArrayView<const UnsignedInt> attributes);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, std::initializer_list<UnsignedInt> attributes);

}}

#endif
