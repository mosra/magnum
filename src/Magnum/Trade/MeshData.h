#ifndef Magnum_Trade_MeshData_h
#define Magnum_Trade_MeshData_h
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
 * @brief Class @ref Magnum::Trade::MeshData, @ref Magnum::Trade::MeshIndexData, @ref Magnum::Trade::MeshAttributeData, enum @ref Magnum::Trade::MeshAttribute, function @ref Magnum::Trade::isMeshAttributeCustom(), @ref Magnum::Trade::meshAttributeCustom()
 * @m_since{2020,06}
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
@m_since{2020,06}

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
     * Position. Type is usually @ref VertexFormat::Vector2 for 2D and
     * @ref VertexFormat::Vector3 for 3D, but can be also any of
     * @ref VertexFormat::Vector2h, @ref VertexFormat::Vector3h,
     * @ref VertexFormat::Vector2ub, @ref VertexFormat::Vector2ubNormalized,
     * @ref VertexFormat::Vector2b, @ref VertexFormat::Vector2bNormalized,
     * @ref VertexFormat::Vector2us, @ref VertexFormat::Vector2usNormalized,
     * @ref VertexFormat::Vector2s, @ref VertexFormat::Vector2sNormalized,
     * @ref VertexFormat::Vector3ub, @ref VertexFormat::Vector3ubNormalized,
     * @ref VertexFormat::Vector3b, @ref VertexFormat::Vector3bNormalized,
     * @ref VertexFormat::Vector3us, @ref VertexFormat::Vector3usNormalized,
     * @ref VertexFormat::Vector3s or @ref VertexFormat::Vector3sNormalized.
     * Corresponds to @ref Shaders::Generic::Position.
     * @see @ref MeshData::positions2DAsArray(),
     *      @ref MeshData::positions3DAsArray()
     */
    Position = 1,

    /**
     * Tangent, optionally including bitangent sign. In the first case the type
     * is usually @ref VertexFormat::Vector3, but can be also
     * @ref VertexFormat::Vector3h, @ref VertexFormat::Vector3bNormalized or
     * @ref VertexFormat::Vector3sNormalized; in the second case the type is
     * @ref VertexFormat::Vector4 (or @ref VertexFormat::Vector4h,
     * @ref VertexFormat::Vector4bNormalized,
     * @ref VertexFormat::Vector4sNormalized) and the fourth component is a
     * sign value (@cpp -1.0f @ce or @cpp +1.0f @ce) defining handedness of the
     * tangent basis. Reconstruct the @ref MeshAttribute::Bitangent can be then
     * done like this:
     *
     * @snippet MagnumTrade.cpp MeshAttribute-bitangent-from-tangent
     *
     * Corresponds to @ref Shaders::Generic::Tangent or
     * @ref Shaders::Generic::Tangent4.
     * @see @ref MeshData::tangentsAsArray(),
     *      @ref MeshData::bitangentSignsAsArray()
     */
    Tangent,

    /**
     * Bitangent. Type is usually @ref VertexFormat::Vector3, but can be also
     * @ref VertexFormat::Vector3h, @ref VertexFormat::Vector3bNormalized or
     * @ref VertexFormat::Vector3sNormalized. For better storage efficiency,
     * the bitangent can be also reconstructed from the normal and tangent, see
     * @ref MeshAttribute::Tangent for more information. Corresponds to
     * @ref Shaders::Generic::Bitangent.
     * @see @ref MeshData::bitangentsAsArray()
     */
    Bitangent,

    /**
     * Normal. Type is usually @ref VertexFormat::Vector3, but can be also
     * @ref VertexFormat::Vector3h. @ref VertexFormat::Vector3bNormalized or
     * @ref VertexFormat::Vector3sNormalized. Corresponds to
     * @ref Shaders::Generic::Normal.
     * @see @ref MeshData::normalsAsArray()
     */
    Normal,

    /**
     * Texture coordinates. Type is usually @ref VertexFormat::Vector2 for
     * 2D coordinates, but can be also any of @ref VertexFormat::Vector2h,
     * @ref VertexFormat::Vector2ub, @ref VertexFormat::Vector2ubNormalized,
     * @ref VertexFormat::Vector2b, @ref VertexFormat::Vector2bNormalized,
     * @ref VertexFormat::Vector2us, @ref VertexFormat::Vector2usNormalized,
     * @ref VertexFormat::Vector2s or @ref VertexFormat::Vector2sNormalized.
     * Corresponds to @ref Shaders::Generic::TextureCoordinates.
     * @see @ref MeshData::textureCoordinates2DAsArray()
     */
    TextureCoordinates,

    /**
     * Vertex color. Type is usually @ref VertexFormat::Vector3 or
     * @ref VertexFormat::Vector4, but can be also any of
     * @ref VertexFormat::Vector3h, @ref VertexFormat::Vector4h,
     * @ref VertexFormat::Vector3ubNormalized,
     * @ref VertexFormat::Vector3usNormalized,
     * @ref VertexFormat::Vector4ubNormalized or
     * @ref VertexFormat::Vector4usNormalized. Corresponds to
     * @ref Shaders::Generic::Color3 or @ref Shaders::Generic::Color4.
     * @see @ref MeshData::colorsAsArray()
     */
    Color,

    /**
     * (Instanced) object ID for editor selection or scene annotation. Type is
     * usually @ref VertexFormat::UnsignedInt, but can be also
     * @ref VertexFormat::UnsignedShort or @ref VertexFormat::UnsignedByte.
     * Corresponds to @ref Shaders::Generic::ObjectId.
     * @see @ref MeshData::objectIdsAsArray()
     */
    ObjectId,

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
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MeshAttribute value);

/**
@brief Whether a mesh attribute is custom
@m_since{2020,06}

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
@m_since{2020,06}

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
@m_since{2020,06}

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
@m_since{2020,06}

Convenience type for populating @ref MeshData, see its documentation for an
introduction.

@section Trade-MeshIndexData-usage Usage

The most straightforward usage is constructing the instance from a view on the
index array. The @ref MeshIndexType gets inferred from the view type:

@snippet MagnumTrade.cpp MeshIndexData-usage

Alternatively, you can pass a typeless @cpp const void @ce view and supply
@ref MeshIndexType explicitly, or a contiguous 2D view and let the class detect
the actual index type from second dimension size. Note that the class accepts
only contiguous views and not @ref Corrade::Containers::StridedArrayView,
following limitations of GPU index buffers that also have to be contiguous.
@see @ref MeshAttributeData
*/
class MAGNUM_TRADE_EXPORT MeshIndexData {
    public:
        /** @brief Construct for a non-indexed mesh */
        explicit MeshIndexData(std::nullptr_t = nullptr) noexcept: _type{} {}

        /**
         * @brief Construct with a runtime-specified index type
         * @param type      Index type
         * @param data      Index data
         *
         * The @p data size is expected to correspond to given @p type (e.g.,
         * for @ref MeshIndexType::UnsignedInt the @p data array size should
         * be divisible by 4). If you know the @p type at compile time, you can
         * use one of the @ref MeshIndexData(Containers::ArrayView<const UnsignedByte>),
         * @ref MeshIndexData(Containers::ArrayView<const UnsignedShort>) or
         * @ref MeshIndexData(Containers::ArrayView<const UnsignedInt>)
         * constructors, which infer the index type automatically.
         *
         * If @p data is empty, the mesh will be treated as indexed but with
         * zero indices. To create a non-indexed mesh, use the
         * @ref MeshIndexData(std::nullptr_t) constructor.
         */
        explicit MeshIndexData(MeshIndexType type, Containers::ArrayView<const void> data) noexcept;

        /** @brief Construct with unsigned byte indices */
        constexpr explicit MeshIndexData(Containers::ArrayView<const UnsignedByte> data) noexcept: _type{MeshIndexType::UnsignedByte}, _data{data} {}

        /** @brief Construct with unsigned short indices */
        constexpr explicit MeshIndexData(Containers::ArrayView<const UnsignedShort> data) noexcept: _type{MeshIndexType::UnsignedShort}, _data{data} {}

        /** @brief Construct with unsigned int indices */
        constexpr explicit MeshIndexData(Containers::ArrayView<const UnsignedInt> data) noexcept: _type{MeshIndexType::UnsignedInt}, _data{data} {}

        /**
         * @brief Constructor
         *
         * Expects that @p data is contiguous and size of the second dimension
         * is either 1, 2 or 4, corresponding to one of the @ref MeshIndexType
         * values. As a special case, if second dimension size is 0, the
         * constructor is equivalent to @ref MeshIndexData(std::nullptr_t).
         */
        explicit MeshIndexData(const Containers::StridedArrayView2D<const char>& data) noexcept;

        /** @brief Index type */
        constexpr MeshIndexType type() const { return _type; }

        /** @brief Type-erased index data */
        constexpr Containers::ArrayView<const void> data() const { return _data; }

    private:
        friend MeshData;
        MeshIndexType _type;
        /* Void so the constructors can be constexpr */
        Containers::ArrayView<const void> _data;
};

/**
@brief Mesh attribute data
@m_since{2020,06}

Convenience type for populating @ref MeshData, see its documentation for an
introduction. Additionally usable in various @ref MeshTools algorithms such as
@ref MeshTools::duplicate(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttributeData>)
or @ref MeshTools::interleave(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttributeData>).

@section Trade-MeshAttributeData-usage Usage

The most straightforward usage is constructing an instance from a pair of
@ref MeshAttribute and a strided view. The @ref VertexFormat gets inferred from
the view type:

@snippet MagnumTrade.cpp MeshAttributeData-usage

Alternatively, you can pass a typeless @cpp const void @ce view and supply
@ref VertexFormat explicitly, or a 2D view.

@subsection Trade-MeshAttributeData-usage-offset-only Offset-only attribute data

If the actual attribute data location is not known yet, the instance can be
created as "offset-only", meaning the actual view gets created only later when
passed to a @ref MeshData instance with a concrete vertex data array. This is
useful for example when vertex layout is static (and thus can be defined at
compile time), but the actual data is allocated / populated at runtime:

@snippet MagnumTrade.cpp MeshAttributeData-usage-offset-only

Note that @ref MeshTools algorithms generally don't accept offset-only
@ref MeshAttributeData instances except when passed through a @ref MeshData
instance.

@section Trade-MeshAttributeData-custom-vertex-format Custom vertex formats

Apart from custom @ref MeshAttribute names, shown in
@ref Trade-MeshData-populating-custom, @ref VertexFormat can be extended with
implementation-specific formats as well. Formats that don't have a generic
@ref VertexFormat equivalent can be created using @ref vertexFormatWrap(),
however note that most APIs and @ref MeshTools functions can't work with those
as their size or contents can't be known:

@snippet MagnumTrade.cpp MeshAttributeData-custom-vertex-format

@see @ref MeshIndexData
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
        constexpr explicit MeshAttributeData() noexcept: _format{}, _name{}, _isOffsetOnly{false}, _vertexCount{}, _stride{}, _arraySize{}, _data{} {}

        /**
         * @brief Type-erased constructor
         * @param name      Attribute name
         * @param format    Vertex format
         * @param data      Attribute data
         * @param arraySize Array size. Use @cpp 0 @ce for non-array
         *      attributes.
         *
         * Expects that @p data stride is large enough to fit all @p arraySize
         * items of @p type, @p type corresponds to @p name and @p arraySize is
         * zero for builtin attributes.
         */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<const void>& data, UnsignedShort arraySize = 0) noexcept;

        /**
         * @brief Constructor
         * @param name      Attribute name
         * @param format    Vertex format
         * @param data      Attribute data
         * @param arraySize Array size. Use @cpp 0 @ce for non-array
         *      attributes.
         *
         * Expects that the second dimension of @p data is contiguous and its
         * size matches @p type and @p arraSize, that @p type corresponds to
         * @p name and @p arraySize is zero for builtin attributes.
         */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView2D<const char>& data, UnsignedShort arraySize = 0) noexcept;

        /** @overload */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, std::nullptr_t, UnsignedShort arraySize = 0) noexcept: MeshAttributeData{nullptr, name, format, nullptr, arraySize} {}

        /**
         * @brief Constructor
         * @param name      Attribute name
         * @param data      Attribute data
         *
         * Detects @ref VertexFormat based on @p T and calls
         * @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&, UnsignedShort).
         * For most types known by Magnum, the detected @ref VertexFormat is of
         * the same name as the type (so e.g. @ref Magnum::Vector3ui "Vector3ui"
         * gets recognized as @ref VertexFormat::Vector3ui), with the
         * following exceptions:
         *
         * -    @ref Color3ub is recognized as
         *      @ref VertexFormat::Vector3ubNormalized
         * -    @ref Color3us is recognized as
         *      @ref VertexFormat::Vector3usNormalized
         * -    @ref Color4ub is recognized as
         *      @ref VertexFormat::Vector4ubNormalized
         * -    @ref Color4us is recognized as
         *      @ref VertexFormat::Vector4usNormalized
         * -    @ref Matrix2x2b is recognized as
         *      @ref VertexFormat::Matrix2x2bNormalized (and similarly for
         *      other matrix sizes)
         * -    @ref Matrix2x2s is recognized as
         *      @ref VertexFormat::Matrix2x2sNormalized (and similarly for
         *      other matrix sizes)
         *
         * This also means that if you have a @ref Magnum::Vector2s "Vector2s",
         * for example, and want to pick a
         * @ref VertexFormat::Vector2sNormalized instead of the
         * (autodetected) @ref VertexFormat::Vector2s, you need to specify
         * it explicitly --- there's no way the library can infer this from the
         * type alone, except for the color types above (which are generally
         * always normalized).
         * @todo Pick a type based on the combination of T and name? E.g., for
         *      a Tangent it would pick Vector3sNormalized instead of Vector3s
         */
        template<class T> constexpr explicit MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView1D<T>& data) noexcept;

        /** @overload */
        template<class T> constexpr explicit MeshAttributeData(MeshAttribute name, const Containers::ArrayView<T>& data) noexcept: MeshAttributeData{name, Containers::stridedArrayView(data)} {}

        /**
         * @brief Construct an array attribute
         * @param name      Attribute name
         * @param data      Attribute data
         *
         * Detects @ref VertexFormat based on @p T and calls
         * @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&, UnsignedShort)
         * with the second dimension size passed to @p arraySize. Expects that
         * the second dimension is contiguous. At the moment only custom
         * attributes can be arrays, which means this function can't be used
         * with a builtin @p name. See @ref MeshAttributeData(MeshAttribute, const Containers::StridedArrayView1D<T>&)
         * for details about @ref VertexFormat detection.
         */
        template<class T> constexpr explicit MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView2D<T>& data) noexcept;

        /**
         * @brief Construct an offset-only attribute
         * @param name          Attribute name
         * @param format        Attribute format
         * @param offset        Attribute data offset
         * @param vertexCount   Attribute vertex count
         * @param stride        Attribute stride
         * @param arraySize     Array size. Use @cpp 0 @ce for non-array
         *      attributes.
         *
         * Instances created this way refer to an offset in unspecified
         * external vertex data instead of containing the data view directly.
         * Useful when the location of the vertex data array is not known at
         * attribute construction time. Expects that @p arraySize is zero for
         * builtin attributes. Note that instances created this way can't be
         * used in most @ref MeshTools algorithms.
         *
         * Additionally, for even more flexibility, the @p vertexCount can be
         * overriden at @ref MeshData construction time, however all attributes
         * are still required to have the same vertex count to catch accidents.
         *
         * Note that due to the @cpp constexpr @ce nature of this constructor,
         * no @p format / @p arraySize checks against @p stride can be done.
         * You're encouraged to use the @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&, UnsignedShort)
         * constructor if you want additional safeguards.
         * @see @ref isOffsetOnly(), @ref arraySize(),
         *      @ref data(Containers::ArrayView<const void>) const
         */
        explicit constexpr MeshAttributeData(MeshAttribute name, VertexFormat format, std::size_t offset, UnsignedInt vertexCount, std::ptrdiff_t stride, UnsignedShort arraySize = 0) noexcept;

        /**
         * @brief Construct a pad value
         *
         * Usable in various @ref MeshTools algorithms to insert padding
         * between interleaved attributes. Negative values can be used to alias
         * multiple different attributes onto each other. Not meant to be
         * passed to @ref MeshData.
         * @see @ref stride()
         */
        constexpr explicit MeshAttributeData(Int padding): _format{}, _name{}, _isOffsetOnly{false}, _vertexCount{0}, _stride{
            (CORRADE_CONSTEXPR_ASSERT(padding >= -32768 && padding <= 32767,
                "Trade::MeshAttributeData: at most 32k padding supported, got" << padding), Short(padding))
        }, _arraySize{}, _data{nullptr} {}

        /**
         * @brief If the attribute is offset-only
         *
         * Returns @cpp true @ce if the attribute doesn't contain the data view
         * directly, but instead refers to unspecified external vertex data.
         * @see @ref data(Containers::ArrayView<const void>) const,
         *      @ref MeshAttributeData(MeshAttribute, VertexFormat, std::size_t, UnsignedInt, std::ptrdiff_t, UnsignedShort)
         */
        constexpr bool isOffsetOnly() const { return _isOffsetOnly; }

        /** @brief Attribute name */
        constexpr MeshAttribute name() const { return _name; }

        /** @brief Attribute format */
        constexpr VertexFormat format() const { return _format; }

        /**
         * @brief Attribute offset
         *
         * If the attribute is offset-only, returns the offset directly,
         * otherwise uses the @p vertexData parameter to calculate the offset.
         * @see @ref isOffsetOnly()
         */
        std::size_t offset(Containers::ArrayView<const void> vertexData) const {
            return _isOffsetOnly ? _data.offset : reinterpret_cast<const char*>(_data.pointer) - reinterpret_cast<const char*>(vertexData.data());
        }

        /**
         * @brief Attribute stride
         *
         * Can be negative for pad values, never negative for real attributes.
         * @see @ref MeshAttributeData(Int)
         */
        constexpr Short stride() const { return _stride; }

        /** @brief Attribute array size */
        constexpr UnsignedShort arraySize() const { return _arraySize; }

        /**
         * @brief Type-erased attribute data
         *
         * Expects that the attribute is not offset-only, in that case use the
         * @ref data(Containers::ArrayView<const void>) const overload
         * instead.
         * @see @ref isOffsetOnly()
         */
        constexpr Containers::StridedArrayView1D<const void> data() const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                {_data.pointer, ~std::size_t{}}, _vertexCount,
                (CORRADE_CONSTEXPR_ASSERT(!_isOffsetOnly, "Trade::MeshAttributeData::data(): the attribute is a relative offset, supply a data array"), _stride)};
        }

        /**
         * @brief Type-erased attribute data for an offset-only attribute
         *
         * If the attribute is not offset-only, the @p vertexData parameter is
         * ignored.
         * @see @ref isOffsetOnly(), @ref data() const
         */
        Containers::StridedArrayView1D<const void> data(Containers::ArrayView<const void> vertexData) const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                vertexData, _isOffsetOnly ? reinterpret_cast<const char*>(vertexData.data()) + _data.offset : _data.pointer, _vertexCount, _stride};
        }

    private:
        friend MeshData;

        /* nullptr first, to avoid accidental matches as much as possible */
        constexpr explicit MeshAttributeData(std::nullptr_t, MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<const void>& data, UnsignedShort arraySize) noexcept;

        VertexFormat _format;
        MeshAttribute _name;
        bool _isOffsetOnly;
        /* 1 byte free for more stuff on 64b (23, aligned to 24) and on 32b
           (19, aligned to 20) */

        /* Vertex count in MeshData is currently 32-bit, so this doesn't need
           to be 64-bit either */
        UnsignedInt _vertexCount;
        /* According to https://opengl.gpuinfo.org/displaycapability.php?name=GL_MAX_VERTEX_ATTRIB_STRIDE,
           current largest reported stride is 4k so 32k should be enough */
        Short _stride;
        UnsignedShort _arraySize;

        /* Data pointer last. Its size varies between 32 and 64 bit and having
           it last reduces the amount of pain when serializing. */
        union Data {
            /* FFS C++ why this doesn't JUST WORK goddamit?! */
            constexpr Data(const void* pointer = nullptr): pointer{pointer} {}
            constexpr Data(std::size_t offset): offset{offset} {}

            const void* pointer;
            std::size_t offset;
        } _data;
};

/** @relatesalso MeshAttributeData
@brief Create a non-owning array of @ref MeshAttributeData items
@m_since{2020,06}

Useful when you have the attribute definitions statically defined (for example
when the vertex data themselves are already defined at compile time) and don't
want to allocate just to pass those to @ref MeshData.
*/
Containers::Array<MeshAttributeData> MAGNUM_TRADE_EXPORT meshAttributeDataNonOwningArray(Containers::ArrayView<const MeshAttributeData> view);

/**
@brief Mesh data
@m_since{2020,06}

Provides access to mesh vertex and index data, together with additional
information such as primitive type. Populated instances of this class are
returned from @ref AbstractImporter::mesh() and from particular functions in
the @ref Primitives library.

@section Trade-MeshData-usage Basic usage

The simplest usage is through the convenience functions @ref positions2DAsArray(),
@ref positions3DAsArray(), @ref tangentsAsArray(), @ref bitangentsAsArray(),
@ref normalsAsArray(), @ref tangentsAsArray(), @ref textureCoordinates2DAsArray(),
@ref colorsAsArray() and @ref objectIdsAsArray(). Each of these takes an index
(as there can be multiple sets of texture coordinates, for example) and you're
expected to check for attribute presence first with either @ref hasAttribute()
or @ref attributeCount(MeshAttribute) const:

@snippet MagnumTrade.cpp MeshData-usage

@section Trade-MeshData-usage-advanced Advanced usage

The @ref positions2DAsArray(), ... functions shown above always return a
newly-allocated @ref Corrade::Containers::Array instance with a clearly defined
type that's large enough to represent most data. While that's fine for many use
cases, sometimes you may want to minimize the import time of a large model or
the imported data may be already in a well-optimized layout and format that you
want to preserve. The @ref MeshData class internally stores a contiguous blob
of data, which you can directly upload, and then use provided metadata to let
the GPU know of the format and layout. Because there's a lot of possible types
of each attribute (floats, packed integers, ...), the @ref GL::DynamicAttribute
can accept a pair of @ref GL::Attribute defined by the shader and the actual
@ref VertexFormat, figuring out all properties such as component count and
element data type without having to explicitly handle all relevant types:

@snippet MagnumTrade.cpp MeshData-usage-advanced

@section Trade-MeshData-usage-compile Using MeshTools::compile()

For a quick yet efficient way to upload all data and configure a mesh for all
known attributes that are present, @ref MeshTools::compile() can be used.
Compared to the above, it's just an oneliner:

@snippet MagnumTrade.cpp MeshData-usage-compile

Compared to configuring the mesh manually you may lose a bit of flexibility,
especially when you need to set up custom attributes or modify the data after.
See @ref MeshTools::compile(const Trade::MeshData&, GL::Buffer&, GL::Buffer&)
for a possible solution.

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

@section Trade-MeshData-populating Populating an instance

A @ref MeshData instance by default takes over the ownership of an
@ref Corrade::Containers::Array containing the vertex / index data together
with a @ref MeshIndexData instance and a list of @ref MeshAttributeData
describing various index and vertex properties. For example, an interleaved
indexed mesh with 3D positions and RGBA colors would look like this ---
and variants with just vertex data or just index data or neither are possible
too:

@snippet MagnumTrade.cpp MeshData-populating

In cases where you want the @ref MeshData instance to only refer to external
data without taking ownership (for example in a memory-mapped file, constant
memory etc.). Instead of moving in an @ref Corrade::Containers::Array you pass
@ref DataFlags describing if the data is mutable or not together with an
@ref Corrade::Containers::ArrayView. A variant of the above where the index
data is constant and vertex data mutable, both referenced externally:

@snippet MagnumTrade.cpp MeshData-populating-non-owned

@subsection Trade-MeshData-populating-custom Custom mesh attributes

To allow for greater flexibility, a @ref MeshData instance can describe not
just attributes that are predefined in the @ref MeshAttribute enum, but also
custom attributes, created with @ref meshAttributeCustom(). For example, the
snippet below describes a custom per-face structure that exposes faces as
higher-order polygons combining multiple triangles together ---in this case,
each face has an array of 15 IDs, which is exposed as a 2D array:

@snippet MagnumTrade.cpp MeshData-populating-custom

Later, the (array) attributes can be retrieved back using the same custom
identifiers --- note the use of @cpp [] @ce to get back a 2D array again:

@snippet MagnumTrade.cpp MeshData-populating-custom-retrieve

When a custom attribute is exposed through @ref AbstractImporter, it's possible
to map custom @ref MeshAttribute values to human-readable string names using
@ref AbstractImporter::meshAttributeName() and
@ref AbstractImporter::meshAttributeForName(). Using @ref meshPrimitiveWrap()
you can also supply implementation-specific values that are not available in
the generic @ref MeshPrimitive enum, similarly see also
@ref Trade-MeshAttributeData-custom-vertex-format for details on
implementation-specific @ref VertexFormat values.
@see @ref AbstractImporter::mesh()
*/
class MAGNUM_TRADE_EXPORT MeshData {
    public:
        enum: UnsignedInt {
            /**
             * Implicit vertex count. When passed to a constructor, indicates
             * that vertex count should be taken from attribute data views.
             */
            ImplicitVertexCount = ~UnsignedInt{}
        };

        /**
         * @brief Construct an indexed mesh data
         * @param primitive     Primitive
         * @param indexData     Index data
         * @param indices       Index data description
         * @param vertexData    Vertex data
         * @param attributes    Description of all vertex attribute data
         * @param vertexCount   Vertex count. If set to
         *      @ref ImplicitVertexCount, vertex count is taken from data views
         *      passed to @p attributes (in which case there has to be at least
         *      one).
         * @param importerState Importer-specific state
         *
         * The @p indices are expected to point to a sub-range of @p indexData.
         * The @p attributes are expected to reference (sparse) sub-ranges of
         * @p vertexData. If the mesh has no attributes, the @p indices are
         * expected to be valid (but can be empty). If you want to create an
         * attribute-less non-indexed mesh, use
         * @ref MeshData(MeshPrimitive, UnsignedInt, const void*) to specify
         * desired vertex count.
         *
         * The @ref indexDataFlags() / @ref vertexDataFlags() are implicitly
         * set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable. For non-owned data use the
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * constructor or its variants instead.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr);

        /**
         * @brief Construct indexed mesh data with non-owned index and vertex data
         * @param primitive         Primitive
         * @param indexDataFlags    Index data flags
         * @param indexData         View on index data
         * @param indices           Index data description
         * @param vertexDataFlags   Vertex data flags
         * @param vertexData        View on vertex data
         * @param attributes        Description of all vertex attribute data
         * @param vertexCount       Vertex count. If set to
         *      @ref ImplicitVertexCount, vertex count is taken from data views
         *      passed to @p attributes (in which case there has to be at least
         *      one).
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * creates an instance that doesn't own the passed vertex and index
         * data. The @p indexDataFlags / @p vertexDataFlags parameters can
         * contain @ref DataFlag::Mutable to indicate the external data can be
         * modified, and is expected to *not* have @ref DataFlag::Owned set.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr);

        /**
         * @brief Construct indexed mesh data with non-owned index data
         * @param primitive         Primitive
         * @param indexDataFlags    Index data flags
         * @param indexData         View on index data
         * @param indices           Index data description
         * @param vertexData        Vertex data
         * @param attributes        Description of all vertex attribute data
         * @param vertexCount       Vertex count. If set to
         *      @ref ImplicitVertexCount, vertex count is taken from data views
         *      passed to @p attributes (in which case there has to be at least
         *      one).
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * creates an instance that doesn't own the passed index data. The
         * @p indexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. The @ref vertexDataFlags() are
         * implicitly set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr);

        /**
         * @brief Construct indexed mesh data with non-owned vertex data
         * @param primitive         Primitive
         * @param indexData         Index data
         * @param indices           Index data description
         * @param vertexDataFlags   Vertex data flags
         * @param vertexData        View on vertex data
         * @param attributes        Description of all vertex attribute data
         * @param vertexCount       Vertex count. If set to
         *      @ref ImplicitVertexCount, vertex count is taken from data views
         *      passed to @p attributes (in which case there has to be at least
         *      one).
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * creates an instance that doesn't own the passed vertex data. The
         * @p vertexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. The @ref indexDataFlags() are
         * implicitly set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr);

        /**
         * @brief Construct a non-indexed mesh data
         * @param primitive     Primitive
         * @param vertexData    Vertex data
         * @param attributes    Description of all vertex attribute data
         * @param vertexCount   Vertex count. If set to
         *      @ref ImplicitVertexCount, vertex count is taken from data views
         *      passed to @p attributes (in which case there has to be at least
         *      one).
         * @param importerState Importer-specific state
         *
         * Same as calling @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * with default-constructed @p indexData and @p indices arguments.
         *
         * The @ref vertexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For consistency,
         * the @ref indexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate. For non-owned data use the
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * constructor instead.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned non-indexed mesh data
         * @param primitive         Primitive
         * @param vertexDataFlags   Vertex data flags
         * @param vertexData        View on vertex data
         * @param attributes        Description of all vertex attribute data
         * @param vertexCount       Vertex count. If set to
         *      @ref ImplicitVertexCount, vertex count is taken from data views
         *      passed to @p attributes (in which case there has to be at least
         *      one).
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p vertexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. For consistency, the
         * @ref indexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MeshData(MeshPrimitive primitive, DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, UnsignedInt vertexCount = ImplicitVertexCount, const void* importerState = nullptr);

        /**
         * @brief Construct an attribute-less indexed mesh data
         * @param primitive     Primitive
         * @param indexData     Index data
         * @param indices       Index data description
         * @param vertexCount   Vertex count. Passing @ref ImplicitVertexCount
         *      is not allowed in this overload.
         * @param importerState Importer-specific state
         *
         * Same as calling @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * with default-constructed @p vertexData and @p attributes arguments.
         * The @p indices are expected to be valid (but can be empty). If you
         * want to create an attribute-less non-indexed mesh, use
         * @ref MeshData(MeshPrimitive, UnsignedInt, const void*) to specify
         * desired vertex count.
         *
         * The @ref indexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For consistency,
         * the @ref vertexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate. For non-owned data use the
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, UnsignedInt, const void*)
         * constructor instead.
         */
        explicit MeshData(MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, UnsignedInt vertexCount, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct a non-owned attribute-less indexed mesh data
         * @param primitive         Primitive
         * @param indexDataFlags    Index data flags
         * @param indexData         View on index data
         * @param indices           Index data description
         * @param vertexCount       Vertex count. Passing
         *      @ref ImplicitVertexCount is not allowed in this overload.
         * @param importerState     Importer-specific state
         *
         * Compared to @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, UnsignedInt, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p indexDataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set. For consistency, the
         * @ref vertexDataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable, even though there
         * isn't any data to own or to mutate.
         */
        explicit MeshData(MeshPrimitive primitive, DataFlags indexDataFlags, Containers::ArrayView<const void> indexData, const MeshIndexData& indices, UnsignedInt vertexCount, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct an index-less attribute-less mesh data
         * @param primitive     Primitive
         * @param vertexCount   Vertex count. Passing @ref ImplicitVertexCount
         *      is not allowed in this overload.
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
         * @brief Raw attribute metadata
         *
         * Returns the raw data that are used as a base for all `attribute*()`
         * accessors, or @cpp nullptr @ce if the mesh has no attributes. In
         * most cases you don't want to access those directly, but rather use
         * the @ref attribute(), @ref attributeName(), @ref attributeFormat(),
         * @ref attributeOffset(), @ref attributeStride() etc. accessors.
         * Compared to those and to @ref attributeData(UnsignedInt) const, the
         * @ref MeshAttributeData instances returned by this function may have
         * different data pointers, and some of them might be offset-only ---
         * use this function only if you *really* know what are you doing.
         * @see @ref MeshAttributeData::isOffsetOnly()
         */
        Containers::ArrayView<const MeshAttributeData> attributeData() const & { return _attributes; }

        /** @brief Taking a view to a r-value instance is not allowed */
        Containers::ArrayView<const MeshAttributeData> attributeData() && = delete;

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
         * @see @ref isIndexed(), @ref indexType(),
         *      @ref MeshTools::primitiveCount(MeshPrimitive, UnsignedInt)
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
         * @brief Index offset
         *
         * Byte offset of the first index from the beginning of the
         * @ref indexData(), or a byte difference between pointers returned
         * from @ref indexData() and @ref indices(). Expects that the mesh is
         * indexed.
         * @see @ref attributeOffset()
         */
        std::size_t indexOffset() const;

        /**
         * @brief Mesh indices
         *
         * For an indexed mesh, the view is guaranteed to be contiguous and its
         * second dimension represents the actual data type (its size is equal
         * to type size, even in case there's zero indices). For a non-indexed
         * mesh, the returned view has a zero size in both dimensions.
         *
         * Use the templated overload below to get the indices in a concrete
         * type.
         * @see @ref Corrade::Containers::StridedArrayView::isContiguous()
         */
        Containers::StridedArrayView2D<const char> indices() const;

        /**
         * @brief Mutable mesh indices
         *
         * Like @ref indices() const, but returns a mutable view. Expects that
         * the mesh is mutable.
         * @see @ref indexDataFlags()
         */
        Containers::StridedArrayView2D<char> mutableIndices();

        /**
         * @brief Mesh indices in a concrete type
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
         * @brief Mutable mesh indices in a concrete type
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
         * @see @ref MeshTools::primitiveCount(MeshPrimitive, UnsignedInt)
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
        UnsignedInt attributeCount() const { return UnsignedInt(_attributes.size()); }

        /**
         * @brief Raw attribute data
         *
         * Returns the raw data that are used as a base for all `attribute*()`
         * accessors. In most cases you don't want to access those directly,
         * but rather use the @ref attribute(), @ref attributeName(),
         * @ref attributeFormat(), @ref attributeOffset(),
         * @ref attributeStride() etc. accessors.
         *
         * Useful mainly for passing particular attributes unchanged directly
         * to @ref MeshTools algorithms --- unlike with @ref attributeData()
         * and @ref releaseAttributeData(), returned instances are guaranteed
         * to always have an absolute data pointer (i.e.,
         * @ref MeshAttributeData::isOffsetOnly() always returning
         * @cpp false @ce). The @p id is expected to be smaller than
         * @ref attributeCount() const.
         * @see @ref MeshTools::interleavedData()
         */
        MeshAttributeData attributeData(UnsignedInt id) const;

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
         * @see @ref indexOffset(), @ref MeshTools::isInterleaved()
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
         * @see @ref MeshTools::isInterleaved()
         */
        UnsignedInt attributeStride(UnsignedInt id) const;

        /**
         * @brief Attribute array size
         *
         * In case given attribute is an array (the equivalent of e.g.
         * @cpp int[30] @ce), returns array size, otherwise returns @cpp 0 @ce.
         * At the moment only custom attributes can be arrays, no builtin
         * @ref MeshAttribute is an array attribute. You can also use
         * @ref attributeArraySize(MeshAttribute, UnsignedInt) const to
         * directly get array size of given named attribute.
         *
         * Note that this is different from vertex count, which is exposed
         * through @ref vertexCount(), and is an orthogonal concept to having
         * multiple attributes of the same name (for example two sets of
         * texture coordinates), which is exposed through
         * @ref attributeCount(MeshAttribute) const. See
         * @ref Trade-MeshData-populating-custom for an example.
         * @see @ref isMeshAttributeCustom()
         */
        UnsignedShort attributeArraySize(UnsignedInt id) const;

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
         * @brief Absolute ID of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const.
         */
        UnsignedInt attributeId(MeshAttribute name, UnsignedInt id = 0) const;

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
         * @brief Array size of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const. Note that this is
         * different from vertex count, and is an orthogonal concept to having
         * multiple attributes of the same name --- see
         * @ref attributeArraySize(UnsignedInt) const for more information.
         */
        UnsignedShort attributeArraySize(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Data for given attribute
         *
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * The second dimension represents the actual data type (its size is
         * equal to format size for known @ref VertexFormat values and to
         * attribute stride for implementation-specific values) and is
         * guaranteed to be contiguous. Use the templated overload below to get
         * the attribute in a concrete type.
         * @see @ref Corrade::Containers::StridedArrayView::isContiguous(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::StridedArrayView2D<const char> attribute(UnsignedInt id) const;

        /**
         * @brief Mutable data for given attribute
         *
         * Like @ref attribute(UnsignedInt) const, but returns a mutable view.
         * Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        Containers::StridedArrayView2D<char> mutableAttribute(UnsignedInt id);

        /**
         * @brief Data for given attribute in a concrete type
         *
         * The @p id is expected to be smaller than @ref attributeCount() const
         * and @p T is expected to correspond to
         * @ref attributeFormat(UnsignedInt) const. Expects that the vertex
         * format is *not* implementation-specific, in that case you can only
         * access the attribute via the typeless @ref attribute(UnsignedInt) const
         * above. The attribute is also expected to not be an array, in that
         * case you need to use the overload below by using @cpp T[] @ce
         * instead of @cpp T @ce. You can also use the non-templated
         * @ref positions2DAsArray(), @ref positions3DAsArray(),
         * @ref tangentsAsArray(), @ref bitangentSignsAsArray(),
         * @ref bitangentsAsArray(), @ref normalsAsArray(),
         * @ref textureCoordinates2DAsArray(), @ref colorsAsArray() and
         * @ref objectIdsAsArray() accessors to get common attributes converted
         * to usual types, but note that these operations involve extra
         * allocation and data conversion.
         * @see @ref attribute(MeshAttribute, UnsignedInt) const,
         *      @ref mutableAttribute(MeshAttribute, UnsignedInt),
         *      @ref isVertexFormatImplementationSpecific(),
         *      @ref attributeArraySize()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<const T> attribute(UnsignedInt id) const;

        /**
         * @brief Data for given array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes instead
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> attribute(UnsignedInt id) const;

        /**
         * @brief Mutable data for given attribute in a concrete type
         *
         * Like @ref attribute(UnsignedInt) const, but returns a mutable view.
         * Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<T> mutableAttribute(UnsignedInt id);

        /**
         * @brief Mutable data for given array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes instead
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableAttribute(UnsignedInt id);

        /**
         * @brief Data for given named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const. The second dimension
         * represents the actual data type (its size is equal to format size
         * for known @ref VertexFormat values and to attribute stride for
         * implementation-specific values) and is guaranteed to be contiguous.
         * Use the templated overload below to get the attribute in a concrete
         * type.
         * @see @ref attribute(UnsignedInt) const,
         *      @ref mutableAttribute(MeshAttribute, UnsignedInt),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::StridedArrayView2D<const char> attribute(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Mutable data for given named attribute
         *
         * Like @ref attribute(MeshAttribute, UnsignedInt) const, but returns a
         * mutable view. Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        Containers::StridedArrayView2D<char> mutableAttribute(MeshAttribute name, UnsignedInt id = 0);

        /**
         * @brief Data for given named attribute in a concrete type
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute) const and @p T is expected to
         * correspond to @ref attributeFormat(MeshAttribute, UnsignedInt) const.
         * Expects that the vertex format is *not* implementation-specific, in
         * that case you can only access the attribute via the typeless
         * @ref attribute(MeshAttribute, UnsignedInt) const above. You can also
         * use the non-templated @ref positions2DAsArray(),
         * @ref positions3DAsArray(), @ref normalsAsArray(),
         * @ref textureCoordinates2DAsArray() and @ref colorsAsArray()
         * accessors to get common attributes converted to usual types, but
         * note that these operations involve extra data conversion and an
         * allocation.
         * @see @ref attribute(UnsignedInt) const,
         *      @ref mutableAttribute(MeshAttribute, UnsignedInt),
         *      @ref isVertexFormatImplementationSpecific()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<const T> attribute(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Data for given named array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes instead
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> attribute(MeshAttribute name, UnsignedInt id = 0) const;

        /**
         * @brief Mutable data for given named attribute in a concrete type
         *
         * Like @ref attribute(MeshAttribute, UnsignedInt) const, but returns a
         * mutable view. Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<T> mutableAttribute(MeshAttribute name, UnsignedInt id = 0);

        /**
         * @brief Mutable data for given named array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes instead
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableAttribute(MeshAttribute name, UnsignedInt id = 0);

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
        void indicesInto(Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Positions as 2D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Position as the first argument. Converts
         * the position array from an arbitrary underlying type and returns it
         * in a newly-allocated array. If the underlying type is
         * three-component, the last component is dropped. Expects that the
         * vertex format is *not* implementation-specific, in that case you can
         * only access the attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         * @see @ref positions2DInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
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
         * the Z component is set to @cpp 0.0f @ce. Expects that the vertex
         * format is *not* implementation-specific, in that case you can only
         * access the attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         * @see @ref positions3DInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
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
         * @brief Tangents as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Tangent as the first argument. Converts the
         * tangent array from an arbitrary underlying type and returns it in a
         * newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         *
         * If the tangents contain a fourth component with bitangent direction,
         * it's ignored here --- use @ref bitangentSignsAsArray() to get those
         * instead. You can also use @ref tangentsInto() together with
         * @ref bitangentSignsInto() to put them both in a single array.
         * @see @ref bitangentsAsArray(), @ref normalsAsArray(),
         *      @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector3> tangentsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Tangents as 3D float vectors into a pre-allocated view
         *
         * Like @ref tangentsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void tangentsInto(Containers::StridedArrayView1D<Vector3> destination, UnsignedInt id = 0) const;

        /**
         * @brief Bitangent signs as floats
         *
         * Counterpart to @ref tangentsAsArray() returning value of the fourth
         * component. Expects that the type of @ref MeshAttribute::Tangent is
         * four-component. You can also use @ref tangentsInto() together with
         * @ref bitangentSignsInto() to put them both in a single array.
         * @see @ref tangentsAsArray(), @ref bitangentsAsArray(),
         *      @ref normalsAsArray(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Float> bitangentSignsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Bitangent signs as floats into a pre-allocated view
         *
         * Like @ref bitangentsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void bitangentSignsInto(Containers::StridedArrayView1D<Float> destination, UnsignedInt id = 0) const;

        /**
         * @brief Bitangents as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Bitangent as the first argument. Converts
         * the bitangent array from an arbitrary underlying type and returns it
         * in a newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         *
         * Note that in some cases the bitangents aren't provided directly but
         * calculated from normals and four-component tangents. In that case
         * you'll need to get bitangent signs via @ref bitangentSignsAsArray()
         * and calculate the bitangents as shown in the documentation of
         * @ref MeshAttribute::Tangent.
         * @see @ref bitangentsInto(), @ref tangentsAsArray(),
         *      @ref normalsAsArray(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector3> bitangentsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Bitangents as 3D float vectors into a pre-allocated view
         *
         * Like @ref bitangentsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void bitangentsInto(Containers::StridedArrayView1D<Vector3> destination, UnsignedInt id = 0) const;

        /**
         * @brief Normals as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::Normal as the first argument. Converts the
         * normal array from an arbitrary underlying type and returns it in a
         * newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         * @see @ref normalsInto(), @ref tangentsAsArray(),
         *      @ref bitangentsAsArray(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
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
         * type and returns it in a newly-allocated array. Expects that the
         * vertex format is *not* implementation-specific, in that case you can
         * only access the attribute via the typeless
         * @ref attribute(MeshAttribute, UnsignedInt) const.
         * @see @ref textureCoordinates2DInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
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
         * the alpha component is set to @cpp 1.0f @ce. Expects that the vertex
         * format is *not* implementation-specific, in that case you can only
         * access the attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         * @see @ref colorsInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
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
         * @brief Object IDs as 32-bit integers
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt) const
         * with @ref MeshAttribute::ObjectId as the first argument. Converts
         * the object ID array from an arbitrary underlying type and returns it
         * in a newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt) const.
         * @see @ref objectIdsInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<UnsignedInt> objectIdsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Object IDs as 32-bit integers into a pre-allocated view
         *
         * Like @ref objectIdsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void objectIdsInto(Containers::StridedArrayView1D<UnsignedInt> destination, UnsignedInt id = 0) const;

        /**
         * @brief Release index data storage
         *
         * Releases the ownership of the index data array and resets internal
         * index-related state to default. The mesh then behaves like
         * it has zero indices (but it can still have a non-zero vertex count),
         * however @ref indexData() still return a zero-sized non-null array so
         * index offset calculation continues to work as expected.
         *
         * Note that the returned array has a custom no-op deleter when the
         * data are not owned by the mesh, and while the returned array type is
         * mutable, the actual memory might be not.
         * @see @ref indexData(), @ref indexDataFlags()
         */
        Containers::Array<char> releaseIndexData();

        /**
         * @brief Release attribute data storage
         *
         * Releases the ownership of the attribute data array and resets
         * internal attribute-related state to default. The mesh then behaves
         * like if it has no attributes (but it can still have a non-zero
         * vertex count). Note that the returned array has a custom no-op
         * deleter when the data are not owned by the mesh, and while the
         * returned array type is mutable, the actual memory might be not.
         * Additionally, the returned @ref MeshAttributeData instances
         * may have different data pointers and sizes than what's returned by
         * the @ref attribute() and @ref attributeData(UnsignedInt) const
         * accessors as some of them might be offset-only --- use this function
         * only if you *really* know what are you doing.
         * @see @ref attributeData(), @ref MeshAttributeData::isOffsetOnly()
         */
        Containers::Array<MeshAttributeData> releaseAttributeData();

        /**
         * @brief Release vertex data storage
         *
         * Releases the ownership of the vertex data array and resets internal
         * attribute-related state to default. The mesh then behaves like it
         * has zero vertices (but it can still have a non-zero amount of
         * attributes), however @ref vertexData() will still return a zero-
         * sized non-null array so attribute offset calculation continues to
         * work as expected.
         *
         * Note that the returned array has a custom no-op deleter when the
         * data are not owned by the mesh, and while the returned array type is
         * mutable, the actual memory might be not.
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
        friend AbstractSceneConverter;

        /* Internal helper that doesn't assert, unlike attributeId() */
        UnsignedInt attributeFor(MeshAttribute name, UnsignedInt id) const;

        /* Like attribute(), but returning just a 1D view */
        Containers::StridedArrayView1D<const void> attributeDataViewInternal(const MeshAttributeData& attribute) const;

        #ifndef CORRADE_NO_ASSERT
        template<class T> bool checkAttributeTypeCompatibility(const MeshAttributeData& attribute, const char* prefix) const;
        #endif

        /* GPUs don't currently support more than 32-bit index types / vertex
           counts so this should be enough. Sanity check:
           https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkIndexType.html */
        UnsignedInt _indexCount, _vertexCount;
        MeshPrimitive _primitive;
        MeshIndexType _indexType;
        DataFlags _indexDataFlags, _vertexDataFlags;
        const void* _importerState;
        const char* _indices;
        Containers::Array<MeshAttributeData> _attributes;
        Containers::Array<char> _indexData, _vertexData;
};

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

    /* Implicit mapping from a format to enum (1:1) */
    template<class T> constexpr VertexFormat vertexFormatFor() {
        /* C++ why there isn't an obvious way to do such a thing?! */
        static_assert(sizeof(T) == 0, "unsupported attribute type");
        return {};
    }
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(format) \
        template<> constexpr VertexFormat vertexFormatFor<format>() { return VertexFormat::format; }
    _c(Float)
    _c(Half)
    _c(Double)
    _c(UnsignedByte)
    _c(Byte)
    _c(UnsignedShort)
    _c(Short)
    _c(UnsignedInt)
    _c(Int)
    _c(Vector2)
    _c(Vector2h)
    _c(Vector2d)
    _c(Vector2ub)
    _c(Vector2b)
    _c(Vector2us)
    _c(Vector2s)
    _c(Vector2ui)
    _c(Vector2i)
    _c(Vector3)
    _c(Vector3h)
    _c(Vector3d)
    _c(Vector3ub)
    _c(Vector3b)
    _c(Vector3us)
    _c(Vector3s)
    _c(Vector3ui)
    _c(Vector3i)
    _c(Vector4)
    _c(Vector4h)
    _c(Vector4d)
    _c(Vector4ub)
    _c(Vector4b)
    _c(Vector4us)
    _c(Vector4s)
    _c(Vector4ui)
    _c(Vector4i)
    #define _cn(format) \
        template<> constexpr VertexFormat vertexFormatFor<format>() { return VertexFormat::format ## Normalized; }
     _c(Matrix2x2)
     _c(Matrix2x2h)
     _c(Matrix2x2d)
    _cn(Matrix2x2b)
    _cn(Matrix2x2s)
     _c(Matrix2x3)
     _c(Matrix2x3h)
     _c(Matrix2x3d)
    _cn(Matrix2x3b)
    _cn(Matrix2x3s)
     _c(Matrix2x4)
     _c(Matrix2x4h)
     _c(Matrix2x4d)
    _cn(Matrix2x4b)
    _cn(Matrix2x4s)
     _c(Matrix3x2)
     _c(Matrix3x2h)
     _c(Matrix3x2d)
    _cn(Matrix3x2b)
    _cn(Matrix3x2s)
     _c(Matrix3x3)
     _c(Matrix3x3h)
     _c(Matrix3x3d)
    _cn(Matrix3x3b)
    _cn(Matrix3x3s)
     _c(Matrix3x4)
     _c(Matrix3x4h)
     _c(Matrix3x4d)
    _cn(Matrix3x4b)
    _cn(Matrix3x4s)
     _c(Matrix4x2)
     _c(Matrix4x2h)
     _c(Matrix4x2d)
    _cn(Matrix4x2b)
    _cn(Matrix4x2s)
     _c(Matrix4x3)
     _c(Matrix4x3h)
     _c(Matrix4x3d)
    _cn(Matrix4x3b)
    _cn(Matrix4x3s)
     _c(Matrix4x4)
     _c(Matrix4x4h)
     _c(Matrix4x4d)
    _cn(Matrix4x4b)
    _cn(Matrix4x4s)
    #undef _c
    #undef _cn
    #endif
    template<> constexpr VertexFormat vertexFormatFor<Color3>() { return VertexFormat::Vector3; }
    template<> constexpr VertexFormat vertexFormatFor<Color3h>() { return VertexFormat::Vector3h; }
    template<> constexpr VertexFormat vertexFormatFor<Color3ub>() { return VertexFormat::Vector3ubNormalized; }
    template<> constexpr VertexFormat vertexFormatFor<Color3us>() { return VertexFormat::Vector3usNormalized; }
    template<> constexpr VertexFormat vertexFormatFor<Color4>() { return VertexFormat::Vector4; }
    template<> constexpr VertexFormat vertexFormatFor<Color4h>() { return VertexFormat::Vector4h; }
    template<> constexpr VertexFormat vertexFormatFor<Color4ub>() { return VertexFormat::Vector4ubNormalized; }
    template<> constexpr VertexFormat vertexFormatFor<Color4us>() { return VertexFormat::Vector4usNormalized; }
    template<> constexpr VertexFormat vertexFormatFor<Matrix3>() { return VertexFormat::Matrix3x3; }
    template<> constexpr VertexFormat vertexFormatFor<Matrix3d>() { return VertexFormat::Matrix3x3d; }
    template<> constexpr VertexFormat vertexFormatFor<Matrix4>() { return VertexFormat::Matrix4x4; }
    template<> constexpr VertexFormat vertexFormatFor<Matrix4d>() { return VertexFormat::Matrix4x4d; }

    #ifndef CORRADE_NO_ASSERT
    /* Check if enum is compatible with a format (1:n). Mostly just 1:1 mapping
       tho, so reusing vertexFormatFor(), with a few exceptions. */
    template<class T> constexpr bool isVertexFormatCompatible(VertexFormat type) {
        return vertexFormatFor<T>() == type;
    }
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(format_)                                                     \
        template<> constexpr bool isVertexFormatCompatible<format_>(VertexFormat format) { \
            return format == VertexFormat::format_ ||                       \
                   format == VertexFormat::format_ ## Normalized;           \
        }
    _c(UnsignedByte)
    _c(Byte)
    _c(UnsignedShort)
    _c(Short)
    _c(Vector2ub)
    _c(Vector2b)
    _c(Vector2us)
    _c(Vector2s)
    _c(Vector3ub)
    _c(Vector3b)
    _c(Vector3us)
    _c(Vector3s)
    _c(Vector4ub)
    _c(Vector4b)
    _c(Vector4us)
    _c(Vector4s)
    /* For Color[34]u[sb] we expect the format to be normalized, which is
       handled by vertexFormatFor() properly already */
    template<> constexpr bool isVertexFormatCompatible<Matrix2x4h>(VertexFormat format) {
        return format == VertexFormat::Matrix2x4h ||
               format == VertexFormat::Matrix2x3hAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix2x4b>(VertexFormat format) {
        return format == VertexFormat::Matrix2x4bNormalized ||
               format == VertexFormat::Matrix2x2bNormalizedAligned ||
               format == VertexFormat::Matrix2x3bNormalizedAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix2x4s>(VertexFormat format) {
        return format == VertexFormat::Matrix2x4sNormalized ||
               format == VertexFormat::Matrix2x3sNormalizedAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix3x4h>(VertexFormat format) {
        return format == VertexFormat::Matrix3x4h ||
               format == VertexFormat::Matrix3x3hAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix3x4b>(VertexFormat format) {
        return format == VertexFormat::Matrix3x4bNormalized ||
               format == VertexFormat::Matrix3x2bNormalizedAligned ||
               format == VertexFormat::Matrix3x3bNormalizedAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix3x4s>(VertexFormat format) {
        return format == VertexFormat::Matrix3x4sNormalized ||
               format == VertexFormat::Matrix3x3sNormalizedAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix4x4h>(VertexFormat format) {
        return format == VertexFormat::Matrix4x4h ||
               format == VertexFormat::Matrix4x3hAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix4x4b>(VertexFormat format) {
        return format == VertexFormat::Matrix4x4bNormalized ||
               format == VertexFormat::Matrix4x2bNormalizedAligned ||
               format == VertexFormat::Matrix4x3bNormalizedAligned;
    }
    template<> constexpr bool isVertexFormatCompatible<Matrix4x4s>(VertexFormat format) {
        return format == VertexFormat::Matrix4x4sNormalized ||
               format == VertexFormat::Matrix4x3sNormalizedAligned;
    }
    #undef _c
    #endif
    /* LCOV_EXCL_STOP */

    constexpr bool isVertexFormatCompatibleWithAttribute(MeshAttribute name, VertexFormat format) {
        /* Double types intentionally not supported for any builtin attributes
           right now -- only for custom types */
        return
            /* Implementation-specific formats can be used for any attribute
               (tho the access capabilities will be reduced) */
            isVertexFormatImplementationSpecific(format) ||
            /* Named attributes are restricted so we can decode them */
            (name == MeshAttribute::Position &&
                (format == VertexFormat::Vector2 ||
                 format == VertexFormat::Vector2h ||
                 format == VertexFormat::Vector2ub ||
                 format == VertexFormat::Vector2ubNormalized ||
                 format == VertexFormat::Vector2b ||
                 format == VertexFormat::Vector2bNormalized ||
                 format == VertexFormat::Vector2us ||
                 format == VertexFormat::Vector2usNormalized ||
                 format == VertexFormat::Vector2s ||
                 format == VertexFormat::Vector2sNormalized ||
                 format == VertexFormat::Vector3 ||
                 format == VertexFormat::Vector3h ||
                 format == VertexFormat::Vector3ub ||
                 format == VertexFormat::Vector3ubNormalized ||
                 format == VertexFormat::Vector3b ||
                 format == VertexFormat::Vector3bNormalized ||
                 format == VertexFormat::Vector3us ||
                 format == VertexFormat::Vector3usNormalized ||
                 format == VertexFormat::Vector3s ||
                 format == VertexFormat::Vector3sNormalized)) ||
            (name == MeshAttribute::Tangent &&
                (format == VertexFormat::Vector3 ||
                 format == VertexFormat::Vector3h ||
                 format == VertexFormat::Vector3bNormalized ||
                 format == VertexFormat::Vector3sNormalized ||
                 format == VertexFormat::Vector4 ||
                 format == VertexFormat::Vector4h ||
                 format == VertexFormat::Vector4bNormalized ||
                 format == VertexFormat::Vector4sNormalized)) ||
            ((name == MeshAttribute::Bitangent || name == MeshAttribute::Normal) &&
                (format == VertexFormat::Vector3 ||
                 format == VertexFormat::Vector3h ||
                 format == VertexFormat::Vector3bNormalized ||
                 format == VertexFormat::Vector3sNormalized)) ||
            (name == MeshAttribute::Color &&
                (format == VertexFormat::Vector3 ||
                 format == VertexFormat::Vector3h ||
                 format == VertexFormat::Vector3ubNormalized ||
                 format == VertexFormat::Vector3usNormalized ||
                 format == VertexFormat::Vector4 ||
                 format == VertexFormat::Vector4h ||
                 format == VertexFormat::Vector4ubNormalized ||
                 format == VertexFormat::Vector4usNormalized)) ||
            (name == MeshAttribute::TextureCoordinates &&
                (format == VertexFormat::Vector2 ||
                 format == VertexFormat::Vector2h ||
                 format == VertexFormat::Vector2ub ||
                 format == VertexFormat::Vector2ubNormalized ||
                 format == VertexFormat::Vector2b ||
                 format == VertexFormat::Vector2bNormalized ||
                 format == VertexFormat::Vector2us ||
                 format == VertexFormat::Vector2usNormalized ||
                 format == VertexFormat::Vector2s ||
                 format == VertexFormat::Vector2sNormalized)) ||
            (name == MeshAttribute::ObjectId &&
                (format == VertexFormat::UnsignedInt ||
                 format == VertexFormat::UnsignedShort ||
                 format == VertexFormat::UnsignedByte)) ||
            /* Custom attributes can be anything */
            isMeshAttributeCustom(name);
    }

    constexpr bool isAttributeArrayAllowed(MeshAttribute name) {
        return isMeshAttributeCustom(name);
    }
    #endif
}

constexpr MeshAttributeData::MeshAttributeData(std::nullptr_t, const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView1D<const void>& data, const UnsignedShort arraySize) noexcept:
    _format{(CORRADE_CONSTEXPR_ASSERT(!arraySize || !isVertexFormatImplementationSpecific(format),
        "Trade::MeshAttributeData: array attributes can't have an implementation-specific format"), format)},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isVertexFormatCompatibleWithAttribute(name, format),
        "Trade::MeshAttributeData:" << format << "is not a valid format for" << name), name)},
    _isOffsetOnly{false}, _vertexCount{UnsignedInt(data.size())},
    /** @todo support zero / negative stride? would be hard to transfer to GL */
    _stride{(CORRADE_CONSTEXPR_ASSERT(!(UnsignedInt(data.stride()) & 0xffff8000),
        "Trade::MeshAttributeData: expected stride to be positive and at most 32k, got" << data.stride()),
        Short(data.stride()))},
    _arraySize{(CORRADE_CONSTEXPR_ASSERT(!arraySize || Implementation::isAttributeArrayAllowed(name),
        "Trade::MeshAttributeData:" << name << "can't be an array attribute"), arraySize)},
    _data{data.data()} {}

constexpr MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const std::size_t offset, const UnsignedInt vertexCount, const std::ptrdiff_t stride, UnsignedShort arraySize) noexcept:
    _format{(CORRADE_CONSTEXPR_ASSERT(!arraySize || !isVertexFormatImplementationSpecific(format),
        "Trade::MeshAttributeData: array attributes can't have an implementation-specific format"), format)},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isVertexFormatCompatibleWithAttribute(name, format),
        "Trade::MeshAttributeData:" << format << "is not a valid format for" << name), name)},
    _isOffsetOnly{true}, _vertexCount{vertexCount},
    /** @todo support zero / negative stride? would be hard to transfer to GL */
    _stride{(CORRADE_CONSTEXPR_ASSERT(!(UnsignedInt(stride) & 0xffff8000),
        "Trade::MeshAttributeData: expected stride to be positive and at most 32k, got" << stride),
        Short(stride))},
    _arraySize{(CORRADE_CONSTEXPR_ASSERT(!arraySize || Implementation::isAttributeArrayAllowed(name),
        "Trade::MeshAttributeData:" << name << "can't be an array attribute"), arraySize)},
    _data{offset} {}

template<class T> constexpr MeshAttributeData::MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView1D<T>& data) noexcept: MeshAttributeData{nullptr, name, Implementation::vertexFormatFor<typename std::remove_const<T>::type>(), data, 0} {}

template<class T> constexpr MeshAttributeData::MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView2D<T>& data) noexcept: MeshAttributeData{(CORRADE_CONSTEXPR_ASSERT(data.stride()[1] == sizeof(T), "Trade::MeshAttributeData: second view dimension is not contiguous"), nullptr), name, Implementation::vertexFormatFor<typename std::remove_const<T>::type>(), Containers::StridedArrayView1D<const void>{{data.data(), ~std::size_t{}}, data.size()[0], data.stride()[0]}, UnsignedShort(data.size()[1])} {}

template<class T> Containers::ArrayView<const T> MeshData::indices() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indices(): the mesh is not indexed", {});
    Containers::StridedArrayView2D<const char> data = indices();
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(Implementation::meshIndexTypeFor<T>() == _indexType,
        "Trade::MeshData::indices(): improper type requested for" << _indexType, nullptr);
    return Containers::arrayCast<1, const T>(data).asContiguous();
}

template<class T> Containers::ArrayView<T> MeshData::mutableIndices() {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::mutableIndices(): the mesh is not indexed", {});
    Containers::StridedArrayView2D<char> data = mutableIndices();
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(Implementation::meshIndexTypeFor<T>() == _indexType,
        "Trade::MeshData::mutableIndices(): improper type requested for" << _indexType, nullptr);
    return Containers::arrayCast<1, T>(data).asContiguous();
}

#ifndef CORRADE_NO_ASSERT
template<class T> bool MeshData::checkAttributeTypeCompatibility(const MeshAttributeData& attribute, const char* const prefix) const {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        prefix << "can't cast data from an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), false);
    CORRADE_ASSERT(Implementation::isVertexFormatCompatible<typename std::remove_extent<T>::type>(attribute._format),
        prefix << "improper type requested for" << attribute._name << "of format" << attribute._format, false);
    CORRADE_ASSERT(std::is_array<T>::value == !!attribute._arraySize,
        prefix << "use T[] to access an array attribute", false);
    return true;
}
#endif

template<class T, class> Containers::StridedArrayView1D<const T> MeshData::attribute(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = attribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkAttributeTypeCompatibility<T>(_attributes[id], "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, class> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> MeshData::attribute(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = attribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    const MeshAttributeData& attribute = _attributes[id];
    if(!checkAttributeTypeCompatibility<T>(attribute, "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, class> Containers::StridedArrayView1D<T> MeshData::mutableAttribute(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableAttribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkAttributeTypeCompatibility<T>(_attributes[id], "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, class> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> MeshData::mutableAttribute(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableAttribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    const MeshAttributeData& attribute = _attributes[id];
    if(!checkAttributeTypeCompatibility<T>(attribute, "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}

template<class T, class> Containers::StridedArrayView1D<const T> MeshData::attribute(MeshAttribute name, UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = attribute(name, id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkAttributeTypeCompatibility<T>(_attributes[attributeFor(name, id)], "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, class> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> MeshData::attribute(MeshAttribute name, UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = attribute(name, id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    const MeshAttributeData& attribute = _attributes[attributeFor(name, id)];
    if(!checkAttributeTypeCompatibility<T>(attribute, "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, class> Containers::StridedArrayView1D<T> MeshData::mutableAttribute(MeshAttribute name, UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableAttribute(name, id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkAttributeTypeCompatibility<T>(_attributes[attributeFor(name, id)], "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, class> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> MeshData::mutableAttribute(MeshAttribute name, UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableAttribute(name, id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    const MeshAttributeData& attribute = _attributes[attributeFor(name, id)];
    if(!checkAttributeTypeCompatibility<T>(attribute, "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}

}}

#endif
