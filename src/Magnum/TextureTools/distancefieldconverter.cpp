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
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/Range.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
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

namespace Magnum {

/** @page magnum-distancefieldconverter Distance Field conversion utility
@brief Converts red channel of an image to distance field representation

@m_footernavigation
@m_keywords{magnum-distancefieldconverter distancefieldconverter}

This utility is built if both `WITH_TEXTURETOOLS` and
`WITH_DISTANCEFIELDCONVERTER` is enabled when building Magnum. To use this
utility with CMake, you need to request the `distancefieldconverter` component
of the `Magnum` package and use the `Magnum::distancefieldconverter` target for
example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED distancefieldconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::distancefieldconverter ...)
@endcode

See @ref building, @ref cmake and the @ref TextureTools namespace for more
information.

@section magnum-distancefieldconverter-usage Usage

@code{.sh}
magnum-distancefieldconverter [--magnum-...] [-h|--help] [--importer IMPORTER]
    [--converter CONVERTER] [--plugin-dir DIR] --output-size "X Y" --radius N
    [--] input output
@endcode

Arguments:

-   `input` --- input image
-   `output` --- output image
-   `-h`, `--help` --- display help message and exit
-   `--importer IMPORTER` --- image importer plugin (default:
    @ref Trade::AnyImageImporter "AnyImageImporter")
-   `--converter CONVERTER` --- image converter plugin (default:
    @ref Trade::AnyImageConverter "AnyImageConverter")
-   `--plugin-dir DIR` --- override base plugin dir
-   `--output-size "X Y"` --- size of output image
-   `--radius N` --- distance field computation radius
-   `--magnum-...` --- engine-specific options (see
    @ref GL-Context-command-line for details)

Images with @ref PixelFormat::R8Unorm, @ref PixelFormat::RGB8Unorm or
@ref PixelFormat::RGBA8Unorm are accepted on input.

The resulting image can be then used with @ref Shaders::DistanceFieldVector
shader. See also @ref TextureTools::DistanceField for more information about
the algorithm and parameters.

@section magnum-distancefield-example Example usage

@code{.sh}
magnum-distancefieldconverter --output-size "256 256" --radius 24 logo-src.png logo.png
@endcode

This will open monochrome `logo-src.png` image using any plugin that can open
PNG files and converts it to 256x256 distance field `logo.png` using any plugin
that can write PNG files.

@note This executable is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
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
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        .addNamedArgument("output-size").setHelp("output-size", "size of output image", "\"X Y\"")
        .addNamedArgument("radius").setHelp("radius", "distance field computation radius", "N")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setGlobalHelp("Converts red channel of an image to distance field representation.")
        .parse(arguments.argc, arguments.argv);

    createContext();
}

int DistanceFieldConverter::exec() {
    /* Load importer plugin */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImporter::pluginSearchPaths()[0])};
    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
    if(!importer) return 1;

    /* Load converter plugin */
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImageConverter::pluginSearchPaths()[0])};
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate(args.value("converter"));
    if(!converter) return 2;

    /* Open input file */
    Containers::Optional<Trade::ImageData2D> image;
    if(!importer->openFile(args.value("input")) || !(image = importer->image2D(0))) {
        Error() << "Cannot open file" << args.value("input");
        return 3;
    }

    /* Decide about internal format */
    GL::TextureFormat internalFormat;
    if(image->format() == PixelFormat::R8Unorm)
        internalFormat = GL::TextureFormat::R8;
    else if(image->format() == PixelFormat::RGB8Unorm)
        internalFormat = GL::TextureFormat::RGB8;
    else if(image->format() == PixelFormat::RGBA8Unorm)
        internalFormat = GL::TextureFormat::RGBA8;
    else {
        Error() << "Unsupported image format" << image->format();
        return 4;
    }

    /* Input texture */
    GL::Texture2D input;
    input.setMinificationFilter(SamplerFilter::Linear)
        .setMagnificationFilter(SamplerFilter::Linear)
        .setWrapping(SamplerWrapping::ClampToEdge)
        .setStorage(1, internalFormat, image->size())
        .setSubImage(0, {}, *image);

    /* Output texture */
    GL::Texture2D output;
    output.setStorage(1, GL::TextureFormat::R8, args.value<Vector2i>("output-size"));

    CORRADE_INTERNAL_ASSERT(GL::Renderer::error() == GL::Renderer::Error::NoError);

    /* Do it */
    Debug() << "Converting image of size" << image->size() << "to distance field...";
    TextureTools::DistanceField{args.value<UnsignedInt>("radius")}(input, output, {{}, args.value<Vector2i>("output-size")}, image->size());

    /* Save image */
    Image2D result{PixelFormat::R8Unorm};
    output.image(0, result);
    if(!converter->exportToFile(result, args.value("output"))) {
        Error() << "Cannot save file" << args.value("output");
        return 5;
    }

    return 0;
}

}}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::TextureTools::DistanceFieldConverter)
