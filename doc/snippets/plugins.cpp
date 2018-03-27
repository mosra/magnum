#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/Text/AbstractFont.h"
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

int main() {

/* [loading] */
{
    PluginManager::Manager<Trade::AbstractImporter> manager;
    std::unique_ptr<Trade::AbstractImporter> importer =
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
std::unique_ptr<Trade::AbstractImporter> importer =
    manager.instantiate("AnyImageImporter");
importer->openFile("texture.dds"); /* Delegates to the DdsImporter plugin */
/* [anyimporter] */
}

{
PluginManager::Manager<Trade::AbstractImporter> manager;
/* [configuration] */
std::unique_ptr<Trade::AbstractImporter> importer =
    manager.instantiate("AssimpImporter");
importer->configuration().group("postprocess")->setValue("PreTransformVertices", true);
/* [configuration] */
}

}
