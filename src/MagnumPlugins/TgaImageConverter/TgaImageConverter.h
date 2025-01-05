#ifndef Magnum_Trade_TgaImageConverter_h
#define Magnum_Trade_TgaImageConverter_h
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

@m_class{m-note m-success}

@par
    This class is a plugin that's meant to be dynamically loaded and used
    via the base @ref AbstractImageConverter interface. See its documentation
    for introduction and usage examples.

This plugin depends on the @ref Trade library and is built if
`MAGNUM_WITH_TGAIMAGECONVERTER` is enabled when building Magnum. To use as a
dynamic plugin, load @cpp "TgaImageConverter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(MAGNUM_WITH_TGAIMAGECONVERTER ON CACHE BOOL "" FORCE)
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

The output is RLE-compressed by default unless the uncompressed output is
smaller than RLE. You can always produce uncompressed files by disabling the
@cb{.ini} rle @ce @ref Trade-TgaImageConverter-configuration "configuration option", and always RLE files by disabling the
@cb{.ini} rleFallbackIfLarger @ce option. Enabling
@cb{.ini} rleAcrossScanlines @ce can result in even smaller files but
[such files are considered invalid in the TGA 2.0 spec](https://en.wikipedia.org/wiki/Truevision_TGA#Specification_discrepancies)
and thus may cause issues in certain importers.

The TGA file format doesn't have a way to distinguish between 2D and 1D array
images. If an image has @ref ImageFlag2D::Array set, a warning is printed and
the file is saved as a regular 2D image.

While TGA files can have several extensions, @ref extension() always returns
@cpp "tga" @ce as that's the most common one. As TGA doesn't have a registered
MIME type, @ref mimeType() returns @cpp "image/x-tga" @ce.

The converter recognizes @ref ImageConverterFlag::Verbose, printing additional
info when the flag is enabled. @ref ImageConverterFlag::Quiet is recognized as
well and causes all conversion warnings to be suppressed.

@section Trade-TgaImageConverter-configuration Plugin-specific configuration

It's possible to tune various output options through @ref configuration(). See
below for all options and their default values:

@snippet MagnumPlugins/TgaImageConverter/TgaImageConverter.conf configuration_

See @ref plugins-configuration for more information and an example showing how
to edit the configuration values.
*/
class MAGNUM_TGAIMAGECONVERTER_EXPORT TgaImageConverter: public AbstractImageConverter {
    public:
        /** @brief Default constructor */
        explicit TgaImageConverter();

        /** @brief Plugin manager constructor */
        explicit TgaImageConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

    private:
        MAGNUM_TGAIMAGECONVERTER_LOCAL ImageConverterFeatures doFeatures() const override;
        MAGNUM_TGAIMAGECONVERTER_LOCAL Containers::String doExtension() const override;
        MAGNUM_TGAIMAGECONVERTER_LOCAL Containers::String doMimeType() const override;
        MAGNUM_TGAIMAGECONVERTER_LOCAL Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D& image) override;
};

}}

#endif
