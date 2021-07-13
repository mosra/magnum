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

#include "AnySceneConverter.h"

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

AnySceneConverter::AnySceneConverter(PluginManager::Manager<AbstractSceneConverter>& manager): AbstractSceneConverter{manager} {}

AnySceneConverter::AnySceneConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractSceneConverter{manager, plugin} {}

AnySceneConverter::~AnySceneConverter() = default;

SceneConverterFeatures AnySceneConverter::doFeatures() const {
    return SceneConverterFeature::ConvertMeshToFile;
}

bool AnySceneConverter::doConvertToFile(const MeshData& mesh, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo once Directory is std::string-free, use splitExtension(), but
        only if we don't detect more than one extension yet */
    const Containers::StringView normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalized.hasSuffix(".ply"_s))
        plugin = "StanfordSceneConverter"_s;
    else {
        Error{} << "Trade::AnySceneConverter::convertToFile(): cannot determine the format of" << filename;
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnySceneConverter::convertToFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & SceneConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnySceneConverter::convertToFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractSceneConverter> converter = static_cast<PluginManager::Manager<AbstractSceneConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnySceneConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration());

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(mesh, filename);
}

}}

CORRADE_PLUGIN_REGISTER(AnySceneConverter, Magnum::Trade::AnySceneConverter,
    "cz.mosra.magnum.Trade.AbstractSceneConverter/0.1.1")
