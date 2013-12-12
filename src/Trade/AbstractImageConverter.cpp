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

#include "AbstractImageConverter.h"

#include <Containers/Array.h>
#include <Utility/Assert.h>
#include <Utility/Directory.h>

namespace Magnum { namespace Trade {

AbstractImageConverter::AbstractImageConverter() = default;

AbstractImageConverter::AbstractImageConverter(PluginManager::AbstractManager* manager, std::string plugin): AbstractPlugin(manager, std::move(plugin)) {}

Image2D* AbstractImageConverter::exportToImage(const ImageReference2D& image) const {
    CORRADE_ASSERT(features() & Feature::ConvertImage,
        "Trade::AbstractImageConverter::exportToImage(): feature not supported", nullptr);

    return doExportToImage(image);
}

Image2D* AbstractImageConverter::doExportToImage(const ImageReference2D&) const {
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToImage(): feature advertised but not implemented", nullptr);
}

Containers::Array<unsigned char> AbstractImageConverter::exportToData(const ImageReference2D& image) const {
    #ifndef CORRADE_GCC45_COMPATIBILITY
    CORRADE_ASSERT(features() & Feature::ConvertData,
        "Trade::AbstractImageConverter::exportToData(): feature not supported", nullptr);
    #else
    CORRADE_ASSERT(features() & Feature::ConvertData,
        "Trade::AbstractImageConverter::exportToData(): feature not supported", {});
    #endif

    return doExportToData(image);
}

Containers::Array<unsigned char> AbstractImageConverter::doExportToData(const ImageReference2D&) const {
    #ifndef CORRADE_GCC45_COMPATIBILITY
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented", nullptr);
    #else
    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented", {});
    #endif
}

bool AbstractImageConverter::exportToFile(const ImageReference2D& image, const std::string& filename) const {
    return doExportToFile(image, filename);
}

bool AbstractImageConverter::doExportToFile(const ImageReference2D& image, const std::string& filename) const {
    CORRADE_ASSERT(features() & Feature::ConvertData, "Trade::AbstractImageConverter::exportToFile(): not implemented", false);

    const auto data = doExportToData(image);
    if(!data) return false;

    /* Open file */
    if(!Utility::Directory::write(filename, data)) {
        Error() << "Trade::AbstractImageConverter::exportToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

}}
