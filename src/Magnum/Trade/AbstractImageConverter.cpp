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

#include "AbstractImageConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/Trade/ImageData.h"

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Trade/configure.h"
#endif

namespace Magnum { namespace Trade {

std::string AbstractImageConverter::pluginInterface() {
    return "cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1";
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractImageConverter::pluginSearchPaths() {
    return {
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/imageconverters",
        Utility::Directory::join(MAGNUM_PLUGINS_DEBUG_DIR, "imageconverters")
        #else
        "magnum/imageconverters",
        Utility::Directory::join(MAGNUM_PLUGINS_DIR, "imageconverters")
        #endif
    };
}
#endif

AbstractImageConverter::AbstractImageConverter() = default;

AbstractImageConverter::AbstractImageConverter(PluginManager::Manager<AbstractImageConverter>& manager): PluginManager::AbstractManagingPlugin<AbstractImageConverter>{manager} {}

AbstractImageConverter::AbstractImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin): PluginManager::AbstractManagingPlugin<AbstractImageConverter>{manager, plugin} {}

Containers::Optional<Image2D> AbstractImageConverter::exportToImage(const ImageView2D& image) {
    CORRADE_ASSERT(features() & Feature::ConvertImage,
        "Trade::AbstractImageConverter::exportToImage(): feature not supported", {});

    return doExportToImage(image);
}

Containers::Optional<Image2D> AbstractImageConverter::doExportToImage(const ImageView2D&) {
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToImage(): feature advertised but not implemented", {});
}

Containers::Optional<CompressedImage2D> AbstractImageConverter::exportToCompressedImage(const ImageView2D& image) {
    CORRADE_ASSERT(features() & Feature::ConvertCompressedImage,
        "Trade::AbstractImageConverter::exportToCompressedImage(): feature not supported", {});

    return doExportToCompressedImage(image);
}

Containers::Optional<CompressedImage2D> AbstractImageConverter::doExportToCompressedImage(const ImageView2D&) {
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToCompressedImage(): feature advertised but not implemented", {});
}

Containers::Array<char> AbstractImageConverter::exportToData(const ImageView2D& image) {
    CORRADE_ASSERT(features() & Feature::ConvertData,
        "Trade::AbstractImageConverter::exportToData(): feature not supported", nullptr);

    return doExportToData(image);
}

Containers::Array<char> AbstractImageConverter::doExportToData(const ImageView2D&) {
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::exportToData(const CompressedImageView2D& image) {
    CORRADE_ASSERT(features() & Feature::ConvertCompressedData,
        "Trade::AbstractImageConverter::exportToData(): feature not supported", nullptr);

    return doExportToData(image);
}

Containers::Array<char> AbstractImageConverter::doExportToData(const CompressedImageView2D&) {
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented", nullptr);
}

Containers::Array<char> AbstractImageConverter::exportToData(const ImageData2D& image) {
    return image.isCompressed() ? exportToData(CompressedImageView2D(image)) : exportToData(ImageView2D(image));
}

bool AbstractImageConverter::exportToFile(const ImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() & Feature::ConvertFile,
        "Trade::AbstractImageConverter::exportToFile(): feature not supported", {});

    return doExportToFile(image, filename);
}

bool AbstractImageConverter::doExportToFile(const ImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() >= Feature::ConvertData, "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented", false);

    const auto data = doExportToData(image);
    if(!data) return false;

    /* Open file */
    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::exportToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::exportToFile(const CompressedImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() & Feature::ConvertCompressedFile,
        "Trade::AbstractImageConverter::exportToFile(): feature not supported", {});

    return doExportToFile(image, filename);
}

bool AbstractImageConverter::doExportToFile(const CompressedImageView2D& image, const std::string& filename) {
    CORRADE_ASSERT(features() >= Feature::ConvertCompressedData, "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented", false);

    const auto data = doExportToData(image);
    if(!data) return false;

    /* Open file */
    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::exportToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

bool AbstractImageConverter::exportToFile(const ImageData2D& image, const std::string& filename) {
    return image.isCompressed() ? exportToFile(CompressedImageView2D(image), filename) : exportToFile(ImageView2D(image), filename);
}

Debug& operator<<(Debug& debug, const AbstractImageConverter::Feature value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case AbstractImageConverter::Feature::v: return debug << "Trade::AbstractImageConverter::Feature::" #v;
        _c(ConvertImage)
        _c(ConvertCompressedImage)
        _c(ConvertFile)
        _c(ConvertCompressedFile)
        _c(ConvertData)
        _c(ConvertCompressedData)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Trade::AbstractImageConverter::Feature(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const AbstractImageConverter::Features value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::AbstractImageConverter::Features{}", {
        AbstractImageConverter::Feature::ConvertImage,
        AbstractImageConverter::Feature::ConvertCompressedImage,
        AbstractImageConverter::Feature::ConvertData,
        AbstractImageConverter::Feature::ConvertCompressedData,
        /* These are implied by Convert[Compressed]Data, so have to be last */
        AbstractImageConverter::Feature::ConvertFile,
        AbstractImageConverter::Feature::ConvertCompressedFile});
}

}}
