#ifndef Magnum_Vk_Mesh_h
#define Magnum_Vk_Mesh_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Vk::Mesh, enum @ref Magnum::Vk::MeshIndexType, function @ref Magnum::Vk::meshIndexType()
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Vk/MeshLayout.h"

namespace Magnum { namespace Vk {

/**
@brief Mesh index type
@m_since_latest

Wraps a @type_vk_keyword{IndexType}.
*/
enum class MeshIndexType: Int {
    /**
     * @ref Magnum::UnsignedByte "UnsignedByte".
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      Discouraged on contemporary GPU architectures, prefer to use 16-bit
     *      indices instead.
     *
     * @requires_vk_feature @ref DeviceFeature::IndexTypeUnsignedByte
     */
    UnsignedByte = VK_INDEX_TYPE_UINT8_EXT,

    /** @ref Magnum::UnsignedShort "UnsignedShort" */
    UnsignedShort = VK_INDEX_TYPE_UINT16,

    /**
     * @ref Magnum::UnsignedInt "UnsignedInt"
     * @see @ref DeviceFeature::FullDrawIndexUnsignedInt
     */
    UnsignedInt = VK_INDEX_TYPE_UINT32
};

/**
@debugoperatorenum{MeshIndexType}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MeshIndexType value);

/**
@brief Convert a generic index type to Vulkan index type
@m_since_latest

@see @ref meshPrimitive(), @ref vertexFormat()
*/
MAGNUM_VK_EXPORT MeshIndexType meshIndexType(Magnum::MeshIndexType type);

/**
@brief Mesh
@m_since_latest

Connects @ref MeshLayout with concrete vertex/index @ref Buffer instances and
manages related information such as vertex or instance count.

@section Vk-Mesh-populating Populating a mesh

Continuing from the @ref Vk-MeshLayout-usage "mesh layout setup", the @ref Mesh
gets concrete buffers bound using @ref addVertexBuffer() and vertex count
specified with @ref setCount():

@snippet MagnumVk.cpp Mesh-populating

For an indexed mesh, the index buffer can be specified via @ref setIndexBuffer().
With an index buffer present, @ref setCount() then describes count of indices,
instead of vertices:

@snippet MagnumVk.cpp Mesh-populating-indexed

@subsection Vk-Mesh-populating-owned Transferring buffer and layout ownership

To simplify resource management, it's possible to have the @ref Buffer
instances owned by the @ref Mesh, as well as the @ref MeshLayout, either using
@ref std::move() or by directly passing a r-value. If a single buffer is used
for multiple bindings (for example as both a vertex and an index buffer),
perform the move last:

@snippet MagnumVk.cpp Mesh-populating-owned

@section Vk-Mesh-drawing Drawing a mesh

Assuming a rasterization pipeline with the same @ref MeshLayout was bound, a
mesh can be then drawn using @ref CommandBuffer::draw(). The function takes
care of binding all buffers and executing an appropriate draw command:

@snippet MagnumVk.cpp Mesh-drawing

@subsection Vk-Mesh-drawing-dynamic Dynamic pipeline state

Both the @ref MeshPrimitive set in @ref MeshLayout constructor and binding
stride set in @ref MeshLayout::addBinding() can be set as dynamic in the
pipeline using @ref DynamicRasterizationState::MeshPrimitive and
@relativeref{DynamicRasterizationState,VertexInputBindingStride}, assuming
@ref DeviceFeature::ExtendedDynamicState is supported and enabled. The
@ref CommandBuffer::draw() function then checks what dynamic state is enabled
in the currently bound pipeline and implicitly sets all dynamic states.

Taking this to the extreme, with these two dynamic states and a dedicated
binding for each attribute you can make the pipeline accept basically any mesh
as long as just the attribute locations and types are the same --- offsets and
strided of particular attributes are then fully dynamic.

@snippet MagnumVk.cpp Mesh-drawing-dynamic

<b></b>

@m_class{m-note m-success}

@par
    The performance aspect of this approach is a whole different topic, of
    course. It isn't expected to be as fast as if the vertex and primitive
    layout was fixed, but it *may* be faster than having to create and bind a
    whole new pipeline several times over when drawing a large set of
    layout-incompatible meshes.
*/
class MAGNUM_VK_EXPORT Mesh {
    public:
        /**
         * @brief Construct with a reference to external @ref MeshLayout
         *
         * Assumes @p layout stays in scope for the whole lifetime of the
         * @ref Mesh instance.
         */
        explicit Mesh(const MeshLayout& layout);

        /** @brief Construct with taking over @ref MeshLayout ownership */
        explicit Mesh(MeshLayout&& layout);

        /** @brief Copying is not allowed */
        Mesh(const Mesh&) = delete;

        /** @brief Move constructor */
        Mesh(Mesh&&) noexcept;

        /**
         * @brief Destructor
         *
         * If any buffers were added using @ref addVertexBuffer(UnsignedInt, Buffer&&, UnsignedLong)
         * or @ref setIndexBuffer(Buffer&&, UnsignedLong, MeshIndexType), their
         * owned instanced are destructed at this point.
         */
        ~Mesh();

        /** @brief Copying is not allowed */
        Mesh& operator=(const Mesh&) = delete;

        /** @brief Move assignment */
        Mesh& operator=(Mesh&&) noexcept;

        /** @brief Vertex/index count */
        UnsignedInt count() const;

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * If the mesh is indexed, the value is treated as index count,
         * otherwise the value is vertex count. If set to @cpp 0 @ce, no draw
         * commands are issued when calling @ref CommandBuffer::draw().
         *
         * @attention To prevent nothing being rendered by accident, this
         *      function has to be always called, even to just set the count to
         *      @cpp 0 @ce.
         *
         * @see @ref isIndexed(), @ref setInstanceCount(),
         *      @ref setVertexOffset(), @ref setIndexOffset()
         */
        Mesh& setCount(UnsignedInt count) {
            _count = count;
            return *this;
        }

        /** @brief Vertex offset */
        UnsignedInt vertexOffset() const { return _vertexOffset; }

        /**
         * @brief Set vertex offset
         * @return Reference to self (for method chaining)
         *
         * For non-indexed meshes specifies the first vertex that will be
         * drawn, for indexed meshes specifies the offset added to each index.
         * Default is @cpp 0 @ce.
         * @see @ref isIndexed(), @ref setIndexOffset(), @ref setCount()
         */
        Mesh& setVertexOffset(UnsignedInt offset) {
            _vertexOffset = offset;
            return *this;
        }

        /** @brief Index offset */
        UnsignedInt indexOffset() const { return _indexOffset; }

        /**
         * @brief Set index offset
         * @return Reference to self (for method chaining)
         *
         * Expects that the mesh is indexed. Specifies the first index that
         * will be drawn. Default is @cpp 0 @ce.
         * @see @ref isIndexed(), @ref setVertexOffset(), @ref setCount()
         */
        Mesh& setIndexOffset(UnsignedInt offset) {
            _indexOffset = offset;
            return *this;
        }

        /** @brief Instance count */
        UnsignedInt instanceCount() const { return _instanceCount; }

        /**
         * @brief Set instance count
         * @return Reference to self (for method chaining)
         *
         * If set to @cpp 0 @ce, no draw commands are issued when calling
         * @ref CommandBuffer::draw(). Default is @cpp 1 @ce.
         * @see @ref setCount(), @ref setInstanceOffset()
         */
        Mesh& setInstanceCount(UnsignedInt count) {
            _instanceCount = count;
            return *this;
        }

        /** @brief Instance offset */
        UnsignedInt instanceOffset() const { return _instanceOffset; }

        /**
         * @brief Set instance offset
         * @return Reference to self (for method chaining)
         *
         * Specifies the first instance that will be drawn. Default is
         * @cpp 0 @ce.
         * @see @ref setInstanceCount()
         */
        Mesh& setInstanceOffset(UnsignedInt offset) {
            _instanceOffset = offset;
            return *this;
        }

        /**
         * @brief Add a vertex buffer
         * @param binding   Binding corresponding to a particular
         *      @ref MeshLayout::addBinding() call
         * @param buffer    A @ref Buffer instance or a raw Vulkan buffer
         *      handle
         * @param offset    Offset into the buffer, in bytes
         * @return Reference to self (for method chaining)
         *
         * @see @ref setCount(), @ref setVertexOffset()
         */
        Mesh& addVertexBuffer(UnsignedInt binding, VkBuffer buffer, UnsignedLong offset);

        /**
         * @brief Add a vertex buffer and take over its ownership
         * @return Reference to self (for method chaining)
         *
         * Compared to @ref addVertexBuffer(UnsignedInt, VkBuffer, UnsignedLong)
         * the @p buffer instance ownership is transferred to the class and
         * thus doesn't have to be managed separately.
         */
        Mesh& addVertexBuffer(UnsignedInt binding, Buffer&& buffer, UnsignedLong offset);

        /**
         * @brief Set an index buffer
         * @param buffer    A @ref Buffer instance or a raw Vulkan buffer
         *      handle
         * @param offset    Offset into the buffer, in bytes
         * @param indexType Index type
         * @return Reference to self (for method chaining)
         *
         * @see @ref setCount(), @ref setIndexOffset()
         */
        Mesh& setIndexBuffer(VkBuffer buffer, UnsignedLong offset, MeshIndexType indexType);
        /** @overload */
        Mesh& setIndexBuffer(VkBuffer buffer, UnsignedLong offset, Magnum::MeshIndexType indexType);

        /**
         * @brief Set an index buffer and take over its ownership
         * @return Reference to self (for method chaining)
         *
         * Compared to @ref setIndexBuffer(VkBuffer, UnsignedLong, MeshIndexType)
         * the @p buffer instance ownership is transferred to the class and
         * thus doesn't have to be managed separately.
         */
        Mesh& setIndexBuffer(Buffer&& buffer, UnsignedLong offset, MeshIndexType indexType);
        /** @overload */
        Mesh& setIndexBuffer(Buffer&& buffer, UnsignedLong offset, Magnum::MeshIndexType indexType);

        /** @brief Layout of this mesh */
        const MeshLayout& layout() const { return _layout; }

        /**
         * @brief Vertex buffers
         *
         * Has the same length as the vertex buffer binding array in
         * @ref layout(), the buffers correspond to binding IDs at the same
         * index.
         */
        Containers::ArrayView<const VkBuffer> vertexBuffers();

        /**
         * @brief Vertex buffer offsets
         *
         * Has the same length as the vertex buffer binding array in
         * @ref layout(), offsets correspond to @ref vertexBuffers() at the
         * same index.
         */
        Containers::ArrayView<const UnsignedLong> vertexBufferOffsets() const;

        /**
         * @brief Vertex buffer strides
         *
         * Has the same length as the vertex buffer binding array in
         * @ref layout(). The strides are the same as strides in the layout
         * at the same index, but here in a form that's usable by
         * @fn_vk{CmdBindVertexBuffers2} if
         * @ref DynamicRasterizationState::VertexInputBindingStride is enabled.
         */
        Containers::ArrayView<const UnsignedLong> vertexBufferStrides() const;

        /**
         * @brief Whether the mesh is indexed
         *
         * The mesh is considered indexed if @ref setIndexBuffer() was called.
         */
        bool isIndexed() const;

        /**
         * @brief Index buffer
         *
         * Expects that the mesh is indexed.
         * @see @ref isIndexed()
         */
        VkBuffer indexBuffer();

        /**
         * @brief Index buffer offset
         *
         * Expects that the mesh is indexed.
         * @see @ref isIndexed()
         */
        UnsignedLong indexBufferOffset() const;

        /**
         * @brief Index type
         *
         * Expects that the mesh is indexed.
         * @see @ref isIndexed()
         */
        MeshIndexType indexType() const;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        #ifndef CORRADE_NO_ASSERT
        /* Used by CommandBuffer::draw() for a sanity assert */
        bool isCountSet() const { return _count != ~UnsignedInt{}; }
        #endif

    private:
        /* This is all here and not in the State struct in order to avoid
           unnecessary allocations for buffer-less meshes -- like with GL, we
           want `draw(Mesh{MeshLayout{MeshPrimitive::Triangle}}.setCount(3))`
           to be performant enough to not need to invent any alternatives. The
           MeshLayout class does a similar thing. */
        UnsignedInt _count = ~UnsignedInt{},
            _vertexOffset = 0,
            _indexOffset = 0,
            _instanceCount = 1,
            _instanceOffset = 0;
        MeshLayout _layout;

        MAGNUM_VK_LOCAL std::size_t addVertexBufferInternal(UnsignedInt binding, VkBuffer buffer, UnsignedLong offset);

        struct State;
        Containers::Pointer<State> _state;
};

}}

#endif
