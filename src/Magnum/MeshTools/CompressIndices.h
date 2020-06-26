#ifndef Magnum_MeshTools_CompressIndices_h
#define Magnum_MeshTools_CompressIndices_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function @ref Magnum::MeshTools::compressIndices()
 */

#include <utility>
#include <Corrade/Containers/Containers.h>

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <tuple>
#include <Corrade/Utility/Macros.h>
#include <Corrade/Utility/StlForwardVector.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Compress an index array
@param indices  Index array
@param atLeast  Smallest allowed type
@param offset   Offset to subtract from each index
@return Compressed index array and corresponding type
@m_since{2020,06}

This function compresses @p indices to the smallest possible size. For example
when your indices have the maximum vertex index 463, it's wasteful to store
them in array of 32-bit integers, array of 16-bit integers is sufficient. The
@p atLeast parameter allows you to specify the smallest type to use and it
defaults to @ref MeshIndexType::UnsignedShort as 8-bit types are not friendly
to many GPUs (and for example unextended Vulkan or D3D12 don't even support
them). It's also possible to choose a type larger than the input type to
"inflate" an index buffer of a smaller type. Example usage:

@snippet MagnumMeshTools-gl.cpp compressIndices

In case the indices all start from a large offset, the @p offset parameter can
be used to subtract it, allowing them to be compressed even further. For
example, if all indices are in range @f$ [ 75000 ; 96000 ] @f$ (which fits only
into a 32-bit type), subtracting 75000 makes them in range @f$ [ 0; 21000 ] @f$
which fits into 16 bits. Note that you also need to update vertex attribute
offsets accordingly. Example:

@snippet MagnumMeshTools.cpp compressIndices-offset

A negative @p offset value will do an operation inverse to the above. See also
@ref compressIndices(const Trade::MeshData&, MeshIndexType) that can do this
operation directly on a @ref Trade::MeshData instance.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort, Long offset = 0);

/**
@overload
@m_since{2020,06}
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort, Long offset = 0);

/**
@overload
@m_since{2020,06}
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort, Long offset = 0);

/**
@overload
@m_since{2020,06}

Same as @ref compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType, Long)
with @p atLeast set to @ref MeshIndexType::UnsignedShort.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, Long offset);

/**
@overload
@m_since{2020,06}

Same as @ref compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>&, MeshIndexType, Long)
with @p atLeast set to @ref MeshIndexType::UnsignedShort.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, Long offset);

/**
@overload
@m_since{2020,06}

Same as @ref compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>&, MeshIndexType, Long)
with @p atLeast set to @ref MeshIndexType::UnsignedShort.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, Long offset);

/**
@brief Compress a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType, Long)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView2D<const char>& indices, MeshIndexType atLeast = MeshIndexType::UnsignedShort, Long offset = 0);

/**
@overload
@m_since{2020,06}

Same as @ref compressIndices(const Containers::StridedArrayView2D<const char>&, MeshIndexType, Long)
with @p atLeast set to @ref MeshIndexType::UnsignedShort.
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView2D<const char>& indices, Long offset);

/**
@brief Compress mesh data indices
@m_since{2020,06}

Does the same as @ref compressIndices(const Containers::StridedArrayView2D<const char>&, MeshIndexType, Long),
but together with adjusting vertex attribute offsets in the passed
@ref Trade::MeshData instance. This function will unconditionally make a copy
of all vertex data, use @ref compressIndices(Trade::MeshData&&, MeshIndexType)
to avoid that copy.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData compressIndices(const Trade::MeshData& data, MeshIndexType atLeast = MeshIndexType::UnsignedShort);

/**
@brief Compress mesh data indices
@m_since{2020,06}

Compared to @ref compressIndices(const Trade::MeshData&, MeshIndexType) this
function can transfer ownership of @p data vertex buffer (in case it is
owned) to the returned instance instead of making a copy of it. Index and
attribute data are copied always.
@see @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData compressIndices(Trade::MeshData&& data, MeshIndexType atLeast = MeshIndexType::UnsignedShort);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Compress vertex indices
@param indices  Index array
@return Index range, type and compressed index array
@m_deprecated_since{2020,06} Use @ref compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType, Long)
    instead. The index range isn't returned anymore, use @ref Math::minmax(const Containers::StridedArrayView1D<const T>&)
    to get it if needed.

This function takes index array and outputs them compressed to smallest
possible size. For example when your indices have maximum number 463, it's
wasteful to store them in array of 32bit integers, array of 16bit integers is
sufficient.

Example usage:

@snippet MagnumMeshTools-gl.cpp compressIndices-stl
*/
CORRADE_DEPRECATED("use compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType, Long) instead") MAGNUM_MESHTOOLS_EXPORT std::tuple<Containers::Array<char>, MeshIndexType, UnsignedInt, UnsignedInt> compressIndices(const std::vector<UnsignedInt>& indices);

/**
@brief Compress vertex indices as given type
@m_deprecated_since{2020,06} Use @ref compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType, Long)
    instead.

The type can be either @ref Magnum::UnsignedByte "UnsignedByte",
@ref Magnum::UnsignedShort "UnsignedShort" or @ref Magnum::UnsignedInt "UnsignedInt".
Values in the index array are expected to be representable with given type.

Example usage:

@snippet MagnumMeshTools.cpp compressIndicesAs
*/
template<class T> CORRADE_DEPRECATED("use compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, MeshIndexType, Long) instead") MAGNUM_MESHTOOLS_EXPORT Containers::Array<T> compressIndicesAs(const std::vector<UnsignedInt>& indices);

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedByte> compressIndicesAs<UnsignedByte>(const std::vector<UnsignedInt>&);
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedShort> compressIndicesAs<UnsignedShort>(const std::vector<UnsignedInt>&);
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> compressIndicesAs<UnsignedInt>(const std::vector<UnsignedInt>&);
#endif
#endif

}}

#endif
