#ifndef Magnum_Trade_AbstractImageConverter_h
#define Magnum_Trade_AbstractImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Trade::AbstractImageConverter, enum @ref Magnum::Trade::ImageConverterFeature, enum set @ref Magnum::Trade::ImageConverterFeatures
 */

#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Features supported by an image converter
@m_since{2020,06}

@see @ref ImageConverterFeatures, @ref AbstractImageConverter::features()
*/
enum class ImageConverterFeature: UnsignedInt {
    /**
     * Convert a 1D image with
     * @ref AbstractImageConverter::convert(const ImageView1D&)
     * @m_since_latest
     */
    Convert1D = 1 << 0,

    /**
     * Convert a 2D image with
     * @ref AbstractImageConverter::convert(const ImageView2D&)
     * @m_since_latest
     */
    Convert2D = 1 << 1,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc ImageConverterFeature::Convert2D
     * @m_deprecated_since_latest Use @ref ImageConverterFeature::Convert2D
     *      instead.
     */
    ConvertImage CORRADE_DEPRECATED_ENUM("use ImageConverterFeature::Convert2D instead") = Convert2D,

    /**
     * @copydoc ImageConverterFeature::Convert2D
     * @m_deprecated_since_latest Use @ref ImageConverterFeature::Convert2D
     *      instead. Since @ref AbstractImageConverter::convert() is now
     *      capable of returning both uncompressed and compressed images, this
     *      feature is the same as @ref ImageConverterFeature::Convert2D, as
     *      opposed to @ref ImageConverterFeature::ConvertCompressed2D, which
     *      is about *input* images.
     */
    ConvertCompressedImage CORRADE_DEPRECATED_ENUM("use ImageConverterFeature::Convert2D instead") = Convert2D,
    #endif

    /**
     * Convert a 3D image with
     * @ref AbstractImageConverter::convert(const ImageView3D&)
     * @m_since_latest
     */
    Convert3D = 1 << 3,

    /**
     * Convert a compressed 1D image with
     * @ref AbstractImageConverter::convert(const CompressedImageView1D&)
     * @m_since_latest
     */
    ConvertCompressed1D = 1 << 4,

    /**
     * Convert a compressed 2D image with
     * @ref AbstractImageConverter::convert(const CompressedImageView2D&)
     * @m_since_latest
     */
    ConvertCompressed2D = 1 << 5,

    /**
     * Convert a compressed 3D image with
     * @ref AbstractImageConverter::convert(const CompressedImageView3D&)
     * @m_since_latest
     */
    ConvertCompressed3D = 1 << 6,

    /**
     * Convert a 1D image to a file with
     * @ref AbstractImageConverter::convertToFile(const ImageView1D&, Containers::StringView)
     * @m_since_latest
     */
    Convert1DToFile = 1 << 7,

    /**
     * Convert a 2D image to a file with
     * @ref AbstractImageConverter::convertToFile(const ImageView2D&, Containers::StringView)
     * @m_since_latest
     */
    Convert2DToFile = 1 << 8,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc ImageConverterFeature::Convert2DToFile
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert2DToFile instead.
     */
    ConvertFile CORRADE_DEPRECATED_ENUM("use ImageConverterFeature::Convert2DToFile instead") = Convert2DToFile,
    #endif

    /**
     * Convert a 3D image to a file with
     * @ref AbstractImageConverter::convertToFile(const ImageView3D&, Containers::StringView)
     * @m_since_latest
     */
    Convert3DToFile = 1 << 9,

    /**
     * Convert a compressed 1D image to a file with
     * @ref AbstractImageConverter::convertToFile(const CompressedImageView1D&, Containers::StringView)
     * @m_since_latest
     */
    ConvertCompressed1DToFile = 1 << 10,

    /**
     * Convert a compressed 2D image to a file with
     * @ref AbstractImageConverter::convertToFile(const CompressedImageView2D&, Containers::StringView)
     * @m_since_latest
     */
    ConvertCompressed2DToFile = 1 << 11,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc ImageConverterFeature::ConvertCompressed2DToFile
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed2DToFile instead.
     */
    ConvertCompressedFile CORRADE_DEPRECATED_ENUM("use ImageConverterFeature::ConvertCompressed2DToFile instead") = ConvertCompressed2DToFile,
    #endif

    /**
     * Convert a compressed 3D image to a file with
     * @ref AbstractImageConverter::convertToFile(const CompressedImageView3D&, Containers::StringView)
     * @m_since_latest
     */
    ConvertCompressed3DToFile = 1 << 12,

    /**
     * Convert a 1D image to raw data with
     * @ref AbstractImageConverter::convertToData(const ImageView1D&).
     * Implies @ref ImageConverterFeature::Convert1DToFile.
     * @m_since_latest
     */
    Convert1DToData = Convert1DToFile|(1 << 13),

    /**
     * Convert a 2D image to raw data with
     * @ref AbstractImageConverter::convertToData(const ImageView2D&).
     * Implies @ref ImageConverterFeature::Convert2DToFile.
     * @m_since_latest
     */
    Convert2DToData = Convert2DToFile|(1 << 13),

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc ImageConverterFeature::Convert2DToData
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert2DToData instead.
     */
    ConvertData CORRADE_DEPRECATED_ENUM("use ImageConverterFeature::Convert2DToData instead") = Convert2DToData,
    #endif

    /**
     * Convert a 3D image to raw data with
     * @ref AbstractImageConverter::convertToData(const ImageView3D&).
     * Implies @ref ImageConverterFeature::Convert3DToFile.
     * @m_since_latest
     */
    Convert3DToData = Convert3DToFile|(1 << 13),

    /**
     * Convert a compressed 1D image to raw data with
     * @ref AbstractImageConverter::convertToData(const CompressedImageView1D&).
     * Implies @ref ImageConverterFeature::ConvertCompressed1DToFile.
     * @m_since_latest
     */
    ConvertCompressed1DToData = ConvertCompressed1DToFile|(1 << 13),

    /**
     * Convert a compressed 2D image to raw data with
     * @ref AbstractImageConverter::convertToData(const CompressedImageView2D&).
     * Implies @ref ImageConverterFeature::ConvertCompressed2DToFile.
     * @m_since_latest
     */
    ConvertCompressed2DToData = ConvertCompressed2DToFile|(1 << 13),

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc ImageConverterFeature::ConvertCompressed2DToData
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed2DToData instead.
     */
    ConvertCompressedData CORRADE_DEPRECATED_ENUM("use ImageConverterFeature::ConvertCompressed2DToData instead") = ConvertCompressed2DToData,
    #endif

    /**
     * Convert a compressed 3D image to raw data with
     * @ref AbstractImageConverter::convertToData(const CompressedImageView3D&).
     * Implies @ref ImageConverterFeature::ConvertCompressed3DToFile.
     * @m_since_latest
     */
    ConvertCompressed3DToData = ConvertCompressed3DToFile|(1 << 13)
};

/**
@brief Features supported by an image converter
@m_since{2020,06}

@see @ref AbstractImageConverter::features()
*/
typedef Containers::EnumSet<ImageConverterFeature> ImageConverterFeatures;

CORRADE_ENUMSET_OPERATORS(ImageConverterFeatures)

/** @debugoperatorenum{ImageConverterFeature} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImageConverterFeature value);

/** @debugoperatorenum{ImageConverterFeatures} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImageConverterFeatures value);

/**
@brief Image converter flag
@m_since{2020,06}

@see @ref ImageConverterFlags, @ref AbstractImageConverter::setFlags()
*/
enum class ImageConverterFlag: UnsignedByte {
    /**
     * Print verbose diagnostic during conversion. By default the converter
     * only prints messages on error or when some operation might cause
     * unexpected data modification or loss.
     *
     * Corresponds to the `-v` / `--verbose` option in
     * @ref magnum-imageconverter "magnum-imageconverter".
     */
    Verbose = 1 << 0

    /** @todo ~~Y flip~~ Y up */
};

/**
@brief Image converter flags
@m_since{2020,06}

@see @ref AbstractImageConverter::setFlags()
*/
typedef Containers::EnumSet<ImageConverterFlag> ImageConverterFlags;

CORRADE_ENUMSET_OPERATORS(ImageConverterFlags)

/**
@debugoperatorenum{ImageConverterFlag}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImageConverterFlag value);

/**
@debugoperatorenum{ImageConverterFlags}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImageConverterFlags value);

/**
@brief Base for image converter plugins

Provides functionality for converting images between various formats,
compressing them or saving to files.

The interface supports two main kinds of operation, with implementations
commonly advertising support for either one or the other via @ref features():

-   Saving a (compressed) 1D/2D/3D image to a file / data using
    @ref convertToFile() / @ref convertToData(). This is mostly for exporting
    the image data to a common format like JPEG or PNG in order to be used with
    an external tool. Advertised with
    @ref ImageConverterFeature::Convert1DToFile /
    @relativeref{ImageConverterFeature,Convert2DToFile} /
    @relativeref{ImageConverterFeature,Convert3DToFile} or
    @ref ImageConverterFeature::Convert1DToData /
    @relativeref{ImageConverterFeature,Convert2DToData} /
    @relativeref{ImageConverterFeature,Convert3DToData} and
    @ref ImageConverterFeature::ConvertCompressed1DToFile /
    @relativeref{ImageConverterFeature,ConvertCompressed2DToFile} /
    @relativeref{ImageConverterFeature,ConvertCompressed3DToFile} or
    @ref ImageConverterFeature::ConvertCompressed1DToData
    @relativeref{ImageConverterFeature,ConvertCompressed2DToData} /
    @relativeref{ImageConverterFeature,ConvertCompressed3DToData} for
    compressed input images.
-   Performing an operation on the image data itself using @ref convert(), from
    which you get an @ref ImageData back again. This includes operations like
    pixel format conversion or for example resampling. Advertised with
    @ref ImageConverterFeature::Convert1D /
    @relativeref{ImageConverterFeature,Convert2D} /
    @relativeref{ImageConverterFeature,Convert3D} and
    @ref ImageConverterFeature::ConvertCompressed1D /
    @relativeref{ImageConverterFeature,ConvertCompressed2D} /
    @relativeref{ImageConverterFeature,ConvertCompressed3D} for compressed
    input images.

@section Trade-AbstractImageConverter-usage Usage

Image converters are commonly implemented as plugins, which means the concrete
converter implementation is loaded and instantiated through a
@relativeref{Corrade,PluginManager::Manager}. Then, based on the intent and on
what the particular converter supports, @ref convertToFile(),
@ref convertToData() or @ref convert() gets called.

As each converter has different requirements and supports different pixel
formats, you're expected to perform error handling on the application side ---
if a conversion fails, you get an empty
@relativeref{Corrade,Containers::Optional} /
@relativeref{Corrade,Containers::Array} or @cpp false @ce and a reason printed
to the error output. Everything else (using a feature not implemented in the
converter, ...) is treated as a programmer error and will produce the usual
assertions.

@subsection Trade-AbstractImageConverter-usage-file Saving an image to a file

In the following example an 8-bit RGBA image is saved as a PNG using the
@ref AnyImageConverter plugin, together with all needed error handling. In this
case we *know* that @ref AnyImageConverter supports
@ref ImageConverterFeature::Convert2DToFile, however in a more general case and
especially when dealing with compressed image formats it might be good to check
against the reported @ref features() first.

@snippet MagnumTrade.cpp AbstractImageConverter-usage-file

See @ref plugins for more information about general plugin usage,
@ref file-formats to compare implementations of common file formats and the
list of @m_class{m-doc} [derived classes](#derived-classes) for all available
image converter plugins.

@m_class{m-note m-success}

@par
    There's also a @ref magnum-imageconverter "magnum-imageconverter" tool,
    exposing functionality of all image converter plugins on a command line as
    well as performing introspection of image files.

@subsection Trade-AbstractImageConverter-usage-image Converting image data

In the following snippet we use @ref StbDxtImageConverter to convert the same
8-bit RGBA image as above to a block-compressed one with
@ref CompressedPixelFormat::Bc3RGBAUnorm. While @ref AnyImageConverter can
detect the desired format when writing to a file and act accordingly, here it
would have no way to know what we want and so we request the concrete plugin
name directly. Here we again know that @ref StbDxtImageConverter gives us back
a compressed image and so we can put in just a sanity assert, but in the
general case it's converter-dependent and may even rely on configuration
options set for the plugin.

@snippet MagnumTrade.cpp AbstractImageConverter-usage-image

Commonly, when operating directly on the image data, each plugin exposes a set
of configuration options to specify what actually gets done and how, and the
default setup may not even do anything. See @ref plugins-configuration for
details and a usage example.

@section Trade-AbstractImageConverter-data-dependency Data dependency

The instances returned from various functions *by design* have no dependency on
the converter instance and neither on the dynamic plugin module. In other
words, you don't need to keep the converter instance (or the plugin manager
instance) around in order to have the `*Data` instances valid. Moreover, all
returned @relativeref{Corrade,Containers::Array} instances and
@relativeref{Corrade,Containers::Array} instances returned through
@ref ImageData are only allowed to have default deleters --- this is to avoid
potential dangling function pointer calls when destructing such instances after
the plugin module has been unloaded.

@section Trade-AbstractImageConverter-subclassing Subclassing

The plugin needs to implement the @ref doFeatures() function and one or more of
@ref doConvert(), @ref doConvertToData() or @ref doConvertToFile() functions
based on what features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The function @ref doConvert(const ImageView2D&) is called only if
    @ref ImageConverterFeature::Convert2D is supported and equivalently for the
    1D and 3D case.
-   The function @ref doConvert(const CompressedImageView2D&) is called only if
    @ref ImageConverterFeature::ConvertCompressed2D is supported and
    equivalently for the 1D and 3D case.
-   The function @ref doConvertToData(const ImageView2D&) is called only if
    @ref ImageConverterFeature::Convert2DToData is supported and equivalently
    for the 1D and 3D case.
-   The function @ref doConvertToData(const CompressedImageView2D&) is called
    only if @ref ImageConverterFeature::ConvertCompressed2DToData is supported
    and equivalently for the 1D and 3D case.
-   The function @ref doConvertToFile(const ImageView2D&, Containers::StringView)
    is called only if @ref ImageConverterFeature::Convert2DToFile is supported
    and equivalently for the 1D and 3D case.
-   The function @ref doConvertToFile(const CompressedImageView2D&, Containers::StringView)
    is called only if @ref ImageConverterFeature::ConvertCompressed2DToFile is
    supported and equivalently for the 1D and 3D case.

@m_class{m-block m-warning}

@par Dangling function pointers on plugin unload
    As @ref Trade-AbstractImageConverter-data-dependency "mentioned above",
    @relativeref{Corrade,Containers::Array} instances returned from plugin
    implementations are not allowed to use anything else than the default
    deleter, otherwise this could cause dangling function pointer call on array
    destruction if the plugin gets unloaded before the array is destroyed. This
    is asserted by the base implementation on return.
*/
class MAGNUM_TRADE_EXPORT AbstractImageConverter: public PluginManager::AbstractManagingPlugin<AbstractImageConverter> {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief ImageConverterFeature
         * @m_deprecated_since{2020,06} Use @ref ImageConverterFeature instead.
         */
        typedef CORRADE_DEPRECATED("use ImageConverterFeature instead") ImageConverterFeature Feature;

        /** @brief @copybrief ImageConverterFeatures
         * @m_deprecated_since{2020,06} Use @ref ImageConverterFeatures instead.
         */
        typedef CORRADE_DEPRECATED("use ImageConverterFeatures instead") ImageConverterFeatures Features;
        #endif

        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Trade/AbstractImageConverter.cpp interface
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/imageconverters/` or `magnum-d/imageconverters/`
         * next to the dynamic @ref Trade library, next to the executable and
         * elsewhere according to the rules documented in
         * @ref Corrade::PluginManager::implicitPluginSearchPaths(). The search
         * directory can be also hardcoded using the `MAGNUM_PLUGINS_DIR` CMake
         * variables, see @ref building for more information.
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
        ImageConverterFeatures features() const { return doFeatures(); }

        /**
         * @brief Converter flags
         * @m_since{2020,06}
         */
        ImageConverterFlags flags() const { return _flags; }

        /**
         * @brief Set converter flags
         * @m_since{2020,06}
         *
         * Some flags can be set only if the converter supports particular
         * features, see documentation of each @ref ImageConverterFlag for more
         * information. By default no flags are set. To avoid clearing
         * potential future default flags by accident, prefer to use
         * @ref addFlags() and @ref clearFlags() instead.
         *
         * Corresponds to the `-v` / `--verbose` option in
         * @ref magnum-imageconverter "magnum-imageconverter".
         */
        void setFlags(ImageConverterFlags flags);

        /**
         * @brief Add converter flags
         * @m_since_latest
         *
         * Calls @ref setFlags() with the existing flags ORed with @p flags.
         * Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        void addFlags(ImageConverterFlags flags);

        /**
         * @brief Clear converter flags
         * @m_since_latest
         *
         * Calls @ref setFlags() with the existing flags ANDed with inverse of
         * @p flags. Useful for removing default flags.
         * @see @ref addFlags()
         */
        void clearFlags(ImageConverterFlags flags);

        /**
         * @brief Convert a 1D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert1D is
         * supported. Returns converted image on success,
         * @ref Containers::NullOpt otherwise. The implementation is allowed to
         * return both a compressed an an uncompressed image, see documentation
         * of a particular converter for more information.
         * @see @ref features(), @ref convert(const CompressedImageView1D&),
         *      @ref convert(const ImageData1D&), @ref convertToData(),
         *      @ref convertToFile(), @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData1D> convert(const ImageView1D& image);

        /**
         * @brief Convert a 2D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2D is
         * supported. Returns converted image on success,
         * @ref Containers::NullOpt otherwise. The implementation is allowed to
         * return both a compressed an an uncompressed image, see documentation
         * of a particular converter for more information.
         * @see @ref features(), @ref convert(const CompressedImageView2D&),
         *      @ref convert(const ImageData2D&), @ref convertToData(),
         *      @ref convertToFile(), @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData2D> convert(const ImageView2D& image);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convert(const ImageView2D&)
         * @m_deprecated_since_latest Use @ref convert(const ImageView2D&)
         *      instead.
         */
        CORRADE_DEPRECATED("use convert(const ImageView2D&) instead") Containers::Optional<Image2D> exportToImage(const ImageView2D& image);

        /**
         * @brief Convert a 2D image to compressed format
         * @m_deprecated_since_latest Use @ref convert(const ImageView2D&)
         *      instead.
         */
        CORRADE_DEPRECATED("use convert(const ImageView2D&) instead") Containers::Optional<CompressedImage2D> exportToCompressedImage(const ImageView2D& image);
        #endif

        /**
         * @brief Convert a 3D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert3D is
         * supported. Returns converted image on success,
         * @ref Containers::NullOpt otherwise. The implementation is allowed to
         * return both a compressed an an uncompressed image, see documentation
         * of a particular converter for more information.
         * @see @ref features(), @ref convert(const CompressedImageView3D&),
         *      @ref convert(const ImageData3D&), @ref convertToData(),
         *      @ref convertToFile(), @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData3D> convert(const ImageView3D& image);

        /**
         * @brief Convert a compressed 1D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed1D is
         * supported. Returns converted image on success,
         * @ref Containers::NullOpt otherwise. The implementation is allowed to
         * return both a compressed an an uncompressed image, see documentation
         * of a particular converter for more information.
         * @see @ref features(), @ref convert(const ImageView1D&),
         *      @ref convert(const ImageData1D&), @ref convertToData(),
         *      @ref convertToFile(), @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData1D> convert(const CompressedImageView1D& image);

        /**
         * @brief Convert a compressed 2D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed2D is
         * supported. Returns converted image on success,
         * @ref Containers::NullOpt otherwise. The implementation is allowed to
         * return both a compressed an an uncompressed image, see documentation
         * of a particular converter for more information.
         * @see @ref features(), @ref convert(const ImageView2D&),
         *      @ref convert(const ImageData2D&), @ref convertToData(),
         *      @ref convertToFile(), @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData2D> convert(const CompressedImageView2D& image);

        /**
         * @brief Convert a compressed 3D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed3D is
         * supported. Returns converted image on success,
         * @ref Containers::NullOpt otherwise. The implementation is allowed to
         * return both a compressed an an uncompressed image, see documentation
         * of a particular converter for more information.
         * @see @ref features(), @ref convert(const ImageView3D&),
         *      @ref convert(const ImageData3D&), @ref convertToData(),
         *      @ref convertToFile(), @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData3D> convert(const CompressedImageView3D& image);

        /**
         * @brief Convert a 1D image data
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convert(const ImageView1D&) or
         * @ref convert(const CompressedImageView1D&). See documentation of
         * these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData1D> convert(const ImageData1D& image);

        /**
         * @brief Convert a 2D image data
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convert(const ImageView2D&) or
         * @ref convert(const CompressedImageView2D&). See documentation of
         * these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData2D> convert(const ImageData2D& image);

        /**
         * @brief Convert a 3D image data
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convert(const ImageView3D&) or
         * @ref convert(const CompressedImageView3D&). See documentation of
         * these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData3D> convert(const ImageData3D& image);

        /**
         * @brief Convert a 1D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert1DToData is
         * supported. Returns data on success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref convertToData(const CompressedImageView1D&),
         *      @ref convertToData(const ImageData1D&), @ref convert(),
         *      @ref convertToFile()
         */
        Containers::Array<char> convertToData(const ImageView1D& image);

        /**
         * @brief Convert a 2D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2DToData is
         * supported. Returns data on success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref convertToData(const CompressedImageView2D&),
         *      @ref convertToData(const ImageData2D&), @ref convert(),
         *      @ref convertToFile()
         */
        Containers::Array<char> convertToData(const ImageView2D& image);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToData(const ImageView2D&)
         * @m_deprecated_since_latest Use @ref convertToData(const ImageView2D&)
         *      instead.
         */
        CORRADE_DEPRECATED("use convertToData(const ImageView2D&) instead") Containers::Array<char> exportToData(const ImageView2D& image);
        #endif

        /**
         * @brief Convert a 3D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert3DToData is
         * supported. Returns data on success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref convertToData(const CompressedImageView3D&),
         *      @ref convertToData(const ImageData3D&), @ref convert(),
         *      @ref convertToFile()
         */
        Containers::Array<char> convertToData(const ImageView3D& image);

        /**
         * @brief Convert a compressed 1D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed1DToData
         * is supported. Returns data on success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref convertToData(const ImageView1D&),
         *      @ref convertToData(const ImageData1D&), @ref convert(),
         *      @ref convertToFile()
         */
        Containers::Array<char> convertToData(const CompressedImageView1D& image);

        /**
         * @brief Convert a compressed 2D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed2DToData
         * is supported. Returns data on success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref convertToData(const ImageView2D&),
         *      @ref convertToData(const ImageData2D&), @ref convert(),
         *      @ref convertToFile()
         */
        Containers::Array<char> convertToData(const CompressedImageView2D& image);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToData(const CompressedImageView2D&)
         * @m_deprecated_since_latest Use
         *      @ref convertToData(const CompressedImageView2D&) instead.
         */
        CORRADE_DEPRECATED("use convertToData(const CompressedImageView2D&) instead") Containers::Array<char> exportToData(const CompressedImageView2D& image);
        #endif

        /**
         * @brief Convert a compressed 3D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed3DToData
         * is supported. Returns data on success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref convertToData(const ImageView3D&),
         *      @ref convertToData(const ImageData3D&), @ref convert()
         *      @ref convertToFile()
         */
        Containers::Array<char> convertToData(const CompressedImageView3D& image);

        /**
         * @brief Convert a 1D image data to a raw data
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convertToData(const ImageView1D&) or
         * @ref convertToData(const CompressedImageView1D&). See documentation
         * of these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Array<char> convertToData(const ImageData1D& image);

        /**
         * @brief Convert a 2D image data to a raw data
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convertToData(const ImageView2D&) or
         * @ref convertToData(const CompressedImageView2D&). See documentation
         * of these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Array<char> convertToData(const ImageData2D& image);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToData(const ImageData2D&)
         * @m_deprecated_since_latest Use @ref convertToData(const ImageData2D&)
         *      instead.
         */
        CORRADE_DEPRECATED("use convertToData(const ImageView2D&) instead") Containers::Array<char> exportToData(const ImageData2D& image);
        #endif

        /**
         * @brief Convert a 3D image data to a raw data
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convertToData(const ImageView3D&) or
         * @ref convertToData(const CompressedImageView3D&). See documentation
         * of these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Array<char> convertToData(const ImageData3D& image);

        /**
         * @brief Convert a 1D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert1DToFile or
         * @ref ImageConverterFeature::Convert1DToData is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref convertToFile(const CompressedImageView1D&, Containers::StringView),
         *      @ref convertToFile(const ImageData1D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const ImageView1D& image, Containers::StringView filename);

        /**
         * @brief Convert a 2D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2DToFile or
         * @ref ImageConverterFeature::Convert2DToData is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref convertToFile(const CompressedImageView2D&, Containers::StringView),
         *      @ref convertToFile(const ImageData2D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const ImageView2D& image, Containers::StringView filename);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToFile(const ImageView2D&, Containers::StringView)
         * @m_deprecated_since_latest Use
         *      @ref convertToFile(const ImageView2D&, Containers::StringView)
         *      instead.
         */
        CORRADE_DEPRECATED("use convertToFile(const ImageView2D&, Containers::StringView) instead") bool exportToFile(const ImageView2D& image, const std::string& filename);
        #endif

        /**
         * @brief Convert a 3D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert3DToFile or
         * @ref ImageConverterFeature::Convert3DToData is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref convertToFile(const CompressedImageView3D&, Containers::StringView),
         *      @ref convertToFile(const ImageData3D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const ImageView3D& image, Containers::StringView filename);

        /**
         * @brief Convert a compressed 1D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed1DToFile
         * or @ref ImageConverterFeature::ConvertCompressed1DToData is
         * supported. Returns @cpp true @ce on success, @cpp false @ce
         * otherwise.
         * @see @ref features(), @ref convertToFile(const ImageView1D&, Containers::StringView),
         *      @ref convertToFile(const ImageData1D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const CompressedImageView1D& image, Containers::StringView filename);

        /**
         * @brief Convert a compressed 2D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed2DToFile
         * or @ref ImageConverterFeature::ConvertCompressed2DToData is
         * supported. Returns @cpp true @ce on success, @cpp false @ce
         * otherwise.
         * @see @ref features(), @ref convertToFile(const ImageView2D&, Containers::StringView),
         *      @ref convertToFile(const ImageData2D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const CompressedImageView2D& image, Containers::StringView filename);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToFile(const CompressedImageView2D&, Containers::StringView)
         * @m_deprecated_since_latest Use
         *      @ref convertToFile(const CompressedImageView2D&, Containers::StringView)
         *      instead.
         */
        CORRADE_DEPRECATED("use convertToFile(const CompressedImageView2D&, Containers::StringView) instead") bool exportToFile(const CompressedImageView2D& image, const std::string& filename);
        #endif

        /**
         * @brief Convert a compressed 3D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed3DToFile
         * or @ref ImageConverterFeature::ConvertCompressed3DToData is
         * supported. Returns @cpp true @ce on success, @cpp false @ce
         * otherwise.
         * @see @ref features(), @ref convertToFile(const ImageView3D&, Containers::StringView),
         *      @ref convertToFile(const ImageData3D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const CompressedImageView3D& image, Containers::StringView filename);

        /**
         * @brief Convert a 1D image data to a file
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convertToFile(const ImageView1D&, Containers::StringView) or
         * @ref convertToFile(const CompressedImageView1D&, Containers::StringView).
         * See documentation of these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        bool convertToFile(const ImageData1D& image, Containers::StringView filename);

        /**
         * @brief Convert a 2D image data to a file
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convertToFile(const ImageView2D&, Containers::StringView) or
         * @ref convertToFile(const CompressedImageView2D&, Containers::StringView).
         * See documentation of these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        bool convertToFile(const ImageData2D& image, Containers::StringView filename);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToFile(const ImageData2D&, Containers::StringView)
         * @m_deprecated_since_latest Use
         *      @ref convertToFile(const ImageData2D&, Containers::StringView)
         *      instead.
         */
        CORRADE_DEPRECATED("use convertToFile(const ImageData2D&, Containers::StringView) instead") bool exportToFile(const ImageData2D& image, const std::string& filename);
        #endif

        /**
         * @brief Convert a 3D image data to a file
         * @m_since_latest
         *
         * Based on whether the image is compressed or not, calls either
         * @ref convertToFile(const ImageView3D&, Containers::StringView) or
         * @ref convertToFile(const CompressedImageView3D&, Containers::StringView).
         * See documentation of these two functions for details.
         * @see @ref ImageData::isCompressed()
         */
        bool convertToFile(const ImageData3D& image, Containers::StringView filename);

    protected:
        /**
         * @brief Implementation for @ref convertToFile(const ImageView1D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Convert1DToData is supported, default
         * implementation calls @ref doConvertToData(const ImageView1D&) and
         * saves the result to given file. It is allowed to call this function
         * from your @ref doConvertToFile() implementation, for example when
         * you only need to do format detection based on file extension.
         */
        virtual bool doConvertToFile(const ImageView1D& image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(const ImageView2D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Convert2DToData is supported, default
         * implementation calls @ref doConvertToData(const ImageView2D&) and
         * saves the result to given file. It is allowed to call this function
         * from your @ref doConvertToFile() implementation, for example when
         * you only need to do format detection based on file extension.
         */
        virtual bool doConvertToFile(const ImageView2D& image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(const ImageView3D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Convert3DToData is supported, default
         * implementation calls @ref doConvertToData(const ImageView3D&) and
         * saves the result to given file. It is allowed to call this function
         * from your @ref doConvertToFile() implementation, for example when
         * you only need to do format detection based on file extension.
         */
        virtual bool doConvertToFile(const ImageView3D& image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(const CompressedImageView1D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::ConvertCompressed1DToData is
         * supported, default implementation calls @ref doConvertToData(const CompressedImageView1D&)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(const CompressedImageView1D& image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(const CompressedImageView2D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::ConvertCompressed2DToData is
         * supported, default implementation calls @ref doConvertToData(const CompressedImageView2D&)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(const CompressedImageView2D& image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(const CompressedImageView3D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::ConvertCompressed3DToData is
         * supported, default implementation calls @ref doConvertToData(const CompressedImageView3D&)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(const CompressedImageView3D& image, Containers::StringView filename);

    private:
        /** @brief Implementation for @ref features() */
        virtual ImageConverterFeatures doFeatures() const = 0;

        /**
         * @brief Implementation for @ref setFlags()
         * @m_since{2020,06}
         *
         * Useful when the converter needs to modify some internal state on
         * flag setup. Default implementation does nothing and this
         * function doesn't need to be implemented --- the flags are available
         * through @ref flags().
         *
         * To reduce the amount of error checking on user side, this function
         * isn't expected to fail --- if a flag combination is invalid /
         * unsuported, error reporting should be delayed to various conversion
         * functions, where the user is expected to do error handling anyway.
         */
        virtual void doSetFlags(ImageConverterFlags flags);

        /**
         * @brief Implementation for @ref convert(const ImageView1D&)
         * @m_since_latest
         */
        virtual Containers::Optional<ImageData1D> doConvert(const ImageView1D& image);

        /**
         * @brief Implementation for @ref convert(const ImageView2D&)
         * @m_since_latest
         */
        virtual Containers::Optional<ImageData2D> doConvert(const ImageView2D& image);

        /**
         * @brief Implementation for @ref convert(const ImageView3D&)
         * @m_since_latest
         */
        virtual Containers::Optional<ImageData3D> doConvert(const ImageView3D& image);

        /**
         * @brief Implementation for @ref convert(const CompressedImageView1D&)
         * @m_since_latest
         */
        virtual Containers::Optional<ImageData1D> doConvert(const CompressedImageView1D& image);

        /**
         * @brief Implementation for @ref convert(const CompressedImageView2D&)
         * @m_since_latest
         */
        virtual Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D& image);

        /**
         * @brief Implementation for @ref convert(const CompressedImageView3D&)
         * @m_since_latest
         */
        virtual Containers::Optional<ImageData3D> doConvert(const CompressedImageView3D& image);

        /**
         * @brief Implementation for @ref convertToData(const ImageView1D&)
         * @m_since_latest
         */
        virtual Containers::Array<char> doConvertToData(const ImageView1D& image);

        /**
         * @brief Implementation for @ref convertToData(const ImageView2D&)
         * @m_since_latest
         */
        virtual Containers::Array<char> doConvertToData(const ImageView2D& image);

        /**
         * @brief Implementation for @ref convertToData(const ImageView3D&)
         * @m_since_latest
         */
        virtual Containers::Array<char> doConvertToData(const ImageView3D& image);

        /**
         * @brief Implementation for @ref convertToData(const CompressedImageView1D&)
         * @m_since_latest
         */
        virtual Containers::Array<char> doConvertToData(const CompressedImageView1D& image);

        /**
         * @brief Implementation for @ref convertToData(const CompressedImageView2D&)
         * @m_since_latest
         */
        virtual Containers::Array<char> doConvertToData(const CompressedImageView2D& image);

        /**
         * @brief Implementation for @ref convertToData(const CompressedImageView3D&)
         * @m_since_latest
         */
        virtual Containers::Array<char> doConvertToData(const CompressedImageView3D& image);

        ImageConverterFlags _flags;
};

}}

#endif
