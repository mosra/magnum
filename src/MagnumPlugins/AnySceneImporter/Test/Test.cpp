/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <sstream>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Trade/MeshData3D.h>

#include "MagnumPlugins/AnySceneImporter/AnySceneImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

struct AnySceneImporterTest: TestSuite::Tester {
    explicit AnySceneImporterTest();

    void obj();
    void ply();

    void unknown();

    private:
        PluginManager::Manager<AbstractImporter> _manager;
};

AnySceneImporterTest::AnySceneImporterTest(): _manager{MAGNUM_PLUGINS_IMPORTER_DIR} {
    addTests({&AnySceneImporterTest::obj,
              &AnySceneImporterTest::ply,

              &AnySceneImporterTest::unknown});
}

void AnySceneImporterTest::obj() {
    if(_manager.loadState("ObjImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("ObjImporter plugin not found, cannot test");

    AnySceneImporter importer{_manager};
    CORRADE_VERIFY(importer.openFile(OBJ_FILE));

    /* Check only size, as it is good enough proof that it is working */
    std::optional<MeshData3D> mesh = importer.mesh3D(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->positions(0).size(), 3);
}

void AnySceneImporterTest::ply() {
    if(_manager.loadState("StanfordImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StanfordImporter plugin not found, cannot test");

    AnySceneImporter importer{_manager};
    CORRADE_VERIFY(importer.openFile(PLY_FILE));

    /* Check only size, as it is good enough proof that it is working */
    std::optional<MeshData3D> mesh = importer.mesh3D(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->positions(0).size(), 5);
}

void AnySceneImporterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    AnySceneImporter importer{_manager};
    CORRADE_VERIFY(!importer.openFile("mesh.wtf"));

    CORRADE_COMPARE(output.str(), "Trade::AnySceneImporter::openFile(): cannot determine type of file mesh.wtf\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneImporterTest)
