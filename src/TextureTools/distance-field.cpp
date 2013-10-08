/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Utility/Arguments.h>
#include <PluginManager/Manager.h>

#include "Math/Geometry/Rectangle.h"
#include "ColorFormat.h"
#include "Image.h"
#include "Renderer.h"
#include "Texture.h"
#include "TextureFormat.h"
#include "Platform/WindowlessGlxApplication.h"
#include "TextureTools/DistanceField.h"
#include "Trade/AbstractImporter.h"
#include "Trade/AbstractImageConverter.h"
#include "Trade/ImageData.h"

#include "configure.h"

namespace Magnum {

class DistanceFieldConverter: public Platform::WindowlessApplication {
    public:
        explicit DistanceFieldConverter(const Arguments& arguments);

        int exec() override;

    private:
        Utility::Arguments args;
};

DistanceFieldConverter::DistanceFieldConverter(const Arguments& arguments): WindowlessGlxApplication(arguments, nullptr) {
    args.addArgument("input").setHelp("input", "input image")
        .addArgument("output").setHelp("output", "output image")
        .addOption("importer", "TgaImporter").setHelp("image importer plugin")
        .addOption("converter", "TgaImageConverter").setHelp("image converter plugin")
        .addNamedArgument("output-size").setHelpKey("output-size", "\"X Y\"").setHelp("output-size", "size of output image")
        .addNamedArgument("radius").setHelpKey("radius", "N").setHelp("radius", "distance field computation radius")
        .setHelp("Converts black&white image to distance-field representation.")
        .parse(arguments.argc, arguments.argv);

    createContext({});
}

int DistanceFieldConverter::exec() {
    /* Load plugins */
    PluginManager::Manager<Trade::AbstractImporter> importerManager(MAGNUM_IMPORTER_PLUGIN_DIR);
    if(!(importerManager.load(args.value("importer")) & PluginManager::LoadState::Loaded)) {
        Error() << "Cannot load importer plugin" << args.value("importer") << "from" << MAGNUM_IMPORTER_PLUGIN_DIR;
        return 1;
    }
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager(MAGNUM_IMAGECONVERTER_PLUGIN_DIR);
    if(!(converterManager.load(args.value("converter")) & PluginManager::LoadState::Loaded)) {
        Error() << "Cannot load converter plugin" << args.value("converter") << "from" << MAGNUM_IMAGECONVERTER_PLUGIN_DIR;
        return 1;
    }

    /* Instance plugins */
    std::unique_ptr<Trade::AbstractImporter> importer = importerManager.instance(args.value("importer"));
    CORRADE_INTERNAL_ASSERT(importer);
    std::unique_ptr<Trade::AbstractImageConverter> converter = converterManager.instance(args.value("converter"));
    CORRADE_INTERNAL_ASSERT(converter);

    /* Open input file */
    std::optional<Trade::ImageData2D> image;
    if(!importer->openFile(args.value("input")) || !(image = importer->image2D(0))) {
        Error() << "Cannot open file" << args.value("input");
        return 1;
    }

    if(image->format() != ColorFormat::Red) {
        Error() << "Unsupported image format" << image->format();
        return 1;
    }

    /* Input texture */
    Texture2D input;
    input.setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setWrapping(Sampler::Wrapping::ClampToEdge)
        .setImage(0, TextureFormat::R8, *image);

    /* Output texture */
    Texture2D output;
    output.setStorage(1, TextureFormat::R8, args.value<Vector2i>("output-size"));

    CORRADE_INTERNAL_ASSERT(Renderer::error() == Renderer::Error::NoError);

    /* Do it */
    Debug() << "Converting image of size" << image->size() << "to distance field...";
    TextureTools::distanceField(input, output, {{}, args.value<Vector2i>("output-size")}, args.value<Int>("radius"), image->size());

    /* Save image */
    Image2D result(ColorFormat::Red, ColorType::UnsignedByte);
    output.image(0, result);
    if(!converter->exportToFile(result, args.value("output"))) {
        Error() << "Cannot save file" << args.value("output");
        return 1;
    }

    return 0;
}

}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::DistanceFieldConverter)
