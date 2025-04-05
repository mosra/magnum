#ifndef Magnum_MeshTools_Interleave_h
#define Magnum_MeshTools_Interleave_h
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
 * @brief Function @ref Magnum::MeshTools::interleave(), @ref Magnum::MeshTools::interleaveInto(), @ref Magnum::MeshTools::isInterleaved(), @ref Magnum::MeshTools::interleavedLayout()
 */

#include <cstring>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/TypeTraits.h>

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/InterleaveFlags.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {

/* Attribute count, skipping gaps. If the attributes are just gaps, returns
   ~std::size_t{0}. It must be in the structure to have proper overload
   resolution (the functions would otherwise need to be de-inlined to break
   cyclic dependencies) */
struct AttributeCount {
    template<class T, class ...U, typename std::enable_if<!std::is_convertible<T, std::size_t>::value, int>::type = 0> std::size_t operator()(const T& first, const U&...
        #ifndef CORRADE_NO_ASSERT
        next
        #endif
    ) const {
        #ifdef CORRADE_TARGET_MSVC
        #pragma warning(push)
        #pragma warning(disable:4127) /* conditional expression is constant (of course) */
        #endif
        CORRADE_ASSERT(sizeof...(next) == 0 || AttributeCount{}(next...) == first.size() || AttributeCount{}(next...) == ~std::size_t(0), "MeshTools::interleave(): attribute arrays don't have the same length, expected" << first.size() << "but got" << AttributeCount{}(next...), 0);
        #ifdef CORRADE_TARGET_MSVC
        #pragma warning(pop)
        #endif

        return first.size();
    }
    template<class T, class... U> std::size_t operator()(std::size_t, const T& first, const U&... next) const {
        return AttributeCount{}(first, next...);
    }
    constexpr std::size_t operator()(std::size_t) const { return ~std::size_t(0); }
    constexpr std::size_t operator()() const { return 0; }
};

/* If anybody has an idea how to do this better and not require C++17 if
   constexpr, please tell me. This is horrendous. */
CORRADE_HAS_TYPE(HasType, typename T::Type);
template<bool hasType> struct TypeSizeImpl;
template<> struct TypeSizeImpl<true> {
    template<class T> constexpr static std::size_t get() { return sizeof(typename T::Type); }
};
template<> struct TypeSizeImpl<false> {
    template<class T> constexpr static std::size_t get() { return sizeof(typename T::value_type); }
};
template<class T> constexpr std::size_t typeSize() {
    return TypeSizeImpl<HasType<T>::value>::template get<T>();
}

/* Stride, taking gaps into account. It must be in the structure, same reason
   as above */
struct Stride {
    template<class T, class ...U, typename std::enable_if<!std::is_convertible<T, std::size_t>::value, int>::type = 0> std::size_t operator()(const T&, const U&... next) const {
        return typeSize<T>() + Stride{}(next...);
    }
    template<class... T> std::size_t operator()(std::size_t gap, const T&... next) const {
        return gap + Stride{}(next...);
    }
    constexpr std::size_t operator()() const { return 0; }
};

/* Copy data to the buffer */
template<class T, typename std::enable_if<!std::is_convertible<T, std::size_t>::value, int>::type = 0> std::size_t writeOneInterleaved(std::size_t stride, char* startingOffset, const T& attributeList) {
    auto it = attributeList.begin();
    for(std::size_t i = 0; i != attributeList.size(); ++i, ++it)
        std::memcpy(startingOffset + i*stride, reinterpret_cast<const char*>(&*it), typeSize<T>());

    return typeSize<T>();
}

/* Skip gap */
constexpr std::size_t writeOneInterleaved(std::size_t, char*, std::size_t gap) { return gap; }

/* Write interleaved data */
inline void writeInterleaved(std::size_t, char*) {}
template<class T, class ...U> void writeInterleaved(std::size_t stride, char* startingOffset, const T& first, const U&... next) {
    writeInterleaved(stride, startingOffset + writeOneInterleaved(stride, startingOffset, first), next...);
}

}

/**
@brief Interleave vertex attributes

This function takes list of attribute arrays and returns them interleaved, so
data for each attribute are in continuous place in memory. Expects that all
attributes have the same element count.

Example usage:

@snippet MeshTools-gl.cpp interleave1

It's often desirable to align data for one vertex on 32bit boundaries. To
achieve that, you can specify gaps between the attributes:

@snippet MeshTools.cpp interleave2

All gap bytes are set zero. This way vertex stride is 24 bytes, without gaps it
would be 21 bytes, causing possible performance loss.

@note The only requirements to the attribute array type is that it must have
    either a typedef @cpp T::Type @ce (in case of Corrade types such as
    @ref Corrade::Containers::ArrayView) or a typedef @cpp T::value_type @ce
    (in case of STL types such as @ref std::vector or @ref std::array), a
    forward iterator (to be used with range-based for) and a @cpp size() @ce
    method returning element count.

@see @ref interleaveInto()
*/
template<class T, class ...U
    #ifndef DOXYGEN_GENERATING_OUTPUT
    /* So it doesn't clash with the MeshData variant */
    , typename std::enable_if<Utility::IsIterable<T>::value, int>::type = 0
    #endif
> Containers::Array<char> interleave(const T& first, const U&... next)
{
    /* Compute buffer size and stride */
    const std::size_t attributeCount = Implementation::AttributeCount{}(first, next...);
    const std::size_t stride = Implementation::Stride{}(first, next...);

    /* Create output buffer only if we have some attributes */
    if(attributeCount && attributeCount != ~std::size_t(0)) {
        Containers::Array<char> data{ValueInit, attributeCount*stride};
        Implementation::writeInterleaved(stride, data.begin(), first, next...);

        return data;

    /* Otherwise return nullptr */
    } else return nullptr;
}

/**
@brief Interleave vertex attributes into existing buffer
@return Filled buffer size

Unlike @ref interleave() this function interleaves the data into existing
buffer and leaves gaps untouched instead of zero-initializing them. This
function can thus be used for interleaving data depending on runtime
parameters. Expects that all arrays have the same size and the passed buffer is
large enough to contain the interleaved data.
*/
template<class T, class ...U> std::size_t interleaveInto(Containers::ArrayView<char> buffer, const T& first, const U&... next) {
    /* Verify expected buffer size */
    const std::size_t attributeCount = Implementation::AttributeCount{}(first, next...);
    const std::size_t stride = Implementation::Stride{}(first, next...);
    CORRADE_ASSERT(attributeCount*stride <= buffer.size(),
        "MeshTools::interleaveInto(): expected a buffer of at least" << attributeCount*stride << "bytes but got" << buffer.size(), {});

    /* Write data */
    Implementation::writeInterleaved(stride, buffer.begin(), first, next...);

    return attributeCount*stride;
}

/**
@brief If the mesh data is interleaved
@m_since{2020,06}

Returns @cpp true @ce if all attributes have the same *positive* stride and the
difference between minimal and maximal offset is not larger than the stride,
@cpp false @ce otherwise. In particular, returns @cpp true @ce also if the mesh
has just one or no attributes.

While interleaved layouts technically may also have zero or negative strides,
this case is currently not implemented and such layouts are treated as
non-interleaved.
@see @ref Trade::MeshData::attributeStride(),
    @ref Trade::MeshData::attributeOffset(), @ref interleavedData()
*/
MAGNUM_MESHTOOLS_EXPORT bool isInterleaved(const Trade::MeshData& mesh);

/**
@brief Type-erased view on interleaved mesh data
@m_since{2020,06}

Returns a 2D view on @ref Trade::MeshData::vertexData() that spans all
interleaved attributes. Expects that the mesh is interleaved.

First dimension of the returned view has size equal to vertex count and stride
equal to original stride, second dimension size is the smallest possible byte
count to cover all interleaved attributes, including any padding between them
but not before or after.
@see @ref isInterleaved()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::StridedArrayView2D<const char> interleavedData(const Trade::MeshData& mesh);

/**
@brief Mutable type-erased view on interleaved mesh data
@m_since{2020,06}

Same as @ref interleavedData(), but returns a mutable view. Expects that the
mesh is interleaved and vertex data is mutable.
@see @ref isInterleaved(), @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::StridedArrayView2D<char> interleavedMutableData(Trade::MeshData& mesh);

/**
@brief Create an interleaved mesh layout
@m_since{2020,06}

Returns a @ref Trade::MeshData instance with its vertex data allocated for
@p vertexCount vertices containing attributes from both @p mesh and @p extra
interleaved together. No data is actually copied, only an interleaved layout is
created. If @p mesh is already interleaved according to @ref isInterleaved()
and @ref InterleaveFlag::PreserveInterleavedAttributes is set in @p flags,
keeps the attributes in the same layout, potentially extending them with
@p extra. The @p extra attributes, if any, are interleaved together with
existing attributes. Returned instance vertex data flags have both
@ref Trade::DataFlag::Mutable and @ref Trade::DataFlag::Owned, so mutable
attribute access is guaranteed.

For greater control over the layout you can also pass an empty
@ref Trade::MeshData instance and fill @p extra with attributes cherry-picked
using @ref Trade::MeshData::attributeData(UnsignedInt) const on an existing
instance. By default the attributes are tightly packed, you can add arbitrary
padding using instances constructed via
@ref Trade::MeshAttributeData::MeshAttributeData(Int). Example:

@snippet MeshTools.cpp interleavedLayout-extra

This function doesn't preserve index data information in any way, making the
output non-indexed. If you want to preserve index data, create a new indexed
instance with attribute and vertex data transferred from the returned instance:

@snippet MeshTools.cpp interleavedLayout-indices

This function will unconditionally allocate a new array to store all
@ref Trade::MeshAttributeData, use @ref interleavedLayout(Trade::MeshData&&, UnsignedInt, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags)
to avoid that allocation.

All attributes in both @p mesh and @p extra are expected to not have an
implementation-specific format, except for @p mesh attributes in case @p mesh
is already interleaved, then the layout is untouched.
@see @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleavedLayout(const Trade::MeshData& mesh, UnsignedInt vertexCount, Containers::ArrayView<const Trade::MeshAttributeData> extra = {}, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleavedLayout(const Trade::MeshData& mesh, UnsignedInt vertexCount, std::initializer_list<Trade::MeshAttributeData> extra, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Create an interleaved mesh layout
@m_since{2020,06}

Compared to @ref interleavedLayout(const Trade::MeshData&, UnsignedInt, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags)
this function can reuse the @ref Trade::MeshAttributeData array from @p mesh
instead of allocating a new one if there are no attributes passed in @p extra,
the attribute array is owned by the mesh and
@ref InterleaveFlag::PreserveInterleavedAttributes is set in @p flags.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleavedLayout(Trade::MeshData&& mesh, UnsignedInt vertexCount, Containers::ArrayView<const Trade::MeshAttributeData> extra = {}, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleavedLayout(Trade::MeshData&& mesh, UnsignedInt vertexCount, std::initializer_list<Trade::MeshAttributeData> extra, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Interleave mesh data
@m_since{2020,06}

Returns a copy of @p mesh with all attributes interleaved. The @p extra
attributes, if any, are interleaved together with existing attributes (or, in
case the attribute view is null, only the corresponding space for given
attribute type is reserved, with memory left uninitialized). See the
@ref interleave(MeshPrimitive, const Trade::MeshIndexData&, Containers::ArrayView<const Trade::MeshAttributeData>)
overload if you only have loose attributes and want to interleave them
together.

The data layouting is done by @ref interleavedLayout() with the @p flags
parameter propagated to it, see its documentation for detailed behavior
description. Note that offset-only @ref Trade::MeshAttributeData instances are
not supported in the @p extra array. Indices (if any) are kept as-is only if
they're tightly packed and not with an implementation-specific type. Otherwise
the behavior depends on presence of @ref InterleaveFlag::PreserveStridedIndices.

Expects that each attribute in @p extra has either the same amount of elements
as @p mesh vertex count or has none. This function will unconditionally make a
copy of all data even if @p mesh is already interleaved and needs no change,
use @ref interleave(Trade::MeshData&&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags)
to avoid that copy.

All attributes in both @p mesh and @p extra are expected to not have an
implementation-specific format, except for @p mesh attributes in case @p data
is already interleaved, then the layout is untouched.
@see @ref isInterleaved(), @ref isMeshIndexTypeImplementationSpecific(),
    @ref isVertexFormatImplementationSpecific(),
    @ref Trade::MeshData::attributeData(),
    @ref meshtools-attributes-insert
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(const Trade::MeshData& mesh, Containers::ArrayView<const Trade::MeshAttributeData> extra = {}, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttributeData> extra, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Interleave mesh data
@m_since{2020,06}

Compared to @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags)
this function can transfer ownership of @p mesh index buffer (in case it is
owned) and vertex buffer (in case it is owned, already interleaved, there's no
@p extra attributes and @ref InterleaveFlag::PreserveInterleavedAttributes is
set in @p flags) to the returned instance instead of making copies of them.
@see @ref isInterleaved(), @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags(),
    @ref Trade::MeshData::attributeData()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(Trade::MeshData&& mesh, Containers::ArrayView<const Trade::MeshAttributeData> extra = {}, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(Trade::MeshData&& mesh, std::initializer_list<Trade::MeshAttributeData> extra, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Create an indexed interleaved mesh
@m_since_latest

Creates a mesh data instance out of given indices and attributes. Usage
example:

@snippet MeshTools.cpp interleave-meshdata

The @ref interleave(MeshPrimitive, Containers::ArrayView<const Trade::MeshAttributeData>)
overload creates a non-indexed mesh. This function is a convenience shorthand
for calling @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags)
with a @ref Trade::MeshData instance created out of @p primitive and
@p indices and vertex count matching @p attributes. If a particular attribute
view is null, only the corresponding space for given attribute type is
reserved, with memory left uninitialized. The attribute can also be a are a
padding value created with @ref Trade::MeshAttributeData::MeshAttributeData(Int),
see documentation of @ref interleavedLayout() for an example snippet.

Expects that @p attributes all have the same amount of elements or have none,
there's at least one non-padding attribute, none of them have an
implementation-specific format and none of them are offset-only
@ref Trade::MeshAttributeData instances. The @p indices, if present, are
assumed to not have an implementation-specific type. Returned instance vertex
and index data flags have both @ref Trade::DataFlag::Mutable and
@ref Trade::DataFlag::Owned, so mutable attribute access is guaranteed.
@see @ref meshtools-create,
    @ref Trade-MeshData-populating "Populating a MeshData instance directly"
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(MeshPrimitive primitive, const Trade::MeshIndexData& indices, Containers::ArrayView<const Trade::MeshAttributeData> attributes);

/**
@overload
@m_since_latest
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(MeshPrimitive primitive, const Trade::MeshIndexData& indices, std::initializer_list<Trade::MeshAttributeData> attributes);

/**
@brief Create a non-indexed interleaved mesh
@m_since_latest

Same as calling @ref interleave(MeshPrimitive, const Trade::MeshIndexData&, Containers::ArrayView<const Trade::MeshAttributeData>)
with a default-constructed @ref Trade::MeshIndexData instance. See its
documentation for more information and a usage example.
*/
/* No InterleaveFlags as there's no index array for PreserveStridedIndices,
   and PreserveInterleavedAttributes makes sense only for an input mesh */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(MeshPrimitive primitive, Containers::ArrayView<const Trade::MeshAttributeData> attributes);

/**
@overload
@m_since_latest
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData interleave(MeshPrimitive primitive, std::initializer_list<Trade::MeshAttributeData> attributes);

namespace Implementation {

/* Used internally by interleavedLayout() and concatenate() */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Trade::MeshAttributeData> interleavedLayout(Trade::MeshData&& mesh, Containers::ArrayView<const Trade::MeshAttributeData> extra, InterleaveFlags flags);

}

}}

#endif
