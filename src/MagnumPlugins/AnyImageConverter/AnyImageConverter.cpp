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

#include "AnyImageConverter.h"

#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h> /* lowercase() */

#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/Implementation/propagateConfiguration.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

AnyImageConverter::AnyImageConverter(PluginManager::Manager<AbstractImageConverter>& manager): AbstractImageConverter{manager} {}

AnyImageConverter::AnyImageConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractImageConverter{manager, plugin} {}

AnyImageConverter::~AnyImageConverter() = default;

ImageConverterFeatures AnyImageConverter::doFeatures() const {
    return
        ImageConverterFeature::Convert1DToFile|
        ImageConverterFeature::Convert2DToFile|
        ImageConverterFeature::Convert3DToFile|
        ImageConverterFeature::ConvertCompressed1DToFile|
        ImageConverterFeature::ConvertCompressed2DToFile|
        ImageConverterFeature::ConvertCompressed3DToFile|
        ImageConverterFeature::Levels;
}

bool AnyImageConverter::doConvertToFile(const ImageView1D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a 1D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const ImageView2D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".bmp"_s)
        plugin = "BmpImageConverter"_s;
    else if(normalizedExtension == ".basis"_s)
        plugin = "BasisImageConverter"_s;
    else if(normalizedExtension == ".exr"_s)
        plugin = "OpenExrImageConverter"_s;
    else if(normalizedExtension == ".hdr"_s)
        plugin = "HdrImageConverter"_s;
    else if(normalizedExtension == ".jpg"_s ||
            normalizedExtension == ".jpeg"_s ||
            normalizedExtension == ".jpe"_s)
        plugin = "JpegImageConverter"_s;
    else if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else if(normalizedExtension == ".png"_s)
        plugin = "PngImageConverter"_s;
    else if(normalizedExtension == ".tga"_s ||
            normalizedExtension == ".vda"_s ||
            normalizedExtension == ".icb"_s ||
            normalizedExtension ==  ".vst"_s)
        plugin = "TgaImageConverter"_s;
    else if(normalizedExtension == ".webp"_s)
        plugin = "WebPImageConverter"_s;
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const ImageView3D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".basis"_s)
        plugin = "BasisImageConverter"_s;
    else if(normalizedExtension == ".exr"_s)
        plugin = "OpenExrImageConverter"_s;
    else if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else if(normalizedExtension == ".vdb"_s)
        plugin = "OpenVdbImageConverter"_s;
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const CompressedImageView1D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a compressed 1D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const CompressedImageView2D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a compressed 2D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const CompressedImageView3D& image, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a compressed 3D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(image, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView1D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level 1D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView2D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".basis"_s)
        plugin = "BasisImageConverter"_s;
    else if(normalizedExtension == ".exr"_s)
        plugin = "OpenExrImageConverter"_s;
    else if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const ImageView3D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".basis"_s)
        plugin = "BasisImageConverter"_s;
    else if(normalizedExtension == ".exr"_s)
        plugin = "OpenExrImageConverter"_s;
    else if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const CompressedImageView1D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level compressed 1D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const CompressedImageView2D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level compressed 2D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

bool AnyImageConverter::doConvertToFile(const Containers::ArrayView<const CompressedImageView3D> imageLevels, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".ktx2"_s)
        plugin = "KtxImageConverter"_s;
    else {
        Error{} << "Trade::AnyImageConverter::convertToFile(): cannot determine the format of" << filename << "for a multi-level compressed 3D image";
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
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & ImageConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(imageLevels, filename);
}

}}

CORRADE_PLUGIN_REGISTER(AnyImageConverter, Magnum::Trade::AnyImageConverter,
    MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE)
