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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/MaterialTools/RemoveDuplicates.h"
#include "Magnum/SceneTools/Map.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/SceneData.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainMaterialTools();
void mainMaterialTools() {
{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Trade::AbstractImporter> manager;
/* [removeDuplicatesInPlace] */
Containers::Pointer<Trade::AbstractImporter> importer = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate("SomethingWhatever"));

/* Import all materials */
Containers::Array<Trade::MaterialData> materials;
for(UnsignedInt i = 0; i != importer->materialCount(); ++i)
    arrayAppend(materials, *importer->material(i));

/* Remove duplicates, putting the unique materials to the prefix and removing
   the rest */
Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> mapping =
    MaterialTools::removeDuplicatesInPlace(materials);
arrayRemoveSuffix(materials, materials.size() - mapping.second());

/* Apply the mapping to unique materials to the scene */
Trade::SceneData scene = DOXYGEN_ELLIPSIS(Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}});
SceneTools::mapIndexFieldInPlace(scene,
    Trade::SceneField::MeshMaterial, mapping.first());
/* [removeDuplicatesInPlace] */
}
}
