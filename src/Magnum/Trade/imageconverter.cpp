/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Arguments is std::string-free */
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Implementation/converterUtilities.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/Implementation/converterUtilities.h"

namespace Magnum {

/** @page magnum-imageconverter Image conversion utility
@brief Converts images of different formats

@m_footernavigation
@m_keywords{magnum-imageconverter imageconverter}

This utility is built if `WITH_IMAGECONVERTER` is enabled when building Magnum.
To use this utility with CMake, you need to request the `imageconverter`
component of the `Magnum` package and use the `Magnum::imageconverter` target
for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED imageconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::imageconverter ...)
@endcode

See @ref building, @ref cmake and the @ref Trade namespace for more
information.

@section magnum-imageconverter-usage Usage

@code{.sh}
magnum-imageconverter [-h|--help] [-I|--importer PLUGIN]
    [-C|--converter PLUGIN] [--plugin-dir DIR] [--map]
    [-i|--importer-options key=val,key2=val2,…]
    [-c|--converter-options key=val,key2=val2,…] [-D|--dimensions N]
    [--image N] [--level N] [--layer N] [--layers] [--levels] [--in-place]
    [--info] [--color on|off|auto] [-v|--verbose] [--profile] [--] input output
@endcode

Arguments:

-   `input` --- input image
-   `output` --- output image; ignored if `--info` is present, disallowed for
    `--in-place`
-   `-h`, `--help` --- display this help message and exit
-   `-I`, `--importer PLUGIN` --- image importer plugin (default:
    @ref Trade::AnyImageImporter "AnyImageImporter")
-   `-C`, `--converter PLUGIN` --- image converter plugin (default:
    @ref Trade::AnyImageConverter "AnyImageConverter")
-   `--plugin-dir DIR` --- override base plugin dir
-   `--map` --- memory-map the input for zero-copy import (works only for
    standalone files)
-   `-i`, `--importer-options key=val,key2=val2,…` --- configuration options to
    pass to the importer
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to the converter
-   `-D`, `--dimensions N` --- import and convert image of given dimensions
    (default: `2`)
-   `--image N` --- image to import (default: `0`)
-   `--level N` --- import given image level instead of all
-   `--layer N` --- extract a layer into an image with one dimension less
-   `--layers` --- combine multiple layers into an image with one dimension
    more
-   `--levels` --- combine multiple image levels into a single file
-   `--in-place` --- overwrite the input image with the output
-   `--info` --- print info about the input file and exit
-   `--color` --- colored output for `--info` (default: `auto`)
-   `-v`, `--verbose` --- verbose output from importer and converter plugins
-   `--profile` --- measure import and conversion time

Specifying `--importer raw:&lt;format&gt;` will treat the input as a raw
tightly-packed square of pixels in given @ref PixelFormat. Specifying `-C` /
`--converter raw` will save raw imported data instead of using a converter
plugin.

If `--info` is given, the utility will print information about all images
present in the file, independently of the `-D` / `--dimensions` option. In this
case no conversion is done and output file doesn't need to be specified.

The `-i` / `--importer-options` and `-c` / `--converter-options` arguments
accept a comma-separated list of key/value pairs to set in the importer /
converter plugin configuration. If the `=` character is omitted, it's
equivalent to saying `key=true`; configuration subgroups are delimited with
`/`.

@section magnum-imageconverter-example Usage examples

Converting a JPEG file to a PNG:

@code{.sh}
magnum-imageconverter image.jpg image.png
@endcode

Creating a JPEG file with 95% quality from a PNG, by setting a plugin-specific
configuration option. The @relativeref{Trade,AnyImageConverter} plugin is
implicitly used and it proxies the option to either
@relativeref{Trade,JpegImageConverter} or
@relativeref{Trade,StbImageConverter}, depending on which one is available:

@code{.sh}
magnum-imageconverter image.png image.jpg -c jpegQuality=0.95
@endcode

Extracting raw (uncompressed, compressed) data from a DDS file for manual
inspection:

@code{.sh}
magnum-imageconverter image.dds --converter raw data.dat
@endcode

Extracting an arbitrary image from a glTF file. Note that only image formats
are considered by default so you have to explicitly supply a scene importer,
either the generic @relativeref{Trade,AnySceneImporter} or for example directly
the @relativeref{Trade,TinyGltfImporter}:

@code{.shell-session}
$ # print a list of all images in the file
$ magnum-imageconverter -I AnySceneImporter --info file.gltf
2D image 0: PixelFormat::RGBA8Unorm Vector(2048, 2048)
2D image 1: PixelFormat::RGBA8Unorm Vector(2048, 2048)
2D image 2: PixelFormat::RGBA8Unorm Vector(2048, 2048)
…
$ # extract the third image to a PNG file for inspection
$ magnum-imageconverter -I AnySceneImporter --image 2 file.gltf image.png
@endcode

@subsection magnum-imageconverter-example-levels-layers Dealing with image levels and layers

Converting six 2D images to a 3D cube map file using @relativeref{Trade,OpenExrImageConverter}. Note the `-c envmap-cube` which the
plugin needs to produce an actual cube map file, the `--` is then used to avoid
`-x.exr` and others to be treated as options instead of files. On Unix shells
you could also use `./-x.exr` etc. to circumvent that ambiguity.

@code{.sh}
magnum-imageconverter --layers -c envmap=cube -- \
    +x.exr -x.exr +y.exr -y.exr +z.exr -z.exr cube.exr
@endcode

Creating a multi-level OpenEXR cube map file from a set of input files. Note
the use of `-D3` which switches to dealing with 3D images instead of 2D:

@code{.sh}
magnum-imageconverter --levels -c envmap=cube -D3 \
    cube-256.exr cube-128.exr cube-64.exr cube-mips.exr
@endcode

Extracting the second level of a +Y face (third layer) of the above cube map
file again:

@code{.sh}
magnum-imageconverter cube-mips.exr --layer 2 --level 1 +x-128.exr
@endcode

@see @ref magnum-sceneconverter
*/

}

using namespace Magnum;
using namespace Containers::Literals;

namespace {

template<UnsignedInt dimensions> bool checkCommonFormat(const Utility::Arguments& args, const Containers::Array<Trade::ImageData<dimensions>>& images) {
    CORRADE_INTERNAL_ASSERT(!images.isEmpty());
    const bool compressed = images.front().isCompressed();
    PixelFormat format{};
    CompressedPixelFormat compressedFormat{};
    if(!compressed) format = images.front().format();
    else compressedFormat = images.front().compressedFormat();
    for(std::size_t i = 1; i != images.size(); ++i) {
        if(images[i].isCompressed() != compressed ||
           (!compressed && images[i].format() != format) ||
           (compressed && images[i].compressedFormat() != compressedFormat))
        {
            Error e;
            e << "Images have different formats," << args.arrayValue("input", i) << "has";
            if(images[i].isCompressed())
                e << images[i].compressedFormat();
            else
                e << images[i].format();
            e << Debug::nospace << ", expected";
            if(compressed)
                e << compressedFormat;
            else
                e << format;
            return false;
        }
    }

    return true;
}

template<UnsignedInt dimensions> bool checkCommonFormatAndSize(const Utility::Arguments& args, const Containers::Array<Trade::ImageData<dimensions>>& images) {
    if(!checkCommonFormat(args, images)) return false;

    CORRADE_INTERNAL_ASSERT(!images.isEmpty());
    Math::Vector<dimensions, Int> size = images.front().size();
    for(std::size_t i = 1; i != images.size(); ++i) {
        if(images[i].size() != size) {
            Error{} << "Images have different sizes," << args.arrayValue("input", i) << "has a size of" << images[i].size() << Debug::nospace << ", expected" << size;
            return false;
        }
    }

    return true;
}

template<template<UnsignedInt, class> class View, UnsignedInt dimensions> bool convertOneOrMoreImages(Trade::AbstractImageConverter& converter, const Containers::Array<Trade::ImageData<dimensions>>& outputImages, const Containers::StringView output) {
    Containers::Array<View<dimensions, const char>> views;
    arrayReserve(views, outputImages.size());
    for(const Trade::ImageData<dimensions>& outputImage: outputImages)
        arrayAppend(views, View<dimensions, const char>{outputImage});
    return converter.convertToFile(views, output);
}

template<UnsignedInt dimensions> bool convertOneOrMoreImages(Trade::AbstractImageConverter& converter, const Containers::Array<Trade::ImageData<dimensions>>& outputImages, const Containers::StringView output) {
    /* If there's just one image, convert it using the single-level API.
       Otherwise the multi-level entrypoint would require the plugin to support
       multi-level conversion, and only some file formats have that. */
    if(outputImages.size() == 1)
        return converter.convertToFile(outputImages.front(), output);

    CORRADE_INTERNAL_ASSERT(!outputImages.isEmpty());
    if(outputImages.front().isCompressed())
        return convertOneOrMoreImages<CompressedImageView, dimensions>(converter, outputImages, output);
    else
        return convertOneOrMoreImages<ImageView, dimensions>(converter, outputImages, output);
}

}

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArrayArgument("input").setHelp("input", "input image(s)")
        .addArgument("output").setHelp("output", "output image; ignored if --info is present, disallowed for --in-place")
        .addOption('I', "importer", "AnyImageImporter").setHelp("importer", "image importer plugin", "PLUGIN")
        .addOption('C', "converter", "AnyImageConverter").setHelp("converter", "image converter plugin", "PLUGIN")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
        .addBooleanOption("map").setHelp("map", "memory-map the input for zero-copy import (works only for standalone files)")
        #endif
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter", "key=val,key2=val2,…")
        .addOption('D', "dimensions", "2").setHelp("dimensions", "import and convert image of given dimensions", "N")
        .addOption("image", "0").setHelp("image", "image to import", "N")
        .addOption("level").setHelp("level", "import given image level instead of all", "N")
        .addOption("layer").setHelp("layer", "extract a layer into an image with one dimension less", "N")
        .addBooleanOption("layers").setHelp("layers", "combine multiple layers into an image with one dimension more")
        .addBooleanOption("levels").setHelp("layers", "combine multiple image levels into a single file")
        .addBooleanOption("in-place").setHelp("in-place", "overwrite the input image with the output")
        .addBooleanOption("info").setHelp("info", "print info about the input file and exit")
        .addOption("color", "auto").setHelp("color", "colored output for --info", "on|off|auto")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from importer and converter plugins")
        .addBooleanOption("profile").setHelp("profile", "measure import and conversion time")
        .setParseErrorCallback([](const Utility::Arguments& args, Utility::Arguments::ParseError error, const std::string& key) {
            /* If --in-place or --info is passed, we don't need the output
               argument */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
               key == "output" && (args.isSet("in-place") || args.isSet("info")))
                return true;

            /* Handle all other errors as usual */
            return false;
        })
        .setGlobalHelp(R"(Converts images of different formats.

Specifying --importer raw:<format> will treat the input as a raw tightly-packed
square of pixels in given pixel format. Specifying -C / --converter raw will
save raw imported data instead of using a converter plugin.

If --info is given, the utility will print information about all images present
in the file, independently of the -D / --dimensions option. In this case no
conversion is done and output file doesn't need to be specified.

The -i / --importer-options and -c / --converter-options arguments accept a
comma-separated list of key/value pairs to set in the importer / converter
plugin configuration. If the = character is omitted, it's equivalent to saying
key=true; configuration subgroups are delimited with /.)")
        .parse(argc, argv);

    /* Generic checks */
    if(!args.value<Containers::StringView>("output").isEmpty()) {
        if(args.isSet("in-place")) {
            Error{} << "Output file shouldn't be set for --in-place:" << args.value<Containers::StringView>("output");
            return 1;
        }

        /* Not an error in this case, it should be possible to just append
           --info to existing command line without having to remove anything.
           But print a warning at least, it could also be a mistyped option. */
        if(args.isSet("info"))
            Warning{} << "Ignoring output file for --info:" << args.value<Containers::StringView>("output");
    }

    /* Mutually incompatible options */
    if(args.isSet("layers") && args.isSet("levels")) {
        Error{} << "The --layers and --levels options can't be used together. First combine layers of each level and then all levels in a second step.";
        return 1;
    }
    if((args.isSet("layers") || args.isSet("levels")) && args.isSet("in-place")) {
        Error{} << "The --layers / --levels option can't be combined with --in-place";
        return 1;
    }
    if((args.isSet("layers") || args.isSet("levels")) && args.isSet("info")) {
        Error{} << "The --layers / --levels option can't be combined with --info";
        return 1;
    }
    /* It can be combined with --levels though. This could potentially be
       possible to implement, but I don't see a reason, all it would do is
       picking Nth image from the input set and recompress it. OTOH, combining
       --levels and --level "works", the --level picks Nth level from each
       input image, although the usefulness of that is also doubtful. Why
       create multi-level images from images that are already multi-level? */
    if(args.isSet("layers") && !args.value("layer").empty()) {
        Error{} << "The --layers option can't be combined with --layer.";
        return 1;
    }
    if(args.isSet("levels") && args.value("converter") == "raw") {
        Error{} << "The --levels option can't be combined with raw data output";
        return 1;
    }
    if(!args.isSet("layers") && !args.isSet("levels") && args.arrayValueCount("input") > 1) {
        Error{} << "Multiple input files require the --layers / --levels option to be set";
        return 1;
    }

    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Trade::AbstractImporter::pluginSearchPaths().back())};

    const Int dimensions = args.value<Int>("dimensions");
    /** @todo make them array options as well? */
    const UnsignedInt image = args.value<UnsignedInt>("image");
    Containers::Optional<UnsignedInt> level;
    if(!args.value("level").empty()) level = args.value<UnsignedInt>("level");
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
    Containers::Array<Containers::Array<const char, Utility::Path::MapDeleter>> mapped;
    #endif
    Containers::Array<Trade::ImageData1D> images1D;
    Containers::Array<Trade::ImageData2D> images2D;
    Containers::Array<Trade::ImageData3D> images3D;

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration importTime{};

    for(std::size_t i = 0, max = args.arrayValueCount("input"); i != max; ++i) {
        const Containers::StringView input = args.arrayValue<Containers::StringView>("input", i);

        /* Load raw data, if requested; assume it's a tightly-packed square of
           given format */
        /** @todo implement image slicing and then use `--slice "0 0 w h"` to
            specify non-rectangular size (and +x +y to specify padding?) */
        if(args.value<Containers::StringView>("importer").hasPrefix("raw:"_s)) {
            if(dimensions != 2) {
                Error{} << "Raw data inputs can be only used for 2D images";
                return 1;
            }

            /** @todo Any chance to do this without using internal APIs? */
            const PixelFormat format = Utility::ConfigurationValue<PixelFormat>::fromString(args.value("importer").substr(4), {});
            const UnsignedInt pixelSize = Magnum::pixelSize(format);
            if(format == PixelFormat{}) {
                Error{} << "Invalid raw pixel format" << args.value("importer");
                return 4;
            }

            /* Read the file or map it if requested */
            Containers::Array<char> data;
            #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
            if(args.isSet("map")) {
                arrayAppend(mapped, InPlaceInit);

                Trade::Implementation::Duration d{importTime};
                Containers::Optional<Containers::Array<const char, Utility::Path::MapDeleter>> mappedMaybe = Utility::Path::mapRead(input);
                if(!mappedMaybe) {
                    Error() << "Cannot memory-map file" << input;
                    return 3;
                }

                /* Fake a mutable array with a non-owning deleter to have the
                   same type as from Path::read(). The actual memory is owned
                   by the `mapped` array. */
                mapped.back() = *std::move(mappedMaybe);
                data = Containers::Array<char>{const_cast<char*>(mapped.back().data()), mapped.back().size(), [](char*, std::size_t){}};
            } else
            #endif
            {
                Trade::Implementation::Duration d{importTime};
                Containers::Optional<Containers::Array<char>> dataMaybe = Utility::Path::read(input);
                if(!dataMaybe) {
                    Error{} << "Cannot read file" << input;
                    return 3;
                }

                data = *std::move(dataMaybe);
            }

            auto side = Int(std::sqrt(data.size()/pixelSize));
            if(data.size() % pixelSize || side*side*pixelSize != data.size()) {
                Error{} << "File of size" << data.size() << "is not a tightly-packed square of" << format;
                return 5;
            }

            /* Print image info, if requested */
            if(args.isSet("info")) {
                Debug{} << "Image 0:" << format << Vector2i{side};

                if(args.isSet("profile")) {
                    Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds";
                }

                return 0;
            }

            arrayAppend(images2D, InPlaceInit, format, Vector2i{side}, std::move(data));

        /* Otherwise load it using an importer plugin */
        } else {
            Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
            if(!importer) {
                Debug{} << "Available importer plugins:" << ", "_s.join(importerManager.aliasList());
                return 1;
            }

            /* Set options, if passed */
            if(args.isSet("verbose")) importer->addFlags(Trade::ImporterFlag::Verbose);
            Implementation::setOptions(*importer, "AnyImageImporter", args.value("importer-options"));

            /* Open the file or map it if requested */
            #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
            if(args.isSet("map")) {
                arrayAppend(mapped, InPlaceInit);

                Trade::Implementation::Duration d{importTime};
                Containers::Optional<Containers::Array<const char, Utility::Path::MapDeleter>> mappedMaybe = Utility::Path::mapRead(input);
                if(!mappedMaybe || !importer->openMemory(*mappedMaybe)) {
                    Error() << "Cannot memory-map file" << input;
                    return 3;
                }

                mapped.back() = *std::move(mappedMaybe);
            } else
            #endif
            {
                Trade::Implementation::Duration d{importTime};
                if(!importer->openFile(input)) {
                    Error{} << "Cannot open file" << input;
                    return 3;
                }
            }

            /* Print image info, if requested. This is always done for just one
               file, checked above. */
            if(args.isSet("info")) {
                /* Don't fail when there's no image -- we could be asking for
                   info on a scene file without images, after all */
                if(!importer->image1DCount() && !importer->image2DCount() && !importer->image3DCount()) {
                    Debug{} << "No images found in" << input;
                    return 0;
                }

                /* Parse everything first to avoid errors interleaved with
                   output */
                bool error = false;
                Containers::Array<Trade::Implementation::ImageInfo> infos =
                    Trade::Implementation::imageInfo(*importer, error, importTime);

                /* Colored output. Enable only if a TTY. */
                Debug::Flags useColor;
                if(args.value("color") == "on")
                    useColor = Debug::Flags{};
                else if(args.value("color") == "off")
                    useColor = Debug::Flag::DisableColors;
                else
                    useColor = Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors;

                for(const Trade::Implementation::ImageInfo& info: infos) {
                    Debug d{useColor};
                    if(info.level == 0) {
                        d << Debug::boldColor(Debug::Color::White);
                        if(info.size.z()) d << "3D image";
                        else if(info.size.y()) d << "2D image";
                        else d << "1D image";
                        d << info.image << Debug::nospace << ":"
                            << Debug::resetColor;
                        if(info.name) d << Debug::boldColor(Debug::Color::Yellow)
                            << info.name << Debug::resetColor;
                        d << Debug::newline;
                    }
                    d << "  Level" << info.level << Debug::nospace << ":"
                        << Debug::packed;
                    if(info.size.z()) d << info.size;
                    else if(info.size.y()) d << info.size.xy();
                    else d << Math::Vector<1, Int>(info.size.x());
                    d << Debug::color(Debug::Color::Blue) << "@" << Debug::resetColor;
                    d << Debug::packed;
                    if(info.compressed) d << Debug::color(Debug::Color::Yellow) << info.compressedFormat;
                    else d << Debug::color(Debug::Color::Cyan) << info.format;
                    d << Debug::resetColor << "(" << Debug::nospace << Utility::format("{:.1f}", info.dataSize/1024.0f) << "kB";
                    if(info.dataFlags != (Trade::DataFlag::Owned|Trade::DataFlag::Mutable))
                        d << Debug::nospace << "," << Debug::packed
                            << Debug::color(Debug::Color::Green)
                            << info.dataFlags << Debug::resetColor;
                    d << Debug::nospace << ")";
                }

                if(args.isSet("profile")) {
                    Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds";
                }

                return error ? 1 : 0;
            }

            /* Bail early if there's no image whatsoever. More detailed errors
               with hints are provided for each dimension below. */
            if(!importer->image1DCount() && !importer->image2DCount() && !importer->image3DCount()) {
                Error{} << "No images found in" << input;
                return 1;
            }

            bool imported = false;
            if(dimensions == 1) {
                if(!importer->image1DCount()) {
                    Error{} << "No 1D images found in" << input << Debug::nospace << ". Specify -D2 or -D3 for 2D or 3D image conversion.";
                    return 1;
                }
                if(image >= importer->image1DCount()) {
                    Error{} << "1D image number" << image << "not found in" << input << Debug::nospace << ", the file has only" << importer->image1DCount() << "1D images";
                    return 1;
                }

                /* Import all levels of the input or just one if specified */
                UnsignedInt minLevel, maxLevel;
                if(level) {
                    minLevel = *level;
                    maxLevel = *level + 1;
                    if(*level >= importer->image1DLevelCount(image)) {
                        Error{} << "1D image" << image << "in" << input << "doesn't have a level number" << level << Debug::nospace << ", only" << importer->image1DLevelCount(image) << "levels";
                        return 1;
                    }
                } else {
                    minLevel = 0;
                    maxLevel = importer->image1DLevelCount(image);
                    if(maxLevel > 1 && (args.isSet("layers") || args.isSet("levels") || args.value("converter") == "raw")) {
                        Error{} << "Cannot use --layers / --levels or raw output with multi-level input images. Specify --level N to extract just one level from each.";
                        return 1;
                    }
                }
                for(; minLevel != maxLevel; ++minLevel) {
                    if(Containers::Optional<Trade::ImageData1D> image1D = importer->image1D(image, minLevel)) {
                        /* The --layer option is only for 2D/3D, not checking
                           any bounds here. If the option is present, the
                           extraction code below will fail. */
                        arrayAppend(images1D, std::move(*image1D));
                        imported = true;
                    }
                }

            } else if(dimensions == 2) {
                if(!importer->image2DCount()) {
                    Error{} << "No 2D images found in" << input << Debug::nospace << ". Specify -D1 or -D3 for 1D or 3D image conversion.";
                    return 1;
                }
                if(image >= importer->image2DCount()) {
                    Error{} << "2D image number" << image << "not found in" << input << Debug::nospace << ", the file has only" << importer->image2DCount() << "2D images";
                    return 1;
                }

                /* Import all levels of the input or just one if specified */
                UnsignedInt minLevel, maxLevel;
                if(level) {
                    minLevel = *level;
                    maxLevel = *level + 1;
                    if(*level >= importer->image2DLevelCount(image)) {
                        Error{} << "2D image" << image << "in" << input << "doesn't have a level number" << level << Debug::nospace << ", only" << importer->image2DLevelCount(image) << "levels";
                        return 1;
                    }
                } else {
                    minLevel = 0;
                    maxLevel = importer->image2DLevelCount(image);
                    if(maxLevel > 1 && (args.isSet("layers") || args.isSet("levels") || args.value("converter") == "raw")) {
                        Error{} << "Cannot use --layers / --levels or raw output with multi-level input images. Specify --level N to extract just one level from each.";
                        return 1;
                    }
                }
                for(; minLevel != maxLevel; ++minLevel) {
                    if(Containers::Optional<Trade::ImageData2D> image2D = importer->image2D(image, minLevel)) {
                        /* Check bounds for the --layer option here, as we
                           won't have the filename etc. later */
                        if(!args.value("layer").empty() && args.value<Int>("layer") >= image2D->size().y()) {
                            Error{} << "2D image" << image << Debug::nospace << ":" << Debug::nospace << minLevel << "in" << input << "doesn't have a layer number" << args.value<Int>("layer") << Debug::nospace << ", only" << image2D->size().y() << "layers";
                            return 1;
                        }

                        arrayAppend(images2D, std::move(*image2D));
                        imported = true;
                    }
                }

            } else if(dimensions == 3) {
                if(!importer->image3DCount()) {
                    Error{} << "No 3D images found in" << input << Debug::nospace << ". Specify -D1 or -D2 for 1D or 2D image conversion.";
                    return 1;
                }
                if(image >= importer->image3DCount()) {
                    Error{} << "3D image number" << image << "not found in" << input << Debug::nospace << ", the file has only" << importer->image3DCount() << "3D images";
                    return 1;
                }

                /* Import all levels of the input or just one if specified */
                UnsignedInt minLevel, maxLevel;
                if(level) {
                    minLevel = *level;
                    maxLevel = *level + 1;
                    if(*level >= importer->image3DLevelCount(image)) {
                        Error{} << "3D image" << image << "in" << input << "doesn't have a level number" << level << Debug::nospace << ", only" << importer->image3DLevelCount(image) << "levels";
                        return 1;
                    }
                } else {
                    minLevel = 0;
                    maxLevel = importer->image3DLevelCount(image);
                    if(maxLevel > 1 && (args.isSet("layers") || args.isSet("levels") || args.value("converter") == "raw")) {
                        Error{} << "Cannot use --layers / --levels or raw output with multi-level input images. Specify --level N to extract just one level from each.";
                        return 1;
                    }
                }
                for(; minLevel != maxLevel; ++minLevel) {
                    if(Containers::Optional<Trade::ImageData3D> image3D = importer->image3D(image, minLevel)) {
                        /* Check bounds for the --layer option here, as we
                           won't have the filename etc. later */
                        if(!args.value("layer").empty() && args.value<Int>("layer") >= image3D->size().z()) {
                            Error{} << "3D image" << image << Debug::nospace << ":" << Debug::nospace << minLevel << "in" << input << "doesn't have a layer number" << args.value<Int>("layer") << Debug::nospace << ", only" << image3D->size().z() << "layers";
                            return 1;
                        }

                        arrayAppend(images3D, std::move(*image3D));
                        imported = true;
                    }
                }

            } else {
                Error{} << "Invalid --dimensions option:" << args.value("dimensions");
                return 1;
            }

            if(!imported) {
                Error{} << "Cannot import image" << image << Debug::nospace << ":" << Debug::nospace << level << "from" << input;
                return 4;
            }
        }
    }

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration conversionTime{};

    Containers::StringView output;
    if(args.isSet("in-place")) {
        /* Should have been checked in a graceful way above */
        CORRADE_INTERNAL_ASSERT(args.arrayValueCount("input") == 1);
        output = args.arrayValue<Containers::StringView>("input", 0);
    } else output = args.value<Containers::StringView>("output");

    Int outputDimensions;
    Containers::Array<Trade::ImageData1D> outputImages1D;
    Containers::Array<Trade::ImageData2D> outputImages2D;
    Containers::Array<Trade::ImageData3D> outputImages3D;

    /* Combine multiple layers into an image of one dimension more */
    if(args.isSet("layers")) {
        /* To include allocation + copy costs in the output */
        Trade::Implementation::Duration d{conversionTime};

        if(dimensions == 1) {
            if(!checkCommonFormatAndSize(args, images1D)) return 1;

            outputDimensions = 2;
            if(!images1D.front().isCompressed()) {
                /* Allocate a new image */
                /** @todo simplify once ImageData is able to allocate on its
                    own, including correct padding etc */
                const Vector2i size{images1D.front().size()[0], Int(images1D.size())};
                arrayAppend(outputImages2D, InPlaceInit,
                    /* Don't want to bother with row padding, it's temporary
                       anyway */
                    PixelStorage{}.setAlignment(1),
                    images1D.front().format(),
                    size,
                    Containers::Array<char>{NoInit, size.product()*images1D.front().pixelSize()}
                );

                /* Copy the pixel data over */
                const Containers::StridedArrayView3D<char> outputPixels = outputImages2D.front().mutablePixels();
                for(std::size_t i = 0; i != images1D.size(); ++i)
                    Utility::copy(images1D[i].pixels(), outputPixels[i]);

            } else {
                Error{} << "The --layers option isn't implemented for compressed images yet.";
                return 1;
            }

        } else if(dimensions == 2) {
            if(!checkCommonFormatAndSize(args, images2D)) return 1;

            outputDimensions = 3;
            if(!images2D.front().isCompressed()) {
                /* Allocate a new image */
                /** @todo simplify once ImageData is able to allocate on its
                    own, including correct padding etc */
                const Vector3i size{images2D.front().size(), Int(images2D.size())};
                arrayAppend(outputImages3D, InPlaceInit,
                    /* Don't want to bother with row padding, it's temporary
                       anyway */
                    PixelStorage{}.setAlignment(1),
                    images2D.front().format(),
                    size,
                    Containers::Array<char>{NoInit, size.product()*images2D.front().pixelSize()}
                );

                /* Copy the pixel data over */
                const Containers::StridedArrayView4D<char> outputPixels = outputImages3D.front().mutablePixels();
                for(std::size_t i = 0; i != images2D.size(); ++i)
                    Utility::copy(images2D[i].pixels(), outputPixels[i]);

            } else {
                Error{} << "The --layers option isn't implemented for compressed images yet.";
                return 1;
            }

        } else if(dimensions == 3) {
            Error{} << "The --layers option can be only used with 1D and 2D inputs, not 3D";
            return 1;

        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    /* Extracting a layer, inverse of the above */
    } else if(!args.value("layer").empty()) {
        const Int layer = args.value<Int>("layer");

        if(dimensions == 1) {
            Error{} << "The --layer option can be only used with 2D and 3D inputs, not 1D";
            return 1;

        } else if(dimensions == 2) {
            outputDimensions = 1;

            /* There can be multiple input levels, and a layer should get
               extracted from each level, forming a multi-level image again */
            if(!checkCommonFormat(args, images2D)) return 1;
            if(!images2D.front().isCompressed()) {
                for(std::size_t i = 0; i != images2D.size(); ++i) {
                    /* Diagnostic printed in the import loop above, as here we
                       don't have the filename etc. anymore */
                    CORRADE_INTERNAL_ASSERT(layer >= images2D[i].size().y());

                    /* release() will set the size to 0, extract it first */
                    const Int size = images2D[i].size().x();

                    /* Compared to --layers we can just reuse a slice of the
                       input data without having to allocate any copy */
                    arrayAppend(outputImages1D, InPlaceInit,
                        images2D[i].storage().setSkip({0, Int(layer), 0}),
                        images2D[i].format(),
                        size,
                        images2D[i].release());
                }

            } else {
                Error{} << "The --layer option isn't implemented for compressed images yet.";
                return 1;
            }

        } else if(dimensions == 3) {
            outputDimensions = 2;

            /* There can be multiple input levels, and a layer should get
               extracted from each level, forming a multi-level image again */
            if(!checkCommonFormat(args, images3D)) return 1;
            if(!images3D.front().isCompressed()) {
                for(std::size_t i = 0; i != images3D.size(); ++i) {
                    /* release() will set the size to 0, extract it first */
                    const Vector2i size = images3D[i].size().xy();

                    /* Compared to --layers we can just reuse a slice of the
                       input data without having to allocate any copy */
                    arrayAppend(outputImages2D, InPlaceInit,
                        images3D[i].storage().setSkip({0, 0, Int(layer)}),
                        images3D[i].format(),
                        size,
                        images3D[i].release());
                }

            } else {
                Error{} << "The --layer option isn't implemented for compressed images yet.";
                return 1;
            }

        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    /* Single-image (potentially multi-level) conversion, verify that all have
       the same format and pass the input through. This happens either if
       --levels is set or if the (single) input image is multi-level. */
    } else {
        if(dimensions == 1) {
            if(!checkCommonFormat(args, images1D)) return 1;
            outputDimensions = 1;
            outputImages1D = std::move(images1D);
        } else if(dimensions == 2) {
            if(!checkCommonFormat(args, images2D)) return 1;
            outputDimensions = 2;
            outputImages2D = std::move(images2D);
        } else if(dimensions == 3) {
            if(!checkCommonFormat(args, images3D)) return 1;
            outputDimensions = 3;
            outputImages3D = std::move(images3D);
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
    }

    {
        Debug d;
        if(args.value("converter") == "raw")
            d << "Writing raw image data of size";
        else
            d << "Converting image of size";
        if(outputDimensions == 1) {
            d << outputImages1D.front().size();
            if(outputImages1D.size() > 1)
                d << "(and" << outputImages1D.size() - 1 << "more levels)";
        } else if(outputDimensions == 2) {
            d << outputImages2D.front().size();
            if(outputImages2D.size() > 1)
                d << "(and" << outputImages2D.size() - 1 << "more levels)";
        } else if(outputDimensions == 3) {
            d << outputImages3D.front().size();
            if(outputImages3D.size() > 1)
                d << "(and" << outputImages3D.size() - 1 << "more levels)";
        }
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        d << "and format";
        if(outputDimensions == 1) {
            if(outputImages1D.front().isCompressed())
                d << outputImages1D.front().compressedFormat();
            else d << outputImages1D.front().format();
        } else if(outputDimensions == 2) {
            if(outputImages2D.front().isCompressed())
                d << outputImages2D.front().compressedFormat();
            else d << outputImages2D.front().format();
        } else if(outputDimensions == 3) {
            if(outputImages3D.front().isCompressed())
                d << outputImages3D.front().compressedFormat();
            else d << outputImages3D.front().format();
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        d << "to" << output;
    }

    /* Save raw data, if requested. Only for single-level images as the data
       layout would be messed up otherwise. */
    if(args.value("converter") == "raw") {
        Containers::ArrayView<const char> data;
        if(outputDimensions == 1) {
            CORRADE_INTERNAL_ASSERT(outputImages1D.size() == 1);
            data = outputImages1D.front().data();
        } else if(outputDimensions == 2) {
            CORRADE_INTERNAL_ASSERT(outputImages2D.size() == 1);
            data = outputImages2D.front().data();
        } else if(outputDimensions == 3) {
            CORRADE_INTERNAL_ASSERT(outputImages3D.size() == 1);
            data = outputImages3D.front().data();
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

        {
            Trade::Implementation::Duration d{conversionTime};
            if(!Utility::Path::write(output, data)) return 1;
        }

        if(args.isSet("profile")) {
            Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds, conversion"
                << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count())/1.0e3f << "seconds";
        }

        return 0;
    }

    /* Load converter plugin */
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Trade::AbstractImageConverter::pluginSearchPaths().back())};
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate(args.value("converter"));
    if(!converter) {
        Debug{} << "Available converter plugins:" << ", "_s.join(converterManager.aliasList());
        return 2;
    }

    /* Set options, if passed */
    if(args.isSet("verbose")) converter->addFlags(Trade::ImageConverterFlag::Verbose);
    Implementation::setOptions(*converter, "AnyImageConverter", args.value("converter-options"));

    /* Save output file */
    bool converted;
    {
        Trade::Implementation::Duration d{conversionTime};
        if(outputDimensions == 1)
            converted = convertOneOrMoreImages(*converter, outputImages1D, output);
        else if(outputDimensions == 2)
            converted = convertOneOrMoreImages(*converter, outputImages2D, output);
        else if(outputDimensions == 3)
            converted = convertOneOrMoreImages(*converter, outputImages3D, output);
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
    }
    if(!converted) {
        Error{} << "Cannot save file" << output;
        return 5;
    }

    if(args.isSet("profile")) {
        Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds, conversion"
            << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count())/1.0e3f << "seconds";
    }
}
