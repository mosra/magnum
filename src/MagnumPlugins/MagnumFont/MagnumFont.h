#ifndef Magnum_Text_MagnumFont_h
#define Magnum_Text_MagnumFont_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Class @ref Magnum::Text::MagnumFont
 */

#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Trade/Trade.h"

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

This plugin depends on the @ref Text library and the
@ref Trade::TgaImporter "TgaImporter" plugin. It is built if `WITH_MAGNUMFONT`
is enabled when building Magnum. To use as a dynamic plugin, you need to load
the @cpp "MagnumFont" @ce plugin from `MAGNUM_PLUGINS_FONT_DIR`. To use as a
static plugin or as a dependency of another plugin with CMake, you need to
request the `MagnumFont` component of the `Magnum` package and link to the
`Magnum::MagnumFont` target. See @ref building, @ref cmake and @ref plugins
for more information.

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
*/
class MAGNUM_MAGNUMFONT_EXPORT MagnumFont: public AbstractFont {
    public:
        /** @brief Default constructor */
        explicit MagnumFont();

        /** @brief Plugin manager constructor */
        explicit MagnumFont(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~MagnumFont();

    private:
        struct Data;

        MAGNUM_MAGNUMFONT_LOCAL Features doFeatures() const override;
        MAGNUM_MAGNUMFONT_LOCAL bool doIsOpened() const override;
        MAGNUM_MAGNUMFONT_LOCAL Metrics doOpenData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data, Float) override;
        MAGNUM_MAGNUMFONT_LOCAL Metrics doOpenFile(const std::string& filename, Float) override;
        MAGNUM_MAGNUMFONT_LOCAL void doClose() override;

        MAGNUM_MAGNUMFONT_LOCAL UnsignedInt doGlyphId(char32_t character) override;
        MAGNUM_MAGNUMFONT_LOCAL Vector2 doGlyphAdvance(UnsignedInt glyph) override;
        MAGNUM_MAGNUMFONT_LOCAL std::unique_ptr<GlyphCache> doCreateGlyphCache() override;
        MAGNUM_MAGNUMFONT_LOCAL std::unique_ptr<AbstractLayouter> doLayout(const GlyphCache& cache, Float size, const std::string& text) override;

        MAGNUM_MAGNUMFONT_LOCAL Metrics openInternal(Utility::Configuration&& conf, Trade::ImageData2D&& image);

        Data* _opened;
};

}}

#endif
