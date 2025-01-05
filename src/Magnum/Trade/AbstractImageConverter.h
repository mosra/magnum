#ifndef Magnum_Trade_AbstractImageConverter_h
#define Magnum_Trade_AbstractImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Trade::AbstractImageConverter, enum @ref Magnum::Trade::ImageConverterFeature, enum set @ref Magnum::Trade::ImageConverterFeatures
 */

#include <initializer_list>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* For *ToData() APIs that used to return just an Array before */
#include <Corrade/Containers/Optional.h>

/* So deprecated APIs taking a std::string don't fail to compile */
/** @todo remove once they are gone */
#include <Corrade/Utility/StlForwardString.h>
#endif

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
    ConvertCompressed3DToData = ConvertCompressed3DToFile|(1 << 13),

    /**
     * Convert multiple image levels with
     * @ref AbstractImageConverter::convertToFile(Containers::ArrayView<const ImageView1D>, Containers::StringView) /
     * @ref AbstractImageConverter::convertToFile(Containers::ArrayView<const CompressedImageView1D>, Containers::StringView)
     * if @ref ImageConverterFeature::Convert1DToFile /
     * @relativeref{ImageConverterFeature,ConvertCompressed1DToFile} is also
     * supported; with
     * @ref AbstractImageConverter::convertToFile(Containers::ArrayView<const ImageView2D>, Containers::StringView) /
     * @ref AbstractImageConverter::convertToFile(Containers::ArrayView<const CompressedImageView2D>, Containers::StringView)
     * if @ref ImageConverterFeature::Convert2DToFile /
     * @relativeref{ImageConverterFeature,ConvertCompressed2DToFile} is also
     * supported; with
     * @ref AbstractImageConverter::convertToFile(Containers::ArrayView<const ImageView3D>, Containers::StringView) /
     * @ref AbstractImageConverter::convertToFile(Containers::ArrayView<const CompressedImageView3D>, Containers::StringView)
     * if @ref ImageConverterFeature::Convert3DToFile /
     * @relativeref{ImageConverterFeature,ConvertCompressed3DToFile} is also
     * supported; with
     * @ref AbstractImageConverter::convertToData(Containers::ArrayView<const ImageView1D>) /
     * @ref AbstractImageConverter::convertToData(Containers::ArrayView<const CompressedImageView1D>)
     * if @ref ImageConverterFeature::Convert1DToData /
     * @relativeref{ImageConverterFeature,ConvertCompressed1DToData} is also
     * supported; with
     * @ref AbstractImageConverter::convertToData(Containers::ArrayView<const ImageView2D>) /
     * @ref AbstractImageConverter::convertToData(Containers::ArrayView<const CompressedImageView2D>)
     * if @ref ImageConverterFeature::Convert2DToData /
     * @relativeref{ImageConverterFeature,ConvertCompressed2DToData} is also
     * supported; with
     * @ref AbstractImageConverter::convertToData(Containers::ArrayView<const ImageView3D>) /
     * @ref AbstractImageConverter::convertToData(Containers::ArrayView<const CompressedImageView3D>)
     * if @ref ImageConverterFeature::Convert3DToData /
     * @relativeref{ImageConverterFeature,ConvertCompressed3DToData} is also
     * supported.
     * @m_since_latest
     */
    Levels = 1 << 14,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert1DToFile together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertLevels1DToFile CORRADE_DEPRECATED_ENUM("use Convert1DToFile together with Levels instead") = Convert1DToFile|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert2DToFile together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertLevels2DToFile CORRADE_DEPRECATED_ENUM("use Convert2DToFile together with Levels instead") = Convert2DToFile|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert3DToFile together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertLevels3DToFile CORRADE_DEPRECATED_ENUM("use Convert3DToFile together with Levels instead") = Convert3DToFile|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed1DToFile together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertCompressedLevels1DToFile CORRADE_DEPRECATED_ENUM("use ConvertCompressed1DToFile together with Levels instead") = ConvertCompressed1DToFile|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed2DToFile together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertCompressedLevels2DToFile CORRADE_DEPRECATED_ENUM("use ConvertCompressed2DToFile together with Levels instead") = ConvertCompressed2DToFile|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed3DToFile together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertCompressedLevels3DToFile CORRADE_DEPRECATED_ENUM("use ConvertCompressed3DToFile together with Levels instead") = ConvertCompressed3DToFile|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert1DToData together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertLevels1DToData CORRADE_DEPRECATED_ENUM("use Convert1DToData together with Levels instead") = Convert1DToData|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert2DToData together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertLevels2DToData CORRADE_DEPRECATED_ENUM("use Convert2DToData together with Levels instead") = Convert2DToData|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::Convert3DToData together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertLevels3DToData CORRADE_DEPRECATED_ENUM("use Convert3DToData together with Levels instead") = Convert3DToData|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed1DToData together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertCompressedLevels1DToData CORRADE_DEPRECATED_ENUM("use ConvertCompressed1DToData together with Levels instead") = ConvertCompressed1DToData|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed2DToData together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertCompressedLevels2DToData CORRADE_DEPRECATED_ENUM("use ConvertCompressed2DToData together with Levels instead") = ConvertCompressed2DToData|Levels,

    /**
     * @m_deprecated_since_latest Use
     *      @ref ImageConverterFeature::ConvertCompressed3DToData together with
     *      @relativeref{ImageConverterFeature,Levels} instead.
     */
    ConvertCompressedLevels3DToData CORRADE_DEPRECATED_ENUM("use ConvertCompressed3DToData together with Levels instead") = ConvertCompressed3DToData|Levels
    #endif
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
     * Suppress warnings, print just errors. By default the converter prints
     * both warnings and errors.
     * @m_since_latest
     */
    Quiet = 1 << 1,

    /**
     * Print verbose diagnostic during conversion. By default the converter
     * only prints messages on error or when some operation might cause
     * unexpected data modification or loss.
     *
     * Corresponds to the `-v` / `--verbose` option in
     * @ref magnum-imageconverter "magnum-imageconverter" and
     * @ref magnum-sceneconverter "magnum-sceneconverter".
     */
    Verbose = 1 << 0

    /** @todo is warning as error (like in ShaderConverter) usable for anything
        here? in case of a compiler it makes sense, in case of an importer not
        so much probably? it'd also mean expanding each and every Warning
        print (using Error, adding a return) which is a lot to maintain */

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

#ifdef MAGNUM_BUILD_DEPRECATED
namespace Implementation {
    /* Could be a concrete type as it's always only char, but that would mean
       I'd need to include Optional and Array here. It's named like this
       because AbstractSceneConverter and ShaderTools::AbstractConverter each
       have its own and introducing a common header containing just deprecated
       functionality seems silly. */
    template<class T> struct ImageConverterOptionalButAlsoArray: Containers::Optional<Containers::Array<T>> {
        /*implicit*/ ImageConverterOptionalButAlsoArray() = default;
        /*implicit*/ ImageConverterOptionalButAlsoArray(Containers::Optional<Containers::Array<T>>&& optional): Containers::Optional<Containers::Array<T>>{Utility::move(optional)} {}
        CORRADE_DEPRECATED("use Containers::Optional<Containers::Array<T>> instead") /*implicit*/ operator Containers::Array<T>() && {
            return *this ? Containers::Array<T>{Utility::move(**this)} : nullptr;
        }
    };
}
#endif

/**
@brief Base for image converter plugins

Provides functionality for converting images between various formats,
compressing them or saving to files.

The interface supports three main kinds of operation, with implementations
commonly advertising support for a subset of them via @ref features():

-   Saving a single (compressed) 1D/2D/3D image to a file / data using
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
-   Saving a set of (compressed) 1D/2D/3D image levels to a file / data using
    @ref convertToFile() / @ref convertToData(). Common use case is to save
    already pregenerated levels instead of having to create them during load.
    Advertised with @ref ImageConverterFeature::Levels in addition to the
    single-image feature. Note that if a plugin advertises multi-level support,
    it's also capable of saving single images --- in that case the single-image
    @ref convertToFile() / @ref convertToData() delegates to the multi-level
    variant with just a single image.
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
@relativeref{Corrade,Containers::Optional} or @cpp false @ce and a reason
printed to @relativeref{Magnum,Error}. Everything else (using a feature not
implemented in the converter, ...) is treated as a programmer error and will
produce the usual assertions.

@subsection Trade-AbstractImageConverter-usage-file Saving an image to a file

In the following example an 8-bit RGBA image is saved as a PNG using the
@ref AnyImageConverter plugin, together with all needed error handling. In this
case we *know* that @ref AnyImageConverter supports
@ref ImageConverterFeature::Convert2DToFile, however in a more general case and
especially when dealing with compressed image formats it might be good to check
against the reported @ref features() first.

@snippet Trade.cpp AbstractImageConverter-usage-file

See @ref plugins for more information about general plugin usage,
@ref file-formats to compare implementations of common file formats and the
list of @m_class{m-doc} <a href="#derived-classes">derived classes</a> for all
available image converter plugins.

@m_class{m-note m-success}

@par
    There's also the @ref magnum-imageconverter "magnum-imageconverter" tool
    that exposes functionality of all image converter plugins through a command
    line interface.

@subsection Trade-AbstractImageConverter-usage-file-levels Saving a set of mip levels

Certain file formats, such as OpenEXR, DDS or ICO, are capable of storing
multiple image mip levels in a single file. Each format has a slightly
different set of rules for image sizes and their order, in general putting the
largest level first and then gradually halving the size should always work. In
the following snippet we'll save three mip levels to an EXR file, again using
the @ref AnyImageConverter plugin, which will then most likely delegate to
@link OpenExrImageConverter @endlink:

@snippet Trade.cpp AbstractImageConverter-usage-file-levels

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

@snippet Trade.cpp AbstractImageConverter-usage-image

Commonly, when operating directly on the image data, each plugin exposes a set
of configuration options to specify what actually gets done and how, and the
default setup may not even do anything. See @ref plugins-configuration for
details and a usage example.

@section Trade-AbstractImageConverter-data-dependency Data dependency

The @ref ImageData instances returned from various functions *by design* have
no dependency on the converter instance and neither on the dynamic plugin
module. In other words, you don't need to keep the converter instance (or the
plugin manager instance) around in order to have the @ref ImageData instances
valid. Moreover, all returned @relativeref{Corrade,Containers::Array} instances
returned either directly or through @ref ImageData are only allowed to have
default deleters --- this is to avoid potential dangling function pointer calls
when destructing such instances after the plugin module has been unloaded.

Some converter implementations may point @ref ImageData::importerState() to
some internal state in the converter instance, but in that case the relation is
* *weak* --- these will be valid only as long as the particular converter
documents, usually either until the next conversion is performed or until the
converter is destroyed. After that, the state pointers become dangling, and
that's fine as long as you don't access them.

@section Trade-AbstractImageConverter-subclassing Subclassing

The plugin needs to implement the @ref doFeatures() function and one or more of
@ref doConvert(), @ref doConvertToData() or @ref doConvertToFile() functions
based on what features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The @ref doExtension() and @ref doMimeType() functions are called only
    if @ref ImageConverterFeature::Convert2DToData "ImageConverterFeature::Convert*ToData"
    or @ref ImageConverterFeature::Convert2DToFile "Convert*ToFile" is
    supported
-   The @ref doConvert(), @ref doConvertToData() and @ref doConvertToFile()
    functions are called only if a corresponding @ref ImageConverterFeature is
    supported
-   All @ref doConvertToData() and @ref doConvertToFile() functions taking a
    single (compressed) image are called only if the image has a non-zero size
    in all dimensions and the view is not @cpp nullptr @ce. Note that this does
    not apply to @ref doConvert() --- there a zero-size image or a
    @cpp nullptr @ce view may be a valid use case, and if the plugin
    implementation doesn't work with any of those, it's expected to check that
    on its own and produce a runtime error.
-   All @ref doConvertToData() and @ref doConvertToFile() functions taking
    multiple (compressed) images are called only if the list has at least one
    image, each of the images has a non-zero size, the views are not
    @cpp nullptr @ce and additionally all views have the same pixel format and
    layout flags. Since file formats have varying requirements on image level
    sizes and their order and some don't impose any requirements at all, the
    plugin implementation is expected to check the sizes on its own.

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
         * @snippet Magnum/Trade/AbstractImageConverter.h interface
         *
         * @see @ref MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE
         */
        static Containers::StringView pluginInterface();

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
        static Containers::Array<Containers::String> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractImageConverter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractImageConverter(PluginManager::Manager<AbstractImageConverter>& manager);

        /** @brief Plugin manager constructor */
        /* The plugin name is passed as a const& to make it possible for people
           to implement plugins without even having to include the StringView
           header. */
        explicit AbstractImageConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

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
         * @brief File extension
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2DToFile "ImageConverterFeature::Convert*ToFile"
         * or @ref ImageConverterFeature::Convert2DToData "ImageConverterFeature::Convert*ToData"
         * is supported. Returns a standardized file extension corresponding
         * to the file format used, such as @cpp "png" @ce for PNG files. If
         * the file format doesn't have a standardized extension, empty string
         * is returned.
         *
         * The returned value may depend on flags or configuration options and
         * can change during plugin lifetime.
         */
        Containers::String extension() const;

        /**
         * @brief File MIME type
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2DToFile "ImageConverterFeature::Convert*ToFile"
         * or @ref ImageConverterFeature::Convert2DToData "ImageConverterFeature::Convert*ToData"
         * is supported. Returns a standardized [MIME type](https://en.wikipedia.org/wiki/Media_type)
         * corresponding to the file format used, such as @cpp "image/png" @ce
         * for PNG files. If the file format doesn't have a standardized MIME
         * type, empty string is returned.
         *
         * The returned value may depend on flags or configuration options and
         * can change during plugin lifetime.
         */
        Containers::String mimeType() const;

        /**
         * @brief Convert a 1D image
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert1D is
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}. The
         * implementation is allowed to return both a compressed and an
         * uncompressed image, see documentation of a particular converter for
         * more information.
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
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}. The
         * implementation is allowed to return both a compressed and an
         * uncompressed image, see documentation of a particular converter for
         * more information.
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
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}. The
         * implementation is allowed to return both a compressed and an
         * uncompressed image, see documentation of a particular converter for
         * more information.
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
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}. The
         * implementation is allowed to return both a compressed and an
         * uncompressed image, see documentation of a particular converter for
         * more information.
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
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}. The
         * implementation is allowed to return both a compressed and an
         * uncompressed image, see documentation of a particular converter for
         * more information.
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
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}. The
         * implementation is allowed to return both a compressed and an
         * uncompressed image, see documentation of a particular converter for
         * more information.
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
         *
         * This overload is not provided for multi-level conversion as the
         * view list creation can be done more optimally on the application
         * side.
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
         *
         * This overload is not provided for multi-level conversion as the
         * view list creation can be done more optimally on the application
         * side.
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
         *
         * This overload is not provided for multi-level conversion as the
         * view list creation can be done more optimally on the application
         * side.
         * @see @ref ImageData::isCompressed()
         */
        Containers::Optional<ImageData3D> convert(const ImageData3D& image);

        /**
         * @brief Convert a 1D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert1DToData is
         * supported. The image view is expected to not be @cpp nullptr @ce and
         * to have a non-zero size. On failure prints a message to
         * @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(const CompressedImageView1D&),
         *      @ref convertToData(const ImageData1D&), @ref convert(),
         *      @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const ImageView1D& image);

        /**
         * @brief Convert a 2D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2DToData is
         * supported. The image view is expected to not be @cpp nullptr @ce and
         * to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(const CompressedImageView2D&),
         *      @ref convertToData(const ImageData2D&), @ref convert(),
         *      @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const ImageView2D& image);

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
         * supported. The image view is expected to not be @cpp nullptr @ce and
         * to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(const CompressedImageView3D&),
         *      @ref convertToData(const ImageData3D&), @ref convert(),
         *      @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const ImageView3D& image);

        /**
         * @brief Convert a compressed 1D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed1DToData
         * is supported. The image view is expected to not be @cpp nullptr @ce
         * and to have a non-zero size. On failure prints a message to
         * @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(const ImageView1D&),
         *      @ref convertToData(const ImageData1D&), @ref convert(),
         *      @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const CompressedImageView1D& image);

        /**
         * @brief Convert a compressed 2D image to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::ConvertCompressed2DToData
         * is supported. The image view is expected to not be @cpp nullptr @ce
         * and to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(const ImageView2D&),
         *      @ref convertToData(const ImageData2D&), @ref convert(),
         *      @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const CompressedImageView2D& image);

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
         * is supported. The image view is expected to not be @cpp nullptr @ce
         * and to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(const ImageView3D&),
         *      @ref convertToData(const ImageData3D&), @ref convert(),
         *      @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const CompressedImageView3D& image);

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
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const ImageData1D& image);

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
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const ImageData2D& image);

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
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const ImageData3D& image);

        /**
         * @brief Convert a set of 1D image levels to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,Convert1DToData} is supported.
         * The function expects at least one image to be passed, with each view
         * expected to not be @cpp nullptr @ce, to have a non-zero size, and
         * all of them sharing the same pixel format and layout flags. Note
         * that certain converters may impose additional size and order
         * restrictions on the images, see documentation of a particular plugin
         * for more information. On failure prints a message to
         * @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(Containers::ArrayView<const CompressedImageView1D>),
         *      @ref convert(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(Containers::ArrayView<const ImageView1D> imageLevels);
        /**
         * @overload
         * @m_since_latest
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(std::initializer_list<ImageView1D> imageLevels);

        /**
         * @brief Convert a set of 2D image levels to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,Convert2DToData} is supported.
         * The function expects at least one image to be passed, with each view
         * expected to not be @cpp nullptr @ce, to have a non-zero size in all
         * dimensions, and all views sharing the same pixel format and layout
         * flags. Note that certain converters may impose additional size and
         * order restrictions on the images, see documentation of a particular
         * plugin for more information. On failure prints a message to
         * @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(Containers::ArrayView<const CompressedImageView2D>),
         *      @ref convert(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(Containers::ArrayView<const ImageView2D> imageLevels);
        /**
         * @overload
         * @m_since_latest
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(std::initializer_list<ImageView2D> imageLevels);

        /**
         * @brief Convert a set of 3D image levels to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,Convert3DToData} is supported.
         * The function expects at least one image to be passed, with each view
         * expected to not be @cpp nullptr @ce, to have a non-zero size in all
         * dimensions, and all views sharing the same pixel format and layout
         * flags. Note that certain converters may impose additional size and
         * order restrictions on the images, see documentation of a particular
         * plugin for more information. On failure prints a message to
         * @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(Containers::ArrayView<const CompressedImageView3D>),
         *      @ref convert(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(Containers::ArrayView<const ImageView3D> imageLevels);
        /**
         * @overload
         * @m_since_latest
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(std::initializer_list<ImageView3D> imageLevels);

        /**
         * @brief Convert a set of compressed 1D image levels to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,ConvertCompressed1DToData} is
         * supported. The function expects at least one image to be passed,
         * with each view expected to not be @cpp nullptr @ce, to have a
         * non-zero size, and all views sharing the same pixel format and
         * layout flags. Note that certain converters may impose additional
         * size and order restrictions on the images, see documentation of a
         * particular plugin for more information. On failure prints a message
         * to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(Containers::ArrayView<const ImageView1D>),
         *      @ref convert(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(Containers::ArrayView<const CompressedImageView1D> imageLevels);
        /**
         * @overload
         * @m_since_latest
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(std::initializer_list<CompressedImageView1D> imageLevels);

        /**
         * @brief Convert a set of compressed 2D image levels to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,ConvertCompressed2DToData} is
         * supported. The function expects at least one image to be passed,
         * with each view expected to not be @cpp nullptr @ce, to have a
         * non-zero size in all dimensions, and all views sharing the same
         * pixel format and layout flags. Note that certain converters may
         * impose additional size and order restrictions on the images, see
         * documentation of a particular plugin for more information. On
         * failure prints a message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(Containers::ArrayView<const ImageView2D>),
         *      @ref convert(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(Containers::ArrayView<const CompressedImageView2D> imageLevels);
        /**
         * @overload
         * @m_since_latest
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(std::initializer_list<CompressedImageView2D> imageLevels);

        /**
         * @brief Convert a set of compressed 3D image levels to a raw data
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,ConvertCompressed3DToData} is supported. The function expects at least one image to be passed,
         * with each view expected to not be @cpp nullptr @ce, to have a
         * non-zero size in all dimensions, and all views sharing the same
         * pixel format and layout flags. Note that certain converters may
         * impose additional size and order restrictions on the images, see
         * documentation of a particular plugin for more information. On
         * failure prints a message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @see @ref features(), @ref convertToData(Containers::ArrayView<const ImageView3D>),
         *      @ref convert(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(Containers::ArrayView<const CompressedImageView3D> images);
        /**
         * @overload
         * @m_since_latest
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::ImageConverterOptionalButAlsoArray<char>
        #endif
        convertToData(std::initializer_list<CompressedImageView3D> images);

        /**
         * @brief Convert a 1D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert1DToFile is
         * supported. The image view is expected to not be @cpp nullptr @ce and
         * to have a non-zero size. On failure prints a message to
         * @relativeref{Magnum,Error} and returns @cpp false @ce.
         * @see @ref features(), @ref convertToFile(const CompressedImageView1D&, Containers::StringView),
         *      @ref convertToFile(const ImageData1D&, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(const ImageView1D& image, Containers::StringView filename);

        /**
         * @brief Convert a 2D image to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Convert2DToFile is
         * supported. The image view is expected to not be @cpp nullptr @ce and
         * to have a non-zero size in all dimensions. Returns @cpp true @ce on
         * success, @cpp false @ce otherwise.
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
         * Available only if @ref ImageConverterFeature::Convert3DToFile is
         * supported. The image view is expected to not be @cpp nullptr @ce and
         * to have a non-zero size. On failure prints a message to
         * @relativeref{Magnum,Error} and returns @cpp false @ce.
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
         * is supported. The image view is expected to not be @cpp nullptr @ce
         * and to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns @cpp false @ce.
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
         * is supported. The image view is expected to not be @cpp nullptr @ce
         * and to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns @cpp false @ce.
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
         * is supported. The image view is expected to not be @cpp nullptr @ce
         * and to have a non-zero size in all dimensions. On failure prints a
         * message to @relativeref{Magnum,Error} and returns @cpp false @ce.
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
         *
         * This overload is not provided for multi-level conversion as the
         * view list creation can be done more optimally on the application
         * side.
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
         *
         * This overload is not provided for multi-level conversion as the
         * view list creation can be done more optimally on the application
         * side.
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
         *
         * This overload is not provided for multi-level conversion as the
         * view list creation can be done more optimally on the application
         * side.
         * @see @ref ImageData::isCompressed()
         */
        bool convertToFile(const ImageData3D& image, Containers::StringView filename);

        /**
         * @brief Convert a set of 1D image levels to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,Convert1DToFile} is supported.
         * The function expects at least one image to be passed, with each view
         * expected to not be @cpp nullptr @ce, to have a non-zero size, and
         * all views sharing the same pixel format and layout flags. Note that
         * certain converters may impose additional size and order restrictions
         * on the images, see documentation of a particular plugin for more
         * information. On failure prints a message to
         * @relativeref{Magnum,Error} and returns @cpp false @ce.
         * @see @ref features(), @ref convertToFile(Containers::ArrayView<const CompressedImageView1D>, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(Containers::ArrayView<const ImageView1D> imageLevels, Containers::StringView filename);
        /**
         * @overload
         * @m_since_latest
         */
        bool convertToFile(std::initializer_list<ImageView1D> imageLevels, Containers::StringView filename);

        /**
         * @brief Convert a set of 2D image levels to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,Convert2DToFile} is supported.
         * The function expects at least one image to be passed, with each view
         * expected to not be @cpp nullptr @ce, to have a non-zero size in all
         * dimensions, and all views sharing the same pixel format and layout
         * flags. Note that certain converters may impose additional size and
         * order restrictions on the images, see documentation of a particular
         * plugin for more information. On failure prints a message to
         * @relativeref{Magnum,Error} and returns @cpp false @ce.
         * @see @ref features(), @ref convertToFile(Containers::ArrayView<const CompressedImageView2D>, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(Containers::ArrayView<const ImageView2D> imageLevels, Containers::StringView filename);
        /**
         * @overload
         * @m_since_latest
         */
        bool convertToFile(std::initializer_list<ImageView2D> imageLevels, Containers::StringView filename);

        /**
         * @brief Convert a set of 3D image levels to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,Convert3DToFile} is supported.
         * The function expects at least one image to be passed, with each view
         * expected to not be @cpp nullptr @ce, to have a non-zero size in all
         * dimensions, and all views sharing the same pixel format and layout
         * flags. Note that certain converters may impose additional size and
         * order restrictions on the images, see documentation of a particular
         * plugin for more information. On failure prints a message to
         * @relativeref{Magnum,Error} and returns @cpp false @ce.
         * @see @ref features(), @ref convertToFile(Containers::ArrayView<const CompressedImageView3D>, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(Containers::ArrayView<const ImageView3D> imageLevels, Containers::StringView filename);
        /**
         * @overload
         * @m_since_latest
         */
        bool convertToFile(std::initializer_list<ImageView3D> imageLevels, Containers::StringView filename);

        /**
         * @brief Convert a set of compressed 1D image levels to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,ConvertCompressed1DToFile} is
         * supported. The function expects at least one image to be passed,
         * with each view expected to not be @cpp nullptr @ce, to have a
         * non-zero size, and all views sharing the same pixel format and
         * layout flags. Note that certain converters may impose additional
         * size and order restrictions on the images, see documentation of a
         * particular plugin for more information. On failure prints a message
         * to @relativeref{Magnum,Error} and returns @cpp false @ce.
         * @see @ref features(), @ref convertToFile(Containers::ArrayView<const ImageView1D>, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(Containers::ArrayView<const CompressedImageView1D> imageLevels, Containers::StringView filename);
        /**
         * @overload
         * @m_since_latest
         */
        bool convertToFile(std::initializer_list<CompressedImageView1D> imageLevels, Containers::StringView filename);

        /**
         * @brief Convert a set of compressed 2D image levels to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,ConvertCompressed2DToFile} is
         * supported. The function expects at least one image to be passed,
         * with each view expected to not be @cpp nullptr @ce, to have a
         * non-zero size in all dimensions, and all views sharing the same
         * pixel format and layout flags. Note that certain converters may
         * impose additional size and order restrictions on the images, see
         * documentation of a particular plugin for more information. On
         * failure prints a message to @relativeref{Magnum,Error} and returns
         * @cpp false @ce.
         * @see @ref features(), @ref convertToFile(Containers::ArrayView<const ImageView2D>, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(Containers::ArrayView<const CompressedImageView2D> imageLevels, Containers::StringView filename);
        /**
         * @overload
         * @m_since_latest
         */
        bool convertToFile(std::initializer_list<CompressedImageView2D> imageLevels, Containers::StringView filename);

        /**
         * @brief Convert a set of compressed 3D image levels to a file
         * @m_since_latest
         *
         * Available only if @ref ImageConverterFeature::Levels together with
         * @relativeref{ImageConverterFeature,ConvertCompressed3DToFile} is
         * supported. The function expects at least one image to be passed,
         * with each view expected to not be @cpp nullptr @ce, to have a
         * non-zero size in all dimensions, and all views sharing the same
         * pixel format and layout flags. Note that certain converters may
         * impose additional size and order restrictions on the images, see
         * documentation of a particular plugin for more information. On
         * failure prints a message to @relativeref{Magnum,Error} and returns
         * @cpp false @ce.
         * @see @ref features(), @ref convertToFile(Containers::ArrayView<const ImageView3D>, Containers::StringView),
         *      @ref convert(), @ref convertToData()
         */
        bool convertToFile(Containers::ArrayView<const CompressedImageView3D> imageLevels, Containers::StringView filename);
        /**
         * @overload
         * @m_since_latest
         */
        bool convertToFile(std::initializer_list<CompressedImageView3D> imageLevels, Containers::StringView filename);

    protected:
        /**
         * @brief Implementation for @ref convertToFile(const ImageView1D&, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToFile(Containers::ArrayView<const ImageView1D>, Containers::StringView)
         * with just the single @p image. Otherwise, if
         * @ref ImageConverterFeature::Convert1DToData is supported, default
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
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToFile(Containers::ArrayView<const ImageView2D>, Containers::StringView)
         * with just the single @p image. Otherwise, if
         * @ref ImageConverterFeature::Convert2DToData is supported, default
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
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToFile(Containers::ArrayView<const ImageView3D>, Containers::StringView)
         * with just the single @p image. Otherwise, if
         * @ref ImageConverterFeature::Convert3DToData is supported, default
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
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToFile(Containers::ArrayView<const CompressedImageView1D>, Containers::StringView)
         * with just the single @p image. Otherwise, if
         * @ref ImageConverterFeature::ConvertCompressed1DToData is supported,
         * default implementation calls @ref doConvertToData(const CompressedImageView1D&)
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
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToFile(Containers::ArrayView<const CompressedImageView2D>, Containers::StringView)
         * with just the single @p image. Otherwise, if
         * @ref ImageConverterFeature::ConvertCompressed2DToData is supported,
         * default implementation calls @ref doConvertToData(const CompressedImageView2D&)
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
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToFile(Containers::ArrayView<const CompressedImageView3D>, Containers::StringView)
         * with just the single @p image. Otherwise, if
         * @ref ImageConverterFeature::ConvertCompressed3DToData is supported,
         * default implementation calls @ref doConvertToData(const CompressedImageView3D&)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(const CompressedImageView3D& image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(Containers::ArrayView<const ImageView1D>, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Convert1DToData is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const ImageView1D>)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(Containers::ArrayView<const ImageView1D> imageLevels, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(Containers::ArrayView<const ImageView2D>, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Convert2DToData is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const ImageView2D>)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(Containers::ArrayView<const ImageView2D> imageLevels, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(Containers::ArrayView<const ImageView3D>, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Convert3DToData is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const ImageView3D>)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(Containers::ArrayView<const ImageView3D> imageLevels, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(Containers::ArrayView<const CompressedImageView1D>, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::ConvertCompressed1DToData is
         * supported, default implementation calls
         * @ref doConvertToData(Containers::ArrayView<const CompressedImageView1D>)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(Containers::ArrayView<const CompressedImageView1D> image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(Containers::ArrayView<const CompressedImageView2D>, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::ConvertCompressed2DToData is
         * supported, default implementation calls @ref doConvertToData(Containers::ArrayView<const CompressedImageView2D>)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(Containers::ArrayView<const CompressedImageView2D> image, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertToFile(Containers::ArrayView<const CompressedImageView3D>, Containers::StringView)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::ConvertCompressed3DToData is
         * supported, default implementation calls @ref doConvertToData(Containers::ArrayView<const CompressedImageView3D>)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         */
        virtual bool doConvertToFile(Containers::ArrayView<const CompressedImageView3D> image, Containers::StringView filename);

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
         * @brief Implementation for @ref extension()
         * @m_since_latest
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doExtension() const;

        /**
         * @brief Implementation for @ref mimeType()
         * @m_since_latest
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doMimeType() const;

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
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const ImageView1D>)
         * with just the single @p image and propagates the result back.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D& image);

        /**
         * @brief Implementation for @ref convertToData(const ImageView2D&)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const ImageView2D>)
         * with just the single @p image and propagates the result back.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D& image);

        /**
         * @brief Implementation for @ref convertToData(const ImageView3D&)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const ImageView3D>)
         * with just the single @p image and propagates the result back.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D& image);

        /**
         * @brief Implementation for @ref convertToData(const CompressedImageView1D&)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const CompressedImageView1D>)
         * with just the single @p image and propagates the result back.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D& image);

        /**
         * @brief Implementation for @ref convertToData(const CompressedImageView2D&)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const CompressedImageView2D>)
         * with just the single @p image and propagates the result back.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D& image);

        /**
         * @brief Implementation for @ref convertToData(const CompressedImageView3D&)
         * @m_since_latest
         *
         * If @ref ImageConverterFeature::Levels is supported, default
         * implementation calls @ref doConvertToData(Containers::ArrayView<const CompressedImageView3D>)
         * with just the single @p image and propagates the result back.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D& image);

        /**
         * @brief Implementation for @ref convertToData(Containers::ArrayView<const ImageView1D>)
         * @m_since_latest
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D> imageLevels);

        /**
         * @brief Implementation for @ref convertToData(Containers::ArrayView<const ImageView2D>)
         * @m_since_latest
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D> imageLevels);

        /**
         * @brief Implementation for @ref convertToData(Containers::ArrayView<const ImageView3D>)
         * @m_since_latest
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D> imageLevels);

        /**
         * @brief Implementation for @ref convertToData(Containers::ArrayView<const CompressedImageView1D>)
         * @m_since_latest
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D> imageLevels);

        /**
         * @brief Implementation for @ref convertToData(Containers::ArrayView<const CompressedImageView2D>)
         * @m_since_latest
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D> imageLevels);

        /**
         * @brief Implementation for @ref convertToData(Containers::ArrayView<const CompressedImageView3D>)
         * @m_since_latest
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D> imageLevels);

        ImageConverterFlags _flags;
};

/**
@brief Image converter plugin interface
@m_since_latest

Same string as returned by
@relativeref{Magnum::Trade,AbstractImageConverter::pluginInterface()}, meant to
be used inside @ref CORRADE_PLUGIN_REGISTER() to avoid having to update the
interface string by hand every time the version gets bumped:

@snippet Trade.cpp MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE

The interface string version gets increased on every ABI break to prevent
silent crashes and memory corruption. Plugins built against the previous
version will then fail to load, a subsequent rebuild will make them pick up the
updated interface string.
*/
/* Silly indentation to make the string appear in pluginInterface() docs */
#define MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE /* [interface] */ \
"cz.mosra.magnum.Trade.AbstractImageConverter/0.3.3"
/* [interface] */

}}

#endif
