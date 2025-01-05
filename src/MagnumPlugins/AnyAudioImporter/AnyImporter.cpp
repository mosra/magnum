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

#include "AnyImporter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h> /* lowercase() */

#include "MagnumPlugins/Implementation/propagateConfiguration.h"

namespace Magnum { namespace Audio {

using namespace Containers::Literals;

AnyImporter::AnyImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnyImporter::AnyImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

AnyImporter::~AnyImporter() = default;

ImporterFeatures AnyImporter::doFeatures() const { return {}; }

bool AnyImporter::doIsOpened() const { return !!_in; }

void AnyImporter::doClose() { _in = nullptr; }

void AnyImporter::doOpenFile(const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".aac"_s)
        plugin = "AacAudioImporter"_s;
    else if(normalizedExtension == ".mp3"_s)
        plugin = "Mp3AudioImporter"_s;
    else if(normalizedExtension == ".ogg"_s)
        plugin = "VorbisAudioImporter"_s;
    else if(normalizedExtension == ".wav"_s)
        plugin = "WavAudioImporter"_s;
    else if(normalizedExtension == ".flac"_s)
        plugin = "FlacAudioImporter"_s;
    else {
        Error{} << "Audio::AnyImporter::openFile(): cannot determine the format of" << filename;
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Audio::AnyImporter::openFile(): cannot load the" << plugin << "plugin";
        return;
    }

    /* Instantiate the plugin */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);

    /* Propagate configuration */
    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    Magnum::Implementation::propagateConfiguration("Audio::AnyImporter::openFile():", {}, metadata->name(), configuration(), importer->configuration());

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = Utility::move(importer);
}

BufferFormat AnyImporter::doFormat() const { return _in->format(); }

UnsignedInt AnyImporter::doFrequency() const { return _in->frequency(); }

Containers::Array<char> AnyImporter::doData() { return _in->data(); }

}}

CORRADE_PLUGIN_REGISTER(AnyAudioImporter, Magnum::Audio::AnyImporter,
    MAGNUM_AUDIO_ABSTRACTIMPORTER_PLUGIN_INTERFACE)
