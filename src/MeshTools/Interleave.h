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
 * @brief Class Magnum::MeshTools::Interleave, function Magnum::MeshTools::interleave()
 */

#include <cassert>
#include <cstring>
#include <vector>
#include <limits>

#include "Mesh.h"
#include "Buffer.h"

namespace Magnum { namespace MeshTools {

/**
@brief Vertex attribute interleaver implementation

See interleave() for full documentation.
*/
class Interleave {
    public:
        /**
         * @brief Interleaved attribute array
         *
         * Size of the data buffer can be computed as follows:
         * @code
         * Result result;
         * size_t size = result.attributeCount*result.stride;
         * @endcode
         */
        struct Result {
            size_t attributeCount;  /**< @brief Attribute count */
            size_t stride;          /**< @brief Distance between two attributes in resulting array */
            char* data;             /**< @brief Data buffer */
        };

        /** @brief Constructor */
        inline Interleave(): result{0, 0, 0} {}

        /**
         * @brief Functor
         *
         * See interleave(const std::vector<T>&...) for full documentation.
         */
        template<class ...T> Result operator()(const std::vector<T>&... attributes) {
            /* Compute buffer size and stride */
            result.attributeCount = attributeCount(attributes...);
            if(result.attributeCount) {
                result.stride = stride(attributes...);

                /* Create output buffer */
                result.data = new char[result.attributeCount*result.stride];

                /* Save the data */
                write(result.data, attributes...);
            }

            return result;
        }

        /**
         * @brief Functor
         *
         * See interleave(Mesh*, Buffer*, Buffer::Usage, const std::vector<T>&...) for full documentation.
         */
        template<class ...T> void operator()(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const std::vector<T>&... attributes) {
            if(!mesh->isInterleaved(buffer)) {
                Corrade::Utility::Error() << "MeshTools::Interleave: the buffer is not interleaved, nothing done";
                assert(0);
                return;
            }

            operator()(attributes...);

            mesh->setVertexCount(result.attributeCount);
            buffer->setData(result.attributeCount*result.stride, result.data, usage);

            delete[] result.data;
        }

        /** @brief Minimal count of passed attributes */
        template<class T, class ...U> inline static size_t attributeCount(const std::vector<T>& attribute, const std::vector<U>&... attributes) {
            size_t count = attributeCount(attributes...);
            if(sizeof...(attributes) != 0 && count != attribute.size()) {
                Corrade::Utility::Error() << "MeshTools::Interleave: attribute arrays don't have the same length, nothing done.";
                assert(0);
                return 0;
            }
            return attribute.size();
        }

        /** @brief Distance between two attributes in resulting array */
        template<class T, class ...U> inline static size_t stride(const std::vector<T>& attribute, const std::vector<U>&... attributes) {
            return sizeof(T) + stride(attributes...);
        }

    private:
        template<class T, class ...U> void write(char* startingOffset, const std::vector<T>& attribute, const std::vector<U>&... attributes) const {
            /* Copy the data to the buffer */
            for(size_t i = 0; i != result.attributeCount; ++i)
                memcpy(startingOffset+i*result.stride, reinterpret_cast<const char*>(&attribute[i]), sizeof(T));

            write(startingOffset+sizeof(T), attributes...);
        }

        /* Terminator functions for recursive calls */
        inline static size_t attributeCount() { return 0; }
        inline static size_t stride() { return 0; }
        inline void write(char*) const {}

        Result result;
};

/**
@brief %Interleave vertex attributes
@param attributes   Attribute arrays
@return Interleaved attribute array. Deleting the buffer is user's
    responsibility.

This function takes two or more attribute arrays and interleaves them, so data
for each attribute are in continuous place in memory.

@attention Each vector should have the same size, if not, resulting array has
zero length.

This is convenience function supplementing direct usage of Interleave class,
instead of
@code
MeshTools::Interleave()(attributes...);
@endcode
you can just write
@code
MeshTools::interleave(attributes...);
@endcode
*/
template<class ...T> inline Interleave::Result interleave(const std::vector<T>&... attributes) {
    return Interleave()(attributes...);
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

This is convenience function supplementing direct usage of Interleave class,
instead of
@code
MeshTools::Interleave()(mesh, buffer, usage, attributes...);
@endcode
you can just write
@code
MeshTools::interleave(mesh, buffer, usage, attributes...);
@endcode
*/
template<class ...T> inline void interleave(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const std::vector<T>&... attributes) {
    return Interleave()(mesh, buffer, usage, attributes...);
}

}}

#endif
