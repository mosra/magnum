/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2026 Andrew Snyder <asnyder@minitab.com>

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

#include <string> /** @todo remove once file callbacks are <string>-free */
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once file callbacks are <string>-free */
#include <Corrade/PluginManager/Manager.hpp>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Range.h"
#endif

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Text/configure.h"
#endif

namespace Corrade { namespace PluginManager {

template class MAGNUM_TEXT_EXPORT Manager<Magnum::Text::AbstractFont>;

}}

namespace Magnum { namespace Text {

using namespace Containers::Literals;

Debug& operator<<(Debug& debug, const FontFeature value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Text::FontFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FontFeature::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
        _c(OpenData)
        _c(FileCallback)
        _c(PreparedGlyphCache)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const FontFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Text::FontFeatures{}", {
        FontFeature::OpenData,
        FontFeature::FileCallback,
        FontFeature::PreparedGlyphCache
    });
}

Debug& operator<<(Debug& debug, const DataFlag value) {
    debug << "Text::DataFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case DataFlag::v: return debug << "::" #v;
        _c(Owned)
        _c(ExternallyOwned)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DataFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Text::DataFlags{}", {
        DataFlag::Owned,
        DataFlag::ExternallyOwned
    });
}

Containers::StringView AbstractFont::pluginInterface() {
    return MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE ""_s;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
Containers::Array<Containers::String> AbstractFont::pluginSearchPaths() {
    const Containers::Optional<Containers::String> libraryLocation = Utility::Path::libraryLocation(&pluginInterface);
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        libraryLocation ? *libraryLocation : Containers::String{},
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
        "fonts"_s);
}
#endif

AbstractFont::AbstractFont() = default;

AbstractFont::AbstractFont(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractPlugin{manager, plugin} {}

void AbstractFont::setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* const userData) {
    CORRADE_ASSERT(!isOpened(), "Text::AbstractFont::setFileCallback(): can't be set while a font is opened", );
    CORRADE_ASSERT(features() & (FontFeature::FileCallback|FontFeature::OpenData), "Text::AbstractFont::setFileCallback(): font plugin supports neither loading from data nor via callbacks, callbacks can't be used", );

    _fileCallback = callback;
    _fileCallbackUserData = userData;
    doSetFileCallback(callback, userData);
}

void AbstractFont::doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) {}

Containers::Optional<UnsignedInt> AbstractFont::dataFontCount(const Containers::ArrayView<const void> data) {
    CORRADE_ASSERT(features() & FontFeature::OpenData,
        "Text::AbstractFont::dataFontCount(): feature not supported", {});
    /* Similarly to openData() we accept empty data here (instead of checking
       for them and failing so the check doesn't be done on the plugin side)
       because for some file formats it could be valid (MagnumFont in
       particular). */
    const Containers::Optional<UnsignedInt> count = doDataFontCount(Containers::arrayCast<const char>(data));
    CORRADE_ASSERT(count != 0,
        "Text::AbstractFont::dataFontCount(): implementation returned zero", {});
    return count;
}

Containers::Optional<UnsignedInt> AbstractFont::doDataFontCount(Containers::ArrayView<const char>) {
    return 1;
}

#ifdef MAGNUM_BUILD_DEPRECATED
/* On deprecated builds, doOpenData() / doOpenFile() returns the Properties
   instead of being returned through doProperties(). For compatibility we save
   contents of the returned struct to the _size etc. members and default
   implementation of this function then returns them back. */
/** @todo remove this once deprecated doOpenData() / doOpenFile() is gone */
AbstractFont::Properties AbstractFont::doProperties() {
    /* If a plugin never implements doProperties(), and doesn't use the
       deprecated doOpenData() / doOpenFile() implementations that return a
       Properties struct, the glyph count stays set to the original value,
       triggering this assert. If it implements doProperties(), this function
       never gets called, if it uses the deprecated doOpenData() / doOpenFile()
       the value gets overwritten to something else. */
    CORRADE_ASSERT(_glyphCount != ~UnsignedInt{},
        "Text::AbstractFont: doProperties() not implemented", {});

    return {_size, _ascent, _descent, _lineHeight, _glyphCount};
}
#endif

bool AbstractFont::openData(const Containers::ArrayView<const void> data, const Float size, const UnsignedInt fontId) {
    CORRADE_ASSERT(features() & FontFeature::OpenData,
        "Text::AbstractFont::openData(): feature not supported", false);

    /* We accept empty data here (instead of checking for them and failing so
       the check doesn't be done on the plugin side) because for some file
       formats it could be valid (MagnumFont in particular). */
    close();
    doOpenData(Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), [](char*, std::size_t) {}}, {}, size, fontId);

    /* If opening succeeded, cache reported font properties. If not, the values
       were reset to default-constructed values by close(). (OTOH, on
       deprecated builds the properties are saved to the _size etc. members
       from the deprecated doOpenData() return value, and if doProperties()
       isn't implemented, it returns the contents of _size etc. thus there the
       following is effectively a no-op, assigning values that are already
       there.) */
    if(isOpened()) {
        const Properties properties = doProperties();
        _size = properties.size;
        _ascent = properties.ascent;
        _descent = properties.descent;
        _lineHeight = properties.lineHeight;
        _glyphCount = properties.glyphCount;
        return true;
    }

    return false;
}

void AbstractFont::doOpenData(Containers::Array<char>&& data, DataFlags, const Float size, const UnsignedInt fontId) {
    #ifndef MAGNUM_BUILD_DEPRECATED
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFont::openData(): feature advertised but not implemented", );
    static_cast<void>(data);
    static_cast<void>(size);
    static_cast<void>(fontId);
    #else
    /* If this function is not implemented, fall back to the deprecated
       overload that doesn't take a font ID if the requested ID is 0, and fail
       otherwise. */
    if(fontId != 0) {
        Error() << "Text::AbstractFont::openData(): cannot open font at index" << fontId;
        return;
    }

    /* The deprecated overload returns the properties instead of exposing them
       through doProperties(). Save them to the _size etc. members, the default
       doProperties() implementation then returns those back. */
    CORRADE_IGNORE_DEPRECATED_PUSH
    const Properties properties = doOpenData(data, size);
    CORRADE_IGNORE_DEPRECATED_POP
    _size = properties.size;
    _ascent = properties.ascent;
    _descent = properties.descent;
    _lineHeight = properties.lineHeight;
    _glyphCount = properties.glyphCount;
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
auto AbstractFont::doOpenData(Containers::ArrayView<const char>, Float) -> Properties {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFont::openData(): feature advertised but not implemented", {});
}
#endif

bool AbstractFont::openMemory(Containers::ArrayView<const void> memory, const Float size, const UnsignedInt fontId) {
    CORRADE_ASSERT(features() & FontFeature::OpenData,
        "Text::AbstractFont::openData(): feature not supported", false);

    /* We accept empty data here (instead of checking for them and failing so
       the check doesn't be done on the plugin side) because for some file
       formats it could be valid (MagnumFont in particular). */
    close();
    doOpenData(Containers::Array<char>{const_cast<char*>(static_cast<const char*>(memory.data())), memory.size(), [](char*, std::size_t) {}}, DataFlag::ExternallyOwned, size, fontId);

    /* If opening succeeded, cache reported font properties. Same logic as in
       openData() */
    if(isOpened()) {
        const Properties properties = doProperties();
        _size = properties.size;
        _ascent = properties.ascent;
        _descent = properties.descent;
        _lineHeight = properties.lineHeight;
        _glyphCount = properties.glyphCount;
        return true;
    }

    return false;
}

Containers::Optional<UnsignedInt> AbstractFont::fileFontCount(const Containers::StringView filename) {
    /* The logic here is mirroring what's in openFile(), just with delegating
       to different APIs. Comments are kept whole, just referring to different
       functions, as it's easier to reason about that way. */

    Containers::Optional<UnsignedInt> count;

    /* If file loading callbacks are not set or the font implementation
       supports handling them directly, call into the implementation */
    if(!_fileCallback || (doFeatures() & FontFeature::FileCallback)) {
        count = doFileFontCount(filename);

    /* Otherwise, if loading from data is supported, use the callback and pass
       the data through to dataFontCount(). Mark the file as ready to be closed
       once opening is finished. */
    } else if(doFeatures() & FontFeature::OpenData) {
        /* This needs to be duplicated here and in the doFileFontCount()
           implementation in order to support both following cases:
            - plugins that don't support FileCallback but have their own
              doFileFontCount() implementation (callback needs to be used here,
              because the base doFileFontCount() implementation might never get
              called)
            - plugins that support FileCallback but want to delegate the actual
              file loading to the default implementation (callback used in the
              base doFileFontCount() implementation, because this branch is
              never taken in that case) */
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Text::AbstractFont::fileFontCount(): cannot open file" << filename;
            return {};
        }

        count = doDataFontCount(*data);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    CORRADE_ASSERT(count != 0,
        "Text::AbstractFont::fileFontCount(): implementation returned zero", {});
    return count;
}

Containers::Optional<UnsignedInt> AbstractFont::doFileFontCount(const Containers::StringView filename) {
    /* The logic here is mirroring what's in doOpenFile(), just with delegating
       to different APIs and having a non-asserting default implementation.
       Comments are kept whole, just referring to different functions, as it's
       easier to reason about that way. */

    if(!(features() & FontFeature::OpenData))
        return 1;

    Containers::Optional<UnsignedInt> count;

    /* If callbacks are set, use them. This is the same implementation as in
       openFile(), see the comment there for details. */
    if(_fileCallback) {
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Text::AbstractFont::fileFontCount(): cannot open file" << filename;
            return {};
        }

        count = doDataFontCount(*data);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Otherwise open the file directly */
    } else {
        const Containers::Optional<Containers::Array<char>> data = Utility::Path::read(filename);
        if(!data) {
            Error() << "Text::AbstractFont::fileFontCount(): cannot open file" << filename;
            return {};
        }

        count = doDataFontCount(*data);
    }

    return count;
}

bool AbstractFont::openFile(const Containers::StringView filename, const Float size, const UnsignedInt fontId) {
    close();

    /* If file loading callbacks are not set or the font implementation
       supports handling them directly, call into the implementation */
    if(!_fileCallback || (doFeatures() & FontFeature::FileCallback)) {
        #ifdef MAGNUM_BUILD_DEPRECATED
        /* Call the deprecated doOpenFile() if fontId is 0, to make plugins
           that didn't get adapted to the new APIs still work. If a plugin
           doesn't implement the deprecated doOpenFile(), it delegates back to
           the new doOpenFile() implementation. */
        if(fontId == 0) {
            /* The deprecated overload returns the properties instead of
               exposing them through doProperties(). Save them to the _size
               etc. members, the default doProperties() implementation then
               returns those back. */
            CORRADE_IGNORE_DEPRECATED_PUSH
            const Properties properties = doOpenFile(filename, size);
            CORRADE_IGNORE_DEPRECATED_POP
            _size = properties.size;
            _ascent = properties.ascent;
            _descent = properties.descent;
            _lineHeight = properties.lineHeight;
            _glyphCount = properties.glyphCount;
        } else
        #endif
        {
            doOpenFile(filename, size, fontId);
        }

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

        doOpenData(Containers::Array<char>{const_cast<char*>(data->data()), data->size(), [](char*, std::size_t) {}}, {}, size, fontId);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* If opening succeeded, cache reported font properties. If not, the values
       were reset to default-constructed values by close(). (OTOH, on
       deprecated builds the properties are saved to the _size etc. members
       from the deprecated doOpenData() return value, and if doProperties()
       isn't implemented, it returns the contents of _size etc. thus there the
       following is effectively a no-op, assigning values that are already
       there.) */
    if(isOpened()) {
        const Properties properties = doProperties();
        _size = properties.size;
        _ascent = properties.ascent;
        _descent = properties.descent;
        _lineHeight = properties.lineHeight;
        _glyphCount = properties.glyphCount;
        return true;
    }

    return false;
}

void AbstractFont::doOpenFile(const Containers::StringView filename, const Float size, const UnsignedInt fontId) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    /* If this function is not implemented and opening data isn't supported
       either, assume the plugin implements only the deprecated doOpenFile()
       and fail gracefully for a non-zero font ID instead of asserting below.
       The reasoning is that plugin implementations should *never* assert for a
       font ID out of bounds, no matter whether they were updated to the new
       API or not. */
    if(!(features() & FontFeature::OpenData) && fontId != 0) {
        Error() << "Text::AbstractFont::openFile(): cannot open font at index" << fontId;
        return;
    }
    #endif

    CORRADE_ASSERT(features() & FontFeature::OpenData,
        "Text::AbstractFont::openFile(): not implemented", );

    /* If callbacks are set, use them. This is the same implementation as in
       openFile(), see the comment there for details. */
    if(_fileCallback) {
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Text::AbstractFont::openFile(): cannot open file" << filename;
            return;
        }

        doOpenData(Containers::Array<char>{const_cast<char*>(data->data()), data->size(), [](char*, std::size_t) {}}, {}, size, fontId);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Otherwise open the file directly */
    } else {
        Containers::Optional<Containers::Array<char>> data = Utility::Path::read(filename);
        if(!data) {
            Error() << "Text::AbstractFont::openFile(): cannot open file" << filename;
            return;
        }

        doOpenData(*Utility::move(data), DataFlag::Owned, size, fontId);
    }
}

#ifdef MAGNUM_BUILD_DEPRECATED
auto AbstractFont::doOpenFile(const Containers::StringView filename, const Float size) -> Properties {
    /* This function gets called from openFile() if fontId is 0, to call a
       potential implementation in old plugins that didn't get adapted to the
       new APIs yet. If a plugin doesn't implement doOpenFile() at all, this
       function delegates to the default implementation that then delegates to
       doOpenData() (which then again delegates to deprecated doOpenData(). If
       a plugin implements the new doOpenFile(), that implementation gets
       called from here for fontId 0, and from openFile() for all other font
       IDs. */
    doOpenFile(filename, size, 0);

    /* If opening suceeded, return the properties that the implementation
       exposes (or, if the above resulted in deprecated doOpenData() being
       called, properties that got saved from its return value) */
    if(isOpened())
        return doProperties();

    /* Otherwise return default-constructed properties, as doProperties() are
       meant to be called only if a file is opened */
    return {};
}
#endif

void AbstractFont::close() {
    if(!isOpened())
        return;

    doClose();
    CORRADE_INTERNAL_ASSERT(!isOpened());

    /* Clear the saved values to avoid accidental use of stale state (even
       though their public access is guarded with isOpened()) */
    _size = {};
    _lineHeight = {};
    _descent = {};
    _lineHeight = {};
    _glyphCount = {};
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

UnsignedInt AbstractFont::glyphCount() const {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphCount(): no font opened", 0);
    return _glyphCount;
}

UnsignedInt AbstractFont::glyphId(const char32_t character) {
    const char32_t characters[]{character};
    UnsignedInt glyphs[1];
    glyphIdsInto(characters, glyphs);
    return *glyphs;
}

void AbstractFont::glyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::glyphIdsInto(): no font opened", );
    CORRADE_ASSERT(glyphs.size() == characters.size(),
        "Text::AbstractFont::glyphIdsInto(): expected the characters and glyphs views to have the same size but got" << characters.size() << "and" << glyphs.size(), );

    doGlyphIdsInto(characters, glyphs);
    #ifndef CORRADE_NO_DEBUG_ASSERT
    for(std::size_t i = 0; i != characters.size(); ++i) {
        CORRADE_DEBUG_ASSERT(glyphs[i] < _glyphCount,
            "Text::AbstractFont::glyphIdsInto(): implementation-returned index" << glyphs[i] << "for character" << characters[i] << "out of range for" << _glyphCount << "glyphs", );
    }
    #endif
}

Containers::String AbstractFont::glyphName(const UnsignedInt glyph) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphName(): no font opened", {});
    CORRADE_ASSERT(glyph < _glyphCount, "Text::AbstractFont::glyphName(): index" << glyph << "out of range for" << _glyphCount << "glyphs", {});

    return doGlyphName(glyph);
}

Containers::String AbstractFont::doGlyphName(UnsignedInt) { return {}; }

UnsignedInt AbstractFont::glyphForName(const Containers::StringView name) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphForName(): no font opened", {});

    const UnsignedInt glyph = doGlyphForName(name);
    CORRADE_ASSERT(glyph < _glyphCount, "Text::AbstractFont::glyphForName(): implementation-returned index" << glyph << "out of range for" << _glyphCount << "glyphs", {});

    return glyph;
}

UnsignedInt AbstractFont::doGlyphForName(Containers::StringView) { return 0; }

Vector2 AbstractFont::glyphSize(const UnsignedInt glyph) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphSize(): no font opened", {});
    CORRADE_ASSERT(glyph < _glyphCount, "Text::AbstractFont::glyphSize(): index" << glyph << "out of range for" << _glyphCount << "glyphs", {});

    return doGlyphSize(glyph);
}

Vector2 AbstractFont::glyphAdvance(const UnsignedInt glyph) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::glyphAdvance(): no font opened", {});
    CORRADE_ASSERT(glyph < _glyphCount, "Text::AbstractFont::glyphAdvance(): index" << glyph << "out of range for" << _glyphCount << "glyphs", {});

    return doGlyphAdvance(glyph);
}

bool AbstractFont::fillGlyphCache(AbstractGlyphCache& cache, const Containers::StringView characters) {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::fillGlyphCache(): no font opened", {});
    CORRADE_ASSERT(!(features() & FontFeature::PreparedGlyphCache),
        "Text::AbstractFont::fillGlyphCache(): feature not supported", {});

    struct Glyph {
        char32_t character;
        UnsignedInt glyph;
    };

    /* Convert UTF-8 to Unicode codepoints */
    Containers::Array<Glyph> glyphs;
    arrayReserve(glyphs, characters.size());
    for(std::size_t i = 0; i != characters.size(); ) {
        const Containers::Pair<char32_t, std::size_t> next = Utility::Unicode::nextChar(characters, i);
        CORRADE_ASSERT(next.first() != U'\xffffffff',
            "Text::AbstractFont::fillGlyphCache(): not a valid UTF-8 string:" << characters, {});
        arrayAppend(glyphs, InPlaceInit, next.first(), 0u);
        i = next.second();
    }

    /* Convert the codepoints to glyph IDs */
    glyphIdsInto(stridedArrayView(glyphs).slice(&Glyph::character),
                 stridedArrayView(glyphs).slice(&Glyph::glyph));

    /* If this font isn't in the cache yet, include also the invalid glyph */
    if(!cache.findFont(*this))
        arrayAppend(glyphs, InPlaceInit, U'\0', 0u);

    /* Create a unique (ordered) set */
    /** @todo reuse the memory from `glyphs` for this somehow? tho there could
        be thousands of glyphs and the `glyphs` might be just a few entries */
    Containers::BitArray uniqueGlyphs{ValueInit, _glyphCount};
    for(const Glyph& glyph: glyphs)
        uniqueGlyphs.set(glyph.glyph);

    /* Convert the unique set back to a list of glyph IDs, reusing the original
       glyph memory */
    const std::size_t uniqueCount = uniqueGlyphs.count();
    CORRADE_INTERNAL_ASSERT(uniqueCount <= glyphs.size());
    std::size_t offset = 0;
    for(UnsignedInt i = 0; i != uniqueGlyphs.size(); ++i)
        if(uniqueGlyphs[i])
            glyphs[offset++].glyph = i;
    CORRADE_INTERNAL_ASSERT(offset == uniqueCount);

    /* Pass the unique set to the implementation */
    return doFillGlyphCache(cache, stridedArrayView(glyphs).slice(&Glyph::glyph).prefix(uniqueCount));
}

bool AbstractFont::fillGlyphCache(AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const UnsignedInt>& glyphs) {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::fillGlyphCache(): no font opened", {});
    CORRADE_ASSERT(!(features() & FontFeature::PreparedGlyphCache),
        "Text::AbstractFont::fillGlyphCache(): feature not supported", {});

    #ifndef CORRADE_NO_DEBUG_ASSERT
    Containers::BitArray uniqueGlyphs{ValueInit, _glyphCount};
    for(const UnsignedInt& glyph: glyphs) {
        CORRADE_DEBUG_ASSERT(glyph < _glyphCount,
            "Text::AbstractFont::fillGlyphCache(): index" << glyph << "out of range for" << _glyphCount << "glyphs", {});
        CORRADE_DEBUG_ASSERT(!uniqueGlyphs[glyph],
            "Text::AbstractFont::fillGlyphCache(): duplicate glyph" << glyph, {});
        uniqueGlyphs.set(glyph);
    }
    #endif

    return doFillGlyphCache(cache, glyphs);
}

bool AbstractFont::fillGlyphCache(AbstractGlyphCache& cache, const std::initializer_list<UnsignedInt> glyphs) {
    return fillGlyphCache(cache, Containers::stridedArrayView(glyphs));
}

bool AbstractFont::doFillGlyphCache(AbstractGlyphCache&, const Containers::StridedArrayView1D<const UnsignedInt>&) {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented", {});
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

Containers::Pointer<AbstractShaper> AbstractFont::createShaper() {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::createShaper(): no font opened", {});
    Containers::Pointer<AbstractShaper> out = doCreateShaper();
    CORRADE_ASSERT(out,
        "Text::AbstractFont::createShaper(): implementation returned nullptr", {});
    return out;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Containers::Pointer<AbstractLayouter> AbstractFont::layout(const AbstractGlyphCache& cache, const Float size, const Containers::StringView text) {
    CORRADE_ASSERT(isOpened(),
        "Text::AbstractFont::layout(): no font opened", {});
    /* This was originally added as a runtime error into plugin
       implementations during the transition period for the new
       AbstractGlyphCache API, now it's an assert. Shouldn't get triggered by
       existing code in practice. */
    CORRADE_ASSERT(cache.size().z() == 1,
        "Text::AbstractFont::layout(): array glyph caches are not supported", {});

    /* Find this font in the cache. This is kept as a runtime error however. */
    Containers::Optional<UnsignedInt> fontId = cache.findFont(*this);
    if(!fontId) {
        Error{} << "Text::AbstractFont::layout(): font not found among" << cache.fontCount() << "fonts in passed glyph cache";
        return {};
    }

    /* Ignoring the failures in this case, as the old API was never failing
       also -- it'll simply return an empty AbstractLayouter */
    Containers::Pointer<AbstractShaper> shaper = createShaper();
    shaper->shape(text);

    /* Scaling factor */
    const Float scale = size/this->size();

    /* Get the glyph data. Yes, this is one extra temporary allocation which
       could be aliased with the output array, but for the deprecated API implementation should be as unsurprising and unclever as possible. */
    struct Glyph {
        UnsignedInt id;
        Vector2 offset;
        Vector2 advance;
    };
    Containers::Array<Glyph> glyphs{NoInit, shaper->glyphCount()};
    shaper->glyphIdsInto(
        stridedArrayView(glyphs).slice(&Glyph::id));
    shaper->glyphOffsetsAdvancesInto(
        stridedArrayView(glyphs).slice(&Glyph::offset),
        stridedArrayView(glyphs).slice(&Glyph::advance));

    /* Create the data to return from AbstractLayouter::renderGlyph(). Most of
       this used to be copypasted in various *Layouter::doRenderGlyph()
       implementations, ugh. */
    Containers::Array<Containers::Triple<Range2D, Range2D, Vector2>> out{NoInit, glyphs.size()};
    for(std::size_t i = 0; i != glyphs.size(); ++i) {
        /* Offset of the glyph rectangle relative to the cursor, layer, texture
           coordinates. We checked that the glyph cache is 2D above so the
           layer can be ignored. */
        const Containers::Triple<Vector2i, Int, Range2Di> cacheGlyph = cache.glyph(*fontId, glyphs[i].id);
        CORRADE_INTERNAL_ASSERT(cacheGlyph.second() == 0);

        out[i] = {
            /* Quad rectangle, created from cache and shaper offset and the
               texture rectangle, scaled to requested text size */
            Range2D::fromSize(Vector2{cacheGlyph.first()} + glyphs[i].offset,
                              Vector2{cacheGlyph.third().size()})
                .scaled(Vector2{scale}),

            /* Normalized texture coordinates */
            Range2D{cacheGlyph.third()}
                .scaled(1.0f/Vector2{cache.size().xy()}),

            /* Advance from the font, again scaled */
            glyphs[i].advance*scale
        };
    }

    return Containers::pointer<AbstractLayouter>(Utility::move(out));
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
AbstractLayouter::AbstractLayouter(Containers::Array<Containers::Triple<Range2D, Range2D, Vector2>>&& glyphs): _glyphs{Utility::move(glyphs)} {}

AbstractLayouter::~AbstractLayouter() = default;

Containers::Pair<Range2D, Range2D> AbstractLayouter::renderGlyph(const UnsignedInt i, Vector2& cursorPosition, Range2D& rectangle) {
    CORRADE_ASSERT(i < _glyphs.size(),
        "Text::AbstractLayouter::renderGlyph(): index" << i << "out of range for" << _glyphs.size() << "glyphs", {});

    /* Move the quad to cursor */
    const Range2D quadPosition = _glyphs[i].first().translated(cursorPosition);

    /* Extend the rectangle with current quad bounds. If the original is zero
       size, it gets replaced. */
    rectangle = Math::join(rectangle, quadPosition);

    /* Advance cursor position to next character */
    cursorPosition += _glyphs[i].third();

    /* Return moved quad and unchanged texture coordinates */
    return {quadPosition, _glyphs[i].second()};
}
#endif

}}
