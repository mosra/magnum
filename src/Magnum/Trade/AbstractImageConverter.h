#ifndef Magnum_Trade_AbstractImageConverter_h
#define Magnum_Trade_AbstractImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Base for image converter plugins

Provides functionality for converting images between various internal formats
or compressing them. See @ref plugins for more information and `*ImageConverter`
classes in @ref Trade namespace for available image converter plugins.

@section Trade-AbstractImageConverter-subclassing Subclassing

The plugin needs to implement the@ref doFeatures() function and one or more of
@ref doExportToImage(), @ref doExportToCompressedImage(), @ref doExportToData()
or @ref doExportToFile() functions based on what features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The function @ref doExportToImage() is called only if
    @ref Feature::ConvertImage
    is supported.
-   The function @ref doExportToCompressedImage() is called only if
    @ref Feature::ConvertCompressedImage is supported.
-   The function @ref doExportToData(const ImageView2D&) is called only if
    @ref Feature::ConvertData is supported.
-   The function @ref doExportToData(const CompressedImageView2D&) is called
    only if @ref Feature::ConvertCompressedData is supported.

@attention @ref Corrade::Containers::Array instances returned from the plugin
    should *not* use anything else than the default deleter, otherwise this can
    cause dangling function pointer call on array destruction if the plugin
    gets unloaded before the array is destroyed.
*/
class MAGNUM_TRADE_EXPORT AbstractImageConverter: public PluginManager::AbstractManagingPlugin<AbstractImageConverter> {
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
            ConvertCompressedImage = 1 << 1,

            /** Exporting to file with @ref exportToFile(const ImageView2D&, const std::string&) */
            ConvertFile = 1 << 2,

            /** Exporting to file with @ref exportToFile(const CompressedImageView2D&, const std::string&) */
            ConvertCompressedFile = 1 << 3,

            /**
             * Exporting to raw data with @ref exportToData(const ImageView2D&).
             * Implies @ref Feature::ConvertFile.
             */
            ConvertData = ConvertFile|(1 << 4),

            /**
             * Exporting compressed image to raw data with
             * @ref exportToData(const CompressedImageView2D&). Implies
             * @ref Feature::ConvertCompressedFile.
             */
            ConvertCompressedData = ConvertCompressedFile|(1 << 4)
        };

        /**
         * @brief Features supported by this converter
         *
         * @see @ref features()
         */
        typedef Containers::EnumSet<Feature> Features;

        /**
         * @brief Plugin interface
         *
         * @code{.cpp}
         * "cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1"
         * @endcode
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * First looks in `magnum/imageconverters/` or `magnum-d/imageconverters/`
         * next to the executable and as a fallback in `magnum/imageconverters/`
         * or `magnum-d/imageconverters/` in the runtime install location
         * (`lib[64]/` on Unix-like systems, `bin/` on Windows). The
         * system-wide plugin search directory is configurable using the
         * `MAGNUM_PLUGINS_DIR` CMake variables, see @ref building for more
         * information.
         *
         * Not defined on platforms without
         * @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support".
         */
        static std::vector<std::string> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractImageConverter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractImageConverter(PluginManager::Manager<AbstractImageConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AbstractImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this converter */
        Features features() const { return doFeatures(); }

        /**
         * @brief Convert image to different format
         *
         * Available only if @ref Feature::ConvertImage is supported. Returns
         * converted image on success, @ref Containers::NullOpt otherwise.
         * @see @ref features(), @ref exportToData(), @ref exportToFile()
         */
        Containers::Optional<Image2D> exportToImage(const ImageView2D& image);

        /**
         * @brief Convert image to compressed format
         *
         * Available only if @ref Feature::ConvertCompressedImage is supported.
         * Returns converted image on success, @ref Containers::NullOpt otherwise.
         * @see @ref features(), @ref exportToData(), @ref exportToFile()
         */
        Containers::Optional<CompressedImage2D> exportToCompressedImage(const ImageView2D& image);

        /**
         * @brief Export image to raw data
         *
         * Available only if @ref Feature::ConvertData is supported. Returns
         * data on success, zero-sized array otherwise.
         * @see @ref features(), @ref exportToImage(),
         *      @ref exportToFile(const ImageView2D&, const std::string&)
         */
        Containers::Array<char> exportToData(const ImageView2D& image);

        /**
         * @brief Export compressed image to raw data
         *
         * Available only if @ref Feature::ConvertCompressedData is supported.
         * Returns data on success, zero-sized array otherwise.
         * @see @ref features(), @ref exportToCompressedImage(),
         *      @ref exportToFile(const CompressedImageView2D&, const std::string&)
         */
        Containers::Array<char> exportToData(const CompressedImageView2D& image);

        /**
         * @brief Export image to raw data
         *
         * Based on whether the image is compressed or not, calls either
         * @ref exportToData(const ImageView2D&) or
         * @ref exportToData(const CompressedImageView2D&). See documentation
         * of those two functions for details.
         */
        Containers::Array<char> exportToData(const ImageData2D& image);

        /**
         * @brief Export image to file
         *
         * Available only if @ref Feature::ConvertFile or
         * @ref Feature::ConvertData is supported. Returns `true` on success,
         * `false` otherwise.
         * @see @ref features(), @ref exportToImage(),
         *      @ref exportToData(const ImageView2D&)
         */
        bool exportToFile(const ImageView2D& image, const std::string& filename);

        /**
         * @brief Export compressed image to file
         *
         * Available only if @ref Feature::ConvertCompressedFile or
         * @ref Feature::ConvertCompressedData is supported. Returns `true` on
         * success, `false` otherwise.
         * @see @ref features(), @ref exportToCompressedImage(),
         *      @ref exportToData(const CompressedImageView2D&)
         */
        bool exportToFile(const CompressedImageView2D& image, const std::string& filename);

        /**
         * @brief Export image to file
         *
         * Based on whether the image is compressed or not, calls either
         * @ref exportToFile(const ImageView2D&, const std::string&) or
         * @ref exportToFile(const CompressedImageView2D&, const std::string&).
         * See documentation of those two functions for details.
         */
        bool exportToFile(const ImageData2D& image, const std::string& filename);

    private:
        /** @brief Implementation of @ref features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation of @ref exportToImage() */
        virtual Containers::Optional<Image2D> doExportToImage(const ImageView2D& image);

        /** @brief Implementation of @ref exportToCompressedImage() */
        virtual Containers::Optional<CompressedImage2D> doExportToCompressedImage(const ImageView2D& image);

        /** @brief Implementation of @ref exportToData(const ImageView2D&) */
        virtual Containers::Array<char> doExportToData(const ImageView2D& image);

        /** @brief Implementation of @ref exportToData(const CompressedImageView2D&) */
        virtual Containers::Array<char> doExportToData(const CompressedImageView2D& image);

        /**
         * @brief Implementation of @ref exportToFile(const ImageView2D&, const std::string&)
         *
         * If @ref Feature::ConvertData is supported, default implementation
         * calls @ref doExportToData(const ImageView2D&) and saves the result
         * to given file.
         */
        virtual bool doExportToFile(const ImageView2D& image, const std::string& filename);

        /**
         * @brief Implementation of @ref exportToFile(const CompressedImageView2D&, const std::string&)
         *
         * If @ref Feature::ConvertCompressedData is supported, default
         * implementation calls @ref doExportToData(const CompressedImageView2D&)
         * and saves the result to given file.
         */
        virtual bool doExportToFile(const CompressedImageView2D& image, const std::string& filename);
};

CORRADE_ENUMSET_OPERATORS(AbstractImageConverter::Features)

/** @debugoperatorclassenum{AbstractImageConverter,AbstractImageConverter::Feature} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AbstractImageConverter::Feature value);

/** @debugoperatorclassenum{AbstractImageConverter,AbstractImageConverter::Features} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AbstractImageConverter::Features value);

}}

#endif
