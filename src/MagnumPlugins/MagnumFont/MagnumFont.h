#ifndef Magnum_Text_MagnumFont_h
#define Magnum_Text_MagnumFont_h
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
 * @brief Class @ref Magnum::Text::MagnumFont
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Text/AbstractFont.h"
#include "MagnumPlugins/MagnumFont/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_MAGNUMFONT_BUILD_STATIC
    #ifdef MagnumFont_EXPORTS
        #define MAGNUM_MAGNUMFONT_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_MAGNUMFONT_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_MAGNUMFONT_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_MAGNUMFONT_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_MAGNUMFONT_EXPORT
#define MAGNUM_MAGNUMFONT_LOCAL
#endif

namespace Magnum { namespace Text {

/**
@brief Simple bitmap font plugin

The font consists of two files, one text file containing character and glyph
info and one TGA file containing the glyphs in distance field format. The font
can be conveniently created from any other format using
@ref MagnumFontConverter. The file syntax is as in following:

@code{.ini}
# Font image filename
image=font.tga

# Size of unscaled font image
originalImageSize=1536 1536

# Glyph padding
padding=9

# Font size
fontSize=128

# Line height
lineHeight=270

# Character
[char]

# UTF-32 codepoint ('a')
unicode=0061

# Glyph ID
glyph=1

# Advance to next character in pixels (i.e. on unscaled font image)
advance=45 0

# Another character
[char]
unicode=0062
glyph=2
advance=42 0

# ...

# Glyph 0, a.k.a. "Not Found" glyph
[glyph]

# Glyph texture position relative to baseline, in pixels
position=5 -1

# Glyph rectangle in font image, in pixels (left, bottom, right, top)
rectangle=0 0 42 25

# Glyph 1
[glyph]
position=0 0
rectangle=45 0 44 25

# ...
@endcode

@section Text-MagnumFont-usage Usage

This plugin depends on the @ref Text library and the
@ref Trade::TgaImporter "TgaImporter" plugin. It is built if `WITH_MAGNUMFONT`
is enabled when building Magnum. To use as a dynamic plugin, load
@cpp "MagnumFont" @ce via @ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_MAGNUMFONT ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::MagnumFont)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `MagnumFont` component of the `Magnum` package and link to
the `Magnum::MagnumFont` target:

@code{.cmake}
find_package(Magnum REQUIRED MagnumFont)

# ...
target_link_libraries(your-app PRIVATE Magnum::MagnumFont)
@endcode

Because the plugin needs access to @ref Trade::AbstractImporter plugins, you
need to instantiate a manager for them and register it with
@ref Corrade::PluginManager::Manager::registerExternalManager():

@snippet plugins.cpp MagnumFont-importer-register

See @ref building, @ref cmake and @ref plugins for more information.
*/
class MAGNUM_MAGNUMFONT_EXPORT MagnumFont: public AbstractFont {
    public:
        /** @brief Default constructor */
        explicit MagnumFont();

        /** @brief Plugin manager constructor */
        explicit MagnumFont(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~MagnumFont();

    private:
        MAGNUM_MAGNUMFONT_LOCAL FontFeatures doFeatures() const override;
        MAGNUM_MAGNUMFONT_LOCAL bool doIsOpened() const override;
        MAGNUM_MAGNUMFONT_LOCAL Metrics doOpenData(Containers::ArrayView<const char> data, Float) override;
        MAGNUM_MAGNUMFONT_LOCAL Metrics doOpenFile(const std::string& filename, Float) override;
        MAGNUM_MAGNUMFONT_LOCAL void doClose() override;

        MAGNUM_MAGNUMFONT_LOCAL UnsignedInt doGlyphId(char32_t character) override;
        MAGNUM_MAGNUMFONT_LOCAL Vector2 doGlyphAdvance(UnsignedInt glyph) override;
        MAGNUM_MAGNUMFONT_LOCAL Containers::Pointer<AbstractGlyphCache> doCreateGlyphCache() override;
        MAGNUM_MAGNUMFONT_LOCAL Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache& cache, Float size, const std::string& text) override;

        struct Data;
        Containers::Pointer<Data> _opened;
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
