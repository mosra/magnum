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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Shaders/Flat.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct FlatTest: TestSuite::Tester {
    explicit FlatTest();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();
};

FlatTest::FlatTest() {
    addTests({&FlatTest::constructNoCreate<2>,
              &FlatTest::constructNoCreate<3>,

              &FlatTest::constructCopy<2>,
              &FlatTest::constructCopy<3>,

              &FlatTest::debugFlag,
              &FlatTest::debugFlags,
              &FlatTest::debugFlagsSupersets});
}

template<UnsignedInt dimensions> void FlatTest::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        Flat<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void FlatTest::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY((std::is_constructible<Flat<dimensions>, Flat<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_constructible<Flat<dimensions>, const Flat<dimensions>&>{}));

    CORRADE_VERIFY((std::is_assignable<Flat<dimensions>, Flat<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Flat<dimensions>, const Flat<dimensions>&>{}));
}

void FlatTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << Flat3D::Flag::Textured << Flat3D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::Flat::Flag::Textured Shaders::Flat::Flag(0xf0)\n");
}

void FlatTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (Flat3D::Flag::Textured|Flat3D::Flag::AlphaMask) << Flat3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::Flat::Flag::Textured|Shaders::Flat::Flag::AlphaMask Shaders::Flat::Flags{}\n");
}

void FlatTest::debugFlagsSupersets() {
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId is a superset of ObjectId so only one should be
       printed */
    {
        std::ostringstream out;
        Debug{&out} << (Flat3D::Flag::ObjectId|Flat3D::Flag::InstancedObjectId);
        CORRADE_COMPARE(out.str(), "Shaders::Flat::Flag::InstancedObjectId\n");
    }
    #endif

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    std::ostringstream out;
    Debug{&out} << (Flat3D::Flag::InstancedTextureOffset|Flat3D::Flag::TextureTransformation);
    CORRADE_COMPARE(out.str(), "Shaders::Flat::Flag::InstancedTextureOffset\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatTest)
