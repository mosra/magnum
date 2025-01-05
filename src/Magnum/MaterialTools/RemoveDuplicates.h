#ifndef Magnum_MaterialTools_RemoveDuplicates_h
#define Magnum_MaterialTools_RemoveDuplicates_h
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

/**
 * @file
 * @brief Function @ref Magnum::MaterialTools::removeDuplicatesInPlace(), @ref Magnum::MaterialTools::removeDuplicatesInPlaceInto(), @ref Magnum::MaterialTools::removeDuplicates(), @ref Magnum::MaterialTools::removeDuplicatesInto()
 * @m_since_latest
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/MaterialTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MaterialTools {

/**
@brief Remove duplicate materials from a list in-place
@param[in,out] materials    List of materials
@return Index array to map the original material indices to the output indices
    and size of the unique prefix in the cleaned up @p materials array
@m_since_latest

Removes duplicate materials from the input by comparing material types,
attribute names, types and values and layer offsets. Floating-point attribute
values are compared using fuzzy comparison. Importer state and data flags
aren't considered when comparing the materials. Unique materials are shifted to
the front with order preserved, the returned mapping array has the same size as
the @p materials list and maps from the original indices to prefix of the
output. See @ref removeDuplicates() for a variant that doesn't modify the input
list in any way but instead returns a mapping array pointing to original data
locations.

The operation is done in an @f$ \mathcal{O}(n^2 m) @f$ complexity with
@f$ n @f$ being the material list size and @f$ m @f$ the per-material attribute
count --- every material in the list is compared to all unique materials
collected so far. As attributes are sorted in @ref Trade::MaterialData,
material comparison is just a linear operation. The function doesn't allocate
any temporary memory.

The output index array can be passed to @ref SceneTools::mapIndexField() to
update a @ref Trade::SceneField::MeshMaterial field to reference only the
unique materials. For example:

@snippet MaterialTools.cpp removeDuplicatesInPlace

@see @ref removeDuplicatesInPlaceInto()
*/
MAGNUM_MATERIALTOOLS_EXPORT Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::Iterable<Trade::MaterialData>& materials);

/**
@brief Remove duplicate materials from a list in-place and put mapping into given output array
@param[in,out]  materials   List of materials
@param[out]     mapping     Where to put the resulting mapping array
@return Size of the unique prefix in the cleaned up @p materials array
@m_since_latest

Like @ref removeDuplicatesInPlace() but puts the mapping indices into
@p mapping instead of allocating a new array. Expects that @p mapping has the
same size as @p materials.
@see @ref removeDuplicatesInto()
*/
MAGNUM_MATERIALTOOLS_EXPORT std::size_t removeDuplicatesInPlaceInto(const Containers::Iterable<Trade::MaterialData>& materials, const Containers::StridedArrayView1D<UnsignedInt>& mapping);

/**
@brief Remove duplicate materials from a list
@param[in] materials        List of materials
@return Array to map the original material indices to unique materials and size
    of the unique prefix in the cleaned up @p materials array
@m_since_latest

Removes duplicate materials from the input by comparing material types,
attribute names, types and values and layer offsets. Floating-point attribute
values are compared using fuzzy comparison. Importer state and data flags
aren't considered when comparing the materials. The returned mapping array has
the same size as the @p materials list and maps from the original indices to
only unique materials in the input array. See @ref removeDuplicatesInPlace()
for a variant that also shifts the unique materials to the front of the list
and for a practical usage example.

The operation is done in an @f$ \mathcal{O}(n^2 m) @f$ complexity with
@f$ n @f$ being the material list size and @f$ m @f$ the per-material attribute
count --- every material in the list is compared to all unique materials
collected so far, by iterating the filled prefix of the output index list and
considering only index for which the index value is the same as the index. As
attributes are sorted in @ref Trade::MaterialData, material comparison is just
a linear operation. The function doesn't allocate any temporary memory.
@see @ref removeDuplicatesInto()
*/
MAGNUM_MATERIALTOOLS_EXPORT Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicates(const Containers::Iterable<const Trade::MaterialData>& materials);

/**
@brief Remove duplicate materials from a list in-place and put mapping into given output array
@param[in,out]  materials   List of materials
@param[out]     mapping     Where to put the resulting mapping array
@return Size of the unique prefix in the cleaned up @p materials array
@m_since_latest

Like @ref removeDuplicates() but puts the mapping indices into @p mapping
instead of allocating a new array. Expects that @p mapping has the same size as
@p materials.
@see @ref removeDuplicatesInPlaceInto()
*/
MAGNUM_MATERIALTOOLS_EXPORT std::size_t removeDuplicatesInto(const Containers::Iterable<const Trade::MaterialData>& materials, const Containers::StridedArrayView1D<UnsignedInt>& mapping);

}}

#endif
