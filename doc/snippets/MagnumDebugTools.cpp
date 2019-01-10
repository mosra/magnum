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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Image.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/DebugTools/CompareImage.h>
#include <Magnum/DebugTools/ResourceManager.h>
#include <Magnum/DebugTools/ObjectRenderer.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Trade/AbstractImporter.h>

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
    (DebugTools::CompareImageFile{15.5f, 5.0f}));
/* [CompareImageFile] */
}

{
/* [CompareImageFile-manager] */
PluginManager::Manager<Trade::AbstractImporter> manager;

CORRADE_COMPARE_WITH("actual.png", "expected.png",
    (DebugTools::CompareImageFile{manager, 15.5f, 5.0f}));
/* [CompareImageFile-manager] */
}

{
/* [CompareImageFile-skip] */
PluginManager::Manager<Trade::AbstractImporter> manager;
if(manager.loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
   manager.loadState("PngImporter") == PluginManager::LoadState::NotFound)
    CORRADE_SKIP("AnyImageImporter/PngImporter not found, can't compare.");

CORRADE_COMPARE_WITH("actual.png", "expected.png",
    (DebugTools::CompareImageFile{manager, 15.5f, 5.0f}));
/* [CompareImageFile-skip] */
}

{
/* [CompareImageToFile] */
Image2D actual = doProcessing();
CORRADE_COMPARE_WITH(actual, "expected.png",
    (DebugTools::CompareImageToFile{15.5f, 5.0f}));
/* [CompareImageToFile] */

/* [CompareFileToImage] */
Image2D expected = loadExpectedImage();
CORRADE_COMPARE_WITH("actual.png", expected,
    (DebugTools::CompareFileToImage{15.5f, 5.0f}));
/* [CompareFileToImage] */
}
}
};

int main() {
{
SceneGraph::Object<SceneGraph::MatrixTransformation3D>* object{};
/* [debug-tools-renderers] */
// Global instance of debug resource manager, drawable group for the renderers
DebugTools::ResourceManager manager;
SceneGraph::DrawableGroup3D debugDrawables;

// Create renderer options which will be referenced later by "my" resource key
DebugTools::ResourceManager::instance().set("my",
    DebugTools::ObjectRendererOptions{}.setSize(0.3f));

// Create debug renderer for given object, use "my" options for it. The
// renderer is automatically added to the object features and also to
// specified drawable group.
new DebugTools::ObjectRenderer3D{*object, "my", &debugDrawables};
/* [debug-tools-renderers] */
}
}
