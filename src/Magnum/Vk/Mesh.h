#ifndef Magnum_Vk_Mesh_h
#define Magnum_Vk_Mesh_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Vk::Mesh
 */

#include <vector>

#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT Mesh {
    public:

        /** @brief Copying is not allowed */
        Mesh(const Mesh&) = delete;

        /** @brief Move constructor */
        Mesh(Mesh&& other);

        Mesh(): _count(0), _drawIndexed(false) {};

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyBuffer}
         */
        ~Mesh();

        /** @brief Copying is not allowed */
        Mesh& operator=(const Mesh&) = delete;

        /** @brief Move assignment is not allowed */
        Mesh& operator=(Mesh&&) = delete;

        auto draw() {
            const VkBuffer* vertexBuffers = _vertexBuffers.data();
            const VkBuffer indexBuffer = _indexBuffer;

            const UnsignedInt numVertexBuffers = _vertexBuffers.size();
            const UnsignedInt count = _count;
            const bool drawIndexed = _drawIndexed;
            return [vertexBuffers, indexBuffer, numVertexBuffers, count, drawIndexed](VkCommandBuffer cmdBuffer){
                VkDeviceSize offset = 0;

                vkCmdBindVertexBuffers(cmdBuffer, 0, numVertexBuffers, vertexBuffers, &offset);

                if(drawIndexed) {
                    vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

                    vkCmdDrawIndexed(cmdBuffer, count, 1, 0, 0, 1);
                } else {
                    vkCmdDraw(cmdBuffer, count, 1, 0, 0);
                }
            };
        }

        Mesh& addVertexBuffer(VkBuffer buffer) {
            _vertexBuffers.push_back(buffer);
            return *this;
        }

        Mesh& setIndexBuffer(const Buffer& buffer) {
            _indexBuffer = buffer;
            _count = buffer.size();
            _drawIndexed = true;
            return *this;
        }

        Mesh& setCount(UnsignedInt count) {
            _count = count;
            return *this;
        }

        Mesh& setDrawIndexed(bool indexed) {
            _drawIndexed = indexed;
            return *this;
        }

    private:
        std::vector<VkBuffer> _vertexBuffers;
        VkBuffer _indexBuffer;

        bool _drawIndexed;

        UnsignedInt _count;
};

}}

#endif
