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

#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/Math/Range.h"
#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Renderer.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/TextureTools/DistanceField.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#ifdef MAGNUM_TARGET_HEADLESS
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessEglApplication.h"
#else
#include "Magnum/Platform/WindowlessGlxApplication.h"
#endif
#elif defined(CORRADE_TARGET_WINDOWS)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessWindowsEglApplication.h"
#else
#include "Magnum/Platform/WindowlessWglApplication.h"
#endif
#else
#error no windowless application available on this platform
#endif

#include "distancefieldconverterConfigure.h"

namespace Magnum {

/** @page magnum-distancefieldconverter Distance Field conversion utility
@brief Converts red channel of an image to distance field representation

@section magnum-distancefieldconverter-usage Usage

    magnum-distancefieldconverter [--magnum-...] [-h|--help] [--importer IMPORTER] [--converter CONVERTER] [--plugin-dir DIR] --output-size "X Y" --radius N [--] input output

Arguments:

-   `input` -- input image
-   `output` -- output image
-   `-h`, `--help` -- display help message and exit
-   `--importer IMPORTER` -- image importer plugin (default: @ref Trade::AnyImageImporter "AnyImageImporter")
-   `--converter CONVERTER` -- image converter plugin (default: @ref Trade::AnyImageConverter "AnyImageConverter")
-   `--plugin-dir DIR` -- base plugin dir (defaults to plugin directory in
    Magnum install location)
-   `--output-size "X Y"` -- size of output image
-   `--radius N` -- distance field computation radius
-   `--magnum-...` -- engine-specific options (see @ref Context for details)

Images with @ref PixelFormat::Red, @ref PixelFormat::RGB or @ref PixelFormat::RGBA
are accepted on input.

The resulting image can be then used with @ref Shaders::DistanceFieldVector
shader. See also @ref TextureTools::distanceField() for more information about
the algorithm and parameters.

@section magnum-distancefield-example Example usage

    magnum-distancefieldconverter --output-size "256 256" --radius 24 logo-src.png logo.png

This will open monochrome `logo-src.png` image using any plugin that can open
PNG files and converts it to 256x256 distance field `logo.png` using any plugin
that can write PNG files.

*/

namespace TextureTools {

class DistanceFieldConverter: public Platform::WindowlessApplication {
    public:
        explicit DistanceFieldConverter(const Arguments& arguments);

        int exec() override;

    private:
        Utility::Arguments args;
};

DistanceFieldConverter::DistanceFieldConverter(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {
    args.addArgument("input").setHelp("input", "input image")
        .addArgument("output").setHelp("output", "output image")
        .addOption("importer", "AnyImageImporter").setHelp("importer", "image importer plugin")
        .addOption("converter", "AnyImageConverter").setHelp("converter", "image converter plugin")
        .addOption("plugin-dir", Utility::Directory::join(Utility::Directory::path(Utility::Directory::executableLocation()), MAGNUM_PLUGINS_DIR)).setHelp("plugin-dir", "base plugin dir", "DIR")
        .addNamedArgument("output-size").setHelp("output-size", "size of output image", "\"X Y\"")
        .addNamedArgument("radius").setHelp("radius", "distance field computation radius", "N")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setHelp("Converts red channel of an image to distance field representation.")
        .parse(arguments.argc, arguments.argv);

    createContext();
}

int DistanceFieldConverter::exec() {
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

    /* Decide about internal format */
    TextureFormat internalFormat;
    if(image->format() == PixelFormat::Red) internalFormat = TextureFormat::R8;
    else if(image->format() == PixelFormat::RGB) internalFormat = TextureFormat::RGB8;
    else if(image->format() == PixelFormat::RGBA) internalFormat = TextureFormat::RGBA8;
    else {
        Error() << "Unsupported image format" << image->format();
        return 1;
    }

    /* Input texture */
    Texture2D input;
    input.setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setWrapping(Sampler::Wrapping::ClampToEdge)
        .setStorage(1, internalFormat, image->size())
        .setSubImage(0, {}, *image);

    /* Output texture */
    Texture2D output;
    output.setStorage(1, TextureFormat::R8, args.value<Vector2i>("output-size"));

    CORRADE_INTERNAL_ASSERT(Renderer::error() == Renderer::Error::NoError);

    /* Do it */
    Debug() << "Converting image of size" << image->size() << "to distance field...";
    TextureTools::distanceField(input, output, {{}, args.value<Vector2i>("output-size")}, args.value<Int>("radius"), image->size());

    /* Save image */
    Image2D result(PixelFormat::Red, PixelType::UnsignedByte);
    output.image(0, result);
    if(!converter->exportToFile(result, args.value("output"))) {
        Error() << "Cannot save file" << args.value("output");
        return 1;
    }

    return 0;
}

}}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::TextureTools::DistanceFieldConverter)
