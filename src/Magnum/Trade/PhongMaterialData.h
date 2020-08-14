#ifndef Magnum_Trade_PhongMaterialData_h
#define Magnum_Trade_PhongMaterialData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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
 * @brief Class @ref Magnum::Trade::PhongMaterialData
 */

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief Phong material data

See @ref Trade-MaterialData-usage-types for more information about how to use
this class.
@see @ref AbstractImporter::material(), @ref FlatMaterialData,
    @ref PbrMetallicRoughnessMaterialData,
    @ref PbrSpecularGlossinessMaterialData, @ref PbrClearCoatMaterialData
*/
class MAGNUM_TRADE_EXPORT PhongMaterialData: public MaterialData {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material flag
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided(), @ref hasTextureTransformation(),
         *      @ref hasTextureCoordinates() etc.
         *
         * A superset of @ref MaterialData::Flag.
         * @see @ref Flags, @ref flags()
         */
        enum class CORRADE_DEPRECATED_ENUM("use hasAttribute() etc. instead") Flag: UnsignedInt {
            /** @copydoc MaterialData::Flag::DoubleSided */
            DoubleSided = 1 << 0,

            /** The material has an ambient texture */
            AmbientTexture = 1 << 1,

            /** The material has a diffuse texture */
            DiffuseTexture = 1 << 2,

            /** The material has a specular texture */
            SpecularTexture = 1 << 3,

            /**
             * The material has a normal texture
             * @m_since{2020,06}
             */
            NormalTexture = 1 << 4,

            /**
             * The material has a texture coordinate transformation
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 5,

            /**
             * The material uses non-default texture coordinate sets
             * @m_since_latest
             */
            TextureCoordinates = 1 << 6,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * The material uses non-default texture coordinate sets
             * @m_deprecated_since_latest Use @ref Flag::TextureCoordinates
             *      instead.
             */
            TextureCoordinateSets CORRADE_DEPRECATED_ENUM("use Flag::TextureCoordinates instead") = TextureCoordinates
            #endif
        };

        /**
         * @brief Material flags
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided(), @ref hasTextureTransformation(),
         *      @ref hasTextureCoordinates() etc.
         *
         * A superset of @ref MaterialData::Flags.
         * @see @ref flags()
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        typedef CORRADE_DEPRECATED("use hasAttribute() etc. instead") Containers::EnumSet<Flag> Flags;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Allow constructing subclasses directly. While not used in the
           general Importer workflow, it allows users to create instances with
           desired convenience APIs easier (and simplifies testing). It's
           however hidden from the docs as constructing instances this way
           isn't really common and it would add a lot of noise. */
        using MaterialData::MaterialData;
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @param flags             Material flags
         * @param ambientColor      Ambient color. Use
         *      @cpp 0x000000ff_rgbaf @ce for a default value for a
         *      non-textured material and @cpp 0xffffffff_rgbaf @ce for a
         *      default value for a textured material.
         * @param ambientTexture    Ambient texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::AmbientTexture.
         * @param diffuseColor      Diffuse color. Use
         *      @cpp 0xffffffff_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param diffuseTexture    Diffuse texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::DiffuseTexture.
         * @param specularColor     Specular color. Use
         *      @cpp 0xffffff00_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param specularTexture   Specular texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::SpecularTexture.
         * @param normalTexture     Normal texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::NormalTexture.
         * @param textureMatrix     Texture coordinate transformation
         * @param alphaMode         Alpha mode. Use
         *      @ref MaterialAlphaMode::Opaque for a default value.
         * @param alphaMask         Alpha mask value. Use @cpp 0.5f @ce for a
         *      default value.
         * @param shininess         Shininess. Use @cpp 80.0f @ce for a default
         *      value.
         * @param importerState     Importer-specific state
         *
         * All `*TextureCoordinates()` accessors are implicitly zero with this
         * constructor. If @p textureMatrix is not default-constructed, expects
         * @ref Flag::TextureTransformation to be enabled as well.
         *
         * @m_deprecated_since_latest Populate a @ref MaterialData instance
         *      using @ref MaterialData::MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*)
         *      instead. This class is not meant to be constructed directly
         *      anymore.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        explicit CORRADE_DEPRECATED("use MaterialData::MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*) instead") PhongMaterialData(Flags flags, const Color4& ambientColor, UnsignedInt ambientTexture, const Color4& diffuseColor, UnsignedInt diffuseTexture, const Color4& specularColor, UnsignedInt specularTexture, UnsignedInt normalTexture, const Matrix3& textureMatrix, MaterialAlphaMode alphaMode, Float alphaMask, Float shininess, const void* importerState = nullptr) noexcept;
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Construct with non-zero texture coordinate sets
         * @param flags                 Material flags
         * @param ambientColor          Ambient color. Use
         *      @cpp 0x000000ff_rgbaf @ce for a default value for a
         *      non-textured material and @cpp 0xffffffff_rgbaf @ce for a
         *      default value for a textured material.
         * @param ambientTexture        Ambient texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::AmbientTexture.
         * @param ambientTextureCoordinates Ambient texture coordinate set.
         *      Ignored if @p flags doesn't have @ref Flag::AmbientTexture
         * @param diffuseColor          Diffuse color. Use
         *      @cpp 0xffffffff_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param diffuseTexture        Diffuse texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::DiffuseTexture.
         * @param diffuseTextureCoordinates Diffuse texture coordinate set.
         *      Ignored if @p flags doesn't have @ref Flag::DiffuseTexture
         * @param specularColor         Specular color. Use
         *      @cpp 0xffffff00_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param specularTexture       Specular texture ID. Ignored if
         *      @p flags doesn't have @ref Flag::SpecularTexture.
         * @param specularTextureCoordinates Specular texture coordinate set.
         *      Ignored if @p flags doesn't have @ref Flag::SpecularTexture.
         * @param normalTexture         Normal texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::NormalTexture.
         * @param normalTextureCoordinates Normal texture coordinate set.
         *      Ignored if @p flags doesn't have @ref Flag::NormalTexture.
         * @param textureMatrix         Texture coordinate transformation
         * @param alphaMode             Alpha mode. Use
         *      @ref MaterialAlphaMode::Opaque for a default value.
         * @param alphaMask             Alpha mask value. Use @cpp 0.5f @ce for
         *      a default value.
         * @param shininess             Shininess. Use @cpp 80.0f @ce for a
         *      default value.
         * @param importerState         Importer-specific state
         *
         * If @p textureMatrix is not default-constructed, expects
         * @ref Flag::TextureTransformation to be enabled as well. If any
         * `*Coordinates` parameter is non-zero, expects
         * @ref Flag::TextureCoordinates to be enabled as well.
         *
         * @m_deprecated_since_latest Populate a @ref MaterialData instance
         *      using @ref MaterialData::MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*)
         *      instead. This class is not meant to be constructed directly
         *      anymore.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        explicit CORRADE_DEPRECATED("use MaterialData::MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*) instead") PhongMaterialData(Flags flags, const Color4& ambientColor, UnsignedInt ambientTexture, UnsignedInt ambientTextureCoordinates, const Color4& diffuseColor, UnsignedInt diffuseTexture, UnsignedInt diffuseTextureCoordinates, const Color4& specularColor, UnsignedInt specularTexture, UnsignedInt specularTextureCoordinates, UnsignedInt normalTexture, UnsignedInt normalTextureCoordinates, const Matrix3& textureMatrix, MaterialAlphaMode alphaMode, Float alphaMask, Float shininess, const void* importerState = nullptr) noexcept;
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Constructor
         * @m_deprecated_since{2020,06} Populate a @ref MaterialData instance
         *      using @ref MaterialData::MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*)
         *      instead. This class is not meant to be constructed directly
         *      anymore.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        explicit CORRADE_DEPRECATED("use MaterialData::MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*) instead") PhongMaterialData(Flags flags, MaterialAlphaMode alphaMode, Float alphaMask, Float shininess, const void* importerState = nullptr) noexcept;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Whether the material has a specular texture
         * @m_since_latest
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::SpecularTexture or
         * @ref MaterialAttribute::SpecularGlossinessTexture attributes is
         * present, @cpp false @ce otherwise.
         */
        bool hasSpecularTexture() const;

        /**
         * @brief Whether the material has texture transformation
         * @m_since_latest
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::AmbientTextureMatrix,
         * @ref MaterialAttribute::DiffuseTextureMatrix,
         * @ref MaterialAttribute::SpecularTextureMatrix,
         * @ref MaterialAttribute::NormalTextureMatrix or
         * @ref MaterialAttribute::TextureMatrix attributes is present,
         * @cpp false @ce otherwise.
         * @see @ref hasCommonTextureTransformation()
         */
        bool hasTextureTransformation() const;

        /**
         * @brief Whether the material has a common transformation for all textures
         * @m_since_latest
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref ambientTextureMatrix(), @ref diffuseTextureMatrix(),
         * @ref specularTextureMatrix() and @ref normalTextureMatrix() have the
         * same value, @cpp false @ce otherwise. In particular, returns
         * @cpp true @ce also if there's no texture transformation at all. Use
         * @ref hasTextureTransformation() to distinguish that case.
         */
        bool hasCommonTextureTransformation() const;

        /**
         * @brief Whether the material uses extra texture coordinate sets
         * @m_since_latest
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::AmbientTextureCoordinates,
         * @ref MaterialAttribute::DiffuseTextureCoordinates,
         * @ref MaterialAttribute::SpecularTextureCoordinates,
         * @ref MaterialAttribute::NormalTextureCoordinates or
         * @ref MaterialAttribute::TextureCoordinates attributes is present and
         * has a non-zero value, @cpp false @ce otherwise.
         * @see @ref hasCommonTextureCoordinates()
         */
        bool hasTextureCoordinates() const;

        /**
         * @brief Whether the material has a common coordinate set for all textures
         * @m_since_latest
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref ambientTextureCoordinates(), @ref diffuseTextureCoordinates(),
         * @ref specularTextureCoordinates() and @ref normalTextureCoordinates()
         * have the same value, @cpp false @ce otherwise. In particular,
         * returns @cpp true @ce also if there's no extra texture coordinate
         * set used at all. Use @ref hasTextureCoordinates() to distinguish
         * that case.
         */
        bool hasCommonTextureCoordinates() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material flags
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided(), @ref hasTextureTransformation(),
         *      @ref hasTextureCoordinates() etc.
         *
         * A superset of @ref MaterialData::flags().
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        CORRADE_DEPRECATED("use hasAttribute() instead") Flags flags() const;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Ambient color
         *
         * Convenience access to the @ref MaterialAttribute::AmbientColor
         * attribute. If not present, the default is either
         * @cpp 0xffffffff_rgbaf @ce if there's
         * @ref MaterialAttribute::AmbientTexture and @cpp 0x000000ff_rgbaf @ce
         * otherwise.
         *
         * If the material has @ref MaterialAttribute::AmbientTexture, the
         * color and texture is meant to be multiplied together.
         * @see @ref hasAttribute()
         */
        Color4 ambientColor() const;

        /**
         * @brief Ambient texture ID
         *
         * Available only if @ref MaterialAttribute::AmbientTexture is
         * present. Meant to be multiplied with @ref ambientColor().
         * @see @ref hasAttribute()
         */
        UnsignedInt ambientTexture() const;

        /**
         * @brief Ambient texture coordinate transformation matrix
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::AmbientTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::AmbientTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 ambientTextureMatrix() const;

        /**
         * @brief Ambient texture coordinate set
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::AmbientTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::AmbientTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt ambientTextureCoordinates() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ambientTextureCoordinates()
         * @m_deprecated_since_latest Use @ref ambientTextureCoordinates()
         *      instead.
         */
        CORRADE_DEPRECATED("use ambientTextureCoordinates() instead") UnsignedInt ambientCoordinateSet() const {
            return ambientTextureCoordinates();
        }
        #endif

        /**
         * @brief Diffuse color
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseColor
         * attribute. If not present, the default is @cpp 0xffffffff_rgbaf @ce.
         *
         * If the material has @ref MaterialAttribute::DiffuseTexture, the
         * color and texture is meant to be multiplied together.
         * @see @ref hasAttribute()
         */
        Color4 diffuseColor() const;

        /**
         * @brief Diffuse texture ID
         *
         * Available only if @ref MaterialAttribute::DiffuseTexture is
         * present. Meant to be multiplied with @ref diffuseColor().
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt diffuseTexture() const;

        /**
         * @brief Diffuse texture coordinate transformation matrix
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::DiffuseTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 diffuseTextureMatrix() const;

        /**
         * @brief Diffuse texture coordinate set
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::DiffuseTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt diffuseTextureCoordinates() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief diffuseTextureCoordinates()
         * @m_deprecated_since_latest Use @ref diffuseTextureCoordinates()
         *      instead.
         */
        CORRADE_DEPRECATED("use diffuseTextureCoordinates() instead") UnsignedInt diffuseCoordinateSet() const {
            return diffuseTextureCoordinates();
        }
        #endif

        /**
         * @brief Specular color
         *
         * Convenience access to the @ref MaterialAttribute::SpecularColor
         * attribute. If not present, the default is @cpp 0xffffff00_rgbaf @ce.
         *
         * If the material has a specular texture, the color and texture is
         * meant to be multiplied together.
         * @see @ref hasSpecularTexture()
         */
        Color4 specularColor() const;

        /**
         * @brief Specular texture ID
         *
         * Available only if either @ref MaterialAttribute::SpecularTexture or
         * @ref MaterialAttribute::SpecularGlossinessTexture is present. Meant
         * to be multiplied with @ref specularColor().
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt specularTexture() const;

        /**
         * @brief Specular texture swizzle
         * @m_since_latest
         *
         * If @ref MaterialAttribute::SpecularGlossinessTexture is present,
         * returns always @ref MaterialTextureSwizzle::RGB. Otherwise returns
         * the @ref MaterialAttribute::SpecularTextureSwizzle attribute, or
         * @ref MaterialTextureSwizzle::RGB if it's not present. Available only
         * if the material has a specular texture.
         * @see @ref hasSpecularTexture()
         */
        MaterialTextureSwizzle specularTextureSwizzle() const;

        /**
         * @brief Specular texture coordinate transformation matrix
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::SpecularTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has a specular texture.
         * @see @ref hasSpecularTexture()
         */
        Matrix3 specularTextureMatrix() const;

        /**
         * @brief Specular texture coordinate set
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::SpecularTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has a specular texture.
         * @see @ref hasSpecularTexture()
         */
        UnsignedInt specularTextureCoordinates() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief specularTextureCoordinates()
         * @m_deprecated_since_latest Use @ref specularTextureCoordinates()
         *      instead.
         */
        CORRADE_DEPRECATED("use specularTextureCoordinates() instead") UnsignedInt specularCoordinateSet() const {
            return specularTextureCoordinates();
        }
        #endif

        /**
         * @brief Normal texture ID
         * @m_since{2020,06}
         *
         * Available only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt normalTexture() const;

        /**
         * @brief Normal texture scale
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureScale
         * attribute. If not present, the default is @cpp 1.0f @ce.
         * Available only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute()
         */
        Float normalTextureScale() const;

        /**
         * @brief Normal texture swizzle
         * @m_since_latest
         *
         * Convenience access to the
         * @ref MaterialAttribute::NormalTextureSwizzle attribute. If not
         * present, the default is @ref MaterialTextureSwizzle::RGB. Available
         * only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle normalTextureSwizzle() const;

        /**
         * @brief Normal texture coordinate transformation matrix
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::NormalTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 normalTextureMatrix() const;

        /**
         * @brief Normal texture coordinate set
         * @m_since_latest
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::NormalTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt normalTextureCoordinates() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief normalTextureCoordinates()
         * @m_deprecated_since_latest Use @ref normalTextureCoordinates()
         *      instead.
         */
        CORRADE_DEPRECATED("use normalTextureCoordinates() instead") UnsignedInt normalCoordinateSet() const {
            return normalTextureCoordinates();
        }
        #endif

        /**
         * @brief Common texture coordinate transformation matrix for all textures
         * @m_since_latest
         *
         * Expects that @ref hasCommonTextureTransformation() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref ambientTextureMatrix(), @ref diffuseTextureMatrix(),
         * @ref specularTextureMatrix() and @ref normalTextureMatrix() where a
         * texture is present. If no texture is present, returns an identity
         * matrix.
         */
        Matrix3 commonTextureMatrix() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Common texture coordinate transformation matrix for all textures
         * @m_deprecated_since_latest Because the material may now also define
         *      per-texture transformations, which take precedence over the
         *      common one, this value is misleading. Use either
         *      @ref commonTextureMatrix() or separate
         *      @ref ambientTextureMatrix(), @ref diffuseTextureMatrix(),
         *      @ref specularTextureMatrix() and @ref normalTextureMatrix()
         *      accessors instead.
         */
        CORRADE_DEPRECATED("use commonTextureMatrix() or per-texture accessors instead") Matrix3 textureMatrix() const;
        #endif

        /**
         * @brief Common texture coordinate set index for all textures
         * @m_since_latest
         *
         * Expects that @ref hasCommonTextureCoordinates() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref ambientTextureCoordinates(), @ref diffuseTextureCoordinates(),
         * @ref specularTextureCoordinates() and @ref normalTextureCoordinates()
         * where a texture is present. If no texture is present, returns
         * @cpp 0 @ce.
         */
        UnsignedInt commonTextureCoordinates() const;

        /**
         * @brief Shininess
         *
         * Convenience access to the @ref MaterialAttribute::Shininess
         * attribute. If not present, the default is @cpp 80.0f @ce.
         */
        Float shininess() const;
};

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(PhongMaterialData::Flags)

/**
@debugoperatorclassenum{PhongMaterialData,PhongMaterialData::Flag}
@m_deprecated_since_latest The flags are no longer stored directly but
    generated on-the-fly from attribute data, which makes them less efficient
    than calling @ref MaterialData::hasAttribute(),
    @ref MaterialData::isDoubleSided(),
    @ref PhongMaterialData::hasTextureTransformation(),
    @ref PhongMaterialData::hasTextureCoordinates() etc.
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, PhongMaterialData::Flag value);

/**
@debugoperatorclassenum{PhongMaterialData,PhongMaterialData::Flags}
@m_deprecated_since_latest The flags are no longer stored directly but
    generated on-the-fly from attribute data, which makes them less efficient
    than calling @ref MaterialData::hasAttribute(),
    @ref MaterialData::isDoubleSided(),
    @ref PhongMaterialData::hasTextureTransformation(),
    @ref PhongMaterialData::hasTextureCoordinates() etc.
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, PhongMaterialData::Flags value);
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}

#endif
