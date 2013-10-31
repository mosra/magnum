#ifndef Magnum_Trade_TgaImageConverter_h
#define Magnum_Trade_TgaImageConverter_h
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
 * @brief Class Magnum::Trade::TgaImageConverter
 */

#include <Trade/AbstractImageConverter.h>

#ifndef DOXYGEN_GENERATING_OUTPUT
#if defined(TgaImageConverter_EXPORTS) || defined(TgaImageConverterObjects_EXPORTS)
    #define MAGNUM_TRADE_TGAIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
#else
    #define MAGNUM_TRADE_TGAIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
#endif
#define MAGNUM_TRADE_TGAIMAGECONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief TGA image converter plugin

Supports images with format @ref ColorFormat::BGR, @ref ColorFormat::BGRA or
@ref ColorFormat::Red and type @ref ColorType::UnsignedByte.

This plugin is built if `WITH_TGAIMAGECONVERTER` is enabled in CMake. To use
dynamic plugin, you need to load `%TgaImageConverter` plugin from
`imageconverters/` subdirectory of your plugin dir. To use static plugin or use
this as a dependency of another plugin, you need to request `%TgaImageConverter`
component in CMake and link to `${MAGNUM_TGAIMAGECONVERTER_LIBRARIES}`. See
@ref building and @ref cmake for more information.
*/
class MAGNUM_TRADE_TGAIMAGECONVERTER_EXPORT TgaImageConverter: public AbstractImageConverter {
    public:
        /** @brief Default constructor */
        explicit TgaImageConverter();

        /** @brief Plugin manager constructor */
        explicit TgaImageConverter(PluginManager::AbstractManager* manager, std::string plugin);

    private:
        Features MAGNUM_TRADE_TGAIMAGECONVERTER_LOCAL doFeatures() const override;
        Containers::Array<unsigned char> MAGNUM_TRADE_TGAIMAGECONVERTER_LOCAL doExportToData(const ImageReference2D& image) const override;
};

}}

#endif
