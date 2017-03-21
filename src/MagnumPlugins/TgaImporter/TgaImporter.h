#ifndef Magnum_Trade_TgaImporter_h
#define Magnum_Trade_TgaImporter_h
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

/** @file
 * @brief Class @ref Magnum::Trade::TgaImporter
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/VisibilityMacros.h>

#include "Magnum/Trade/AbstractImporter.h"

#include "MagnumPlugins/TgaImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TGAIMPORTER_BUILD_STATIC
    #if defined(TgaImporter_EXPORTS) || defined(TgaImporterObjects_EXPORTS)
        #define MAGNUM_TGAIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_TGAIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_TGAIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_TGAIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief TGA importer plugin

Supports Truevision TGA (`*.tga`, `*.vda`, `*.icb`, `*.vst`) uncompressed BGR,
BGRA or grayscale images with 8 bits per channel.

This plugin is built if `WITH_TGAIMPORTER` is enabled when building Magnum. To
use dynamic plugin, you need to load `TgaImporter` plugin from
`MAGNUM_PLUGINS_IMPORTER_DIR`. To use static plugin or use this as a dependency
of another plugin, you need to request `TgaImporter` component of `Magnum`
package in CMake and link to `Magnum::TgaImporter` target. See @ref building,
@ref cmake and @ref plugins for more information.

The images are imported with @ref PixelType::UnsignedByte and @ref PixelFormat::RGB,
@ref PixelFormat::RGBA or @ref PixelFormat::Red, respectively. Grayscale images
require extension @extension{ARB,texture_rg}. Imported images are imported with
default @ref PixelStorage parameters except for alignment, which may be changed
to `1` if the data require it.

In OpenGL ES 2.0, if @extension{EXT,texture_rg} is not supported and in
WebGL 1.0, grayscale images use @ref PixelFormat::Luminance instead of
@ref PixelFormat::Red.
*/
class MAGNUM_TGAIMPORTER_EXPORT TgaImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit TgaImporter();

        /** @brief Plugin manager constructor */
        explicit TgaImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~TgaImporter();

    private:
        Features MAGNUM_TGAIMPORTER_LOCAL doFeatures() const override;
        bool MAGNUM_TGAIMPORTER_LOCAL doIsOpened() const override;
        void MAGNUM_TGAIMPORTER_LOCAL doOpenData(Containers::ArrayView<const char> data) override;
        void MAGNUM_TGAIMPORTER_LOCAL doClose() override;
        UnsignedInt MAGNUM_TGAIMPORTER_LOCAL doImage2DCount() const override;
        std::optional<ImageData2D> MAGNUM_TGAIMPORTER_LOCAL doImage2D(UnsignedInt id) override;

        Containers::Array<char> _in;
};

}}

#endif
