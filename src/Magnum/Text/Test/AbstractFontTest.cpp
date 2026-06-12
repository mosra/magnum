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

#include <string> /** @todo remove once file callbacks are std::string-free */
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once file callbacks are std::string-free */
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

    void debugFeature();
    void debugFeaturePacked();
    void debugFeatures();
    void debugFeaturesPacked();

    void construct();

    void dataFontCount();
    void dataFontCountFailed();
    void dataFontCountNotImplemented();
    void fileFontCount();
    void fileFontCountFailed();
    void fileFontCountNotImplemented();
    void fileFontCountAsData();
    void fileFontCountAsDataNotFound();
    void fileFontCountAsDataFailed();
    void fileFontCountAsDataNotImplemented();

    void fontCountInvalid();
    void dataFontCountNotSupported();

    void openData();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openDataDeprecated();
    void openDataDeprecatedNonZeroFontId();
    #endif
    void openDataFailed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openDataFailedDeprecated();
    #endif
    void openFile();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openFileDeprecated();
    void openFileDeprecatedNonZeroFontId();
    #endif
    void openFileFailed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openFileFailedDeprecated();
    #endif
    void openFileAsData();
    void openFileAsDataNotFound();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openFileAsDataDeprecated();
    void openFileAsDataDeprecatedNonZeroFontId();
    #endif
    void openFileAsDataFailed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openFileAsDataFailedDeprecated();
    #endif

    void openFileNotImplemented();
    void openDataNotSupported();
    void openDataNotImplemented();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void propertiesNotImplemented();
    #endif

    void setFileCallback();
    void setFileCallbackTemplate();
    void setFileCallbackTemplateNull();
    void setFileCallbackTemplateConst();
    void setFileCallbackFileOpened();
    void setFileCallbackNotImplemented();
    void setFileCallbackNotSupported();
    void setFileCallbackOpenFileDirectly();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setFileCallbackOpenFileDirectlyDeprecated();
    void setFileCallbackOpenFileDirectlyDeprecatedNonZeroFontId();
    #endif
    void setFileCallbackOpenFileDirectlyFailed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setFileCallbackOpenFileDirectlyFailedDeprecated();
    #endif
    void setFileCallbackOpenFileThroughBaseImplementation();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setFileCallbackOpenFileThroughBaseImplementationDeprecated();
    /* No non-zero font ID variant here, as the deprecated doOpenFile() that'd
       delegate to the base implementation doesn't get called at all */
    #endif
    void setFileCallbackOpenFileThroughBaseImplementationNotFound();
    void setFileCallbackOpenFileThroughBaseImplementationFailed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setFileCallbackOpenFileThroughBaseImplementationFailedDeprecated();
    #endif
    void setFileCallbackOpenFileAsData();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setFileCallbackOpenFileAsDataDeprecated();
    void setFileCallbackOpenFileAsDataDeprecatedNonZeroFontId();
    #endif
    void setFileCallbackOpenFileAsDataNotFound();
    void setFileCallbackOpenFileAsDataFailed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setFileCallbackOpenFileAsDataFailedDeprecated();
    #endif

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
};

const struct {
    const char* name;
    bool openedBefore;
    /** @todo remove this once the deprecated APIs are gone */
    UnsignedInt fontId;
} OpenDataFileData[]{
    {"", false, 0},
    {"opened before", true, 0},
    {"non-zero font ID", false, 1}
};

/** @todo remove this once the deprecated APIs are gone */
const struct {
    const char* name;
    UnsignedInt fontId;
} OpenData[]{
    {"", 0},
    {"non-zero font ID", 1}
};

const struct {
    const char* name;
    bool fileOpened;
} FontCountData[]{
    {"", false},
    {"with a font opened", true}
};

#ifdef MAGNUM_BUILD_DEPRECATED
const struct {
    const char* name;
    bool openFile;
    UnsignedInt fontId;
} PropertiesNotImplementedData[]{
    {"open data", false, 0},
    {"open data, non-zero font ID", false, 1},
    {"open file", true, 0},
    {"open file, non-zero font ID", true, 1},
};
#endif

AbstractFontTest::AbstractFontTest() {
    addTests({&AbstractFontTest::debugFeature,
              &AbstractFontTest::debugFeaturePacked,
              &AbstractFontTest::debugFeatures,
              &AbstractFontTest::debugFeaturesPacked,

              &AbstractFontTest::construct});

    addInstancedTests({&AbstractFontTest::dataFontCount,
                       &AbstractFontTest::dataFontCountFailed},
        Containers::arraySize(FontCountData));

    addTests({&AbstractFontTest::dataFontCountNotImplemented});

    addInstancedTests({&AbstractFontTest::fileFontCount,
                       &AbstractFontTest::fileFontCountFailed},
        Containers::arraySize(FontCountData));

    addTests({&AbstractFontTest::fileFontCountNotImplemented});

    addInstancedTests({&AbstractFontTest::fileFontCountAsData,
                       &AbstractFontTest::fileFontCountAsDataNotFound,
                       &AbstractFontTest::fileFontCountAsDataFailed},
        Containers::arraySize(FontCountData));

    addTests({&AbstractFontTest::fileFontCountAsDataNotImplemented,
              &AbstractFontTest::fontCountInvalid,
              &AbstractFontTest::dataFontCountNotSupported});

    addInstancedTests({&AbstractFontTest::openData},
        Containers::arraySize(OpenDataFileData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::openDataDeprecated,
              &AbstractFontTest::openDataDeprecatedNonZeroFontId});
    #endif

    addInstancedTests({&AbstractFontTest::openDataFailed},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::openDataFailedDeprecated});
    #endif

    addInstancedTests({&AbstractFontTest::openFile},
        Containers::arraySize(OpenDataFileData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::openFileDeprecated,
              &AbstractFontTest::openFileDeprecatedNonZeroFontId});
    #endif

    addInstancedTests({&AbstractFontTest::openFileFailed},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::openFileFailedDeprecated});
    #endif

    addInstancedTests({&AbstractFontTest::openFileAsData,
                       &AbstractFontTest::openFileAsDataNotFound},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::openFileAsDataDeprecated,
              &AbstractFontTest::openFileAsDataDeprecatedNonZeroFontId});
    #endif

    addInstancedTests({&AbstractFontTest::openFileAsDataFailed},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::openFileAsDataFailedDeprecated});
    #endif

    addInstancedTests({&AbstractFontTest::openFileNotImplemented,
                       &AbstractFontTest::openDataNotSupported,
                       &AbstractFontTest::openDataNotImplemented},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&AbstractFontTest::propertiesNotImplemented},
        Containers::arraySize(PropertiesNotImplementedData));
    #endif

    addTests({&AbstractFontTest::setFileCallback,
              &AbstractFontTest::setFileCallbackTemplate,
              &AbstractFontTest::setFileCallbackTemplateNull,
              &AbstractFontTest::setFileCallbackTemplateConst,
              &AbstractFontTest::setFileCallbackFileOpened,
              &AbstractFontTest::setFileCallbackNotImplemented,
              &AbstractFontTest::setFileCallbackNotSupported});

    addInstancedTests({&AbstractFontTest::setFileCallbackOpenFileDirectly},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::setFileCallbackOpenFileDirectlyDeprecated,
              &AbstractFontTest::setFileCallbackOpenFileDirectlyDeprecatedNonZeroFontId});
    #endif

    addInstancedTests({&AbstractFontTest::setFileCallbackOpenFileDirectlyFailed},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::setFileCallbackOpenFileDirectlyFailedDeprecated});
    #endif

    addInstancedTests({&AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementation},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationDeprecated});
    #endif

    addInstancedTests({&AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationNotFound,
                       &AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationFailed},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationFailedDeprecated});
    #endif

    addInstancedTests({&AbstractFontTest::setFileCallbackOpenFileAsData},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::setFileCallbackOpenFileAsDataDeprecated,
              &AbstractFontTest::setFileCallbackOpenFileAsDataDeprecatedNonZeroFontId});
    #endif

    addInstancedTests({&AbstractFontTest::setFileCallbackOpenFileAsDataNotFound,
                       &AbstractFontTest::setFileCallbackOpenFileAsDataFailed},
        Containers::arraySize(OpenData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractFontTest::setFileCallbackOpenFileAsDataFailedDeprecated});
    #endif

    addTests({&AbstractFontTest::properties,
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
              });
}

using namespace Containers::Literals;

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

void AbstractFontTest::construct() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {
            CORRADE_FAIL("This should not be called");
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    CORRADE_COMPARE(font.features(), FontFeatures{});
    CORRADE_VERIFY(!font.isOpened());

    /* This should be a no-op with doClose() not called at all */
    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

void AbstractFontTest::dataFontCount() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char> data) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xac'}),
                TestSuite::Compare::Container);
            return 37;
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openData(nullptr, 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    const char fontData[]{'\xac'};
    CORRADE_COMPARE(font.dataFontCount(fontData), 37);

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::dataFontCountFailed() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            called = true;
            return {};
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openData(nullptr, 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!font.dataFontCount(nullptr));
        CORRADE_VERIFY(font.called);
        CORRADE_COMPARE(out, "");
    }

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::dataFontCountNotImplemented() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    /* Should call the default doDataFontCount() which returns 1 */
    CORRADE_COMPARE(font.dataFontCount(nullptr), 1);
}

void AbstractFontTest::fileFontCount() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView filename) override {
            CORRADE_COMPARE(filename, "thingy.ttc");
            return 37;
        }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openFile(nullptr, 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    CORRADE_COMPARE(font.fileFontCount("thingy.ttc"), 37);

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::fileFontCountFailed() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView) override {
            called = true;
            return {};
        }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = true;

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openFile(nullptr, 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!font.fileFontCount(""));
        CORRADE_VERIFY(font.called);
        CORRADE_COMPARE(out, "");
    }

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::fileFontCountNotImplemented() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    /* Should call the default doFileFontCount() which returns 1 if OpenData
       is not supported */
    CORRADE_COMPARE(font.fileFontCount(nullptr), 1);
}

void AbstractFontTest::fileFontCountAsData() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char> data) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            return 37;
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    CORRADE_COMPARE(font.fileFontCount(Utility::Path::join(TEXT_TEST_DIR, "data.bin")), 37);

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::fileFontCountAsDataNotFound() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            CORRADE_FAIL("This should not get called.");
            return {};
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!font.fileFontCount("nonexistent.foo"));
        /* There's an error message from Path::read() before */
        CORRADE_COMPARE_AS(out,
            "\nText::AbstractFont::fileFontCount(): cannot open file nonexistent.foo\n",
            TestSuite::Compare::StringHasSuffix);
    }

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::fileFontCountAsDataFailed() {
    auto&& data = FontCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_FAIL("This should not get called.");
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            called = true;
            return {};
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;

        private:
            bool _opened = false;
    } font;

    /* Verify that the API doesn't affect the currently opened font in any way
       -- it stays open if it was, and stays closed if wasn't */
    if(data.fileOpened)
        CORRADE_VERIFY(font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 1.0f));
    CORRADE_COMPARE(font.isOpened(), data.fileOpened);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!font.fileFontCount(Utility::Path::join(TEXT_TEST_DIR, "data.bin")));
        CORRADE_VERIFY(font.called);
        CORRADE_COMPARE(out, "");
    }

    CORRADE_COMPARE(font.isOpened(), data.fileOpened);
}

void AbstractFontTest::fileFontCountAsDataNotImplemented() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    /* Should call the default doFileFontCount() which then delegates to
       the default doDataFontCount() which then returns 1 */
    CORRADE_COMPARE(font.dataFontCount(nullptr), 1);
}

void AbstractFontTest::fontCountInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView) override {
            return 0;
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            return 0;
        }

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.fileFontCount(nullptr);
    font.dataFontCount(nullptr);
    CORRADE_COMPARE_AS(out,
        "Text::AbstractFont::fileFontCount(): implementation returned zero\n"
        "Text::AbstractFont::dataFontCount(): implementation returned zero\n",
        TestSuite::Compare::String);
}

void AbstractFontTest::dataFontCountNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    font.dataFontCount(nullptr);
    CORRADE_COMPARE(out, "Text::AbstractFont::dataFontCount(): feature not supported\n");
}

void AbstractFontTest::openData() {
    auto&& data = OpenDataFileData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return opened; }
        void doClose() override {
            CORRADE_VERIFY(opened);
            opened = false;
        }

        void doOpenData(Containers::ArrayView<const char> data, Float size, UnsignedInt fontId) override {
            CORRADE_VERIFY(!opened);
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(size, 13.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            opened = true;
        }

        Properties doProperties() override {
            return {31.0f, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        UnsignedInt expectedFontId;
        bool opened;
    } font;
    font.expectedFontId = data.fontId;
    font.opened = data.openedBefore;
    CORRADE_COMPARE(font.isOpened(), data.openedBefore);

    const char a5[]{'\xa5'};
    /* Verify that even with font ID 0 it delegates to the non-deprecated
       doOpenData() overload */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(font.openData(a5, 13.0f, data.fontId));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 31.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);

    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::openDataDeprecated() {
    /* Like openData(), but implementing the deprecated doOpenData() and
       verifying that it's correctly delegated to */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_VERIFY(_opened);
            _opened = false;
        }

        Properties doOpenData(Containers::ArrayView<const char> data, Float size) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    CORRADE_VERIFY(!font.isOpened());
    const char a5[]{'\xa5'};
    /* Should delegate to the deprecated doOpenData(), and fail with font ID
       being non-zero, which is tested below */
    CORRADE_VERIFY(font.openData(a5, 13.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);

    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

void AbstractFontTest::openDataDeprecatedNonZeroFontId() {
    /* When openData() gets a non-zero font ID, it should not delegate to the
       deprecated overload but fail directly */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    CORRADE_VERIFY(!font.isOpened());
    const char a5[]{'\xa5'};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openData(a5, 13.0f, 1));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): cannot open font at index 1\n");
}
#endif

void AbstractFontTest::openDataFailed() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            called = true;
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openData(nullptr, 1.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::openDataFailedDeprecated() {
    /* Like openDataFailed(), but implementing the deprecated doOpenData() and
       verifying that it's correctly delegated to */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            called = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openData(nullptr, 1.0f));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}
#endif

void AbstractFontTest::openFile() {
    auto&& data = OpenDataFileData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return opened; }
        void doClose() override {
            CORRADE_VERIFY(opened);
            opened = false;
        }

        void doOpenFile(Containers::StringView filename, Float size, UnsignedInt fontId) override {
            CORRADE_COMPARE(filename, "hello.ttf");
            CORRADE_COMPARE(size, 13.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            opened = true;
        }

        Properties doProperties() override {
            return {31.0f, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        UnsignedInt expectedFontId;
        bool opened;
    } font;
    font.expectedFontId = data.fontId;
    font.opened = data.openedBefore;
    CORRADE_COMPARE(font.isOpened(), data.openedBefore);

    /* Verify that even with font ID 0 it delegates to the non-deprecated
       doOpenFile() overload */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(font.openFile("hello.ttf", 13.0f, data.fontId));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 31.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);

    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::openFileDeprecated() {
    /* Like openFile(), but implementing the deprecated doOpenFile() and
       verifying that it's correctly delegated to */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_VERIFY(_opened);
            _opened = false;
        }

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            CORRADE_COMPARE(filename, "hello.ttf");
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    CORRADE_VERIFY(!font.isOpened());
    /* Should delegate to the deprecated doOpenFile(), and fail with font ID
       being non-zero, which is tested below */
    CORRADE_VERIFY(font.openFile("hello.ttf", 13.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);

    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

void AbstractFontTest::openFileDeprecatedNonZeroFontId() {
    /* When openFile() gets a non-zero font ID, it should not delegate to the
       deprecated overload but fail directly */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    CORRADE_VERIFY(!font.isOpened());

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("hello.ttf", 13.0f, 1));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_COMPARE(out, "Text::AbstractFont::openFile(): cannot open font at index 1\n");
}
#endif

void AbstractFontTest::openFileFailed() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            called = true;
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("hello.ttf", 1.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::openFileFailedDeprecated() {
    /* Like openFileFailed(), but implementing the deprecated doOpenFile() and
       verifying that it's correctly delegated to */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView, Float) override {
            called = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("hello.ttf", 1.0f));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}
#endif

void AbstractFontTest::openFileAsData() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char> data, Float size, UnsignedInt fontId) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(size, 13.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            _opened = true;
        }

        Properties doProperties() override {
            return {31.0f, 1.0f, 2.0f, 3.0f, 15};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        UnsignedInt expectedFontId;

        private:
            bool _opened = false;
    } font;
    font.expectedFontId = data.fontId;

    /* doOpenFile() should call doOpenData() */
    CORRADE_VERIFY(!font.isOpened());
    /* Verify that even with font ID 0 it delegates to the non-deprecated
       doOpenData() overload */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 13.0f, data.fontId));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 31.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::openFileAsDataNotFound() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("nonexistent.foo", 12.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    /* There's an error message from Path::read() before */
    CORRADE_COMPARE_AS(out,
        "\nText::AbstractFont::openFile(): cannot open file nonexistent.foo\n",
        TestSuite::Compare::StringHasSuffix);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::openFileAsDataDeprecated() {
    /* Like openFileAsData(), but implementing the deprecated doOpenData() and
       verifying that it's correctly delegated to */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {
            CORRADE_VERIFY(_opened);
            _opened = false;
        }

        Properties doOpenData(Containers::ArrayView<const char> data, Float size) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    CORRADE_VERIFY(!font.isOpened());
    /* Should delegate to the deprecated doOpenData(), and fail with font ID
       being non-zero, which is tested below */
    CORRADE_VERIFY(font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 13.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);

    font.close();
    CORRADE_VERIFY(!font.isOpened());
}

void AbstractFontTest::openFileAsDataDeprecatedNonZeroFontId() {
    /* When openFile() gets a non-zero font ID, it should not delegate to the
       deprecated overload but fail directly (after actually opening the file
       and delegating to data) */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile(Utility::Path::join(TEXT_TEST_DIR, "data.bin"), 13.0f, 1));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): cannot open font at index 1\n");
}
#endif

void AbstractFontTest::openFileAsDataFailed() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            called = true;
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openData(nullptr, 1.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::openFileAsDataFailedDeprecated() {
    /* Like openFileAsDataFailed(), but implementing the deprecated
       doOpenData() and verifying that it's correctly delegated to */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            called = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openData(nullptr, 1.0f));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}
#endif

void AbstractFontTest::openFileNotImplemented() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    /** @todo remove the instanced font ID and use the default argument once
        the deprecated APIs are gone, as there will be no delegation variants
        to test */
    font.openFile("file.foo", 34.0f, data.fontId);
    {
        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_EXPECT_FAIL_IF(data.fontId != 0,
            "On deprecated builds it's impossible to detect that even the deprecated doOpenFile() overload was not implemented for a non-zero font ID, and a different (non-asserting) message is printed.");
        #endif
        CORRADE_COMPARE(out, "Text::AbstractFont::openFile(): not implemented\n");
    }
    #ifdef MAGNUM_BUILD_DEPRECATED
    /* This is printed for non-zero font IDs on deprecated builds instead */
    if(data.fontId != 0)
        CORRADE_COMPARE(out, "Text::AbstractFont::openFile(): cannot open font at index 1\n");
    #endif
}

void AbstractFontTest::openDataNotSupported() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    /** @todo remove the instanced font ID and use the default argument once
        the deprecated APIs are gone, as there will be no delegation variants
        to test */
    font.openData(nullptr, 34.0f, data.fontId);
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): feature not supported\n");
}

void AbstractFontTest::openDataNotImplemented() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Containers::String out;
    Error redirectError{&out};
    /** @todo remove the instanced font ID and use the default argument once
        the deprecated APIs are gone, as there will be no delegation variants
        to test */
    font.openData(nullptr, 34.0f, data.fontId);
    {
        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_EXPECT_FAIL_IF(data.fontId != 0,
            "On deprecated builds it's impossible to detect that even the deprecated doOpenData() overload was not implemented for a non-zero font ID, and a different (non-asserting) message is printed.");
        #endif
        CORRADE_COMPARE(out, "Text::AbstractFont::openData(): feature advertised but not implemented\n");
    }
    #ifdef MAGNUM_BUILD_DEPRECATED
    /* This is printed for non-zero font IDs on deprecated builds instead */
    if(data.fontId != 0)
        CORRADE_COMPARE(out, "Text::AbstractFont::openData(): cannot open font at index 1\n");
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::propertiesNotImplemented() {
    auto&& data = PropertiesNotImplementedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* On non-deprecated builds doProperties() is pure virtual, which forces
       its implementation at compile time. This assertion is to tell the same
       to implementers on builds with deprecated features enabled, although
       only at runtime. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            _opened = true;
        }
        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    Containers::String out;
    Error redirectError{&out};
    if(data.openFile)
        font.openFile({}, 1.0f, data.fontId);
    else
        font.openData({}, 1.0f, data.fontId);
    CORRADE_COMPARE(out, "Text::AbstractFont: doProperties() not implemented\n");
}
#endif

void AbstractFontTest::setFileCallback() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData|FontFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            *static_cast<int*>(userData) = 1337;
        }

        Properties doProperties() override { return {}; }
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

        Properties doProperties() override { return {}; }
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
            CORRADE_VERIFY(!callback);
            CORRADE_VERIFY(!userData);
            called = true;
        }

        Properties doProperties() override { return {}; }
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

        Properties doProperties() override { return {}; }
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

        Properties doProperties() override { return {}; }
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

        Properties doProperties() override { return {}; }
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

        Properties doProperties() override { return {}; }
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
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView filename) override {
            /* Called because FileCallback is supported */
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            return 37;
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doOpenFile(Containers::StringView filename, Float size, UnsignedInt fontId) override {
            /* Called because FileCallback is supported */
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_COMPARE(size, 42.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            _opened = true;
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        Properties doProperties() override {
            return {24.0f, 1.0f, 2.0f, 3.0f, 15};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        UnsignedInt expectedFontId;

        private:
            bool _opened = false;
    } font;
    font.expectedFontId = data.fontId;

    /* The callback shouldn't be called from the class itself, it's the
       doOpenFile() implementation responsibility to call it. In this case the
       implementation only verifies that it's set, along with the user data. */
    int dummy;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_FAIL("This should not be called");
        return {};
    }, &dummy);

    CORRADE_COMPARE(font.fileFontCount("file.dat"), 37);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(font.openFile("file.dat", 42.0f, data.fontId));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 24.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::setFileCallbackOpenFileDirectlyDeprecated() {
    /* Like setFileCallbackOpenFileDirectly(), but implementing the deprecated
       doOpenFile() and verifying that it's correctly delegated to. There's no
       deprecated doFileFontCount() variant so that part is omitted here. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            /* Called because FileCallback is supported */
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }
        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    /* The callback shouldn't be called from the class itself, it's the
       doOpenFile() implementation responsibility to call it. In this case the
       implementation only verifies that it's set, along with the user data. */
    int dummy;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_FAIL("This shouldn't be called");
        return {};
    }, &dummy);

    CORRADE_VERIFY(font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 42.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::setFileCallbackOpenFileDirectlyDeprecatedNonZeroFontId() {
    /* When openFile() gets a non-zero font ID, it should not delegate to the
       deprecated overload but fail directly (after actually opening the file
       through the callback and delegating to data) */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    struct State {
        bool loaded = false;
        bool closed = false;
    } state;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::ArrayView<const char>{};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f, 1));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): cannot open font at index 1\n");
}
#endif

void AbstractFontTest::setFileCallbackOpenFileDirectlyFailed() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView) override {
            countCalled = true;
            return {};
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            openCalled = true;
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool countCalled = false;
        bool openCalled = false;
    } font;

    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_FAIL("This shouldn't be called");
        return {};
    });

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.fileFontCount("file.dat"));
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(!font.openCalled);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(font.openCalled);
    CORRADE_COMPARE(out, "");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::setFileCallbackOpenFileDirectlyFailedDeprecated() {
    /* Like setFileCallbackOpenFileDirectly(), but implementing the deprecated
       doOpenFile() and verifying that it's correctly delegated to. There's no
       deprecated doFileFontCount() variant so that part is omitted here. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView, Float) override {
            called = true;
            return {};
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }
        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    font.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_FAIL("This shouldn't be called");
        return {};
    });

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_COMPARE(out, "");
}
#endif

void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementation() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView filename) override {
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            countCalled = true;
            return AbstractFont::doFileFontCount(filename);
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char> data) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            return 37;
        }

        void doOpenFile(Containers::StringView filename, Float size, UnsignedInt fontId) override {
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_COMPARE(size, 42.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            openCalled = true;
            AbstractFont::doOpenFile(filename, size, fontId);
        }

        void doOpenData(Containers::ArrayView<const char> data, Float size, UnsignedInt fontId) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(size, 42.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            _opened = true;
        }

        Properties doProperties() override {
            return {24.0f, 1.0f, 2.0f, 3.0f, 15};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        UnsignedInt expectedFontId;
        bool countCalled = false;
        bool openCalled = false;

        private:
            bool _opened = false;
    } font;
    font.expectedFontId = data.fontId;

    struct State {
        const char data = '\xb0';
        Int loaded = 0;
        Int closed = 0;
    } state;
    font.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_COMPARE(Containers::StringView{filename}, "file.dat");

        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            ++state.loaded;
            return Containers::arrayView(&state.data, 1);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            ++state.closed;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    CORRADE_COMPARE(font.fileFontCount("file.dat"), 37);
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(!font.openCalled);
    CORRADE_COMPARE(state.loaded, 1);
    CORRADE_COMPARE(state.closed, 1);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(font.openFile("file.dat", 42.0f, data.fontId));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(font.openCalled);
    CORRADE_COMPARE(state.loaded, 2);
    CORRADE_COMPARE(state.closed, 2);
    CORRADE_COMPARE(font.size(), 24.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationDeprecated() {
    /* Like setFileCallbackOpenFileThroughBaseImplementation(), but
       implementing the deprecated doOpenFile() and doOpenData() and verifying
       that it's correctly delegated to. There's no deprecated
       doFileFontCount() or doDataFontCount() variant so that part is omitted
       here. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            called = true;
            CORRADE_IGNORE_DEPRECATED_PUSH
            return AbstractFont::doOpenFile(filename, size);
            CORRADE_IGNORE_DEPRECATED_POP
        }

        Properties doOpenData(Containers::ArrayView<const char> data, Float size) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;

        private:
            bool _opened = false;
    } font;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;
    font.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_COMPARE(Containers::StringView{filename}, "file.dat");

        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    CORRADE_VERIFY(font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(font.size(), 42.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}
#endif

void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationNotFound() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView filename) override {
            countCalled = true;
            return AbstractFont::doFileFontCount(filename);
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            CORRADE_FAIL("This should not be called");
            return 37;
        }

        void doOpenFile(Containers::StringView filename, Float size, UnsignedInt fontId) override {
            openCalled = true;
            AbstractFont::doOpenFile(filename, size, fontId);
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool countCalled = false;
        bool openCalled = false;
    } font;

    Int fileCallbackCalled = 0;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, Int& fileCallbackCalled) -> Containers::Optional<Containers::ArrayView<const char>> {
        /* The callback should be only called to open the file, not to close it
           afterwards */
        CORRADE_COMPARE(policy, InputFileCallbackPolicy::LoadTemporary);
        ++fileCallbackCalled;
        return {};
    }, fileCallbackCalled);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.fileFontCount("file.dat"));
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(!font.openCalled);
    CORRADE_COMPARE(fileCallbackCalled, 1);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(font.openCalled);
    CORRADE_COMPARE(fileCallbackCalled, 2);
    CORRADE_COMPARE_AS(out,
        "Text::AbstractFont::fileFontCount(): cannot open file file.dat\n"
        "Text::AbstractFont::openFile(): cannot open file file.dat\n",
        TestSuite::Compare::String);
}

void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationFailed() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView filename) override {
            fileCountCalled = true;
            return AbstractFont::doFileFontCount(filename);
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            dataCountCalled = true;
            return {};
        }

        void doOpenFile(Containers::StringView filename, Float size, UnsignedInt fontId) override {
            openFileCalled = true;
            AbstractFont::doOpenFile(filename, size, fontId);
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            openDataCalled = true;
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool fileCountCalled = false;
        bool dataCountCalled = false;
        bool openFileCalled = false;
        bool openDataCalled = false;
    } font;

    struct State {
        Int loaded = 0;
        Int closed = 0;
    } state;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            ++state.loaded;
            return Containers::ArrayView<const char>{};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            ++state.closed;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.fileFontCount("file.dat"));
    CORRADE_VERIFY(font.fileCountCalled);
    CORRADE_VERIFY(font.dataCountCalled);
    CORRADE_VERIFY(!font.openFileCalled);
    CORRADE_VERIFY(!font.openDataCalled);
    CORRADE_COMPARE(state.loaded, 1);
    CORRADE_COMPARE(state.closed, 1);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.fileCountCalled);
    CORRADE_VERIFY(font.dataCountCalled);
    CORRADE_VERIFY(font.openFileCalled);
    CORRADE_VERIFY(font.openDataCalled);
    CORRADE_COMPARE(state.loaded, 2);
    CORRADE_COMPARE(state.closed, 2);
    CORRADE_COMPARE(out, "");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::setFileCallbackOpenFileThroughBaseImplementationFailedDeprecated() {
    /* Like setFileCallbackOpenFileThroughBaseImplementationFailed(), but
       implementing the deprecated doOpenFile() and doOpenData() and verifying
       that it's correctly delegated to. There's no deprecated
       doFileFontCount() or doDataFontCount() variant so that part is omitted
       here. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::FileCallback|FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doOpenFile(Containers::StringView filename, Float size) override {
            openFileCalled = true;
            CORRADE_IGNORE_DEPRECATED_PUSH
            return AbstractFont::doOpenFile(filename, size);
            CORRADE_IGNORE_DEPRECATED_POP
        }

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            openDataCalled = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool openFileCalled = false;
        bool openDataCalled = false;
    } font;

    struct State {
        bool loaded = false;
        bool closed = false;
    } state;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::ArrayView<const char>{};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("file.dat", 42.0f));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.openFileCalled);
    CORRADE_VERIFY(font.openDataCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(out, "");
}
#endif

void AbstractFontTest::setFileCallbackOpenFileAsData() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char> data) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            return 37;
        }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        void doOpenData(Containers::ArrayView<const char> data, Float size, UnsignedInt fontId) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(size, 13.0f);
            CORRADE_COMPARE(fontId, expectedFontId);
            _opened = true;
        }

        Properties doProperties() override {
            return {31.0f, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        UnsignedInt expectedFontId;

        private:
            bool _opened = false;
    } font;
    font.expectedFontId = data.fontId;

    struct State {
        const char data = '\xb0';
        Int loaded = 0;
        Int closed = 0;
    } state;
    font.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_COMPARE(Containers::StringView{filename}, "file.dat");

        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            ++state.loaded;
            return Containers::arrayView(&state.data, 1);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            ++state.closed;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    CORRADE_COMPARE(font.fileFontCount("file.dat"), 37);
    CORRADE_COMPARE(state.loaded, 1);
    CORRADE_COMPARE(state.closed, 1);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(font.openFile("file.dat", 13.0f, data.fontId));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(state.loaded, 2);
    CORRADE_COMPARE(state.closed, 2);
    CORRADE_COMPARE(font.size(), 31.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::setFileCallbackOpenFileAsDataDeprecated() {
    /* Like setFileCallbackOpenFileAsData(), but implementing the deprecated
       doOpenData() and verifying that it's correctly delegated to. There's no
       deprecated doDataFontCount() variant so that part is omitted here. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }
        Properties doOpenFile(Containers::StringView, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Properties doOpenData(Containers::ArrayView<const char> data, Float size) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            _opened = true;
            return {size, 1.0f, 2.0f, 3.0f, 15};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;
    font.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        CORRADE_COMPARE(Containers::StringView{filename}, "file.dat");

        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    CORRADE_VERIFY(font.openFile("file.dat", 13.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::setFileCallbackOpenFileAsDataDeprecatedNonZeroFontId() {
    /* When openData() gets a non-zero font ID, it should not delegate to the
       deprecated overload but fail directly */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }
        Properties doOpenFile(Containers::StringView, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;

    struct State {
        bool loaded = false;
        bool closed = false;
    } state;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::ArrayView<const char>{};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("file.dat", 13.0f, 1));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(out, "Text::AbstractFont::openData(): cannot open font at index 1\n");
}
#endif

void AbstractFontTest::setFileCallbackOpenFileAsDataNotFound() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;

    Int fileCallbackCalled = 0;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, Int& fileCallbackCalled) -> Containers::Optional<Containers::ArrayView<const char>> {
        /* The callback should be only called to open the file, not to close it
           afterwards */
        CORRADE_COMPARE(policy, InputFileCallbackPolicy::LoadTemporary);
        ++fileCallbackCalled;
        return {};
    }, fileCallbackCalled);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.fileFontCount("file.dat"));
    CORRADE_COMPARE(fileCallbackCalled, 1);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("file.dat", 132.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_COMPARE(fileCallbackCalled, 2);
    CORRADE_COMPARE_AS(out,
        "Text::AbstractFont::fileFontCount(): cannot open file file.dat\n"
        "Text::AbstractFont::openFile(): cannot open file file.dat\n",
        TestSuite::Compare::String);
}

void AbstractFontTest::setFileCallbackOpenFileAsDataFailed() {
    auto&& data = OpenData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Containers::Optional<UnsignedInt> doFileFontCount(Containers::StringView) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Containers::Optional<UnsignedInt> doDataFontCount(Containers::ArrayView<const char>) override {
            countCalled = true;
            return {};
        }

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            openCalled = true;
        }

        Properties doProperties() override {
            CORRADE_FAIL("This should not be called");
            return {};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool countCalled = false;
        bool openCalled = false;
    } font;

    struct State {
        Int loaded = 0;
        Int closed = 0;
    } state;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            ++state.loaded;
            return Containers::ArrayView<const char>{};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            ++state.closed;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.fileFontCount("file.dat"));
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(!font.openCalled);
    CORRADE_COMPARE(state.loaded, 1);
    CORRADE_COMPARE(state.closed, 1);
    /* Verify that the behavior is the same also with a potential deprecated
       overload for font 0 */
    /** @todo remove the instanced font ID once the deprecated APIs are gone,
        supply a constant instead */
    CORRADE_VERIFY(!font.openFile("file.dat", 132.0f, data.fontId));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.countCalled);
    CORRADE_VERIFY(font.openCalled);
    CORRADE_COMPARE(state.loaded, 2);
    CORRADE_COMPARE(state.closed, 2);
    CORRADE_COMPARE(out, "");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractFontTest::setFileCallbackOpenFileAsDataFailedDeprecated() {
    /* Like setFileCallbackOpenFileAsDataFailed(), but implementing the
       deprecated doOpenData() and verifying that it's correctly delegated to.
       There's no deprecated doDataFontCount() variant so that part is omitted
       here. */

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenFile(Containers::StringView, Float, UnsignedInt) override {
            CORRADE_FAIL("This should not be called");
        }
        Properties doOpenFile(Containers::StringView, Float) override {
            CORRADE_FAIL("This should not be called");
            return {};
        }

        Properties doOpenData(Containers::ArrayView<const char>, Float) override {
            called = true;
            return {};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool called = false;
    } font;

    struct State {
        bool loaded = false;
        bool closed = false;
    } state;
    font.setFileCallback([](const std::string&, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::ArrayView<const char>{};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_FAIL("Unexpected policy" << policy);
        return {};
    }, state);

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("file.dat", 132.0f));
    CORRADE_VERIFY(!font.isOpened());
    CORRADE_VERIFY(font.called);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(out, "");
}
#endif

void AbstractFontTest::properties() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
            return {13.0f, 1.0f, 2.0f, 3.0f, 15};
        }
        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;

    CORRADE_VERIFY(font.openData(nullptr, 0.0f));
    CORRADE_VERIFY(font.isOpened());
    CORRADE_COMPARE(font.size(), 13.0f);
    CORRADE_COMPARE(font.ascent(), 1.0f);
    CORRADE_COMPARE(font.descent(), 2.0f);
    CORRADE_COMPARE(font.lineHeight(), 3.0f);
    CORRADE_COMPARE(font.glyphCount(), 15);
}

void AbstractFontTest::propertiesNoFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override {
            return FontFeature::OpenData;
        }
        bool doIsOpened() const override { return _opened; }
        void doClose() override {}

        void doOpenData(Containers::ArrayView<const char>, Float, UnsignedInt) override {
            _opened = true;
        }

        Properties doProperties() override {
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return FontFeature::PreparedGlyphCache; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Properties doProperties() override { return {}; }
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

    struct: AbstractFont {
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
    struct: AbstractFont {
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
    struct: AbstractFont {
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

    struct: AbstractFont {
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
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 200}};
    CORRADE_IGNORE_DEPRECATED_PUSH
    font.layout(cache, 0.25f, "hello");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractFont::layout(): no font opened\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontTest)
