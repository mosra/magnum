#ifndef Magnum_Trade_AnyImageConverter_h
#define Magnum_Trade_AnyImageConverter_h
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
 * @brief Class @ref Magnum::Trade::AnyImageConverter
 */

#include "Magnum/Trade/AbstractImageConverter.h"
#include "MagnumPlugins/AnyImageConverter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYIMAGECONVERTER_BUILD_STATIC
    #ifdef AnyImageConverter_EXPORTS
        #define MAGNUM_ANYIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYIMAGECONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYIMAGECONVERTER_EXPORT
#define MAGNUM_ANYIMAGECONVERTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any image converter plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to convert the file with it. Supported formats for uncompressed data:

-   Basis Universal (`*.basis`), converted with @ref BasisImageConverter or any
    other plugin that provides it
-   Windows Bitmap (`*.bmp`), converted with any plugin that provides
    `BmpImageConverter`
-   OpenEXR (`*.exr`), converted with any plugin that provides
    `OpenExrImageConverter`
-   Radiance HDR (`*.hdr`), converted with any plugin that provides
    `HdrImageConverter`
-   JPEG (`*.jpg`, `*.jpe`, `*.jpeg`), converted with @ref JpegImageConverter
    or any other plugin that provides it
-   Portable Network Graphics (`*.png`), converted with @ref PngImageConverter
    or any other plugin that provides it
-   Truevision TGA (`*.tga`, `*.vda`, `*.icb`, `*.vst`), converted with
    @ref TgaImageConverter or any other plugin that provides it

No supported formats for compressed data yet. Only exporting to files is
supported.

@section Trade-AnyImageConverter-usage Usage

This plugin depends on the @ref Trade library and is built if
`WITH_ANYIMAGECONVERTER` is enabled when building Magnum. To use as a dynamic
plugin, load @cpp "AnyImageConverter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_ANYIMAGECONVERTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::AnyImageConverter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `AnyImageConverter` component of the `Magnum` package and
link to the `Magnum::AnyImageConverter` target:

@code{.cmake}
find_package(Magnum REQUIRED AnyImageConverter)

# ...
target_link_libraries(your-app PRIVATE Magnum::AnyImageConverter)
@endcode

See @ref building, @ref cmake, @ref plugins and @ref file-formats for more
information.
*/
class MAGNUM_ANYIMAGECONVERTER_EXPORT AnyImageConverter: public AbstractImageConverter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnyImageConverter(PluginManager::Manager<AbstractImageConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyImageConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~AnyImageConverter();

    private:
        MAGNUM_ANYIMAGECONVERTER_LOCAL ImageConverterFeatures doFeatures() const override;
        MAGNUM_ANYIMAGECONVERTER_LOCAL bool doExportToFile(const ImageView2D& image, const std::string& filename) override;
        MAGNUM_ANYIMAGECONVERTER_LOCAL bool doExportToFile(const CompressedImageView2D& image, const std::string& filename) override;
};

}}

#endif
