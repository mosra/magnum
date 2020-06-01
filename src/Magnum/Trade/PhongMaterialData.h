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

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/AbstractMaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief Phong material data

@see @ref AbstractImporter::material()
*/
class MAGNUM_TRADE_EXPORT PhongMaterialData: public AbstractMaterialData {
    public:
        /**
         * @brief Material flag
         *
         * A superset of @ref AbstractMaterialData::Flag.
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedShort {
            /** @copydoc AbstractMaterialData::Flag::DoubleSided */
            DoubleSided = 1 << 0,

            /** The material has an ambient texture */
            AmbientTexture = 1 << 1,

            /** The material has a diffuse texture */
            DiffuseTexture = 1 << 2,

            /** The material has a specular texture */
            SpecularTexture = 1 << 3,

            /**
             * The material has a normal texture
             * @m_since_latest
             */
            NormalTexture = 1 << 4,

            /**
             * The material has a texture coordinate transformation
             * @m_since_latest
             */
            TextureTransformation = 1 << 5,

            /**
             * The material uses non-default texture coordinate sets
             * @m_since_latest
             */
            TextureCoordinateSets = 1 << 6
        };

        /**
         * @brief Material flags
         *
         * A superset of @ref AbstractMaterialData::Flags.
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags             Material flags
         * @param ambientColor      Ambient color. Use
         *      @cpp 0x000000ff_rgbaf @ce for a default value for a
         *      non-textured material and @cpp 0xffffffff_rgbaf @ce for a
         *      default value for a textured material.
         * @param ambientTexture    Ambient texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::AmbientTexture
         * @param diffuseColor      Diffuse color. Use
         *      @cpp 0xffffffff_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param diffuseTexture    Diffuse texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::DiffuseTexture
         * @param specularColor     Specular color. Use
         *      @cpp 0xffffffff_rgbaf @ce for a default value for both a
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
         * @m_since_latest
         *
         * All `*CoordinateSet` accessors are implicitly zero with this
         * constructor. If @p textureMatrix is not default-constructed, expects
         * @ref Flag::TextureTransformation to be enabled as well.
         */
        explicit PhongMaterialData(Flags flags, const Color4& ambientColor, UnsignedInt ambientTexture, const Color4& diffuseColor, UnsignedInt diffuseTexture, const Color4& specularColor, UnsignedInt specularTexture, UnsignedInt normalTexture, const Matrix3& textureMatrix, MaterialAlphaMode alphaMode, Float alphaMask, Float shininess, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct with non-zero texture coordinate sets
         * @param flags                 Material flags
         * @param ambientColor          Ambient color. Use
         *      @cpp 0x000000ff_rgbaf @ce for a default value for a
         *      non-textured material and @cpp 0xffffffff_rgbaf @ce for a
         *      default value for a textured material.
         * @param ambientTexture        Ambient texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::AmbientTexture
         * @param ambientCoordinateSet  Ambient texture coordinate set. Ignored
         *      if @p flags doesn't have @ref Flag::AmbientTexture
         * @param diffuseColor          Diffuse color. Use
         *      @cpp 0xffffffff_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param diffuseTexture        Diffuse texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::DiffuseTexture
         * @param diffuseCoordinateSet  Diffuse texture coordinate set. Ignored
         *      if @p flags doesn't have @ref Flag::DiffuseTexture
         * @param specularColor         Specular color. Use
         *      @cpp 0xffffffff_rgbaf @ce for a default value for both a
         *      non-textured and a textured material.
         * @param specularTexture       Specular texture ID. Ignored if
         *      @p flags doesn't have @ref Flag::SpecularTexture.
         * @param specularCoordinateSet Specular texture coordinate set.
         *      Ignored if @p flags doesn't have @ref Flag::SpecularTexture.
         * @param normalTexture         Normal texture ID. Ignored if @p flags
         *      doesn't have @ref Flag::NormalTexture.
         * @param normalCoordinateSet   Normal texture coordinate set. Ignored
         *      if @p flags doesn't have @ref Flag::NormalTexture.
         * @param textureMatrix         Texture coordinate transformation
         * @param alphaMode             Alpha mode. Use
         *      @ref MaterialAlphaMode::Opaque for a default value.
         * @param alphaMask             Alpha mask value. Use @cpp 0.5f @ce for
         *      a default value.
         * @param shininess             Shininess. Use @cpp 80.0f @ce for a
         *      default value.
         * @param importerState         Importer-specific state
         * @m_since_latest
         *
         * If @p textureMatrix is not default-constructed, expects
         * @ref Flag::TextureTransformation to be enabled as well. If any
         * `*CoordinateSet` is non-zero, expects @ref Flag::TextureCoordinateSets
         * to be enabled as well.
         */
        explicit PhongMaterialData(Flags flags, const Color4& ambientColor, UnsignedInt ambientTexture, UnsignedInt ambientCoordinateSet, const Color4& diffuseColor, UnsignedInt diffuseTexture, UnsignedInt diffuseCoordinateSet, const Color4& specularColor, UnsignedInt specularTexture, UnsignedInt specularCoordinateSet, UnsignedInt normalTexture, UnsignedInt normalCoordinateSet, const Matrix3& textureMatrix, MaterialAlphaMode alphaMode, Float alphaMask, Float shininess, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref PhongMaterialData(Flags, const Color4&, UnsignedInt, const Color4&, UnsignedInt, const Color4&, UnsignedInt, UnsignedInt, const Matrix3&, MaterialAlphaMode, Float, Float, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use PhongMaterialData(Flags, const Color4&, UnsignedInt, const Color4&, UnsignedInt, const Color4&, UnsignedInt, UnsignedInt, const Matrix3&, MaterialAlphaMode, Float, Float, const void*) instead") PhongMaterialData(Flags flags, MaterialAlphaMode alphaMode, Float alphaMask, Float shininess, const void* importerState = nullptr) noexcept;

        /**
         * @brief Constructor
         * @m_deprecated_since{2018,10} Use @ref PhongMaterialData(Flags, MaterialAlphaMode, Float, Float, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use PhongMaterialData(Flags, MaterialAlphaMode, Float, Float, const void*) instead") PhongMaterialData(Flags flags, Float shininess, const void* importerState = nullptr) noexcept;
        #endif

        /** @brief Copying is not allowed */
        PhongMaterialData(const PhongMaterialData&) = delete;

        /** @brief Move constructor */
        PhongMaterialData(PhongMaterialData&& other) noexcept;

        /** @brief Copying is not allowed */
        PhongMaterialData& operator=(const PhongMaterialData&) = delete;

        /** @brief Move assignment */
        PhongMaterialData& operator=(PhongMaterialData&& other) noexcept;

        /**
         * @brief Material flags
         *
         * A superset of @ref AbstractMaterialData::flags().
         */
        Flags flags() const {
            return Flag(UnsignedShort(AbstractMaterialData::flags()));
        }

        /**
         * @brief Ambient color
         *
         * If the material has @ref Flag::AmbientTexture, the color and texture
         * is multiplied together.
         * @see @ref flags()
         */
        Color4 ambientColor() const { return _ambientColor; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @m_deprecated_since_latest Use the constructor to populate all
         *      values instead.
         */
        /* Not marked with CORRADE_DEPRECATED() because the compiler picks this
           overload over the const one for a non-const instance and there's no
           way to selectively enable deprecation warnings only if the code
           attempts to modify the value. */
        Color4& ambientColor() { return _ambientColor; }
        #endif

        /**
         * @brief Ambient texture ID
         *
         * Available only if the material has @ref Flag::AmbientTexture.
         * Multiplied with @ref ambientColor().
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt ambientTexture() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @m_deprecated_since_latest Use the constructor to populate all
         *      values instead of modifying the instance afterwards.
         */
        /* Not using CORRADE_DEPRECATED(), see why above */
        UnsignedInt& ambientTexture();
        #endif

        /**
         * @brief Ambient texture coordinate set
         * @m_since_latest
         *
         * Available only if the material has @ref Flag::AmbientTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt ambientCoordinateSet() const;

        /**
         * @brief Diffuse color
         *
         * If the material has @ref Flag::DiffuseTexture, the color and texture
         * is multiplied together.
         * @see @ref flags()
         */
        Color4 diffuseColor() const { return _diffuseColor; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @m_deprecated_since_latest Use the constructor to populate all
         *      values instead of modifying the instance afterwards.
         */
        /* Not using CORRADE_DEPRECATED(), see why above */
        Color4& diffuseColor() { return _diffuseColor; }
        #endif

        /**
         * @brief Diffuse texture ID
         *
         * Available only if the material has @ref Flag::DiffuseTexture.
         * Multiplied with @ref diffuseColor().
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt diffuseTexture() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @m_deprecated_since_latest Use the constructor to populate all
         *      values instead of modifying the instance afterwards.
         */
        /* Not using CORRADE_DEPRECATED(), see why above */
        UnsignedInt& diffuseTexture();
        #endif

        /**
         * @brief Diffuse texture coordinate set
         * @m_since_latest
         *
         * Available only if the material has @ref Flag::DiffuseTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt diffuseCoordinateSet() const;

        /**
         * @brief Specular color
         *
         * If the material has @ref Flag::SpecularTexture, the color and
         * texture is multiplied together.
         * @see @ref flags()
         */
        Color4 specularColor() const { return _specularColor; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @m_deprecated_since_latest Use the constructor to populate all
         *      values instead of modifying the instance afterwards.
         */
        /* Not using CORRADE_DEPRECATED(), see why above */
        Color4& specularColor() { return _specularColor; }
        #endif

        /**
         * @brief Specular texture ID
         *
         * Available only if the material has @ref Flag::SpecularTexture.
         * Multiplied with @ref specularColor().
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt specularTexture() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @m_deprecated_since_latest Use the constructor to populate all
         *      values instead of modifying the instance afterwards.
         */
        /* Not using CORRADE_DEPRECATED(), see why above */
        UnsignedInt& specularTexture();
        #endif

        /**
         * @brief Specular texture coordinate set
         * @m_since_latest
         *
         * Available only if the material has @ref Flag::SpecularTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt specularCoordinateSet() const;

        /**
         * @brief Normal texture ID
         * @m_since_latest
         *
         * Available only if the material has @ref Flag::NormalTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt normalTexture() const;

        /**
         * @brief Normal texture coordinate set
         * @m_since_latest
         *
         * Available only if the material has @ref Flag::NormalTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt normalCoordinateSet() const;

        /**
         * @brief Texture coordinate transformation matrix
         * @m_since_latest
         *
         * If the material doesn't have @ref Flag::TextureTransformation,
         * returns an identity matrix.
         * @see @ref flags()
         */
        Matrix3 textureMatrix() const { return _textureMatrix; }

        /** @brief Shininess */
        Float shininess() const { return _shininess; }

    private:
        /* Initializing texture IDs to insane values to make accidents worse
           and thus better noticeable */
        Color4 _ambientColor;
        UnsignedInt _ambientTexture{~UnsignedInt{}};
        UnsignedInt _ambientCoordinateSet;
        Color4 _diffuseColor;
        UnsignedInt _diffuseTexture{~UnsignedInt{}};
        UnsignedInt _diffuseCoordinateSet;
        Color4 _specularColor;
        UnsignedInt _specularTexture{~UnsignedInt{}};
        UnsignedInt _specularCoordinateSet;
        UnsignedInt _normalTexture{~UnsignedInt{}};
        UnsignedInt _normalCoordinateSet;
        Matrix3 _textureMatrix;
        Float _shininess;
};

CORRADE_ENUMSET_OPERATORS(PhongMaterialData::Flags)

/** @debugoperatorclassenum{PhongMaterialData,PhongMaterialData::Flag} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, PhongMaterialData::Flag value);

/** @debugoperatorclassenum{PhongMaterialData,PhongMaterialData::Flags} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, PhongMaterialData::Flags value);

}}

#endif
