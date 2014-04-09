#ifndef Magnum_MeshTools_Interleave_h
#define Magnum_MeshTools_Interleave_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Function @ref Magnum::MeshTools::interleave(), @ref Magnum::MeshTools::interleaveInto()
 */

#include <cstring>
#include <vector>
#include <limits>
#include <tuple>

#include "Magnum/Mesh.h"
#include "Magnum/Buffer.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {

/* Attribute count, skipping gaps. If the attributes are just gaps, returns
   ~std::size_t{0}. It must be in the structure to have proper overload
   resolution (the functions would otherwise need to be de-inlined to break
   cyclic dependencies) */
struct AttributeCount {
    template<class T, class ...U> typename std::enable_if<!std::is_convertible<T, std::size_t>::value, std::size_t>::type operator()(const T& first, const U&... next) const {
        /* {} causes ICE in MSVC 2013 */
        CORRADE_ASSERT(sizeof...(next) == 0 || AttributeCount()(next...) == first.size() || AttributeCount()(next...) == ~std::size_t(0), "MeshTools::interleave(): attribute arrays don't have the same length, expected" << first.size() << "but got" << AttributeCount()(next...), 0);

        return first.size();
    }
    template<class T, class... U> std::size_t operator()(std::size_t, const T& first, const U&... next) const {
        return AttributeCount{}(first, next...);
    }
    constexpr std::size_t operator()(std::size_t) const { return ~std::size_t(0); }
    constexpr std::size_t operator()() const { return 0; }
};

/* Stride, taking gaps into account. It must be in the structure, same reason
   as above */
struct Stride {
    template<class T, class ...U> typename std::enable_if<!std::is_convertible<T, std::size_t>::value, std::size_t>::type operator()(const T&, const U&... next) const {
        return sizeof(typename T::value_type) + Stride{}(next...);
    }
    template<class... T> std::size_t operator()(std::size_t gap, const T&... next) const {
        return gap + Stride{}(next...);
    }
    constexpr std::size_t operator()() const { return 0; }
};

/* Copy data to the buffer */
template<class T> typename std::enable_if<!std::is_convertible<T, std::size_t>::value, std::size_t>::type writeOneInterleaved(std::size_t stride, char* startingOffset, const T& attributeList) {
    auto it = attributeList.begin();
    for(std::size_t i = 0; i != attributeList.size(); ++i, ++it)
        std::memcpy(startingOffset + i*stride, reinterpret_cast<const char*>(&*it), sizeof(typename T::value_type));

    return sizeof(typename T::value_type);
}

/* Skip gap */
inline constexpr std::size_t writeOneInterleaved(std::size_t, char*, std::size_t gap) { return gap; }

/* Write interleaved data */
inline void writeInterleaved(std::size_t, char*) {}
template<class T, class ...U> void writeInterleaved(std::size_t stride, char* startingOffset, const T& first, const U&... next) {
    writeInterleaved(stride, startingOffset + writeOneInterleaved(stride, startingOffset, first), next...);
}

}

/**
@brief %Interleave vertex attributes

This function takes list of attribute arrays and returns them interleaved, so
data for each attribute are in continuous place in memory. Returned tuple
contains attribute count, stride and data array. Deleting the data array is up
to the user.

Size of the data buffer can be computed from attribute count and stride, as
shown below. Example usage:
@code
std::vector<Vector4> positions;
std::vector<Vector2> textureCoordinates;
std::size_t attributeCount;
std::size_t stride;
Containers::Array<char> data;
std::tie(attributeCount, stride, data) = MeshTools::interleave(positions, textureCoordinates);
// ...
@endcode

It's often desirable to align data for one vertex on 32bit boundaries. To
achieve that, you can specify gaps between the attributes:
@code
std::vector<Vector4> positions;
std::vector<UnsignedShort> weights;
std::vector<Color3ub> vertexColors;
std::size_t attributeCount;
std::size_t stride;
Containers::Array<char> data;
std::tie(attributeCount, stride, data) = MeshTools::interleave(positions, weights, 2, textureCoordinates, 1);
@endcode
All gap bytes are set zero. This way vertex stride is 24 bytes, without gaps it
would be 21 bytes, causing possible performance loss.

@attention The function expects that all arrays have the same size.

@note The only requirements to attribute array type is that it must have
    typedef `T::value_type`, forward iterator (to be used with range-based
    for) and function `size()` returning count of elements. In most cases it
    will be `std::vector` or `std::array`.

See also @ref interleave(Mesh&, Buffer&, BufferUsage, const T&...),
which writes the interleaved array directly into buffer of given mesh or
@ref interleaveInto() which writes the data into existing buffer instead of
creating new one.
*/
/* enable_if to avoid clash with overloaded function below */
template<class T, class ...U> typename std::enable_if<!std::is_same<T, Mesh>::value, std::tuple<std::size_t, std::size_t, Containers::Array<char>>>::type interleave(const T& first, const U&... next) {
    /* Compute buffer size and stride */
    const std::size_t attributeCount = Implementation::AttributeCount{}(first, next...);
    const std::size_t stride = Implementation::Stride{}(first, next...);

    /* Create output buffer only if we have some attributes */
    if(attributeCount && attributeCount != ~std::size_t(0)) {
        Containers::Array<char> data = Containers::Array<char>::zeroInitialized(attributeCount*stride);
        Implementation::writeInterleaved(stride, data.begin(), first, next...);

        return std::make_tuple(attributeCount, stride, std::move(data));

    /* Otherwise return nullptr */
    } else {
        #if !defined(CORRADE_GCC44_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
        return std::make_tuple(0, stride, nullptr);
        #else
        return std::tuple<std::size_t, std::size_t, Containers::Array<char>>(0, stride, Containers::Array<char>());
        #endif
    }
}

/**
@brief %Interleave vertex attributes into existing buffer

Unlike @ref interleave() this function interleaves the data into existing
buffer and leaves gaps untouched instead of zero-initializing them. This
function can thus be used for interleaving data depending on runtime
parameters.

@attention Similarly to @ref interleave(), this function expects that all
    arrays have the same size. The passed buffer must also be large enough to
    contain the interleaved data.
*/
template<class T, class ...U> std::tuple<std::size_t, std::size_t> interleaveInto(Containers::ArrayReference<char> buffer, const T& first, const U&... next) {
    /* Verify expected buffer size */
    const std::size_t attributeCount = Implementation::AttributeCount{}(first, next...);
    const std::size_t stride = Implementation::Stride{}(first, next...);
    CORRADE_ASSERT(attributeCount*stride <= buffer.size(), "MeshTools::interleaveInto(): the data buffer is too small, expected" << attributeCount*stride << "but got" << buffer.size(), {});

    /* Write data */
    Implementation::writeInterleaved(stride, buffer.begin(), first, next...);

    return std::make_tuple(attributeCount, stride);
}

/**
@brief %Interleave vertex attributes, write them to array buffer and configure the mesh
@param mesh         Output mesh
@param buffer       Output vertex buffer
@param usage        Vertex buffer usage
@param attributes   Attribute arrays and gaps

The same as @ref interleave(const T&, const U&...), but this function writes the
output to given array buffer and updates vertex count in the mesh accordingly,
so you don't have to call @ref Mesh::setVertexCount() on your own.

@attention You still must call @ref Mesh::setPrimitive() and
    @ref Mesh::addVertexBuffer() on the mesh afterwards.

@see @ref compressIndices(), @ref compile()
@todo rework so Mesh & Buffer doesn't need to be included in header
*/
template<class ...T> void interleave(Mesh& mesh, Buffer& buffer, BufferUsage usage, const T&... attributes) {
    Containers::Array<char> data;
    std::size_t attributeCount;
    std::tie(attributeCount, std::ignore, data) = interleave(attributes...);

    mesh.setVertexCount(attributeCount);
    buffer.setData(data, usage);
}

/**
@brief Write vertex attribute to array buffer and configure the mesh

Simplified specialization of the above function for only one attribute array,
equivalent to the following:
@code
buffer.setData(attribute, usage);
mesh.setVertexCount(attribute.size());
@endcode
*/
template<class T> typename std::enable_if<!std::is_convertible<T, std::size_t>::value, void>::type interleave(Mesh& mesh, Buffer& buffer, BufferUsage usage, const T& attribute) {
    mesh.setVertexCount(attribute.size());
    buffer.setData(attribute, usage);
}

}}

#endif
