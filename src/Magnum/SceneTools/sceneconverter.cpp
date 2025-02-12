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

#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Arguments is std::string-free */
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h> /* parseNumberSequence() */

#include "Magnum/MaterialTools/PhongToPbrMetallicRoughness.h"
#include "Magnum/MaterialTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/SceneTools/Hierarchy.h"
#include "Magnum/SceneTools/Map.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractSceneConverter.h"

#include "Magnum/Implementation/converterUtilities.h"
#include "Magnum/SceneTools/Implementation/sceneConverterUtilities.h"

namespace Magnum {

/** @page magnum-sceneconverter Scene conversion utility
@brief Converts scenes of different formats
@m_since{2020,06}

@tableofcontents
@m_footernavigation
@m_keywords{magnum-sceneconverter sceneconverter}

This utility is built if `MAGNUM_WITH_SCENECONVERTER` is enabled when building
Magnum. To use this utility with CMake, you need to request the
`sceneconverter` component of the `Magnum` package and use the
`Magnum::sceneconverter` target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED imageconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::sceneconverter ...)
@endcode

See @ref building and @ref cmake and the @ref Trade namespace for more
information. There's also a corresponding @ref magnum-imageconverter "image conversion utility".

@section magnum-sceneconverter-example Example usage

Listing contents of a glTF file, implicitly using
@relativeref{Trade,AnySceneImporter} that delegates to
@relativeref{Trade,GltfImporter}, @relativeref{Trade,AssimpImporter} or
@ref file-formats "any other plugin capable of glTF import" depending on what's
available:

@m_class{m-code-figure}

@parblock

@code{.sh}
magnum-sceneconverter --info Box.gltf
@endcode

<b></b>

@m_class{m-nopad}

@include sceneconverter-info.ansi

@endparblock

Converting an OBJ file to a glTF, implicitly using
@relativeref{Trade,AnySceneConverter} that delegates to
@relativeref{Trade,GltfSceneConverter} or
@ref file-formats "any other plugin capable of glTF export" depending on what's
available:

@code{.sh}
magnum-sceneconverter chair.obj chair.gltf
@endcode

Extracting a single mesh from a glTF to a PLY file, implicitly delegated to
@relativeref{Trade,StanfordSceneConverter}, for closer inspection:

@code{.sh}
magnum-sceneconverter scene.gltf --mesh 17 mesh17.ply
@endcode

Repacking a glTF and encoding all its images as Basis UASTC with
@relativeref{Trade,BasisImageConverter} using the @cb{.ini} imageConverter @ce
@ref Trade-GltfSceneConverter-configuration "option of GltfSceneConverter":

@code{.sh}
magnum-sceneconverter scene.gltf scene.basis.gltf \
    -c imageConverter=BasisKtxImageConverter,imageConverter/uastc
@endcode

Printing features and documented options of a particular scene converter
plugin. For debugging convenience the printed configuration file will reflect
also all options specified via `-c`:

@m_class{m-code-figure}

@parblock

@code{.sh}
magnum-sceneconverter --info-converter -C GltfSceneConverter -c copyright="Me & Myself"
@endcode

<b></b>

@m_class{m-nopad}

@include sceneconverter-info-converter.ansi

@endparblock

@subsection magnum-sceneconverter-example-image-mesh-conversion Performing operations on all images and meshes in the file

Processing a glTF file and removing duplicates in all its meshes:

@code{.sh}
magnum-sceneconverter scene.gltf --remove-duplicate-vertices scene.deduplicated.gltf
@endcode

Processing a glTF file, resizing all its images to 512x512 with
@relativeref{Trade,StbResizeImageConverter}, block-compressing their data to a
BC3 using @relativeref{Trade,StbDxtImageConverter} with high-quality output and
saving them in a KTX2 container with @relativeref{Trade,KtxImageConverter} and
an experimental [KHR_texture_ktx](https://github.com/KhronosGroup/glTF/pull/1964)
glTF extension:

@code{.sh}
magnum-sceneconverter scene.gltf scene.dxt.gltf \
    -P StbResizeImageConverter -p size="512 512" \
    -P StbDxtImageConverter -p highQuality \
    -c imageConverter=KtxImageConverter,experimentalKhrTextureKtx
@endcode

Processing a glTF file and decimating all its meshes to a half size with
@relativeref{Trade,MeshOptimizerSceneConverter}, with verbose output showing
the processing stats. The `-M` / `-m` options can be chained the same way as
`-P` / `-p` above, if needed:

@code{.sh}
magnum-sceneconverter scene.gltf scene.decimated.gltf \
    -M MeshOptimizerSceneConverter \
    -m simplify,simplifyTargetIndexCountThreshold=0.5 -v
@endcode

@section magnum-sceneconverter-usage Full usage documentation

@code{.sh}
magnum-sceneconverter [-h|--help] [-I|--importer PLUGIN]
    [-C|--converter PLUGIN]... [-P|--image-converter PLUGIN]...
    [-M|--mesh-converter PLUGIN]... [--plugin-dir DIR]
    [--prefer alias:plugin1,plugin2,…]... [--set plugin:key=val,key2=val2,…]...
    [--map] [--only-mesh-attributes N1,N2-N3…] [--remove-duplicate-vertices]
    [--remove-duplicate-vertices-fuzzy EPSILON] [--phong-to-pbr]
    [--remove-duplicate-materials]
    [-i|--importer-options key=val,key2=val2,…]
    [-c|--converter-options key=val,key2=val2,…]...
    [-p|--image-converter-options key=val,key2=val2,…]...
    [-m|--mesh-converter-options key=val,key2=val2,…]...
    [--passthrough-on-image-converter-failure]
    [--passthrough-on-mesh-converter-failure]
    [--mesh ID] [--mesh-level INDEX] [--concatenate-meshes] [--info-importer]
    [--info-converter] [--info-image-converter] [--info-animations]
    [--info-images] [--info-lights] [--info-cameras] [--info-materials]
    [--info-meshes] [--info-objects] [--info-scenes] [--info-skins]
    [--info-textures] [--info] [--color on|4bit|off|auto] [--bounds]
    [--object-hierarchy] [-v|--verbose] [--profile] [--] input output
@endcode

Arguments:

-   `input` --- input file
-   `output` --- output file; ignored if `--info` is present
-   `-h`, `--help` --- display this help message and exit
-   `-I`, `--importer PLUGIN` --- scene importer plugin (default:
    @ref Trade::AnySceneImporter "AnySceneImporter")
-   `-C`, `--converter PLUGIN` --- scene converter plugin(s)
-   `-P`, `--image-converter PLUGIN` --- converter plugin(s) to apply to each
    image in the scene
-   `-M`, `--mesh-converter PLUGIN` --- converter plugin(s) to apply to each
    mesh in the scene
-   `--plugin-dir DIR` --- override base plugin dir
-   `--prefer alias:plugin1,plugin2,…` --- prefer particular plugins for given
    alias(es)
-   `--set plugin:key=val,key2=val2,…` ---  set global plugin(s) option
-   `--map` --- memory-map the input for zero-copy import (works only for
    standalone files)
-   `--only-mesh-attributes N1,N2-N3…` --- include only mesh attributes of
    given IDs in the output. See
    @relativeref{Corrade,Utility::String::parseNumberSequence()} for syntax
    description.
-   `--remove-duplicate-vertices` --- remove duplicate vertices using
    @ref MeshTools::removeDuplicates(const Trade::MeshData&) in all meshes
    after import
-   `--remove-duplicate-vertices-fuzzy EPSILON` --- remove duplicate vertices
    using @ref MeshTools::removeDuplicatesFuzzy(const Trade::MeshData&, Float, Double)
    in all meshes after import
-   `--phong-to-pbr` --- convert Phong materials to PBR metallic/roughness
    using @ref MaterialTools::phongToPbrMetallicRoughness()
-   `--remove-duplicate-materials` --- remove duplicate materials using
    @ref MaterialTools::removeDuplicatesInPlace()
-   `-i`, `--importer-options key=val,key2=val2,…` --- configuration options to
    pass to the importer
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to scene converter(s)
-   `-p`, `--image-converter-options key=val,key2=val2,…` --- configuration
    options to pass to image converter(s)
-   `-m`, `--mesh-converter-options key=val,key2=val2,…` --- configuration
    options to pass to mesh converter(s)
-   `--passthrough-on-image-converter-failure` --- pass original data through
    if `--image-converter` fails
-   `--passthrough-on-mesh-converter-failure` --- pass original data through
    if `--mesh-converter` fails
-   `--mesh ID` --- convert just a single mesh instead of the whole scene
-   `--mesh-level LEVEL` --- level to select for single-mesh conversion
-   `--concatenate-meshes` --- flatten mesh hierarchy and concatenate them all
    together @m_class{m-label m-warning} **experimental**
-   `--info-importer` --- print info about the importer plugin and exit
-   `--info-converter` --- print info about the scene or mesh converter plugin
    and exit
-   `--info-image-converter` --- print info about the image converter plugin
    and exit
-   `--info-animations` --- print into about animations in the input file and
    exit
-   `--info-images` --- print into about images in the input file and exit
-   `--info-lights` --- print into about lights in the input file and exit
-   `--info-cameras` --- print into about cameras in the input file and exit
-   `--info-materials` --- print into about materials in the input file and
    exit
-   `--info-meshes` --- print into about meshes in the input file and exit
-   `--info-objects` --- print into about objects in the input file and exit
-   `--info-scenes` --- print into about scenes in the input file and exit
-   `--info-skins` --- print into about skins in the input file and exit
-   `--info-textures` --- print into about textures in the input file and exit
-   `--info` --- print info about everything in the input file and exit, same
    as specifying all other data-related `--info-*` options together
-   `--color` --- colored output for `--info` (default: `auto`)
-   `--bounds` --- show bounds of known attributes in `--info` output
-   `--object-hierarchy` --- visualize object hierarchy in `--info` output
-   `-v`, `--verbose` --- verbose output from importer and converter plugins
-   `--profile` --- measure import and conversion time

If any of the `--info-importer`, `--info-converter` or `--info-image-converter`
options are given, the utility will print information about given plugin
specified via the `-I`, `-C` or `-P` option, including its configuration
options potentially overriden with `-i`, `-c` or `-p`. In this case no file is
read and no conversion is done and neither the input nor the output file needs
to be specified.

If any of the other `--info-*` options are given, the utility will print
information about given data. In this case the input file is read but no
conversion is done and the output file doesn't need to be specified. In case
one data references another and both `--info-*` options are specified, the
output will also list reference count (for example, `--info-scenes` together
with `--info-meshes` will print how many objects reference given mesh).

The `-i`, `-c` and `-m` arguments accept a comma-separated list of key/value
pairs to set in the importer / converter plugin configuration. If the `=`
character is omitted, it's equivalent to saying `key=true`; configuration
subgroups are delimited with `/`. Prefix the key with `+` to add new options or
multiple options of the same name.

It's possible to specify the `-C` option (and correspondingly also `-c`)
multiple times in order to chain more converters together. All converters in
the chain have to support the
@ref Trade::SceneConverterFeature::ConvertMultiple or
@relativeref{Trade::SceneConverterFeature,ConvertMesh} feature, the last
converter either @ref Trade::SceneConverterFeature::ConvertMultiple,
@relativeref{Trade::SceneConverterFeature,ConvertMesh},
@relativeref{Trade::SceneConverterFeature,ConvertMultipleToFile} or
@relativeref{Trade::SceneConverterFeature,ConvertMeshToFile}. If the last
converter doesn't support conversion to a file,
@relativeref{Trade,AnySceneConverter} is used to save its output. If no `-C` is
specified, @relativeref{Trade,AnySceneConverter} is used.

Similarly, the `-P` / `-M` options (and correspondingly also `-p` / `-m`) can
be specified multiple times in order to chain more image / mesh converters
together. All image converters in the chain have to support the ConvertImage*D
feature for given image dimensions, all mesh converters in the chain have to
support the ConvertMesh feature. If no `-P` / `-M` is specified, the imported
images / meshes are passed directly to the scene converter.

The `--remove-duplicate-vertices`, `--phong-to-pbr` and
`--remove-duplicate-materials` operations are performed on meshes and materials
before passing them to any converter.

If `--concatenate-meshes` is given, all meshes of the input file are
first concatenated into a single mesh using @ref MeshTools::concatenate(), with
the scene hierarchy transformation baked in using
@ref SceneTools::absoluteFieldTransformations3D(), and then passed through the
remaining operations. Only attributes that are present in the first mesh are
taken, if `--only-mesh-attributes` is specified as well, the IDs reference
attributes of the first mesh.
*/

}

using namespace Magnum;
using namespace Containers::Literals;

namespace {

bool isPluginInfoRequested(const Utility::Arguments& args) {
    return args.isSet("info-importer") ||
           args.isSet("info-converter") ||
           args.isSet("info-image-converter");
}

bool isDataInfoRequested(const Utility::Arguments& args) {
    return args.isSet("info-animations") ||
           args.isSet("info-images") ||
           args.isSet("info-lights") ||
           args.isSet("info-cameras") ||
           args.isSet("info-materials") ||
           args.isSet("info-meshes") ||
           args.isSet("info-objects") ||
           args.isSet("info-scenes") ||
           args.isSet("info-skins") ||
           args.isSet("info-textures") ||
           args.isSet("info");
}

template<UnsignedInt dimensions> bool runImageConverters(PluginManager::Manager<Trade::AbstractImageConverter>& imageConverterManager, const Utility::Arguments& args, const UnsignedInt i, Containers::Optional<Trade::ImageData<dimensions>>& image) {
    const bool passthroughOnConversionFailure = args.isSet("passthrough-on-image-converter-failure");

    for(std::size_t j = 0, imageConverterCount = args.arrayValueCount("image-converter"); j != imageConverterCount; ++j) {
        const Containers::StringView imageConverterName = args.arrayValue<Containers::StringView>("image-converter", j);
        if(args.isSet("verbose")) {
            Debug d;
            d << "Processing" << dimensions << Debug::nospace << "D image" << i;
            if(imageConverterCount > 1)
                d << "(" << Debug::nospace << (j+1) << Debug::nospace << "/" << Debug::nospace << imageConverterCount << Debug::nospace << ")";
            d << "with" << imageConverterName << Debug::nospace << "...";
        }

        Containers::Pointer<Trade::AbstractImageConverter> imageConverter = imageConverterManager.loadAndInstantiate(imageConverterName);
        if(!imageConverter) {
            Debug{} << "Available image converter plugins:" << ", "_s.join(imageConverterManager.aliasList());
            return false;
        }

        /* Set options, if passed. The AnyImageConverter check makes no
            sense here, is just there because the helper wants it */
        if(args.isSet("verbose")) imageConverter->addFlags(Trade::ImageConverterFlag::Verbose);
        if(j < args.arrayValueCount("image-converter-options"))
            Implementation::setOptions(*imageConverter, "AnyImageConverter", args.arrayValue("image-converter-options", j));

        Trade::ImageConverterFeatures expectedFeatures;
        if(dimensions == 2) {
            expectedFeatures = image->isCompressed() ?
                Trade::ImageConverterFeature::ConvertCompressed2D :
                Trade::ImageConverterFeature::Convert2D;
        } else if(dimensions == 3) {
            expectedFeatures = image->isCompressed() ?
                Trade::ImageConverterFeature::ConvertCompressed3D :
                Trade::ImageConverterFeature::Convert3D;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        /** @todo level-related features, once testable */
        if(!(imageConverter->features() >= expectedFeatures)) {
            Error err;
            err << imageConverterName << "doesn't support";
            /** @todo level-related message, once testable */
            if(image->isCompressed())
                err << "compressed";
            err << dimensions << Debug::nospace << "D image conversion, only" << Debug::packed << imageConverter->features();
            return false;
        }

        /** @todo handle image levels here, once GltfSceneConverter is capable
            of converting them (which needs AbstractImageConverter to be
            reworked around ImageData) */
        if(Containers::Optional<Trade::ImageData<dimensions>> converted = imageConverter->convert(*image)) {
            image = Utility::move(converted);
        } else if(passthroughOnConversionFailure) {
            Warning{} << "Cannot process" << dimensions << Debug::nospace << "D image" << i << "with" << imageConverterName << Debug::nospace << ", passing the original through";
        } else {
            Error{} << "Cannot process" << dimensions << Debug::nospace << "D image" << i << "with" << imageConverterName;
            return false;
        }
    }

    return true;
}

}

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input file")
        .addArgument("output").setHelp("output", "output file; ignored if --info is present")
        .addOption('I', "importer", "AnySceneImporter").setHelp("importer", "scene importer plugin", "PLUGIN")
        .addArrayOption('C', "converter").setHelp("converter", "scene converter plugin(s)", "PLUGIN")
        .addArrayOption('P', "image-converter").setHelp("image-converter", "converter plugin(s) to apply to each image in the scene", "PLUGIN")
        .addArrayOption('M', "mesh-converter").setHelp("mesh-converter", "converter plugin(s) to apply to each mesh in the scene", "PLUGIN")
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        #endif
        .addArrayOption("prefer").setHelp("prefer", "prefer particular plugins for given alias(es)", "alias:plugin1,plugin2,…")
        .addArrayOption("set").setHelp("set", "set global plugin(s) options", "plugin:key=val,key2=val2,…")
        #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
        .addBooleanOption("map").setHelp("map", "memory-map the input for zero-copy import (works only for standalone files)")
        #endif
        .addOption("only-mesh-attributes").setHelp("only-mesh-attributes", "include only mesh attributes of given IDs in the output", "N1,N2-N3…")
        .addBooleanOption("remove-duplicate-vertices").setHelp("remove-duplicate-vertices", "remove duplicate vertices in all meshes after import")
        .addOption("remove-duplicate-vertices-fuzzy").setHelp("remove-duplicate-vertices-fuzzy", "remove duplicate vertices with fuzzy comparison in all meshes after import", "EPSILON")
        .addBooleanOption("phong-to-pbr").setHelp("phong-to-pbr", "convert Phong materials to PBR metallic/roughness")
        .addBooleanOption("remove-duplicate-materials").setHelp("remove-duplicate-materials", "remove duplicate materials")
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addArrayOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter(s)", "key=val,key2=val2,…")
        .addArrayOption('p', "image-converter-options").setHelp("image-converter-options", "configuration options to pass to the image converter(s)", "key=val,key2=val2,…")
        .addArrayOption('m', "mesh-converter-options").setHelp("mesh-converter-options", "configuration options to pass to the mesh converter(s)", "key=val,key2=val2,…")
        .addBooleanOption("passthrough-on-image-converter-failure").setHelp("passthrough-on-image-converter-failure", "pass original data through if --image-converter fails")
        .addBooleanOption("passthrough-on-mesh-converter-failure").setHelp("passthrough-on-mesh-converter-failure", "pass original data through if --mesh-converter fails")
        .addOption("mesh").setHelp("mesh", "convert just a single mesh instead of the whole scene, ignored if --concatenate-meshes is specified", "ID")
        .addOption("mesh-level").setHelp("mesh-level", "level to select for single-mesh conversion", "index")
        .addBooleanOption("concatenate-meshes").setHelp("concatenate-meshes", "flatten mesh hierarchy and concatenate them all together")
        .addBooleanOption("info-importer").setHelp("info-importer", "print info about the importer plugin and exit")
        .addBooleanOption("info-converter").setHelp("info-converter", "print info about the scene or mesh converter plugin and exit")
        .addBooleanOption("info-image-converter").setHelp("info-image-converter", "print info about the image converter plugin and exit")
        .addBooleanOption("info-animations").setHelp("info-animations", "print info about animations in the input file and exit")
        .addBooleanOption("info-images").setHelp("info-images", "print info about images in the input file and exit")
        .addBooleanOption("info-lights").setHelp("info-lights", "print info about images in the input file and exit")
        .addBooleanOption("info-cameras").setHelp("info-cameras", "print info about cameras in the input file and exit")
        .addBooleanOption("info-materials").setHelp("info-materials", "print info about materials in the input file and exit")
        .addBooleanOption("info-meshes").setHelp("info-meshes", "print info about meshes in the input file and exit")
        .addBooleanOption("info-objects").setHelp("info-objects", "print info about objects in the input file and exit")
        .addBooleanOption("info-scenes").setHelp("info-scenes", "print info about scenes in the input file and exit")
        .addBooleanOption("info-skins").setHelp("info-skins", "print info about skins in the input file and exit")
        .addBooleanOption("info-textures").setHelp("info-textures", "print info about textures in the input file and exit")
        .addBooleanOption("info").setHelp("info", "print info about everything in the input file and exit, same as specifying all other data-related --info-* options together")
        .addOption("color", "auto").setHelp("color", "colored output for --info", "on|4bit|off|auto")
        .addBooleanOption("bounds").setHelp("bounds", "show bounds of known attributes in --info output")
        .addBooleanOption("object-hierarchy").setHelp("object-hierarchy", "visualize object hierarchy in --info output")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from importer and converter plugins")
        .addBooleanOption("profile").setHelp("profile", "measure import and conversion time")
        .setParseErrorCallback([](const Utility::Arguments& args, Utility::Arguments::ParseError error, const std::string& key) {
            /* If --info for plugins is passed, we don't need the input */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
               key == "input" && isPluginInfoRequested(args))
                return true;
            /* If --info for plugins or data is passed, we don't need the
               output argument */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
                key == "output" && (isPluginInfoRequested(args) || isDataInfoRequested(args)))
                return true;

            /* Handle all other errors as usual */
            return false;
        })
        .setGlobalHelp(R"(Converts scenes of different formats.

If any of the --info-importer, --info-converter or --info-image-converter
options are given, the utility will print information about given plugin
specified via the -I, -C or -P option. In this case no file is read and no
conversion is done and neither the input nor the output file needs to be
specified.

If any of the other --info-* options are given, the utility will print
information about given data. In this case the input file is read but no
conversion is done and the output file doesn't need to be specified. In case
one data references another and both --info-* options are specified, the output
will also list reference count (for example, --info-scenes together with
--info-meshes will print how many objects reference given mesh).

The -i, -c and -m arguments accept a comma-separated list of key/value
pairs to set in the importer / converter plugin configuration. If the =
character is omitted, it's equivalent to saying key=true; configuration
subgroups are delimited with /. Prefix the key with + to add new options or
multiple options of the same name.

It's possible to specify the -C option (and correspondingly also -c) multiple
times in order to chain more scene converters together. All converters in the
chain have to support the ConvertMultiple or ConvertMesh feature, the last
converter either ConvertMultiple, ConvertMesh, ConvertMultipleToFile or
ConvertMeshToFile. If the last converter doesn't support conversion to a file,
AnySceneConverter is used to save its output. If no -C is specified,
AnySceneConverter is used.

Similarly, the -P / -M options (and correspondingly also -p / -m) can be
specified multiple times in order to chain more image / mesh converters
together. All image converters in the chain have to support the ConvertImage*D
feature for given image dimensions, all mesh converters in the chain have to
support the ConvertMesh feature. If no -P / -M is specified, the imported
images / meshes are passed directly to the scene converter.

The --remove-duplicate-vertices, --phong-to-pbr and
--remove-duplicate-materials operations are performed on meshes and materials
before passing them to any converter.

If --concatenate-meshes is given, all meshes of the input file are first
concatenated into a single mesh, with the scene hierarchy transformation baked
in, and then passed through the remaining operations. Only attributes that are
present in the first mesh are taken, if --only-mesh-attributes is specified as
well, the IDs reference attributes of the first mesh.)")
        .parse(argc, argv);

    /* Colored output. Enable only if a TTY. */
    Debug::Flags useColor;
    bool useColor24;
    if(args.value("color") == "on") {
        useColor = Debug::Flags{};
        useColor24 = true;
    } else if(args.value("color") == "4bit") {
        useColor = Debug::Flags{};
        useColor24 = false;
    } else if(args.value("color") == "off") {
        useColor = Debug::Flag::DisableColors;
        useColor24 = false;
    } else if(Debug::isTty()) {
        useColor = Debug::Flags{};
        /* https://unix.stackexchange.com/a/450366, not perfect but good enough
           I'd say */
        /** @todo make this more robust and put directly on Debug,
            including a "Disable 24 colors" flag */
        const Containers::StringView colorterm = std::getenv("COLORTERM");
        useColor24 = colorterm == "truecolor"_s || colorterm == "24bit"_s;
    } else {
        useColor = Debug::Flag::DisableColors;
        useColor24 = false;
    }

    /* Generic checks */
    if(args.value<Containers::StringView>("input")) {
        /* Not an error in this case, it should be possible to just append
           --info* to existing command line without having to remove anything.
           But print a warning at least, it could also be a mistyped option. */
        if(isPluginInfoRequested(args))
            Warning{} << "Ignoring input file for --info:" << args.value<Containers::StringView>("input");
    }
    if(args.value<Containers::StringView>("output")) {
        /* Same as above, it should be possible to just append --info* to
           existing command line */
        if(isPluginInfoRequested(args) || isDataInfoRequested(args))
            Warning{} << "Ignoring output file for --info:" << args.value<Containers::StringView>("output");
    }
    if(args.isSet("concatenate-meshes") && args.value<Containers::StringView>("mesh")) {
        Error{} << "The --mesh and --concatenate-meshes options are mutually exclusive";
        return 1;
    }
    if(args.value<Containers::StringView>("mesh-level") && !args.value<Containers::StringView>("mesh")) {
        Error{} << "The --mesh-level option can only be used with --mesh";
        return 1;
    }
    /** @todo remove this once only-mesh-attributes can work with attribute
        names and thus for more meshes */
    if(args.value<Containers::StringView>("only-mesh-attributes") && !args.value<Containers::StringView>("mesh") && !args.isSet("concatenate-meshes")) {
        Error{} << "The --only-mesh-attributes option can only be used with --mesh or --concatenate-meshes";
        return 1;
    }

    /* Importer manager */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Trade::AbstractImporter::pluginSearchPaths().back()))
        #endif
    };

    /* Image converter manager for potential dependencies. Needs to be
       constructed before the scene converter manager for proper destruction
       order. */
    PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Trade::AbstractImageConverter::pluginSearchPaths().back()))
        #endif
    };

    /* Scene converter manager, register the image converter manager with it */
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Trade::AbstractSceneConverter::pluginSearchPaths().back()))
        #endif
    };
    converterManager.registerExternalManager(imageConverterManager);

    /* Set preferred plugins */
    for(std::size_t i = 0, iMax = args.arrayValueCount("prefer"); i != iMax; ++i) {
        const auto value = args.arrayValue<Containers::StringView>("prefer", i);
        const Containers::Array3<Containers::StringView> aliasNames = value.partition(':');
        if(!aliasNames[1]) {
            Error{} << "Invalid --prefer option" << value;
            return 1;
        }

        /* Figure out manager name */
        PluginManager::AbstractManager* manager;
        if(aliasNames[0].hasSuffix("Importer"_s))
            manager = &importerManager;
        else if(aliasNames[0].hasSuffix("ImageConverter"_s))
            manager = &imageConverterManager;
        else if(aliasNames[0].hasSuffix("SceneConverter"_s))
            manager = &converterManager;
        else {
            Error{} << "Alias" << aliasNames[0] << "not recognized for a --prefer option";
            return 1;
        }

        /* The alias has to be found, otherwise it'd assert */
        if(manager->loadState(aliasNames[0]) == PluginManager::LoadState::NotFound) {
            Error{} << "Alias" << aliasNames[0] << "not found for a --prefer option";
            return 1;
        }

        /* Check that the names actually provide given alias, otherwise it'd
           assert */
        const Containers::Array<Containers::StringView> names = aliasNames[2].splitWithoutEmptyParts(',');
        for(const Containers::StringView name: names) {
            /* Not found plugins are allowed in the list */
            const PluginManager::PluginMetadata* const metadata = manager->metadata(name);
            if(!metadata)
                continue;

            bool found = false;
            for(const Containers::StringView provides: metadata->provides()) {
                if(provides == aliasNames[0]) {
                    found = true;
                    break;
                }
            }
            if(!found) {
                Error{} << name << "doesn't provide" << aliasNames[0] << "for a --prefer option";
                return 1;
            }
        }

        manager->setPreferredPlugins(aliasNames[0], names);
    }

    /* Set global plugin options */
    for(std::size_t i = 0, iMax = args.arrayValueCount("set"); i != iMax; ++i) {
        const auto value = args.arrayValue<Containers::StringView>("set", i);
        const Containers::Array3<Containers::StringView> nameOptions = value.partition(':');
        if(!nameOptions[1]) {
            Error{} << "Invalid --set option" << value;
            return 1;
        }

        /* Figure out manager name */
        PluginManager::AbstractManager* manager;
        if(nameOptions[0].hasSuffix("Importer"_s))
            manager = &importerManager;
        else if(nameOptions[0].hasSuffix("ImageConverter"_s))
            manager = &imageConverterManager;
        else if(nameOptions[0].hasSuffix("SceneConverter"_s))
            manager = &converterManager;
        else {
            Error{} << "Plugin" << nameOptions[0] << "not recognized for a --set option";
            return 1;
        }

        /* Get the metadata to access global configuration */
        PluginManager::PluginMetadata* const metadata = manager->metadata(nameOptions[0]);
        if(!metadata) {
            Error{} << "Plugin" << nameOptions[0] << "not found for a --set option";
            return 1;
        }

        /* Set options. Doing things like --set AnyImageImporter:foo=bar makes
           no sense, so this isn't excluding any "Any*" plugins from the
           unrecognized option warnings */
        Implementation::setOptions(nameOptions[0], metadata->configuration(), {}, nameOptions[2]);
    }

    /* Print plugin info, if requested */
    /** @todo these all duplicate plugin loading & option setting, move to
        some helpers (shared among all command-line tools)? */
    if(args.isSet("info-importer")) {
        Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
        if(!importer) {
            Debug{} << "Available importer plugins:" << ", "_s.join(importerManager.aliasList());
            return 1;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) importer->addFlags(Trade::ImporterFlag::Verbose);
        Implementation::setOptions(*importer, "AnySceneImporter", args.value("importer-options"));
        Trade::Implementation::printImporterInfo(useColor, *importer);
        return 0;
    }
    if(args.isSet("info-converter")) {
        Containers::Pointer<Trade::AbstractSceneConverter> converter = converterManager.loadAndInstantiate(args.arrayValueCount("converter") ? args.arrayValue("converter", 0) : "AnySceneConverter");
        if(!converter) {
            Debug{} << "Available converter plugins:" << ", "_s.join(converterManager.aliasList());
            return 1;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) converter->addFlags(Trade::SceneConverterFlag::Verbose);
        if(args.arrayValueCount("converter-options"))
            Implementation::setOptions(*converter, "AnySceneConverter", args.arrayValue("converter-options", 0));
        SceneTools::Implementation::printSceneConverterInfo(useColor, *converter);
        return 0;
    }
    if(args.isSet("info-image-converter")) {
        Containers::Pointer<Trade::AbstractImageConverter> converter = imageConverterManager.loadAndInstantiate(args.arrayValueCount("image-converter") ? args.arrayValue("image-converter", 0) : "AnyImageConverter");
        if(!converter) {
            Debug{} << "Available image converter plugins:" << ", "_s.join(imageConverterManager.aliasList());
            return 1;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) converter->addFlags(Trade::ImageConverterFlag::Verbose);
        if(args.arrayValueCount("image-converter-options"))
            Implementation::setOptions(*converter, "AnyImageConverter", args.arrayValue("image-converter-options", 0));
        Trade::Implementation::printImageConverterInfo(useColor, *converter);
        return 0;
    }

    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
    if(!importer) {
        Debug{} << "Available importer plugins:" << ", "_s.join(importerManager.aliasList());
        return 1;
    }

    /* Set options, if passed */
    if(args.isSet("verbose")) importer->addFlags(Trade::ImporterFlag::Verbose);
    Implementation::setOptions(*importer, "AnySceneImporter", args.value("importer-options"));

    /* Wow, C++, you suck. This implicitly initializes to random shit?!

       Also, because of addSupportedImporterContents() it's not really possible
       to distinguish between time spent importing and time spent converting.
       So it's lumped into a single variable. Steps that are really just
       conversion are measured separately. */
    std::chrono::high_resolution_clock::duration importConversionTime{};

    /* Open the file or map it if requested */
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
    Containers::Optional<Containers::Array<const char, Utility::Path::MapDeleter>> mapped;
    if(args.isSet("map")) {
        Trade::Implementation::Duration d{importConversionTime};
        if(!(mapped = Utility::Path::mapRead(args.value("input"))) || !importer->openMemory(*mapped)) {
            Error() << "Cannot memory-map file" << args.value("input");
            return 3;
        }
    } else
    #endif
    {
        Trade::Implementation::Duration d{importConversionTime};
        if(!importer->openFile(args.value("input"))) {
            Error() << "Cannot open file" << args.value("input");
            return 3;
        }
    }

    /* Print file info, if requested */
    if(isDataInfoRequested(args)) {
        const bool error = SceneTools::Implementation::printInfo(useColor, useColor24, args, *importer, importConversionTime);

        if(args.isSet("profile")) {
            Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importConversionTime).count())/1.0e3f << "seconds";
        }

        return error ? 1 : 0;
    }

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration conversionTime{};

    /* Import all scenes, in case something later needs to modify them. There's
       currently no other operations done on those. */
    Containers::Array<Trade::SceneData> scenes;
    if(args.isSet("remove-duplicate-materials")) {
        arrayReserve(scenes, importer->sceneCount());

        for(UnsignedInt i = 0; i != importer->sceneCount(); ++i) {
            Containers::Optional<Trade::SceneData> scene;
            {
                Trade::Implementation::Duration d{importConversionTime};
                if(!(scene = importer->scene(i))) {
                    Error{} << "Cannot import scene" << i;
                    return 1;
                }
            }

            /* There's currently no operations done on scenes directly */

            arrayAppend(scenes, *Utility::move(scene));
        }
    }

    /* Take a single mesh or concatenate all meshes together, if requested.
       After that, the importer is changed to one that contains just a single
       mesh... */
    bool singleMesh = false;
    if(args.isSet("concatenate-meshes") || args.value<Containers::StringView>("mesh")) {
        singleMesh = true;
        /* ... and subsequent conversion deals with just meshes, throwing away
           materials and everything else (if present). */

        Containers::Optional<Trade::MeshData> mesh;

        /* Concatenate all meshes together */
        if(args.isSet("concatenate-meshes")) {
            if(!importer->meshCount()) {
                Error{} << "No meshes found in" << args.value("input");
                return 1;
            }

            Containers::Array<Trade::MeshData> meshes;
            arrayReserve(meshes, importer->meshCount());
            /** @todo handle mesh levels here, once any plugin is capable of
                importing them */
            for(std::size_t i = 0, iMax = importer->meshCount(); i != iMax; ++i) {
                Trade::Implementation::Duration d{importConversionTime};
                Containers::Optional<Trade::MeshData> meshToConcatenate = importer->mesh(i);
                if(!meshToConcatenate) {
                    Error{} << "Cannot import mesh" << i;
                    return 1;
                }

                arrayAppend(meshes, *Utility::move(meshToConcatenate));
            }

            /* If there's a scene, use it to flatten mesh hierarchy. If not,
               assume all meshes are in the root. */
            if(importer->defaultScene() != -1 || importer->sceneCount()) {
                Containers::Optional<Trade::SceneData> scene;
                {
                    /** @todo once the required SceneTools APIs exist, rework
                        this to concatenate only what actually makes sense (and
                        thus preserve the (multi-)scene hierarchy, with the
                        original behavior only being achievable if everything
                        except meshes and scene hierarchy is filtered away */
                    const UnsignedInt defaultScene = importer->defaultScene() == -1 ? 0 : importer->defaultScene();
                    Trade::Implementation::Duration d{importConversionTime};
                    if(!(scene = importer->scene(defaultScene))) {
                        Error{} << "Cannot import scene" << defaultScene << "for mesh concatenation";
                        return 1;
                    }
                }

                Containers::Array<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>
                    meshesMaterials = scene->meshesMaterialsAsArray();
                Containers::Array<Matrix4> transformations =
                    SceneTools::absoluteFieldTransformations3D(*scene, Trade::SceneField::Mesh);
                Containers::Array<Trade::MeshData> flattenedMeshes;
                {
                    Trade::Implementation::Duration d{conversionTime};
                    /** @todo once there are 2D scenes, check the scene is 3D */
                    for(std::size_t i = 0; i != meshesMaterials.size(); ++i) {
                        arrayAppend(flattenedMeshes, MeshTools::transform3D(
                            meshes[meshesMaterials[i].second().first()], transformations[i]));
                    }
                }
                meshes = Utility::move(flattenedMeshes);
            }

            {
                Trade::Implementation::Duration d{conversionTime};
                /** @todo this will assert if the meshes have incompatible primitives
                    (such as some triangles, some lines), or if they have
                    loops/strips/fans -- handle that explicitly */
                mesh = MeshTools::concatenate(meshes);
            }

        /* Otherwise import just one */
        } else {
            Trade::Implementation::Duration d{importConversionTime};
            if(!(mesh = importer->mesh(args.value<UnsignedInt>("mesh"), args.value<UnsignedInt>("mesh-level")))) {
                Error{} << "Cannot import the mesh";
                return 4;
            }
        }

        /* Filter mesh attributes, if requested */
        /** @todo move outside of the --mesh / --concatenate-meshes branch once
            it's possible to filter attributes by name */
        if(Containers::StringView onlyAttributes = args.value<Containers::StringView>("only-mesh-attributes")) {
            const Containers::Optional<Containers::Array<UnsignedInt>> only = Utility::String::parseNumberSequence(onlyAttributes, 0, mesh->attributeCount());
            if(!only) return 2;

            /** @todo use MeshTools::filterOnlyAttributes() once it has a
                rvalue overload that transfers ownership */
            Containers::Array<Trade::MeshAttributeData> attributes;
            arrayReserve(attributes, only->size());
            for(UnsignedInt i: *only)
                arrayAppend(attributes, mesh->attributeData(i));

            const Trade::MeshIndexData indices{mesh->indices()};
            const UnsignedInt vertexCount = mesh->vertexCount();
            mesh = Trade::MeshData{mesh->primitive(),
                mesh->releaseIndexData(), indices,
                mesh->releaseVertexData(), Utility::move(attributes),
                vertexCount};
        }

        /* Create an importer instance that contains just the single mesh and
           related metadata for further steps, without anything else */
        /** @todo might be useful to have this split out of the file and tested
            directly if the complexity grows even further */
        struct SingleMeshImporter: Trade::AbstractImporter {
            explicit SingleMeshImporter(Trade::MeshData&& mesh_, Containers::String&& name, Trade::AbstractImporter& original): mesh{Utility::move(mesh_)}, name{Utility::move(name)} {
                for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
                    const Trade::MeshAttribute attributeName = mesh.attributeName(i);
                    if(!isMeshAttributeCustom(attributeName)) continue;
                    /* Appending even empty ones so we don't have to
                       special-case "not found" in doMeshAttributeName() */
                    arrayAppend(attributeNames, InPlaceInit, attributeName, original.meshAttributeName(attributeName));
                }
            }

            Trade::ImporterFeatures doFeatures() const override { return {}; } /* LCOV_EXCL_LINE */
            bool doIsOpened() const override { return true; }
            void doClose() override {} /* LCOV_EXCL_LINE */

            UnsignedInt doMeshCount() const override { return 1; }
            Containers::String doMeshName(UnsignedInt) override {
                return name;
            }
            Containers::String doMeshAttributeName(Trade::MeshAttribute name) override {
                for(const Containers::Pair<Trade::MeshAttribute, Containers::String>& i: attributeNames)
                    if(i.first() == name) return i.second();
                /* All custom attributes, including the unnamed, are in the
                   attributeNames array and both our attribute name propagation
                   here and addSupportedImporterContents() call
                   meshAttributeName() only with attributes present in the
                   actual mesh, so this should never be reached */
                CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
            }
            Containers::Optional<Trade::MeshData> doMesh(UnsignedInt, UnsignedInt) override {
                return MeshTools::reference(mesh);
            }

            Trade::MeshData mesh;
            Containers::String name;
            Containers::Array<Containers::Pair<Trade::MeshAttribute, Containers::String>> attributeNames;
        };

        /* Save the previous importer so we can pass it to the constructor in
           emplace(), otherwise the constructor would access a deleted pointer */
        /** @todo would it make sense for emplace() to first construct and only
            then delete so I wouldn't need to juggle the previous value
            manually? */
        Containers::Pointer<Trade::AbstractImporter> previousImporter = Utility::move(importer);
        importer.emplace<SingleMeshImporter>(*Utility::move(mesh),
            /* Propagate the name only in case of a single mesh, for
               concatenation it wouldn't make sense */
            args.value<Containers::StringView>("mesh") ? previousImporter->meshName(args.value<UnsignedInt>("mesh")) : Containers::String{},
            *previousImporter);
    }

    /* Operations to perform on all images in the importer. If there are any,
       images are supplied manually to the converter from the array below. */
    Containers::Array<Trade::ImageData2D> images2D;
    Containers::Array<Trade::ImageData3D> images3D;
    if(args.arrayValueCount("image-converter")) {
        /** @todo implement once there's any file format capable of storing
            these */
        if(importer->image1DCount()) {
            Error{} << "Sorry, 1D image conversion is not implemented yet";
            return 1;
        }

        for(UnsignedInt i = 0; i != importer->image2DCount(); ++i) {
            Containers::Optional<Trade::ImageData2D> image;
            {
                /** @todo handle image levels once GltfSceneConverter can save
                    them (which needs AbstractImageConverter to be reworked
                    around ImageData) -- there could be an image2DOffsets
                    array saying which subrange is levels for which image */
                Trade::Implementation::Duration d{importConversionTime};
                if(!(image = importer->image2D(i))) {
                    Error{} << "Cannot import 2D image" << i;
                    return 1;
                }
            }

            if(!runImageConverters(imageConverterManager, args, i, image))
                return 1;

            arrayAppend(images2D, *Utility::move(image));
        }

        for(UnsignedInt i = 0; i != importer->image3DCount(); ++i) {
            Containers::Optional<Trade::ImageData3D> image;
            {
                /** @todo handle image levels once GltfSceneConverter can save
                    them (which needs AbstractImageConverter to be reworked
                    around ImageData) -- there could be an image2DOffsets
                    array saying which subrange is levels for which image */
                Trade::Implementation::Duration d{importConversionTime};
                if(!(image = importer->image3D(i))) {
                    Error{} << "Cannot import 3D image" << i;
                    return 1;
                }
            }

            if(!runImageConverters(imageConverterManager, args, i, image))
                return 1;

            arrayAppend(images3D, *Utility::move(image));
        }
    }

    /* Operations to perform on all meshes in the importer. If there are any,
       meshes are supplied manually to the converter from the array below. */
    Containers::Array<Trade::MeshData> meshes;
    if(args.isSet("remove-duplicate-vertices") ||
       args.value<Containers::StringView>("remove-duplicate-vertices-fuzzy") ||
       args.arrayValueCount("mesh-converter"))
    {
        const bool passthroughOnConversionFailure = args.isSet("passthrough-on-mesh-converter-failure");

        arrayReserve(meshes, importer->meshCount());

        for(UnsignedInt i = 0; i != importer->meshCount(); ++i) {
            Containers::Optional<Trade::MeshData> mesh;
            {
                /** @todo handle mesh levels here, once any plugin is capable
                    of importing them */
                Trade::Implementation::Duration d{importConversionTime};
                if(!(mesh = importer->mesh(i))) {
                    Error{} << "Cannot import mesh" << i;
                    return 1;
                }
            }

            /* Duplicate removal */
            if(args.isSet("remove-duplicate-vertices") ||
               args.value<Containers::StringView>("remove-duplicate-vertices-fuzzy"))
            {
                const UnsignedInt beforeVertexCount = mesh->vertexCount();
                const bool fuzzy = !!args.value<Containers::StringView>("remove-duplicate-vertices-fuzzy");

                /** @todo accept two values for float and double fuzzy
                    comparison, or maybe also different for positions, normals
                    and texcoords? ugh... */
                if(fuzzy) {
                    Trade::Implementation::Duration d{conversionTime};
                    mesh = MeshTools::removeDuplicatesFuzzy(*Utility::move(mesh), args.value<Float>("remove-duplicate-vertices-fuzzy"));
                } else {
                    Trade::Implementation::Duration d{conversionTime};
                    mesh = MeshTools::removeDuplicates(*Utility::move(mesh));
                }

                if(args.isSet("verbose")) {
                    Debug d;
                    /* Mesh index 0 would be confusing in case of
                        --concatenate-meshes and plain wrong with --mesh, so
                        don't even print it */
                    if(singleMesh)
                        d << (fuzzy ? "Fuzzy duplicate removal:" : "Duplicate removal:");
                    else
                        d << "Mesh" << i << (fuzzy ? "fuzzy duplicate removal:" : "duplicate removal:");
                    d << beforeVertexCount << "->" << mesh->vertexCount() << "vertices";
                }
            }

            /* Arbitrary mesh converters */
            for(std::size_t j = 0, meshConverterCount = args.arrayValueCount("mesh-converter"); j != meshConverterCount; ++j) {
                const Containers::StringView meshConverterName = args.arrayValue<Containers::StringView>("mesh-converter", j);
                if(args.isSet("verbose")) {
                    Debug d;
                    d << "Processing mesh" << i;
                    if(meshConverterCount > 1)
                        d << "(" << Debug::nospace << (j+1) << Debug::nospace << "/" << Debug::nospace << meshConverterCount << Debug::nospace << ")";
                    d << "with" << meshConverterName << Debug::nospace << "...";
                }

                Containers::Pointer<Trade::AbstractSceneConverter> meshConverter = converterManager.loadAndInstantiate(meshConverterName);
                if(!meshConverter) {
                    Debug{} << "Available mesh converter plugins:" << ", "_s.join(converterManager.aliasList());
                    return 2;
                }

                /* Set options, if passed. The AnySceneConverter check makes no
                   sense here, is just there because the helper wants it */
                if(args.isSet("verbose")) meshConverter->addFlags(Trade::SceneConverterFlag::Verbose);
                if(j < args.arrayValueCount("mesh-converter-options"))
                    Implementation::setOptions(*meshConverter, "AnySceneConverter", args.arrayValue("mesh-converter-options", j));

                if(!(meshConverter->features() & (Trade::SceneConverterFeature::ConvertMesh))) {
                    Error{} << meshConverterName << "doesn't support mesh conversion, only" << Debug::packed << meshConverter->features();
                    return 1;
                }

                /** @todo handle mesh levels here, once any plugin is capable
                    of converting them */
                if(Containers::Optional<Trade::MeshData> converted = meshConverter->convert(*mesh)) {
                    mesh = Utility::move(converted);
                } else if(passthroughOnConversionFailure) {
                    Warning{} << "Cannot process mesh" << i << "with" << meshConverterName << Debug::nospace << ", passing the original through";
                } else {
                    Error{} << "Cannot process mesh" << i << "with" << meshConverterName;
                    return 1;
                }
            }

            arrayAppend(meshes, *Utility::move(mesh));
        }
    }

    /* Operations to perform on all materials in the importer. If there are
       any, materials are supplied manually to the converter from the array
       below. */
    Containers::Array<Trade::MaterialData> materials;
    if(args.isSet("phong-to-pbr") ||
       args.isSet("remove-duplicate-materials"))
    {
        arrayReserve(materials, importer->materialCount());

        for(UnsignedInt i = 0; i != importer->materialCount(); ++i) {
            Containers::Optional<Trade::MaterialData> material;
            {
                Trade::Implementation::Duration d{importConversionTime};
                if(!(material = importer->material(i))) {
                    Error{} << "Cannot import material" << i;
                    return 1;
                }
            }

            /* Phong to PBR conversion */
            if(args.isSet("phong-to-pbr")) {
                if(args.isSet("verbose"))
                    Debug{} << "Converting material" << i << "to PBR";

                Trade::Implementation::Duration d{conversionTime};
                /** @todo make the flags configurable as well? then the below
                    assert can actually fire, convert to a runtime error */
                material = MaterialTools::phongToPbrMetallicRoughness(*material, MaterialTools::PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes);
                CORRADE_INTERNAL_ASSERT(material);
            }

            arrayAppend(materials, *Utility::move(material));
        }

        /* Duplicate removal */
        if(args.isSet("remove-duplicate-materials")) {
            Trade::Implementation::Duration d{conversionTime};

            Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> mapping = MaterialTools::removeDuplicatesInPlace(materials);
            if(args.isSet("verbose"))
                Debug{} << "Duplicate material removal:" << materials.size() << "->" << mapping.second() << "materials";

            arrayRemoveSuffix(materials, materials.size() - mapping.second());

            /* Remap scene material references. The scenes should have been
               imported for --remove-duplicate-materials above already. */
            CORRADE_INTERNAL_ASSERT(scenes.size() == importer->sceneCount());
            for(Trade::SceneData& scene: scenes) {
                if(const Containers::Optional<UnsignedInt> materialFieldId = scene.findFieldId(Trade::SceneField::MeshMaterial)) {
                    /** @todo handle a case with immutable scene data, once it
                        exists (PrimitiveImporter is closest, but it doesn't
                        have materials so it never enters this branch) */

                    /* Deduplication makes the material index range smaller, so
                       we can map them in-place without having to worry that
                       the new indices won't fit into existing packed types */
                    SceneTools::mapIndexFieldInPlace(scene, *materialFieldId, mapping.first());
                }
            }
        }
    }

    /* Assume there's always one passed --converter option less, and the last
       is implicitly AnySceneConverter. All converters except the last one are
       expected to support Convert{Mesh,Multiple} and the mesh/scene is "piped"
       from one to the other. If the last converter supports
       Convert{Mesh,Multiple}ToFile instead of Convert{Mesh,Multiple}, it's
       used instead of the last implicit AnySceneConverter. */
    for(std::size_t i = 0, converterCount = args.arrayValueCount("converter"); i <= converterCount; ++i) {
        /* Load converter plugin */
        const Containers::StringView converterName = i == converterCount ?
            "AnySceneConverter"_s : args.arrayValue<Containers::StringView>("converter", i);
        Containers::Pointer<Trade::AbstractSceneConverter> converter = converterManager.loadAndInstantiate(converterName);
        if(!converter) {
            Debug{} << "Available converter plugins:" << ", "_s.join(converterManager.aliasList());
            return 2;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) converter->addFlags(Trade::SceneConverterFlag::Verbose);
        if(i < args.arrayValueCount("converter-options"))
            Implementation::setOptions(*converter, "AnySceneConverter", args.arrayValue("converter-options", i));

        /* Decide if this is the last converter, capable of saving to a file */
        const bool isLastConverter = i + 1 >= converterCount && (converter->features() & (Trade::SceneConverterFeature::ConvertMeshToFile|Trade::SceneConverterFeature::ConvertMultipleToFile));

        /* No verbose output for just one converter */
        if(converterCount > 1 && args.isSet("verbose")) {
            if(isLastConverter) {
                Debug{} << "Saving output (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";
            } else {
                CORRADE_INTERNAL_ASSERT(i < converterCount);
                Debug{} << "Processing (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";
            }
        }

        /* This is the last --converter (or the implicit AnySceneConverter at
           the end), output to a file */
        if(isLastConverter) {
            {
                Trade::Implementation::Duration d{conversionTime};
                if(!converter->beginFile(args.value("output"))) {
                    Error{} << "Cannot begin conversion of file" << args.value("output");
                    return 1;
                }
            }

        /* This is not the last converter, expect that it's capable of
           converting to an importer instance (or a MeshData wrapped in an
           importer instance) */
        } else {
            if(!(converter->features() & (Trade::SceneConverterFeature::ConvertMesh|Trade::SceneConverterFeature::ConvertMultiple))) {
                Error{} << converterName << "doesn't support importer conversion, only" << Debug::packed << converter->features();
                return 6;
            }

            {
                Trade::Implementation::Duration d{conversionTime};
                if(!converter->begin()) {
                    Error{} << "Cannot begin importer conversion";
                    return 1;
                }
            }
        }

        /* Contents to convert, by default all of them */
        /** @todo make it possible to filter this on the command line, once the
            converters receive this for SceneData, MaterialData and TextureData
            as well */
        /** @todo and then also test all branching on Names in direct image,
            mesh, material and scene import */
        Trade::SceneContents contents = ~Trade::SceneContents{};

        /* If there are any loose images from previous conversion steps, add
           them directly, and clear the array so the next iteration (if any)
           takes them from the importer instead */
        /** @todo 1D images, once there's any format that supports them */
        if(images2D) {
            if(!(Trade::sceneContentsFor(*converter) & Trade::SceneContent::Images2D)) {
                Warning{} << "Ignoring" << images2D.size() << "2D images not supported by the converter";
            } else for(UnsignedInt j = 0; j != images2D.size(); ++j) {
                Trade::Implementation::Duration d{conversionTime};
                if(!converter->add(images2D[j], contents & Trade::SceneContent::Names ? importer->image2DName(j) : Containers::String{})) {
                    Error{} << "Cannot add 2D image" << j;
                    return 1;
                }
            }

            /* Ensure the images are not added by addSupportedImporterContents()
               below. Do this also in case the converter actually doesn't
               support image addition, as it would otherwise cause two warnings
               about the same thing being printed. */
            contents &= ~Trade::SceneContent::Images2D;

            /* Delete the list to avoid adding them again for the next
               converter (at which point they would be stale) */
            /** @todo this line is untested, needs first an importer->importer
                converter supporting images */
            images2D = {};
        }
        if(images3D) {
            if(!(Trade::sceneContentsFor(*converter) & Trade::SceneContent::Images3D)) {
                Warning{} << "Ignoring" << images3D.size() << "3D images not supported by the converter";
            } else for(UnsignedInt j = 0; j != images3D.size(); ++j) {
                Trade::Implementation::Duration d{conversionTime};
                if(!converter->add(images3D[j], contents & Trade::SceneContent::Names ? importer->image3DName(j) : Containers::String{})) {
                    Error{} << "Cannot add 3D image" << j;
                    return 1;
                }
            }

            /* Ensure the images are not added by addSupportedImporterContents()
               below. Do this also in case the converter actually doesn't
               support image addition, as it would otherwise cause two warnings
               about the same thing being printed. */
            contents &= ~Trade::SceneContent::Images3D;

            /* Delete the list to avoid adding them again for the next
               converter (at which point they would be stale) */
            /** @todo this line is untested, needs first an importer->importer
                converter supporting images */
            images3D = {};
        }

        /* If there are any loose meshes from previous conversion steps, add
           them directly, and clear the array so the next iteration (if any)
           takes them from the importer instead */
        if(meshes) {
            if(!(Trade::sceneContentsFor(*converter) & Trade::SceneContent::Meshes)) {
                /** @todo test this branch once there's a plugin that doesn't
                    support meshes (URDF exporter, for example? glXF?) */
                Warning{} << "Ignoring" << meshes.size() << "meshes not supported by the converter";
            } else for(UnsignedInt j = 0; j != meshes.size(); ++j) {
                Trade::Implementation::Duration d{conversionTime};

                const Trade::MeshData& mesh = meshes[j];

                /* Propagate custom attribute names, skip ones that are empty.
                   Compared to data names this is done always to avoid
                   information loss. */
                for(UnsignedInt k = 0; k != mesh.attributeCount(); ++k) {
                    /** @todo have some kind of a map to not have to query the
                        same custom attribute again for each mesh */
                    const Trade::MeshAttribute name = mesh.attributeName(k);
                    if(!isMeshAttributeCustom(name)) continue;
                    /* The expectation here is that the meshes are coming from
                       the importer instance. If --mesh or --concatenate-meshes
                       was used, the original importer is replaced a new one
                       containing just one mesh, so in that case it works
                       too. */
                    if(const Containers::String nameString = importer->meshAttributeName(name)) {
                        converter->setMeshAttributeName(name, nameString);
                    }
                }

                if(!converter->add(mesh, contents & Trade::SceneContent::Names ? importer->meshName(j) : Containers::String{})) {
                    Error{} << "Cannot add mesh" << j;
                    return 1;
                }
            }

            /* Ensure the meshes are not added by addSupportedImporterContents()
               below. Do this also in case the converter actually doesn't
               support mesh addition, as it would otherwise cause two warnings
               about the same thing being printed. */
            contents &= ~Trade::SceneContent::Meshes;

            /* Delete the list to avoid adding them again for the next
               converter (at which point they would be stale) */
            /** @todo this line is untested, needs two chained conversion steps
                that each change the output to verify the old meshes don't get
                reused in the next step again */
            meshes = {};
        }

        /* If there are any loose materials from previous conversion steps, add
           them directly, and clear the array so the next iteration (if any)
           takes them from the importer instead */
        if(materials) {
            /* Materials reference textures (and those reference images), thus
               we need to add those first */
            {
                const Trade::SceneContents materialDependencies = contents &
                    (Trade::SceneContent::Images1D|
                     Trade::SceneContent::Images2D|
                     Trade::SceneContent::Images3D|
                     Trade::SceneContent::ImageLevels|
                     Trade::SceneContent::Textures|
                     Trade::SceneContent::Names);

                Trade::Implementation::Duration d{importConversionTime};
                if(!converter->addSupportedImporterContents(*importer, materialDependencies)) {
                    Error{} << "Cannot add material dependencies";
                    return 5;
                }

                /* Ensure these are not added by addSupportedImporterContents()
                   again below, except for names -- those should be added as
                   long as they were in the contents originally. */
                contents &= ~(materialDependencies & ~Trade::SceneContent::Names);
            }

            if(!(Trade::sceneContentsFor(*converter) & Trade::SceneContent::Materials)) {
                Warning{} << "Ignoring" << materials.size() << "materials not supported by the converter";
            } else for(UnsignedInt j = 0; j != materials.size(); ++j) {
                Trade::Implementation::Duration d{conversionTime};

                if(!converter->add(materials[j], contents & Trade::SceneContent::Names ? importer->materialName(j) : Containers::String{})) {
                    Error{} << "Cannot add material" << j;
                    return 1;
                }
            }

            /* Ensure the materials are not added by
               addSupportedImporterContents() below. Do this also in case the
               converter actually doesn't support mesh addition, as it would
               otherwise cause two warnings about the same "not supported"
               thing being printed. */
            contents &= ~Trade::SceneContent::Materials;

            /* Delete the list to avoid adding them again for the next
               converter (at which point they would be stale) */
            /** @todo this line is untested, needs two chained conversion steps
                that each change the output to verify the old materials don't
                get reused in the next step again */
            materials = {};
        }

        /* If there are any loose scenes from previous conversion steps, add
           them directly, and clear the array so the next iteration (if any)
           takes them from the importer instead */
        if(scenes) {
            /* Scenes may reference almost everything else except skins an
               animations (which reference scenes instead), thus we need to add
               all that first */
            {
                const Trade::SceneContents sceneDependencies = contents &
                    ~(Trade::SceneContent::Skins2D|
                      Trade::SceneContent::Skins3D|
                      Trade::SceneContent::Scenes|
                      Trade::SceneContent::Animations);

                Trade::Implementation::Duration d{importConversionTime};
                if(!converter->addSupportedImporterContents(*importer, sceneDependencies)) {
                    Error{} << "Cannot add scene dependencies";
                    return 5;
                }

                /* Ensure these are not added by addSupportedImporterContents()
                   again below, except for names -- those should be added as
                   long as they were in the contents originally. */
                contents &= ~(sceneDependencies & ~Trade::SceneContent::Names);
            }

            if(!(Trade::sceneContentsFor(*converter) & Trade::SceneContent::Scenes)) {
                Warning{} << "Ignoring" << scenes.size() << "scenes not supported by the converter";
            } else for(UnsignedInt j = 0; j != scenes.size(); ++j) {
                Trade::Implementation::Duration d{conversionTime};

                if(!converter->add(scenes[j], contents & Trade::SceneContent::Names ? importer->sceneName(j) : Containers::String{})) {
                    Error{} << "Cannot add scene" << j;
                    return 1;
                }
            }

            /* Ensure the scenes are not added by
               addSupportedImporterContents() below. Do this also in case the
               converter actually doesn't support scene addition, as it would
               otherwise cause two warnings about the same "not supported"
               thing being printed. */
            contents &= ~Trade::SceneContent::Scenes;

            /* Delete the list to avoid adding them again for the next
               converter (at which point they would be stale) */
            /** @todo this line is untested, needs two chained conversion steps
                that each change the output to verify the old scenes don't get
                reused in the next step again */
            scenes = {};
        }

        {
            Trade::Implementation::Duration d{importConversionTime};
            if(!converter->addSupportedImporterContents(*importer, contents)) {
                Error{} << "Cannot add importer contents";
                return 5;
            }
        }

        /* This is the last --converter (or the implicit AnySceneConverter at
           the end), end the file and exit the loop */
        if(isLastConverter) {
            Trade::Implementation::Duration d{conversionTime};
            if(!converter->endFile()) {
                Error{} << "Cannot end conversion of file" << args.value("output");
                return 5;
            }

            break;

        /* This is not the last converter, save the resulting importer instance
           for the next loop iteration. By design, the importer should not
           depend on any data from the converter instance, only on the
           converter plugin, so we should be fine replacing the converter with
           a different one in the next iteration and keeping just the importer
           returned from it. */
        } else {
            Trade::Implementation::Duration d{conversionTime};
            if(!(importer = converter->end())) {
                Error{} << "Cannot end importer conversion";
                return 1;
            }
        }
    }

    if(args.isSet("profile")) {
        Debug{} << "Import and conversion took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importConversionTime).count())/1.0e3f << "seconds, conversion"
            << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count())/1.0e3f << "seconds";
    }
}
