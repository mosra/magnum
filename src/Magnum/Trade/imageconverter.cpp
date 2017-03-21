/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#include "imageconverterConfigure.h"

namespace Magnum {

/**
@page magnum-imageconverter Image conversion utility
@brief Converts images of different formats

@section magnum-imageconverter-usage Usage

    magnum-imageconverter [-h|--help] [--importer IMPORTER] [--converter CONVERTER] [--plugin-dir DIR] [--] input output

Arguments:

-   `input` -- input image
-   `output` -- output image
-   `-h`, `--help` -- display this help message and exit
-   `--importer IMPORTER` -- image importer plugin (default:
    @ref Trade::AnyImageImporter "AnyImageImporter")
-   `--converter CONVERTER` -- image converter plugin (default:
    @ref Trade::AnyImageConverter "AnyImageConverter")
-   `--plugin-dir DIR` -- base plugin dir (defaults to plugin directory in
    Magnum install location)

@section magnum-imageconverter-example Example usage

Converting a JPEG file to a PNG:

    magnum-imageconverter image.jpg image.png

*/

}

using namespace Magnum;

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input image")
        .addArgument("output").setHelp("output", "output image")
        .addOption("importer", "AnyImageImporter").setHelp("importer", "image importer plugin")
        .addOption("converter", "AnyImageConverter").setHelp("converter", "image converter plugin")
        .addOption("plugin-dir", Utility::Directory::join(Utility::Directory::path(Utility::Directory::executableLocation()), MAGNUM_PLUGINS_DIR)).setHelp("plugin-dir", "base plugin dir", "DIR")
        .setHelp("Converts images of different formats.")
        .parse(argc, argv);

    /* Load importer plugin */
    PluginManager::Manager<Trade::AbstractImporter> importerManager(Utility::Directory::join(args.value("plugin-dir"), "importers/"));
    if(!(importerManager.load(args.value("importer")) & PluginManager::LoadState::Loaded))
        return 1;
    std::unique_ptr<Trade::AbstractImporter> importer = importerManager.instance(args.value("importer"));

    /* Load converter plugin */
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager(Utility::Directory::join(args.value("plugin-dir"), "imageconverters/"));
    if(!(converterManager.load(args.value("converter")) & PluginManager::LoadState::Loaded))
        return 1;
    std::unique_ptr<Trade::AbstractImageConverter> converter = converterManager.instance(args.value("converter"));

    /* Open input file */
    std::optional<Trade::ImageData2D> image;
    if(!importer->openFile(args.value("input")) || !(image = importer->image2D(0))) {
        Error() << "Cannot open file" << args.value("input");
        return 1;
    }

    Debug() << "Converting image of size" << image->size() << Debug::nospace << ", format" << image->format() << "and type"  << image->type() << "to" << args.value("output");

    /* Save output file */
    if(!converter->exportToFile(*image, args.value("output"))) {
        Error() << "Cannot save file" << args.value("output");
        return 1;
    }
}
