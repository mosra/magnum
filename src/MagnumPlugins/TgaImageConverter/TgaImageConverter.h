#ifndef Magnum_Trade_TgaImageConverter_h
#define Magnum_Trade_TgaImageConverter_h
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

/** @file
 * @brief Class @ref Magnum::Trade::TgaImageConverter
 */

#include "Magnum/Trade/AbstractImageConverter.h"

#include "MagnumPlugins/TgaImageConverter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TGAIMAGECONVERTER_BUILD_STATIC
    #if defined(TgaImageConverter_EXPORTS) || defined(TgaImageConverterObjects_EXPORTS)
        #define MAGNUM_TGAIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_TGAIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_TGAIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_TGAIMAGECONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_TGAIMAGECONVERTER_EXPORT
#define MAGNUM_TGAIMAGECONVERTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief TGA image converter plugin

Creates Truevision TGA (`*.tga`) files from images with format
@ref PixelFormat::RGB8Unorm, @ref PixelFormat::RGBA8Unorm or
@ref PixelFormat::R8Unorm.

@section Trade-TgaImageConverter-usage Usage

This plugin depends on the @ref Trade library and is built if
`WITH_TGAIMAGECONVERTER` is enabled when building Magnum. To use as a dynamic
plugin, load @cpp "TgaImageConverter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_TGAIMAGECONVERTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::TgaImageConverter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `TgaImageConverter` component of the `Magnum` package and
link to the `Magnum::TgaImageConverter` target:

@code{.cmake}
find_package(Magnum REQUIRED TgaImageConverter)

# ...
target_link_libraries(your-app PRIVATE Magnum::TgaImageConverter)
@endcode

See @ref building, @ref cmake, @ref plugins and @ref file-formats for more
information.

@section Trade-TgaImageConverter-behavior Behavior and limitations

The output is always uncompressed. If you want to make use of RLE compression
and have the files smaller, use the @ref StbImageConverter plugin instead.
*/
class MAGNUM_TGAIMAGECONVERTER_EXPORT TgaImageConverter: public AbstractImageConverter {
    public:
        /** @brief Default constructor */
        explicit TgaImageConverter();

        /** @brief Plugin manager constructor */
        explicit TgaImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

    private:
        ImageConverterFeatures MAGNUM_TGAIMAGECONVERTER_LOCAL doFeatures() const override;
        Containers::Array<char> MAGNUM_TGAIMAGECONVERTER_LOCAL doExportToData(const ImageView2D& image) override;
};

}}

#endif
