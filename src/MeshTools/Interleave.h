#ifndef Magnum_MeshTools_Interleave_h
#define Magnum_MeshTools_Interleave_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function Magnum::MeshTools::interleave()
 */

#include <cstring>
#include <vector>
#include <limits>
#include <tuple>

#include "Mesh.h"
#include "Buffer.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {

class Interleave {
    public:
        inline Interleave(): _attributeCount(0), _stride(0), _data(nullptr) {}

        template<class ...T> std::tuple<std::size_t, std::size_t, char*> operator()(const T&... attributes) {
            /* Compute buffer size and stride */
            _attributeCount = attributeCount(attributes...);
            if(_attributeCount && _attributeCount != ~std::size_t(0)) {
                _stride = stride(attributes...);

                /* Create output buffer */
                _data = new char[_attributeCount*_stride];

                /* Save the data */
                write(_data, attributes...);
            }

            return std::make_tuple(_attributeCount, _stride, _data);
        }

        template<class ...T> void operator()(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const T&... attributes) {
            operator()(attributes...);

            mesh->setVertexCount(_attributeCount);
            buffer->setData(_attributeCount*_stride, _data, usage);

            delete[] _data;
        }

        /* Specialization for only one attribute array */
        template<class T> typename std::enable_if<!std::is_convertible<T, std::size_t>::value, void>::type operator()(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const T& attribute) {
            mesh->setVertexCount(attribute.size());
            buffer->setData(attribute, usage);
        }

        template<class T, class ...U> inline static typename std::enable_if<!std::is_convertible<T, std::size_t>::value, std::size_t>::type attributeCount(const T& first, const U&... next) {
            CORRADE_ASSERT(sizeof...(next) == 0 || attributeCount(next...) == first.size() || attributeCount(next...) == ~std::size_t(0), "MeshTools::interleave(): attribute arrays don't have the same length, nothing done.", 0);

            return first.size();
        }

        template<class... T> inline static std::size_t attributeCount(std::size_t, const T&... next) {
            return attributeCount(next...);
        }

        template<class ...T> inline static std::size_t attributeCount(std::size_t) {
            return ~std::size_t(0);
        }

        template<class T, class ...U> inline static typename std::enable_if<!std::is_convertible<T, std::size_t>::value, std::size_t>::type stride(const T&, const U&... next) {
            return sizeof(typename T::value_type) + stride(next...);
        }

        template<class... T> inline static std::size_t stride(std::size_t gap, const T&... next) {
            return gap + stride(next...);
        }

    private:
        template<class T, class ...U> inline void write(char* startingOffset, const T& first, const U&... next) {
            write(startingOffset+writeOne(startingOffset, first), next...);
        }

        /* Copy data to the buffer */
        template<class T>  typename std::enable_if<!std::is_convertible<T, std::size_t>::value, std::size_t>::type writeOne(char* startingOffset, const T& attributeList) {
            auto it = attributeList.begin();
            for(std::size_t i = 0; i != _attributeCount; ++i, ++it)
                memcpy(startingOffset+i*_stride, reinterpret_cast<const char*>(&*it), sizeof(typename T::value_type));

            return sizeof(typename T::value_type);
        }

        /* Fill gap with zeros */
        std::size_t writeOne(char* startingOffset, std::size_t gap) {
            char* data = new char[gap]();
            for(std::size_t i = 0; i != _attributeCount; ++i)
                memcpy(startingOffset+i*_stride, data, gap);

            delete[] data;
            return gap;
        }

        /* Terminator functions for recursive calls */
        inline static std::size_t attributeCount() { return 0; }
        inline static std::size_t stride() { return 0; }
        inline void write(char*) {}

        std::size_t _attributeCount;
        std::size_t _stride;
        char* _data;
};

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
char* data;
std::tie(attributeCount, stride, data) = MeshTools::interleave(positions, textureCoordinates);
std::size_t dataSize = attributeCount*stride;
// ...
delete[] data;
@endcode

It's often desirable to align data for one vertex on 32bit boundaries. To
achieve that, you can specify gaps between the attributes:
@code
std::vector<Vector4> positions;
std::vector<GLushort> weights;
std::vector<Color3<GLubyte>> vertexColors;
std::size_t attributeCount;
std::size_t stride;
char* data;
std::tie(attributeCount, stride, data) = MeshTools::interleave(positions, weights, 2, textureCoordinates, 1);
@endcode
This way vertex stride is 24 bytes, without gaps it would be 21 bytes, causing
possible performance loss.

@attention The function expects that all arrays have the same size.

@note The only requirements to attribute array type is that it must have
    typedef `T::value_type`, forward iterator (to be used with range-based
    for) and function `size()` returning count of elements. In most cases it
    will be `std::vector` or `std::array`.

See also interleave(Mesh*, Buffer*, Buffer::Usage, const T&...),
which writes the interleaved array directly into buffer of given mesh.
*/
/* enable_if to avoid clash with overloaded function below */
template<class T, class ...U> inline typename std::enable_if<!std::is_convertible<T, Mesh*>::value, std::tuple<std::size_t, std::size_t, char*>>::type interleave(const T& first, const U&... next) {
    return Implementation::Interleave()(first, next...);
}

/**
@brief %Interleave vertex attributes and write them to array buffer
@param mesh         Output mesh
@param buffer       Output vertex buffer
@param usage        Vertex buffer usage
@param attributes   Attribute arrays and gaps

The same as interleave(const T&, const U&...), but this function writes the
output to given array buffer and updates vertex count in the mesh accordingly,
so you don't have to call Mesh::setVertexCount() on your own.

@attention Setting primitive type and binding the attributes to shader is left
    to user - see @ref Mesh-configuration "Mesh documentation".

For only one attribute array this function is convenient equivalent to the
following, without any performance loss:
@code
buffer->setData(attribute, usage);
mesh->setVertexCount(attribute.size());
@endcode

@see MeshTools::compressIndices()
*/
template<class ...T> inline void interleave(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const T&... attributes) {
    return Implementation::Interleave()(mesh, buffer, usage, attributes...);
}

}}

#endif
