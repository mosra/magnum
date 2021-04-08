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

#include "AbstractImageConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h> /* for Directory */
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/Trade/ImageData.h"

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Trade/configure.h"
#endif

namespace Magnum { namespace Trade {

std::string AbstractImageConverter::pluginInterface() {
    return
/* [interface] */
"cz.mosra.magnum.Trade.AbstractImageConverter/0.3"
/* [interface] */
    ;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractImageConverter::pluginSearchPaths() {
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        Utility::Directory::libraryLocation(&pluginInterface),
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
        "imageconverters");
}
#endif

AbstractImageConverter::AbstractImageConverter() = default;

AbstractImageConverter::AbstractImageConverter(PluginManager::Manager<AbstractImageConverter>& manager): PluginManager::AbstractManagingPlugin<AbstractImageConverter>{manager} {}

AbstractImageConverter::AbstractImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin): PluginManager::AbstractManagingPlugin<AbstractImageConverter>{manager, plugin} {}

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

Containers::Optional<ImageData1D> AbstractImageConverter::convert(const ImageView1D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert1D,
        "Trade::AbstractImageConverter::convert(): 1D image conversion not supported", {});

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

Containers::Array<char> AbstractImageConverter::convertToData(const ImageView1D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert1DToData,
        "Trade::AbstractImageConverter::convertToData(): 1D image conversion not supported", nullptr);

    Containers::Array<char> out = doConvertToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doConvertToData(const ImageView1D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): 1D image conversion advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::convertToData(const ImageView2D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert2DToData,
        "Trade::AbstractImageConverter::convertToData(): 2D image conversion not supported", nullptr);

    Containers::Array<char> out = doConvertToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doConvertToData(const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): 2D image conversion advertised but not implemented", nullptr);
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Array<char> AbstractImageConverter::exportToData(const ImageView2D& image) {
    return convertToData(image);
}
#endif

Containers::Array<char> AbstractImageConverter::convertToData(const ImageView3D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert3DToData,
        "Trade::AbstractImageConverter::convertToData(): 3D image conversion not supported", nullptr);

    Containers::Array<char> out = doConvertToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doConvertToData(const ImageView3D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): 3D image conversion advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::convertToData(const CompressedImageView1D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed1DToData,
        "Trade::AbstractImageConverter::convertToData(): compressed 1D image conversion not supported", nullptr);

    Containers::Array<char> out = doConvertToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doConvertToData(const CompressedImageView1D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): compressed 1D image conversion advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::convertToData(const CompressedImageView2D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed2DToData,
        "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion not supported", nullptr);

    Containers::Array<char> out = doConvertToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doConvertToData(const CompressedImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion advertised but not implemented", nullptr);
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Array<char> AbstractImageConverter::exportToData(const CompressedImageView2D& image) {
    return convertToData(image);
}
#endif

Containers::Array<char> AbstractImageConverter::convertToData(const CompressedImageView3D& image) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed3DToData,
        "Trade::AbstractImageConverter::convertToData(): compressed 3D image conversion not supported", nullptr);

    Containers::Array<char> out = doConvertToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doConvertToData(const CompressedImageView3D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::convertToData(): compressed 3D image conversion advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::convertToData(const ImageData1D& image) {
    return image.isCompressed() ? convertToData(CompressedImageView1D(image)) : convertToData(ImageView1D(image));
}

Containers::Array<char> AbstractImageConverter::convertToData(const ImageData2D& image) {
    return image.isCompressed() ? convertToData(CompressedImageView2D(image)) : convertToData(ImageView2D(image));
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Array<char> AbstractImageConverter::exportToData(const ImageData2D& image) {
    return convertToData(image);
}
#endif

Containers::Array<char> AbstractImageConverter::convertToData(const ImageData3D& image) {
    return image.isCompressed() ? convertToData(CompressedImageView3D(image)) : convertToData(ImageView3D(image));
}

bool AbstractImageConverter::convertToFile(const ImageView1D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert1DToFile,
        "Trade::AbstractImageConverter::convertToFile(): 1D image conversion not supported", {});

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const ImageView1D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert1DToData, "Trade::AbstractImageConverter::convertToFile(): 1D image conversion advertised but not implemented", false);

    const auto data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const ImageView2D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::Convert2DToFile,
        "Trade::AbstractImageConverter::convertToFile(): 2D image conversion not supported", {});

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const ImageView2D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert2DToData, "Trade::AbstractImageConverter::convertToFile(): 2D image conversion advertised but not implemented", false);

    const auto data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
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

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const ImageView3D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::Convert3DToData, "Trade::AbstractImageConverter::convertToFile(): 3D image conversion advertised but not implemented", false);

    const auto data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const CompressedImageView1D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed1DToFile,
        "Trade::AbstractImageConverter::convertToFile(): compressed 1D image conversion not supported", {});

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const CompressedImageView1D& image, Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed1DToData, "Trade::AbstractImageConverter::convertToFile(): compressed 1D image conversion advertised but not implemented", false);

    const auto data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::convertToFile(const CompressedImageView2D& image, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressed2DToFile,
        "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion not supported", {});

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const CompressedImageView2D& image, Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed2DToData, "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion advertised but not implemented", false);

    const auto data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
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

    return doConvertToFile(image, filename);
}

bool AbstractImageConverter::doConvertToFile(const CompressedImageView3D& image, Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressed3DToData, "Trade::AbstractImageConverter::convertToFile(): compressed 3D image conversion advertised but not implemented", false);

    const auto data = doConvertToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
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

Debug& operator<<(Debug& debug, const ImageConverterFeature value) {
    debug << "Trade::ImageConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImageConverterFeature::v: return debug << "::" #v;
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
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImageConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::ImageConverterFeatures{}", {
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
        /* These are implied by Convert[Compressed]ToData, so have to be last */
        ImageConverterFeature::Convert1DToFile,
        ImageConverterFeature::Convert2DToFile,
        ImageConverterFeature::Convert3DToFile,
        ImageConverterFeature::ConvertCompressed1DToFile,
        ImageConverterFeature::ConvertCompressed2DToFile,
        ImageConverterFeature::ConvertCompressed3DToFile});
}

Debug& operator<<(Debug& debug, const ImageConverterFlag value) {
    debug << "Trade::ImageConverterFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImageConverterFlag::v: return debug << "::" #v;
        _c(Verbose)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImageConverterFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::ImageConverterFlags{}", {
        ImageConverterFlag::Verbose});
}

}}
