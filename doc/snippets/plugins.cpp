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

#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"

#ifdef __has_include
#if __has_include(<MagnumPlugins/TinyGltfImporter/importStaticPlugin.cpp>) && \
    __has_include(<MagnumPlugins/StbTrueTypeFont/importStaticPlugin.cpp>)
/* [static-import] */
/* No need to do this if you use CMake */
#include <MagnumPlugins/TinyGltfImporter/importStaticPlugin.cpp>
#include <MagnumPlugins/StbTrueTypeFont/importStaticPlugin.cpp>
/* [static-import] */
#endif
#endif

using namespace Magnum;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainPlugins();
void mainPlugins() {
/* [loading] */
{
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
        manager.loadAndInstantiate("TgaImporter");
    if(!importer) Fatal{} << "Cannot load the TgaImporter plugin";

    // Use the plugin...

    /* At the end of the scope the importer instance gets deleted and then
       the manager automatically unloads the plugin on destruction */
}
/* [loading] */

{
/* [dependencies] */
PluginManager::Manager<Trade::AbstractImporter> importerManager;
PluginManager::Manager<Text::AbstractFont> fontManager;
fontManager.registerExternalManager(importerManager);

// As a side effect TgaImporter is loaded by importerManager
fontManager.load("MagnumFont");
/* [dependencies] */
}

{
/* [aliases] */
PluginManager::Manager<Text::AbstractFont> manager;
manager.setPreferredPlugins("TrueTypeFont", {"HarfBuzzFont", "FreeTypeFont"});
/* [aliases] */
}

{
PluginManager::Manager<Trade::AbstractImporter> manager;
/* [anyimporter] */
Containers::Pointer<Trade::AbstractImporter> importer =
    manager.instantiate("AnyImageImporter");

/* Delegates to the DdsImporter plugin, if it's available */
importer->openFile("texture.dds");
/* [anyimporter] */
}

{
PluginManager::Manager<Trade::AbstractImporter> manager;
/* [configuration] */
Containers::Pointer<Trade::AbstractImporter> importer =
    manager.instantiate("AssimpImporter");
importer->configuration().setValue("mergeAnimationClips", true);
importer->configuration().group("postprocess")->setValue("PreTransformVertices", true);
/* [configuration] */
}

{
/* [MagnumFont-importer-register] */
PluginManager::Manager<Trade::AbstractImporter> importerManager;
PluginManager::Manager<Text::AbstractFont> fontManager;
fontManager.registerExternalManager(importerManager);
/* [MagnumFont-importer-register] */
}

{
/* [MagnumFontConverter-imageconverter-register] */
PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager;
PluginManager::Manager<Text::AbstractFontConverter> fontConverterManager;
fontConverterManager.registerExternalManager(imageConverterManager);
/* [MagnumFontConverter-imageconverter-register] */
}

}
