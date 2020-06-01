#ifndef Magnum_Text_MagnumFontConverter_h
#define Magnum_Text_MagnumFontConverter_h
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
 * @brief Class @ref Magnum::Text::MagnumFontConverter
 */

#include "Magnum/Text/AbstractFontConverter.h"

#include "MagnumPlugins/MagnumFontConverter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_MAGNUMFONTCONVERTER_BUILD_STATIC
    #ifdef MagnumFontConverter_EXPORTS
        #define MAGNUM_MAGNUMFONTCONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_MAGNUMFONTCONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_MAGNUMFONTCONVERTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_MAGNUMFONTCONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_MAGNUMFONTCONVERTER_EXPORT
#define MAGNUM_MAGNUMFONTCONVERTER_LOCAL
#endif

namespace Magnum { namespace Text {

/**
@brief MagnumFont converter plugin

Expects filename prefix, creates two files, `prefix.conf` and `prefix.tga`. See
@ref MagnumFont for more information about the font. The plugin requires the
passed @ref AbstractGlyphCache to support @ref GlyphCacheFeature::ImageDownload.

@section Text-MagnumFontConverter-usage Usage

This plugin depends on the @ref Text library and the
@ref Trade::TgaImageConverter "TgaImageConverter" plugin. It is built if
`WITH_MAGNUMFONTCONVERTER` is enabled when building Magnum. To use as a
dynamic plugin, load @cpp "MagnumFontConverter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_MAGNUMFONTCONVERTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::MagnumFontConverter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `MagnumFontConverter` component of the `Magnum` package and
link to the `Magnum::MagnumFontConverter` target:

@code{.cmake}
find_package(Magnum REQUIRED MagnumFontConverter)

# ...
target_link_libraries(your-app PRIVATE Magnum::MagnumFontConverter)
@endcode

Because the plugin needs access to @ref Trade::AbstractImageConverter plugins,
you need to instantiate a manager for them and register it with
@ref Corrade::PluginManager::Manager::registerExternalManager():

@snippet plugins.cpp MagnumFontConverter-imageconverter-register

See @ref building, @ref cmake and @ref plugins for more information.
*/
class MAGNUM_MAGNUMFONTCONVERTER_EXPORT MagnumFontConverter: public Text::AbstractFontConverter {
    public:
        /** @brief Default constructor */
        explicit MagnumFontConverter();

        /** @brief Plugin manager constructor */
        explicit MagnumFontConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

    private:
        MAGNUM_MAGNUMFONTCONVERTER_LOCAL FontConverterFeatures doFeatures() const override;
        MAGNUM_MAGNUMFONTCONVERTER_LOCAL std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::u32string& characters) const override;
};

}}

#endif
