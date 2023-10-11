/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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

#include "AbstractShaper.h"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Text/Script.h"

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const Direction value) {
    debug << "Text::Direction" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Direction::v: return debug << "::" #v;
        _c(Unspecified)
        _c(LeftToRight)
        _c(RightToLeft)
        _c(TopToBottom)
        _c(BottomToTop)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

AbstractShaper::AbstractShaper(AbstractFont& font): _font(font), _glyphCount{0} {}

AbstractShaper::AbstractShaper(AbstractShaper&&) noexcept = default;

AbstractShaper::~AbstractShaper() = default;

AbstractShaper& AbstractShaper::operator=(AbstractShaper&&) noexcept = default;

bool AbstractShaper::setScript(const Script script) {
    return doSetScript(script);
}

bool AbstractShaper::doSetScript(Script) { return false; }

bool AbstractShaper::setLanguage(const Containers::StringView language) {
    return doSetLanguage(language);
}

bool AbstractShaper::doSetLanguage(Containers::StringView) { return false; }

bool AbstractShaper::setDirection(const Direction direction) {
    return doSetDirection(direction);
}

bool AbstractShaper::doSetDirection(Direction) { return false; }

UnsignedInt AbstractShaper::shape(const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const Containers::ArrayView<const FeatureRange> features) {
    CORRADE_ASSERT((end == ~UnsignedInt{} && begin <= text.size()) ||
                   (begin <= end && end <= text.size()),
        "Text::AbstractShaper::shape(): begin" << begin << "and end" << end << "out of range for a text of" << text.size() << "bytes", {});
    CORRADE_ASSERT(begin < text.size() && begin != end,
        "Text::AbstractShaper::shape(): shaped text at begin" << begin << "is empty", {});
    #ifndef CORRADE_NO_ASSERT
    for(std::size_t i = 0; i != features.size(); ++i) {
        const FeatureRange& feature = features[i];
        CORRADE_ASSERT(
            (feature._end == ~UnsignedInt{} && feature._begin <= text.size()) ||
            (feature._begin <= feature._end && feature._end <= text.size()),
            "Text::AbstractShaper::shape(): feature" << i << "begin" << feature._begin << "and end" << feature._end << "out of range for a text of" << text.size() << "bytes", {});
        /** @todo catch empty feature ranges too? or not important */
    }
    #endif
    return _glyphCount = doShape(text, begin, end, features);
}

UnsignedInt AbstractShaper::shape(const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const std::initializer_list<FeatureRange> features) {
    return shape(text, begin, end, Containers::arrayView(features));
}

UnsignedInt AbstractShaper::shape(const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end) {
    return shape(text, begin, end, nullptr);
}

UnsignedInt AbstractShaper::shape(const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    return shape(text, 0, ~UnsignedInt{}, features);
}

UnsignedInt AbstractShaper::shape(const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return shape(text, Containers::arrayView(features));
}

UnsignedInt AbstractShaper::shape(const Containers::StringView text) {
    return shape(text, nullptr);
}

Script AbstractShaper::script() const {
    return _glyphCount ? doScript() : Script::Unspecified;
}

Script AbstractShaper::doScript() const { return Script::Unspecified; }

Containers::StringView AbstractShaper::language() const {
    return _glyphCount ? doLanguage() : Containers::StringView{};
}

Containers::StringView AbstractShaper::doLanguage() const { return {}; }

Direction AbstractShaper::direction() const {
    return _glyphCount ? doDirection() : Direction::Unspecified;
}

Direction AbstractShaper::doDirection() const { return Direction::Unspecified; }

void AbstractShaper::glyphsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids, const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const {
    CORRADE_ASSERT(ids.size() == _glyphCount && offsets.size() == _glyphCount && advances.size() == _glyphCount,
        "Text::AbstractShaper::glyphsInto(): expected the ids, offsets and advanced views to have a size of" << _glyphCount << "but got" << ids.size() << Debug::nospace << "," << offsets.size() << "and" << advances.size(), );
    /* Call into the implementation only if there's actually anything shaped,
       otherwise it might not yet have everything properly set up */
    if(_glyphCount)
        doGlyphsInto(ids, offsets, advances);
}

}}
