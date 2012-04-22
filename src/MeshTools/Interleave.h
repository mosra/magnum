#ifndef Magnum_MeshTools_Interleave_h
#define Magnum_MeshTools_Interleave_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

class Interleave {
    public:
        inline Interleave(): _attributeCount(0), _stride(0), _data(nullptr) {}

        template<class ...T> std::tuple<size_t, size_t, char*> operator()(const std::vector<T>&... attributes) {
            /* Compute buffer size and stride */
            _attributeCount = attributeCount(attributes...);
            if(_attributeCount) {
                _stride = stride(attributes...);

                /* Create output buffer */
                _data = new char[_attributeCount*_stride];

                /* Save the data */
                write(_data, attributes...);
            }

            return std::make_tuple(_attributeCount, _stride, _data);
        }

        template<class ...T> void operator()(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const std::vector<T>&... attributes) {
            CORRADE_ASSERT(mesh->isInterleaved(buffer), "MeshTools::interleave(): the buffer is not interleaved, nothing done", )

            operator()(attributes...);

            mesh->setVertexCount(_attributeCount);
            buffer->setData(_attributeCount*_stride, _data, usage);

            delete[] _data;
        }

        template<class T, class ...U> inline static size_t attributeCount(const std::vector<T>& first, const std::vector<U>&... next) {
            CORRADE_ASSERT(sizeof...(next) == 0 || attributeCount(next...) == first.size(), "MeshTools::interleave(): attribute arrays don't have the same length, nothing done.", 0)

            return first.size();
        }

        template<class T, class ...U> inline static size_t stride(const std::vector<T>& first, const std::vector<U>&... next) {
            return sizeof(T) + stride(next...);
        }

    private:
        template<class T, class ...U> void write(char* startingOffset, const std::vector<T>& first, const std::vector<U>&... next) {
            /* Copy the data to the buffer */
            for(size_t i = 0; i != _attributeCount; ++i)
                memcpy(startingOffset+i*_stride, reinterpret_cast<const char*>(&first[i]), sizeof(T));

            write(startingOffset+sizeof(T), next...);
        }

        /* Terminator functions for recursive calls */
        inline static size_t attributeCount() { return 0; }
        inline static size_t stride() { return 0; }
        inline void write(char*) {}

        size_t _attributeCount;
        size_t _stride;
        char* _data;
};

}
#endif

/**
@brief %Interleave vertex attributes
@param attributes   Attribute arrays
@return Attribute count, stride and interleaved attribute array. Deleting the
    array is user responsibility.

This function takes two or more attribute arrays and returns them interleaved,
so data for each attribute are in continuous place in memory. Size of the data
buffer can be computed from attribute count and stride, as shown below. Example
usage:
@code
size_t attributeCount;
size_t stride;
char* data;
std::tie(attributeCount, stride, data) = MeshTools::interleave(attributes);
size_t dataSize = attributeCount*stride;
// ...
delete[] data;
@endcode

See also interleave(Mesh*, Buffer*, Buffer::Usage, const std::vector<T>&...),
which writes the interleaved array directly into buffer of given mesh.

@attention Each vector should have the same size, if not, resulting array has
    zero length.
*/
template<class ...T> inline std::tuple<size_t, size_t, char*> interleave(const std::vector<T>&... attributes) {
    return Implementation::Interleave()(attributes...);
}

/**
@brief %Interleave vertex attributes and write them to array buffer
@param attributes   Attribute arrays
@param mesh         Output mesh
@param buffer       Output array buffer
@param usage        Array buffer usage

The same as interleave(const std::vector<T>&...), but this function writes
the output to given array buffer and updates vertex count in the mesh
accordingly.

@attention The buffer must be set as interleaved (see Mesh::addBuffer()),
    otherwise this function does nothing. Binding the attributes to shader is
    left to user.
*/
template<class ...T> inline void interleave(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const std::vector<T>&... attributes) {
    return Implementation::Interleave()(mesh, buffer, usage, attributes...);
}

}}

#endif
