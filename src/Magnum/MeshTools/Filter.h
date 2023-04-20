#ifndef Magnum_MeshTools_Filter_h
#define Magnum_MeshTools_Filter_h
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
 * @brief Function @ref Magnum::MeshTools::filterAttributes(), @ref Magnum::MeshTools::filterOnlyAttributes(), @ref Magnum::MeshTools::filterExceptAttributes()
 * @m_since_latest
 */

#include <initializer_list>

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Filter a mesh to contain only the selected subset of attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p mesh with
only the attributes for which the corresponding bit in @p attributesToKeep was
set. The index buffer, if present, is left untouched. The size of
@p attributesToKeep is expected to be equal to
@ref Trade::MeshData::attributeCount().

This function only operates on the attribute metadata --- if you'd like to have
the vertex data repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
@see @ref reference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterAttributes(const Trade::MeshData& mesh, Containers::BitArrayView attributesToKeep);

/**
@brief Filter a mesh to contain only the selected subset of named attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p mesh with
only the attributes that are listed in @p attributes. The index buffer, if
present, is left untouched. Attributes from the list that are not present in
@p mesh are skipped, duplicates in the list are treated the same as if given
attribute was listed just once. If given attribute is present multiple times in
the mesh (such as secondary colors or texture coordinates), all its occurences
are kept --- if you want a different behavior, use the
@ref filterOnlyAttributes(const Trade::MeshData&, Containers::ArrayView<const UnsignedInt>)
overload and pick attributes by their IDs instead.

This function only operates on the attribute metadata --- if you'd like to have
the vertex data repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
@see @ref reference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterOnlyAttributes(const Trade::MeshData& mesh, Containers::ArrayView<const Trade::MeshAttribute> attributes);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterOnlyAttributes(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttribute> attributes);

/**
@brief Filter a mesh to contain everything except the selected subset of named attributes
@m_since_latest

Returns a non-owning reference to the vertex and index buffer from @p mesh with
only the attributes that are not listed in @p attributes. The index buffer, if
present, is left untouched. Attributes from the list that are not present in
@p mesh are skipped, duplicates in the list are treated the same as if given
attribute was listed just once. If given attribute is present multiple times in
the mesh (such as secondary colors or texture coordinates), all its occurences
are removed --- if you want a different behavior, use the
@ref filterOnlyAttributes(const Trade::MeshData&, Containers::ArrayView<const UnsignedInt>)
overload and pick attributes by their IDs instead.

This function only operates on the attribute metadata --- if you'd like to have
the vertex mesh repacked to contain just the remaining attributes as well, pass
the output to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleave()"
without @ref InterleaveFlag::PreserveInterleavedAttributes set.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterExceptAttributes(const Trade::MeshData& mesh, Containers::ArrayView<const Trade::MeshAttribute> attributes);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData filterExceptAttributes(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttribute> attributes);

}}

#endif
