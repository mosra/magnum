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

#include "AnyImageConverter.h"

#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h> /* for PluginManager */
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h> /* for PluginMetadata::name() */
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/Implementation/propagateConfiguration.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

AnyImageConverter::AnyImageConverter(PluginManager::Manager<AbstractImageConverter>& manager): AbstractImageConverter{manager} {}

AnyImageConverter::AnyImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImageConverter{manager, plugin} {}

AnyImageConverter::~AnyImageConverter() = default;

ImageConverterFeatures AnyImageConverter::doFeatures() const {
    return
        ImageConverterFeature::Convert1DToFile|
        ImageConverterFeature::Convert2DToFile|
        ImageConverterFeature::Convert3DToFile|
        ImageConverterFeature::ConvertCompressed1DToFile|
        ImageConverterFeature::ConvertCompressed2DToFile|
        ImageConverterFeature::ConvertCompressed3DToFile|
        ImageConverterFeature::ConvertLevels1DToFile|
        ImageConverterFeature::ConvertLevels2DToFile|
        ImageConverterFeature::ConvertLevels3DToFile|
        ImageConverterFeature::ConvertCompressedLevels1DToFile|
        ImageConverterFeature::ConvertCompressedLevels2DToFile|
        ImageConverterFeature::ConvertCompressedLevels3DToFile;
}

bool AnyImageConverter::doConvertToFile(const ImageView1D&, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store 1D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a 1D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(const ImageView2D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo once Directory is std::string-free, use splitExtension(), but
        only if we don't detect more than one extension yet */
    const Containers::String normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalized.hasSuffix(".bmp"_s))
        plugin = "BmpImageConverter"_s;
    else if(normalized.hasSuffix(".basis"_s))
        plugin = "BasisImageConverter"_s;
    else if(normalized.hasSuffix(".exr"_s))
        plugin = "OpenExrImageConverter"_s;
    else if(normalized.hasSuffix(".hdr"_s))
        plugin = "HdrImageConverter"_s;
    else if(normalized.hasSuffix(".jpg"_s) ||
            normalized.hasSuffix(".jpeg"_s) ||
            normalized.hasSuffix(".jpe"_s))
        plugin = "JpegImageConverter"_s;
    else if(normalized.hasSuffix(".ktx2"_s))
        plugin = "KtxImageConverter"_s;
    else if(normalized.hasSuffix(".png"_s))
        plugin = "PngImageConverter"_s;
    else if(normalized.hasSuffix(".tga"_s) ||
            normalized.hasSuffix(".vda"_s) ||
            normalized.hasSuffix(".icb"_s) ||
            normalized.hasSuffix( ".vst"_s))
        plugin = "TgaImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a 2D image";
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImageConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageConverter::convertToFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImageConverter> converter = static_cast<PluginManager::Manager<AbstractImageConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration());

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const ImageView3D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo once Directory is std::string-free, use splitExtension(), but
        only if we don't detect more than one extension yet */
    const Containers::String normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalized.hasSuffix(".exr"_s))
        plugin = "OpenExrImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a 3D image";
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImageConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageConverter::convertToFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImageConverter> converter = static_cast<PluginManager::Manager<AbstractImageConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration());

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const CompressedImageView1D&, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store compressed 1D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a compressed 1D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(const CompressedImageView2D&, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store compressed 2D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a compressed 2D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(const CompressedImageView3D&, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store compressed 3D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a compressed 3D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(Containers::ArrayView<const ImageView1D>, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store multi-level 1D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level 1D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView2D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo once Directory is std::string-free, use splitExtension(), but
        only if we don't detect more than one extension yet */
    const Containers::String normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalized.hasSuffix(".exr"_s))
        plugin = "OpenExrImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level 2D image";
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImageConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageConverter::convertToFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImageConverter> converter = static_cast<PluginManager::Manager<AbstractImageConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration());

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView3D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo once Directory is std::string-free, use splitExtension(), but
        only if we don't detect more than one extension yet */
    const Containers::String normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalized.hasSuffix(".exr"_s))
        plugin = "OpenExrImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level 3D image";
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImageConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageConverter::convertToFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImageConverter> converter = static_cast<PluginManager::Manager<AbstractImageConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration());

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(Containers::ArrayView<const CompressedImageView1D>, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store multi-level compressed 1D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level compressed 1D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(Containers::ArrayView<const CompressedImageView2D>, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store multi-level compressed 2D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level compressed 2D image";
    return false;
}

bool AnyImageConverter::doConvertToFile(Containers::ArrayView<const CompressedImageView3D>, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* No file formats to store multi-level compressed 3D data yet */

    Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level compressed 3D image";
    return false;
}

}}

CORRADE_PLUGIN_REGISTER(AnyImageConverter, Magnum::Trade::AnyImageConverter,
    "cz.mosra.magnum.Trade.AbstractImageConverter/0.3.1")
