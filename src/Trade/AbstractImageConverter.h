#ifndef Magnum_Trade_AbstractImageConverter_h
#define Magnum_Trade_AbstractImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Trade::AbstractImageConverter
 */

#include <PluginManager/AbstractPlugin.h>

#include "Magnum.h"
#include "Text/Text.h"
#include "magnumVisibility.h"

namespace Magnum { namespace Trade {

/**
@brief Base for image converter plugins

Provides functionality for converting images between various internal formats
or compressing them.

@section AbstractImageConverter-subclassing Subclassing

Plugin implements function features() and one or more of convertToImage(),
convertToData() or convertToFile() functions based on what features are
supported.
*/
class MAGNUM_EXPORT AbstractImageConverter: public PluginManager::AbstractPlugin {
    CORRADE_PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImageConverter/0.1")

    public:
        /**
         * @brief Features supported by this converter
         *
         * @see Features, features()
         */
        enum class Feature: UnsignedByte {
            /** Converting to image with different format with convertToImage() */
            ConvertToImage = 1 << 0,

            /** Converting to data with convertToData() */
            ConvertToData = 1 << 1,

            /** Converting to file with convertToFile() */
            ConvertToFile = 1 << 2
        };

        /**
         * @brief Features supported by this converter
         *
         * @see features()
         */
        typedef Containers::EnumSet<Feature, UnsignedByte> Features;

        /** @brief Default constructor */
        explicit AbstractImageConverter();

        /** @brief Plugin manager constructor */
        explicit AbstractImageConverter(PluginManager::AbstractManager* manager, std::string plugin);

        /** @brief Features supported by this converter */
        virtual Features features() const = 0;

        /**
         * @brief Convert image to different format
         *
         * Available only if @ref Feature "Feature::ConvertToImage" is supported.
         * Returns converted image on success, `nullptr` otherwise.
         * @see features(), convertToData(), convertToFile()
         */
        virtual Image2D* convertToImage(const Image2D* image) const;

        /**
         * @brief Convert image to raw data
         *
         * Available only if @ref Feature "Feature::ConvertToData" is supported.
         * Returns data pointer and size on success, `nullptr` otherwise.
         * @see features(), convertToImage(), convertToFile()
         */
        virtual std::pair<const unsigned char*, std::size_t> convertToData(const Image2D* image) const;

        /**
         * @brief Convert image and save it to file
         *
         * Available only if @ref Feature "Feature::ConvertToFile" is supported.
         * Returns `true` on success, `false` otherwise.
         * @see features(), convertToImage(), convertToData()
         */
        virtual bool convertToFile(const Image2D* image, const std::string& filename) const;
};

CORRADE_ENUMSET_OPERATORS(AbstractImageConverter::Features)

}}

#endif
