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

#include "AbstractImporter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Audio/configure.h"
#endif

namespace Magnum { namespace Audio {

std::string AbstractImporter::pluginInterface() {
    return "cz.mosra.magnum.Audio.AbstractImporter/0.1";
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractImporter::pluginSearchPaths() {
    return {
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/audioimporters",
        Utility::Directory::join(MAGNUM_PLUGINS_DEBUG_DIR, "audioimporters")
        #else
        "magnum/audioimporters",
        Utility::Directory::join(MAGNUM_PLUGINS_DIR, "audioimporters")
        #endif
    };
}
#endif

AbstractImporter::AbstractImporter() = default;

AbstractImporter::AbstractImporter(PluginManager::Manager<AbstractImporter>& manager): PluginManager::AbstractManagingPlugin<AbstractImporter>{manager} {}

AbstractImporter::AbstractImporter(PluginManager::AbstractManager& manager, const std::string& plugin): PluginManager::AbstractManagingPlugin<AbstractImporter>{manager, plugin} {}

bool AbstractImporter::openData(Containers::ArrayView<const char> data) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Audio::AbstractImporter::openData(): feature not supported", {});

    close();
    doOpenData(data);
    return isOpened();
}

void AbstractImporter::doOpenData(Containers::ArrayView<const char>) {
    CORRADE_ASSERT(false, "Audio::AbstractImporter::openData(): feature advertised but not implemented", );
}

bool AbstractImporter::openFile(const std::string& filename) {
    close();
    doOpenFile(filename);
    return isOpened();
}

void AbstractImporter::doOpenFile(const std::string& filename) {
    CORRADE_ASSERT(features() & Feature::OpenData, "Audio::AbstractImporter::openFile(): not implemented", );

    /* Open file */
    if(!Utility::Directory::exists(filename)) {
        Error() << "Trade::AbstractImporter::openFile(): cannot open file" << filename;
        return;
    }

    doOpenData(Utility::Directory::read(filename));
}

void AbstractImporter::close() {
    if(isOpened()) {
        doClose();
        CORRADE_INTERNAL_ASSERT(!isOpened());
    }
}

BufferFormat AbstractImporter::format() const {
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::format(): no file opened", {});
    return doFormat();
}

UnsignedInt AbstractImporter::frequency() const {
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::frequency(): no file opened", {});
    return doFrequency();
}

Containers::Array<char> AbstractImporter::data() {
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::data(): no file opened", nullptr);
    return doData();
}

}}
