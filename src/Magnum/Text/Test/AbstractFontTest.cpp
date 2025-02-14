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

#include <string> /** @todo remove once file callbacks are std::string-free */
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/FileCallback.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"

#include "configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Algorithms.h>
#endif

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractFontTest: TestSuite::Tester {
    explicit AbstractFontTest();

    void construct();

    void openData();
    void openFileAsData();
    void openFileAsDataNotFound();

    void openFileNotImplemented();
    void openDataNotSupported();
    void openDataNotImplemented();

    void setFileCallback();
    void setFileCallbackTemplate();
    void setFileCallbackTemplateNull();
    void setFileCallbackTemplateConst();
    void setFileCallbackFileOpened();
    void setFileCallbackNotImplemented();
    void setFileCallbackNotSupported();
    void setFileCallbackOpenFileDirectly();
    void setFileCallbackOpenFileThroughBaseImplementation();
    void setFileCallbackOpenFileThroughBaseImplementationFailed();
    void setFileCallbackOpenFileAsData();
    void setFileCallbackOpenFileAsDataFailed();

    void properties();
    void propertiesNoFont();

    void glyphId();
    void glyphIdNoFont();
    void glyphIdInvalidSize();
    void glyphIdOutOfRange();

    void glyphName();
    void glyphNameNotImplemented();
    void glyphNameNoFont();
    void glyphNameOutOfRange();

    void glyphSizeAdvance();
    void glyphSizeAdvanceNoFont();
    void glyphSizeAdvanceOutOfRange();

    void fillGlyphCache();
    void fillGlyphCacheOutOfRange();
    void fillGlyphCacheNotUnique();
    void fillGlyphCacheFromString();
    void fillGlyphCacheFailed();
    void fillGlyphCacheNotSupported();
    void fillGlyphCacheNotImplemented();
    void fillGlyphCacheNoFont();
    void fillGlyphCacheInvalidUtf8();

    void createGlyphCache();
    void createGlyphCacheNotSupported();
    void createGlyphCacheNotImplemented();
    void createGlyphCacheNoFont();

    void createShaper();
    void createShaperNoFont();
    void createShaperNullptr();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void layout();
    void layoutArrayGlyphCache();
    void layoutGlyphCacheFontNotFound();
    void layoutGlyphOutOfRange();
    void layoutNoFont();
    #endif

    void debugFeature();
    void debugFeaturePacked();
    void debugFeatures();
    void debugFeaturesPacked();
};

AbstractFontTest::AbstractFontTest() {
    addTests({&AbstractFontTest::construct,

              &AbstractFontTest::openData,
              &AbstractFontTest::openFileAsData,
              &AbstractFontTest::openFileAsDataNotFound,

              &AbstractFontTest::openFileNotImplemented,
              &AbstractFontTest::openDataNotSupported,
              &AbstractFontTest::openDataNotImplemented,

              &AbstractFontTest::setFileCallback,
              &AbstractFontTest::setFileCallbackTemplate,
              &AbstractFontTest::setFileCallbackTemplateNull,
              &AbstractFontTest::setFileCallbackTemplateConst,
              &AbstractFontTest::setFileCallbackFileOpened,
              &AbstractFontTest::setFileCallbackNotImplemented,
              &AbstractFontTest::setFileCallbackNotSupported,
              &AbstractFontTest::setFileCallbackOpenFileDirectly,
              &AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementation,
              &AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationFailed,
              &AbstractFontTest::setFileCallbackOpenFileAsData,
              &AbstractFontTest::setFileCallbackOpenFileAsDataFailed,

              &AbstractFontTest::properties,
              &AbstractFontTest::propertiesNoFont,

              &AbstractFontTest::glyphId,
              &AbstractFontTest::glyphIdNoFont,
              &AbstractFontTest::glyphIdInvalidSize,
              &AbstractFontTest::glyphIdOutOfRange,

              &AbstractFontTest::glyphName,
              &AbstractFontTest::glyphNameNotImplemented,
              &AbstractFontTest::glyphNameNoFont,
              &AbstractFontTest::glyphNameOutOfRange,

              &AbstractFontTest::glyphSizeAdvance,
              &AbstractFontTest::glyphSizeAdvanceNoFont,
              &AbstractFontTest::glyphSizeAdvanceOutOfRange,

              &AbstractFontTest::fillGlyphCache,
              &AbstractFontTest::fillGlyphCacheOutOfRange,
              &AbstractFontTest::fillGlyphCacheNotUnique,
              &AbstractFontTest::fillGlyphCacheFromString,
              &AbstractFontTest::fillGlyphCacheFailed,
              &AbstractFontTest::fillGlyphCacheNotSupported,
              &AbstractFontTest::fillGlyphCacheNotImplemented,
              &AbstractFontTest::fillGlyphCacheNoFont,
              &AbstractFontTest::fillGlyphCacheInvalidUtf8,

              &AbstractFontTest::createGlyphCache,
              &AbstractFontTest::createGlyphCacheNotSupported,
              &AbstractFontTest::createGlyphCacheNotImplemented,
              &AbstractFontTest::createGlyphCacheNoFont,

              &AbstractFontTest::createShaper,
              &AbstractFontTest::createShaperNoFont,
              &AbstractFontTest::createShaperNullptr,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractFontTest::layout,
              &AbstractFontTest::layoutArrayGlyphCache,
              &AbstractFontTest::layoutGlyphCacheFontNotFound,
              &AbstractFontTest::layoutGlyphOutOfRange,
              &AbstractFontTest::layoutNoFont,
              #endif

              &AbstractFontTest::debugFeature,
              &AbstractFontTest::debugFeaturePacked,
              &AbstractFontTest::debugFeatures,
              &AbstractFontTest::debugFeaturesPacked});
}

using namespace Containers::Literals;

void AbstractFontTest::construct() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    CORRADE_COMPARE(font.features(), FontFeatures{});
    CORRADE_VERIFY(!font.isOpened());

    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

void AbstractFontTest::openData() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(const Containers::ArrayView<const char> data, Float size) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;

    CORRADE_VERIFY(!font.isOpened());
    const char a5 = '\xa5';
    font.openData({&a5, 1}, 13.0f);
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::openFileAsData() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(const Containers::ArrayView<const char> data, Float size) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;

    /* doOpenFile() should call doOpenData() */
    CORRADE_VERIFY(!font.isOpened());
    font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 13.0f);
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::openFileAsDataNotFound() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("nonexistent.foo", 12.0f));
    /* There's an error message from Path::read() before */
    CORRADE_COMPARE_AS(out,
        "\nText::AbstractFont::openFile(): cannot open file nonexistent.foo\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractFontTest::openFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.openFile("file.foo", 34.0f);
    CORRADE_COMPARE(out, "Text::AbstractFont::openFile(): not implemented\n");
}

void AbstractFontTest::openDataNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.openData(nullptr, 34.0f);
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): feature not supported\n");
}

void AbstractFontTest::openDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.openData(nullptr, 34.0f);
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): feature advertised but not implemented\n");
}

void AbstractFontTest::setFileCallback() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData|FontFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            *static_cast<int*>(userData) = 1337;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    font.setFileCallback(lambda, &a);
    CORRADE_COMPARE(font.fileCallback(), lambda);
    CORRADE_COMPARE(font.fileCallbackUserData(), &a);
    CORRADE_COMPARE(a, 1337);
}

void AbstractFontTest::setFileCallbackTemplate() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData|FontFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    font.setFileCallback(lambda, a);
    CORRADE_VERIFY(font.fileCallback());
    CORRADE_VERIFY(font.fileCallbackUserData());
    CORRADE_VERIFY(font.called);

    /* The data pointers should be wrapped, thus not the same */
    CORRADE_VERIFY(reinterpret_cast<void(*)()>(font.fileCallback()) != reinterpret_cast<void(*)()>(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(lambda)));
    CORRADE_VERIFY(font.fileCallbackUserData() != &a);
}

void AbstractFontTest::setFileCallbackTemplateNull() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData|FontFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            called = !callback && !userData;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    int a = 0;
    font.setFileCallback(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(nullptr), a);
    CORRADE_VERIFY(!font.fileCallback());
    CORRADE_VERIFY(!font.fileCallbackUserData());
    CORRADE_VERIFY(font.called);
}

void AbstractFontTest::setFileCallbackTemplateConst() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData|FontFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    const int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, const int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    font.setFileCallback(lambda, a);
    CORRADE_VERIFY(font.fileCallback());
    CORRADE_VERIFY(font.fileCallbackUserData());
    CORRADE_VERIFY(font.called);
}

void AbstractFontTest::setFileCallbackFileOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};

    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });
    CORRADE_COMPARE(out, "Text::AbstractFont::setFileCallback(): can't be set while a font is opened\n");
}

void AbstractFontTest::setFileCallbackNotImplemented() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    int a;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    font.setFileCallback(lambda, &a);
    CORRADE_COMPARE(font.fileCallback(), lambda);
    CORRADE_COMPARE(font.fileCallbackUserData(), &a);
    /* Should just work, no need to implement the function */
}

void AbstractFontTest::setFileCallbackNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};

    int a;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);
    CORRADE_COMPARE(out, "Text::AbstractFont::setFileCallback(): font plugin supports neither loading from data nor via callbacks, callbacks can't be used\n");
}

void AbstractFontTest::setFileCallbackOpenFileDirectly() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            /* Called because FileCallback is supported */
            _opened = filename == "file.dat" && fileCallback() && fileCallbackUserData();
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            /* Shouldn't be called because FileCallback is supported */
            openDataCalledNotSureWhy = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
        bool openDataCalledNotSureWhy = false;
    } font;

    bool calledNotSureWhy = false;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, bool& calledNotSureWhy) -> Containers::Optional<Containers::ArrayView<const char>> {
        calledNotSureWhy = true;
        return {};
    }, calledNotSureWhy);

    CORRADE_VERIFY(font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(!calledNotSureWhy);
    CORRADE_VERIFY(!font.openDataCalledNotSureWhy);
    CORRADE_COMPARE(font.size(), 42.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementation() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            openFileCalled = filename == "file.dat" && fileCallback() && fileCallbackUserData();
            return AbstractFont::doOpenFile(filename, size);
        }

        Properties doOpenData(Containers::ArrayView<const char> data, Float size) override {
            _opened = (data.size() == 1 && data[0] == '\xb0');
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
        bool openFileCalled = false;
    } font;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
        bool calledNotSureWhy = false;
    } state;
    font.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        state.calledNotSureWhy = true;
        return {};
    }, state);

    CORRADE_VERIFY(font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(font.openFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_VERIFY(!state.calledNotSureWhy);
    CORRADE_COMPARE(font.size(), 42.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationFailed() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            openFileCalled = true;
            return AbstractFont::doOpenFile(filename, size);
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

         bool openFileCalled = false;
    } font;

    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        return {};
    });

    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(font.openFileCalled);
    CORRADE_COMPARE(out, "Text::AbstractFont::openFile(): cannot open file file.dat\n");
}

void AbstractFontTest::setFileCallbackOpenFileAsData() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float) override {
            openFileCalled = true;
            return {};
        }

        Properties doOpenData(Containers::ArrayView<const char> data, Float size) override {
            _opened = (data.size() == 1 && data[0] == '\xb0');
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
        bool openFileCalled = false;
    } font;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
        bool calledNotSureWhy = false;
    } state;

    font.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        state.calledNotSureWhy = true;
        return {};
    }, state);

    CORRADE_VERIFY(font.openFile("file.dat", 13.0f));
    CORRADE_VERIFY(!font.openFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_VERIFY(!state.calledNotSureWhy);
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::setFileCallbackOpenFileAsDataFailed() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView, Float) override {
            openFileCalled = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool openFileCalled = false;
    } font;

    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!font.openFile("file.dat", 132.0f));
    CORRADE_VERIFY(!font.openFileCalled);
    CORRADE_COMPARE(out, "Text::AbstractFont::openFile(): cannot open file file.dat\n");
}

void AbstractFontTest::properties() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(const Containers::ArrayView<const char>, Float size) override {
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;

    CORRADE_VERIFY(font.openData(nullptr, 13.0f));
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::propertiesNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.size();
    font.ascent();
    font.descent();
    font.lineHeight();
    font.glyphCount();
    CORRADE_COMPARE(out,
        "Text::AbstractFont::size(): no font opened\n"
        "Text::AbstractFont::ascent(): no font opened\n"
        "Text::AbstractFont::descent(): no font opened\n"
        "Text::AbstractFont::lineHeight(): no font opened\n"
        "Text::AbstractFont::glyphCount(): no font opened\n");
}

void AbstractFontTest::glyphId() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 1280};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(std::size_t i = 0; i != characters.size(); ++i)
                glyphs[i] = characters[i]*10;
        }

        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    const char32_t characters[]{U'a', U'W', U'!'};
    UnsignedInt glyphs[3];
    font.glyphIdsInto(characters, glyphs);
    CORRADE_COMPARE_AS(Containers::arrayView(glyphs), Containers::arrayView({
        970u, 870u, 330u
    }), TestSuite::Compare::Container);

    /* Single-item convenience overload */
    CORRADE_COMPARE(font.glyphId(U'a'), 970);
}

void AbstractFontTest::glyphIdNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.glyphIdsInto({}, {});
    font.glyphId('a');
    CORRADE_COMPARE(out,
        "Text::AbstractFont::glyphIdsInto(): no font opened\n"
        /* Both delegate to the same function so the assert is the same */
        "Text::AbstractFont::glyphIdsInto(): no font opened\n");
}

void AbstractFontTest::glyphIdInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    char32_t characters[3];
    UnsignedInt glyphs[4];

    Containers::String out;
    Error redirectError{&out};
    font.glyphIdsInto(characters, glyphs);
    CORRADE_COMPARE(out, "Text::AbstractFont::glyphIdsInto(): expected the characters and glyphs views to have the same size but got 3 and 4\n");
}

void AbstractFontTest::glyphIdOutOfRange() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 4};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(std::size_t i = 0; i != glyphs.size(); ++i)
                glyphs[i] = i;
        }

        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    const char32_t characters[6]{U'\x1234', U'\x5678', U'\xabcd',
                                 U'\xef01', U'\x2345', U'\x6789'};
    UnsignedInt glyphs[6];

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    Containers::String out;
    Error redirectError{&out};
    font.glyphIdsInto(characters, glyphs);
    CORRADE_COMPARE(out, "Text::AbstractFont::glyphIdsInto(): implementation-returned index 4 for character U+2345 out of range for 4 glyphs\n");
}

void AbstractFontTest::glyphName() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 4};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Containers::String doGlyphName(UnsignedInt glyph) override {
            return glyph == 3 ? "WHATEVER" : "";
        }
        UnsignedInt doGlyphForName(Containers::StringView name) override {
            return name == "whatever" ? 3 : 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    CORRADE_COMPARE(font.glyphName(3), "WHATEVER");
    CORRADE_COMPARE(font.glyphForName("whatever"), 3);
}

void AbstractFontTest::glyphNameNotImplemented() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 4};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    CORRADE_COMPARE(font.glyphName(3), "");
    CORRADE_COMPARE(font.glyphForName("whatever"), 0);
}

void AbstractFontTest::glyphNameNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.glyphName(0);
    font.glyphForName("");
    CORRADE_COMPARE(out,
        "Text::AbstractFont::glyphName(): no font opened\n"
        "Text::AbstractFont::glyphForName(): no font opened\n");
}

void AbstractFontTest::glyphNameOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 4};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        UnsignedInt doGlyphForName(Containers::StringView) override { return 4; }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    Containers::String out;
    Error redirectError{&out};
    font.glyphName(4);
    font.glyphForName("");
    CORRADE_COMPARE(out,
        "Text::AbstractFont::glyphName(): index 4 out of range for 4 glyphs\n"
        "Text::AbstractFont::glyphForName(): implementation-returned index 4 out of range for 4 glyphs\n");
}

void AbstractFontTest::glyphSizeAdvance() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 98};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt a) override { return {a*2.0f, a/3.0f}; }
        Vector2 doGlyphAdvance(UnsignedInt a) override { return {a*10.0f, -Float(a)/10.0f}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));
    CORRADE_COMPARE(font.glyphSize(33), (Vector2{66.0f, 11.0f}));
    CORRADE_COMPARE(font.glyphAdvance(97), (Vector2{970.0f, -9.7f}));
}

void AbstractFontTest::glyphSizeAdvanceNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.glyphSize(33);
    font.glyphAdvance(97);
    CORRADE_COMPARE(out,
        "Text::AbstractFont::glyphSize(): no font opened\n"
        "Text::AbstractFont::glyphAdvance(): no font opened\n");
}

void AbstractFontTest::glyphSizeAdvanceOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 3};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    Containers::String out;
    Error redirectError{&out};
    font.glyphSize(3);
    font.glyphAdvance(3);
    CORRADE_COMPARE(out,
        "Text::AbstractFont::glyphSize(): index 3 out of range for 3 glyphs\n"
        "Text::AbstractFont::glyphAdvance(): index 3 out of range for 3 glyphs\n");
}

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

void AbstractFontTest::fillGlyphCache() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 17};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {
            CORRADE_FAIL("This should not be called.");
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool doFillGlyphCache(AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const UnsignedInt>& glyphs) override {
            CORRADE_COMPARE(cache.size(), (Vector3i{100, 100, 1}));
            /* The glyph list isn't sorted in this case, nothing is implicitly
               added to it either */
            CORRADE_COMPARE_AS(glyphs, Containers::arrayView({
                16u, 5u, 11u, 2u
            }), TestSuite::Compare::Container);
            ++called;
            return true;
        }

        Int called = 0;

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};

    CORRADE_VERIFY(font.fillGlyphCache(cache, Containers::arrayView({16u, 5u, 11u, 2u})));
    CORRADE_COMPARE(font.called, 1);

    /* Also the initializer list overload */
    CORRADE_VERIFY(font.fillGlyphCache(cache, {16, 5, 11, 2}));
    CORRADE_COMPARE(font.called, 2);
}

void AbstractFontTest::fillGlyphCacheOutOfRange() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 16};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};
    font.fillGlyphCache(cache, Containers::arrayView({0u, 15u, 3u, 16u, 80u}));
    font.fillGlyphCache(cache, {0, 15, 3, 16, 80});
    CORRADE_COMPARE(out,
        "Text::AbstractFont::fillGlyphCache(): index 16 out of range for 16 glyphs\n"
        "Text::AbstractFont::fillGlyphCache(): index 16 out of range for 16 glyphs\n");
}

void AbstractFontTest::fillGlyphCacheNotUnique() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 16};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};
    font.fillGlyphCache(cache, Containers::arrayView({0u, 15u, 3u, 15u, 80u}));
    CORRADE_COMPARE(out,
        "Text::AbstractFont::fillGlyphCache(): duplicate glyph 15\n");
}

void AbstractFontTest::fillGlyphCacheFromString() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 17};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            CORRADE_COMPARE_AS(characters, Containers::arrayView<char32_t>({
                'h', 'e', 'l', 'l', 'o'
            }), TestSuite::Compare::Container);
            glyphs[0] = 16;
            glyphs[1] = 2;
            glyphs[2] = 11;
            glyphs[3] = 11;
            glyphs[4] = 5;
            ++glyphIdsIntoCalled;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool doFillGlyphCache(AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const UnsignedInt>& glyphs) override {
            CORRADE_COMPARE(cache.size(), (Vector3i{100, 100, 1}));
            /* The array should be sorted by ID, without duplicates and with
               the first ID being 0 if the cache doesn't have this font yet */
            if(!cache.fontCount())
                CORRADE_COMPARE_AS(glyphs, Containers::arrayView({
                    0u, 2u, 5u, 11u, 16u
                }), TestSuite::Compare::Container);
            else
                CORRADE_COMPARE_AS(glyphs, Containers::arrayView({
                    2u, 5u, 11u, 16u
                }), TestSuite::Compare::Container);
            ++fillGlyphCacheCalled;
            return true;
        }

        Int glyphIdsIntoCalled = 0,
            fillGlyphCacheCalled = 0;

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};

    /* First time it should include the zero glyph as well */
    CORRADE_VERIFY(font.fillGlyphCache(cache, "hello"));
    CORRADE_COMPARE(font.glyphIdsIntoCalled, 1);
    CORRADE_COMPARE(font.fillGlyphCacheCalled, 1);

    /* Second time not anymore */
    cache.addFont(10, &font);
    CORRADE_VERIFY(font.fillGlyphCache(cache, "hello"));
    CORRADE_COMPARE(font.glyphIdsIntoCalled, 2);
    CORRADE_COMPARE(font.fillGlyphCacheCalled, 2);
}

void AbstractFontTest::fillGlyphCacheFailed() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 1};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            /* Set all to 0 to avoid an assert that the IDs are out of range */
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool doFillGlyphCache(AbstractGlyphCache&, const Containers::StridedArrayView1D<const UnsignedInt>&) override {
            return false;
        }

        bool called = false;

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};

    CORRADE_VERIFY(!font.fillGlyphCache(cache, Containers::ArrayView<const UnsignedInt>{}));
    CORRADE_VERIFY(!font.fillGlyphCache(cache, {}));
    CORRADE_VERIFY(!font.fillGlyphCache(cache, ""));
}

void AbstractFontTest::fillGlyphCacheNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            /* Set all to 0 to avoid an assert that the IDs are out of range */
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};
    font.fillGlyphCache(cache, Containers::arrayView({0u, 15u}));
    font.fillGlyphCache(cache, {0u, 15u});
    font.fillGlyphCache(cache, "hello");
    CORRADE_COMPARE(out,
        "Text::AbstractFont::fillGlyphCache(): feature not supported\n"
        "Text::AbstractFont::fillGlyphCache(): feature not supported\n"
        "Text::AbstractFont::fillGlyphCache(): feature not supported\n");
}

void AbstractFontTest::fillGlyphCacheNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            _opened = true;
            return {0.0f, 0.0f, 0.0f, 0.0f, 1};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Have to explicitly open in order to make glyphCount() non-zero */
    CORRADE_VERIFY(font.openData(nullptr, 0.0f));

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};
    font.fillGlyphCache(cache, Containers::arrayView({0u}));
    font.fillGlyphCache(cache, {0});
    font.fillGlyphCache(cache, "hello");
    CORRADE_COMPARE(out,
        "Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented\n"
        "Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented\n"
        "Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented\n");
}

void AbstractFontTest::fillGlyphCacheNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};
    font.fillGlyphCache(cache, Containers::arrayView({0u, 15u}));
    font.fillGlyphCache(cache, {0, 15});
    font.fillGlyphCache(cache, "hello");
    CORRADE_COMPARE(out,
        "Text::AbstractFont::fillGlyphCache(): no font opened\n"
        "Text::AbstractFont::fillGlyphCache(): no font opened\n"
        "Text::AbstractFont::fillGlyphCache(): no font opened\n");
}

void AbstractFontTest::fillGlyphCacheInvalidUtf8() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};
    font.fillGlyphCache(cache, "he\xffo");
    CORRADE_COMPARE(out, "Text::AbstractFont::fillGlyphCache(): not a valid UTF-8 string: he\xffo\n");
}

void AbstractFontTest::createGlyphCache() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        Containers::Pointer<AbstractGlyphCache> doCreateGlyphCache() override {
            return Containers::pointer<DummyGlyphCache>(PixelFormat::R8Unorm, Vector2i{123, 345});
        }
    } font;

    Containers::Pointer<AbstractGlyphCache> cache = font.createGlyphCache();
    CORRADE_VERIFY(cache);

    CORRADE_COMPARE(cache->size(), (Vector3i{123, 345, 1}));
}

void AbstractFontTest::createGlyphCacheNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.createGlyphCache();
    CORRADE_COMPARE(out, "Text::AbstractFont::createGlyphCache(): feature not supported\n");
}

void AbstractFontTest::createGlyphCacheNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.createGlyphCache();
    CORRADE_COMPARE(out, "Text::AbstractFont::createGlyphCache(): feature advertised but not implemented\n");
}

void AbstractFontTest::createGlyphCacheNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.createGlyphCache();
    CORRADE_COMPARE(out, "Text::AbstractFont::createGlyphCache(): no font opened\n");
}

void AbstractFontTest::createShaper() {
    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 37;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    };

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return Containers::pointer<Shaper>(*this); }
    } font;

    Containers::Pointer<AbstractShaper> shaper = font.createShaper();
    CORRADE_COMPARE(shaper->shape("eh"), 37);
}

void AbstractFontTest::createShaperNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.createShaper();
    CORRADE_COMPARE(out, "Text::AbstractFont::createShaper(): no font opened\n");
}

void AbstractFontTest::createShaperNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return nullptr; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.createShaper();
    CORRADE_COMPARE(out, "Text::AbstractFont::createShaper(): implementation returned nullptr\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::layout() {
    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 3;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            Utility::copy({3, 7, 3}, ids);
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            Utility::copy({{0.5f, 1.0f},
                           {1.0f, 0.5f},
                           {2.0f, 2.0f}}, offsets);
            Utility::copy({{50.0f, 0.0f},
                           {10.0f, 0.0f},
                           {20.0f, 0.0f}}, advances);
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* Nothing in the old AbstractLayouter uses this */
            CORRADE_FAIL("This shouldn't be called.");
        }
    };

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView, Float) override {
            _opened = true;
            return {0.5f, 0.0f, 0.0f, 0.0f, 666};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override {
            return Containers::pointer<Shaper>(*this);
        }

        bool _opened = false;
    } font;

    /* Have to open the font to fill the font size */
    font.openFile({}, {});
    CORRADE_COMPARE(font.size(), 0.5f);

    /* Default padding is 1 to avoid artifacts, set that to 0 to simplify */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {10, 20}, {}};

    UnsignedInt fontId = cache.addFont(15, &font);

    cache.addGlyph(fontId, 3, {1, 2}, {{3, 4}, {6, 5}});
    cache.addGlyph(fontId, 7, {3, 4}, {{5, 6}, {9, 8}});

    CORRADE_IGNORE_DEPRECATED_PUSH
    Containers::Pointer<AbstractLayouter> layouter = font.layout(cache, 0.25f, "hello");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_VERIFY(layouter);
    CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here too */
    CORRADE_COMPARE(layouter->glyphCount(), 3);
    CORRADE_IGNORE_DEPRECATED_POP

    /* Positions are scaled by 0.25/0.5, texture coordinates by {0.1, 0.05} */
    Vector2 cursor{100.0f, 10.0f};
    Range2D rect{{70.0f, 10.0f}, {70.0f, 10.0f}};

    /* Glyph 3 at initial cursor position, offset by scaled {0.5, 1.0} from
       the shaper and scaled {1, 2} from the glyph cache */
    CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here too */
    CORRADE_COMPARE(layouter->renderGlyph(0, cursor, rect), Containers::pair(
        Range2D::fromSize({100.75f, 11.5f}, {1.5f, 0.5f}),
        Range2D{{0.3f, 0.2f}, {0.6f, 0.25f}}
    ));
    CORRADE_IGNORE_DEPRECATED_POP
    /* Moving the cursor by scaled {50, 0} */
    CORRADE_COMPARE(cursor, (Vector2{125.0f, 10.0f}));
    /* The initial rect is empty, so this replaces it */
    CORRADE_COMPARE(rect, (Range2D{{100.75f, 11.5f}, {102.25f, 12.0f}}));

    /* Glyph 7 at the next cursor position, offset by scaled {1.0, 0.5} from
       the shaper and scaled {3, 4} from the glyph cache */
    CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here too */
    CORRADE_COMPARE(layouter->renderGlyph(1, cursor, rect), Containers::pair(
        Range2D::fromSize({127.0f, 12.25f}, {2.0f, 1.0f}),
        Range2D{{0.5f, 0.3f}, {0.9f, 0.4f}}
    ));
    CORRADE_IGNORE_DEPRECATED_POP
    /* Moving the cursor by scaled {10, 0} */
    CORRADE_COMPARE(cursor, (Vector2{130.0f, 10.0f}));
    /* Union of the two rectangles */
    CORRADE_COMPARE(rect, (Range2D{{100.75f, 11.5f}, {129.0f, 13.25f}}));

    /* Glyph 3 again, offset by scaled {2.0, 2.0} from the shaper and scaled
       {1, 2} from the glyph cache */
    CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here too */
    CORRADE_COMPARE(layouter->renderGlyph(2, cursor, rect), Containers::pair(
        Range2D::fromSize({131.5f, 12.0f}, {1.5f, 0.5f}),
        Range2D{{0.3f, 0.2f}, {0.6f, 0.25f}}
    ));
    CORRADE_IGNORE_DEPRECATED_POP
    /* Moving the cursor by scaled {20, 0} */
    CORRADE_COMPARE(cursor, (Vector2{140.0f, 10.0f}));
    /* Union of the three rectangles */
    CORRADE_COMPARE(rect, (Range2D{{100.75f, 11.5f}, {133.0f, 13.25f}}));
}

void AbstractFontTest::layoutArrayGlyphCache() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    DummyGlyphCache cache{PixelFormat::R8Unorm, {1, 2, 3}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    font.layout(cache, 0.25f, "hello");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractFont::layout(): array glyph caches are not supported\n");
}

void AbstractFontTest::layoutGlyphCacheFontNotFound() {
    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    DummyGlyphCache cache{PixelFormat::R8Unorm, {1, 2}};

    cache.addFont(3);
    cache.addFont(17);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    Containers::Pointer<AbstractLayouter> layouter = font.layout(cache, 0.25f, "hello");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_VERIFY(!layouter);
    CORRADE_COMPARE(out, "Text::AbstractFont::layout(): font not found among 2 fonts in passed glyph cache\n");
}

void AbstractFontTest::layoutGlyphOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 3;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Clear the IDs as otherwise it'd result in OOB calls into the
               glyph cache */
            for(UnsignedInt& i: ids) i = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    };

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override {
            return Containers::pointer<Shaper>(*this);
        }
    } font;

    DummyGlyphCache cache{PixelFormat::R8Unorm, {10, 20}};

    cache.addFont(15, &font);

    CORRADE_IGNORE_DEPRECATED_PUSH
    Containers::Pointer<AbstractLayouter> layouter = font.layout(cache, 0.25f, "hello");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_VERIFY(layouter);
    CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here too */
    CORRADE_COMPARE(layouter->glyphCount(), 3);
    CORRADE_IGNORE_DEPRECATED_POP

    Range2D rectangle;
    Vector2 cursorPosition;

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here too */
    layouter->renderGlyph(3, cursorPosition, rectangle);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractLayouter::renderGlyph(): index 3 out of range for 3 glyphs\n");
}

void AbstractFontTest::layoutNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct MyFont: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 200}};
    CORRADE_IGNORE_DEPRECATED_PUSH
    font.layout(cache, 0.25f, "hello");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractFont::layout(): no font opened\n");
}
#endif

void AbstractFontTest::debugFeature() {
    Containers::String out;

    Debug{&out} << FontFeature::OpenData << FontFeature(0xf0);
    CORRADE_COMPARE(out, "Text::FontFeature::OpenData Text::FontFeature(0xf0)\n");
}

void AbstractFontTest::debugFeaturePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << FontFeature::OpenData << Debug::packed << FontFeature(0xf0) << FontFeature::FileCallback;
    CORRADE_COMPARE(out, "OpenData 0xf0 Text::FontFeature::FileCallback\n");
}

void AbstractFontTest::debugFeatures() {
    Containers::String out;

    Debug{&out} << (FontFeature::OpenData|FontFeature::PreparedGlyphCache) << FontFeatures{};
    CORRADE_COMPARE(out, "Text::FontFeature::OpenData|Text::FontFeature::PreparedGlyphCache Text::FontFeatures{}\n");
}

void AbstractFontTest::debugFeaturesPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (FontFeature::OpenData|FontFeature::PreparedGlyphCache) << Debug::packed << FontFeatures{} << FontFeature::FileCallback;
    CORRADE_COMPARE(out, "OpenData|PreparedGlyphCache {} Text::FontFeature::FileCallback\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontTest)
