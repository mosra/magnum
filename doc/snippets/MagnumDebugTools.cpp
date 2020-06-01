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

#include <chrono>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/DebugTools/FrameProfiler.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/AbstractImporter.h"

using namespace Magnum;

namespace {
    Image2D doProcessing() {
        return Image2D{PixelFormat::RGBA8Unorm, {}, {}};
    }

    Image2D loadExpectedImage() {
        return Image2D{PixelFormat::RGBA8Unorm, {}, {}};
    }
}

struct Foo: TestSuite::Tester {
void foo() {
{
/* [CompareImageFile] */
CORRADE_COMPARE_WITH("actual.png", "expected.png",
    (DebugTools::CompareImageFile{1.5f, 0.01f}));
/* [CompareImageFile] */
}

{
/* [CompareImageFile-manager] */
PluginManager::Manager<Trade::AbstractImporter> manager;

CORRADE_COMPARE_WITH("actual.png", "expected.png",
    (DebugTools::CompareImageFile{manager, 1.5f, 0.01f}));
/* [CompareImageFile-manager] */
}

{
/* [CompareImageFile-skip] */
PluginManager::Manager<Trade::AbstractImporter> manager;
if(manager.loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
   manager.loadState("PngImporter") == PluginManager::LoadState::NotFound)
    CORRADE_SKIP("AnyImageImporter/PngImporter not found, can't compare.");

CORRADE_COMPARE_WITH("actual.png", "expected.png",
    (DebugTools::CompareImageFile{manager, 1.5f, 0.01f}));
/* [CompareImageFile-skip] */
}

{
/* [CompareImageToFile] */
Image2D actual = doProcessing();
CORRADE_COMPARE_WITH(actual, "expected.png",
    (DebugTools::CompareImageToFile{1.5f, 0.01f}));
/* [CompareImageToFile] */

/* [CompareFileToImage] */
Image2D expected = loadExpectedImage();
CORRADE_COMPARE_WITH("actual.png", expected,
    (DebugTools::CompareFileToImage{1.5f, 0.01f}));
/* [CompareFileToImage] */
}

{
Image2D actual = doProcessing();
Image2D expected = loadExpectedImage();
/* [CompareImage-pixels-flip] */
CORRADE_COMPARE_WITH(actual.pixels<Color3ub>().flipped<0>(), expected,
    (DebugTools::CompareImage{1.5f, 0.01f}));
/* [CompareImage-pixels-flip] */
}
}
};

struct MyApp {
    void drawEvent();
    void drawEventAgain();
    void swapBuffers();
    void redraw();

    DebugTools::FrameProfiler _profiler;
};

/* [FrameProfiler-usage] */
void MyApp::drawEvent() {
    _profiler.beginFrame();

    // actual drawing code …

    _profiler.endFrame();

    // possibly other code (such as UI) you don't want to have included in the
    // measurements …

    swapBuffers();
    redraw();
}
/* [FrameProfiler-usage] */

void MyApp::drawEventAgain() {
/* [FrameProfiler-usage-console] */
    _profiler.endFrame();
    _profiler.printStatistics(10);

    swapBuffers();
    if(_profiler.isEnabled()) redraw();
}
/* [FrameProfiler-usage-console] */

int main() {
{
/* [FrameProfiler-setup-immediate] */
using std::chrono::high_resolution_clock;

high_resolution_clock::time_point frameBeginTime;
DebugTools::FrameProfiler profiler{{
    DebugTools::FrameProfiler::Measurement{"CPU time",
        DebugTools::FrameProfiler::Units::Nanoseconds,
        [](void* state) {
            *static_cast<high_resolution_clock::time_point*>(state)
                = high_resolution_clock::now();
        },
        [](void* state) {
            return UnsignedLong(
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    *static_cast<high_resolution_clock::time_point*>(state)
                    - high_resolution_clock::now()).count());
        }, &frameBeginTime}
}, 50};
/* [FrameProfiler-setup-immediate] */
}

}
