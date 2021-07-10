/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Magnum.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SceneDataTest: TestSuite::Tester {
    explicit SceneDataTest();

    void customFieldName();
    void customFieldNameTooLarge();
    void customFieldNameNotCustom();
    void debugFieldName();

    template<class T> void fieldTypeSize();
    void debugFieldType();
    void debugObjectIndexType();

    void constructField();
    void constructFieldDefault();
    void constructFieldCustom();
    void constructField2D();
    void constructField2DWrongSize();
    void constructField2DNonContiguous();
    void constructFieldTypeErased();
    void constructFieldNonOwningArray();
    void constructFieldOffsetOnly();
    void constructFieldInconsistentViewSize();
    void constructFieldWrongType();
    void constructFieldWrongStride();
    void constructFieldWrongDataAccess();

    void constructArrayField();
    void constructArrayFieldNonContiguous();
    void constructArrayField2D();
    void constructArrayField2DWrongSize();
    void constructArrayField2DNonContiguous();
    void constructArrayFieldTypeErased();
    void constructArrayFieldOffsetOnly();
    void constructArrayFieldNotAllowed();

    // TODO where that sorted flag?

    void construct();
    void constructZeroFields();
    void constructZeroObjects();

    void constructNotOwned();

    void constructDuplicateField();
    void constructInconsistentIndexType();
    void constructIndexTypeTooSmall();
    void constructNotOwnedFlagOwned();
    void constructMismatchedTRSViews();
    void constructMismatchedMaterialView();

    void constructCopy();
    void constructMove();

    template<class T> void fieldObjectMappingAsArray();
    void fieldObjectMappingAsArrayLongType();
    void fieldObjectMappingIntoArrayInvalidSize();
    template<class T> void parentsAsArray();
    void parentsAsArrayLongType();
    void parentsIntoArrayInvalidSize();
    template<class T> void transformations2DAsArray();
    template<class T> void transformations2DAsArrayTRS();
    void transformations2DAsArrayBut3DType();
    void transformations2DAsArrayBut3DTypeTRS();
    void transformations2DIntoArrayInvalidSize();
    template<class T> void transformations3DAsArray();
    template<class T> void transformationsTRS3DAsArray();
    void transformations3DIntoArrayInvalidSize();
    void transformations3DBut2DType();
    void transformations3DButTRS2DType();
    template<class T> void meshIdsAsArray();
    void meshIdsIntoArrayInvalidSize();
    template<class T> void meshMaterialIdsAsArray();
    void meshMaterialIdsIntoArrayInvalidSize();
    template<class T> void lightIdsAsArray();
    void lightIdsIntoArrayInvalidSize();
    template<class T> void cameraIdsAsArray();
    void cameraIdsIntoArrayInvalidSize();
    template<class T> void skinIdsAsArray();
    void skinIdsIntoArrayInvalidSize();

    void arrayAttribute();
    void arrayAttributeWrongAccess();

    void mutableAccessNotAllowed();

    void fieldObjectMappingNotFound();
    void fieldObjectMappingWrongType();

    void fieldNotFound();
    void fieldWrongType();

    void releaseData();
    void releaseFieldData();
};

SceneDataTest::SceneDataTest() {
    addTests({&SceneDataTest::construct,
              &SceneDataTest::constructCopy,
              &SceneDataTest::constructMove});
}

void SceneDataTest::construct() {
    const int a{};
    const SceneData data{{0, 1, 4}, {2, 5}, &a};

    CORRADE_COMPARE(data.children2D(), (std::vector<UnsignedInt>{0, 1, 4}));
    CORRADE_COMPARE(data.children3D(), (std::vector<UnsignedInt>{2, 5}));
    CORRADE_COMPARE(data.importerState(), &a);
}

void SceneDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<SceneData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<SceneData>{});
}

void SceneDataTest::constructMove() {
    const int a{};
    SceneData data{{0, 1, 4}, {2, 5}, &a};

    SceneData b{std::move(data)};

    CORRADE_COMPARE(b.children2D(), (std::vector<UnsignedInt>{0, 1, 4}));
    CORRADE_COMPARE(b.children3D(), (std::vector<UnsignedInt>{2, 5}));
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    SceneData d{{1, 3}, {1, 4, 5}, &c};
    d = std::move(b);

    CORRADE_COMPARE(d.children2D(), (std::vector<UnsignedInt>{0, 1, 4}));
    CORRADE_COMPARE(d.children3D(), (std::vector<UnsignedInt>{2, 5}));
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SceneData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SceneData>::value);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SceneDataTest)
