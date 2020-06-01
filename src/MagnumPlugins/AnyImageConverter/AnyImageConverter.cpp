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

#include "AnyImageConverter.h"

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade {

AnyImageConverter::AnyImageConverter(PluginManager::Manager<AbstractImageConverter>& manager): AbstractImageConverter{manager} {}

AnyImageConverter::AnyImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImageConverter{manager, plugin} {}

AnyImageConverter::~AnyImageConverter() = default;

ImageConverterFeatures AnyImageConverter::doFeatures() const {
    return ImageConverterFeature::ConvertFile|ImageConverterFeature::ConvertCompressedFile;
}

bool AnyImageConverter::doExportToFile(const ImageView2D& image, const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo lowercase only the extension, once Directory::split() is done */
    const std::string normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    std::string plugin;
    if(Utility::String::endsWith(normalized, ".bmp"))
        plugin = "BmpImageConverter";
    else if(Utility::String::endsWith(normalized, ".basis"))
        plugin = "BasisImageConverter";
    else if(Utility::String::endsWith(normalized, ".exr"))
        plugin = "OpenExrImageConverter";
    else if(Utility::String::endsWith(normalized, ".hdr"))
        plugin = "HdrImageConverter";
    else if(Utility::String::endsWith(normalized, ".jpg") ||
            Utility::String::endsWith(normalized, ".jpeg") ||
            Utility::String::endsWith(normalized, ".jpe"))
        plugin = "JpegImageConverter";
    else if(Utility::String::endsWith(normalized, ".png"))
        plugin = "PngImageConverter";
    else if(Utility::String::endsWith(normalized, ".tga") ||
            Utility::String::endsWith(normalized, ".vda") ||
            Utility::String::endsWith(normalized, ".icb") ||
            Utility::String::endsWith(normalized, ".vst"))
        plugin = "TgaImageConverter";
    else {
        Error{} << "Trade::AnyImageConverter::exportToFile(): cannot determine the format of" << filename;
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageConverter::exportToFile(): cannot load the" << plugin << "plugin";
        return false;
    }
    if(flags() & ImageConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageConverter::exportToFile(): using" << plugin;
        PluginManager::PluginMetadata* metadata = manager()->metadata(plugin);
        CORRADE_INTERNAL_ASSERT(metadata);
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImageConverter> converter = static_cast<PluginManager::Manager<AbstractImageConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->exportToFile(image, filename);
}

bool AnyImageConverter::doExportToFile(const CompressedImageView2D&, const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store compressed data yet */

    Error{} << "Trade::AnyImageConverter::exportToFile(): cannot determine the format of" << filename << "to store compressed data";
    return false;
}

}}

CORRADE_PLUGIN_REGISTER(AnyImageConverter, Magnum::Trade::AnyImageConverter,
    "cz.mosra.magnum.Trade.AbstractImageConverter/0.2.1")
