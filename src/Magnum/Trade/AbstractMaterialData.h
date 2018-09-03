#ifndef Magnum_Trade_AbstractMaterialData_h
#define Magnum_Trade_AbstractMaterialData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::Trade::AbstractMaterialData, enum @ref Magnum::Trade::MaterialType
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Material type

@see @ref AbstractMaterialData::type()
*/
enum class MaterialType: UnsignedByte {
    Phong       /**< Phong shading (see @ref PhongMaterialData) */
};

/**
@brief Material alpha mode

@see @ref AbstractMaterialData::alphaMode(),
    @ref AbstractMaterialData::alphaMask()
*/
enum class MaterialAlphaMode: UnsignedByte {
    /** Alpha value is ignored and the rendered output is fully opaque. */
    Opaque,

    /**
     * The rendered output is either fully transparent or fully opaque,
     * depending on the alpha value and specified
     * @ref AbstractMaterialData::alphaMask() value.
     */
    Mask,

    /**
     * The alpha value is used to combine source and destination colors using
     * additive blending.
     */
    Blend
};

/**
@brief Base for material data

Subclasses provide access to parameters for given material type.
*/
class MAGNUM_TRADE_EXPORT AbstractMaterialData {
    public:
        /**
         * @brief Material flag
         *
         * This enum is extended in subclasses.
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedShort {
            /**
             * The material is double-sided. Back faces should not be culled
             * away but rendered as well, with normals flipped for correct
             * lighting.
             */
            DoubleSided = 1 << 0
        };

        /**
         * @brief Material flags
         *
         * This enum is extended in subclasses.
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        virtual ~AbstractMaterialData();

        /** @brief Copying is not allowed */
        AbstractMaterialData(const AbstractMaterialData&) = delete;

        /** @brief Move constructor */
        AbstractMaterialData(AbstractMaterialData&&) noexcept;

        /** @brief Copying is not allowed */
        AbstractMaterialData& operator=(const AbstractMaterialData&) = delete;

        /** @brief Move assignment */
        AbstractMaterialData& operator=(AbstractMaterialData&&) noexcept;

        /** @brief Material type */
        MaterialType type() const { return _type; }

        /**
         * @brief Material flags
         *
         * Not all bits returned might be defiend by @ref Flag, subclasses
         * define extra values.
         */
        Flags flags() const { return _flags; }

        /** @brief Alpha mode */
        MaterialAlphaMode alphaMode() const { return _alphaMode; }

        /**
         * @brief Alpha mask
         *
         * If @ref alphaMode() is @ref MaterialAlphaMode::Mask, alpha values
         * below this value are rendered as fully transparent and alpha values
         * above this value as fully opaque. If @ref alphaMode() is not
         * @ref MaterialAlphaMode::Mask, this value is ignored.
         */
        Float alphaMask() const { return _alphaMask; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    protected:
        /**
         * @brief Constructor
         * @param type              Material type
         * @param flags             Untyped material flags
         * @param alphaMode         Alpha mode. Use
         *      @ref MaterialAlphaMode::Opaque for a default value.
         * @param alphaMask         Alpha mask value. Use @cpp 0.5f @ce for a
         *      default value.
         * @param importerState     Importer-specific state
         */
        explicit AbstractMaterialData(MaterialType type, Flags flags, MaterialAlphaMode alphaMode, Float alphaMask, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief Constructor
         * @deprecated Use @ref AbstractMaterialData(MaterialType, Flags, MaterialAlphaMode, Float, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use AbstractMaterialData(MaterialType, UnsignedInt, MaterialAlphaMode, Float) instead") AbstractMaterialData(MaterialType type, const void* importerState = nullptr) noexcept: AbstractMaterialData{type, {}, MaterialAlphaMode::Opaque, 0.5f, importerState} {}
        #endif

    private:
        MaterialType _type;
        MaterialAlphaMode _alphaMode;
        Flags _flags;
        Float _alphaMask;
        const void* _importerState;
};

/** @debugoperatorenum{MaterialType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialType value);

/** @debugoperatorenum{MaterialAlphaMode} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAlphaMode value);

/** @debugoperatorclassenum{AbstractMaterialData,AbstractMaterialData::Flag} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AbstractMaterialData::Flag value);

/** @debugoperatorclassenum{AbstractMaterialData,AbstractMaterialData::Flags} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AbstractMaterialData::Flags value);

}}

#endif
