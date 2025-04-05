#ifndef Magnum_Trade_MeshData_h
#define Magnum_Trade_MeshData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>

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

namespace Implementation {
    enum: UnsignedShort { MeshAttributeCustom = 32768 };
}

/**
@brief Mesh attribute name
@m_since{2020,06}

See @ref MeshData for more information.

Apart from the builtin attribute names it's possible to have custom ones,
which use the upper half of the enum range. Those are detected via
@ref isMeshAttributeCustom() and can be converted to and from a numeric
identifier using @ref meshAttributeCustom(MeshAttribute) and
@ref meshAttributeCustom(UnsignedShort). Unlike the builtin ones, these can
be of any type. An importer that exposes custom attributes then may also
provide a string mapping using @ref AbstractImporter::meshAttributeForName()
and @ref AbstractImporter::meshAttributeName(); conversely a scene converter
supporting custom mesh attributes can have the string mapping specified via
@ref AbstractSceneConverter::setMeshAttributeName(). See documentation of a
particular importer and scene converter for details.
@see @ref MeshAttributeData, @ref VertexFormat
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
     * Corresponds to @ref Shaders::GenericGL::Position.
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
     * tangent basis. Reconstructing the @ref MeshAttribute::Bitangent can be
     * then done like this:
     *
     * @snippet Trade.cpp MeshAttribute-bitangent-from-tangent
     *
     * When used as a morph target attribute, the handedness shouldn't change
     * compared to the base attribute. It's not checked or enforced in any way
     * though.
     *
     * Corresponds to @ref Shaders::GenericGL::Tangent or
     * @ref Shaders::GenericGL::Tangent4.
     * @see @ref MeshData::tangentsAsArray(),
     *      @ref MeshData::bitangentSignsAsArray()
     */
    Tangent,

    /**
     * Bitangent. Type is usually @ref VertexFormat::Vector3, but can be also
     * @ref VertexFormat::Vector3h, @ref VertexFormat::Vector3bNormalized or
     * @ref VertexFormat::Vector3sNormalized. For better storage efficiency,
     * the bitangent can be also reconstructed from the normal and tangent, see
     * @ref MeshAttribute::Tangent for more information.
     *
     * When used as a morph target attribute, the handedness shouldn't change
     * compared to the base attribute. It's not checked or enforced in any way
     * though.
     *
     * Corresponds to @ref Shaders::GenericGL::Bitangent.
     * @see @ref MeshData::bitangentsAsArray()
     */
    Bitangent,

    /**
     * Normal. Type is usually @ref VertexFormat::Vector3, but can be also
     * @ref VertexFormat::Vector3h, @ref VertexFormat::Vector3bNormalized or
     * @ref VertexFormat::Vector3sNormalized. Corresponds to
     * @ref Shaders::GenericGL::Normal.
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
     * Corresponds to @ref Shaders::GenericGL::TextureCoordinates.
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
     * @ref Shaders::GenericGL::Color3 or @ref Shaders::GenericGL::Color4.
     * @see @ref MeshData::colorsAsArray()
     */
    Color,

    /**
     * Skin joint IDs. Array attribute, type is usually
     * @ref VertexFormat::UnsignedInt, but can be also
     * @ref VertexFormat::UnsignedShort or
     * @ref VertexFormat::UnsignedByte. Array size is not limited or enforced
     * in any way, but shaders usually expect at most 8 items, or alternatively
     * two instances of this attribute with at most 4 items in each.
     *
     * Count of instances of this attribute and array size of each instance is
     * expected to match instance count and array sizes of
     * @ref MeshAttribute::Weights. This attribute isn't allowed to be a morph
     * target.
     *
     * Corresponds to @ref Shaders::GenericGL::JointIds and
     * @ref Shaders::GenericGL::SecondaryJointIds, divided between them based
     * on array size or attribute count.
     * @m_since_latest
     * @see @ref MeshData::jointIdsAsArray()
     */
    JointIds,

    /**
     * Skin weights. Array attribute, type is usually @ref VertexFormat::Float,
     * but can be also @ref VertexFormat::Half,
     * @ref VertexFormat::UnsignedByteNormalized or
     * @ref VertexFormat::UnsignedShortNormalized. Array size is not limited or
     * enforced in any way, but shaders usually expect at most 8 items, or
     * alternatively two instances of this attribute with at most 4 items in
     * each.
     *
     * Count of instances of this attribute and array size of each instance is
     * expected to match instance count and array sizes of
     * @ref MeshAttribute::JointIds. This attribute isn't allowed to be a morph
     * target.
     *
     * Corresponds to @ref Shaders::GenericGL::Weights and
     * @ref Shaders::GenericGL::SecondaryWeights, divided between them based
     * on array size or attribute count.
     * @m_since_latest
     * @see @ref MeshData::weightsAsArray()
     */
    Weights,

    /**
     * (Instanced) object ID for editor selection or scene annotation. Type is
     * usually @ref VertexFormat::UnsignedInt, but can be also
     * @ref VertexFormat::UnsignedShort or @ref VertexFormat::UnsignedByte.
     *
     * This attribute isn't allowed to be a morph target.
     *
     * Corresponds to @ref Shaders::GenericGL::ObjectId.
     * @see @ref MeshData::objectIdsAsArray()
     */
    ObjectId
};

/**
@debugoperatorenum{MeshAttribute}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MeshAttribute value);

/**
@brief Whether a mesh attribute is custom
@m_since{2020,06}

Returns @cpp true @ce if @p name has a value in the upper 15 bits of the enum
range, @cpp false @ce otherwise.
@see @ref meshAttributeCustom(UnsignedShort),
    @ref meshAttributeCustom(MeshAttribute),
    @ref AbstractImporter::meshAttributeName()
*/
constexpr bool isMeshAttributeCustom(MeshAttribute name) {
    return UnsignedShort(name) >= Implementation::MeshAttributeCustom;
}

/**
@brief Create a custom mesh attribute
@m_since{2020,06}

Returns a custom mesh attribute with index @p id. The index is expected to fit
into 15 bits. Use @ref meshAttributeCustom(MeshAttribute) to get the index
back.
*/
/* Constexpr so it's usable for creating compile-time MeshAttributeData
   instances */
constexpr MeshAttribute meshAttributeCustom(UnsignedShort id) {
    return CORRADE_CONSTEXPR_ASSERT(id < Implementation::MeshAttributeCustom,
        "Trade::meshAttributeCustom(): index" << id << "too large"),
        MeshAttribute(Implementation::MeshAttributeCustom + id);
}

/**
@brief Get index of a custom mesh attribute
@m_since{2020,06}

Inverse to @ref meshAttributeCustom(UnsignedShort). Expects that the attribute
is custom.
@see @ref isMeshAttributeCustom(), @ref AbstractImporter::meshAttributeName()
*/
constexpr UnsignedShort meshAttributeCustom(MeshAttribute name) {
    return CORRADE_CONSTEXPR_ASSERT(isMeshAttributeCustom(name),
        "Trade::meshAttributeCustom():" << name << "is not custom"),
        UnsignedShort(name) - Implementation::MeshAttributeCustom;
}

/**
@brief Mesh index data
@m_since{2020,06}

Convenience type for populating @ref MeshData, see its documentation for an
introduction.

@section Trade-MeshIndexData-usage Usage

The most straightforward usage is constructing the instance from a view on the
index array. The @ref MeshIndexType gets inferred from the view type:

@snippet Trade.cpp MeshIndexData-usage

Alternatively, you can pass a typeless @cpp const void @ce view and supply
@ref MeshIndexType explicitly, or a contiguous 2D view and let the class detect
the actual index type from second dimension size.
*/
class MAGNUM_TRADE_EXPORT MeshIndexData {
    public:
        /** @brief Construct for a non-indexed mesh */
        explicit MeshIndexData(std::nullptr_t = nullptr) noexcept: _type{} {}

        /**
         * @brief Construct a contiguous index array with a runtime-specified index type
         * @param type      Index type
         * @param data      Index data
         *
         * This overload is picked over @ref MeshIndexData(MeshIndexType, Containers::StridedArrayView1D<const void>)
         * if @p data is convertible to a contiguous view. The @p data size is
         * then expected to correspond to given @p type (e.g., for
         * @ref MeshIndexType::UnsignedInt the @p data array size should be
         * divisible by 4). If you know the @p type at compile time, you can
         * use one of the @ref MeshIndexData(Containers::StridedArrayView1D<const UnsignedByte>),
         * @ref MeshIndexData(Containers::StridedArrayView1D<const UnsignedShort>) or
         * @ref MeshIndexData(Containers::StridedArrayView1D<const UnsignedInt>)
         * constructors, which infer the index type automatically.
         *
         * If @p data is empty, the mesh will be treated as indexed but with
         * zero indices. To create a non-indexed mesh, use the
         * @ref MeshIndexData(std::nullptr_t) constructor.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class T, typename std::enable_if<std::is_convertible<T, Containers::ArrayView<const void>>::value, int>::type = 0> explicit MeshIndexData(MeshIndexType type, T&& data) noexcept;
        #else
        explicit MeshIndexData(MeshIndexType type, Containers::ArrayView<const void> data) noexcept;
        #endif

        /**
         * @brief Construct a strided index array with a runtime-specified index type
         * @param type      Index type
         * @param data      Index data
         * @m_since_latest
         *
         * If you know the @p type at compile time, you can use one of the
         * @ref MeshIndexData(Containers::StridedArrayView1D<const UnsignedByte>),
         * @ref MeshIndexData(Containers::StridedArrayView1D<const UnsignedShort>) or
         * @ref MeshIndexData(Containers::StridedArrayView1D<const UnsignedInt>)
         * constructors, which infer the index type automatically. The view
         * doesn't need to be contiguous and the stride can be even zero or
         * negative, but note that such data layout is not commonly supported
         * by GPU APIs.
         *
         * If @p data is empty, the mesh will be treated as indexed but with
         * zero indices. To create a non-indexed mesh, use the
         * @ref MeshIndexData(std::nullptr_t) constructor.
         */
        constexpr explicit MeshIndexData(MeshIndexType type, Containers::StridedArrayView1D<const void> data) noexcept;

        /**
         * @brief Construct with unsigned byte indices
         *
         * The view doesn't need to be contiguous and the stride can be even
         * zero or negative, but note that such data layout is not commonly
         * supported by GPU APIs.
         */
        constexpr explicit MeshIndexData(Containers::StridedArrayView1D<const UnsignedByte> data) noexcept: MeshIndexData{MeshIndexType::UnsignedByte, data} {}

        /**
         * @brief Construct with unsigned short indices
         *
         * The view doesn't need to be contiguous and the stride can be even
         * zero or negative, but note that such data layout is not commonly
         * supported by GPU APIs.
         */
        constexpr explicit MeshIndexData(Containers::StridedArrayView1D<const UnsignedShort> data) noexcept: MeshIndexData{MeshIndexType::UnsignedShort, data} {}

        /**
         * @brief Construct with unsigned int indices
         *
         * The view doesn't need to be contiguous and the stride can be even
         * zero or negative, but note that such data layout is not commonly
         * supported by GPU APIs.
         */
        constexpr explicit MeshIndexData(Containers::StridedArrayView1D<const UnsignedInt> data) noexcept: MeshIndexData{MeshIndexType::UnsignedInt, data} {}

        /**
         * @brief Constructor
         *
         * Expects that the second dimension of @p data is contiguous and its
         * size is either 1, 2 or 4, corresponding to one of the
         * @ref MeshIndexType values. The first dimension doesn't need to be
         * contiguous and its stride can be even zero or negative, but note
         * that such data layout is not commonly supported by GPU APIs. As a
         * special case, if second dimension size is 0, the constructor is
         * equivalent to @ref MeshIndexData(std::nullptr_t).
         */
        explicit MeshIndexData(const Containers::StridedArrayView2D<const char>& data) noexcept;

        /** @brief Index type */
        constexpr MeshIndexType type() const { return _type; }

        /**
         * @brief Type-erased index data
         *
         * In rare cases the stride may be different from the index type size
         * and even be zero or negative, such data layouts are however not
         * commonly supported by GPU APIs.
         */
        constexpr Containers::StridedArrayView1D<const void> data() const { return _data; }

    private:
        friend MeshData;
        MeshIndexType _type;
        /* Void so the constructors can be constexpr */
        Containers::StridedArrayView1D<const void> _data;
};

/**
@brief Mesh attribute data
@m_since{2020,06}

Convenience type for populating @ref MeshData, see
@ref Trade-MeshData-populating "its documentation" for an introduction.
Additionally usable in various @ref MeshTools algorithms such as
@ref MeshTools::duplicate(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttributeData>)
or @ref MeshTools::interleave(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags).

@section Trade-MeshAttributeData-usage Usage

The most straightforward usage is constructing an instance from a pair of a
@ref MeshAttribute and a strided view. The @ref VertexFormat gets inferred from
the view type:

@snippet Trade.cpp MeshAttributeData-usage

Alternatively, you can pass a typeless @cpp const void @ce or a 2D view and
supply @ref VertexFormat explicitly.

@subsection Trade-MeshAttributeData-usage-offset-only Offset-only attribute data

If the actual attribute data location is not known yet, the instance can be
created as "offset-only" using @ref MeshAttributeData(MeshAttribute, VertexFormat, std::size_t, UnsignedInt, std::ptrdiff_t, UnsignedShort, Int),
meaning the actual view gets created only later when passed to a @ref MeshData
instance with a concrete vertex data array. This is useful mainly to avoid
pointer patching during data serialization, but also for example when vertex
layout is static (and thus can be defined at compile time), but the actual data
is allocated / populated at runtime.

@snippet Trade.cpp MeshAttributeData-usage-offset-only

See @ref Trade-MeshData-populating-non-owned "the corresponding MeshData documentation"
for a complete usage example. Offset-only attributes return @cpp true @ce for
@ref isOffsetOnly(). Note that @ref MeshTools algorithms generally don't accept
offset-only @ref MeshAttributeData instances except when passed through a
@ref MeshData, as for a standalone offset-only @ref MeshAttributeData it's
impossible to know what data it points to.

@section Trade-MeshAttributeData-custom-vertex-format Custom vertex formats

Apart from custom @ref MeshAttribute names, shown in
@ref Trade-MeshData-populating-custom, @ref VertexFormat can be extended with
implementation-specific formats as well. Formats that don't have a generic
@ref VertexFormat equivalent can be created using @ref vertexFormatWrap(),
however note that most APIs and @ref MeshTools functions can't work with those
as their size or contents can't be known:

@snippet Trade.cpp MeshAttributeData-custom-vertex-format

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
        constexpr explicit MeshAttributeData() noexcept: _format{}, _name{}, _isOffsetOnly{false}, _morphTargetId{-1}, _vertexCount{}, _stride{}, _arraySize{}, _data{} {}

        /**
         * @brief Type-erased constructor
         * @param name              Attribute name
         * @param format            Vertex format
         * @param data              Attribute data
         * @param arraySize         Array size. Use @cpp 0 @ce for non-array
         *      attributes.
         * @param morphTargetId     Morph target ID. Use @cpp -1 @ce for
         *      attributes that are not morph targets.
         *
         * Expects that @p data stride fits into a signed 16-bit value, that
         * vertex count fits into 32 bits, and for builtin attributes that
         * @p format corresponds to @p name, @p arraySize is either zero for
         * non-array attributes or non-zero for array attributes. The
         * @p morphTargetId is expected to be greater or equal to @cpp -1 @ce
         * and less than @cpp 128 @ce and has to be @cpp -1 @ce for builtin
         * attributes that can't be morph targets. The stride can be zero or
         * negative, but note that such data layouts are not commonly supported
         * by GPU APIs.
         */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<const void>& data, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept;
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* These two are here to direct StridedArrayView1D<[const ]char> to the
           1D overload, because that's likely what one wants. The 2D conversion
           adds a dimension at the front, which would make the attribute have
           just a single vertex with likely a sparse second dimension, causing
           an immediate assert. */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<char>& data, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept: MeshAttributeData{name, format, Containers::StridedArrayView1D<const void>{data}, arraySize, morphTargetId} {}
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<const char>& data, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept: MeshAttributeData{name, format, Containers::StridedArrayView1D<const void>{data}, arraySize, morphTargetId} {}
        #endif

        /**
         * @brief Constructor
         * @param name              Attribute name
         * @param format            Vertex format
         * @param data              Attribute data
         * @param arraySize         Array size. Use @cpp 0 @ce for non-array
         *      attributes.
         * @param morphTargetId     Morph target ID. Use @cpp -1 @ce for
         *      attributes that are not morph targets.
         *
         * Expects that the second dimension of @p data is contiguous and its
         * size matches @p format and @p arraySize, and for builtin attributes
         * that @p format corresponds to @p name and @p arraySize is either
         * zero for non-array attributes or non-zero for array attributes. The
         * @p morphTargetId is expected to be greater or equal to @cpp -1 @ce
         * and less than @cpp 128 @ce and has to be @cpp -1 @ce for builtin
         * attributes that can't be morph targets. The stride is expected to
         * fit into a signed 16-bit value. It can be zero or negative, but note
         * that such data layouts are not commonly supported by GPU APIs.
         */
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, const Containers::StridedArrayView2D<const char>& data, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept;

        /** @overload */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit MeshAttributeData(MeshAttribute name, VertexFormat format, std::nullptr_t, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept;
        #else
        /* Extra template crap is needed to avoid ambiguity with the
           offset-only constructor (where 0 passed to offset would match with
           std::nullptr_t). 0 as null pointer constant was deprecated in C++11
           already, WHY IS THIS STILL A PROBLEM?! */
        template<class U, typename std::enable_if<std::is_convertible<U, std::nullptr_t>::value && !std::is_convertible<U, std::size_t>::value, int>::type = 0> explicit MeshAttributeData(MeshAttribute name, VertexFormat format, U, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept: MeshAttributeData{nullptr, name, format, nullptr, arraySize, morphTargetId} {}
        #endif

        /**
         * @brief Constructor
         * @param name              Attribute name
         * @param data              Attribute data
         * @param morphTargetId     Morph target ID. Use @cpp -1 @ce for
         *      attributes that are not morph targets.
         *
         * Detects @ref VertexFormat based on @p T and calls
         * @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&, UnsignedShort, Int).
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
        template<class T> constexpr explicit MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView1D<T>& data, Int morphTargetId = -1) noexcept;

        /** @overload */
        template<class T> constexpr explicit MeshAttributeData(MeshAttribute name, const Containers::ArrayView<T>& data, Int morphTargetId = -1) noexcept: MeshAttributeData{name, Containers::stridedArrayView(data), morphTargetId} {}

        /**
         * @brief Construct an array attribute
         * @param name              Attribute name
         * @param data              Attribute data
         * @param morphTargetId     Morph target ID. Use @cpp -1 @ce for
         *      attributes that are not morph targets.
         *
         * Detects @ref VertexFormat based on @p T and calls
         * @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&, UnsignedShort, Int)
         * with the second dimension size passed to @p arraySize. Expects that
         * the second dimension is contiguous, and if @p name is a builtin
         * attribute, it's an array attribute. See @ref MeshAttributeData(MeshAttribute, const Containers::StridedArrayView1D<T>&, Int)
         * for details about @ref VertexFormat detection.
         */
        template<class T> constexpr explicit MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView2D<T>& data, Int morphTargetId = -1) noexcept;

        /**
         * @brief Construct an offset-only attribute
         * @param name              Attribute name
         * @param format            Attribute format
         * @param offset            Attribute data offset
         * @param vertexCount       Attribute vertex count
         * @param stride            Attribute stride
         * @param arraySize         Array size. Use @cpp 0 @ce for non-array
         *      attributes.
         * @param morphTargetId     Morph target ID. Use @cpp -1 @ce for
         *      attributes that are not morph targets.
         *
         * Instances created this way refer to an offset in unspecified
         * external vertex data instead of containing the data view directly.
         * Useful when the location of the vertex data array is not known at
         * attribute construction time. Note that instances created this way
         * can't be used in most @ref MeshTools algorithms.
         *
         * For builtin attributes expects that @p arraySize is zero for
         * non-array attributes and non-zero for array attributes. The
         * @p morphTargetId is expected to be greater or equal to @cpp -1 @ce
         * and less than @cpp 128 @ce and has to be @cpp -1 @ce for builtin
         * attributes that can't be morph targets. The @p stride is expected to
         * fit into a signed 16-bit value. It can be zero or negative, but note
         * that such data layouts are not commonly supported by GPU APIs.
         *
         * Additionally, for even more flexibility, the @p vertexCount can be
         * overridden at @ref MeshData construction time, however all attributes
         * are still required to have the same vertex count to catch accidents.
         *
         * @see @ref isOffsetOnly(), @ref arraySize(),
         *      @ref data(Containers::ArrayView<const void>) const
         */
        explicit constexpr MeshAttributeData(MeshAttribute name, VertexFormat format, std::size_t offset, UnsignedInt vertexCount, std::ptrdiff_t stride, UnsignedShort arraySize = 0, Int morphTargetId = -1) noexcept;

        /**
         * @brief Construct a pad value
         *
         * Usable in various @ref MeshTools algorithms to insert padding
         * between interleaved attributes. Negative values can be used to alias
         * multiple different attributes onto each other. Not meant to be
         * passed to @ref MeshData.
         * @see @ref stride()
         */
        constexpr explicit MeshAttributeData(Int padding): _format{}, _name{}, _isOffsetOnly{false}, _morphTargetId{-1}, _vertexCount{0}, _stride{
            (CORRADE_CONSTEXPR_ASSERT(padding >= -32768 && padding <= 32767,
                "Trade::MeshAttributeData: expected padding to fit into 16 bits but got" << padding), Short(padding))
        }, _arraySize{}, _data{nullptr} {}

        /**
         * @brief If the attribute is offset-only
         *
         * Returns @cpp true @ce if the attribute doesn't contain the data view
         * directly, but instead refers to unspecified external vertex data.
         * @see @ref data(Containers::ArrayView<const void>) const,
         *      @ref MeshAttributeData(MeshAttribute, VertexFormat, std::size_t, UnsignedInt, std::ptrdiff_t, UnsignedShort, Int)
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
         * In rare cases the stride may be zero or negative, such data layouts
         * are however not commonly supported by GPU APIs.
         * @see @ref MeshAttributeData(Int)
         */
        constexpr Short stride() const { return _stride; }

        /**
         * @brief Attribute array size
         *
         * Returns @cpp 0 @ce if the attribute isn't an array.
         */
        constexpr UnsignedShort arraySize() const { return _arraySize; }

        /**
         * @brief Morph target ID
         * @m_since_latest
         *
         * Returns @cpp -1 @ce if the attribute isn't a morph target.
         */
        constexpr Int morphTargetId() const { return _morphTargetId; }

        /**
         * @brief Type-erased attribute data
         *
         * Expects that the attribute is not offset-only, in that case use the
         * @ref data(Containers::ArrayView<const void>) const overload instead.
         * In rare cases the stride of the returned view may be zero or
         * negative, such data layouts are however not commonly supported by
         * GPU APIs.
         * @see @ref isOffsetOnly()
         */
        constexpr Containers::StridedArrayView1D<const void> data() const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                {_data.pointer, ~std::size_t{}}, _vertexCount,
                (CORRADE_CONSTEXPR_ASSERT(!_isOffsetOnly, "Trade::MeshAttributeData::data(): the attribute is offset-only, supply a data array"), _stride)};
        }

        /**
         * @brief Type-erased attribute data for an offset-only attribute
         *
         * If the attribute is not offset-only, the @p vertexData parameter is
         * ignored. In rare cases the stride of the returned view may be zero
         * or negative, such data layouts are however not commonly supported by
         * GPU APIs.
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

        /* Delegated to by all ArrayView constructors, which additionally check
           either stride or second dimension size. Nullptr first, to avoid
           accidental matches as much as possible. */
        constexpr explicit MeshAttributeData(std::nullptr_t, MeshAttribute name, VertexFormat format, const Containers::StridedArrayView1D<const void>& data, UnsignedShort arraySize, Int morphTargetId) noexcept;

        VertexFormat _format;
        MeshAttribute _name;
        bool _isOffsetOnly;
        /* glTF spec says the expected number of morph targets is ~8, so 128
           should be enough. Signed in order to use -1 as "not a morph target",
           if 128 wouldn't be enough then this could get changed to unsigned
           and interpreting only 255 as -1. */
        Byte _morphTargetId;

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
want to allocate just to pass those to @ref MeshData. See documentation about
@ref Trade-MeshData-populating "populating a MeshData instance" for more
information.
*/
Containers::Array<MeshAttributeData> MAGNUM_TRADE_EXPORT meshAttributeDataNonOwningArray(Containers::ArrayView<const MeshAttributeData> view);

/**
@brief Mesh data
@m_since{2020,06}

Provides access to mesh vertex and index data, together with additional
information such as primitive type. Populated instances of this class are
returned from @ref AbstractImporter::mesh(), from particular functions in
the @ref Primitives library, can be passed to
@ref AbstractSceneConverter::convert(const MeshData&),
@ref AbstractSceneConverter::add(const MeshData&, Containers::StringView) and
related APIs, as well as used in various @ref MeshTools algorithms. Like with
other @ref Trade types, the internal representation is fixed upon construction
and allows only optional in-place modification of the index and vertex data
itself, but not of the overall structure.

@section Trade-MeshData-gpu-opengl Populating an OpenGL mesh

If the goal is creating a @ref GL::Mesh instance to be rendered by builtin
shaders, the most straightforward way is to use @ref MeshTools::compile(). This
one-liner uploads the data and configures the mesh for all builtin attributes
listed in the @ref MeshAttribute enum that are present in it:

@snippet Trade.cpp MeshData-gpu-opengl

This works also with any custom shader that follows the attribute binding
defined in @ref Shaders::GenericGL.

@m_class{m-note m-success}

@par
    A generic mesh setup using the high-level utility is used in the
    @ref examples-primitives and @ref examples-viewer examples.

@section Trade-MeshData-gpu-direct Setting GPU mesh properties directly

If you need more control, for example in presence of custom attributes, if
you have a custom shader that doesn't follow the attribute binding defined in
@ref Shaders::GenericGL, or if you want to use the mesh with a 3rd party
renderer, you can access the index and attribute data and properties directly.
The @ref MeshData class internally stores a contiguous blob of data, which you
can directly upload, and then use the associated metadata to let the GPU know
of the format and layout. The following is again creating an OpenGL mesh, but a
similar workflow would be for Vulkan or any other GPU API:

@snippet Trade.cpp MeshData-gpu-opengl-direct

If using a shader that follows the @ref Shaders::GenericGL attribute binding
and only has some custom attributes on top, you can use
@ref MeshTools::compile(const Trade::MeshData&, GL::Buffer&, GL::Buffer&)
for a combined way that gives you both the flexibility needed for custom
attributes as well as the convenience for builtin attributes. See its
documentation for an example.

@section Trade-MeshData-access Accessing mesh data

When access to individual attributes from the CPU side is desired, for example
to inspect the topology or to pass the data to a physics simulation, the
simplest way is accessing attributes through the convenience
@ref indicesAsArray(), @ref positions3DAsArray(), @ref normalsAsArray() etc.
functions. Unless the mesh has a known layout, you're expected to check for
index buffer and attribute presence first with @ref isIndexed() and
@ref hasAttribute() (or, in case you need to access for example secondary
texture coordinates, @ref attributeCount(MeshAttribute, Int) const). Apart from
that, the convenience functions abstract away the internal layout of the mesh
data, giving you always a contiguous array in a predictable type.

@snippet Trade.cpp MeshData-access

If allocation is undesirable, the @ref indicesInto(), @ref positions3DInto()
etc. variants take a target view where to put the output instead of returning a
newly created array. The most efficient way is without copies or conversions
however, by direct accessing the index and attribute data using @ref indices()
and @ref attribute(). In that case you additionally need to be sure about the
data types used or decide based on @ref indexType() and @ref attributeFormat().
Replacing the above with with direct data access would look like this:

@snippet Trade.cpp MeshData-access-direct

There are also non-templated type-erased @ref indices() and @ref attribute()
overloads returning @cpp void @ce views, useful for example when a dispatch
based on the actual type is deferred to an external function. Compared to using
the template versions you however lose the type safety checks implemented in
@ref MeshData itself.

@m_class{m-note m-success}

@par
    If you're loading a mesh from a file, the @ref magnum-sceneconverter "magnum-sceneconverter"
    command-line utility can be used to conveniently inspect its layout and
    used data formats before writing a code that processes it.

@subsection Trade-MeshData-access-mutable Mutable data access

In a general case, mesh index and vertex data can also refer to a memory-mapped
file or constant memory and thus @ref indices() and @ref attribute() return
@cpp const @ce views. When it's desirable to modify the data in-place, there's
the @ref mutableIndexData(), @ref mutableVertexData(), @ref mutableIndices()
and @ref mutableAttribute() set of functions. To use these, you need to
additionally check that the data are mutable using @ref indexDataFlags() or
@ref vertexDataFlags() first. Further continuing from the above, this snippet
applies a transformation to the mesh positions in-place:

@snippet Trade.cpp MeshData-access-mutable

<b></b>

@m_class{m-note m-success}

@par
    @ref MeshTools::transform3D() and other APIs in the @ref MeshTools
    namespace provide a broad set of utilities for mesh transformation,
    filtering and merging, operating on both whole @ref MeshData instances and
    individual data views.

@subsection Trade-MeshData-access-morph-targets Morph targets

By default, named attribute access (either through the @ref positions3DAsArray()
etc. convenience accesors or via @ref attribute(MeshAttribute, UnsignedInt, Int) const "attribute()"
and similar) searches only through the base attributes. Meshes that have morph
targets can have the additional attributes accessed by passing a
`morphTargetId` argument to these functions:

@snippet Trade.cpp MeshData-access-morph-targets

If a base attribute doesn't have a corresponding morph target attribute (which
can be checked using @ref hasAttribute(MeshAttribute, Int) const with
appropriate `morphTargetId` passed), the base attribute is meant to be used
unchanged. Base attributes with multiple sets can have multiple sets of morph
target attributes as well (which can be again checked using
@ref attributeCount(MeshAttribute, Int) const with appropriate `morphTargetId`
passed). If only some instances from the set have a morph target, the remaining
attributes are expected to alias the base ones (i.e., have the same
@ref attributeOffset(), @ref attributeStride() and @ref attributeArraySize())
in order to match their numbering. Finally, there can attributes that are only
defined among morph targets but have no corresponding base attribute. This
isn't restricted in any way and their treatment is left to be
application-specific.

@section Trade-MeshData-special-layouts Special data layouts

The class is able to represent data layouts beyond what's supported by common
GPU vertex pipelines, in particular:

-   attributes with zero stride (a single value repeated for all vertices),
-   attributes with negative stride (causing the attribute data to be read in
    reverse order),
-   indices in a non-contiguous array (strided, similar to attributes).

These are allowed in order to support certain special cases where it would
otherwise be needed to perform a manual and potentially expensive data
repacking operation before putting them in a @ref MeshData. Unless explicitly
stated otherwise, all Magnum APIs *returning* a @ref MeshData (such as the
@ref Primitives library or various importer plugins) don't make use of those
advanced data layout features; and conversely all Magnum APIs *taking* a
@ref MeshData are aware of such features and can handle them approriately.

When passing mesh data to the GPU, the @ref MeshTools::compile() utility will
check and expect that only GPU-compatible layout features are used. However,
when configuring meshes directly like shown in the
@ref Trade-MeshData-gpu-direct chapter above, you may want to check the
constraints explicitly before passing the values over.

@snippet Trade.cpp MeshData-special-layouts

In order to convert a mesh with a special data layout to something the GPU
vertex pipeline is able to consume, @ref MeshTools::interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "MeshTools::interleave()"
can be used. If you pass neither @ref MeshTools::InterleaveFlag::PreserveInterleavedAttributes
nor @ref MeshTools::InterleaveFlag::PreserveStridedIndices, it will interleave
all attributes together, regardless of what stride they had originally, and
repack the indices into a contiguous buffer as well. Note that, however, it
won't be able to work with a custom @ref MeshIndexType or @ref VertexFormat. In
that case, it's your responsibility to know what's actually in the mesh data
and how to handle it.

@section Trade-MeshData-populating Populating an instance

If the goal is creating a @ref MeshData instance from individual attributes as
opposed to getting it from an importer or as an output of another operation and
making a copy of the data is acceptable, easiest is to pass them to the
@ref MeshTools::interleave(MeshPrimitive, const Trade::MeshIndexData&, Containers::ArrayView<const Trade::MeshAttributeData>) "MeshTools::interleave()"
helper and letting it handle all data massaging internally. See its
documentation for a usage example.

Otherwise, creating a @ref MeshData instance from scratch requires you to have
all vertex data contained in a single chunk of memory. By default it takes over
the ownership of an @relativeref{Corrade,Containers::Array} containing the
vertex / index data together with a @ref MeshIndexData instance and a list of
@ref MeshAttributeData describing various index and vertex properties. For
example, an interleaved indexed mesh with 3D positions and RGBA colors would
look like this --- and variants with just vertex data or just index data or
neither are possible too:

@snippet Trade.cpp MeshData-populating

@subsection Trade-MeshData-populating-non-owned Non-owned instances and static vertex layouts

In some cases you may want the @ref MeshData instance to only refer to external
data without taking ownership, for example with a memory-mapped file, global
data etc. For that, instead of moving in
@relativeref{Corrade,Containers::Array} instances, pass @ref DataFlags
describing data mutability and ownership together with
@relativeref{Corrade,Containers::ArrayView} instances to the
@ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
constructor. The following snippet is a variant of the above where the index
data is constant and vertex data mutable, both referenced externally:

@snippet Trade.cpp MeshData-populating-non-owned

There are also other constructor overloads allowing you to mix and match owned
vertex data with non-owned index data and vice versa. The @ref MeshAttributeData
list is still implicitly allocated in the above case, but it can also be
defined externally and referenced via @ref meshAttributeDataNonOwningArray()
instead if desired. Finally, if the vertex layout is constant but the actual
data is allocated / populated at runtime, the @ref MeshAttributeData instances
can be defined in a global array as offset-only:

@snippet Trade.cpp MeshData-populating-offset-only

See also the @ref Trade-MeshAttributeData-usage-offset-only "corresponding MeshAttributeData documentation for offset-only fields".

@subsection Trade-MeshData-populating-custom Custom mesh attributes

To allow for greater flexibility, a @ref MeshData instance can describe not
just attributes that are predefined in the @ref MeshAttribute enum, but also
custom attributes, created with @ref meshAttributeCustom(). For example, the
snippet below describes a custom per-face structure that exposes faces as
higher-order polygons combining multiple triangles together ---in this case,
each face has an array of 15 IDs, which is exposed as a 2D array:

@snippet Trade.cpp MeshData-populating-custom

Later, the (array) attributes can be retrieved back using the same custom
identifiers --- note the use of @cpp [] @ce to get back a 2D array again:

@snippet Trade.cpp MeshData-populating-custom-retrieve

When a custom attribute is exposed through @ref AbstractImporter, it's possible
to map custom @ref MeshAttribute values to human-readable string names using
@ref AbstractImporter::meshAttributeName() and
@ref AbstractImporter::meshAttributeForName(). Using @ref meshPrimitiveWrap()
you can also supply implementation-specific values that are not available in
the generic @relativeref{Magnum,MeshPrimitive} enum, similarly see also
@ref Trade-MeshAttributeData-custom-vertex-format for details on
implementation-specific @ref VertexFormat values.
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
         * For a non-indexed mesh either pass default-constructed
         * @ref indexData and @p indices arguments, or use the
         * @ref MeshData(MeshPrimitive, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * constructor.
         *
         * The @p attributes are expected to reference (sparse) sub-ranges of
         * @p vertexData. Particular attributes can have additional
         * restrictions, see documentation of @ref MeshAttribute values for
         * more information. If the mesh has no attributes, the @p indices are
         * expected to be valid (but can be empty), you can also use the
         * @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, UnsignedInt, const void*)
         * constructor in that case. If you want to create an attribute-less
         * non-indexed mesh, use @ref MeshData(MeshPrimitive, UnsignedInt, const void*)
         * instead.
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
         * modified, and are expected to *not* have @ref DataFlag::Owned set.
         *
         * Use @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, Containers::Array<char>&&, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * to create a mesh with only index data non-owned and
         * @ref MeshData(MeshPrimitive, Containers::Array<char>&&, const MeshIndexData&, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * to create a mesh with only vertex data non-owned. There's also a
         * @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
         * convenience overload for non-indexed meshes with non-owned vertex
         * data and @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, UnsignedInt, const void*)
         * for attribute-less meshes with non-owned index data.
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
         * @see @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
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
         * @see @ref MeshData(MeshPrimitive, DataFlags, Containers::ArrayView<const void>, const MeshIndexData&, DataFlags, Containers::ArrayView<const void>, Containers::Array<MeshAttributeData>&&, UnsignedInt, const void*)
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
         * @ref MeshData(MeshPrimitive, UnsignedInt, const void*) instead.
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

        /** @brief Copying is not allowed */
        MeshData(const MeshData&) = delete;

        /** @brief Move constructor */
        MeshData(MeshData&&) noexcept;

        ~MeshData();

        /** @brief Copying is not allowed */
        MeshData& operator=(const MeshData&) = delete;

        /** @brief Move assignment */
        MeshData& operator=(MeshData&&) noexcept;

        /** @brief Primitive */
        MeshPrimitive primitive() const { return _primitive; }

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
         * @brief Index stride
         * @m_since_latest
         *
         * Stride between consecutive elements in the @ref indexData() array.
         * In rare cases the stride may be different from the index type size
         * and even be zero or negative, such data layouts are however not
         * commonly supported by GPU APIs.
         * @see @ref MeshTools::isInterleaved()
         */
        Short indexStride() const;

        /**
         * @brief Indices
         *
         * For an indexed mesh, the second dimension represent the actual data
         * type (its size is equal to type size for known @ref MeshIndexType
         * values, and to *absolute* @ref indexStride() for
         * implementation-specific values), even in case there's zero indices,
         * and is guaranteed to be contiguous. For a non-indexed mesh, the
         * returned view has a zero size in both dimensions. In rare cases the
         * first dimension stride may be different from the index type size and
         * even be zero or negative, such data layouts are however not commonly
         * supported by GPU APIs.
         *
         * Use the templated overload below to get the indices in a concrete
         * type.
         * @see @relativeref{Corrade,Containers::StridedArrayView::isContiguous()}
         */
        Containers::StridedArrayView2D<const char> indices() const;

        /**
         * @brief Mutable indices
         *
         * Like @ref indices() const, but returns a mutable view. Expects that
         * the mesh is mutable.
         * @see @ref indexDataFlags()
         */
        Containers::StridedArrayView2D<char> mutableIndices();

        /**
         * @brief Indices in a concrete type
         *
         * Expects that the mesh is indexed and that @p T corresponds to
         * @ref indexType(). In rare cases the first dimension stride may be
         * different from the index type size and even be zero or negative,
         * such data layouts are however not commonly supported by GPU APIs.
         * You can also use the non-templated @ref indicesAsArray() accessor to
         * get indices converted to a contiguous 32-bit array, but note that
         * such operation involves extra allocation and data conversion.
         * @see @ref isIndexed(), @ref attribute(), @ref mutableIndices(),
         *      @relativeref{Corrade,Containers::StridedArrayView::isContiguous()}
         */
        template<class T> Containers::StridedArrayView1D<const T> indices() const;

        /**
         * @brief Mutable indices in a concrete type
         *
         * Like @ref indices() const, but returns a mutable view. Expects that
         * the mesh is mutable.
         * @see @ref indexDataFlags()
         */
        template<class T> Containers::StridedArrayView1D<T> mutableIndices();

        /**
         * @brief Vertex count
         *
         * Count of elements in every attribute array returned by
         * @ref attribute() (or, in case of an attribute-less mesh, the
         * desired vertex count). See also @ref indexCount() which returns
         * count of elements in the @ref indices() array, and
         * @ref attributeCount() which returns count of different per-vertex
         * attributes.
         * @see @ref MeshTools::primitiveCount(MeshPrimitive, UnsignedInt)
         */
        UnsignedInt vertexCount() const { return _vertexCount; }

        /**
         * @brief Total attribute count
         *
         * Count of all per-vertex attributes including extra attributes and
         * morph targets, or @cpp 0 @ce for an attribute-less mesh. See also
         * @ref indexCount() which returns count of elements in the
         * @ref indices() array and @ref vertexCount() which returns count of
         * elements in every @ref attribute().
         * @see @ref attributeCount(Int) const,
         *      @ref attributeCount(MeshAttribute, Int) const
         */
        UnsignedInt attributeCount() const { return UnsignedInt(_attributes.size()); }

        /**
         * @brief Attribute count for given morph target
         * @m_since_latest
         *
         * Count of attributes for which @ref attributeMorphTargetId() is equal
         * to @p morphTargetId, or @cpp 0 @ce if there's no such morph target.
         * Use @cpp -1 @ce to get the count of base attributes that aren't
         * morph targets. Total number of attributes in all morph targets can
         * be calculated by subtracting the value of this function with
         * @cpp -1 @ce from @ref attributeCount() const.
         * @see @ref attributeCount(MeshAttribute, Int) const
         */
        UnsignedInt attributeCount(Int morphTargetId) const;

        /**
         * @brief Raw attribute data
         *
         * Returns the raw data that are used as a base for all `attribute*()`
         * accessors. In most cases you don't want to access those directly,
         * but rather use the @ref attribute(), @ref attributeName(),
         * @ref attributeFormat(), @ref attributeOffset(),
         * @ref attributeStride() etc. accessors. This is also the reason why
         * there's no overload taking a @ref MeshAttribute, unlike the other
         * accessors.
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
         * @brief Attribute ID in a set of attributes of the same name and morph target ID
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * Returns the number of attributes of the same @ref attributeName()
         * and @ref attributeMorphTargetId() preceeding @p id, or @cpp 0 @ce if
         * it's the first attribute of given name and given morph target ID.
         * @see @ref attributeId(MeshAttribute, UnsignedInt, Int) const
         */
        UnsignedInt attributeId(UnsignedInt id) const;

        /**
         * @brief Attribute format
         *
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * You can also use @ref attributeFormat(MeshAttribute, UnsignedInt, Int) const
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
         * @ref attributeOffset(MeshAttribute, UnsignedInt, Int) const to
         * directly get an offset of given named attribute.
         * @see @ref indexOffset(), @ref MeshTools::isInterleaved()
         */
        std::size_t attributeOffset(UnsignedInt id) const;

        /**
         * @brief Attribute stride
         *
         * Stride between consecutive elements of given attribute in the
         * @ref vertexData() array. In rare cases the stride may be zero or
         * negative, such data layouts are however not commonly supported by
         * GPU APIs. The @p id is expected to be smaller than
         * @ref attributeCount() const. You can also use
         * @ref attributeStride(MeshAttribute, UnsignedInt, Int) const to
         * directly get a stride of given named attribute.
         * @see @ref MeshTools::isInterleaved()
         */
        Short attributeStride(UnsignedInt id) const;

        /**
         * @brief Attribute array size
         *
         * In case given attribute is an array (the equivalent of e.g.
         * @cpp int[30] @ce), returns array size, otherwise returns @cpp 0 @ce.
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * You can also use @ref attributeArraySize(MeshAttribute, UnsignedInt, Int) const
         * to directly get array size of given named attribute.
         *
         * Note that this is different from vertex count, which is exposed
         * through @ref vertexCount(), and is an orthogonal concept to having
         * multiple attributes of the same name (for example two sets of
         * texture coordinates), which is exposed through
         * @ref attributeCount(MeshAttribute, Int) const. See
         * @ref Trade-MeshData-populating-custom for an example.
         * @see @ref isMeshAttributeCustom()
         */
        UnsignedShort attributeArraySize(UnsignedInt id) const;

        /**
         * @brief Attribute morph target ID
         * @m_since_latest
         *
         * In case given attribute is a morph target, returns its ID, otherwise
         * returns @cpp -1 @ce. The @p id is expected to be smaller than
         * @ref attributeCount() const.
         */
        Int attributeMorphTargetId(UnsignedInt id) const;

        /**
         * @brief Whether the mesh has given attribute
         *
         * By default it checks only attributes that aren't morph targets, set
         * @p morphTargetId to check the attribute for given morph target ID.
         * @see @ref attributeCount(MeshAttribute, Int) const,
         *      @ref findAttributeId()
         */
        bool hasAttribute(MeshAttribute name, Int morphTargetId = -1) const {
            return attributeCount(name, morphTargetId);
        }

        /**
         * @brief Count of given named attribute
         *
         * Unlike @ref attributeCount() const this returns count for given
         * attribute name --- for example a mesh can have more than one set of
         * texture coordinates. By default it counts only attributes that
         * aren't morph targets, set @p morphTargetId to count attributes for
         * given morph target ID.
         * @see @ref hasAttribute(), @ref attributeCount(Int) const
         */
        UnsignedInt attributeCount(MeshAttribute name, Int morphTargetId = -1) const;

        /**
         * @brief Find an absolute ID of a named attribute
         * @m_since_latest
         *
         * If @p name isn't present or @p id is not smaller than
         * @ref attributeCount(MeshAttribute, Int) const, returns
         * @relativeref{Corrade,Containers::NullOpt}. The lookup is done in an
         * @f$ \mathcal{O}(n) @f$ complexity with @f$ n @f$ being the attribute
         * count.
         * @see @ref hasAttribute(), @ref attributeId()
         */
        Containers::Optional<UnsignedInt> findAttributeId(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Absolute ID of a named attribute
         *
         * Like @ref findAttributeId(), but the @p id is expected to be smaller
         * than @ref attributeCount(MeshAttribute, Int) const.
         * @see @ref attributeId(UnsignedInt) const
         */
        UnsignedInt attributeId(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Format of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute, Int) const.
         * @see @ref attributeFormat(UnsignedInt) const
         */
        VertexFormat attributeFormat(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Offset of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute, Int) const. See
         * @ref attributeOffset(UnsignedInt) const for more information.
         */
        std::size_t attributeOffset(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Stride of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute, Int) const. See
         * @ref attributeStride(UnsignedInt) const for more information.
         */
        Short attributeStride(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Array size of a named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute, Int) const. Note that this is
         * different from vertex count, and is an orthogonal concept to having
         * multiple attributes of the same name --- see
         * @ref attributeArraySize(UnsignedInt) const for more information.
         */
        UnsignedShort attributeArraySize(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /* No attributeMorphTargetId(MeshAttribute, ...) overload because the
           lookup is for a concrete morphTargetId already */

        /**
         * @brief Data for given attribute
         *
         * The @p id is expected to be smaller than @ref attributeCount() const.
         * The second dimension represents the actual data type (its size is
         * equal to format size for known @ref VertexFormat values, possibly
         * multiplied by array size, and to *absolute* attribute stride for
         * implementation-specific values) and is guaranteed to be contiguous.
         * In rare cases the first dimension stride may be zero or negative,
         * such data layouts are however not commonly supported by GPU APIs.
         *
         * Use the templated overload below to get the attribute in a concrete
         * type. You can also use
         * @ref attribute(MeshAttribute, UnsignedInt, Int) const to directly
         * get data for given named attribute.
         * @see @relativeref{Corrade,Containers::StridedArrayView::isContiguous()},
         *      @ref vertexFormatSize(),
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
         * instead of @cpp T @ce. In rare cases the stride of the returned view
         * may be zero or negative, such data layouts are however not commonly
         * supported by GPU APIs. You can also use the non-templated
         * @ref positions2DAsArray(), @ref positions3DAsArray(),
         * @ref tangentsAsArray(), @ref bitangentSignsAsArray(),
         * @ref bitangentsAsArray(), @ref normalsAsArray(),
         * @ref textureCoordinates2DAsArray(), @ref colorsAsArray(),
         * @ref jointIdsAsArray(), @ref weightsAsArray() and
         * @ref objectIdsAsArray() accessors to get common attributes converted
         * to usual types in contiguous arrays, but note that these operations
         * involve extra allocation and data conversion.
         * @see @ref attribute(MeshAttribute, UnsignedInt, Int) const,
         *      @ref mutableAttribute(UnsignedInt),
         *      @ref isVertexFormatImplementationSpecific(),
         *      @ref attributeArraySize()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<const T> attribute(UnsignedInt id) const;

        /**
         * @brief Data for given array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes as well
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute. For non-array
         * attributes the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> attribute(UnsignedInt id) const;

        /**
         * @brief Mutable data for given attribute in a concrete type
         *
         * Like @ref attribute(UnsignedInt) const, but returns a mutable view.
         * Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<T> mutableAttribute(UnsignedInt id);

        /**
         * @brief Mutable data for given array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes as well
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute. For non-array
         * attributes the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableAttribute(UnsignedInt id);

        /**
         * @brief Data for given named attribute
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute, Int) const. See
         * @ref attribute(UnsignedInt) const for more information. Use the
         * templated overload below to get the attribute in a concrete type.
         * @see @ref mutableAttribute(MeshAttribute, UnsignedInt, Int),
         *      @relativeref{Corrade,Containers::StridedArrayView::isContiguous()},
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::StridedArrayView2D<const char> attribute(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Mutable data for given named attribute
         *
         * Like @ref attribute(MeshAttribute, UnsignedInt, Int) const, but
         * returns a mutable view. Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        Containers::StridedArrayView2D<char> mutableAttribute(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1);

        /**
         * @brief Data for given named attribute in a concrete type
         *
         * The @p id is expected to be smaller than
         * @ref attributeCount(MeshAttribute, Int) const and @p T is expected
         * to correspond to @ref attributeFormat(MeshAttribute, UnsignedInt, Int) const.
         * Expects that the vertex format is *not* implementation-specific, in
         * that case you can only access the attribute via the typeless
         * @ref attribute(MeshAttribute, UnsignedInt, Int) const above. The
         * attribute is also expected to not be an array, in that case you need
         * to use the overload below by using @cpp T[] @ce instead of
         * @cpp T @ce. In rare cases the stride of the returned view may be
         * zero or negative, such data layouts are however not commonly
         * supported by GPU APIs. You can also use the non-templated
         * @ref positions2DAsArray(), @ref positions3DAsArray(),
         * @ref tangentsAsArray(), @ref bitangentSignsAsArray(),
         * @ref bitangentsAsArray(), @ref normalsAsArray(),
         * @ref textureCoordinates2DAsArray(), @ref colorsAsArray(),
         * @ref jointIdsAsArray(), @ref weightsAsArray() and
         * @ref objectIdsAsArray() accessors to get common attributes converted
         * to usual types in contiguous arrays, but note that these operations
         * involve extra data conversion and an allocation.
         * @see @ref attribute(UnsignedInt) const,
         *      @ref mutableAttribute(MeshAttribute, UnsignedInt, Int),
         *      @ref isVertexFormatImplementationSpecific()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<const T> attribute(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Data for given named array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes as well
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute. For non-array
         * attributes the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> attribute(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Mutable data for given named attribute in a concrete type
         *
         * Like @ref attribute(MeshAttribute, UnsignedInt, Int) const, but
         * returns a mutable view. Expects that the mesh is mutable.
         * @see @ref vertexDataFlags()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<T> mutableAttribute(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1);

        /**
         * @brief Mutable data for given named array attribute in a concrete type
         *
         * Same as above, except that it works with array attributes as well
         * --- you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref attributeArraySize() for given attribute. For non-array
         * attributes the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableAttribute(MeshAttribute name, UnsignedInt id = 0, Int morphTargetId = -1);

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
         * @brief Indices as 32-bit integers into a pre-allocated view
         *
         * Like @ref indicesAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref indexCount()
         */
        void indicesInto(const Containers::StridedArrayView1D<UnsignedInt>& destination) const;

        /**
         * @brief Positions as 2D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Position as the first argument. Converts
         * the position array from an arbitrary underlying type and returns it
         * in a newly-allocated array. If the underlying type is
         * three-component, the last component is dropped. Expects that the
         * vertex format is *not* implementation-specific, in that case you can
         * only access the attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * @see @ref positions2DInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector2> positions2DAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Positions as 2D float vectors into a pre-allocated view
         *
         * Like @ref positions2DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void positions2DInto(const Containers::StridedArrayView1D<Vector2>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Positions as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Position as the first argument. Converts
         * the position array from an arbitrary underlying type and returns it
         * in a newly-allocated array. If the underlying type is two-component,
         * the Z component is set to @cpp 0.0f @ce. Expects that the vertex
         * format is *not* implementation-specific, in that case you can only
         * access the attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * @see @ref positions3DInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector3> positions3DAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Positions as 3D float vectors into a pre-allocated view
         *
         * Like @ref positions3DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void positions3DInto(const Containers::StridedArrayView1D<Vector3>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Tangents as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Tangent as the first argument. Converts the
         * tangent array from an arbitrary underlying type and returns it in a
         * newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         *
         * If the tangents contain a fourth component with bitangent direction,
         * it's ignored here --- use @ref bitangentSignsAsArray() to get those
         * instead. You can also use @ref tangentsInto() together with
         * @ref bitangentSignsInto() to put them both in a single array.
         * @see @ref bitangentsAsArray(), @ref normalsAsArray(),
         *      @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector3> tangentsAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Tangents as 3D float vectors into a pre-allocated view
         *
         * Like @ref tangentsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data. Use @ref bitangentSignsInto() to
         * extract the fourth component wit bitangent direction, if present.
         * @see @ref vertexCount()
         */
        void tangentsInto(const Containers::StridedArrayView1D<Vector3>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

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
        Containers::Array<Float> bitangentSignsAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Bitangent signs as floats into a pre-allocated view
         *
         * Like @ref bitangentsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void bitangentSignsInto(const Containers::StridedArrayView1D<Float>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Bitangents as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Bitangent as the first argument. Converts
         * the bitangent array from an arbitrary underlying type and returns it
         * in a newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
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
        Containers::Array<Vector3> bitangentsAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Bitangents as 3D float vectors into a pre-allocated view
         *
         * Like @ref bitangentsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void bitangentsInto(const Containers::StridedArrayView1D<Vector3>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Normals as 3D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Normal as the first argument. Converts the
         * normal array from an arbitrary underlying type and returns it in a
         * newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * @see @ref normalsInto(), @ref tangentsAsArray(),
         *      @ref bitangentsAsArray(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector3> normalsAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Normals as 3D float vectors into a pre-allocated view
         *
         * Like @ref normalsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void normalsInto(const Containers::StridedArrayView1D<Vector3>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Texture coordinates as 2D float vectors
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::TextureCoordinates as the first argument.
         * Converts the texture coordinate array from an arbitrary underlying
         * type and returns it in a newly-allocated array. Expects that the
         * vertex format is *not* implementation-specific, in that case you can
         * only access the attribute via the typeless
         * @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * @see @ref textureCoordinates2DInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Vector2> textureCoordinates2DAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Texture coordinates as 2D float vectors into a pre-allocated view
         *
         * Like @ref textureCoordinates2DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void textureCoordinates2DInto(const Containers::StridedArrayView1D<Vector2>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Colors as RGBA floats
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Color as the first argument. Converts the
         * color array from an arbitrary underlying type and returns it in a
         * newly-allocated array. If the underlying type is three-component,
         * the alpha component is set to @cpp 1.0f @ce. Expects that the vertex
         * format is *not* implementation-specific, in that case you can only
         * access the attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * @see @ref colorsInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Color4> colorsAsArray(UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Colors as RGBA floats into a pre-allocated view
         *
         * Like @ref colorsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref vertexCount()
         */
        void colorsInto(const Containers::StridedArrayView1D<Color4>& destination, UnsignedInt id = 0, Int morphTargetId = -1) const;

        /**
         * @brief Skin joint IDs as unsigned int arrays
         * @m_since_latest
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::JointIds as the first argument. Converts
         * the joint IDs array from an arbitrary underlying type and returns it
         * in a newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * Unlike other attributes, @ref MeshAttribute::JointIds can't have
         * morph targets so this function provides no morph target ID argument.
         *
         * As it's an array attribute, the returned array has @ref vertexCount()
         * times @ref attributeArraySize(MeshAttribute, UnsignedInt, Int) const
         * elements. You can make a 2D view onto the result to conveniently
         * index the data:
         *
         * @snippet Trade.cpp MeshData-jointIdsAsArray
         *
         * @see @ref weightsAsArray(), @ref jointIdsInto(),
         *      @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        /* Originally I tried with jointIdsWeightsAsArray() because these two
           attributes are usually needed together, and they are also checked to
           have matching counts and array sizes on construction. However, it'd
           mean the two arrays would have to be interleaved for each vertex
           (Array<Pair<UnsignedInt, Float>>) which makes them no longer
           directly usable for mesh attributes, and there are other hurdles
           such as unpackInto() / castInto() expecting the second dimension to
           be contiguous. */
        Containers::Array<UnsignedInt> jointIdsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Skin joint IDs as unsigned int arrays into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref jointIdsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data --- first dimension being
         * @ref vertexCount() and second
         * @ref attributeArraySize(MeshAttribute. UnsignedInt) const. The
         * second dimension is additionally expected to be contiguous.
         */
        void jointIdsInto(const Containers::StridedArrayView2D<UnsignedInt>& destination, UnsignedInt id = 0) const;

        /**
         * @brief Skin weights as float arrays
         * @m_since_latest
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::Weights as the first argument. Converts the
         * weights array from an arbitrary underlying types and returns them in
         * a newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * Unlike other attributes, @ref MeshAttribute::Weights can't have
         * morph targets so this function provides no morph target ID argument.
         *
         * As it's an array attribute, the returned array has @ref vertexCount()
         * times @ref attributeArraySize(MeshAttribute, UnsignedInt, Int) const
         * elements. You can make a 2D view onto the result to conveniently
         * index the data, see @ref jointIdsAsArray() for an example snippet.
         * @see @ref weightsInto(), @ref attributeFormat(),
         *      @ref isVertexFormatImplementationSpecific()
         */
        Containers::Array<Float> weightsAsArray(UnsignedInt id = 0) const;

        /**
         * @brief Skin weights as float arrays into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref weightsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data --- first dimension being
         * @ref vertexCount() and second
         * @ref attributeArraySize(MeshAttribute. UnsignedInt) const. The
         * second dimension is additionally expected to be contiguous.
         */
        void weightsInto(const Containers::StridedArrayView2D<Float>& weightsDestination, UnsignedInt id = 0) const;

        /**
         * @brief Object IDs as 32-bit integers
         *
         * Convenience alternative to @ref attribute(MeshAttribute, UnsignedInt, Int) const
         * with @ref MeshAttribute::ObjectId as the first argument. Converts
         * the object ID array from an arbitrary underlying type and returns it
         * in a newly-allocated array. Expects that the vertex format is *not*
         * implementation-specific, in that case you can only access the
         * attribute via the typeless @ref attribute(MeshAttribute, UnsignedInt, Int) const.
         * Unlike other attributes, @ref MeshAttribute::ObjectId can't have
         * morph targets so this function provides no morph target ID argument.
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
        void objectIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& destination, UnsignedInt id = 0) const;

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

        /* Internal helper without the extra overhead from Optional, returns
           ~UnsignedInt{} on failure */
        UnsignedInt findAttributeIdInternal(MeshAttribute name, UnsignedInt id, Int morphTargetId) const;

        /* Like attribute(), but returning just a 1D view */
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> attributeDataViewInternal(const MeshAttributeData& attribute) const;

        #ifndef CORRADE_NO_ASSERT
        template<class T> bool checkVertexFormatCompatibility(const MeshAttributeData& attribute, const char* prefix) const;
        #endif

        /* GPUs don't currently support more than 32-bit index types / vertex
           counts so this should be enough. Sanity check:
           https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkIndexType.html */
        UnsignedInt _indexCount, _vertexCount;
        MeshPrimitive _primitive;
        MeshIndexType _indexType;
        Short _indexStride;
        DataFlags _indexDataFlags, _vertexDataFlags;
        /* 4 byte padding on 64bit */
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
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T, typename std::enable_if<std::is_convertible<T, Containers::ArrayView<const void>>::value, int>::type> MeshIndexData::MeshIndexData(const MeshIndexType type, T&& data) noexcept: _type{type} {
    const Containers::ArrayView<const void> erased = data;
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(type),
        "Trade::MeshIndexData: can't create index data from a contiguous view and an implementation-specific type" << Debug::hex << meshIndexTypeUnwrap(type) << Debug::nospace << ", pass a strided view instead", );
    const std::size_t typeSize = meshIndexTypeSize(type);
    CORRADE_ASSERT(erased.size()%typeSize == 0,
        "Trade::MeshIndexData: view size" << erased.size() << "does not correspond to" << type, );
    _data = Containers::StridedArrayView1D<const void>{erased, erased.size()/typeSize, std::ptrdiff_t(typeSize)};
}
#endif

constexpr MeshIndexData::MeshIndexData(const MeshIndexType type, const Containers::StridedArrayView1D<const void> data) noexcept:
    _type{type},
    /* The actual limitation is in MeshData, but better to have it caught here
       already */
    _data{(CORRADE_CONSTEXPR_ASSERT(data.stride() >= -32768 && data.stride() <= 32767,
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got" << data.stride()), data)}
    {}

namespace Implementation {
    /* Implicit mapping from a format to enum (1:1) */
    template<class T> constexpr VertexFormat vertexFormatFor() {
        /* C++ why there isn't an obvious way to do such a thing?! */
        static_assert(sizeof(T) == 0, "unsupported vertex format");
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
            (name == MeshAttribute::JointIds &&
                (format == VertexFormat::UnsignedInt ||
                 format == VertexFormat::UnsignedByte ||
                 format == VertexFormat::UnsignedShort)) ||
            (name == MeshAttribute::Weights &&
                (format == VertexFormat::Float ||
                 format == VertexFormat::Half ||
                 format == VertexFormat::UnsignedByteNormalized ||
                 format == VertexFormat::UnsignedShortNormalized)) ||
            (name == MeshAttribute::ObjectId &&
                (format == VertexFormat::UnsignedInt ||
                 format == VertexFormat::UnsignedShort ||
                 format == VertexFormat::UnsignedByte)) ||
            /* Custom attributes can be anything */
            isMeshAttributeCustom(name);
    }

    constexpr bool isAttributeArrayAllowed(MeshAttribute name) {
        return
            name == MeshAttribute::JointIds ||
            name == MeshAttribute::Weights ||
            /* Custom attributes can be anything */
            isMeshAttributeCustom(name);
    }
    constexpr bool isAttributeArrayExpected(MeshAttribute name) {
        /* Custom attributes don't *have to* be arrays */
        return name == MeshAttribute::JointIds ||
               name == MeshAttribute::Weights;
    }

    constexpr bool isMorphTargetAllowed(MeshAttribute name) {
        /* It also makes no sense for custom attributes with non-normalized
           integer formats to be morph targets, but that's impossible to check
           in a constexpr context so we blacklist only the builtin integer
           attributes. */
        return
            name != MeshAttribute::JointIds &&
            name != MeshAttribute::Weights &&
            name != MeshAttribute::ObjectId;
    }
    #endif
}

constexpr MeshAttributeData::MeshAttributeData(std::nullptr_t, const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView1D<const void>& data, const UnsignedShort arraySize, const Int morphTargetId) noexcept:
    _format{format},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isVertexFormatCompatibleWithAttribute(name, format),
        "Trade::MeshAttributeData:" << format << "is not a valid format for" << name), name)},
    _isOffsetOnly{false},
    _morphTargetId{(
        CORRADE_CONSTEXPR_ASSERT(morphTargetId == -1 || UnsignedInt(morphTargetId) < 128,
            "Trade::MeshAttributeData: expected morph target ID to be either -1 or less than 128 but got" << morphTargetId),
        CORRADE_CONSTEXPR_ASSERT(morphTargetId == -1 || Implementation::isMorphTargetAllowed(name),
            "Trade::MeshAttributeData: morph target not allowed for" << name),
        Byte(morphTargetId))},
    _vertexCount{(
        #ifndef CORRADE_TARGET_32BIT
        CORRADE_CONSTEXPR_ASSERT(data.size() <= 0xffffffffu, "Trade::MeshAttributeData: expected vertex count to fit into 32 bits but got" << data.size()),
        #endif
        UnsignedInt(data.size()))},
    _stride{(CORRADE_CONSTEXPR_ASSERT(data.stride() >= -32768 && data.stride() <= 32767,
        "Trade::MeshAttributeData: expected stride to fit into 16 bits but got" << data.stride()),
        Short(data.stride()))},
    _arraySize{(CORRADE_CONSTEXPR_ASSERT(!arraySize || Implementation::isAttributeArrayAllowed(name),
        "Trade::MeshAttributeData:" << name << "can't be an array attribute"),
        CORRADE_CONSTEXPR_ASSERT(arraySize || !Implementation::isAttributeArrayExpected(name), "Trade::MeshAttributeData:" << name << "has to be an array attribute"),
        arraySize)},
    _data{data.data()} {}

constexpr MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const std::size_t offset, const UnsignedInt vertexCount, const std::ptrdiff_t stride, UnsignedShort arraySize, const Int morphTargetId) noexcept:
    _format{format},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isVertexFormatCompatibleWithAttribute(name, format),
        "Trade::MeshAttributeData:" << format << "is not a valid format for" << name), name)},
    _isOffsetOnly{true},
    _morphTargetId{(
        CORRADE_CONSTEXPR_ASSERT(morphTargetId == -1 || UnsignedInt(morphTargetId) < 128,
            "Trade::MeshAttributeData: expected morph target ID to be either -1 or less than 128 but got" << morphTargetId),
        CORRADE_CONSTEXPR_ASSERT(morphTargetId == -1 || Implementation::isMorphTargetAllowed(name),
            "Trade::MeshAttributeData: morph target not allowed for" << name),
        Byte(morphTargetId))},
    _vertexCount{vertexCount},
    _stride{(CORRADE_CONSTEXPR_ASSERT(stride >= -32768 && stride <= 32767,
        "Trade::MeshAttributeData: expected stride to fit into 16 bits but got" << stride),
        Short(stride))},
    _arraySize{(CORRADE_CONSTEXPR_ASSERT(!arraySize || Implementation::isAttributeArrayAllowed(name),
        "Trade::MeshAttributeData:" << name << "can't be an array attribute"), arraySize)},
    _data{offset} {}

template<class T> constexpr MeshAttributeData::MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView1D<T>& data, const Int morphTargetId) noexcept: MeshAttributeData{nullptr, name, Implementation::vertexFormatFor<typename std::remove_const<T>::type>(), data, 0, morphTargetId} {}

template<class T> constexpr MeshAttributeData::MeshAttributeData(MeshAttribute name, const Containers::StridedArrayView2D<T>& data, const Int morphTargetId) noexcept: MeshAttributeData{
    /* Not using isContiguous<1>() as that's not constexpr */
    (CORRADE_CONSTEXPR_ASSERT(data.stride()[1] == sizeof(T), "Trade::MeshAttributeData: second view dimension is not contiguous"), nullptr),
    name,
    Implementation::vertexFormatFor<typename std::remove_const<T>::type>(),
    Containers::StridedArrayView1D<const void>{{data.data(), ~std::size_t{}}, data.size()[0], data.stride()[0]},
    UnsignedShort(data.size()[1]),
    morphTargetId
} {}

template<class T> Containers::StridedArrayView1D<const T> MeshData::indices() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indices(): the mesh is not indexed", {});
    Containers::StridedArrayView2D<const char> data = indices();
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(_indexType),
        "Trade::MeshData::indices(): can't cast data from an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(_indexType), {});
    CORRADE_ASSERT(Implementation::meshIndexTypeFor<T>() == _indexType,
        "Trade::MeshData::indices(): indices are" << _indexType << "but requested" << Implementation::meshIndexTypeFor<T>(), {});
    return Containers::arrayCast<1, const T>(data);
}

template<class T> Containers::StridedArrayView1D<T> MeshData::mutableIndices() {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::mutableIndices(): the mesh is not indexed", {});
    Containers::StridedArrayView2D<char> data = mutableIndices();
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(_indexType),
        "Trade::MeshData::mutableIndices(): can't cast data from an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(_indexType), {});
    CORRADE_ASSERT(Implementation::meshIndexTypeFor<T>() == _indexType,
        "Trade::MeshData::mutableIndices(): indices are" << _indexType << "but requested" << Implementation::meshIndexTypeFor<T>(), {});
    return Containers::arrayCast<1, T>(data);
}

#ifndef CORRADE_NO_ASSERT
template<class T> bool MeshData::checkVertexFormatCompatibility(const MeshAttributeData& attribute, const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
) const {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        prefix << "can't cast data from an implementation-specific vertex format" << Debug::hex << vertexFormatUnwrap(attribute._format), false);
    CORRADE_ASSERT(Implementation::isVertexFormatCompatible<typename std::remove_extent<T>::type>(attribute._format),
        prefix << attribute._name << "is" << attribute._format << "but requested a type equivalent to" << Implementation::vertexFormatFor<typename std::remove_extent<T>::type>(), false);
    CORRADE_ASSERT(!attribute._arraySize || std::is_array<T>::value,
        prefix << attribute._name << "is an array attribute, use T[] to access it", false);
    return true;
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<const T> MeshData::attribute(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = attribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[id], "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> MeshData::attribute(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = attribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[id], "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<T> MeshData::mutableAttribute(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableAttribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[id], "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> MeshData::mutableAttribute(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableAttribute(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[id], "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}

template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<const T> MeshData::attribute(const MeshAttribute name, const UnsignedInt id, const Int morphTargetId) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id, morphTargetId);
    #ifndef CORRADE_NO_ASSERT
    if(morphTargetId == -1) CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes", {});
    else CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes in morph target" << morphTargetId, {});
    #endif
    const Containers::StridedArrayView2D<const char> data = attribute(attributeId);
    /* Unlike mutableAttribute(), the above can't fail, so no early return with
       CORRADE_GRACEFUL_ASSERT */
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[attributeId], "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> MeshData::attribute(const MeshAttribute name, const UnsignedInt id, const Int morphTargetId) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id, morphTargetId);
    #ifndef CORRADE_NO_ASSERT
    if(morphTargetId == -1) CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes", {});
    else CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes in morph target" << morphTargetId, {});
    #endif
    const Containers::StridedArrayView2D<const char> data = attribute(attributeId);
    /* Unlike mutableAttribute(), the above can't fail, so no early return with
       CORRADE_GRACEFUL_ASSERT */
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[attributeId], "Trade::MeshData::attribute():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<T> MeshData::mutableAttribute(const MeshAttribute name, const UnsignedInt id, const Int morphTargetId) {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id, morphTargetId);
    #ifndef CORRADE_NO_ASSERT
    if(morphTargetId == -1) CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes", {});
    else CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes in morph target" << morphTargetId, {});
    #endif
    Containers::StridedArrayView2D<char> data = mutableAttribute(attributeId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[attributeId], "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> MeshData::mutableAttribute(const MeshAttribute name, const UnsignedInt id, const Int morphTargetId) {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id, morphTargetId);
    #ifndef CORRADE_NO_ASSERT
    if(morphTargetId == -1) CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes", {});
    else CORRADE_ASSERT(attributeId != ~UnsignedInt{},
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name, morphTargetId) << name << "attributes in morph target" << morphTargetId, {});
    #endif
    Containers::StridedArrayView2D<char> data = mutableAttribute(attributeId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkVertexFormatCompatibility<T>(_attributes[attributeId], "Trade::MeshData::mutableAttribute():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}
#endif

}}

#endif
