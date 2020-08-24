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

#include "AbstractFont.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Text/AbstractGlyphCache.h"

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Text/configure.h"
#endif

namespace Magnum { namespace Text {

std::string AbstractFont::pluginInterface() {
    return
/* [interface] */
"cz.mosra.magnum.Text.AbstractFont/0.3"
/* [interface] */
    ;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractFont::pluginSearchPaths() {
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        Utility::Directory::libraryLocation(&pluginInterface),
        #else
        {},
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        MAGNUM_PLUGINS_FONT_DEBUG_DIR,
        #else
        MAGNUM_PLUGINS_FONT_DIR,
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/"
        #else
        "magnum/"
        #endif
        "fonts");
}
#endif

AbstractFont::AbstractFont() = default;

AbstractFont::AbstractFont(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractPlugin{manager, plugin} {}

void AbstractFont::setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* const userData) {
    CORRADE_ASSERT(!isOpened(), "Text::AbstractFont::setFileCallback(): can't be set while a font is opened", );
    CORRADE_ASSERT(features() & (FontFeature::FileCallback|FontFeature::OpenData), "Text::AbstractFont::setFileCallback(): font plugin supports neither loading from data nor via callbacks, callbacks can't be used", );

    _fileCallback = callback;
    _fileCallbackUserData = userData;
    doSetFileCallback(callback, userData);
}

void AbstractFont::doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) {}

bool AbstractFont::openData(Containers::ArrayView<const char> data, const Float size) {
    CORRADE_ASSERT(features() & FontFeature::OpenData,
        "Text::AbstractFont::openData(): feature not supported", false);

    /* We accept empty data here (instead of checking for them and failing so
       the check doesn't be done on the plugin side) because for some file
       formats it could be valid (MagnumFont in particular). */
    close();
    const Metrics metrics = doOpenData(data, size);
    _size = metrics.size;
    _ascent = metrics.ascent;
    _descent = metrics.descent;
    _lineHeight = metrics.lineHeight;
    CORRADE_INTERNAL_ASSERT(isOpened() || (!_size && !_ascent && !_descent && !_lineHeight));
    return isOpened();
}

auto AbstractFont::doOpenData(Containers::ArrayView<const char>, Float) -> Metrics {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFont::openData(): feature advertised but not implemented", {});
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractFont::openData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data, const Float size) {
    close();

    setFileCallback([](const std::string& file, InputFileCallbackPolicy, const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data) -> Containers::Optional<Containers::ArrayView<const char>> {
        for(auto&& i: data) if(i.first == file) return i.second;
        return {};
    }, data);

    return !data.empty() && openData(data.front().second, size);
}

bool AbstractFont::openSingleData(const Containers::ArrayView<const char> data, const Float size) {
    return openData(data, size);
}
#endif

bool AbstractFont::openFile(const std::string& filename, const Float size) {
    close();
    Metrics metrics;

    /* If file loading callbacks are not set or the font implementation
       supports handling them directly, call into the implementation */
    if(!_fileCallback || (doFeatures() & FontFeature::FileCallback)) {
        metrics = doOpenFile(filename, size);

    /* Otherwise, if loading from data is supported, use the callback and pass
       the data through to openData(). Mark the file as ready to be closed once
       opening is finished. */
    } else if(doFeatures() & FontFeature::OpenData) {
        /* This needs to be duplicated here and in the doOpenFile()
           implementation in order to support both following cases:
            - plugins that don't support FileCallback but have their own
              doOpenFile() implementation (callback needs to be used here,
              because the base doOpenFile() implementation might never get
              called)
            - plugins that support FileCallback but want to delegate the actual
              file loading to the default implementation (callback used in the
              base doOpenFile() implementation, because this branch is never
              taken in that case) */
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Text::AbstractFont::openFile(): cannot open file" << filename;
            return isOpened();
        }
        metrics = doOpenData(*data, size);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    _size = metrics.size;
    _ascent = metrics.ascent;
    _descent = metrics.descent;
    _lineHeight = metrics.lineHeight;
    CORRADE_INTERNAL_ASSERT(isOpened() || (!_size && !_ascent && !_descent && !_lineHeight));
    return isOpened();
}

auto AbstractFont::doOpenFile(const std::string& filename, const Float size) -> Metrics {
    CORRADE_ASSERT(features() & FontFeature::OpenData, "Text::AbstractFont::openFile(): not implemented", {});

    Metrics metrics;

    /* If callbacks are set, use them. This is the same implementation as in
       openFile(), see the comment there for details. */
    if(_fileCallback) {
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Text::AbstractFont::openFile(): cannot open file" << filename;
            return {};
        }
        metrics = doOpenData(*data, size);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Otherwise open the file directly */
    } else {
        if(!Utility::Directory::exists(filename)) {
            Error() << "Text::AbstractFont::openFile(): cannot open file" << filename;
            return {};
        }

        metrics = doOpenData(Utility::Directory::read(filename), size);
    }

    return metrics;
}

void AbstractFont::close() {
    if(isOpened()) {
        doClose();
        _size = 0.0f;
        _lineHeight = 0.0f;
        CORRADE_INTERNAL_ASSERT(!isOpened());
    }
}

Float AbstractFont::size() const {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::size(): no font opened", {});
    return _size;
}

Float AbstractFont::ascent() const {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::ascent(): no font opened", {});
    return _ascent;
}

Float AbstractFont::descent() const {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::descent(): no font opened", {});
    return _descent;
}

Float AbstractFont::lineHeight() const {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::lineHeight(): no font opened", {});
    return _lineHeight;
}

UnsignedInt AbstractFont::glyphId(const char32_t character) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphId(): no font opened", 0);

    return doGlyphId(character);
}

Vector2 AbstractFont::glyphAdvance(const UnsignedInt glyph) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphAdvance(): no font opened", {});

    return doGlyphAdvance(glyph);
}

void AbstractFont::fillGlyphCache(AbstractGlyphCache& cache, const std::string& characters) {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::fillGlyphCache(): no font opened", );
    CORRADE_ASSERT(!(features() & FontFeature::PreparedGlyphCache),
        "Text::AbstractFont::fillGlyphCache(): feature not supported", );

    doFillGlyphCache(cache, Utility::Unicode::utf32(characters));
}

void AbstractFont::doFillGlyphCache(AbstractGlyphCache&, const std::u32string&) {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented", );
}

Containers::Pointer<AbstractGlyphCache> AbstractFont::createGlyphCache() {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::createGlyphCache(): no font opened", nullptr);
    CORRADE_ASSERT(features() & FontFeature::PreparedGlyphCache,
        "Text::AbstractFont::createGlyphCache(): feature not supported", nullptr);

    return doCreateGlyphCache();
}

Containers::Pointer<AbstractGlyphCache> AbstractFont::doCreateGlyphCache() {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFont::createGlyphCache(): feature advertised but not implemented", nullptr);
}

Containers::Pointer<AbstractLayouter> AbstractFont::layout(const AbstractGlyphCache& cache, const Float size, const std::string& text) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::layout(): no font opened", nullptr);

    return doLayout(cache, size, text);
}

Debug& operator<<(Debug& debug, const FontFeature value) {
    debug << "Text::FontFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FontFeature::v: return debug << "::" #v;
        _c(OpenData)
        _c(FileCallback)
        _c(PreparedGlyphCache)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const FontFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Text::FontFeatures{}", {
        FontFeature::OpenData,
        FontFeature::FileCallback,
        FontFeature::PreparedGlyphCache});
}

AbstractLayouter::AbstractLayouter(UnsignedInt glyphCount): _glyphCount(glyphCount) {}

AbstractLayouter::~AbstractLayouter() = default;

std::pair<Range2D, Range2D> AbstractLayouter::renderGlyph(const UnsignedInt i, Vector2& cursorPosition, Range2D& rectangle) {
    CORRADE_ASSERT(i < glyphCount(), "Text::AbstractLayouter::renderGlyph(): glyph index out of bounds", {});

    /* Render the glyph */
    Range2D quadPosition, textureCoordinates;
    Vector2 advance;
    std::tie(quadPosition, textureCoordinates, advance) = doRenderGlyph(i);

    /* Move the quad to cursor */
    quadPosition.bottomLeft() += cursorPosition;
    quadPosition.topRight() += cursorPosition;

    /* Extend rectangle with current quad bounds. If zero size, replace it. */
    if(!rectangle.size().isZero()) {
        rectangle.bottomLeft() = Math::min(rectangle.bottomLeft(), quadPosition.bottomLeft());
        rectangle.topRight() = Math::max(rectangle.topRight(), quadPosition.topRight());
    } else rectangle = quadPosition;

    /* Advance cursor position to next character */
    cursorPosition += advance;

    /* Return moved quad and unchanged texture coordinates */
    return {quadPosition, textureCoordinates};
}

}}
