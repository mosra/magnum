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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>

#include "Magnum/Math/Vector2.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/Direction.h"
#include "Magnum/Text/Feature.h"
#include "Magnum/Text/Script.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractShaperTest: TestSuite::Tester {
    explicit AbstractShaperTest();

    void construct();
    void constructCopy();
    void constructMove();

    void setScript();
    void setScriptNotImplemented();

    void setLanguage();
    void setLanguageNotImplemented();

    void setDirection();
    void setDirectionNotImplemented();

    void shape();
    void shapeNoFeatures();
    void shapeNoBeginEnd();
    void shapeNoBeginEndFeatures();
    void shapeScriptLanguageDirectionNotImplemented();
    void shapeZeroGlyphs();
    void shapeBeginEndOutOfRange();

    /* glyphsInto() tested in shape() already */
    void glyphsIntoEmpty();
    void glyphsIntoInvalidViewSizes();
};

AbstractShaperTest::AbstractShaperTest() {
    addTests({&AbstractShaperTest::construct,
              &AbstractShaperTest::constructCopy,
              &AbstractShaperTest::constructMove,

              &AbstractShaperTest::setScript,
              &AbstractShaperTest::setScriptNotImplemented,

              &AbstractShaperTest::setLanguage,
              &AbstractShaperTest::setLanguageNotImplemented,

              &AbstractShaperTest::setDirection,
              &AbstractShaperTest::setDirectionNotImplemented,

              &AbstractShaperTest::shape,
              &AbstractShaperTest::shapeNoFeatures,
              &AbstractShaperTest::shapeNoBeginEnd,
              &AbstractShaperTest::shapeNoBeginEndFeatures,
              &AbstractShaperTest::shapeScriptLanguageDirectionNotImplemented,
              &AbstractShaperTest::shapeZeroGlyphs,
              &AbstractShaperTest::shapeBeginEndOutOfRange,

              &AbstractShaperTest::glyphsIntoEmpty,
              &AbstractShaperTest::glyphsIntoInvalidViewSizes});
}

AbstractFont& FakeFont = *reinterpret_cast<AbstractFont*>(std::size_t{0xdeadbeef});

struct DummyShaper: AbstractShaper {
    using AbstractShaper::AbstractShaper;

    UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override { return {}; }
    void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
    void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
};

void AbstractShaperTest::construct() {
    DummyShaper shaper{FakeFont};
    CORRADE_COMPARE(&shaper.font(), &FakeFont);
    CORRADE_COMPARE(shaper.glyphCount(), 0);

    /* Initial state of script() etc getters verified in the shape() test */

    /* Const overloads */
    const DummyShaper& cshaper = shaper;
    CORRADE_COMPARE(&cshaper.font(), &FakeFont);
}

void AbstractShaperTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DummyShaper>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DummyShaper>{});
}

void AbstractShaperTest::constructMove() {
    DummyShaper a{FakeFont};

    DummyShaper b = Utility::move(a);
    CORRADE_COMPARE(&b.font(), &FakeFont);
    CORRADE_COMPARE(b.glyphCount(), 0);

    DummyShaper c{*reinterpret_cast<AbstractFont*>(std::size_t{0xcafebabe})};
    c = Utility::move(b);
    CORRADE_COMPARE(&b.font(), &FakeFont);
    CORRADE_COMPARE(b.glyphCount(), 0);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DummyShaper>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DummyShaper>::value);
}

void AbstractShaperTest::setScript() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        bool doSetScript(Script script) override {
            CORRADE_COMPARE(script, Script::Math);
            called = true;
            return true;
        }

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}

        bool called = false;
    } shaper{FakeFont};

    CORRADE_VERIFY(shaper.setScript(Script::Math));
    CORRADE_VERIFY(shaper.called);
}

void AbstractShaperTest::setScriptNotImplemented() {
    DummyShaper shaper{FakeFont};
    CORRADE_VERIFY(!shaper.setScript(Script::Math));
}

void AbstractShaperTest::setLanguage() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        bool doSetLanguage(Containers::StringView language) override {
            CORRADE_COMPARE(language, "cs");
            called = true;
            return true;
        }

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}

        bool called = false;
    } shaper{FakeFont};

    CORRADE_VERIFY(shaper.setLanguage("cs"));
    CORRADE_VERIFY(shaper.called);
}

void AbstractShaperTest::setLanguageNotImplemented() {
    DummyShaper shaper{FakeFont};
    CORRADE_VERIFY(!shaper.setLanguage("cs"));
}

void AbstractShaperTest::setDirection() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        bool doSetDirection(ShapeDirection direction) override {
            CORRADE_COMPARE(direction, ShapeDirection::BottomToTop);
            called = true;
            return true;
        }

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override { return {}; }        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}

        bool called = false;
    } shaper{FakeFont};

    CORRADE_VERIFY(shaper.setDirection(ShapeDirection::BottomToTop));
    CORRADE_VERIFY(shaper.called);
}

void AbstractShaperTest::setDirectionNotImplemented() {
    DummyShaper shaper{FakeFont};
    CORRADE_VERIFY(!shaper.setDirection(ShapeDirection::BottomToTop));
}

void AbstractShaperTest::shape() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) override {
            CORRADE_COMPARE(text, "some text");
            CORRADE_COMPARE(begin, 3);
            CORRADE_COMPARE(end, 8);
            CORRADE_COMPARE(features.size(), 2);
            CORRADE_COMPARE(features[0].feature(), Feature::ContextualLigatures);
            CORRADE_VERIFY(features[0].isEnabled());
            CORRADE_COMPARE(features[0].begin(), 0);
            CORRADE_COMPARE(features[0].end(), ~UnsignedInt{});
            CORRADE_COMPARE(features[1].feature(), Feature::Kerning);
            CORRADE_VERIFY(!features[1].isEnabled());
            CORRADE_COMPARE(features[1].begin(), 2);
            CORRADE_COMPARE(features[1].end(), 5);
            shapeCalled = true;
            return 24;
        }

        Script doScript() const override {
            return Script::LinearA;
        }

        Containers::StringView doLanguage() const override {
            return "eh-UH";
        }

        ShapeDirection doDirection() const override {
            return ShapeDirection::BottomToTop;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            CORRADE_COMPARE(ids.size(), 24);
            CORRADE_COMPARE(ids[0], 1337);
            ids[1] = 666;
        }

        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            CORRADE_COMPARE(offsets.size(), 24);
            CORRADE_COMPARE(offsets[0], (Vector2{13.0f, 37.0f}));
            CORRADE_COMPARE(advances.size(), 24);
            CORRADE_COMPARE(advances[0], (Vector2{42.0f, 69.0f}));
            offsets[1] = {-4.0f, -5.0f};
            advances[1] = {12.0f, 23.0f};
        }

        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const override {
            CORRADE_COMPARE(clusters.size(), 24);
            CORRADE_COMPARE(clusters[0], 667);
            clusters[1] = 1336;
        }

        bool shapeCalled = false;
    } shaper{FakeFont};

    /* There's no special behavior, it calls into the implementations even if
       nothing has been shaped yet */
    CORRADE_COMPARE(shaper.glyphCount(), 0);
    CORRADE_COMPARE(shaper.script(), Script::LinearA);
    CORRADE_COMPARE(shaper.language(), "eh-UH");
    CORRADE_COMPARE(shaper.direction(), ShapeDirection::BottomToTop);

    /* Shaping fills glyph count. A real implementation would then return
       (different) detected script/language/direction values, for example. */
    CORRADE_COMPARE(shaper.shape("some text", 3, 8, {
        Feature::ContextualLigatures,
        {Feature::Kerning, 2, 5, false}
    }), 24);
    CORRADE_VERIFY(shaper.shapeCalled);
    CORRADE_COMPARE(shaper.glyphCount(), 24);
    CORRADE_COMPARE(shaper.script(), Script::LinearA);
    CORRADE_COMPARE(shaper.language(), "eh-UH");
    CORRADE_COMPARE(shaper.direction(), ShapeDirection::BottomToTop);

    UnsignedInt ids[24];
    Vector2 offsets[24];
    Vector2 advances[24];
    UnsignedInt clusters[24];
    ids[0] = 1337;
    offsets[0] = {13.0f, 37.0f};
    advances[0] = {42.0f, 69.0f};
    clusters[0] = 667;
    shaper.glyphIdsInto(ids);
    shaper.glyphOffsetsAdvancesInto(offsets, advances);
    shaper.glyphClustersInto(clusters);
    CORRADE_COMPARE(ids[1], 666);
    CORRADE_COMPARE(offsets[1], (Vector2{-4.0f, -5.0f}));
    CORRADE_COMPARE(advances[1], (Vector2{12.0f, 23.0f}));
    CORRADE_COMPARE(clusters[1], 1336);
}

void AbstractShaperTest::shapeNoFeatures() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) override {
            CORRADE_COMPARE(text, "some text");
            CORRADE_COMPARE(begin, 3);
            CORRADE_COMPARE(end, 8);
            CORRADE_COMPARE(features.size(), 0);
            shapeCalled = true;
            return 24;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}

        bool shapeCalled = false;
    } shaper{FakeFont};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    CORRADE_COMPARE(shaper.shape("some text", 3, 8), 24);
    CORRADE_VERIFY(shaper.shapeCalled);
    CORRADE_COMPARE(shaper.glyphCount(), 24);
}

void AbstractShaperTest::shapeNoBeginEnd() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) override {
            CORRADE_COMPARE(text, "some text");
            CORRADE_COMPARE(begin, 0);
            CORRADE_COMPARE(end, ~UnsignedInt{});
            CORRADE_COMPARE(features.size(), 2);
            CORRADE_COMPARE(features[0].feature(), Feature::ContextualLigatures);
            CORRADE_VERIFY(features[0].isEnabled());
            CORRADE_COMPARE(features[0].begin(), 0);
            CORRADE_COMPARE(features[0].end(), ~UnsignedInt{});
            CORRADE_COMPARE(features[1].feature(), Feature::Kerning);
            CORRADE_VERIFY(!features[1].isEnabled());
            CORRADE_COMPARE(features[1].begin(), 2);
            CORRADE_COMPARE(features[1].end(), 5);
            shapeCalled = true;
            return 24;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}

        bool shapeCalled = false;
    } shaper{FakeFont};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* Shaping fills glyph count and allows calling into the implementations */
    CORRADE_COMPARE(shaper.shape("some text", {
        Feature::ContextualLigatures,
        {Feature::Kerning, 2, 5, false}
    }), 24);
    CORRADE_VERIFY(shaper.shapeCalled);
    CORRADE_COMPARE(shaper.glyphCount(), 24);
}

void AbstractShaperTest::shapeNoBeginEndFeatures() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) override {
            CORRADE_COMPARE(text, "some text");
            CORRADE_COMPARE(begin, 0);
            CORRADE_COMPARE(end, ~UnsignedInt{});
            CORRADE_COMPARE(features.size(), 0);
            shapeCalled = true;
            return 24;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}

        bool shapeCalled = false;
    } shaper{FakeFont};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    CORRADE_COMPARE(shaper.shape("some text"), 24);
    CORRADE_VERIFY(shaper.shapeCalled);
    CORRADE_COMPARE(shaper.glyphCount(), 24);
}

void AbstractShaperTest::shapeScriptLanguageDirectionNotImplemented() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 24;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{FakeFont};

    /* Initially it won't call into any of the implementations */
    CORRADE_COMPARE(shaper.script(), Script::Unspecified);
    CORRADE_COMPARE(shaper.language(), "");
    CORRADE_COMPARE(shaper.direction(), ShapeDirection::Unspecified);

    CORRADE_COMPARE(shaper.shape("some text"), 24);

    /* It should delegate to the default implementations, which return the same
       values as if shape() wouldn't be called at all */
    CORRADE_COMPARE(shaper.script(), Script::Unspecified);
    CORRADE_COMPARE(shaper.language(), "");
    CORRADE_COMPARE(shaper.direction(), ShapeDirection::Unspecified);
}

void AbstractShaperTest::shapeZeroGlyphs() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 0;
        }

        Script doScript() const override {
            return Script::LinearA;
        }

        Containers::StringView doLanguage() const override {
            return "eh-UH";
        }

        ShapeDirection doDirection() const override {
            return ShapeDirection::BottomToTop;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{FakeFont};

    CORRADE_COMPARE(shaper.shape("some text", 3, 8), 0);

    /* It calls into the implementations even in case no glyphs were actually
       shaped. It could be for example a zero-length slice of a larger string
       for which script/language/direction detection was performed, so it's
       still useful to get the values after */
    CORRADE_COMPARE(shaper.glyphCount(), 0);
    CORRADE_COMPARE(shaper.script(), Script::LinearA);
    CORRADE_COMPARE(shaper.language(), "eh-UH");
    CORRADE_COMPARE(shaper.direction(), ShapeDirection::BottomToTop);
}

void AbstractShaperTest::shapeBeginEndOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            CORRADE_FAIL("This shouldn't be called");
            return 5;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{FakeFont};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    Containers::String out;
    Error redirectError{&out};
    /* Begin out of range, end unbounded */
    shaper.shape("hello", 6, ~UnsignedInt{});
    shaper.shape("hello", {
        Feature::AccessAllAlternates,
        {Feature::Kerning, 6, ~UnsignedInt{}},
    });
    /* Begin and end out of range */
    shaper.shape("hello", 6, 7);
    shaper.shape("hello", {
        Feature::AccessAllAlternates,
        {Feature::Kerning, 6, 7},
    });
    /* End out of range */
    shaper.shape("hello", 4, 6);
    shaper.shape("hello", {
        Feature::AccessAllAlternates,
        {Feature::Kerning, 4, 6},
    });
    /* Begin larger than end */
    shaper.shape("hello", 4, 3);
    shaper.shape("hello", {
        Feature::AccessAllAlternates,
        {Feature::Kerning, 4, 3},
    });
    CORRADE_COMPARE_AS(out,
        "Text::AbstractShaper::shape(): begin 6 and end 4294967295 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): feature 1 begin 6 and end 4294967295 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): begin 6 and end 7 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): feature 1 begin 6 and end 7 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): begin 4 and end 6 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): feature 1 begin 4 and end 6 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): begin 4 and end 3 out of range for a text of 5 bytes\n"
        "Text::AbstractShaper::shape(): feature 1 begin 4 and end 3 out of range for a text of 5 bytes\n",
        TestSuite::Compare::String);
}

void AbstractShaperTest::glyphsIntoEmpty() {
    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 0;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            CORRADE_FAIL("This shouldn't be called");
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {
            CORRADE_FAIL("This shouldn't be called");
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            CORRADE_FAIL("This shouldn't be called");
        }
    } shaper{FakeFont};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* This should not assert but also not call anywhere */
    shaper.glyphIdsInto(nullptr);
    shaper.glyphOffsetsAdvancesInto(nullptr, nullptr);
    shaper.glyphClustersInto(nullptr);
}

void AbstractShaperTest::glyphsIntoInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 5;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            CORRADE_FAIL("This shouldn't be called");
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {
            CORRADE_FAIL("This shouldn't be called");
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            CORRADE_FAIL("This shouldn't be called");
        }
    } shaper{FakeFont};

    CORRADE_COMPARE(shaper.shape("yey"), 5);

    UnsignedInt idsWrong[6];
    Vector2 offsets[5];
    Vector2 offsetsWrong[6];
    Vector2 advances[5];
    Vector2 advancesWrong[6];
    UnsignedInt clustersWrong[6];

    Containers::String out;
    Error redirectError{&out};
    shaper.glyphIdsInto(idsWrong);
    shaper.glyphOffsetsAdvancesInto(offsetsWrong, advances);
    shaper.glyphOffsetsAdvancesInto(offsets, advancesWrong);
    shaper.glyphClustersInto(clustersWrong);
    CORRADE_COMPARE(out,
        "Text::AbstractShaper::glyphIdsInto(): expected the ids view to have a size of 5 but got 6\n"
        "Text::AbstractShaper::glyphOffsetsAdvancesInto(): expected the offsets and advanced views to have a size of 5 but got 6 and 5\n"
        "Text::AbstractShaper::glyphOffsetsAdvancesInto(): expected the offsets and advanced views to have a size of 5 but got 5 and 6\n"
        "Text::AbstractShaper::glyphClustersInto(): expected the clusters view to have a size of 5 but got 6\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractShaperTest)
