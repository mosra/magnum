#ifndef Magnum_Trade_MeshData_h
#define Magnum_Trade_MeshData_h
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
 * @brief Class @ref Magnum::Trade::MeshData, @ref Magnum::Trade::MeshIndexData, @ref Magnum::Trade::MeshAttributeData, enum @ref Magnum::Trade::MeshAttribute, function @ref Magnum::Trade::isMeshAttributeCustom(), @ref Magnum::Trade::meshAttributeCustom()
 * @m_since_latest
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Mesh.h"
#include "Magnum/VertexFormat.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Trade {

/**
@brief Mesh attribute name
@m_since_latest

@see @ref MeshData, @ref MeshAttributeData, @ref VertexFormat,
    @ref AbstractImporter::meshAttributeForName(),
    @ref AbstractImporter::meshAttributeName()
*/
/* 16 bits because 8 bits is not enough to cover all potential per-edge,
   per-face, per-instance and per-meshlet attributes */
enum class MeshAttribute: UnsignedShort {
    /* 0 reserved for an invalid value (returned from
       AbstractImporter::meshAttributeForName()) */

    /**
     * Position. Type is usually @ref Magnum::Vector2 "Vector2" for 2D and
     * @ref Magnum::Vector3 "Vector3" for 3D. Corresponds to
     * @ref Shaders::Generic::Position.
     * @see @ref VertexFormat::Vector2, @ref VertexFormat::Vector3,
     *      @ref MeshData::positions2DAsArray(),
     *      @ref MeshData::positions3DAsArray()
     */
    Position = 1,

    /**
     * Normal. Type is usually @ref Magnum::Vector3 "Vector3". Corresponds to
     * @ref Shaders::Generic::Normal.
     * @see @ref VertexFormat::Vector3, @ref MeshData::normalsAsArray()
     */
    Normal,

    /**
     * Texture coordinates. Type is usually @ref Magnum::Vector2 "Vector2" for
     * 2D coordinates. Corresponds to @ref Shaders::Generic::TextureCoordinates.
     * @see @ref VertexFormat::Vector2,
     *      @ref MeshData::textureCoordinates2DAsArray()
     */
    TextureCoordinates,

    /**
     * Vertex color. Type is usually @ref Magnum::Vector3 "Vector3" or
     * @ref Magnum::Vector4 "Vector4" (or @ref Color3 / @ref Color4).
     * Corresponds to @ref Shaders::Generic::Color3 or
     * @ref Shaders::Generic::Color4.
     * @see @ref VertexFormat::Vector3, @ref VertexFormat::Vector4,
     *      @ref MeshData::colorsAsArray()
     */
    Color,

    /**
     * This and all higher values are for importer-specific attributes. Can be
     * of any type. See documentation of a particular importer for details.
     * @see @ref isMeshAttributeCustom(MeshAttribute)
     *      @ref meshAttributeCustom(MeshAttribute),
     *      @ref meshAttributeCustom(UnsignedShort)
     */
    Custom = 32768
};

/**
@debugoperatorenum{MeshAttribute}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MeshAttribute value);

/**
@brief Whether a mesh attribute is custom
@m_since_latest

Returns @cpp true @ce if @p name has a value larger or equal to
@ref MeshAttribute::Custom, @cpp false @ce otherwise.
@see @ref meshAttributeCustom(UnsignedShort),
    @ref meshAttributeCustom(MeshAttribute)
*/
constexpr bool isMeshAttributeCustom(MeshAttribute name) {
    return UnsignedShort(name) >= UnsignedShort(MeshAttribute::Custom);
}

/**
@brief Create a custom mesh attribute
@m_since_latest

Returns a custom mesh attribute with index @p id. The index is expected to be
less than the value of @ref MeshAttribute::Custom. Use
@ref meshAttributeCustom(MeshAttribute) to get the index back.
*/
/* Constexpr so it's usable for creating compile-time MeshAttributeData
   instances */
constexpr MeshAttribute meshAttributeCustom(UnsignedShort id) {
    return CORRADE_CONSTEXPR_ASSERT(id < UnsignedShort(MeshAttribute::Custom),
        "Trade::meshAttributeCustom(): index" << id << "too large"),
        MeshAttribute(UnsignedShort(MeshAttribute::Custom) + id);
}

/**
@brief Get index of a custom mesh attribute
@m_since_latest

Inverse to @ref meshAttributeCustom(UnsignedShort). Expects that the attribute
is custom.
@see @ref isMeshAttributeCustom()
*/
constexpr UnsignedShort meshAttributeCustom(MeshAttribute name) {
    return CORRADE_CONSTEXPR_ASSERT(isMeshAttributeCustom(name),
        "Trade::meshAttributeCustom():" << name << "is not custom"),
        UnsignedShort(name) - UnsignedShort(MeshAttribute::Custom);
}

/**
@brief Mesh index data
@m_since_latest

Convenience type for populating @ref MeshData. Has no accessors, as the data
are then accessible through @ref MeshData APIs.
@see @ref MeshAttributeData
*/
class MAGNUM_TRADE_EXPORT MeshIndexData {
    public:
        /** @brief Construct for a non-indexed mesh */
        explicit MeshIndexData() noexcept: type{} {}

        /**
         * @brief Construct with a runtime-specified index type
         * @param type      Mesh index type
         * @param data      Index data
         *
         * The @p data size is expected to correspond to given @p type (e.g.,
         * for @ref MeshIndexType::UnsignedInt the @p data array size should
         * be divisible by 4). If you know the @p type at compile time, you can
         * use one of the @ref MeshIndexData(Containers::ArrayView<const UnsignedByte>),
         * @ref MeshIndexData(Containers::ArrayView<const UnsignedShort>) or
         * @ref MeshIndexData(Containers::ArrayView<const UnsignedInt>)
         * constructors, which infer the index type automatically.
         */
        explicit MeshIndexData(MeshIndexType type, Containers::ArrayView<const void> data) noexcept;

        /** @brief Construct with unsigned byte indices */
        constexpr explicit MeshIndexData(Containers::ArrayView<const UnsignedByte> data) noexcept: MeshIndexData{MeshIndexType::UnsignedByte, data, nullptr} {}

        /** @brief Construct with unsigned short indices */
        constexpr explicit MeshIndexData(Containers::ArrayView<const UnsignedShort> data) noexcept: MeshIndexData{MeshIndexType::UnsignedShort, data, nullptr} {}

        /** @brief Construct with unsigned int indices */
        constexpr explicit MeshIndexData(Containers::ArrayView<const UnsignedInt> data) noexcept: MeshIndexData{MeshIndexType::UnsignedInt, data, nullptr} {}

    private:
        /* Contains an assert common for all constexpr constructor, nullptr_t
           to disambiguate from the public constructor of the same signature --
           can't delegate into that one, because it checks against
           meshIndexTypeSize() that's not constexpr, and since we come from a
           template, we don't need that check anyway */
        constexpr explicit MeshIndexData(MeshIndexType type, Containers::ArrayView<const void> data, std::nullptr_t);

        /* Not prefixed with _ because we use them like public in MeshData */
        friend MeshData;
        MeshIndexType type;
        /* Void so the constructors can be constexpr */
        Containers::ArrayView<const void> data;
};

/**
@brief Mesh attribute data
@m_since_latest

Convenience type for populating @ref MeshData. Has no accessors, as the data
are then accessible through @ref MeshData APIs.
*/
class MAGNUM_TRADE_EXPORT MeshAttributeData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of the attribute array for @ref MeshData, expected to
         * be replaced with concrete values later.
         */
        explicit MeshAttributeData() noexcept: name{}, format{}, data{} {}

        /**
         * @brief Type-erased constructor
         * @param name      Attribute name
         * @param format    Vertex format
         * @param data      Attribute data
         *
         * Expects that @p data stride is large enough to fit @p type and that
         * @p type corresponds to @p name.
         */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<const char>& data) noexcept;

        /**
         * @brief Constructor
         * @param name      Attribute name
         * @param data      Attribute data
         *
         * Detects @ref VertexFormat based on @p T and calls
         * @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&).
         */
        template<class T> explicit MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView1D<T>& data) noexcept;

        /** @overload */
        template<class T> explicit MeshAttributeData(MeshAttribute name, const Containers::ArrayView<T>& data) noexcept: MeshAttributeData{name, Containers::stridedArrayView(data)} {}

    private:
        /* Not prefixed with _ because we use them like public in MeshData */
        friend MeshData;
        MeshAttribute name;
        /* Here's some room for flags */
        VertexFormat format;
        Containers::StridedArrayView1D<const char> data;
};

/** @relatesalso MeshAttributeData
@brief Create a non-owning array of @ref MeshAttributeData items
@m_since_latest

Useful when you have the attribute definitions statically defined (for example
when the vertex data themselves are already defined at compile time) and don't
want to allocate just to pass those to @ref MeshData.
*/
Containers::Array<MeshAttributeData> MAGNUM_TRADE_EXPORT meshAttributeDataNonOwningArray(Containers::ArrayView<const MeshAttributeData> view);

/**
@brief Mesh data
@m_since_latest

Provides access to mesh vertex and index data, together with additional
information such as primitive type. Populated instances of this class are
returned from @ref AbstractImporter::mesh().

@section Trade-MeshData-usage Basic usage

The simplest usage is through the convenience functions @ref positions2DAsArray(),
@ref positions3DAsArray(), @ref normalsAsArray(), @ref textureCoordinates2DAsArray()
and @ref colorsAsArray(). Each of these takes an index (as there can be
multiple sets of texture coordinates, for example) and you're expected to check
for attribute presence first with either @ref hasAttribute() or
@ref attributeCount(MeshAttribute) const:

@snippet MagnumTrade.cpp MeshData-usage

@section Trade-MeshData-usage-advanced Advanced usage

The @ref positions2DAsArray(), ... functions shown above always return a
newly-allocated @ref Corrade::Containers::Array instance with a clearly defined
type that's large enough to represent most data. While that's fine for many use
cases, sometimes you may want to minimize the import time of a large model or
the imported data may be already in a well-optimized layout and format that you
want to preserve. The @ref MeshData class internally stores a contiguous blob
of data, which you can directly upload, and then use provided metadata to let
the GPU know of the format and layout:

@snippet MagnumTrade.cpp MeshData-usage-advanced

@section Trade-MeshData-usage-mutable Mutable data access

The interfaces implicitly provide @cpp const @ce views on the contained index
and vertex data through the @ref indexData(), @ref vertexData(),
@ref indices() and @ref attribute() accessors. This is done because in general
case the data can also refer to a memory-mapped file or constant memory. In
cases when it's desirable to modify the data in-place, there's the
@ref mutableIndexData(), @ref mutableVertexData(), @ref mutableIndices() and
@ref mutableAttribute() set of functions. To use these, you need to check that
the data are mutable using @ref indexDataFlags() or @ref vertexDataFlags()
first. The following snippet applies a transformation to the mesh data:

@snippet MagnumTrade.cpp MeshData-usage-mutable

@see @ref AbstractImporter::mesh()
*/
class MAGNUM_TRADE_EXPORT MeshData {
    public:
        /**
         * @brief Construct an indexed mesh data
         * @param primitive     Primitive
         * @param indexData     Index data
         * @param indices       Index data description
         * @param vertexData    Vertex data
         * @param attributes    Description of all vertex attribute data
         * @param importerState Importer-specific state
         *
         * The @p indices are expected to point to a sub-range of @p indexData.
         * The @p attributes are expected to reference (sparse) sub-ranges of
         * @p vertexData. If the mesh has no attributes, the @p indices are
         * expected to be valid and non-empty. If you want to create an
         * index-less attribute-less mesh, use
         * @ref MeshData(MeshPrimitive, UnsignedInt, const void*) to specify
         * desired vertex count.
         *
         * The @ref indexDataFlags() / @ref vertexDataFlags() are implicitly
         * set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable. For non-owned data use the
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, const void*)
         * constructor or its variants instead.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, const void* importerState = nullptr);

        /**
         * @brief Construct indexed mesh data with non-owned index and vertex data
         * @param primitive         Primitive
         * @param indexDataFlags    Index data flags
         * @param indexData         View on index data
         * @param indices           Index data description
         * @param vertexDataFlags   Vertex data flags
         * @param vertexData        View on vertex data
         * @param attributes        Description of all vertex attribute data
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, const void*)
         * creates an instance that doesn't own the passed vertex and index
         * data. The @p indexDataFlags / @p vertexDataFlags parameters can
         * contain @ref DataFlag::Mutable to indicate the external data can be
         * modified, and is expected to *not* have @ref DataFlag::Owned set.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, const void* importerState = nullptr);

        /**
         * @brief Construct indexed mesh data with non-owned index data
         * @param primitive         Primitive
         * @param indexDataFlags    Index data flags
         * @param indexData         View on index data
         * @param indices           Index data description
         * @param vertexData        Vertex data
         * @param attributes        Description of all vertex attribute data
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, const void*)
         * creates an instance that doesn't own the passed index data. The
         * @p indexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. The @ref vertexDataFlags() are
         * implicitly set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, const void* importerState = nullptr);

        /**
         * @brief Construct indexed mesh data with non-owned vertex data
         * @param primitive         Primitive
         * @param indexData         Index data
         * @param indices           Index data description
         * @param vertexDataFlags   Vertex data flags
         * @param vertexData        View on vertex data
         * @param attributes        Description of all vertex attribute data
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, const void*)
         * creates an instance that doesn't own the passed vertex data. The
         * @p vertexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. The @ref indexDataFlags() are
         * implicitly set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, const void* importerState = nullptr);

        /**
         * @brief Construct a non-indexed mesh data
         * @param primitive     Primitive
         * @param vertexData    Vertex data
         * @param attributes    Description of all vertex attribute data
         * @param importerState Importer-specific state
         *
         * Same as calling @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, const void*)
         * with default-constructed @p indexData and @p indices arguments.
         *
         * The @ref vertexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For consistency,
         * the @ref indexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate. For non-owned data use the
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, const void*)
         * constructor instead.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned non-indexed mesh data
         * @param primitive         Primitive
         * @param vertexDataFlags   Vertex data flags
         * @param vertexData        View on vertex data
         * @param attributes        Description of all vertex attribute data
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p vertexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. For consistency, the
         * @ref indexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, const void* importerState = nullptr);

        /**
         * @brief Construct an attribute-less indexed mesh data
         * @param primitive     Primitive
         * @param indexData     Index data
         * @param indices       Index data description
         * @param importerState Importer-specific state
         *
         * Same as calling @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, const void*)
         * with default-constructed @p vertexData and @p attributes arguments.
         * The @p indices are expected to be valid and non-empty. If you want
         * to create an index-less attribute-less mesh, use
         * @ref MeshData(MeshPrimitive, UnsignedInt, const void*) to specify
         * desired vertex count.
         *
         * The @ref indexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For consistency,
         * the @ref vertexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate. For non-owned data use the
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, const void*)
         * constructor instead.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct a non-owned attribute-less indexed mesh data
         * @param primitive         Primitive
         * @param indexDataFlags    Index data flags
         * @param indexData         View on index data
         * @param indices           Index data description
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p indexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. For consistency, the
         * @ref vertexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct an index-less attribute-less mesh data
         * @param primitive     Primitive
         * @param vertexCount   Desired count of vertices to draw
         * @param importerState Importer-specific state
         *
         * Useful in case the drawing is fully driven by a shader. For
         * consistency, the @ref indexDataFlags() / @ref vertexDataFlags() are
         * implicitly set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable, even though there isn't any data to own or
         * to mutate.
         */
        explicit MeshData(MeshPrimitive primitive, UnsignedInt vertexCount, const void* importerState = nullptr) noexcept;

        ~MeshData();

        /** @brief Copying is not allowed */
        MeshData(const MeshData&) = delete;

        /** @brief Move constructor */
        MeshData(MeshData&&) noexcept;

        /** @brief Copying is not allowed */
        MeshData& operator=(const MeshData&) = delete;

        /** @brief Move assignment */
        MeshData& operator=(MeshData&&) noexcept;

        /**
         * @brief Index data flags
         *
         * @see @ref releaseIndexData(), @ref mutableIndexData(),
         *      @ref mutableIndices()
         */
        DataFlags indexDataFlags() const { return _indexDataFlags; }

        /**
         * @brief Vertex data flags
         *
         * @see @ref releaseVertexData(), @ref mutableVertexData(),
         *      @ref mutableAttribute()
         */
        DataFlags vertexDataFlags() const { return _vertexDataFlags; }

        /** @brief Primitive */
        MeshPrimitive primitive() const { return _primitive; }

        /**
         * @brief Raw index data
         *
         * Returns @cpp nullptr @ce if the mesh is not indexed.
         * @see @ref isIndexed(), @ref indexCount(), @ref indexType(),
         *      @ref indices(), @ref mutableIndexData(), @ref releaseIndexData()
         */
        Containers::ArrayView<const char> indexData() const & { return _indexData; }

        /** @brief Taking a view to a r-value instance is not allowed */
        Containers::ArrayView<const char> indexData() const && = delete;

        /**
         * @brief Mutable raw index data
         *
         * Like @ref indexData(), but returns a non-const view. Expects that
         * the mesh is mutable.
         * @see @ref indexDataFlags()
         */
        Containers::ArrayView<char> mutableIndexData() &;

        /** @brief Taking a view to a r-value instance is not allowed */
        Containers::ArrayView<char> mutableIndexData() && = delete;

        /**
         * @brief Raw vertex data
         *
         * Contains data for all vertex attributes. Returns @cpp nullptr @ce if
         * the mesh has no attributes.
         * @see @ref attributeCount(), @ref attributeName(),
         *      @ref attributeFormat(), @ref attribute(),
         *      @ref mutableVertexData(), @ref releaseVertexData()
         */
        Containers::ArrayView<const char> vertexData() const & { return _vertexData; }

        /** @brief Taking a view to a r-value instance is not allowed */
        Containers::ArrayView<const char> vertexData() const && = delete;

        /**
         * @brief Mutable raw vertex data
         *
         * Like @ref vertexData(), but returns a non-const view. Expects that
         * the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        Containers::ArrayView<char> mutableVertexData() &;

        /** @brief Taking a view to a r-value instance is not allowed */
        Containers::ArrayView<char> mutableVertexData() && = delete;

        /** @brief Whether the mesh is indexed */
        bool isIndexed() const { return _indexType != MeshIndexType{}; }

        /**
         * @brief Index count
         *
         * Count of elements in the @ref indices() array. Expects that the
         * mesh is indexed; returned value is always non-zero. See also
         * @ref vertexCount() which returns count of elements in every
         * @ref attribute() array, and @ref attributeCount() which returns
         * count of different per-vertex attribute arrays.
         * @see @ref isIndexed(), @ref indexType()
         */
        UnsignedInt indexCount() const;

        /**
         * @brief Index type
         *
         * Expects that the mesh is indexed.
         * @see @ref isIndexed(), @ref attributeFormat()
         */
        MeshIndexType indexType() const;

        /**
         * @brief Mesh indices
         *
         * Expects that the mesh is indexed and that @p T corresponds to
         * @ref indexType(). You can also use the non-templated
         * @ref indicesAsArray() accessor to get indices converted to 32-bit,
         * but note that such operation involves extra allocation and data
         * conversion.
         * @see @ref isIndexed(), @ref attribute(), @ref mutableIndices()
         */
        template<class T> Containers::ArrayView<const T> indices() const;

        /**
         * @brief Mutable mesh indices
         *
         * Like @ref indices() const, but returns a mutable view. Expects that
         * the mesh is mutable.
         * @see @ref indexDataFlags()
         */
        template<class T> Containers::ArrayView<T> mutableIndices();

        /**
         * @brief Mesh vertex count
         *
         * Count of elements in every attribute array returned by
         * @ref attribute() (or, in case of an attribute-less mesh, the
         * desired vertex count). See also @ref indexCount() which returns
         * count of elements in the @ref indices() array, and
         * @ref attributeCount() which returns count of different per-vertex
         * attribute arrays.
         */
        UnsignedInt vertexCount() const { return _vertexCount; }

        /**
         * @brief Attribute array count
         *
         * Count of different per-vertex attribute arrays, or @cpp 0 @ce for an
         * attribute-less mesh. See also @ref indexCount() which returns count
         * of elements in the @ref indices() array and @ref vertexCount() which
         * returns count of elements in every @ref attribute() array.
         * @see @ref attributeCount(MeshAttribute) const
         */
        UnsignedInt attributeCount() const { return _attributes.size(); }

        /**
         * @brief Attribute name
         *
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * @see @ref attributeFormat(), @ref isMeshAttributeCustom(),
         *      @ref AbstractImporter::meshAttributeForName(),
         *      @ref AbstractImporter::meshAttributeName()
         */
        MeshAttribute attributeName(UnsignedInt id) const;

        /**
         * @brief Attribute format
         *
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * You can also use @ref attributeFormat(MeshAttribute, UnsignedInt) const
         * to directly get a type of given named attribute.
         * @see @ref attributeName(), @ref indexType()
         */
        VertexFormat attributeFormat(UnsignedInt id) const;

        /**
         * @brief Attribute offset
         *
         * Byte offset of the first element of given attribute from the
         * beginning of the @ref vertexData() array, or a byte difference
         * between pointers returned from @ref vertexData() and a particular
         * @ref attribute(). The @p id is expected to be smaller than
         * @ref attributeCount() const. You can also use
         * @ref attributeOffset(MeshAttribute, UnsignedInt) const to
         * directly get an offset of given named attribute.
         */
        std::size_t attributeOffset(UnsignedInt id) const;

        /**
         * @brief Attribute stride
         *
         * Stride between consecutive elements of given attribute in the
         * @ref vertexData() array. The @p id is expected to be smaller
         * than @ref attributeCount() const. You can also use
         * @ref attributeStride(MeshAttribute, UnsignedInt) const to
         * directly get a stride of given named attribute.
         */
        UnsignedInt attributeStride(UnsignedInt id) const;

        /**
         * @brief Whether the mesh has given attribute
         *
         * @see @ref attributeCount(MeshAttribute) const
         */
        bool hasAttribute(MeshAttribute name) const {
            return attributeCount(name);
        }

        /**
         * @brief Count of given named attribute
         *
         * Unlike @ref attributeCount() const this returns count for given
         * attribute name --- for example a mesh can have more than one set of
         * texture coordinates.
         * @see @ref hasAttribute()
         */
        UnsignedInt attributeCount(MeshAttribute name) const;

        /**
         * @brief Format of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const.
         * @see @ref attributeFormat(UnsignedInt) const
         */
        VertexFormat attributeFormat(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Offset of a named attribute
         *
         * Byte offset of the first element of given named attribute from the
         * beginning of the @ref vertexData() array. The @p id is expected to
         * be smaller than @ref attributeCount(MeshAttribute) const.
         * @see @ref attributeOffset(UnsignedInt) const
         */
        std::size_t attributeOffset(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Stride of a named attribute
         *
         * Stride between consecutive elements of given named attribute in the
         * @ref vertexData() array. The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const.
         * @see @ref attributeStride(UnsignedInt) const
         */
        UnsignedInt attributeStride(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Data for given attribute array
         *
         * The @p id is expected to be smaller than @ref attributeCount() const
         * and @p T is expected to correspond to
         * @ref attributeFormat(UnsignedInt) const. You can also use the
         * non-templated @ref positions2DAsArray(), @ref positions3DAsArray(),
         * @ref normalsAsArray(), @ref textureCoordinates2DAsArray() and
         * @ref colorsAsArray() accessors to get common attributes converted to
         * usual types, but note that these operations involve extra allocation
         * and data conversion.
         * @see @ref attribute(MeshAttribute, UnsignedInt) const,
         *      @ref mutableAttribute(MeshAttribute, UnsignedInt)
         */
        template<class T> Containers::StridedArrayView1D<const T> attribute(UnsignedInt id) const;

        /**
         * @brief Mutable data for given attribute array
         *
         * Like @ref attribute(UnsignedInt) const, but returns a mutable view.
         * Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        template<class T> Containers::StridedArrayView1D<T> mutableAttribute(UnsignedInt id);

        /**
         * @brief Data for given named attribute array
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const and @p T is expected to
         * correspond to @ref attributeFormat(MeshAttribute, UnsignedInt) const.
         * You can also use the non-templated @ref positions2DAsArray(),
         * @ref positions3DAsArray(), @ref normalsAsArray(),
         * @ref textureCoordinates2DAsArray() and @ref colorsAsArray()
         * accessors to get common attributes converted to usual types, but
         * note that these operations involve extra data conversion and an
         * allocation.
         * @see @ref attribute(UnsignedInt) const,
         *      @ref mutableAttribute(MeshAttribute, UnsignedInt)
         */
        template<class T> Containers::StridedArrayView1D<const T> attribute(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Mutable data for given named attribute array
         *
         * Like @ref attribute(MeshAttribute, UnsignedInt) const, but returns a
         * mutable view. Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        template<class T> Containers::StridedArrayView1D<T> mutableAttribute(MeshAttribute name, UnsignedInt id = 0);

        /**
         * @brief Indices as 32-bit integers
         *
         * Convenience alternative to the templated @ref indices(). Converts
         * the index array from an arbitrary underlying type and returns it in
         * a newly-allocated array.
         * @see @ref indicesInto()
         */
        Containers::Array<UnsignedInt> indicesAsArray() const;

        /**
         * @brief Positions as 32-bit integers into a pre-allocated view
         *
         * Like @ref indicesAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref indexCount()
         */
        void indicesInto(Containers::ArrayView<UnsignedInt> destination) const;

        /**
         * @brief Positions as 2D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Position as the first argument. Converts
         * the position array from an arbitrary underlying type and returns it
         * in a newly-allocated array. If the underlying type is
         * three-component, the last component is dropped.
         * @see @ref positions2DInto()
         */
        Containers::Array<Vector2> positions2DAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Positions as 2D float vectors into a pre-allocated view
         *
         * Like @ref positions2DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void positions2DInto(Containers::StridedArrayView1D<Vector2> destination, UnsignedInt id = 0) const;

        /**
         * @brief Positions as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Position as the first argument. Converts
         * the position array from an arbitrary underlying type and returns it
         * in a newly-allocated array. If the underlying type is two-component,
         * the Z component is set to @cpp 0.0f @ce.
         * @see @ref positions3DInto()
         */
        Containers::Array<Vector3> positions3DAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Positions as 3D float vectors into a pre-allocated view
         *
         * Like @ref positions3DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void positions3DInto(Containers::StridedArrayView1D<Vector3> destination, UnsignedInt id = 0) const;

        /**
         * @brief Normals as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Normal as the first argument. Converts the
         * normal array from an arbitrary underlying type and returns it in a
         * newly-allocated array.
         * @see @ref normalsInto()
         */
        Containers::Array<Vector3> normalsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Normals as 3D float vectors into a pre-allocated view
         *
         * Like @ref normalsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void normalsInto(Containers::StridedArrayView1D<Vector3> destination, UnsignedInt id = 0) const;

        /**
         * @brief Texture coordinates as 2D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::TextureCoordinates as the first argument.
         * Converts the texture coordinate array from an arbitrary underlying
         * type and returns it in a newly-allocated array.
         * @see @ref textureCoordinates2DInto()
         */
        Containers::Array<Vector2> textureCoordinates2DAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Texture coordinates as 2D float vectors into a pre-allocated view
         *
         * Like @ref textureCoordinates2DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void textureCoordinates2DInto(Containers::StridedArrayView1D<Vector2> destination, UnsignedInt id = 0) const;

        /**
         * @brief Colors as RGBA floats
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Color as the first argument. Converts the
         * color array from an arbitrary underlying type and returns it in a
         * newly-allocated array. If the underlying type is three-component,
         * the alpha component is set to @cpp 1.0f @ce.
         * @see @ref colorsInto()
         */
        Containers::Array<Color4> colorsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Colors as RGBA floats into a pre-allocated view
         *
         * Like @ref colorsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void colorsInto(Containers::StridedArrayView1D<Color4> destination, UnsignedInt id = 0) const;

        /**
         * @brief Release index data storage
         *
         * Releases the ownership of the index data array and resets internal
         * index-related state to default. The mesh then behaves like
         * non-indexed. Note that the returned array has a custom no-op deleter
         * when the data are not owned by the mesh, and while the returned
         * array type is mutable, the actual memory might be not.
         * @see @ref indexData(), @ref indexDataFlags()
         */
        Containers::Array<char> releaseIndexData();

        /**
         * @brief Release vertex data storage
         *
         * Releases the ownership of the index data array and resets internal
         * attribute-related state to default. The mesh then behaves like if
         * it has no attributes. Note that the returned array has a custom
         * no-op deleter when the data are not owned by the mesh, and while the
         * returned array type is mutable, the actual memory might be not.
         * @see @ref vertexData(), @ref vertexDataFlags()
         */
        Containers::Array<char> releaseVertexData();

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        /* For custom deleter checks. Not done in the constructors here because
           the restriction is pointless when used outside of plugin
           implementations. */
        friend AbstractImporter;

        UnsignedInt attributeFor(MeshAttribute name, UnsignedInt id) const;

        UnsignedInt _vertexCount;
        MeshIndexType _indexType;
        MeshPrimitive _primitive;
        DataFlags _indexDataFlags, _vertexDataFlags;
        const void* _importerState;
        Containers::Array<char> _indexData, _vertexData;
        Containers::Array<MeshAttributeData> _attributes;
        /* MeshIndexData are "unpacked" in order to avoid excessive padding */
        Containers::ArrayView<const char> _indices;
};

#if !defined(CORRADE_NO_ASSERT) || defined(CORRADE_GRACEFUL_ASSERT)
namespace Implementation {
    /* LCOV_EXCL_START */
    template<class T> constexpr MeshIndexType meshIndexTypeFor() {
        /* C++ why there isn't an obvious way to do such a thing?! */
        static_assert(sizeof(T) == 0, "unsupported index type");
        return {};
    }
    template<> constexpr MeshIndexType meshIndexTypeFor<UnsignedByte>() { return MeshIndexType::UnsignedByte; }
    template<> constexpr MeshIndexType meshIndexTypeFor<UnsignedShort>() { return MeshIndexType::UnsignedShort; }
    template<> constexpr MeshIndexType meshIndexTypeFor<UnsignedInt>() { return MeshIndexType::UnsignedInt; }

    template<class T> constexpr VertexFormat vertexFormatFor() {
        /* C++ why there isn't an obvious way to do such a thing?! */
        static_assert(sizeof(T) == 0, "unsupported attribute type");
        return {};
    }
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(format) \
        template<> constexpr VertexFormat vertexFormatFor<format>() { return VertexFormat::format; }
    _c(Float)
    _c(UnsignedByte)
    _c(Byte)
    _c(UnsignedShort)
    _c(Short)
    _c(UnsignedInt)
    _c(Int)
    _c(Vector2)
    _c(Vector3)
    _c(Vector4)
    #undef _c
    #endif
    template<> constexpr VertexFormat vertexFormatFor<Color3>() { return VertexFormat::Vector3; }
    template<> constexpr VertexFormat vertexFormatFor<Color4>() { return VertexFormat::Vector4; }
    /* LCOV_EXCL_STOP */
}
#endif

constexpr MeshIndexData::MeshIndexData(MeshIndexType type, Containers::ArrayView<const void> data, std::nullptr_t):
    type{type}, data{(CORRADE_CONSTEXPR_ASSERT(!data.empty(), "Trade::MeshIndexData: index array can't be empty, create a non-indexed mesh instead"), data)} {}

template<class T> MeshAttributeData::MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView1D<T>& data) noexcept: MeshAttributeData{name, Implementation::vertexFormatFor<typename std::remove_const<T>::type>(), Containers::arrayCast<const char>(data)} {}

template<class T> Containers::ArrayView<const T> MeshData::indices() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indices(): the mesh is not indexed", {});
    CORRADE_ASSERT(Implementation::meshIndexTypeFor<T>() == _indexType,
        "Trade::MeshData::indices(): improper type requested for" << _indexType, nullptr);
    return Containers::arrayCast<const T>(_indices);
}

template<class T> Containers::ArrayView<T> MeshData::mutableIndices() {
    CORRADE_ASSERT(_indexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableIndices(): index data not mutable", {});
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::mutableIndices(): the mesh is not indexed", {});
    CORRADE_ASSERT(Implementation::meshIndexTypeFor<T>() == _indexType,
        "Trade::MeshData::mutableIndices(): improper type requested for" << _indexType, nullptr);
    return Containers::arrayCast<T>(reinterpret_cast<Containers::ArrayView<char>&>(_indices));
}

template<class T> Containers::StridedArrayView1D<const T> MeshData::attribute(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attribute(): index" << id << "out of range for" << _attributes.size() << "attributes", nullptr);
    CORRADE_ASSERT(Implementation::vertexFormatFor<T>() == _attributes[id].format,
        "Trade::MeshData::attribute(): improper type requested for" << _attributes[id].name << "of format" << _attributes[id].format, nullptr);
    return Containers::arrayCast<const T>(_attributes[id].data);
}

template<class T> Containers::StridedArrayView1D<T> MeshData::mutableAttribute(UnsignedInt id) {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableAttribute(): vertex data not mutable", {});
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << _attributes.size() << "attributes", nullptr);
    CORRADE_ASSERT(Implementation::vertexFormatFor<T>() == _attributes[id].format,
        "Trade::MeshData::mutableAttribute(): improper type requested for" << _attributes[id].name << "of format" << _attributes[id].format, nullptr);
    return Containers::arrayCast<T>(reinterpret_cast<Containers::StridedArrayView1D<char>&>(_attributes[id].data));
}

template<class T> Containers::StridedArrayView1D<const T> MeshData::attribute(MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attribute<T>(attributeId);
}

template<class T> Containers::StridedArrayView1D<T> MeshData::mutableAttribute(MeshAttribute name, UnsignedInt id) {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableAttribute(): vertex data not mutable", {});
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return mutableAttribute<T>(attributeId);
}

}}

#endif
