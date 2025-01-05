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

#include "AbstractShaper.h"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Text/Direction.h"
#include "Magnum/Text/Feature.h"
#include "Magnum/Text/Script.h"

namespace Magnum { namespace Text {

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

bool AbstractShaper::setDirection(const ShapeDirection direction) {
    return doSetDirection(direction);
}

bool AbstractShaper::doSetDirection(ShapeDirection) { return false; }

UnsignedInt AbstractShaper::shape(const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const Containers::ArrayView<const FeatureRange> features) {
    CORRADE_ASSERT((end == ~UnsignedInt{} && begin <= text.size()) ||
                   (begin <= end && end <= text.size()),
        "Text::AbstractShaper::shape(): begin" << begin << "and end" << end << "out of range for a text of" << text.size() << "bytes", {});
    #ifndef CORRADE_NO_ASSERT
    for(std::size_t i = 0; i != features.size(); ++i) {
        const FeatureRange& feature = features[i];
        CORRADE_ASSERT(
            (feature._end == ~UnsignedInt{} && feature._begin <= text.size()) ||
            (feature._begin <= feature._end && feature._end <= text.size()),
            "Text::AbstractShaper::shape(): feature" << i << "begin" << feature._begin << "and end" << feature._end << "out of range for a text of" << text.size() << "bytes", {});
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
    return doScript();
}

Script AbstractShaper::doScript() const { return Script::Unspecified; }

Containers::StringView AbstractShaper::language() const {
    return doLanguage();
}

Containers::StringView AbstractShaper::doLanguage() const { return {}; }

ShapeDirection AbstractShaper::direction() const {
    return doDirection();
}

ShapeDirection AbstractShaper::doDirection() const {
    return ShapeDirection::Unspecified;
}

void AbstractShaper::glyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const {
    CORRADE_ASSERT(ids.size() == _glyphCount,
        "Text::AbstractShaper::glyphIdsInto(): expected the ids view to have a size of" << _glyphCount << "but got" << ids.size(), );
    /* Call into the implementation only if there's actually anything shaped,
       otherwise it might not yet have everything properly set up */
    if(_glyphCount)
        doGlyphIdsInto(ids);
}

void AbstractShaper::glyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const {
    CORRADE_ASSERT(offsets.size() == _glyphCount && advances.size() == _glyphCount,
        "Text::AbstractShaper::glyphOffsetsAdvancesInto(): expected the offsets and advanced views to have a size of" << _glyphCount << "but got" << offsets.size() << "and" << advances.size(), );
    /* Call into the implementation only if there's actually anything shaped,
       otherwise it might not yet have everything properly set up */
    if(_glyphCount)
        doGlyphOffsetsAdvancesInto(offsets, advances);
}

void AbstractShaper::glyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const {
    CORRADE_ASSERT(clusters.size() == _glyphCount,
        "Text::AbstractShaper::glyphClustersInto(): expected the clusters view to have a size of" << _glyphCount << "but got" << clusters.size(), );
    /* Call into the implementation only if there's actually anything shaped,
       otherwise it might not yet have everything properly set up */
    if(_glyphCount)
        doGlyphClustersInto(clusters);
}

}}
