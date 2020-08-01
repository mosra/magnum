#ifndef Magnum_Trade_MaterialData_h
#define Magnum_Trade_MaterialData_h
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
 * @brief Class @ref Magnum::Trade::MaterialData, @ref Magnum::Trade::MaterialAttributeData, enum @ref Magnum::Trade::MaterialAttribute, @ref Magnum::Trade::MaterialAttributeType
 * @m_since_latest
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/RectangularMatrix.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Material attribute name
@m_since_latest

Convenience aliases to actual attribute name strings. In most cases the alias
is in the same form and capitalization --- so for example
@ref MaterialAttribute::DoubleSided is an alias for @cpp "DoubleSided" @ce, the
only exception is @ref MaterialAttribute::LayerName which is @cpp "$LayerName" @ce.

When this enum si used in
@ref MaterialAttributeData constructors, the data are additionally checked for
type compatibility. Other than that, there is no difference to the string
variants.
@see @ref MaterialAttributeData, @ref MaterialData
*/
enum class MaterialAttribute: UnsignedInt {
    /* Zero used for an invalid value */

    /**
     * Layer name, @ref MaterialAttributeType::String.
     *
     * Unlike other attributes where string name matches the enum name, in this
     * case the corresponding string is @cpp "$LayerName" @ce, done in order to
     * have the layer name attribute appear first in each layer and thus
     * simplify layer implementation.
     * @see @ref MaterialData::layerName()
     */
    LayerName = 1,

    /**
     * Alpha mask, @ref MaterialAttributeType::Float.
     *
     * If set together with @ref MaterialAttribute::AlphaBlend, blending is
     * preferred, however renderers can fall back to alpha-masked rendering.
     * Alpha values below this value are meant to be rendered as fully
     * transparent and alpha values above this value as fully opaque.
     * @see @ref MaterialAlphaMode, @ref MaterialData::alphaMode(),
     *      @ref MaterialData::alphaMask()
     */
    AlphaMask,

    /**
     * Alpha blending, @ref MaterialAttributeType::Bool.
     *
     * If @cpp true @ce, the material is expected to be rendered with blending
     * enabled and in correct depth order. If @cpp false @ce or not present,
     * the material should be treated as opaque. If set together with
     * @ref MaterialAttribute::AlphaMask, blending is preferred, however
     * renderers can fall back to alpha-masked rendering.
     * @see @ref MaterialAlphaMode, @ref MaterialData::alphaMode()
     */
    AlphaBlend,

    /**
     * Double sided, @ref MaterialAttributeType::Bool.
     *
     * If not present, the default value is @cpp false @ce.
     * @see @ref MaterialData::isDoubleSided()
     */
    DoubleSided,

    /**
     * Ambient color for Phong materials, @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::AmbientTexture is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::ambientColor()
     */
    AmbientColor,

    /**
     * Ambient texture index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::AmbientColor is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::ambientTexture()
     */
    AmbientTexture,

    /**
     * Ambient texture transformation matrix for Phong materials,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     * @see @ref PhongMaterialData::ambientTextureMatrix()
     */
    AmbientTextureMatrix,

    /**
     * Ambient texture coordinate set index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PhongMaterialData::ambientTextureCoordinates()
     */
    AmbientTextureCoordinates,

    /**
     * Diffuse color for Phong materials, @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::DiffuseTexture is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::diffuseColor()
     */
    DiffuseColor,

    /**
     * Diffuse texture index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::DiffuseColor is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::diffuseTexture()
     */
    DiffuseTexture,

    /**
     * Diffuse texture transformation matrix for Phong materials,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     * @see @ref PhongMaterialData::diffuseTextureMatrix()
     */
    DiffuseTextureMatrix,

    /**
     * Diffuse texture coordinate set index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PhongMaterialData::diffuseTextureCoordinates()
     */
    DiffuseTextureCoordinates,

    /**
     * Specular color for Phong materials, @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::SpecularTexture is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::specularColor()
     */
    SpecularColor,

    /**
     * Specular texture index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::SpecularColor is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::specularTexture()
     */
    SpecularTexture,

    /**
     * Specular texture transformation matrix for Phong materials,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     * @see @ref PhongMaterialData::specularTextureMatrix()
     */
    SpecularTextureMatrix,

    /**
     * Specular texture coordinate set index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PhongMaterialData::specularTextureCoordinates()
     */
    SpecularTextureCoordinates,

    /**
     * Shininess value for Phong materials, @ref MaterialAttributeType::Float.
     *
     * @see @ref PhongMaterialData::shininess()
     */
    Shininess,

    /**
     * Tangent-space normal map texture index,
     * @ref MaterialAttributeType::UnsignedInt.
     * @see @ref PhongMaterialData::normalTexture()
     */
    NormalTexture,

    /**
     * Normal texture transformation matrix,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     * @see @ref PhongMaterialData::normalTextureMatrix()
     */
    NormalTextureMatrix,

    /**
     * Normal texture coordinate set index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PhongMaterialData::normalTextureCoordinates()
     */
    NormalTextureCoordinates,

    /**
     * Common texture transformation matrix for all textures,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * @ref MaterialAttribute::AmbientTextureMatrix /
     * @ref MaterialAttribute::DiffuseTextureMatrix /
     * @ref MaterialAttribute::SpecularTextureMatrix /
     * @ref MaterialAttribute::NormalTextureMatrix have a precedence over this
     * attribute for given texture, if present.
     * @see @ref PhongMaterialData::textureMatrix()
     */
    TextureMatrix,

    /**
     * Common texture coordinate set index for all textures,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * @ref MaterialAttribute::AmbientTextureCoordinates /
     * @ref MaterialAttribute::DiffuseTextureCoordinates /
     * @ref MaterialAttribute::SpecularTextureCoordinates /
     * @ref MaterialAttribute::NormalTextureCoordinates have a precedence
     * over this attribute for given texture, if present.
     * @see @ref PhongMaterialData::textureCoordinates()
     */
    TextureCoordinates,
};

/**
@debugoperatorenum{MaterialAttribute}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAttribute value);

/**
@brief Material attribute type
@m_since_latest

See @ref MaterialData for more information.
@see @ref MaterialAttribute, @ref MaterialAttributeData,
    @ref materialAttributeTypeSize()
*/
enum class MaterialAttributeType: UnsignedByte {
    /* Zero used for an invalid value */

    Bool = 1,       /**< @cpp bool @ce <b></b> */

    Float,          /**< @ref Magnum::Float "Float" */
    Deg,            /**< @ref Magnum::Deg "Deg" */
    Rad,            /**< @ref Magnum::Rad "Rad" */
    UnsignedInt,    /**< @ref Magnum::UnsignedInt "UnsignedInt" */
    Int,            /**< @ref Magnum::Int "Int" */
    UnsignedLong,   /**< @ref Magnum::UnsignedLong "UnsignedLong" */
    Long,           /**< @ref Magnum::Long "Long" */

    Vector2,        /**< @ref Magnum::Vector2 "Vector2" */
    Vector2ui,      /**< @ref Magnum::Vector2ui "Vector2ui" */
    Vector2i,       /**< @ref Magnum::Vector2i "Vector2i" */

    Vector3,        /**< @ref Magnum::Vector3 "Vector3" */
    Vector3ui,      /**< @ref Magnum::Vector3ui "Vector3ui" */
    Vector3i,       /**< @ref Magnum::Vector3i "Vector3i" */

    Vector4,        /**< @ref Magnum::Vector4 "Vector4" */
    Vector4ui,      /**< @ref Magnum::Vector4ui "Vector4ui" */
    Vector4i,       /**< @ref Magnum::Vector4i "Vector4i" */

    Matrix2x2,      /**< @ref Magnum::Matrix2x2 "Matrix2x2" */
    Matrix2x3,      /**< @ref Magnum::Matrix2x3 "Matrix2x3" */
    Matrix2x4,      /**< @ref Magnum::Matrix2x4 "Matrix2x4" */

    Matrix3x2,      /**< @ref Magnum::Matrix3x2 "Matrix3x2" */
    Matrix3x3,      /**< @ref Magnum::Matrix3x3 "Matrix3x3" */
    Matrix3x4,      /**< @ref Magnum::Matrix3x4 "Matrix3x4" */

    Matrix4x2,      /**< @ref Magnum::Matrix4x2 "Matrix4x2" */
    Matrix4x3,      /**< @ref Magnum::Matrix4x3 "Matrix4x3" */

    /* Matrix4x4 not present */

    /**
     * @cpp const void* @ce, type is not preserved. For convenience it's
     * possible to retrieve the value by calling @cpp attribute<const T>() @ce
     * with an arbitrary `T` but the user has to ensure the type is correct.
     */
    Pointer,

    /**
     * @cpp void* @ce, type is not preserved. For convenience it's possible to
     * retrieve the value by calling @cpp attribute<T>() @ce with an arbitrary
     * `T` but the user has to ensure the type is correct.
     */
    MutablePointer,

    /**
     * Null-terminated string. Can be stored using any type convertible to
     * @ref Corrade::Containers::StringView, retrieval has to be done using
     * @ref Corrade::Containers::StringView.
     */
    String
};

/**
@brief Byte size of a material attribute type
@m_since_latest

Can't be used with @ref MaterialAttributeType::String, as the size varies
depending on the value.
*/
MAGNUM_TRADE_EXPORT std::size_t materialAttributeTypeSize(MaterialAttributeType type);

/**
@debugoperatorenum{MaterialAttributeType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAttributeType value);

namespace Implementation {
    template<class> struct MaterialAttributeTypeFor;
    enum: std::size_t { MaterialAttributeDataSize = 64 };
}

/**
@brief Material attribute data
@m_since_latest

See @ref MaterialData for more information.
*/
class MAGNUM_TRADE_EXPORT MaterialAttributeData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of the attribute array for @ref MaterialData,
         * expected to be replaced with concrete values later.
         */
        constexpr explicit MaterialAttributeData() noexcept: _data{} {}

        /**
         * @brief Construct with a string name
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * The @p name together with @p value is expected to fit into 62 bytes.
         * @ref MaterialAttributeType is inferred from the type passed.
         *
         * This function is useful in @cpp constexpr @ce contexts and for
         * creating custom material attributes. For known attributes prefer to
         * use @ref MaterialAttributeData(MaterialAttribute, const T&) if you
         * don't need @cpp constexpr @ce, as it additionally checks that given
         * attribute has the expected type.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class = typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value>::type
            #endif
        > constexpr /*implicit*/ MaterialAttributeData(Containers::StringView name, const T& value) noexcept;

        /**
         * @brief Construct with a string name and string value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * The combined length of @p name and @p value is expected to fit into
         * 61 bytes. Type is set to @ref MaterialAttributeType::String.
         *
         * This function is useful in @cpp constexpr @ce contexts and for
         * creating custom material attributes. For known attributes prefer to
         * use @ref MaterialAttributeData(MaterialAttribute, const T&) if you
         * don't need @cpp constexpr @ce, as it additionally checks that given
         * attribute has the expected type.
         */
        constexpr /*implicit*/ MaterialAttributeData(Containers::StringView name, Containers::StringView value) noexcept;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* "Sure can't be constexpr" overloads to avoid going through the
           *insane* overload puzzle when not needed */
        template<class T, class = typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value>::type> /*implicit*/ MaterialAttributeData(const char* name, const T& value) noexcept: MaterialAttributeData{name, Implementation::MaterialAttributeTypeFor<T>::type(), sizeof(T), &value} {}
        /*implicit*/ MaterialAttributeData(const char* name, Containers::StringView value) noexcept: MaterialAttributeData{name, MaterialAttributeType::String, 0, &value} {}
        #endif

        /**
         * @brief Construct with a predefined name
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, const T&)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name. Apart from the type check, the following two instances are
         * equivalent:
         *
         * @snippet MagnumTrade.cpp MaterialAttributeData-name
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class = typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value>::type
            #endif
        > /*implicit*/ MaterialAttributeData(MaterialAttribute name, const T& value) noexcept: MaterialAttributeData{name, Implementation::MaterialAttributeTypeFor<T>::type(), &value} {}

        /**
         * @brief Construct with a predefined name and string value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, Containers::StringView)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name. Apart from the type check, the following two instances are
         * equivalent:
         *
         * @snippet MagnumTrade.cpp MaterialAttributeData-name
         */
        /*implicit*/ MaterialAttributeData(MaterialAttribute name, Containers::StringView value) noexcept: MaterialAttributeData{name, MaterialAttributeType::String, &value} {}

        /**
         * @brief Construct from a type-erased value
         * @param name      Attribute name
         * @param type      Attribute type
         * @param value     Type-erased value
         *
         * In case @p type is not @ref MaterialAttributeType::String, copies a
         * number of bytes according to @ref materialAttributeTypeSize() from
         * @p value. The @p name together with @p value is expected to fit into
         * 62 bytes.
         *
         * In case @p type is @ref MaterialAttributeType::String, @p value is
         * expected to point to a @ref Containers::StringView. The combined
         * length of @p name and @p value strings is expected to fit into 61
         * bytes.
         */
        /*implicit*/ MaterialAttributeData(Containers::StringView name, MaterialAttributeType type, const void* value) noexcept;

        /**
         * @brief Construct with a predefined name
         * @param name      Attribute name
         * @param type      Attribute type
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, MaterialAttributeType, const void*)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name.
         */
        /*implicit*/ MaterialAttributeData(MaterialAttribute name, MaterialAttributeType type, const void* value) noexcept;

        /** @brief Attribute type */
        MaterialAttributeType type() const { return _data.type; }

        /**
         * @brief Attribute name
         *
         * The returned view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         */
        Containers::StringView name() const { return _data.data + 1; }

        /**
         * @brief Type-erased attribute value
         *
         * Cast the pointer to a concrete type based on @ref type(). Note that
         * in case of a @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, returns a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case of a @ref MaterialAttributeType::String, returns a
         * null-terminated @cpp const char* @ce (not a pointer to
         * @ref Containers::StringView). This doesn't preserve the actual
         * string size in case the string data contain zero bytes, thus prefer
         * to use typed access in that case.
         */
        const void* value() const;

        /**
         * @brief Attribute value
         *
         * Expects that @p T corresponds to @ref type().
         */
        template<class T> T value() const;

    private:
        friend MaterialData;

        explicit MaterialAttributeData(Containers::StringView name, const MaterialAttributeType type, std::size_t typeSize, const void* value) noexcept;

        /* Most of this is needed only for the constexpr constructor (yay C++),
           the actual data layout is

            |------------- x B ------------|

            +------+------- .. -----+------+
            | type | name   ..   \0 | data |
            | 1 B  |  (x - n - 2) B | n B  |
            +------+------- .. -----+------+

          where

           - `x` is Implementation::MaterialAttributeDataSize,
           - `type` is an 8-bit MaterialAttributeType,
           - `data` is of size matching `type`, at the offset of
             `(x - materialAttributeTypeSize(type))` B,
           - `name` is a null-terminated string filling the rest.

          This way the name is always at the same offset to make binary search
          lookup fast and efficient, and data being at the end (instead of
          right after the null-terminated string) makes them accessible in O(1)
          as well. */
        struct StringData {
            template<std::size_t ...sequence> constexpr explicit StringData(MaterialAttributeType type, Containers::StringView name, Containers::StringView value, Math::Implementation::Sequence<sequence...>): type{type}, nameValue{(sequence < name.size() ? name[sequence] : (sequence - (Implementation::MaterialAttributeDataSize - value.size() - 3) < value.size() ? value[sequence - (Implementation::MaterialAttributeDataSize - value.size() - 3)] : '\0'))...}, size{UnsignedByte(value.size())} {}
            constexpr explicit StringData(MaterialAttributeType type, Containers::StringView name, Containers::StringView value): StringData{type, name, value, typename Math::Implementation::GenerateSequence<Implementation::MaterialAttributeDataSize - 2>::Type{}} {}

            MaterialAttributeType type;
            char nameValue[Implementation::MaterialAttributeDataSize - 2];
            UnsignedByte size;
        };
        union ErasedScalar {
            constexpr explicit ErasedScalar(Float value): f{value} {}
            constexpr explicit ErasedScalar(Deg value): f{Float(value)} {}
            constexpr explicit ErasedScalar(Rad value): f{Float(value)} {}
            constexpr explicit ErasedScalar(UnsignedInt value): u{value} {}
            constexpr explicit ErasedScalar(Int value): i{value} {}

            Float f;
            UnsignedInt u;
            Int i;
        };
        union ErasedLongScalar {
            constexpr explicit ErasedLongScalar(UnsignedLong value): u{value} {}
            constexpr explicit ErasedLongScalar(Long value): i{value} {}

            UnsignedLong u;
            Long i;
        };
        template<std::size_t size> union ErasedVector {
            constexpr explicit ErasedVector(const Math::Vector<size, Float>& value): f{value} {}
            constexpr explicit ErasedVector(const Math::Vector<size, UnsignedInt>& value): u{value} {}
            constexpr explicit ErasedVector(const Math::Vector<size, Int>& value): i{value} {}

            Math::Vector<size, Float> f;
            Math::Vector<size, UnsignedInt> u;
            Math::Vector<size, Int> i;
        };
        template<std::size_t cols, std::size_t rows> union ErasedMatrix {
            constexpr explicit ErasedMatrix(const Math::RectangularMatrix<cols, rows, Float>& value): a{value} {}
            constexpr explicit ErasedMatrix(const Math::RectangularMatrix<rows, cols, Float>& value): b{value} {}

            Math::RectangularMatrix<cols, rows, Float> a;
            Math::RectangularMatrix<rows, cols, Float> b;
        };
        template<class T> struct Data {
            template<class U, std::size_t ...sequence> constexpr explicit Data(MaterialAttributeType type, Containers::StringView name, const U& value, Math::Implementation::Sequence<sequence...>): type{type}, name{(sequence < name.size() ? name[sequence] : '\0')...}, value{value} {}
            template<class U> constexpr explicit Data(MaterialAttributeType type, Containers::StringView name, const U& value): Data{type, name, value, typename Math::Implementation::GenerateSequence<63 - sizeof(T)>::Type{}} {}

            MaterialAttributeType type;
            char name[Implementation::MaterialAttributeDataSize - sizeof(MaterialAttributeType) - sizeof(T)];
            T value;
        };
        union CORRADE_ALIGNAS(8) Storage {
            constexpr explicit Storage() noexcept: data{} {}

            constexpr explicit Storage(Containers::StringView name, Containers::StringView value) noexcept: s{MaterialAttributeType::String, name, value} {}

            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 1, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _1{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 4 && !std::is_pointer<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _4{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 8 && !Math::IsVector<T>::value && !std::is_pointer<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _8{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 8 && Math::IsVector<T>::value && !std::is_pointer<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _8v{type, name, value} {}
            constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const void* value) noexcept: p{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 12, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _12{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 16 && Math::IsVector<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _16{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 16 && !Math::IsVector<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _16m{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 24, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _24{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 32, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _32{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 36, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _36{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 48, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _48{type, name, value} {}

            MaterialAttributeType type;
            char data[Implementation::MaterialAttributeDataSize];
            StringData s;
            Data<bool> _1;
            Data<const void*> p;
            Data<ErasedScalar> _4;
            Data<ErasedLongScalar> _8;
            Data<ErasedVector<2>> _8v;
            Data<ErasedVector<3>> _12;
            Data<ErasedVector<4>> _16;
            Data<Math::RectangularMatrix<2, 2, Float>> _16m;
            Data<ErasedMatrix<2, 3>> _24;
            Data<ErasedMatrix<2, 4>> _32;
            Data<Math::RectangularMatrix<3, 3, Float>> _36;
            Data<ErasedMatrix<3, 4>> _48;
        } _data;

        static_assert(sizeof(Storage) == Implementation::MaterialAttributeDataSize, "something is off, huh");
};

/**
@brief Material type

@see @ref MaterialTypes, @ref MaterialData::types()
*/
enum class MaterialType: UnsignedInt {
    /**
     * Phong. Use @ref PhongMaterialData for convenience attribute access.
     */
    Phong = 1 << 0
};

/** @debugoperatorenum{MaterialType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialType value);

/**
@brief Material types
@m_since_latest

@see @ref MaterialData::types()
*/
typedef Containers::EnumSet<MaterialType> MaterialTypes;

CORRADE_ENUMSET_OPERATORS(MaterialTypes)

/** @debugoperatorenum{MaterialTypes} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialTypes value);

/**
@brief Material alpha mode

Convenience access to @ref MaterialAttribute::AlphaBlend and
@ref MaterialAttribute::AlphaMask attributes.
@see @ref MaterialData::alphaMode(), @ref MaterialData::alphaMask()
*/
enum class MaterialAlphaMode: UnsignedByte {
    /** Alpha value is ignored and the rendered output is fully opaque. */
    Opaque,

    /**
     * The rendered output is either fully transparent or fully opaque,
     * depending on the alpha value and specified
     * @ref MaterialData::alphaMask() value.
     */
    Mask,

    /**
     * The alpha value is used to combine source and destination colors using
     * additive blending.
     */
    Blend
};

/** @debugoperatorenum{MaterialAlphaMode} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAlphaMode value);

/**
@brief Material data
@m_since_latest

Key-value store for material attributes in one of the types defined by
@ref MaterialAttributeType.

@section Trade-MaterialData-representation Internal representation

The attributes are stored sorted by the key in a contiguous array, with each
@ref MaterialAttributeData item occupying 64 bytes. The item contains a 1-byte
type identifier, the actual value and the rest is occupied with null-terminated
name. This means the name length can vary from 14 bytes for
@ref Magnum::Matrix3x4 "Matrix3x4" / @ref Magnum::Matrix4x3 "Matrix4x3" to 61
bytes for @cpp bool @ce (excluding null terminator). As each item has a fixed
size anyway, there's no value in supporting space-efficient 8-, 16- or
half-float types. Conversely, @ref Magnum::Double "Double" types are currently
not supported either as there isn't currently seen any need for extended
precision.

@m_class{m-block m-warning}

@par Max representable data size
    With the current design, @ref MaterialAttributeData is 64 bytes and in
    order to fit a type identifier and a string attribute name of a reasonable
    length, the maximum data size is capped to 48 bytes. This means
    @ref Magnum::Matrix4x4 "Matrix4x4" isn't listed among supported types, but
    it shouldn't be a problem in practice --- ever an arbitrary color
    correction matrix is just 3x4 values with the bottom row being always
    @f$ \begin{pmatrix} 0 & 0 & 0 & 1 \end{pmatrix} @f$. This restriction might
    get lifted eventually.
*/
class MAGNUM_TRADE_EXPORT MaterialData {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material flag
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided() etc.
         *
         * This enum is further extended in subclasses.
         * @see @ref Flags, @ref flags()
         */
        enum class CORRADE_DEPRECATED_ENUM("use hasAttribute() etc. instead") Flag: UnsignedInt {
            /**
             * The material is double-sided. Back faces should not be culled
             * away but rendered as well, with normals flipped for correct
             * lighting.
             */
            DoubleSided = 1 << 0
        };

        /**
         * @brief Material flags
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided() etc.
         *
         * This enum is extended in subclasses.
         * @see @ref flags()
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flag, ugh */
        typedef CORRADE_DEPRECATED("use hasAttribute() etc. instead") Containers::EnumSet<Flag> Flags;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Construct
         * @param types             Which material types are described by this
         *      data. Can be an empty set.
         * @param attributeData     Attribute data
         * @param importerState     Importer-specific state
         *
         * The @p attributeData gets sorted by name internally, expecting no
         * duplicates.
         */
        explicit MaterialData(MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, const void* importerState = nullptr) noexcept: MaterialData{types, std::move(attributeData), nullptr, importerState} {}

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MaterialData(MaterialTypes types, std::initializer_list<MaterialAttributeData> attributeData, const void* importerState = nullptr): MaterialData{types, attributeData, {}, importerState} {}

        /**
         * @brief Construct a non-owned material data
         * @param types                 Which material types are described by
         *      this data. Can be an empty set.
         * @param attributeDataFlags    Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param attributeData         Attribute data
         * @param importerState         Importer-specific state
         *
         * The @p attributeData is expected to be already sorted by name,
         * without duplicates.
         */
        explicit MaterialData(MaterialTypes types, DataFlags attributeDataFlags, Containers::ArrayView<const MaterialAttributeData> attributeData, const void* importerState = nullptr) noexcept: MaterialData{types, attributeDataFlags, attributeData, {}, nullptr, importerState} {}

        /**
         * @brief Construct with layers
         * @param types             Which material types are described by this
         *      data. Can be an empty set.
         * @param attributeData     Attribute data
         * @param layerData         Layer offset data
         * @param importerState     Importer-specific state
         *
         * The @p attributeData gets sorted by name internally, expecting no
         * duplicates inside each layer. The @p layerData is expected to be
         * either empty or a monotonically non-decreasing sequence of offsets
         * not larger than @p attributeData size, with *i*-th item specifying
         * end offset of *i*-th layer.
         */
        explicit MaterialData(MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, Containers::Array<UnsignedInt>&& layerData, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MaterialData(MaterialTypes types, std::initializer_list<MaterialAttributeData> attributeData, std::initializer_list<UnsignedInt> layerData, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned material data with layers
         * @param types                 Which material types are described by
         *      this data. Can be an empty set.
         * @param attributeDataFlags    Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param attributeData         Attribute data
         * @param layerDataFlags        Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param layerData             Layer offset data
         * @param importerState     Importer-specific state
         *
         * The @p data is expected to be already sorted by name, without
         * duplicates inside each layer. The @p layerData is expected to be
         * either empty or a monotonically non-decreasing sequence of offsets
         * not larger than @p attributeData size, with *i*-th item specifying
         * end offset of *i*-th layer.
         */
        /* The second (ignored) DataFlags is present in order to make it ready
           for a possible extension where only one of the data is non-owned.
           But so far I didn't see a need. */
        explicit MaterialData(MaterialTypes types, DataFlags attributeDataFlags, Containers::ArrayView<const MaterialAttributeData> attributeData, DataFlags layerDataFlags, Containers::ArrayView<const UnsignedInt> layerData, const void* importerState = nullptr) noexcept;

        ~MaterialData();

        /** @brief Copying is not allowed */
        MaterialData(const MaterialData&) = delete;

        /** @brief Move constructor */
        MaterialData(MaterialData&&) noexcept;

        /** @brief Copying is not allowed */
        MaterialData& operator=(const MaterialData&) = delete;

        /** @brief Move assignment */
        MaterialData& operator=(MaterialData&&) noexcept;

        /**
         * @brief Material types
         *
         * Each type indicates that the material data can be interpreted as
         * given type. For custom materials the set can also be empty.
         */
        MaterialTypes types() const { return _types; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material type
         * @m_deprecated_since_latest Use @ref types() instead.
         */
        CORRADE_DEPRECATED("use types() instead") MaterialType type() const {
            return MaterialType(UnsignedInt(_types & MaterialType::Phong));
        }
        #endif

        /**
         * @brief Raw layer offset data
         *
         * May return @cpp nullptr @ce if the material doesn't have any extra
         * layers.
         * @see @ref releaseLayerData()
         */
        Containers::ArrayView<const UnsignedInt> layerData() const { return _layerOffsets; }

        /**
         * @brief Raw attribute data
         *
         * Returns @cpp nullptr @ce if the material has no attributes.
         * @see @ref releaseAttributeData()
         */
        Containers::ArrayView<const MaterialAttributeData> attributeData() const { return _data; }

        /**
         * @brief Layer count
         *
         * There's always at least the base material, so this function returns
         * at least @cpp 1 @ce.
         */
        UnsignedInt layerCount() const {
            return _layerOffsets.empty() ? 1 : _layerOffsets.size();
        }

        /**
         * @brief Whether a material has given named layer
         *
         * Layers with no name assigned are skipped. The base material (layer
         * @cpp 0 @ce is skipped as well) to avoid confusing base material with
         * a layer. If you want to create a material consisting of just a
         * layer, use @cpp 0 @ce for the first layer offset in the constructor.
         * @see @ref hasAttribute()
         */
        bool hasLayer(Containers::StringView layer) const;

        /**
         * @brief ID of a named layer
         *
         * The @p layer is expected to exist.
         * @see @ref hasLayer()
         */
        UnsignedInt layerId(Containers::StringView layer) const;

        /**
         * @brief Layer name
         *
         * Retrieves a @ref MaterialAttribute::LayerName attribute from given
         * layer, if present. Returns a @cpp nullptr @ce view if the layer
         * has no name, and an empty non-null view if the layer name is empty.
         * The @p layer is expected to be smaller than @ref layerCount() const.
         *
         * The name, if present, is ignored for the base material (layer
         * @cpp 0 @ce) to avoid confsing base material with a layer. If you
         * want to create a material consisting of just a layer, use @cpp 0 @ce
         * for the first layer offset in the constructor.
         */
        Containers::StringView layerName(UnsignedInt layer) const;

        /**
         * @brief Attribute count in given layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const.
         */
        UnsignedInt attributeCount(UnsignedInt layer) const;

        /**
         * @brief Attribute count in a named layer
         *
         * The @p layer is expected to exist.
         * @see @ref hasLayer()
         */
        UnsignedInt attributeCount(Containers::StringView layer) const;

        /**
         * @brief Attribute count in the base material
         *
         * Equivalent to calling @ref attributeCount(UnsignedInt) const with
         * @p layer set to @cpp 0 @ce.
         */
        UnsignedInt attributeCount() const { return attributeCount(0); }

        /**
         * @brief Whether a material layer has given attribute
         *
         * The @p layer is expected to be smaller than @ref layerCount() const.
         * @see @ref tryAttribute(), @ref attributeOr(), @ref hasLayer()
         */
        bool hasAttribute(UnsignedInt layer, Containers::StringView name) const;
        bool hasAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Whether a named material layer has given attribute
         *
         * The @p layer is expected to exist.
         * @see @ref tryAttribute(), @ref attributeOr(), @ref hasLayer()
         */
        bool hasAttribute(Containers::StringView layer, Containers::StringView name) const;
        bool hasAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Whether the base material has given attribute
         *
         * Equivalent to calling @ref hasAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         * @see @ref tryAttribute(), @ref attributeOr()
         */
        bool hasAttribute(Containers::StringView name) const {
            return hasAttribute(0, name);
        }
        bool hasAttribute(MaterialAttribute name) const {
            return hasAttribute(0, name);
        } /**< @overload */

        /**
         * @brief ID of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer.
         * @see @ref hasAttribute()
         */
        UnsignedInt attributeId(UnsignedInt layer, Containers::StringView name) const;
        UnsignedInt attributeId(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief ID of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        UnsignedInt attributeId(Containers::StringView layer, Containers::StringView name) const;
        UnsignedInt attributeId(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief ID of a named attribute in the base material
         *
         * Equivalent to calling @ref attributeId(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        UnsignedInt attributeId(Containers::StringView name) const {
            return attributeId(0, name);
        }
        UnsignedInt attributeId(MaterialAttribute name) const {
            return attributeId(0, name);
        } /**< @overload */

        /**
         * @brief Name of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. The returned view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         * @see @ref attributeType()
         */
        Containers::StringView attributeName(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Name of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref hasLayer()
         */
        Containers::StringView attributeName(Containers::StringView layer, UnsignedInt id) const;

        /**
         * @brief Name of an attribute in the base material
         *
         * Equivalent to calling @ref attributeName(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        Containers::StringView attributeName(UnsignedInt id) const {
            return attributeName(0, id);
        }

        /**
         * @brief Type of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref attributeName()
         */
        MaterialAttributeType attributeType(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Type of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer.
         * @see @ref hasAttribute()
         */
        MaterialAttributeType attributeType(UnsignedInt layer, Containers::StringView name) const;
        /** @overload */
        MaterialAttributeType attributeType(UnsignedInt layer, MaterialAttribute name) const;

        /**
         * @brief Type of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref hasLayer()
         */
        MaterialAttributeType attributeType(Containers::StringView layer, UnsignedInt id) const;

        /**
         * @brief Type of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        MaterialAttributeType attributeType(Containers::StringView layer, Containers::StringView name) const;
        MaterialAttributeType attributeType(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type of an attribute in the base material
         *
         * Equivalent to calling @ref attributeType(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        MaterialAttributeType attributeType(UnsignedInt id) const {
            return attributeType(0, id);
        }

        /**
         * @brief Type of a named attribute in the base material
         *
         * Equivalent to calling @ref attributeType(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        MaterialAttributeType attributeType(Containers::StringView name) const {
            return attributeType(0, name);
        }
        MaterialAttributeType attributeType(MaterialAttribute name) const {
            return attributeType(0, name);
        } /**< @overload */

        /**
         * @brief Type-erased value of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. Cast the pointer to a concrete type based on
         * @ref type(). Note that in case of a
         * @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, returns a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case of a @ref MaterialAttributeType::String returns a
         * null-terminated @cpp const char* @ce (not a pointer to
         * @ref Containers::StringView). This doesn't preserve the actual
         * string size in case the string data contain zero bytes, thus prefer
         * to use typed access in that case.
         */
        const void* attribute(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Type-erased value of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer. Cast the pointer to
         * a concrete type based on @ref attributeType(). Note that
         * in case of a @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, returns a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case of a @ref MaterialAttributeType::String returns a
         * null-terminated @cpp const char* @ce (not a pointer to
         * @ref Containers::StringView). This doesn't preserve the actual
         * string size in case the string data contain zero bytes, thus prefer
         * to use typed access in that case.
         * @see @ref hasAttribute(), @ref tryAttribute(), @ref attributeOr()
         */
        const void* attribute(UnsignedInt layer, Containers::StringView name) const;
        const void* attribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased value of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. Cast the pointer to a concrete type based on
         * @ref attributeType(). Note that in case of a
         * @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, returns a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case of a @ref MaterialAttributeType::String returns a
         * null-terminated @cpp const char* @ce (not a pointer to
         * @ref Containers::StringView). This doesn't preserve the actual
         * string size in case the string data contain zero bytes, thus prefer
         * to use typed access in that case.
         * @see @ref hasLayer()
         */
        const void* attribute(Containers::StringView layer, UnsignedInt id) const;

        /**
         * @brief Type-erased value of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer. Cast the pointer to a concrete type based on
         * @ref attributeType(). Note that in case of a
         * @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, returns a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case of a @ref MaterialAttributeType::String returns a
         * null-terminated @cpp const char* @ce (not a pointer to
         * @ref Containers::StringView). This doesn't preserve the actual
         * string size in case the string data contain zero bytes, thus prefer
         * to use typed access in that case.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        const void* attribute(Containers::StringView layer, Containers::StringView name) const;
        const void* attribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased value of an attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* attribute(UnsignedInt id) const {
            return attribute(0, id);
        }

        /**
         * @brief Type-erased value of a named attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* attribute(Containers::StringView name) const {
            return attribute(0, name);
        }
        const void* attribute(MaterialAttribute name) const {
            return attribute(0, name);
        } /**< @overload */

        /**
         * @brief Value of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than
         * @ref attributeCount(UnsignedInt) const in that layer. Expects that
         * @p T corresponds to @ref attributeType(UnsignedInt, UnsignedInt) const
         * for given @p layer and @p id. In case of a string, the returned view
         * always has @ref Corrade::Containers::StringViewFlag::NullTerminated
         * set.
         */
        template<class T> T attribute(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Value of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer. Expects that @p T
         * corresponds to @ref attributeType(UnsignedInt, Containers::StringView) const
         * for given @p layer and @p name. In case of a string, the returned
         * view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        template<class T> T attribute(UnsignedInt layer, Containers::StringView name) const;
        template<class T> T attribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of an attribute in a named material layer
         *
         * The @p layer is expected to exist and @p id is expected to be
         * smaller than @ref attributeCount(UnsignedInt) const in that layer.
         * Expects that @p T corresponds to
         * @ref attributeType(Containers::StringView, UnsignedInt) const
         * for given @p layer and @p id. In case of a string, the returned view
         * always has @ref Corrade::Containers::StringViewFlag::NullTerminated
         * set.
         * @see @ref hasLayer()
         */
        template<class T> T attribute(Containers::StringView layer, UnsignedInt id) const;

        /**
         * @brief Value of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer. Expects that @p T corresponds to
         * @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name. In case of a string, the returned
         * view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        template<class T> T attribute(Containers::StringView layer, Containers::StringView name) const;
        template<class T> T attribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of an attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> T attribute(UnsignedInt id) const {
            return attribute<T>(0, id);
        }

        /**
         * @brief Value of a named attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> T attribute(Containers::StringView name) const {
            return attribute<T>(0, name);
        }
        template<class T> T attribute(MaterialAttribute name) const {
            return attribute<T>(0, name);
        } /**< @overload */

        /**
         * @brief Type-erased attribute value in given material layer, if exists
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @cpp nullptr @ce instead of
         * asserting. Expects that @p layer is smaller than @ref layerCount() const.
         * Cast the pointer to a concrete type based on @ref attributeType().
         * @see @ref hasAttribute(), @ref attributeOr()
         */
        const void* tryAttribute(UnsignedInt layer, Containers::StringView name) const;
        const void* tryAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased attribute value in a named material layer, if exists
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @cpp nullptr @ce instead of
         * asserting. Expects that @p layer exists. Cast the pointer to a
         * concrete type based on @ref attributeType().
         * @see @ref hasLayer(), @ref hasAttribute(), @ref attributeOr()
         */
        const void* tryAttribute(Containers::StringView layer, Containers::StringView name) const;
        const void* tryAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of a named attribute in given material layer, if exists
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @ref Corrade::Containers::NullOpt
         * instead of asserting. Expects that @p layer is smaller than
         * @ref layerCount() const and that @p T corresponds to
         * @ref attributeType(UnsignedInt, Containers::StringView) const for
         * given @p layer and @p name.
         */
        template<class T> Containers::Optional<T> tryAttribute(UnsignedInt layer, Containers::StringView name) const;
        template<class T> Containers::Optional<T> tryAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of a named attribute in a named material layer, if exists
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @ref Corrade::Containers::NullOpt
         * instead of asserting. Expects that @p layer exists and that @p T
         * corresponds to @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name.
         * @see @ref hasLayer()
         */
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView layer, Containers::StringView name) const;
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased attribute value in the base material, if exists
         *
         * Equivalent to calling @ref tryAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* tryAttribute(Containers::StringView name) const {
            return tryAttribute(0, name);
        }
        const void* tryAttribute(MaterialAttribute name) const {
            return tryAttribute(0, name);
        } /**< @overload */

        /**
         * @brief Value of a named attribute in the base material, if exists
         *
         * Equivalent to calling @ref tryAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView name) const {
            return tryAttribute<T>(0, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(MaterialAttribute name) const {
            return tryAttribute<T>(0, name);
        } /**< @overload */

        /**
         * @brief Value of a named attribute in given layer or a default
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @p defaultValue instead of
         * asserting. Expects that @p layer is smaller than @ref layerCount()
         * const
         * and that @p T corresponds to @ref attributeType(UnsignedInt, Containers::StringView) const
         * for given @p layer and @p name.
         */
        template<class T> T attributeOr(UnsignedInt layer, Containers::StringView name, const T& defaultValue) const;
        template<class T> T attributeOr(UnsignedInt layer, MaterialAttribute name, const T& defaultValue) const; /**< @overload */

        /**
         * @brief Value of a named attribute in a named layer or a default
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @p defaultValue instead of
         * asserting. Expects that @p layer exists and that @p T corresponds to
         * @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name.
         * @see @ref hasLayer()
         */
        template<class T> T attributeOr(Containers::StringView layer, Containers::StringView name, const T& defaultValue) const;
        template<class T> T attributeOr(Containers::StringView layer, MaterialAttribute name, const T& defaultValue) const; /**< @overload */

        /**
         * @brief Value of a named attribute in the base material or a default
         *
         * Equivalent to calling @ref attributeOr(UnsignedInt, Containers::StringView, const T&) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> T attributeOr(Containers::StringView name, const T& defaultValue) const {
            return attributeOr<T>(0, name, defaultValue);
        }
        template<class T> T attributeOr(MaterialAttribute name, const T& defaultValue) const {
            return attributeOr<T>(0, name, defaultValue);
        }/**< @overload */

        /**
         * @brief Whether a material is double-sided
         *
         * Convenience access to the @ref MaterialAttribute::DoubleSided
         * attribute. If not present, the default is @cpp false @ce.
         */
        bool isDoubleSided() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material flags
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided() etc.
         *
         * Not all bits returned might be defined by @ref Flag, subclasses may
         * define extra values.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        CORRADE_DEPRECATED("use hasAttribute() instead") Flags flags() const;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Alpha mode
         *
         * Convenience access to @ref MaterialAttribute::AlphaBlend and
         * @ref MaterialAttribute::AlphaMask attributes. If neither is present,
         * the default is @ref MaterialAlphaMode::Opaque.
         */
        MaterialAlphaMode alphaMode() const;

        /**
         * @brief Alpha mask
         *
         * Convenience access to the @ref MaterialAttribute::AlphaMask
         * attribute. If not present, the default is @cpp 0.5f @ce.
         */
        Float alphaMask() const;

        /**
         * @brief Release layer data storage
         *
         * Releases the ownership of the layer offset array and resets internal
         * layer-related state to default. The material then behaves like if it
         * has no layers. Note that the returned array has a custom no-op
         * deleter when the data are not owned by the mesh, and while the
         * returned array type is mutable, the actual memory might be not.
         *
         * @attention Querying attributes after calling @ref releaseLayerData()
         *      has undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseAttributeData().
         * @see @ref layerData()
         */
        Containers::Array<UnsignedInt> releaseLayerData();

        /**
         * @brief Release attribute data storage
         *
         * Releases the ownership of the attribute array and resets internal
         * attribute-related state to default. The material then behaves like
         * if it has no attributes. Note that the returned array has a custom
         * no-op deleter when the data are not owned by the mesh, and while the
         * returned array type is mutable, the actual memory might be not.
         *
         * @attention Querying layers after calling @ref releaseAttributeData()
         *      has undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseLayerData().
         * @see @ref attributeData()
         */
        Containers::Array<MaterialAttributeData> releaseAttributeData();

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

        static Containers::StringView attributeString(MaterialAttribute name);
        /* Internal helpers that don't assert, unlike layerId() / attributeId() */
        UnsignedInt layerFor(Containers::StringView layer) const;
        UnsignedInt layerOffset(UnsignedInt layer) const {
            return layer && _layerOffsets ? _layerOffsets[layer - 1] : 0;
        }
        UnsignedInt attributeFor(UnsignedInt layer, Containers::StringView name) const;

        Containers::Array<MaterialAttributeData> _data;
        Containers::Array<UnsignedInt> _layerOffsets;
        MaterialTypes _types;
        const void* _importerState;
};

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(MaterialData::Flags)

/**
@debugoperatorclassenum{MaterialData,MaterialData::Flag}
@m_deprecated_since_latest The flags are no longer stored directly but
    generated on-the-fly from attribute data, which makes them less efficient
    than calling @ref MaterialData::hasAttribute(),
    @ref MaterialData::isDoubleSided() etc.
*/
/* Not marked with CORRADE_DEPRECATED() as there's enough warnings already */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialData::Flag value);

/**
@debugoperatorclassenum{MaterialData,MaterialData::Flags}
@m_deprecated_since_latest The flags are no longer stored directly but
    generated on-the-fly from attribute data, which makes them less efficient
    than calling @ref MaterialData::hasAttribute(),
    @ref MaterialData::isDoubleSided() etc.
*/
/* Not marked with CORRADE_DEPRECATED() as there's enough warnings already */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialData::Flags value);
CORRADE_IGNORE_DEPRECATED_POP
#endif

namespace Implementation {
    /* LCOV_EXCL_START */
    template<class T> struct MaterialAttributeTypeFor {
        /* C++ why there isn't an obvious way to do such a thing?! */
        static_assert(sizeof(T) == 0, "unsupported attribute type");
    };
    template<> struct MaterialAttributeTypeFor<bool> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::Bool;
        }
    };
    template<class T> struct MaterialAttributeTypeFor<const T*> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::Pointer;
        }
    };
    template<class T> struct MaterialAttributeTypeFor<T*> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::MutablePointer;
        }
    };
    /* No specialization for StringView as this type trait should not be used
       in that case */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(type_) template<> struct MaterialAttributeTypeFor<type_> {   \
        constexpr static MaterialAttributeType type() {                     \
            return MaterialAttributeType::type_;                            \
        }                                                                   \
    };
    _c(Float)
    _c(Deg)
    _c(Rad)
    _c(UnsignedInt)
    _c(Int)
    _c(UnsignedLong)
    _c(Long)
    _c(Vector2)
    _c(Vector2ui)
    _c(Vector2i)
    _c(Vector3)
    _c(Vector3ui)
    _c(Vector3i)
    _c(Vector4)
    _c(Vector4ui)
    _c(Vector4i)
    _c(Matrix2x2)
    _c(Matrix2x3)
    _c(Matrix2x4)
    _c(Matrix3x2)
    _c(Matrix3x3)
    _c(Matrix3x4)
    _c(Matrix4x2)
    _c(Matrix4x3)
    #undef _c
    #endif
    template<> struct MaterialAttributeTypeFor<Color3>: MaterialAttributeTypeFor<Vector3> {};
    template<> struct MaterialAttributeTypeFor<Color4>: MaterialAttributeTypeFor<Vector4> {};
    template<> struct MaterialAttributeTypeFor<Matrix3>: MaterialAttributeTypeFor<Matrix3x3> {};
    /* LCOV_EXCL_STOP */
}

/* The 2 extra bytes are for a null byte after the name and a type */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , class
    #endif
> constexpr MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const T& value) noexcept: _data{Implementation::MaterialAttributeTypeFor<T>::type(), (CORRADE_CONSTEXPR_ASSERT(name.size() + sizeof(T) + 2 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "too long, expected at most" << Implementation::MaterialAttributeDataSize - sizeof(T) - 2 << "bytes for" << Implementation::MaterialAttributeTypeFor<T>::type() << "but got" << name.size()), name), value} {}

/* The 4 extra bytes are for a null byte after both the name and value, a type
   and a string size */
constexpr MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const Containers::StringView value) noexcept: _data{(CORRADE_CONSTEXPR_ASSERT(name.size() + value.size() + 4 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "and value" << value << "too long, expected at most" << Implementation::MaterialAttributeDataSize - 4 << "bytes in total but got" << name.size() + value.size()), name), value} {}

template<class T> T MaterialAttributeData::value() const {
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == _data.type,
        "Trade::MaterialAttributeData::value(): improper type requested for" << (_data.data + 1) << "of" << _data.type, {});
    return *reinterpret_cast<const T*>(value());
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> Containers::StringView MaterialAttributeData::value<Containers::StringView>() const;
#endif

template<class T> T MaterialData::attribute(const UnsignedInt layer, const UnsignedInt id) const {
    const void* const value = attribute(layer, id);
    #ifdef CORRADE_GRACEFUL_ASSERT
    if(!value) return {};
    #endif
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == data._data.type,
        "Trade::MaterialData::attribute(): improper type requested for" << (data._data.data + 1) << "of" << data._data.type, {});
    return *reinterpret_cast<const T*>(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> Containers::StringView MaterialData::attribute<Containers::StringView>(UnsignedInt, UnsignedInt) const;
#endif

template<class T> T MaterialData::attribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return attribute<T>(layer, id);
}

template<class T> T MaterialData::attribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute<T>(layer, string);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return attribute<T>(layerId, id);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    const UnsignedInt id = attributeFor(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return attribute<T>(layerId, id);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute<T>(layer, string);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::tryAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    if(id == ~UnsignedInt{}) return {};
    return attribute<T>(layer, id);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << name, {});
    return tryAttribute<T>(layer, string);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::tryAttribute(): layer" << layer << "not found", {});
    return tryAttribute<T>(layerId, name);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << name, {});
    return tryAttribute<T>(layer, string);
}

template<class T> T MaterialData::attributeOr(const UnsignedInt layer, const Containers::StringView name, const T& defaultValue) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeOr(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    if(id == ~UnsignedInt{}) return defaultValue;
    return attribute<T>(layer, id);
}

template<class T> T MaterialData::attributeOr(const UnsignedInt layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << name, {});
    return attributeOr<T>(layer, string, defaultValue);
}

template<class T> T MaterialData::attributeOr(const Containers::StringView layer, const Containers::StringView name, const T& defaultValue) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeOr(): layer" << layer << "not found", {});
    return attributeOr<T>(layerId, name, defaultValue);
}

template<class T> T MaterialData::attributeOr(const Containers::StringView layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << name, {});
    return attributeOr<T>(layer, string, defaultValue);
}

}}

#endif
