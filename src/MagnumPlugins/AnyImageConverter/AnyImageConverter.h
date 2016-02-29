#ifndef Magnum_Trade_AnyImageConverter_h
#define Magnum_Trade_AnyImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

/** @file
 * @brief Class @ref Magnum::Trade::AnyImageConverter
 */

#include <Magnum/Trade/AbstractImageConverter.h>

namespace Magnum { namespace Trade {

/**
@brief Any image converter plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to convert the file with it.

This plugin is built if `WITH_ANYIMAGECONVERTER` is enabled when building
Magnum Plugins. To use dynamic plugin, you need to load `AnyImageConverter`
plugin from `MAGNUM_PLUGINS_IMPORTER_DIR`. To use static plugin, you need to
request `AnyImageConverter` component of `MagnumPlugins` package in CMake and
link to `MagnumPlugins::AnyImageConverter` target. See @ref building-plugins,
@ref cmake-plugins and @ref plugins for more information.

Supported formats for uncompressed data:

-   OpenEXR (`*.exr`), converted with any plugin that provides
    `OpenExrImageConverter`
-   PNG (`*.png`), converted with @ref PngImageConverer or any other plugin
    that provides it
-   TGA (`*.tga`), converted with @ref TgaImageConverter or any other plugin
    that provides it

No supported formats for compressed data yet.

Only exporting to files is supported.
*/
class AnyImageConverter: public AbstractImageConverter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnyImageConverter(PluginManager::Manager<AbstractImageConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyImageConverter(PluginManager::AbstractManager& manager, std::string plugin);

        ~AnyImageConverter();

    private:
        Features doFeatures() const override;
        bool doExportToFile(const ImageView2D& image, const std::string& filename) override;
        bool doExportToFile(const CompressedImageView2D& image, const std::string& filename) override;
};

}}

#endif
