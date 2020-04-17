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

#include <algorithm>
#include <chrono>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/Implementation/converterUtilities.h"

namespace Magnum {

/** @page magnum-sceneconverter Scene conversion utility
@brief Converts scenes of different formats
@m_since_latest

@m_footernavigation
@m_keywords{magnum-sceneconverter sceneconverter}

This utility is built if both `WITH_TRADE` and `WITH_SCENECONVERTER` is enabled
when building Magnum. To use this utility with CMake, you need to request the
`sceneconverter` component of the `Magnum` package and use the
`Magnum::sceneconverter` target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED imageconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::sceneconverter ...)
@endcode

See @ref building, @ref cmake and the @ref Trade namespace for more
information.

@section magnum-sceneconverter-usage Usage

@code{.sh}
magnum-sceneconverter [-h|--help] [--importer IMPORTER] [--converter CONVERTER]
    [--plugin-dir DIR] [-i|--importer-options key=val,key2=val2,…]
    [-c|--converter-options key=val,key2=val2,…] [--info] [-v|--verbose]
    [--profile] [--] input output
@endcode

Arguments:

-   `input` --- input file
-   `output` --- output file
-   `-h`, `--help` --- display this help message and exit
-   `--importer IMPORTER` --- scene importer plugin (default:
    @ref Trade::AnySceneImporter "AnySceneImporter")
-   `--converter CONVERTER` --- scene converter plugin (default:
    @ref Trade::AnyImageConverter "AnySceneConverter")
-   `--plugin-dir DIR` --- override base plugin dir
-   `-i`, `--importer-options key=val,key2=val2,…` --- configuration options to
    pass to the importer
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to the converter
-   `--info` --- print info about the input file and exit
-   `-v`, `--verbose` --- verbose output from importer plugins
-   `--profile` --- measure import and conversion time

If `--info` is given, the utility will print information about all meshes
and images present in the file. **This option is currently mandatory.**

The `-i` / `--importer-options` and `-c` / `--converter-options` arguments
accept a comma-separated list of key/value pairs to set in the importer /
converter plugin configuration. If the `=` character is omitted, it's
equivalent to saying `key=true`; configuration subgroups are delimited with
`/`.

@see @ref magnum-imageconverter
*/

}

using namespace Magnum;

namespace {

struct Duration {
    explicit Duration(std::chrono::high_resolution_clock::duration& output): _output(output), _t{std::chrono::high_resolution_clock::now()} {}

    ~Duration() {
        _output += std::chrono::high_resolution_clock::now() - _t;
    }

    private:
        std::chrono::high_resolution_clock::duration& _output;
        std::chrono::high_resolution_clock::time_point _t;
};

/* Direct shims for fast deserialization / serialization of blob data. Compared
   to MagnumImporter / MagnumSceneConverter these don't make the whole file
   resident in memory, so *much* faster. */
class BlobImporter: public Trade::AbstractImporter {
    Trade::ImporterFeatures doFeatures() const override { return {}; }

    bool doIsOpened() const override { return _in; }
    void doClose() override { _in = nullptr; }
    void doOpenFile(const std::string& filename) override {
        _in = Utility::Directory::mapRead(filename);
    }

    UnsignedInt doMeshCount() const override { return 1; }
    Containers::Optional<Trade::MeshData> doMesh(UnsignedInt, UnsignedInt) override {
        /* GCC 4.8 and old Clang has problems with an implicit cast here */
        return Trade::MeshData::deserialize(Containers::ArrayView<const void>(_in));
    }

    Containers::Array<const char, Utility::Directory::MapDeleter> _in;
};

class BlobSceneConverter: public Trade::AbstractSceneConverter {
    Trade::SceneConverterFeatures doFeatures() const override {
        return Trade::SceneConverterFeature::ConvertMeshToFile;
    }

    bool doConvertToFile(const std::string& filename, const Magnum::Trade::MeshData& mesh) override {
        Containers::Array<char, Utility::Directory::MapDeleter> out = Utility::Directory::mapWrite(filename, mesh.serializedSize());
        if(!out) return false;
        mesh.serializeInto(out);
        return true;
    }
};

}

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input file")
        .addArgument("output").setHelp("output", "output file")
        .addOption("importer", "AnySceneImporter").setHelp("importer", "scene importer plugin")
        .addOption("converter", "AnySceneConverter").setHelp("converter", "scene converter plugin")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter", "key=val,key2=val2,…")
        .addBooleanOption("info").setHelp("info", "print info about the input file and exit")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from importer and converter plugins")
        .addBooleanOption("profile").setHelp("profile", "measure import and conversion time")
        .setParseErrorCallback([](const Utility::Arguments& args, Utility::Arguments::ParseError error, const std::string& key) {
            /* If --info is passed, we don't need the output argument */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
                key == "output" && args.isSet("info")) return true;

            /* Handle all other errors as usual */
            return false;
        })
        .setGlobalHelp(R"(Converts scenes of different formats.

If --info is given, the utility will print information about all meshes and
images present in the file.

The -i / --importer-options and -c / --converter-options arguments accept a
comma-separated list of key/value pairs to set in the importer / converter
plugin configuration. If the = character is omitted, it's equivalent to saying
key=true; configuration subgroups are delimited with /.)")
        .parse(argc, argv);

    /* Load importer plugin, or use the blob shim in case the extension
       matches and we're not overriding the converter to something specific */
    Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> importerManager;
    Containers::Pointer<Trade::AbstractImporter> importer;
    if(Utility::String::endsWith(args.value("input"), ".blob") && args.value("importer") == "AnySceneImporter") {
        importer.reset(new BlobImporter);
        if(!args.value("importer-options").empty())
            Warning{} << "Importer options" << args.value("importer-options") << "ignored when loading a blob file";
    } else {
        importerManager.emplace(
            args.value("plugin-dir").empty() ? std::string{} :
            Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImporter::pluginSearchPaths()[0]));
        importer = importerManager->loadAndInstantiate(args.value("importer"));
        if(!importer) {
            Debug{} << "Available importer plugins:" << Utility::String::join(importerManager->aliasList(), ", ");
            return 1;
        }

        if(args.isSet("verbose")) importer->setFlags(Trade::ImporterFlag::Verbose);
        Trade::Implementation::setOptions(*importer, args.value("importer-options"));
    }

    std::chrono::high_resolution_clock::duration importTime;

    /* Open the file */
    {
        Duration d{importTime};
        if(!importer->openFile(args.value("input"))) {
            Error() << "Cannot open file" << args.value("input");
            return 3;
        }
    }

    /* Print file info, if requested */
    if(args.isSet("info")) {
        if(!importer->meshCount() && !importer->image1DCount() && !importer->image2DCount() && !importer->image2DCount()) {
            Debug{} << "No meshes or images found.";
            return 0;
        }

        struct MeshAttributeInfo {
            std::size_t offset;
            UnsignedInt stride, arraySize;
            Trade::MeshAttribute name;
            std::string customName;
            VertexFormat format;
        };

        struct MeshInfo {
            UnsignedInt mesh, level;
            MeshPrimitive primitive;
            UnsignedInt indexCount, vertexCount;
            MeshIndexType indexType;
            Containers::Array<MeshAttributeInfo> attributes;
            std::size_t indexDataSize, vertexDataSize;
            std::string name;
        };

        /* Parse everything first to avoid errors interleaved with output */
        bool error = false;
        Containers::Array<MeshInfo> meshInfos;
        for(UnsignedInt i = 0; i != importer->meshCount(); ++i) {
            for(UnsignedInt j = 0; j != importer->meshLevelCount(i); ++j) {
                Containers::Optional<Trade::MeshData> mesh;
                {
                    Duration d{importTime};
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
                if(!j) info.name = importer->meshName(i);
                if(mesh->isIndexed()) {
                    info.indexCount = mesh->indexCount();
                    info.indexType = mesh->indexType();
                    info.indexDataSize = mesh->indexData().size();
                }
                for(UnsignedInt k = 0; k != mesh->attributeCount(); ++k) {
                    const Trade::MeshAttribute name = mesh->attributeName(k);
                    arrayAppend(info.attributes, Containers::InPlaceInit,
                        mesh->attributeOffset(k),
                        mesh->attributeStride(k),
                        mesh->attributeArraySize(k),
                        name, Trade::isMeshAttributeCustom(name) ?
                            importer->meshAttributeName(name) : "",
                        mesh->attributeFormat(k));
                }

                std::sort(info.attributes.begin(), info.attributes.end(),
                    [](const MeshAttributeInfo& a, const MeshAttributeInfo& b) {
                        return a.offset < b.offset;
                    });

                arrayAppend(meshInfos, std::move(info));
            }
        }

        Containers::Array<Trade::Implementation::ImageInfo> imageInfos =
            Trade::Implementation::imageInfo(*importer, error);

        for(const MeshInfo& info: meshInfos) {
            Debug d;
            if(info.level == 0) {
                d << "Mesh" << info.mesh << Debug::nospace << ":";
                if(!info.name.empty()) d << info.name;
                d << Debug::newline;
            }
            d << "  Level" << info.level << Debug::nospace << ":"
                << info.primitive << Debug::nospace << "," << info.vertexCount
                << "vertices (" << Debug::nospace
                << Utility::formatString("{:.1f}", info.vertexDataSize/1024.0f)
                << "kB)";
            if(info.indexType != MeshIndexType{}) {
                d << Debug::newline << "   " << info.indexCount << "indices @"
                    << info.indexType << "(" << Debug::nospace
                    << Utility::formatString("{:.1f}", info.indexDataSize/1024.0f)
                    << "kB)";
            }

            for(const MeshAttributeInfo& attribute: info.attributes) {
                d << Debug::newline << "    Offset" << attribute.offset
                    << Debug::nospace << ":" << attribute.name;
                if(Trade::isMeshAttributeCustom(attribute.name)) {
                    d << "(" << Debug::nospace << attribute.customName
                        << Debug::nospace << ")";
                }
                d << "@" << attribute.format << Debug::nospace << ", stride"
                    << attribute.stride;
            }
        }
        for(const Trade::Implementation::ImageInfo& info: imageInfos) {
            Debug d;
            if(info.level == 0) {
                d << "Image" << info.image << Debug::nospace << ":";
                if(!info.name.empty()) d << info.name;
                d << Debug::newline;
            }
            d << "  Level" << info.level << Debug::nospace << ":";
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

    Containers::Optional<Trade::MeshData> mesh;
    {
        Duration d{importTime};
        if(!importer->meshCount() || !(mesh = importer->mesh(0))) {
            Error{} << "Cannot import mesh 0";
            return 4;
        }
    }

    /* Load converter plugin, or use the blob shim in case the extension
       matches and we're not overriding the converter to something specific */
    Containers::Optional<PluginManager::Manager<Trade::AbstractSceneConverter>> converterManager;
    Containers::Pointer<Trade::AbstractSceneConverter> converter;
    if(Utility::String::endsWith(args.value("output"), ".blob") && args.value("converter") == "AnySceneConverter") {
        converter.reset(new BlobSceneConverter);
        if(!args.value("converter-options").empty())
            Warning{} << "Converter options" << args.value("converter-options") << "ignored when writing a blob file";
    } else {
        converterManager.emplace(
            args.value("plugin-dir").empty() ? std::string{} :
            Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractSceneConverter::pluginSearchPaths()[0]));
        converter = converterManager->loadAndInstantiate(args.value("converter"));
        if(!converter) {
            Debug{} << "Available converter plugins:" << Utility::String::join(converterManager->aliasList(), ", ");
            return 2;
        }

        if(args.isSet("verbose")) converter->setFlags(Trade::SceneConverterFlag::Verbose);
        Trade::Implementation::setOptions(*converter, args.value("converter-options"));
    }

    std::chrono::high_resolution_clock::duration conversionTime;

    /* Save output file */
    {
        Duration d{conversionTime};
        if(!converter->convertToFile(args.value("output"), *mesh)) {
            Error{} << "Cannot save file" << args.value("output");
            return 5;
        }
    }

    if(args.isSet("profile")) {
        Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds, conversion"
            << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count())/1.0e3f << "seconds";
    }
}
