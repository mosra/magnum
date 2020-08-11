#ifndef Magnum_Trade_MaterialLayerData_h
#define Magnum_Trade_MaterialLayerData_h
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
 * @brief Class @ref Magnum::Trade::MaterialLayerData
 * @m_since_latest
 */

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief Material layer data
@m_since_latest

Convenience wrapper that re-routes all @ref MaterialData base material layer
and attribute accessors APIS from to a layer specified in the @p layer template
parameter. All APIs expect that given layer exists. See
@ref Trade-MaterialData-usage-layers for more information.
*/
template<MaterialLayer layer> class MaterialLayerData: public MaterialData {
    public:
        /* Allow constructing subclasses directly. While not used in the
           general Importer workflow, it allows users to create instances with
           desired convenience APIs easier (and simplifies testing) */
        using MaterialData::MaterialData;

        /**
         * @brief Layer name
         *
         * Same as calling @ref MaterialData::layerName() with @ref layerId()
         * for @p layer.
         */
        Containers::StringView layerName() const {
            return MaterialData::layerName(layerId(layer));
        }

        /**
         * @brief Layer factor
         *
         * Same as calling @ref MaterialData::layerFactor() with @p layer.
         */
        Float layerFactor() const {
            return MaterialData::layerFactor(layer);
        }

        /**
         * @brief Layer factor texture ID
         *
         * Same as calling @ref MaterialData::layerFactorTexture() with
         * @p layer.
         */
        UnsignedInt layerFactorTexture() const {
            return MaterialData::layerFactorTexture(layer);
        }

        /**
         * @brief Layer factor texture swizzle
         *
         * Same as calling @ref MaterialData::layerFactorTextureSwizzle() with
         * @p layer.
         */
        MaterialTextureSwizzle layerFactorTextureSwizzle() const {
            return MaterialData::layerFactorTextureSwizzle(layer);
        }

        /**
         * @brief Layer factor texture coordinate transformation matrix
         *
         * Same as calling @ref MaterialData::layerFactorTextureMatrix() with
         * @p layer.
         */
        Matrix3 layerFactorTextureMatrix() const {
            return MaterialData::layerFactorTextureMatrix(layer);
        }

        /**
         * @brief Layer factor texture coordinate set
         *
         * Same as calling @ref MaterialData::layerFactorTextureCoordinates()
         * with @p layer.
         */
        UnsignedInt layerFactorTextureCoordinates() const {
            return MaterialData::layerFactorTextureCoordinates(layer);
        }

        /**
         * @brief Attribute count in this layer
         *
         * Same as calling @ref MaterialData::attributeCount() with @p layer.
         */
        UnsignedInt attributeCount() const {
            return MaterialData::attributeCount(layer);
        }

        /**
         * @brief Whether this layer has given attribute
         *
         * Same as calling @ref MaterialData::hasAttribute() with @p layer.
         */
        bool hasAttribute(Containers::StringView name) const {
            return MaterialData::hasAttribute(layer, name);
        }
        bool hasAttribute(MaterialAttribute name) const {
            return MaterialData::hasAttribute(layer, name);
        } /**< @overload */

        /**
         * @brief ID of a named attribute in this layer
         *
         * Same as calling @ref MaterialData::attributeId() with @p layer.
         */
        UnsignedInt attributeId(Containers::StringView name) const {
            return MaterialData::attributeId(layer, name);
        }
        UnsignedInt attributeId(MaterialAttribute name) const {
            return MaterialData::attributeId(layer, name);
        } /**< @overload */

        /**
         * @brief Name of an attribute in this layer
         *
         * Same as calling @ref MaterialData::attributeName() with @p layer.
         */
        Containers::StringView attributeName(UnsignedInt id) const {
            return MaterialData::attributeName(layer, id);
        }

        /**
         * @brief Type of an attribute in this layer
         *
         * Same as calling @ref MaterialData::attributeType() with @p layer.
         */
        MaterialAttributeType attributeType(UnsignedInt id) const {
            return MaterialData::attributeType(layer, id);
        }
        MaterialAttributeType attributeType(Containers::StringView name) const {
            return MaterialData::attributeType(layer, name);
        } /**< @overload */
        MaterialAttributeType attributeType(MaterialAttribute name) const {
            return MaterialData::attributeType(layer, name);
        } /**< @overload */

        /**
         * @brief Type-erased value of an attribute in this layer
         *
         * Same as calling @ref MaterialData::attribute() with @p layer.
         */
        const void* attribute(UnsignedInt id) const {
            return MaterialData::attribute(layer, id);
        }
        const void* attribute(Containers::StringView name) const {
            return MaterialData::attribute(layer, name);
        } /**< @overload */
        const void* attribute(MaterialAttribute name) const {
            return MaterialData::attribute(layer, name);
        } /**< @overload */

        /**
         * @brief Value of an attribute in this layer
         *
         * Same as calling @ref MaterialData::attribute() with @p layer.
         */
        template<class T> T attribute(UnsignedInt id) const {
            return MaterialData::attribute<T>(layer, id);
        }
        template<class T> T attribute(Containers::StringView name) const {
            return MaterialData::attribute<T>(layer, name);
        } /**< @overload */
        template<class T> T attribute(MaterialAttribute name) const {
            return MaterialData::attribute<T>(layer, name);
        } /**< @overload */

        /**
         * @brief Type-erased attribute value in this layer, if exists
         *
         * Same as calling @ref MaterialData::tryAttribute() with @p layer.
         */
        const void* tryAttribute(Containers::StringView name) const {
            return MaterialData::tryAttribute(layer, name);
        }
        const void* tryAttribute(MaterialAttribute name) const {
            return MaterialData::tryAttribute(layer, name);
        } /**< @overload */

        /**
         * @brief Value of a named attribute in this layer, if exists
         *
         * Same as calling @ref MaterialData::tryAttribute() with @p layer.
         */
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView name) const {
            return MaterialData::tryAttribute<T>(layer, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(MaterialAttribute name) const {
            return MaterialData::tryAttribute<T>(layer, name);
        } /**< @overload */

        /**
         * @brief Value of a named attribute in this layer or a default
         *
         * Same as calling @ref MaterialData::attributeOr() with @p layer.
         */
        template<class T> T attributeOr(Containers::StringView name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer, name, defaultValue);
        }
        template<class T> T attributeOr(MaterialAttribute name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer, name, defaultValue);
        } /**< @overload */

        /* Bring in all other overloads as well and override just the ones
           with implicit layers */
        using MaterialData::layerName;
        using MaterialData::layerFactor;
        using MaterialData::layerFactorTexture;
        using MaterialData::layerFactorTextureSwizzle;
        using MaterialData::layerFactorTextureMatrix;
        using MaterialData::layerFactorTextureCoordinates;
        /* MSVC is so damn unbelievably stupid it's setting my ass on fire.
           https://en.cppreference.com/w/cpp/language/using_declaration says
           that "If the derived class already has a member with the same name,
           parameter list, and qualifications, the derived class member hides
           or overrides (doesn't conflict with) the member that is introduced
           from the base class." and EVERY COMPILER ON EARTH EXCEPT THIS DAMN
           POOL OF GOO implements that correctly. So instead of a bunch of sane
           using declarations I have to enumerate EACH AND EVERY overload and
           explicitly pass them through LIKE A FUCKING SAVAGE. FFS. */
        #if !defined(CORRADE_TARGET_MSVC) || defined(CORRADE_TARGET_CLANG_CL)
        using MaterialData::attributeCount;
        using MaterialData::hasAttribute;
        using MaterialData::attributeId;
        using MaterialData::attributeName;
        using MaterialData::attributeType;
        using MaterialData::attribute;
        using MaterialData::tryAttribute;
        using MaterialData::attributeOr;
        #else
        UnsignedInt attributeCount(UnsignedInt layer_) const {
            return MaterialData::attributeCount(layer_);
        }
        UnsignedInt attributeCount(Containers::StringView layer_) const {
            return MaterialData::attributeCount(layer_);
        }
        UnsignedInt attributeCount(MaterialLayer layer_) const {
            return MaterialData::attributeCount(layer_);
        }

        bool hasAttribute(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::hasAttribute(layer_, name);
        }
        bool hasAttribute(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::hasAttribute(layer_, name);
        }
        bool hasAttribute(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::hasAttribute(layer_, name);
        }
        bool hasAttribute(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::hasAttribute(layer_, name);
        }
        bool hasAttribute(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::hasAttribute(layer_, name);
        }
        bool hasAttribute(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::hasAttribute(layer_, name);
        }

        UnsignedInt attributeId(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::attributeId(layer_, name);
        }
        UnsignedInt attributeId(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::attributeId(layer_, name);
        }
        UnsignedInt attributeId(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::attributeId(layer_, name);
        }
        UnsignedInt attributeId(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::attributeId(layer_, name);
        }
        UnsignedInt attributeId(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::attributeId(layer_, name);
        }
        UnsignedInt attributeId(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::attributeId(layer_, name);
        }

        Containers::StringView attributeName(UnsignedInt layer_, UnsignedInt id) const {
            return MaterialData::attributeName(layer_, id);
        }
        Containers::StringView attributeName(Containers::StringView layer_, UnsignedInt id) const {
            return MaterialData::attributeName(layer_, id);
        }
        Containers::StringView attributeName(MaterialLayer layer_, UnsignedInt id) const {
            return MaterialData::attributeName(layer_, id);
        }

        MaterialAttributeType attributeType(UnsignedInt layer_, UnsignedInt id) const {
            return MaterialData::attributeType(layer_, id);
        }
        MaterialAttributeType attributeType(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::attributeType(layer_, name);
        }
        MaterialAttributeType attributeType(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::attributeType(layer_, name);
        }
        MaterialAttributeType attributeType(Containers::StringView layer_, UnsignedInt id) const {
            return MaterialData::attributeType(layer_, id);
        }
        MaterialAttributeType attributeType(MaterialLayer layer_, UnsignedInt id) const {
            return MaterialData::attributeType(layer_, id);
        }
        MaterialAttributeType attributeType(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::attributeType(layer_, name);
        }
        MaterialAttributeType attributeType(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::attributeType(layer_, name);
        }
        MaterialAttributeType attributeType(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::attributeType(layer_, name);
        }
        MaterialAttributeType attributeType(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::attributeType(layer_, name);
        }

        const void* attribute(UnsignedInt layer_, UnsignedInt id) const {
            return MaterialData::attribute(layer_, id);
        }
        const void* attribute(Containers::StringView layer_, UnsignedInt id) const {
            return MaterialData::attribute(layer_, id);
        }
        const void* attribute(MaterialLayer layer_, UnsignedInt id) const {
            return MaterialData::attribute(layer_, id);
        }
        const void* attribute(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::attribute(layer_, name);
        }
        const void* attribute(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::attribute(layer_, name);
        }
        const void* attribute(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::attribute(layer_, name);
        }
        const void* attribute(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::attribute(layer_, name);
        }
        const void* attribute(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::attribute(layer_, name);
        }
        const void* attribute(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::attribute(layer_, name);
        }

        template<class T> T attribute(UnsignedInt layer_, UnsignedInt id) const {
            return MaterialData::attribute<T>(layer_, id);
        }
        template<class T> T attribute(Containers::StringView layer_, UnsignedInt id) const {
            return MaterialData::attribute<T>(layer_, id);
        }
        template<class T> T attribute(MaterialLayer layer_, UnsignedInt id) const {
            return MaterialData::attribute<T>(layer_, id);
        }
        template<class T> T attribute(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::attribute<T>(layer_, name);
        }
        template<class T> T attribute(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::attribute<T>(layer_, name);
        }
        template<class T> T attribute(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::attribute<T>(layer_, name);
        }
        template<class T> T attribute(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::attribute<T>(layer_, name);
        }
        template<class T> T attribute(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::attribute<T>(layer_, name);
        }
        template<class T> T attribute(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::attribute<T>(layer_, name);
        }

        const void* tryAttribute(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::tryAttribute(layer_, name);
        }
        const void* tryAttribute(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::tryAttribute(layer_, name);
        }
        const void* tryAttribute(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::tryAttribute(layer_, name);
        }
        const void* tryAttribute(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::tryAttribute(layer_, name);
        }
        const void* tryAttribute(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::tryAttribute(layer_, name);
        }
        const void* tryAttribute(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::tryAttribute(layer_, name);
        }

        template<class T> Containers::Optional<T> tryAttribute(UnsignedInt layer_, Containers::StringView name) const {
            return MaterialData::tryAttribute<T>(layer_, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(UnsignedInt layer_, MaterialAttribute name) const {
            return MaterialData::tryAttribute<T>(layer_, name);
        }

        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView layer_, Containers::StringView name) const {
            return MaterialData::tryAttribute<T>(layer_, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView layer_, MaterialAttribute name) const {
            return MaterialData::tryAttribute<T>(layer_, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(MaterialLayer layer_, Containers::StringView name) const {
            return MaterialData::tryAttribute<T>(layer_, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(MaterialLayer layer_, MaterialAttribute name) const {
            return MaterialData::tryAttribute<T>(layer_, name);
        }

        template<class T> T attributeOr(UnsignedInt layer_, Containers::StringView name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer_, name, defaultValue);
        }
        template<class T> T attributeOr(UnsignedInt layer_, MaterialAttribute name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer_, name, defaultValue);
        }
        template<class T> T attributeOr(Containers::StringView layer_, Containers::StringView name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer_, name, defaultValue);
        }
        template<class T> T attributeOr(Containers::StringView layer_, MaterialAttribute name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer_, name, defaultValue);
        }
        template<class T> T attributeOr(MaterialLayer layer_, Containers::StringView name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer_, name, defaultValue);
        }
        template<class T> T attributeOr(MaterialLayer layer_, MaterialAttribute name, const T& defaultValue) const {
            return MaterialData::attributeOr<T>(layer_, name, defaultValue);
        }
        #endif
};

}}

#endif
