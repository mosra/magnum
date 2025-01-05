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

#include "AbstractImageConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/StringStl.h>
#endif
#include <Corrade/PluginManager/Manager.hpp>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/ImageData.h"

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Trade/configure.h"
#endif

namespace Corrade { namespace PluginManager {

template class MAGNUM_TRADE_EXPORT Manager<Magnum::Trade::AbstractImageConverter>;

}}

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

Containers::StringView AbstractImageConverter::pluginInterface() {
    return MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE ""_s;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
Containers::Array<Containers::String> AbstractImageConverter::pluginSearchPaths() {
    const Containers::Optional<Containers::String> libraryLocation = Utility::Path::libraryLocation(&pluginInterface);
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        libraryLocation ? *libraryLocation : Containers::String{},
        #else
        {},
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_DIR,
        #else
        MAGNUM_PLUGINS_IMAGECONVERTER_DIR,
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/"
        #else
        "magnum/"
        #endif
        "imageconverters"_s);
}
#endif

AbstractImageConverter::AbstractImageConverter() = default;

AbstractImageConverter::AbstractImageConverter(PluginManager::Manager<AbstractImageConverter>& manager): PluginManager::AbstractManagingPlugin<AbstractImageConverter>{manager} {}

AbstractImageConverter::AbstractImageConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): PluginManager::AbstractManagingPlugin<AbstractImageConverter>{manager, plugin} {}

void AbstractImageConverter::setFlags(ImageConverterFlags flags) {
    _flags = flags;
    doSetFlags(flags);
}

void AbstractImageConverter::doSetFlags(ImageConverterFlags) {}

void AbstractImageConverter::addFlags(ImageConverterFlags flags) {
    setFlags(_flags|flags);
}

void AbstractImageConverter::clearFlags(ImageConverterFlags flags) {
    setFlags(_flags & ~flags);
}

Containers::String AbstractImageConverter::extension() const {
    CORRADE_ASSERT(features() & (ImageConverterFeature::Convert1DToFile|
                                 ImageConverterFeature::Convert2DToFile|
                                 ImageConverterFeature::Convert3DToFile|
                                 ImageConverterFeature::ConvertCompressed1DToFile|
                                 ImageConverterFeature::ConvertCompressed2DToFile|
                                 ImageConverterFeature::ConvertCompressed3DToFile),
        "Trade::AbstractImageConverter::extension(): file conversion not supported", {});

    Containers::String out = doExtension();
    CORRADE_ASSERT(out.isSmall() || !out.deleter(),
        "Trade::AbstractImageConverter::extension(): implementation is not allowed to use a custom String deleter", {});
    return out;
}

Containers::String AbstractImageConverter::doExtension() const { return {}; }

Containers::String AbstractImageConverter::mimeType() const {
    CORRADE_ASSERT(features() & (ImageConverterFeature::Convert1DToFile|
                                 ImageConverterFeature::Convert2DToFile|
                                 ImageConverterFeature::Convert3DToFile|
                                 ImageConverterFeature::ConvertCompressed1DToFile|
                                 ImageConverterFeature::ConvertCompressed2DToFile|
                                 ImageConverterFeature::ConvertCompressed3DToFile),
        "Trade::AbstractImageConverter::mimeType(): file conversion not supported", {});

    Containers::String out = doMimeType();
    CORRADE_ASSERT(out.isSmall() || !out.deleter(),
        "Trade::AbstractImageConverter::mimeType(): implementation is not allowed to use a custom String deleter", {});
    return out;
}

Containers::String AbstractImageConverter::doMimeType() const { return {}; }

Containers::Optional<ImageData1D> AbstractImageConverter::convert(const ImageView1D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert1D,
        "Trade::AbstractImageConverter::convert(): 1D image conversion not supported", {});

    /* Unlike with convertToData() / convertToFile(), where images are checked
       for having a non-zero size and being non-null, here it's explicitly
       allowed:

        -   Usual format conversions / resamplings are a loop that "just works"
            with zero-sized dimensions.
        -   Nullptr views could be potentially useful for converters that fill
            an image of desired format and size with data, such as generating a
            noise image or some test pattern. While it could be implemented as
            an importer plugin as well, the size/format would have to be passed
            manually through plugin-specific configuration there, which is
            rather annoying compared to having an API prepared directly for
            that.

       Plugin implementations that can't work with these have to do this check
       themselves, and since that's then a plugin-specific behavior, it should
       be a runtime error, not an assert. */

    Containers::Optional<ImageData1D> out = doConvert(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<ImageData1D> AbstractImageConverter::doConvert(const ImageView1D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convert(): 1D image conversion advertised but not implemented", {});
}

Containers::Optional<ImageData2D> AbstractImageConverter::convert(const ImageView2D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert2D,
        "Trade::AbstractImageConverter::convert(): 2D image conversion not supported", {});

    /* No zero size / nullptr checks here, see convert(const ImageView1D&) for
       reasons why */

    Containers::Optional<ImageData2D> out = doConvert(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<ImageData2D> AbstractImageConverter::doConvert(const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convert(): 2D image conversion advertised but not implemented", {});
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Image2D> AbstractImageConverter::exportToImage(const ImageView2D& image) {
    Containers::Optional<ImageData2D> out = convert(image);
    if(!out) return {};
    if(out->isCompressed()) {
        Error{} << "Trade::AbstractImageConverter::exportToImage(): implementation returned a compressed image";
        return {};
    }

    const PixelStorage storage = out->storage();
    const PixelFormat format = out->format();
    const UnsignedInt formatExtra = out->formatExtra();
    const UnsignedInt pixelSize = out->pixelSize();
    const Vector2i size = out->size();
    return Image2D{storage, format, formatExtra, pixelSize, size, out->release()};
}

Containers::Optional<CompressedImage2D> AbstractImageConverter::exportToCompressedImage(const ImageView2D& image) {
    Containers::Optional<ImageData2D> out = convert(image);
    if(!out) return {};
    if(!out->isCompressed()) {
        Error{} << "Trade::AbstractImageConverter::exportToCompressedImage(): implementation returned an uncompressed image";
        return {};
    }

    const CompressedPixelStorage storage = out->compressedStorage();
    const CompressedPixelFormat format = out->compressedFormat();
    const Vector2i size = out->size();
    return CompressedImage2D{storage, format, size, out->release()};
}
#endif

Containers::Optional<ImageData3D> AbstractImageConverter::convert(const ImageView3D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert3D,
        "Trade::AbstractImageConverter::convert(): 3D image conversion not supported", {});

    /* No zero size / nullptr checks here, see convert(const ImageView1D&) for
       reasons why */

    Containers::Optional<ImageData3D> out = doConvert(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<ImageData3D> AbstractImageConverter::doConvert(const ImageView3D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convert(): 3D image conversion advertised but not implemented", {});
}

Containers::Optional<ImageData1D> AbstractImageConverter::convert(const CompressedImageView1D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed1D,
        "Trade::AbstractImageConverter::convert(): compressed 1D image conversion not supported", {});

    /* No zero size / nullptr checks here, see convert(const ImageView1D&) for
       reasons why */

    Containers::Optional<ImageData1D> out = doConvert(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<ImageData1D> AbstractImageConverter::doConvert(const CompressedImageView1D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convert(): compressed 1D image conversion advertised but not implemented", {});
}

Containers::Optional<ImageData2D> AbstractImageConverter::convert(const CompressedImageView2D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed2D,
        "Trade::AbstractImageConverter::convert(): compressed 2D image conversion not supported", {});

    /* No zero size / nullptr checks here, see convert(const ImageView1D&) for
       reasons why */

    Containers::Optional<ImageData2D> out = doConvert(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<ImageData2D> AbstractImageConverter::doConvert(const CompressedImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convert(): compressed 2D image conversion advertised but not implemented", {});
}

Containers::Optional<ImageData3D> AbstractImageConverter::convert(const CompressedImageView3D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed3D,
        "Trade::AbstractImageConverter::convert(): compressed 3D image conversion not supported", {});

    /* No zero size / nullptr checks here, see convert(const ImageView1D&) for
       reasons why */

    Containers::Optional<ImageData3D> out = doConvert(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<ImageData3D> AbstractImageConverter::doConvert(const CompressedImageView3D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convert(): compressed 3D image conversion advertised but not implemented", {});
}

Containers::Optional<ImageData1D> AbstractImageConverter::convert(const ImageData1D& image) {
    return image.isCompressed() ? convert(CompressedImageView1D(image)) : convert(ImageView1D(image));
}

Containers::Optional<ImageData2D> AbstractImageConverter::convert(const ImageData2D& image) {
    return image.isCompressed() ? convert(CompressedImageView2D(image)) : convert(ImageView2D(image));
}

Containers::Optional<ImageData3D> AbstractImageConverter::convert(const ImageData3D& image) {
    return image.isCompressed() ? convert(CompressedImageView3D(image)) : convert(ImageView3D(image));
}

#ifndef CORRADE_NO_ASSERT
namespace {

template<UnsignedInt dimensions, template<UnsignedInt, class> class View> bool checkImageValidity(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const messagePrefix
    #endif
    , const View<dimensions, const char>& image)
{
    /* At some point there might be a file format that allows zero-sized
       images, but so far I don't know about any. When such format appears,
       this check will get moved to plugin implementations that can't work with
       zero-sized images.

       Also note that this check isn't done for the Image->Image conversion
       above, there zero-sized images and nullptr *could* make sense. */
    CORRADE_ASSERT(image.size().product(),
        messagePrefix << "can't convert image with a zero size:" << image.size(), false);
    CORRADE_ASSERT(image.data(),
        messagePrefix << "can't convert image with a nullptr view", false);
    return true;
}

}
#endif

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const ImageView1D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert1DToData,
        "Trade::AbstractImageConverter::convertToData(): 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", image))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(image);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(const ImageView1D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): 1D image conversion advertised but not implemented", {});

    return doConvertToData(Containers::arrayView({image}));
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const ImageView2D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert2DToData,
        "Trade::AbstractImageConverter::convertToData(): 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", image))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(image);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(const ImageView2D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): 2D image conversion advertised but not implemented", {});

    return doConvertToData(Containers::arrayView({image}));
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Array<char> AbstractImageConverter::exportToData(const ImageView2D& image) {
    Containers::Optional<Containers::Array<char>> out = convertToData(image);
    return out ? *Utility::move(out) : nullptr;
}
#endif

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const ImageView3D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert3DToData,
        "Trade::AbstractImageConverter::convertToData(): 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", image))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(image);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(const ImageView3D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): 3D image conversion advertised but not implemented", {});

    return doConvertToData(Containers::arrayView({image}));
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const CompressedImageView1D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed1DToData,
        "Trade::AbstractImageConverter::convertToData(): compressed 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", image))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(image);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(const CompressedImageView1D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): compressed 1D image conversion advertised but not implemented", {});

    return doConvertToData(Containers::arrayView({image}));
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const CompressedImageView2D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed2DToData,
        "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", image))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(image);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(const CompressedImageView2D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion advertised but not implemented", {});

    return doConvertToData(Containers::arrayView({image}));
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Array<char> AbstractImageConverter::exportToData(const CompressedImageView2D& image) {
    Containers::Optional<Containers::Array<char>> out = convertToData(image);
    return out ? *Utility::move(out) : nullptr;
}
#endif

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const CompressedImageView3D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed3DToData,
        "Trade::AbstractImageConverter::convertToData(): compressed 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", image))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(image);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(const CompressedImageView3D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): compressed 3D image conversion advertised but not implemented", {});

    return doConvertToData(Containers::arrayView({image}));
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const ImageData1D& image) {
    return image.isCompressed() ? convertToData(CompressedImageView1D(image)) : convertToData(ImageView1D(image));
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const ImageData2D& image) {
    return image.isCompressed() ? convertToData(CompressedImageView2D(image)) : convertToData(ImageView2D(image));
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Array<char> AbstractImageConverter::exportToData(const ImageData2D& image) {
    Containers::Optional<Containers::Array<char>> out = convertToData(image);
    return out ? *Utility::move(out) : nullptr;
}
#endif

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const ImageData3D& image) {
    return image.isCompressed() ? convertToData(CompressedImageView3D(image)) : convertToData(ImageView3D(image));
}

#ifndef CORRADE_NO_ASSERT
namespace {

template<UnsignedInt dimensions> bool checkImageValidity(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const messagePrefix
    #endif
    , const Containers::ArrayView<const BasicImageView<dimensions>> imageLevels)
{
    CORRADE_ASSERT(!imageLevels.isEmpty(),
        messagePrefix << "at least one image has to be specified", false);

    const PixelFormat format = imageLevels[0].format();
    const UnsignedInt formatExtra = imageLevels[0].formatExtra();
    const ImageFlags<dimensions> flags = imageLevels[0].flags();
    /* Going through *all* levels although the format assertion is never fired
       in the first iteration in order to properly check also the first one for
       zero size / nullptr. */
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        CORRADE_ASSERT(imageLevels[i].size().product(),
            messagePrefix << "can't convert image" << i << "with a zero size:" << imageLevels[i].size(), false);
        CORRADE_ASSERT(imageLevels[i].data(),
            messagePrefix << "can't convert image" << i << "with a nullptr view", false);
        CORRADE_ASSERT(imageLevels[i].format() == format,
            messagePrefix << "levels don't have the same format, expected" << format << "but got" << imageLevels[i].format() << "for image" << i, false);
        CORRADE_ASSERT(imageLevels[i].formatExtra() == formatExtra,
            messagePrefix << "levels don't have the same extra format field, expected" << formatExtra << "but got" << imageLevels[i].formatExtra() << "for image" << i, false);
        CORRADE_ASSERT(imageLevels[i].flags() == flags,
            messagePrefix << "levels don't have the same flags, expected" << flags << "but got" << imageLevels[i].flags() << "for image" << i, false);
    }

    return true;
}

template<UnsignedInt dimensions> bool checkImageValidity(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const messagePrefix
    #endif
    , const Containers::ArrayView<const BasicCompressedImageView<dimensions>> imageLevels)
{
    CORRADE_ASSERT(!imageLevels.isEmpty(),
        messagePrefix << "at least one image has to be specified", false);

    const CompressedPixelFormat format = imageLevels[0].format();
    const ImageFlags<dimensions> flags = imageLevels[0].flags();
    /* Going through *all* levels although the format assertion is never fired
       in the first iteration in order to properly check also the first one for
       zero size / nullptr. */
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        CORRADE_ASSERT(imageLevels[i].size().product(),
            messagePrefix << "can't convert image" << i << "with a zero size:" << imageLevels[i].size(), false);
        CORRADE_ASSERT(imageLevels[i].data(),
            messagePrefix << "can't convert image" << i << "with a nullptr view", false);
        CORRADE_ASSERT(imageLevels[i].format() == format,
            messagePrefix << "levels don't have the same format, expected" << format << "but got" << imageLevels[i].format() << "for image" << i, false);
        CORRADE_ASSERT(imageLevels[i].flags() == flags,
            messagePrefix << "levels don't have the same flags, expected" << flags << "but got" << imageLevels[i].flags() << "for image" << i, false);
    }

    return true;
}

}
#endif

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const Containers::ArrayView<const ImageView1D> imageLevels) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): multi-level 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", imageLevels))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(imageLevels);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const std::initializer_list<ImageView1D> imageLevels) {
    return convertToData(Containers::arrayView(imageLevels));
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(Containers::ArrayView<const ImageView1D>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): multi-level 1D image conversion advertised but not implemented", {});
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const Containers::ArrayView<const ImageView2D> imageLevels) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): multi-level 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", imageLevels))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(imageLevels);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const std::initializer_list<ImageView2D> imageLevels) {
    return convertToData(Containers::arrayView(imageLevels));
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(Containers::ArrayView<const ImageView2D>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): multi-level 2D image conversion advertised but not implemented", {});
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const Containers::ArrayView<const ImageView3D> imageLevels) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): multi-level 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", imageLevels))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(imageLevels);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const std::initializer_list<ImageView3D> imageLevels) {
    return convertToData(Containers::arrayView(imageLevels));
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(Containers::ArrayView<const ImageView3D>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): multi-level 3D image conversion advertised but not implemented", {});
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const Containers::ArrayView<const CompressedImageView1D> imageLevels) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): multi-level compressed 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", imageLevels))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(imageLevels);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const std::initializer_list<CompressedImageView1D> imageLevels) {
    return convertToData(Containers::arrayView(imageLevels));
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(Containers::ArrayView<const CompressedImageView1D>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): multi-level compressed 1D image conversion advertised but not implemented", {});
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const Containers::ArrayView<const CompressedImageView2D> imageLevels) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): multi-level compressed 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", imageLevels))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(imageLevels);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const std::initializer_list<CompressedImageView2D> imageLevels) {
    return convertToData(Containers::arrayView(imageLevels));
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(Containers::ArrayView<const CompressedImageView2D>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): multi-level compressed 2D image conversion advertised but not implemented", {});
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const Containers::ArrayView<const CompressedImageView3D> imageLevels) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Levels,
        "Trade::AbstractImageConverter::convertToData(): multi-level compressed 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToData():", imageLevels))
        return {};
    #endif

    Containers::Optional<Containers::Array<char>> out = doConvertToData(imageLevels);
    CORRADE_ASSERT(!out || !out->deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::ImageConverterOptionalButAlsoArray<char>{Utility::move(out)};
    #else
    return out;
    #endif
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::ImageConverterOptionalButAlsoArray<char>
#endif
AbstractImageConverter::convertToData(const std::initializer_list<CompressedImageView3D> imageLevels) {
    return convertToData(Containers::arrayView(imageLevels));
}

Containers::Optional<Containers::Array<char>> AbstractImageConverter::doConvertToData(Containers::ArrayView<const CompressedImageView3D>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): multi-level compressed 3D image conversion advertised but not implemented", {});
}

bool AbstractImageConverter::convertToFile(const ImageView1D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert1DToFile,
        "Trade::AbstractImageConverter::convertToFile(): 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", image))
        return {};
    #endif

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const ImageView1D& image, const Containers::StringView filename) {
    /* Prefer to go through the ToFile variant instead of ToData assuming that
       it could have a more memory-efficient implementation than having to
       materialize the whole output in memory first */
    if(features() >= ImageConverterFeature::Levels)
        return doConvertToFile(Containers::arrayView({image}), filename);

    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert1DToData, "Trade::AbstractImageConverter::convertToFile(): 1D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const ImageView2D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert2DToFile,
        "Trade::AbstractImageConverter::convertToFile(): 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", image))
        return {};
    #endif

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const ImageView2D& image, const Containers::StringView filename) {
    /* Prefer to go through the ToFile variant instead of ToData assuming that
       it could have a more memory-efficient implementation than having to
       materialize the whole output in memory first */
    if(features() >= ImageConverterFeature::Levels)
        return doConvertToFile(Containers::arrayView({image}), filename);

    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert2DToData, "Trade::AbstractImageConverter::convertToFile(): 2D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractImageConverter::exportToFile(const ImageView2D& image, const std::string& filename) {
    return convertToFile(image, filename);
}
#endif

bool AbstractImageConverter::convertToFile(const ImageView3D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert3DToFile,
        "Trade::AbstractImageConverter::convertToFile(): 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", image))
        return {};
    #endif

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const ImageView3D& image, const Containers::StringView filename) {
    /* Prefer to go through the ToFile variant instead of ToData assuming that
       it could have a more memory-efficient implementation than having to
       materialize the whole output in memory first */
    if(features() >= ImageConverterFeature::Levels)
        return doConvertToFile(Containers::arrayView({image}), filename);

    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert3DToData, "Trade::AbstractImageConverter::convertToFile(): 3D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const CompressedImageView1D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed1DToFile,
        "Trade::AbstractImageConverter::convertToFile(): compressed 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", image))
        return {};
    #endif

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const CompressedImageView1D& image, Containers::StringView filename) {
    /* Prefer to go through the ToFile variant instead of ToData assuming that
       it could have a more memory-efficient implementation than having to
       materialize the whole output in memory first */
    if(features() >= ImageConverterFeature::Levels)
        return doConvertToFile(Containers::arrayView({image}), filename);

    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed1DToData, "Trade::AbstractImageConverter::convertToFile(): compressed 1D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const CompressedImageView2D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed2DToFile,
        "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", image))
        return {};
    #endif

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const CompressedImageView2D& image, Containers::StringView filename) {
    /* Prefer to go through the ToFile variant instead of ToData assuming that
       it could have a more memory-efficient implementation than having to
       materialize the whole output in memory first */
    if(features() >= ImageConverterFeature::Levels)
        return doConvertToFile(Containers::arrayView({image}), filename);

    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed2DToData, "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractImageConverter::exportToFile(const CompressedImageView2D& image, const std::string& filename) {
    return convertToFile(image, filename);
}
#endif

bool AbstractImageConverter::convertToFile(const CompressedImageView3D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed3DToFile,
        "Trade::AbstractImageConverter::convertToFile(): compressed 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", image))
        return {};
    #endif

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const CompressedImageView3D& image, Containers::StringView filename) {
    /* Prefer to go through the ToFile variant instead of ToData assuming that
       it could have a more memory-efficient implementation than having to
       materialize the whole output in memory first */
    if(features() >= ImageConverterFeature::Levels)
        return doConvertToFile(Containers::arrayView({image}), filename);

    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed3DToData, "Trade::AbstractImageConverter::convertToFile(): compressed 3D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const ImageData1D& image, const Containers::StringView filename) {
    return image.isCompressed() ? convertToFile(CompressedImageView1D(image), filename) : convertToFile(ImageView1D(image), filename);
}

bool AbstractImageConverter::convertToFile(const ImageData2D& image, const Containers::StringView filename) {
    return image.isCompressed() ? convertToFile(CompressedImageView2D(image), filename) : convertToFile(ImageView2D(image), filename);
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractImageConverter::exportToFile(const ImageData2D& image, const std::string& filename) {
    return convertToFile(image, filename);
}
#endif

bool AbstractImageConverter::convertToFile(const ImageData3D& image, const Containers::StringView filename) {
    return image.isCompressed() ? convertToFile(CompressedImageView3D(image), filename) : convertToFile(ImageView3D(image), filename);
}

bool AbstractImageConverter::convertToFile(const Containers::ArrayView<const ImageView1D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= (ImageConverterFeature::Convert1DToFile|ImageConverterFeature::Levels),
        "Trade::AbstractImageConverter::convertToFile(): multi-level 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", imageLevels))
        return {};
    #endif

    return doConvertToFile(imageLevels, filename);
}

bool AbstractImageConverter::convertToFile(const std::initializer_list<ImageView1D> imageLevels, const Containers::StringView filename) {
    return convertToFile(Containers::arrayView(imageLevels), filename);
}

bool AbstractImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView1D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert1DToData, "Trade::AbstractImageConverter::convertToFile(): multi-level 1D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(imageLevels);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const Containers::ArrayView<const ImageView2D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= (ImageConverterFeature::Convert2DToFile|ImageConverterFeature::Levels),
        "Trade::AbstractImageConverter::convertToFile(): multi-level 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", imageLevels))
        return {};
    #endif

    return doConvertToFile(imageLevels, filename);
}

bool AbstractImageConverter::convertToFile(const std::initializer_list<ImageView2D> imageLevels, const Containers::StringView filename) {
    return convertToFile(Containers::arrayView(imageLevels), filename);
}

bool AbstractImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView2D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert2DToData, "Trade::AbstractImageConverter::convertToFile(): multi-level 2D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(imageLevels);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const Containers::ArrayView<const ImageView3D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= (ImageConverterFeature::Convert3DToFile|ImageConverterFeature::Levels),
        "Trade::AbstractImageConverter::convertToFile(): multi-level 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", imageLevels))
        return {};
    #endif

    return doConvertToFile(imageLevels, filename);
}

bool AbstractImageConverter::convertToFile(const std::initializer_list<ImageView3D> imageLevels, const Containers::StringView filename) {
    return convertToFile(Containers::arrayView(imageLevels), filename);
}

bool AbstractImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView3D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert3DToData, "Trade::AbstractImageConverter::convertToFile(): multi-level 3D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(imageLevels);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const Containers::ArrayView<const CompressedImageView1D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= (ImageConverterFeature::ConvertCompressed1DToFile|ImageConverterFeature::Levels),
        "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 1D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", imageLevels))
        return {};
    #endif

    return doConvertToFile(imageLevels, filename);
}

bool AbstractImageConverter::convertToFile(const std::initializer_list<CompressedImageView1D> imageLevels, const Containers::StringView filename) {
    return convertToFile(Containers::arrayView(imageLevels), filename);
}

bool AbstractImageConverter::doConvertToFile(const Containers::ArrayView<const CompressedImageView1D> imageLevels, Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed1DToData, "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 1D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(imageLevels);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const Containers::ArrayView<const CompressedImageView2D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= (ImageConverterFeature::ConvertCompressed2DToFile|ImageConverterFeature::Levels),
        "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 2D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", imageLevels))
        return {};
    #endif

    return doConvertToFile(imageLevels, filename);
}

bool AbstractImageConverter::convertToFile(const std::initializer_list<CompressedImageView2D> imageLevels, const Containers::StringView filename) {
    return convertToFile(Containers::arrayView(imageLevels), filename);
}

bool AbstractImageConverter::doConvertToFile(const Containers::ArrayView<const CompressedImageView2D> imageLevels, Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed2DToData, "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 2D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(imageLevels);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const Containers::ArrayView<const CompressedImageView3D> imageLevels, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= (ImageConverterFeature::ConvertCompressed3DToFile|ImageConverterFeature::Levels),
        "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 3D image conversion not supported", {});

    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractImageConverter::convertToFile():", imageLevels))
        return {};
    #endif

    return doConvertToFile(imageLevels, filename);
}

bool AbstractImageConverter::convertToFile(const std::initializer_list<CompressedImageView3D> imageLevels, const Containers::StringView filename) {
    return convertToFile(Containers::arrayView(imageLevels), filename);
}

bool AbstractImageConverter::doConvertToFile(const Containers::ArrayView<const CompressedImageView3D> imageLevels, Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed3DToData, "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 3D image conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(imageLevels);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

Debug& operator<<(Debug& debug, const ImageConverterFeature value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* If printing a deprecated flag combination, make it look like the enum
       set */
    if((value & ImageConverterFeature::Levels) && (value & ~ImageConverterFeature::Levels))
        return debug << (value & ~ImageConverterFeature::Levels) << Debug::nospace << (packed ? "|Levels" : "|Trade::ImageConverterFeature::Levels");
    #endif

    if(!packed)
        debug << "Trade::ImageConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImageConverterFeature::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
        _c(Convert1D)
        _c(Convert2D)
        _c(Convert3D)
        _c(ConvertCompressed1D)
        _c(ConvertCompressed2D)
        _c(ConvertCompressed3D)
        _c(Convert1DToFile)
        _c(Convert2DToFile)
        _c(Convert3DToFile)
        _c(ConvertCompressed1DToFile)
        _c(ConvertCompressed2DToFile)
        _c(ConvertCompressed3DToFile)
        _c(Convert1DToData)
        _c(Convert2DToData)
        _c(Convert3DToData)
        _c(ConvertCompressed1DToData)
        _c(ConvertCompressed2DToData)
        _c(ConvertCompressed3DToData)
        _c(Levels)
        #undef _c
        /* LCOV_EXCL_STOP */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /* LCOV_EXCL_START */
        CORRADE_IGNORE_DEPRECATED_PUSH
        case ImageConverterFeature::ConvertLevels1DToData:
        case ImageConverterFeature::ConvertLevels2DToData:
        case ImageConverterFeature::ConvertLevels3DToData:
        case ImageConverterFeature::ConvertCompressedLevels1DToData:
        case ImageConverterFeature::ConvertCompressedLevels2DToData:
        case ImageConverterFeature::ConvertCompressedLevels3DToData:
        case ImageConverterFeature::ConvertLevels1DToFile:
        case ImageConverterFeature::ConvertLevels2DToFile:
        case ImageConverterFeature::ConvertLevels3DToFile:
        case ImageConverterFeature::ConvertCompressedLevels1DToFile:
        case ImageConverterFeature::ConvertCompressedLevels2DToFile:
        case ImageConverterFeature::ConvertCompressedLevels3DToFile:
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        CORRADE_IGNORE_DEPRECATED_POP
        /* LCOV_EXCL_STOP */
        #endif
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const ImageConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Trade::ImageConverterFeatures{}", {
        ImageConverterFeature::Convert1D,
        ImageConverterFeature::Convert2D,
        ImageConverterFeature::Convert3D,
        ImageConverterFeature::ConvertCompressed1D,
        ImageConverterFeature::ConvertCompressed2D,
        ImageConverterFeature::ConvertCompressed3D,
        ImageConverterFeature::Convert1DToData,
        ImageConverterFeature::Convert2DToData,
        ImageConverterFeature::Convert3DToData,
        ImageConverterFeature::ConvertCompressed1DToData,
        ImageConverterFeature::ConvertCompressed2DToData,
        ImageConverterFeature::ConvertCompressed3DToData,
        /* These 6 are implied by Convert[Compressed]ToData, so have to be
           after */
        ImageConverterFeature::Convert1DToFile,
        ImageConverterFeature::Convert2DToFile,
        ImageConverterFeature::Convert3DToFile,
        ImageConverterFeature::ConvertCompressed1DToFile,
        ImageConverterFeature::ConvertCompressed2DToFile,
        ImageConverterFeature::ConvertCompressed3DToFile,
        ImageConverterFeature::Levels});
}

Debug& operator<<(Debug& debug, const ImageConverterFlag value) {
    debug << "Trade::ImageConverterFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImageConverterFlag::v: return debug << "::" #v;
        _c(Quiet)
        _c(Verbose)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImageConverterFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::ImageConverterFlags{}", {
        ImageConverterFlag::Quiet,
        ImageConverterFlag::Verbose});
}

}}
