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

#include "AbstractImageConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
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
"cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1"
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

Containers::Optional<Image2D> AbstractImageConverter::exportToImage(const ImageView2D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertImage,
        "Trade::AbstractImageConverter::exportToImage(): feature not supported", {});

    Containers::Optional<Image2D> out = doExportToImage(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::exportToImage(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<Image2D> AbstractImageConverter::doExportToImage(const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::exportToImage(): feature advertised but not implemented", {});
}

Containers::Optional<CompressedImage2D> AbstractImageConverter::exportToCompressedImage(const ImageView2D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressedImage,
        "Trade::AbstractImageConverter::exportToCompressedImage(): feature not supported", {});

    Containers::Optional<CompressedImage2D> out = doExportToCompressedImage(image);
    CORRADE_ASSERT(!out || !out->_data.deleter(), "Trade::AbstractImageConverter::exportToCompressedImage(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<CompressedImage2D> AbstractImageConverter::doExportToCompressedImage(const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::exportToCompressedImage(): feature advertised but not implemented", {});
}

Containers::Array<char> AbstractImageConverter::exportToData(const ImageView2D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertData,
        "Trade::AbstractImageConverter::exportToData(): feature not supported", nullptr);

    Containers::Array<char> out = doExportToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::exportToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doExportToData(const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::exportToData(const CompressedImageView2D& image) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressedData,
        "Trade::AbstractImageConverter::exportToData(): feature not supported", nullptr);

    Containers::Array<char> out = doExportToData(image);
    CORRADE_ASSERT(!out.deleter(), "Trade::AbstractImageConverter::exportToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractImageConverter::doExportToData(const CompressedImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::exportToData(const ImageData2D& image) {
    return image.isCompressed() ? exportToData(CompressedImageView2D(image)) : exportToData(ImageView2D(image));
}

bool AbstractImageConverter::exportToFile(const ImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertFile,
        "Trade::AbstractImageConverter::exportToFile(): feature not supported", {});

    return doExportToFile(image, filename);
}

bool AbstractImageConverter::doExportToFile(const ImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertData, "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented", false);

    const auto data = doExportToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::exportToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::exportToFile(const CompressedImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() & ImageConverterFeature::ConvertCompressedFile,
        "Trade::AbstractImageConverter::exportToFile(): feature not supported", {});

    return doExportToFile(image, filename);
}

bool AbstractImageConverter::doExportToFile(const CompressedImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() >= ImageConverterFeature::ConvertCompressedData, "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented", false);

    const auto data = doExportToData(image);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::exportToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::exportToFile(const ImageData2D& image, const std::string& filename) {
    return image.isCompressed() ? exportToFile(CompressedImageView2D(image), filename) : exportToFile(ImageView2D(image), filename);
}

Debug& operator<<(Debug& debug, const ImageConverterFeature value) {
    debug << "Trade::ImageConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImageConverterFeature::v: return debug << "::" #v;
        _c(ConvertImage)
        _c(ConvertCompressedImage)
        _c(ConvertFile)
        _c(ConvertCompressedFile)
        _c(ConvertData)
        _c(ConvertCompressedData)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImageConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::ImageConverterFeatures{}", {
        ImageConverterFeature::ConvertImage,
        ImageConverterFeature::ConvertCompressedImage,
        ImageConverterFeature::ConvertData,
        ImageConverterFeature::ConvertCompressedData,
        /* These are implied by Convert[Compressed]Data, so have to be last */
        ImageConverterFeature::ConvertFile,
        ImageConverterFeature::ConvertCompressedFile});
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
