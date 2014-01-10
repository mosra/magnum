#ifndef Magnum_Trade_TgaImporter_h
#define Magnum_Trade_TgaImporter_h
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

/** @file
 * @brief Class Magnum::Trade::TgaImporter
 */

#include <Corrade/Utility/VisibilityMacros.h>

#include "Magnum/Trade/AbstractImporter.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_BUILD_STATIC
    #if defined(TgaImporter_EXPORTS) || defined(TgaImporterObjects_EXPORTS)
        #define MAGNUM_TRADE_TGAIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_TRADE_TGAIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_TRADE_TGAIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_TRADE_TGAIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief TGA importer plugin

Supports uncompressed BGR, BGRA or grayscale images with 8 bits per channel.

This plugin is built if `WITH_TGAIMPORTER` is enabled when building %Magnum. To
use dynamic plugin, you need to load `%TgaImporter` plugin from
`MAGNUM_PLUGINS_IMPORTER_DIR`. To use static plugin or use this as a dependency
of another plugin, you need to request `%TgaImporter` component of `%Magnum`
package in CMake and link to `${MAGNUM_TGAIMPORTER_LIBRARIES}`. See
@ref building, @ref cmake and @ref plugins for more information.

The images are imported with @ref ColorType::UnsignedByte and @ref ColorFormat::BGR,
@ref ColorFormat::BGRA or @ref ColorFormat::Red, respectively. Grayscale images
require extension @extension{ARB,texture_rg}.

In OpenGL ES BGR and BGRA images are converted to @ref ColorFormat::RGB
and @ref ColorFormat::RGBA. In OpenGL ES 2.0, if @es_extension{EXT,texture_rg}
is not supported, grayscale images use @ref ColorFormat::Luminance instead of
@ref ColorFormat::Red.
*/
class MAGNUM_TRADE_TGAIMPORTER_EXPORT TgaImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit TgaImporter();

        /** @brief Plugin manager constructor */
        explicit TgaImporter(PluginManager::AbstractManager& manager, std::string plugin);

        ~TgaImporter();

    private:
        Features MAGNUM_TRADE_TGAIMPORTER_LOCAL doFeatures() const override;
        bool MAGNUM_TRADE_TGAIMPORTER_LOCAL doIsOpened() const override;
        void MAGNUM_TRADE_TGAIMPORTER_LOCAL doOpenData(Containers::ArrayReference<const unsigned char> data) override;
        void MAGNUM_TRADE_TGAIMPORTER_LOCAL doOpenFile(const std::string& filename) override;
        void MAGNUM_TRADE_TGAIMPORTER_LOCAL doClose() override;
        UnsignedInt MAGNUM_TRADE_TGAIMPORTER_LOCAL doImage2DCount() const override;
        std::optional<ImageData2D> MAGNUM_TRADE_TGAIMPORTER_LOCAL doImage2D(UnsignedInt id) override;

        std::istream* in;
};

}}

#endif
