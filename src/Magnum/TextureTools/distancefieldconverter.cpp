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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Arguments is std::string-free */
#include <Corrade/Utility/Path.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/Range.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/TextureTools/DistanceFieldGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#ifdef MAGNUM_TARGET_EGL
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#include "Magnum/Platform/WindowlessGlxApplication.h"
#elif defined(CORRADE_TARGET_WINDOWS)
#include "Magnum/Platform/WindowlessWglApplication.h"
#else
#error no windowless application available on this platform
#endif

namespace Magnum { namespace {

/** @page magnum-distancefieldconverter Distance Field conversion utility
@brief Converts red channel of an image to distance field representation

@tableofcontents
@m_footernavigation
@m_keywords{magnum-distancefieldconverter distancefieldconverter}

This utility is built if `MAGNUM_WITH_DISTANCEFIELDCONVERTER` is enabled when
building Magnum. To use this utility with CMake, you need to request the
`distancefieldconverter` component of the `Magnum` package and use the
`Magnum::distancefieldconverter` target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED distancefieldconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::distancefieldconverter ...)
@endcode

See @ref building, @ref cmake and the @ref TextureTools namespace for more
information.

@note This executable is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section magnum-distancefield-example Example usage

@code{.sh}
magnum-distancefieldconverter logo-src.png logo.png \
    --output-size "256 256" --radius 24
@endcode

This will open monochrome `logo-src.png` image using any plugin that can open
PNG files and converts it to 256x256 distance field `logo.png` using any plugin
that can write PNG files.

@section magnum-distancefieldconverter-usage Full usage documentation

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
    @ref GL-Context-usage-command-line for details)

Images with @ref PixelFormat::R8Unorm, @ref PixelFormat::RGB8Unorm or
@ref PixelFormat::RGBA8Unorm are accepted on input.

The resulting image can then be used with @ref Shaders::DistanceFieldVectorGL.
See @ref TextureTools::DistanceField for more information about the algorithm
and parameters. Size restrictions from it apply here as well, in particular the
ratio of the source image size and and `--output-size` is expected to be a
multiple of 2.
*/

#ifndef DOXYGEN_GENERATING_OUTPUT
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
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        #endif
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
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Trade::AbstractImporter::pluginSearchPaths().back()))
        #endif
    };
    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
    if(!importer) return 1;

    /* Load converter plugin */
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Trade::AbstractImageConverter::pluginSearchPaths().back()))
        #endif
    };
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate(args.value("converter"));
    if(!converter) return 2;

    /* Open input file */
    Containers::Optional<Trade::ImageData2D> image;
    if(!importer->openFile(args.value("input")) || !(image = importer->image2D(0))) {
        Error() << "Cannot open file" << args.value("input");
        return 3;
    }

    /** @todo do all these error checks before a context is created, to avoid
        extra spam in the output (or worse, a failure to create context even
        before the input data can be checked) */

    /* Check that the output size is compatible with what we want to do */
    const Vector2i outputSize = args.value<Vector2i>("output-size");
    if(image->size() % outputSize != Vector2i{0} ||
       (image->size()/outputSize) % 2 != Vector2i{0}) {
        Error{} << "Expected input and output size ratio to be a multiple of 2, got" << Debug::packed << image->size() << "and" << Debug::packed << outputSize;
        return 5;
    }

    /* Decide about internal format */
    /** @todo this doesn't work on ES2, the image pixel format is converted to
        a LUMINANCE which doesn't match GL_RED / GL_R8; it also doesn't check
        that EXT_texture_rg exists, and LUMINANCE isn't really renderable, etc
        etc */
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
    output.setStorage(1, GL::TextureFormat::R8, outputSize);

    /* Rectangle to process */
    const Range2Di rectangle{{}, outputSize};

    /* Output framebuffer */
    GL::Framebuffer framebuffer{rectangle};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment(0), output, 0);

    CORRADE_INTERNAL_ASSERT(GL::Renderer::error() == GL::Renderer::Error::NoError);

    /* Do it */
    Debug() << "Converting image of size" << image->size() << "to distance field...";
    TextureTools::DistanceFieldGL{args.value<UnsignedInt>("radius")}(input, output, rectangle, image->size());

    /* Save image */
    Image2D result{PixelFormat::R8Unorm};
    framebuffer.read(rectangle, result);
    if(!converter->convertToFile(result, args.value("output"))) {
        Error() << "Cannot save file" << args.value("output");
        return 5;
    }

    return 0;
}
#endif

}}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::DistanceFieldConverter)
