#ifndef Magnum_Trade_TgaImageConverter_TgaImageConverter_h
#define Magnum_Trade_TgaImageConverter_TgaImageConverter_h
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
 * @brief Class Magnum::Trade::TgaImageConverter::TgaImageConverter
 */

#include <Trade/AbstractImageConverter.h>

namespace Magnum { namespace Trade { namespace TgaImageConverter {

/**
@brief TGA image converter

Supports images with format @ref AbstractImage::Format "AbstractImage::Format::BGR",
@ref AbstractImage::Format "AbstractImage::Format::BGRA" or @ref AbstractImage::Format "AbstractImage::Format::Red"
and type @ref AbstractImage::Type "AbstractImage::Type::UnsignedByte".
*/
class TgaImageConverter: public AbstractImageConverter {
    public:
        /** @brief Default constructor */
        explicit TgaImageConverter();

        /** @brief Plugin manager constructor */
        explicit TgaImageConverter(Corrade::PluginManager::AbstractPluginManager* manager, std::string plugin);

        Features features() const override;
        std::pair<const unsigned char*, std::size_t> convertToData(const Image2D* const image) const override;
        bool convertToFile(const Image2D* const image, const std::string& filename) const override;
};

}}}

#endif
