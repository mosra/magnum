/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/String.h>

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

This utility is built if both `WITH_TRADE` and `WITH_IMAGECONVERTER` is enabled
when building Magnum. To use this utility with CMake, you need to request the
`imageconverter` component of the `Magnum` package and use the
`Magnum::imageconverter` target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED imageconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::imageconverter ...)
@endcode

See @ref building, @ref cmake and the @ref Trade namespace for more
information.

@section magnum-imageconverter-usage Usage

@code{.sh}
magnum-imageconverter [-h|--help] [-I|--importer IMPORTER]
    [-C|--converter CONVERTER] [--plugin-dir DIR]
    [-i|--importer-options key=val,key2=val2,…]
    [-c|--converter-options key=val,key2=val2,…] [--image IMAGE]
    [--level LEVEL] [--in-place] [--info] [-v|--verbose] [--] input output
@endcode

Arguments:

-   `input` --- input image
-   `output` --- output image, ignored if `--in-place` or `--info` is present
-   `-h`, `--help` --- display this help message and exit
-   `-I`, `--importer IMPORTER` --- image importer plugin (default:
    @ref Trade::AnyImageImporter "AnyImageImporter")
-   `-C`, `--converter CONVERTER` --- image converter plugin (default:
    @ref Trade::AnyImageConverter "AnyImageConverter")
-   `--plugin-dir DIR` --- override base plugin dir
-   `-i`, `--importer-options key=val,key2=val2,…` --- configuration options to
    pass to the importer
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to the converter
-   `--image IMAGE` --- image to import (default: `0`)
-   `--level LEVEL` --- image level to import (default: `0`)
-   `--in-place` --- overwrite the input image with the output
-   `--info` --- print info about the input file and exit
-   `-v`, `--verbose` --- verbose output from importer and converter plugins

Specifying `--importer raw:&lt;format&gt;` will treat the input as a raw
tightly-packed square of pixels in given @ref PixelFormat. Specifying `-C` /
`--converter raw` will save raw imported data instead of using a converter
plugin.

If `--info` is given, the utility will print information about all images
present in the file. In this case no conversion is done and output file doesn't
need to be specified.

The `-i` / `--importer-options` and `-c` / `--converter-options` arguments
accept a comma-separated list of key/value pairs to set in the importer /
converter plugin configuration. If the `=` character is omitted, it's
equivalent to saying `key=true`; configuration subgroups are delimited with
`/`.

@section magnum-imageconverter-example Example usage

Converting a JPEG file to a PNG:

@code{.sh}
magnum-imageconverter image.jpg image.png
@endcode

Creating a JPEG file with 95% quality from a PNG, by setting a
@ref Trade-JpegImageConverter-configuration "plugin-specific configuration option".
Note that currently the proxy @ref Trade::AnyImageImporter "AnyImageImporter"
and @ref Trade::AnyImageConverter "AnyImageConverter" plugins don't know how to
correctly propagate options to the target plugin, so you need to specify
`--importer` / `--converter` explicitly when using the `-i` / `-c` options.

@m_class{m-console-wrap}

@code{.sh}
magnum-imageconverter image.png image.jpg -c jpegQuality=0.95 --converter JpegImageConverter
@endcode

Extracting raw (uncompressed, compressed) data from a DDS file for manual
inspection:

@code{.sh}
magnum-imageconverter image.dds --converter raw data.dat
@endcode

@see @ref magnum-sceneconverter
*/

}

using namespace Magnum;

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input image")
        .addArgument("output").setHelp("output", "output image, ignored if --in-place or --info is present")
        .addOption('I', "importer", "AnyImageImporter").setHelp("importer", "image importer plugin")
        .addOption('C', "converter", "AnyImageConverter").setHelp("converter", "image converter plugin")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter", "key=val,key2=val2,…")
        .addOption("image", "0").setHelp("image", "image to import")
        .addOption("level", "0").setHelp("level", "image level to import")
        .addBooleanOption("in-place").setHelp("in-place", "overwrite the input image with the output")
        .addBooleanOption("info").setHelp("info", "print info about the input file and exit")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from importer and converter plugins")
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
in the file. In this case no conversion is done and output file doesn't need to
be specified.

The -i / --importer-options and -c / --converter-options arguments accept a
comma-separated list of key/value pairs to set in the importer / converter
plugin configuration. If the = character is omitted, it's equivalent to saying
key=true; configuration subgroups are delimited with /.)")
        .parse(argc, argv);

    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImporter::pluginSearchPaths()[0])};

    /* Load raw data, if requested; assume it's a tightly-packed square of
       given format */
    /** @todo implement image slicing and then use `--slice "0 0 w h"` to
        specify non-rectangular size (and +x +y to specify padding?) */
    Containers::Optional<Trade::ImageData2D> image;
    if(Utility::String::beginsWith(args.value("importer"), "raw:")) {
        /** @todo Any chance to do this without using internal APIs? */
        const PixelFormat format = Utility::ConfigurationValue<PixelFormat>::fromString(args.value("importer").substr(4), {});
        const UnsignedInt pixelSize = Magnum::pixelSize(format);
        if(format == PixelFormat{}) {
            Error{} << "Invalid raw pixel format" << args.value("importer");
            return 4;
        }

        /** @todo simplify once read() reliably returns an Optional */
        if(!Utility::Directory::exists(args.value("input"))) {
            Error{} << "Cannot open file" << args.value("input");
            return 3;
        }
        Containers::Array<char> data = Utility::Directory::read(args.value("input"));
        auto side = Int(std::sqrt(data.size()/pixelSize));
        if(data.size() % pixelSize || side*side*pixelSize != data.size()) {
            Error{} << "File of size" << data.size() << "is not a tightly-packed square of" << format;
            return 5;
        }

        /* Print image info, if requested */
        if(args.isSet("info")) {
            Debug{} << "Image 0:\n  Mip 0:" << format << Vector2i{side};
            return 0;
        }

        image = Trade::ImageData2D(format, Vector2i{side}, std::move(data));

    /* Otherwise load it using an importer plugin */
    } else {
        Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
        if(!importer) {
            Debug{} << "Available importer plugins:" << Utility::String::join(importerManager.aliasList(), ", ");
            return 1;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) importer->setFlags(Trade::ImporterFlag::Verbose);
        Implementation::setOptions(*importer, args.value("importer-options"));

        /* Print image info, if requested */
        if(args.isSet("info")) {
            /* Open the file, but don't fail when an image can't be opened */
            if(!importer->openFile(args.value("input"))) {
                Error() << "Cannot open file" << args.value("input");
                return 3;
            }

            if(!importer->image1DCount() && !importer->image2DCount() && !importer->image2DCount()) {
                Debug{} << "No images found.";
                return 0;
            }

            /* Parse everything first to avoid errors interleaved with output.
               In case the images have all just a single level and no names,
               write them in a compact way without listing levels. */
            bool error = false, compact = true;
            Containers::Array<Trade::Implementation::ImageInfo> infos =
                Trade::Implementation::imageInfo(*importer, error, compact);

            for(const Trade::Implementation::ImageInfo& info: infos) {
                Debug d;
                if(info.level == 0) {
                    d << "Image" << info.image << Debug::nospace << ":";
                    if(!info.name.empty()) d << info.name;
                    if(!compact) d << Debug::newline;
                }
                if(!compact) d << "  Level" << info.level << Debug::nospace << ":";
                if(info.compressed) d << info.compressedFormat;
                else d << info.format;
                if(info.size.z()) d << info.size;
                else if(info.size.y()) d << info.size.xy();
                else d << Math::Vector<1, Int>(info.size.x());
            }

            return error ? 1 : 0;
        }

        /* Open input file and the desired image */
        if(!importer->openFile(args.value("input"))) {
            Error() << "Cannot open file" << args.value("input");
            return 3;
        }

         if(!(image = importer->image2D(args.value<UnsignedInt>("image"), args.value<UnsignedInt>("level")))) {
            Error() << "Cannot import the image";
            return 4;
        }
    }

    const std::string output = args.value(args.isSet("in-place") ? "input" : "output");

    {
        Debug d;
        if(args.value("converter") == "raw")
            d << "Writing raw image data of size";
        else
            d << "Converting image of size";
        d << image->size() << "and format";
        if(image->isCompressed()) d << image->compressedFormat();
        else d << image->format();
        d << "to" << output;
    }

    /* Save raw data, if requested */
    if(args.value("converter") == "raw") {
        Utility::Directory::write(output, image->data());
        return 0;
    }

    /* Load converter plugin */
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImageConverter::pluginSearchPaths()[0])};
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate(args.value("converter"));
    if(!converter) {
        Debug{} << "Available converter plugins:" << Utility::String::join(converterManager.aliasList(), ", ");
        return 2;
    }

    /* Set options, if passed */
    if(args.isSet("verbose")) converter->setFlags(Trade::ImageConverterFlag::Verbose);
    Implementation::setOptions(*converter, args.value("converter-options"));

    /* Save output file */
    if(!converter->exportToFile(*image, output)) {
        Error() << "Cannot save file" << output;
        return 5;
    }
}
