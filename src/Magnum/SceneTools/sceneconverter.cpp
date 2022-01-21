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

#include <algorithm>
#include <set>
#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Implementation/converterUtilities.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/SceneTools/FlattenMeshHierarchy.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/Implementation/converterUtilities.h"

namespace Magnum {

/** @page magnum-sceneconverter Scene conversion utility
@brief Converts scenes of different formats
@m_since{2020,06}

@m_footernavigation
@m_keywords{magnum-sceneconverter sceneconverter}

This utility is built if `WITH_SCENECONVERTER` is enabled when building Magnum.
To use this utility with CMake, you need to request the `sceneconverter`
component of the `Magnum` package and use the `Magnum::sceneconverter` target
for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED imageconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::sceneconverter ...)
@endcode

See @ref building and @ref cmake and the @ref Trade namespace for more
information.

@section magnum-sceneconverter-usage Usage

@code{.sh}
magnum-sceneconverter [-h|--help] [-I|--importer IMPORTER]
    [-I|--converter CONVERTER]... [--plugin-dir DIR] [--map]
    [--remove-duplicates] [--remove-duplicates-fuzzy EPSILON]
    [-i|--importer-options key=val,key2=val2,…]
    [-c|--converter-options key=val,key2=val2,…]... [--mesh MESH]
    [--level LEVEL] [--concatenate-meshes] [--info-animations] [--info-images]
    [--info-lights] [--info-materials] [--info-meshes] [--info-skins]
    [--info-textures] [--info] [--bounds] [-v|--verbose] [--profile]
    [--] input output
@endcode

Arguments:

-   `input` --- input file
-   `output` --- output file; ignored if `--info` is present
-   `-h`, `--help` --- display this help message and exit
-   `-I`, `--importer IMPORTER` --- scene importer plugin (default:
    @ref Trade::AnySceneImporter "AnySceneImporter")
-   `-C`, `--converter CONVERTER` --- scene converter plugin(s)
-   `--plugin-dir DIR` --- override base plugin dir
-   `--map` --- memory-map the input for zero-copy import (works only for
    standalone files)
-   `--only-attributes "i j …"` --- include only attributes of given IDs in the
    output
-   `--remove-duplicates` --- remove duplicate vertices using
    @ref MeshTools::removeDuplicates(const Trade::MeshData&) after import
-   `--remove-duplicates-fuzzy EPSILON` --- remove duplicate vertices using
    @ref MeshTools::removeDuplicatesFuzzy(const Trade::MeshData&, Float, Double)
    after import
-   `-i`, `--importer-options key=val,key2=val2,…` --- configuration options to
    pass to the importer
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to the converter(s)
-   `--mesh MESH` --- mesh to import (default: `0`), ignored if
    `--concatenate-meshes` is specified
-   `--level LEVEL` --- mesh level to import (default: `0`), ignored if
    `--concatenate-meshes` is specified
-   `--concatenate-meshes` -- flatten mesh hierarchy and concatenate them all
    together @m_class{m-label m-warning} **experimental**
-   `--info-animations` --- print into about animations in the input file and
    exit
-   `--info-images` --- print into about images in the input file and exit
-   `--info-lights` --- print into about lights in the input file and exit
-   `--info-materials` --- print into about materials in the input file and
    exit
-   `--info-meshes` --- print into about meshes in the input file and exit
-   `--info-skins` --- print into about skins in the input file and exit
-   `--info-textures` --- print into about textures in the input file and exit
-   `--info` --- print info about everything in the input file and exit, same
    as specifying all other `--info-*` options together
-   `--bounds` --- show bounds of known attributes in `--info` output
-   `-v`, `--verbose` --- verbose output from importer and converter plugins
-   `--profile` --- measure import and conversion time

If any of the `--info-*` options are given, the utility will print information
about given data present in the file. In this case no conversion is done and
output file doesn't need to be specified. In case one data references another
and both `--info-*` options are specified, the output will also list reference
count (for example, `--info-scenes` together with `--info-meshes` will print
how many objects reference given mesh).

The `-i` / `--importer-options` and `-c` / `--converter-options` arguments
accept a comma-separated list of key/value pairs to set in the importer /
converter plugin configuration. If the `=` character is omitted, it's
equivalent to saying `key=true`; configuration subgroups are delimited with
`/`.

It's possible to specify the `-C` / `--converter` option (and correspondingly
also `-c` / `--converter-options`) multiple times in order to chain more
converters together. All converters in the chain have to support the
@ref Trade::SceneConverterFeature::ConvertMesh feature,
the last converter either @ref Trade::SceneConverterFeature::ConvertMesh or
@ref Trade::SceneConverterFeature::ConvertMeshToFile. If the last converter
doesn't support conversion to a file,
@ref Trade::AnySceneConverter "AnySceneConverter" is used to save its output;
if no `-C` / `--converter` is specified,
@ref Trade::AnySceneConverter "AnySceneConverter" is used.

@section magnum-sceneconverter-example Example usage

Printing info about all meshes in a glTF file:

@code{.sh}
magnum-sceneconverter --info scene.gltf
@endcode

Converting an OBJ file to a PLY, using @ref Trade::StanfordSceneConverter "StanfordSceneConverter"
picked by @ref Trade::AnySceneConverter "AnySceneConverter":

@code{.sh}
magnum-sceneconverter chair.obj chair.ply
@endcode

Processing an OBJ file with @ref Trade::MeshOptimizerSceneConverter "MeshOptimizerSceneConverter",
setting @ref Trade-MeshOptimizerSceneConverter-configuration "plugin-specific configuration options"
to reduce the index count to half, saving as a PLY, with verbose output showing
the processing stats:

@m_class{m-console-wrap}

@code{.sh}
magnum-sceneconverter chair.obj --converter MeshOptimizerSceneConverter -c simplify=true,simplifyTargetIndexCountThreshold=0.5 chair.ply -v
@endcode

@see @ref magnum-imageconverter
*/

}

using namespace Magnum;

namespace {

/** @todo const Array& doesn't work, minmax() would fail to match */
template<class T> std::string calculateBounds(Containers::Array<T>&& attribute) {
    /** @todo clean up when Debug::toString() exists */
    std::ostringstream out;
    Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << Math::minmax(attribute);
    return out.str();
}

/* Named attribute index from a global index */
/** @todo some helper for this directly on the MeshData class? */
UnsignedInt namedAttributeId(const Trade::MeshData& mesh, UnsignedInt id) {
    const Trade::MeshAttribute name = mesh.attributeName(id);
    for(UnsignedInt i = 0; i != mesh.attributeCount(name); ++i)
        if(mesh.attributeId(name, i) == id) return i;
    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}

bool isInfoRequested(const Utility::Arguments& args) {
    return args.isSet("info-animations") ||
           args.isSet("info-images") ||
           args.isSet("info-lights") ||
           args.isSet("info-materials") ||
           args.isSet("info-meshes") ||
           args.isSet("info-scenes") ||
           args.isSet("info-skins") ||
           args.isSet("info-textures") ||
           args.isSet("info");
}

}

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input file")
        .addArgument("output").setHelp("output", "output file; ignored if --info is present")
        .addOption('I', "importer", "AnySceneImporter").setHelp("importer", "scene importer plugin")
        .addArrayOption('C', "converter").setHelp("converter", "scene converter plugin(s)")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
        .addBooleanOption("map").setHelp("map", "memory-map the input for zero-copy import (works only for standalone files)")
        #endif
        .addOption("only-attributes").setHelp("only-attributes", "include only attributes of given IDs in the output", "\"i j …\"")
        .addBooleanOption("remove-duplicates").setHelp("remove-duplicates", "remove duplicate vertices in the mesh after import")
        .addOption("remove-duplicates-fuzzy").setHelp("remove-duplicates-fuzzy", "remove duplicate vertices with fuzzy comparison in the mesh after import", "EPSILON")
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addArrayOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter(s)", "key=val,key2=val2,…")
        .addOption("mesh", "0").setHelp("mesh", "mesh to import, ignored if --concatenate-meshes is specified")
        .addOption("level", "0").setHelp("level", "mesh level to import, ignored if --concatenate-meshes is specified")
        .addBooleanOption("concatenate-meshes").setHelp("concatenate-meshes", "flatten mesh hierarchy and concatenate them all together")
        .addBooleanOption("info-animations").setHelp("info-animations", "print info about animations in the input file and exit")
        .addBooleanOption("info-images").setHelp("info-images", "print info about images in the input file and exit")
        .addBooleanOption("info-lights").setHelp("info-lights", "print info about images in the input file and exit")
        .addBooleanOption("info-materials").setHelp("info-materials", "print info about materials in the input file and exit")
        .addBooleanOption("info-meshes").setHelp("info-meshes", "print info about meshes in the input file and exit")
        .addBooleanOption("info-scenes").setHelp("info-scenes", "print info about textures in the input file and exit")
        .addBooleanOption("info-skins").setHelp("info-skins", "print info about skins in the input file and exit")
        .addBooleanOption("info-textures").setHelp("info-textures", "print info about textures in the input file and exit")
        .addBooleanOption("info").setHelp("info", "print info about everything in the input file and exit, same as specifying all other --info-* options together")
        .addBooleanOption("bounds").setHelp("bounds", "show bounds of known attributes in --info output")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from importer and converter plugins")
        .addBooleanOption("profile").setHelp("profile", "measure import and conversion time")
        .setParseErrorCallback([](const Utility::Arguments& args, Utility::Arguments::ParseError error, const std::string& key) {
            /* If --info is passed, we don't need the output argument */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
                key == "output" && isInfoRequested(args)) return true;

            /* Handle all other errors as usual */
            return false;
        })
        .setGlobalHelp(R"(Converts scenes of different formats.

If any of the --info-* options are given, the utility will print information
about given data present in the file. In this case no conversion is done and
output file doesn't need to be specified. In case one data references another
and both --info-* options are specified, the output will also list reference
count (for example, --info-scenes together with --info-meshes will print how
many objects reference given mesh).

The -i / --importer-options and -c / --converter-options arguments accept a
comma-separated list of key/value pairs to set in the importer / converter
plugin configuration. If the = character is omitted, it's equivalent to saying
key=true; configuration subgroups are delimited with /.

It's possible to specify the -C / --converter option (and correspondingly also
-c / --converter-options) multiple times in order to chain more converters
together. All converters in the chain have to support the ConvertMesh feature,
the last converter either ConvertMesh or ConvertMeshToFile. If the last
converter doesn't support conversion to a file, AnySceneConverter is used to
save its output; if no -C / --converter is specified, AnySceneConverter is
used.)")
        .parse(argc, argv);

    /* Generic checks */
    if(!args.value<Containers::StringView>("output").isEmpty()) {
        /* Not an error in this case, it should be possible to just append
           --info* to existing command line without having to remove anything.
           But print a warning at least, it could also be a mistyped option. */
        if(isInfoRequested(args))
            Warning{} << "Ignoring output file for --info:" << args.value<Containers::StringView>("output");
    }

    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImporter::pluginSearchPaths()[0])};

    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
    if(!importer) {
        Debug{} << "Available importer plugins:" << Utility::String::join(importerManager.aliasList(), ", ");
        return 1;
    }

    /* Set options, if passed */
    if(args.isSet("verbose")) importer->addFlags(Trade::ImporterFlag::Verbose);
    Implementation::setOptions(*importer, "AnySceneImporter", args.value("importer-options"));

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration importTime{};

    /* Open the file or map it if requested */
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
    Containers::Array<const char, Utility::Directory::MapDeleter> mapped;
    if(args.isSet("map")) {
        Trade::Implementation::Duration d{importTime};
        if(!(mapped = Utility::Directory::mapRead(args.value("input"))) || !importer->openMemory(mapped)) {
            Error() << "Cannot memory-map file" << args.value("input");
            return 3;
        }
    } else
    #endif
    {
        Trade::Implementation::Duration d{importTime};
        if(!importer->openFile(args.value("input"))) {
            Error() << "Cannot open file" << args.value("input");
            return 3;
        }
    }

    /* Print file info, if requested */
    if(isInfoRequested(args)) {
        struct AnimationInfo {
            UnsignedInt animation;
            Trade::AnimationData data{{}, {}};
            std::string name;
        };

        struct SkinInfo {
            UnsignedInt skin;
            Trade::SkinData3D data{{}, {}};
            std::string name;
        };

        struct LightInfo {
            UnsignedInt light;
            Trade::LightData data{{}, {}, {}};
            std::string name;
        };

        struct MaterialInfo {
            UnsignedInt material;
            Trade::MaterialData data{{}, {}};
            std::string name;
        };

        struct TextureInfo {
            UnsignedInt texture;
            Trade::TextureData data{{}, {}, {}, {}, {}, {}};
            std::string name;
        };

        struct MeshAttributeInfo {
            std::size_t offset;
            Int stride;
            UnsignedInt arraySize;
            Trade::MeshAttribute name;
            std::string customName;
            VertexFormat format;
            std::string bounds;
        };

        struct MeshInfo {
            UnsignedInt mesh, level;
            MeshPrimitive primitive;
            UnsignedInt indexCount, vertexCount;
            std::size_t indexOffset;
            Int indexStride;
            std::string indexBounds;
            MeshIndexType indexType;
            Containers::Array<MeshAttributeInfo> attributes;
            std::size_t indexDataSize, vertexDataSize;
            std::string name;
        };

        struct SceneFieldInfo {
            Trade::SceneField name;
            std::string customName;
            Trade::SceneFieldFlags flags;
            Trade::SceneFieldType type;
            UnsignedInt arraySize;
            std::size_t size;
        };

        struct SceneInfo {
            UnsignedInt scene;
            Trade::SceneMappingType mappingType;
            UnsignedLong mappingBound;
            Containers::Array<SceneFieldInfo> fields;
            std::size_t dataSize;
            std::string name;
            /** @todo object names? */
        };

        /* Parse everything first to avoid errors interleaved with output */

        /* Scene properties, together with counting how much is each mesh /
           light / material / skin shared (which gets used only if both
           --info-scenes and --info-{lights,materials,skins} is passed and
           the file has at least one scene). Texture reference count is
           calculated when parsing materials. */
        Containers::Array<SceneInfo> sceneInfos;
        Containers::Array<UnsignedInt> materialReferenceCount;
        Containers::Array<UnsignedInt> lightReferenceCount;
        Containers::Array<UnsignedInt> meshReferenceCount;
        Containers::Array<UnsignedInt> skinReferenceCount;
        if((args.isSet("info") || args.isSet("info-scenes")) && importer->sceneCount()) {
            materialReferenceCount = Containers::Array<UnsignedInt>{importer->materialCount()};
            lightReferenceCount = Containers::Array<UnsignedInt>{importer->lightCount()};
            meshReferenceCount = Containers::Array<UnsignedInt>{importer->meshCount()};
            skinReferenceCount = Containers::Array<UnsignedInt>{importer->skin3DCount()};

            for(UnsignedInt i = 0; i != importer->sceneCount(); ++i) {
                Containers::Optional<Trade::SceneData> scene = importer->scene(i);
                if(!scene) continue;

                SceneInfo info{};
                info.scene = i;
                info.mappingType = scene->mappingType();
                info.mappingBound = scene->mappingBound();
                info.dataSize = scene->data().size();
                info.name = importer->sceneName(i);
                for(UnsignedInt j = 0; j != scene->fieldCount(); ++j) {
                    const Trade::SceneField name = scene->fieldName(j);

                    if(name == Trade::SceneField::Mesh) for(const Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>& meshMaterial: scene->meshesMaterialsAsArray()) {
                        if(meshMaterial.first() < meshReferenceCount.size())
                            ++meshReferenceCount[meshMaterial.first()];
                        if(UnsignedInt(meshMaterial.second().second()) < materialReferenceCount.size())
                            ++materialReferenceCount[meshMaterial.second().second()];
                    }

                    if(name == Trade::SceneField::Skin) for(const Containers::Pair<UnsignedInt, UnsignedInt> skin: scene->skinsAsArray()) {
                        if(skin.second() < skinReferenceCount.size())
                            ++skinReferenceCount[skin.second()];
                        /** @todo 2D/3D distinction */
                    }

                    if(name == Trade::SceneField::Light) for(const Containers::Pair<UnsignedInt, UnsignedInt>& light: scene->lightsAsArray()) {
                        if(light.second() < lightReferenceCount.size())
                            ++lightReferenceCount[light.second()];
                    }

                    arrayAppend(info.fields, InPlaceInit,
                        name,
                        Trade::isSceneFieldCustom(name) ?
                            importer->sceneFieldName(name) : "",
                        scene->fieldFlags(j),
                        scene->fieldType(j),
                        scene->fieldArraySize(j),
                        scene->fieldSize(j));
                }

                arrayAppend(sceneInfos, std::move(info));
            }
        }

        /* Animation properties */
        bool error = false;
        Containers::Array<AnimationInfo> animationInfos;
        if(args.isSet("info") || args.isSet("info-animations")) for(UnsignedInt i = 0; i != importer->animationCount(); ++i) {
            Containers::Optional<Trade::AnimationData> animation;
            {
                Trade::Implementation::Duration d{importTime};
                if(!(animation = importer->animation(i))) {
                    error = true;
                    continue;
                }
            }

            AnimationInfo info{};
            info.animation = i;
            info.name = importer->animationName(i);
            info.data = *std::move(animation);

            arrayAppend(animationInfos, std::move(info));
        }

        /* Skin properties */
        Containers::Array<SkinInfo> skinInfos;
        if(args.isSet("info") || args.isSet("info-skins")) for(UnsignedInt i = 0; i != importer->skin3DCount(); ++i) {
            Containers::Optional<Trade::SkinData3D> skin;
            {
                Trade::Implementation::Duration d{importTime};
                if(!(skin = importer->skin3D(i))) {
                    error = true;
                    continue;
                }
            }

            SkinInfo info{};
            info.skin = i;
            info.name = importer->skin3DName(i);
            info.data = *std::move(skin);

            arrayAppend(skinInfos, std::move(info));
        }

        /* Light properties */
        Containers::Array<LightInfo> lightInfos;
        if(args.isSet("info") || args.isSet("info-lights")) for(UnsignedInt i = 0; i != importer->lightCount(); ++i) {
            Containers::Optional<Trade::LightData> light;
            {
                Trade::Implementation::Duration d{importTime};
                if(!(light = importer->light(i))) {
                    error = true;
                    continue;
                }
            }

            LightInfo info{};
            info.light = i;
            info.name = importer->lightName(i);
            info.data = *std::move(light);

            arrayAppend(lightInfos, std::move(info));
        }

        /* Material properties, together with how much is each texture shared
           (which gets used only if both --info-materials and --info-textures
           is passed and the file has at least one material). */
        Containers::Array<MaterialInfo> materialInfos;
        Containers::Array<UnsignedInt> textureReferenceCount;
        if((args.isSet("info") || args.isSet("info-materials")) && importer->materialCount()) {
            textureReferenceCount = Containers::Array<UnsignedInt>{importer->textureCount()};

            for(UnsignedInt i = 0; i != importer->materialCount(); ++i) {
                Containers::Optional<Trade::MaterialData> material;
                {
                    Trade::Implementation::Duration d{importTime};
                    if(!(material = importer->material(i))) {
                        error = true;
                        continue;
                    }
                }

                /* Calculate texture reference count for all properties that
                   look like a texture */
                for(UnsignedInt j = 0; j != material->layerCount(); ++j) {
                    for(UnsignedInt k = 0; k != material->attributeCount(j); ++k) {
                        if(material->attributeType(j, k) != Trade::MaterialAttributeType::UnsignedInt || !Utility::String::endsWith(material->attributeName(j, k), "Texture"))
                            continue;

                        const UnsignedInt texture = material->attribute<UnsignedInt>(j, k);
                        /** @todo once StridedBitArrayView2D exists, fix this
                            to count each material only once by having one bit
                            for every material and texture */
                        if(texture < textureReferenceCount.size())
                            ++textureReferenceCount[texture];
                    }
                }

                MaterialInfo info{};
                info.material = i;
                info.name = importer->materialName(i);
                info.data = *std::move(material);

                arrayAppend(materialInfos, std::move(info));
            }
        }

        /* Mesh properties */
        Containers::Array<MeshInfo> meshInfos;
        if(args.isSet("info") || args.isSet("info-meshes")) for(UnsignedInt i = 0; i != importer->meshCount(); ++i) {
            for(UnsignedInt j = 0; j != importer->meshLevelCount(i); ++j) {
                Containers::Optional<Trade::MeshData> mesh;
                {
                    Trade::Implementation::Duration d{importTime};
                    if(!(mesh = importer->mesh(i, j))) {
                        error = true;
                        continue;
                    }
                }

                MeshInfo info{};
                info.mesh = i;
                info.level = j;
                info.primitive = mesh->primitive();
                info.vertexCount = mesh->vertexCount();
                info.vertexDataSize = mesh->vertexData().size();
                if(!j) {
                    info.name = importer->meshName(i);
                }
                if(mesh->isIndexed()) {
                    info.indexCount = mesh->indexCount();
                    info.indexType = mesh->indexType();
                    info.indexOffset = mesh->indexOffset();
                    info.indexStride = mesh->indexStride();
                    info.indexDataSize = mesh->indexData().size();
                    if(args.isSet("bounds"))
                        info.indexBounds = calculateBounds(mesh->indicesAsArray());
                }
                for(UnsignedInt k = 0; k != mesh->attributeCount(); ++k) {
                    const Trade::MeshAttribute name = mesh->attributeName(k);

                    /* Calculate bounds, if requested and this is not an
                       implementation-specific format */
                    std::string bounds;
                    if(args.isSet("bounds") && !isVertexFormatImplementationSpecific(mesh->attributeFormat(k))) switch(name) {
                        case Trade::MeshAttribute::Position:
                            bounds = calculateBounds(mesh->positions3DAsArray(namedAttributeId(*mesh, k)));
                            break;
                        case Trade::MeshAttribute::Tangent:
                            bounds = calculateBounds(mesh->tangentsAsArray(namedAttributeId(*mesh, k)));
                            break;
                        case Trade::MeshAttribute::Bitangent:
                            bounds = calculateBounds(mesh->bitangentsAsArray(namedAttributeId(*mesh, k)));
                            break;
                        case Trade::MeshAttribute::Normal:
                            bounds = calculateBounds(mesh->normalsAsArray(namedAttributeId(*mesh, k)));
                            break;
                        case Trade::MeshAttribute::TextureCoordinates:
                            bounds = calculateBounds(mesh->textureCoordinates2DAsArray(namedAttributeId(*mesh, k)));
                            break;
                        case Trade::MeshAttribute::Color:
                            bounds = calculateBounds(mesh->colorsAsArray(namedAttributeId(*mesh, k)));
                            break;
                        case Trade::MeshAttribute::ObjectId:
                            Debug{} << mesh->objectIdsAsArray(namedAttributeId(*mesh, k));
                            bounds = calculateBounds(mesh->objectIdsAsArray(namedAttributeId(*mesh, k)));
                            break;

                        /* And also all other custom attribs. Not saying
                           default: here so we get notified when we forget to
                           handle newly added attribute names */
                        case Trade::MeshAttribute::Custom:
                            break;
                    }

                    arrayAppend(info.attributes, InPlaceInit,
                        mesh->attributeOffset(k),
                        mesh->attributeStride(k),
                        mesh->attributeArraySize(k),
                        name, Trade::isMeshAttributeCustom(name) ?
                            importer->meshAttributeName(name) : "",
                        mesh->attributeFormat(k),
                        bounds);
                }

                std::sort(info.attributes.begin(), info.attributes.end(),
                    [](const MeshAttributeInfo& a, const MeshAttributeInfo& b) {
                        return a.offset < b.offset;
                    });

                arrayAppend(meshInfos, std::move(info));
            }
        }

        /* Texture properties, together with how much is each image shared
           (which gets used only if both --info-textures and --info-images is
           passed and the file has at least one texture). */
        Containers::Array<TextureInfo> textureInfos;
        Containers::Array<UnsignedInt> image1DReferenceCount;
        Containers::Array<UnsignedInt> image2DReferenceCount;
        Containers::Array<UnsignedInt> image3DReferenceCount;
        if((args.isSet("info") || args.isSet("info-textures")) && importer->textureCount()) {
            image1DReferenceCount = Containers::Array<UnsignedInt>{importer->image1DCount()};
            image2DReferenceCount = Containers::Array<UnsignedInt>{importer->image2DCount()};
            image3DReferenceCount = Containers::Array<UnsignedInt>{importer->image3DCount()};
            for(UnsignedInt i = 0; i != importer->textureCount(); ++i) {
                Containers::Optional<Trade::TextureData> texture;
                {
                    Trade::Implementation::Duration d{importTime};
                    if(!(texture = importer->texture(i))) {
                        error = true;
                        continue;
                    }
                }

                switch(texture->type()) {
                    case Trade::TextureType::Texture1D:
                        if(texture->image() < image1DReferenceCount.size())
                            ++image1DReferenceCount[texture->image()];
                        break;
                    case Trade::TextureType::Texture1DArray:
                    case Trade::TextureType::Texture2D:
                        if(texture->image() < image2DReferenceCount.size())
                            ++image2DReferenceCount[texture->image()];
                        break;
                    case Trade::TextureType::CubeMap:
                    case Trade::TextureType::CubeMapArray:
                    case Trade::TextureType::Texture2DArray:
                    case Trade::TextureType::Texture3D:
                        if(texture->image() < image3DReferenceCount.size())
                            ++image3DReferenceCount[texture->image()];
                        break;
                }

                TextureInfo info{};
                info.texture = i;
                info.name = importer->textureName(i);
                info.data = *std::move(texture);

                arrayAppend(textureInfos, std::move(info));
            }
        }

        /* In case the images have all just a single level and no names, write
           them in a compact way without listing levels. */
        bool compactImages = false;
        Containers::Array<Trade::Implementation::ImageInfo> imageInfos;
        if(args.isSet("info") || args.isSet("info-images")) {
            imageInfos = Trade::Implementation::imageInfo(*importer, error, compactImages, importTime);
        }

        for(const SceneInfo& info: sceneInfos) {
            Debug d;
            d << "Scene" << info.scene << Debug::nospace << ":";
            if(!info.name.empty()) d << info.name;
            d << Debug::newline;
            d << "    bound:" << info.mappingBound << "objects," << info.mappingType
                << "(" << Debug::nospace << Utility::formatString("{:.1f}", info.dataSize/1024.0f) << "kB)";

            for(const SceneFieldInfo& field: info.fields) {
                d << Debug::newline << " " << field.name;
                if(Trade::isSceneFieldCustom(field.name)) {
                    d << "(" << Debug::nospace << field.customName
                        << Debug::nospace << ")";
                }
                d << "@" << field.type;
                if(field.arraySize)
                    d << Debug::nospace << Utility::formatString("[{}]", field.arraySize);
                d << Debug::nospace << "," << field.size << "entries";
                if(field.flags)
                    d << Debug::newline << "   " << field.flags;
            }
        }

        for(const AnimationInfo& info: animationInfos) {
            Debug d;
            d << "Animation" << info.animation << Debug::nospace << ":";
            if(!info.name.empty()) d << info.name;

            d << Debug::newline << "  Duration:" << info.data.duration();

            for(UnsignedInt i = 0; i != info.data.trackCount(); ++i) {
                d << Debug::newline << "  Track" << i << Debug::nospace << ":"
                    << info.data.trackType(i);
                if(info.data.trackType(i) != info.data.trackResultType(i))
                    d << "->" << info.data.trackResultType(i);
                d << Debug::nospace << "," << info.data.track(i).size()
                    << "keyframes";
                if(info.data.track(i).duration() != info.data.duration())
                    d << Debug::nospace << "," << info.data.track(i).duration();
                d << Debug::newline << "   "
                    << info.data.track(i).interpolation();
                d << Debug::newline << "   "
                    << info.data.track(i).before() << Debug::nospace << ","
                    << info.data.track(i).after();
                d << Debug::newline << "    Target: object"
                    << info.data.trackTarget(i) << Debug::nospace << ","
                    << info.data.trackTargetType(i);
                /** @todo might be useful to show bounds here as well, though
                    not so much for things like complex numbers or quats */
            }
        }
        for(const SkinInfo& info: skinInfos) {
            Debug d;
            d << "Skin" << info.skin;

            /* Print reference count only if there actually are scenes and they
               were parsed, otherwise this information is useless */
            if(skinReferenceCount)
                d << Utility::formatString("(referenced by {} objects)", skinReferenceCount[info.skin]);

            d << Debug::nospace << ":";
            if(!info.name.empty()) d << info.name;

            d << Debug::newline << "  Joints:" << info.data.joints();
        }

        for(const LightInfo& info: lightInfos) {
            Debug d;
            d << "Light" << info.light;

            /* Print reference count only if there actually are scenes and they
               were parsed, otherwise this information is useless */
            if(lightReferenceCount)
                d << Utility::formatString("(referenced by {} objects)", lightReferenceCount[info.light]);

            d << Debug::nospace << ":";
            if(!info.name.empty()) d << info.name;

            d << Debug::newline << "  Type:" << info.data.type();
            d << Debug::newline << "  Color:" << info.data.color();
            d << Debug::newline << "  Intensity:" << info.data.intensity();
            d << Debug::newline << "  Attenuation:" << info.data.attenuation();
            d << Debug::newline << "  Range:" << info.data.range();
            if(info.data.type() == Trade::LightData::Type::Spot)
                d << Debug::newline << "  Cone angles:" << Deg(info.data.innerConeAngle()) << Deg(info.data.outerConeAngle());
        }

        for(const MaterialInfo& info: materialInfos) {
            Debug d;
            d << "Material" << info.material;

            /* Print reference count only if there actually are scenes and they
               were parsed, otherwise this information is useless */
            if(materialReferenceCount)
                d << Utility::formatString("(referenced by {} objects)", materialReferenceCount[info.material]);

            d << Debug::nospace << ":";
            if(!info.name.empty()) d << info.name;

            d << Debug::newline << "  Type:" << info.data.types();

            for(UnsignedInt i = 0; i != info.data.layerCount(); ++i) {
                /* Print extra layers with extra indent */
                const char* indent;
                if(info.data.layerCount() != 1 && i != 0) {
                    d << Debug::newline << "  Layer" << i << Debug::nospace << ":";
                    if(!info.data.layerName(i).isEmpty())
                        d << info.data.layerName(i);
                    indent = "   ";
                } else indent = " ";

                for(UnsignedInt j = 0; j != info.data.attributeCount(i); ++j) {
                    /* Ignore layer name (which is always first) unless it's in
                       the base material, in which case we print it as it
                       wouldn't otherwise be shown anywhere */
                    if(i && !j && info.data.attributeName(i, j) == " LayerName")
                        continue;

                    d << Debug::newline << indent
                        << info.data.attributeName(i, j) << "@"
                        << info.data.attributeType(i, j) << Debug::nospace
                        << ":";
                    switch(info.data.attributeType(i, j)) {
                        case Trade::MaterialAttributeType::Bool:
                            d << info.data.attribute<bool>(i, j);
                            break;
                        #define _c(type) case Trade::MaterialAttributeType::type: \
                            d << info.data.attribute<type>(i, j);           \
                            break;
                        _c(Float)
                        _c(Deg)
                        _c(Rad)
                        _c(UnsignedInt)
                        _c(Int)
                        _c(UnsignedLong)
                        _c(Long)
                        _c(Vector2)
                        _c(Vector2ui)
                        _c(Vector2i)
                        _c(Vector3)
                        _c(Vector3ui)
                        _c(Vector3i)
                        _c(Vector4)
                        _c(Vector4ui)
                        _c(Vector4i)
                        _c(Matrix2x2)
                        _c(Matrix2x3)
                        _c(Matrix2x4)
                        _c(Matrix3x2)
                        _c(Matrix3x3)
                        _c(Matrix3x4)
                        _c(Matrix4x2)
                        _c(Matrix4x3)
                        #undef _c
                        case Trade::MaterialAttributeType::Pointer:
                            d << info.data.attribute<const void*>(i, j);
                            break;
                        case Trade::MaterialAttributeType::MutablePointer:
                            d << info.data.attribute<void*>(i, j);
                            break;
                        case Trade::MaterialAttributeType::String:
                            d << info.data.attribute<Containers::StringView>(i, j);
                            break;
                        case Trade::MaterialAttributeType::TextureSwizzle:
                            d << info.data.attribute<Trade::MaterialTextureSwizzle>(i, j);
                            break;
                    }
                }
            }
        }

        for(const MeshInfo& info: meshInfos) {
            Debug d;
            if(info.level == 0) {
                d << "Mesh" << info.mesh;

                /* Print reference count only if there actually are scenes and
                   they were parsed, otherwise this information is useless */
                if(meshReferenceCount)
                    d << Utility::formatString("(referenced by {} objects)", meshReferenceCount[info.mesh]);

                d << Debug::nospace << ":";
                if(!info.name.empty()) d << info.name;
                d << Debug::newline;
            }
            d << "  Level" << info.level << Debug::nospace << ":"
                << info.primitive << Debug::nospace << "," << info.vertexCount
                << "vertices (" << Debug::nospace
                << Utility::formatString("{:.1f}", info.vertexDataSize/1024.0f)
                << "kB)";
            if(info.indexType != MeshIndexType{}) {
                d << Debug::newline << "   " << info.indexCount << "indices, offset" << info.indexOffset << "@"
                    << info.indexType << Debug::nospace << ", stride" << info.indexStride << "(" << Debug::nospace
                    << Utility::formatString("{:.1f}", info.indexDataSize/1024.0f)
                    << "kB)";
                if(!info.indexBounds.empty())
                    d << Debug::newline << "      bounds:" << info.indexBounds;
            }

            for(const MeshAttributeInfo& attribute: info.attributes) {
                d << Debug::newline << "    Offset" << attribute.offset
                    << Debug::nospace << ":" << attribute.name;
                if(Trade::isMeshAttributeCustom(attribute.name)) {
                    d << "(" << Debug::nospace << attribute.customName
                        << Debug::nospace << ")";
                }
                d << "@" << attribute.format;
                if(attribute.arraySize)
                    d << Debug::nospace << Utility::formatString("[{}]", attribute.arraySize);
                d << Debug::nospace << ", stride"
                    << attribute.stride;
                if(!attribute.bounds.empty())
                    d << Debug::newline << "      bounds:" << attribute.bounds;
            }
        }

        for(const TextureInfo& info: textureInfos) {
            Debug d;
            d << "Texture" << info.texture;

            /* Print reference count only if there actually are materials and
               they were parsed, otherwise this information is useless */
            if(textureReferenceCount)
                d << Utility::formatString("(referenced by {} material attributes)", textureReferenceCount[info.texture]);

            d << Debug::nospace << ":";
            if(!info.name.empty()) d << info.name;
            d << Debug::newline;
            d << "  Type:" << info.data.type();
            d << "\n  Minification:" << info.data.minificationFilter() << info.data.mipmapFilter();
            d << "\n  Magnification:" << info.data.magnificationFilter();
            d << "\n  Wrapping:" << info.data.wrapping();
            d << "\n  Image:" << info.data.image();
        }

        for(const Trade::Implementation::ImageInfo& info: imageInfos) {
            Debug d;
            if(info.level == 0) {
                if(info.size.z()) d << "3D image";
                else if(info.size.y()) d << "2D image";
                else d << "1D image";
                d << info.image;

                /* Print reference count only if there actually are textures
                   and they were parsed otherwise this information is
                   useless */
                if(info.size.z() && image3DReferenceCount)
                    d << Utility::formatString("(referenced by {} textures)", image3DReferenceCount[info.image]);
                else if(info.size.y() && image2DReferenceCount)
                    d << Utility::formatString("(referenced by {} textures)", image2DReferenceCount[info.image]);
                else if(image1DReferenceCount)
                    d << Utility::formatString("(referenced by {} textures)", image1DReferenceCount[info.image]);

                d << Debug::nospace << ":";
                if(!info.name.empty()) d << info.name;
                if(!compactImages) d << Debug::newline;
            }
            if(!compactImages) d << "  Level" << info.level << Debug::nospace << ":";
            if(info.compressed) d << info.compressedFormat;
            else d << info.format;
            if(info.size.z()) d << info.size;
            else if(info.size.y()) d << info.size.xy();
            else d << Math::Vector<1, Int>(info.size.x());
        }

        if(args.isSet("profile")) {
            Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds";
        }

        return error ? 1 : 0;
    }

    if(!importer->meshCount()) {
        Error{} << "No meshes found in" << args.value("input");
        return 1;
    }

    Containers::Optional<Trade::MeshData> mesh;

    /* Concatenate input meshes, if requested */
    if(args.isSet("concatenate-meshes")) {
        Containers::Array<Containers::Optional<Trade::MeshData>> meshes{importer->meshCount()};
        for(std::size_t i = 0; i != meshes.size(); ++i) if(!(meshes[i] = importer->mesh(i))) {
            Error{} << "Cannot import mesh" << i;
            return 1;
        }

        /* If there's a scene, use it to flatten mesh hierarchy. If not, assume
           all meshes are in the root. */
        /** @todo make it possible to choose the scene */
        if(importer->defaultScene() != -1) {
            Containers::Optional<Trade::SceneData> scene;
            if(!(scene = importer->scene(importer->defaultScene()))) {
                Error{} << "Cannot import scene" << importer->defaultScene() << "for mesh concatenation";
                return 1;
            }

            /** @todo once there are 2D scenes, check the scene is 3D */
            Containers::Array<Containers::Optional<Trade::MeshData>> flattenedMeshes;
            for(const Containers::Triple<UnsignedInt, Int, Matrix4>& meshTransformation: SceneTools::flattenMeshHierarchy3D(*scene))
                arrayAppend(flattenedMeshes, MeshTools::transform3D(*meshes[meshTransformation.first()], meshTransformation.third()));
            meshes = std::move(flattenedMeshes);
        }

        /* Concatenate all meshes together */
        /** @todo some better way than having to create a whole new array of
            references with the nasty NoInit, yet keeping the flexibility? */
        Containers::Array<Containers::Reference<const Trade::MeshData>> meshReferences{NoInit, meshes.size()};
        for(std::size_t i = 0; i != meshes.size(); ++i)
            meshReferences[i] = *meshes[i];
        /** @todo this will assert if the meshes have incompatible primitives
            (such as some triangles, some lines), or if they have
            loops/strips/fans -- handle that explicitly */
        mesh = MeshTools::concatenate(meshReferences);

    /* Otherwise import just one */
    } else {
        Trade::Implementation::Duration d{importTime};
        if(!(mesh = importer->mesh(args.value<UnsignedInt>("mesh"), args.value<UnsignedInt>("level")))) {
            Error{} << "Cannot import the mesh";
            return 4;
        }
    }

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration conversionTime{};

    /* Filter attributes, if requested */
    if(!args.value("only-attributes").empty()) {
        std::set<UnsignedInt> only;
        /** @todo drop this awful STL mess once we have an string-to-int
            API that can take non-null-terminated string views */
        for(const std::string& i: Utility::String::split(args.value("only-attributes"), ' '))
            only.insert(std::stoi(i));

        /** @todo use MeshTools::filterOnlyAttributes() once it has a rvalue
            overload that transfers ownership */
        Containers::Array<Trade::MeshAttributeData> attributes;
        for(UnsignedInt i = 0; i != mesh->attributeCount(); ++i) {
            if(only.find(i) != only.end())
                arrayAppend(attributes, mesh->attributeData(i));
        }

        const Trade::MeshIndexData indices{mesh->indices()};
        const UnsignedInt vertexCount = mesh->vertexCount();
        mesh = Trade::MeshData{mesh->primitive(),
            mesh->releaseIndexData(), indices,
            mesh->releaseVertexData(), std::move(attributes),
            vertexCount};
    }

    /* Remove duplicates, if requested */
    if(args.isSet("remove-duplicates")) {
        const UnsignedInt beforeVertexCount = mesh->vertexCount();
        {
            Trade::Implementation::Duration d{conversionTime};
            mesh = MeshTools::removeDuplicates(*std::move(mesh));
        }
        if(args.isSet("verbose"))
            Debug{} << "Duplicate removal:" << beforeVertexCount << "->" << mesh->vertexCount() << "vertices";
    }

    /* Remove duplicates with fuzzy comparison, if requested */
    /** @todo accept two values for float and double fuzzy comparison */
    if(!args.value("remove-duplicates-fuzzy").empty()) {
        const UnsignedInt beforeVertexCount = mesh->vertexCount();
        {
            Trade::Implementation::Duration d{conversionTime};
            mesh = MeshTools::removeDuplicatesFuzzy(*std::move(mesh), args.value<Float>("remove-duplicates-fuzzy"));
        }
        if(args.isSet("verbose"))
            Debug{} << "Fuzzy duplicate removal:" << beforeVertexCount << "->" << mesh->vertexCount() << "vertices";
    }

    /* Load converter plugin */
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractSceneConverter::pluginSearchPaths()[0])};

    /* Assume there's always one passed --converter option less, and the last
       is implicitly AnySceneConverter. All converters except the last one are
       expected to support ConvertMesh and the mesh is "piped" from one to the
       other. If the last converter supports ConvertMeshToFile instead of
       ConvertMesh, it's used instead of the last implicit AnySceneConverter. */
    for(std::size_t i = 0, converterCount = args.arrayValueCount("converter"); i <= converterCount; ++i) {
        const std::string converterName = i == converterCount ?
            "AnySceneConverter" : args.arrayValue("converter", i);
        Containers::Pointer<Trade::AbstractSceneConverter> converter = converterManager.loadAndInstantiate(converterName);
        if(!converter) {
            Debug{} << "Available converter plugins:" << Utility::String::join(converterManager.aliasList(), ", ");
            return 2;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) converter->addFlags(Trade::SceneConverterFlag::Verbose);
        if(i < args.arrayValueCount("converter-options"))
            Implementation::setOptions(*converter, "AnySceneConverter", args.arrayValue("converter-options", i));

        /* This is the last --converter (or the implicit AnySceneConverter at
           the end), output to a file and exit the loop */
        if(i + 1 >= converterCount && (converter->features() & Trade::SceneConverterFeature::ConvertMeshToFile)) {
            /* No verbose output for just one converter */
            if(converterCount > 1 && args.isSet("verbose"))
                Debug{} << "Saving output with" << converterName << Debug::nospace << "...";

            Trade::Implementation::Duration d{conversionTime};
            if(!converter->convertToFile(*mesh, args.value("output"))) {
                Error{} << "Cannot save file" << args.value("output");
                return 5;
            }

            break;

        /* This is not the last converter, expect that it's capable of
           ConvertMesh */
        } else {
            CORRADE_INTERNAL_ASSERT(i < converterCount);
            if(converterCount > 1 && args.isSet("verbose"))
                Debug{} << "Processing (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";

            if(!(converter->features() & Trade::SceneConverterFeature::ConvertMesh)) {
                Error{} << converterName << "doesn't support mesh conversion, only" << converter->features();
                return 6;
            }

            Trade::Implementation::Duration d{conversionTime};
            if(!(mesh = converter->convert(*mesh))) {
                Error{} << converterName << "cannot convert the mesh";
                return 7;
            }
        }
    }

    if(args.isSet("profile")) {
        Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds, conversion"
            << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count())/1.0e3f << "seconds";
    }
}
