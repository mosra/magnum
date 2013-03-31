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

#include "AbstractImageConverter.h"

#include <Utility/Assert.h>

namespace Magnum { namespace Trade {

AbstractImageConverter::AbstractImageConverter() = default;

AbstractImageConverter::AbstractImageConverter(Corrade::PluginManager::AbstractPluginManager* manager, std::string plugin): AbstractPlugin(manager, std::move(plugin)) {}

Image2D* AbstractImageConverter::convertToImage(const Image2D* const) const {
    CORRADE_ASSERT(features() & Feature::ConvertToImage,
        "Trade::AbstractImageConverter::convertToImage(): feature advertised but not implemented", nullptr);

    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::convertToImage(): feature not implemented", nullptr);
}

std::pair<const unsigned char*, std::size_t> AbstractImageConverter::convertToData(const Image2D* const) const {
    CORRADE_ASSERT(features() & Feature::ConvertToData,
        "Trade::AbstractImageConverter::convertToData(): feature advertised but not implemented", std::make_pair(nullptr, 0));

    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::convertToData(): feature not implemented", std::make_pair(nullptr, 0));
}

bool AbstractImageConverter::convertToFile(const Image2D* const, const std::string&) const {
    CORRADE_ASSERT(features() & Feature::ConvertToFile,
        "Trade::AbstractImageConverter::convertToFile(): feature advertised but not implemented", false);

    CORRADE_ASSERT(false, "Trade::AbstractImageConverter::convertToFile(): feature not implemented", false);
}

}}
