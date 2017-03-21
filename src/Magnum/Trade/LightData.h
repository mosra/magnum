#ifndef Magnum_Trade_LightData_h
#define Magnum_Trade_LightData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Trade::LightData
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace Trade {

/**
@brief Light data
*/
class LightData {
    public:
        /**
         * @brief Light type
         *
         * @see @ref type()
         */
        enum class Type: UnsignedByte {
            /**
             * Light at position that is infinitely far away so its rays are
             * parallel. The light rays point in a direction of negative Z
             * axis.
             */
            Infinite,

            /** Point light, radiating in all directions */
            Point,

            /**
             * Spot light, radiating in a limited range of direction. The
             * primary direction is negative Z axis.
             */
            Spot
        };

        /**
         * @brief Constructor
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param importerState     Importer-specific state
         */
        constexpr explicit LightData(Type type, const Color3& color, Float intensity, const void* importerState = nullptr) noexcept: _type{type}, _color{color}, _intensity{intensity}, _importerState{importerState} {}

        /** @brief Copying is not allowed */
        LightData(const LightData&) = delete;

        /** @brief Move constructor */
        LightData(LightData&&) noexcept = default;

        /** @brief Copying is not allowed */
        LightData& operator=(const LightData&) = delete;

        /** @brief Move assignment */
        LightData& operator=(LightData&&) noexcept = default;

        /** @brief Light type */
        constexpr Type type() const { return _type; }

        /** @brief Light color */
        constexpr Color3 color() const { return _color; }

        /** @brief Light intensity */
        constexpr Float intensity() const { return _intensity; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        Type _type;
        Vector3 _color;
        Float _intensity;
        const void* _importerState;
};

/** @debugoperatorenum{Magnum::Trade::LightData::Type} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, LightData::Type value);

}}

#endif
