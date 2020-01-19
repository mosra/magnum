#ifndef Magnum_MeshTools_CompressIndices_h
#define Magnum_MeshTools_CompressIndices_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Function @ref Magnum::MeshTools::compressIndices(), @ref Magnum::MeshTools::compressIndicesAs()
 */

#include <utility>
#include <Corrade/Containers/Containers.h>
#include <Corrade/Utility/StlForwardVector.h>

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <tuple>
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Compress an index array
@param indices  Index array
@param atLeast  Smallest allowed type
@return Compressed index array and corresponding type
@m_since_latest

This function compresses @p indices to the smallest possible size. For example
when your indices have the maximum vertex index 463, it's wasteful to store
them in array of 32bit integers, array of 16bit integers is sufficient. The
@p atLeast parameter allows you to specify the smallest type to use and it
defaults to @ref MeshIndexType::UnsignedShort as 8-bit types are not friendly
to many GPUs (and for example unextended Vulkan or D3D12 don't even support
them). It's also possible to choose a type larger than the input type to
"inflate" an index buffer of a smaller type.

Example usage:

@snippet MagnumMeshTools-gl.cpp compressIndices
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort);

/**
@overload
@m_since_latest
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort);

/**
@overload
@m_since_latest
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort);

/**
@brief Compress a type-erased index array
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView2D<const char>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Compress vertex indices
@param indices  Index array
@return Index range, type and compressed index array
@m_deprecated_since_latest Use @ref compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType)
    instead. The index range isn't returned anymore, use @ref Math::minmax(const Containers::StridedArrayView1D<const T>&)
    to get it if needed.

This function takes index array and outputs them compressed to smallest
possible size. For example when your indices have maximum number 463, it's
wasteful to store them in array of 32bit integers, array of 16bit integers is
sufficient.

Example usage:

@snippet MagnumMeshTools-gl.cpp compressIndices-stl

@see @ref compressIndicesAs()
*/
CORRADE_DEPRECATED("use compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType) instead") MAGNUM_MESHTOOLS_EXPORT std::tuple<Containers::Array<char>, MeshIndexType, UnsignedInt, UnsignedInt> compressIndices(const std::vector<UnsignedInt>& indices);
#endif

/**
@brief Compress vertex indices as given type

The type can be either @ref Magnum::UnsignedByte "UnsignedByte",
@ref Magnum::UnsignedShort "UnsignedShort" or @ref Magnum::UnsignedInt "UnsignedInt".
Values in the index array are expected to be representable with given type.

Example usage:

@snippet MagnumMeshTools.cpp compressIndicesAs

@see @ref compressIndices()
*/
template<class T> MAGNUM_MESHTOOLS_EXPORT Containers::Array<T> compressIndicesAs(const std::vector<UnsignedInt>& indices);

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedByte> compressIndicesAs<UnsignedByte>(const std::vector<UnsignedInt>&);
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedShort> compressIndicesAs<UnsignedShort>(const std::vector<UnsignedInt>&);
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> compressIndicesAs<UnsignedInt>(const std::vector<UnsignedInt>&);
#endif

}}

#endif
