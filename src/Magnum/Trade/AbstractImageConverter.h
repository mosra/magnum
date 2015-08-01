#ifndef Magnum_Trade_AbstractImageConverter_h
#define Magnum_Trade_AbstractImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
 * @brief Class @ref Magnum::Trade::AbstractImageConverter
 */

#include <Corrade/PluginManager/AbstractPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"
#include "MagnumExternal/Optional/optional.hpp"

namespace Magnum { namespace Trade {

/**
@brief Base for image converter plugins

Provides functionality for converting images between various internal formats
or compressing them. See @ref plugins for more information and `*ImageConverter`
classes in @ref Trade namespace for available image converter plugins.

## Subclassing

Plugin implements function @ref doFeatures() and one or more of
@ref doExportToImage(), @ref doExportToCompressedImage(), @ref doExportToData()
or @ref doExportToFile() functions based on what features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Function @ref doExportToImage() is called only if @ref Feature::ConvertImage
    is supported.
-   Function @ref doExportToData() are called only if @ref Feature::ConvertData
    is supported.
-   Function @ref doExportToCompressedImage() is called only if
    @ref Feature::CompressImage is supported.

Plugin interface string is `"cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1"`.
*/
class MAGNUM_EXPORT AbstractImageConverter: public PluginManager::AbstractPlugin {
    CORRADE_PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1")

    public:
        /**
         * @brief Features supported by this converter
         *
         * @see @ref Features, @ref features()
         */
        enum class Feature: UnsignedByte {
            /** Conversion to image with different format with @ref exportToImage() */
            ConvertImage = 1 << 0,

            /** Conversion to compressed image with @ref exportToCompressedImage() */
            CompressImage = 1 << 1,

            /** Exporting to raw data with @ref exportToData() */
            ConvertData = 1 << 2
        };

        /**
         * @brief Features supported by this converter
         *
         * @see @ref features()
         */
        typedef Containers::EnumSet<Feature> Features;

        /** @brief Default constructor */
        explicit AbstractImageConverter();

        /** @brief Plugin manager constructor */
        explicit AbstractImageConverter(PluginManager::AbstractManager& manager, std::string plugin);

        /** @brief Features supported by this converter */
        Features features() const { return doFeatures(); }

        /**
         * @brief Convert image to different format
         *
         * Available only if @ref Feature::ConvertImage is supported. Returns
         * converted image on success, `std::nullopt` otherwise.
         * @see @ref features(), @ref exportToData(), @ref exportToFile()
         */
        std::optional<Image2D> exportToImage(const ImageView2D& image) const;

        /**
         * @brief Convert image to compressed format
         *
         * Available only if @ref Feature::CompressImage is supported. Returns
         * converted image on success, `std::nullopt` otherwise.
         * @see @ref features(), @ref exportToData(), @ref exportToFile()
         */
        std::optional<CompressedImage2D> exportToCompressedImage(const ImageView2D& image) const;

        /**
         * @brief Export image to raw data
         *
         * Available only if @ref Feature::ConvertData is supported. Returns
         * data on success, zero-sized array otherwise.
         * @see @ref features(), @ref exportToImage(), @ref exportToFile()
         */
        Containers::Array<char> exportToData(const ImageView2D& image) const;

        /**
         * @brief Export image to file
         *
         * Returns `true` on success, `false` otherwise.
         * @see @ref features(), @ref exportToImage(), @ref exportToData()
         */
        bool exportToFile(const ImageView2D& image, const std::string& filename) const;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        /** @brief Implementation of @ref features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation of @ref exportToImage() */
        virtual std::optional<Image2D> doExportToImage(const ImageView2D& image) const;

        /** @brief Implementation of @ref exportToCompressedImage() */
        virtual std::optional<CompressedImage2D> doExportToCompressedImage(const ImageView2D& image) const;

        /** @brief Implementation of @ref exportToData() */
        virtual Containers::Array<char> doExportToData(const ImageView2D& image) const;

        /**
         * @brief Implementation of @ref exportToFile()
         *
         * If @ref Feature::ConvertData is supported, default implementation
         * calls @ref doExportToData() and saves the result to given file.
         */
        virtual bool doExportToFile(const ImageView2D& image, const std::string& filename) const;
};

CORRADE_ENUMSET_OPERATORS(AbstractImageConverter::Features)

}}

#endif
