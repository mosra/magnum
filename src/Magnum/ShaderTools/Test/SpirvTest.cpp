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

#include <string>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/ShaderTools/Implementation/spirv.h"
#include "MagnumExternal/Vulkan/spirv.h"

#include "configure.h"

namespace Magnum { namespace ShaderTools { namespace Test { namespace {

struct SpirvTest: TestSuite::Tester {
    explicit SpirvTest();

    void data();
    void dataInvalid();

    void findInstruction();
    void findInstructionNotEnoughData();

    void nextEntrypoint();
    void nextEntrypointInvalidInstruction();

    void entrypointInterface();
    void entrypointInterfaceNothing();
};

const UnsignedInt Data[] {
    SpvMagicNumber, SpvVersion, 0, 66, 0,
    0 /* first instruction */
};

const UnsignedInt JustHeader[]{
    SpvMagicNumber, SpvVersion, 0, 66, 0
};

const UnsignedInt InvalidMagic[]{
    SpvMagicNumber + 1, SpvVersion, 0, 66, 0,
    0 /* first instruction */
};

const struct {
    const char* name;
    Containers::ArrayView<const void> data;
} InvalidData[] {
    {"empty", {}},
    /* GCC 4.8 needs the ArrayView conversion explicit */
    {"just the header", Containers::arrayView(JustHeader)},
    {"invalid magic", Containers::arrayView(InvalidMagic)},
    {"size not divisible by four", Containers::arrayCast<const char>(Data).except(1)}
};

SpirvTest::SpirvTest() {
    addTests({&SpirvTest::data});

    addInstancedTests({&SpirvTest::dataInvalid},
        Containers::arraySize(InvalidData));

    addTests({&SpirvTest::findInstruction,
              &SpirvTest::findInstructionNotEnoughData,

              &SpirvTest::nextEntrypoint,
              &SpirvTest::nextEntrypointInvalidInstruction,

              &SpirvTest::entrypointInterface,
              &SpirvTest::entrypointInterfaceNothing});
}

void SpirvTest::data() {
    CORRADE_COMPARE(Implementation::spirvData(Data, sizeof(Data)), Containers::arrayView(Data + 5, 1));
}

void SpirvTest::dataInvalid() {
    auto&& data = InvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    CORRADE_VERIFY(!Implementation::spirvData(data.data, data.data.size()));
}

UnsignedInt op(UnsignedInt length, SpvOp op) {
    return length << 16 | op;
}

void SpirvTest::findInstruction() {
    const UnsignedInt data[] {
        op(3, SpvOpMemoryModel), SpvAddressingModelLogical, SpvMemoryModelGLSL450,
        op(4, SpvOpDecorate), 12, SpvDecorationLocation, 0,
        op(1, SpvOpNop),
        op(1, SpvOpNop),
        op(4, SpvOpDecorate), 13, SpvDecorationLocation, 1,
    };
    Containers::ArrayView<const UnsignedInt> view = data;

    Containers::ArrayView<const UnsignedInt> decorate1 = Implementation::spirvFindInstruction(view, SpvOpDecorate);
    CORRADE_COMPARE(decorate1.size(), 4);
    CORRADE_COMPARE(decorate1.data(), data + 3);
    CORRADE_COMPARE(view.data(), data + 7);

    /* Verify a single-word instruction works too */
    Containers::ArrayView<const UnsignedInt> nop = Implementation::spirvFindInstruction(view, SpvOpNop);
    CORRADE_COMPARE(nop.size(), 1);
    CORRADE_COMPARE(nop.data(), data + 7);
    CORRADE_COMPARE(view.data(), data + 8);

    Containers::ArrayView<const UnsignedInt> decorate2 = Implementation::spirvFindInstruction(view, SpvOpDecorate);
    CORRADE_COMPARE(decorate2.size(), 4);
    CORRADE_COMPARE(decorate2.data(), data + 9);
    CORRADE_COMPARE(view.data(), data + 13);

    /* We're at the end, there's no more OpDecorate instructions to find */
    CORRADE_VERIFY(!Implementation::spirvFindInstruction(view, SpvOpDecorate));
}

void SpirvTest::findInstructionNotEnoughData() {
    const UnsignedInt data[] {
        op(3, SpvOpMemoryModel), SpvAddressingModelLogical, SpvMemoryModelGLSL450,
        /* Should be 4 */
        op(5, SpvOpDecorate), 12, SpvDecorationLocation, 0
    };
    Containers::ArrayView<const UnsignedInt> view = data;

    CORRADE_VERIFY(!Implementation::spirvFindInstruction(view, SpvOpDecorate));
    /* View gets set to the first invalid instruction */
    CORRADE_COMPARE(view.data(), data + 3);
}

void SpirvTest::nextEntrypoint() {
    Containers::Array<char> data = Utility::Directory::read(Utility::Directory::join(SHADERTOOLS_TEST_DIR, "SpirvTestFiles/entrypoint-interface.spv"));

    /* The file is a full SPIR-V, strip the header first */
    Containers::ArrayView<const UnsignedInt> view = Implementation::spirvData(data, data.size());
    CORRADE_VERIFY(view);

    Containers::Optional<Implementation::SpirvEntrypoint> vert = Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(vert);
    /* Verify that long names get recognized properly */
    CORRADE_COMPARE(vert->name, "vertexLongEntrypointName");
    CORRADE_COMPARE(vert->executionModel, SpvExecutionModelVertex);
    /* We don't care about the contents, those would change with each assembly
       anyway. Verified fully in entrypointInterface(). */
    CORRADE_COMPARE(vert->interfaces.size(), 4);

    Containers::Optional<Implementation::SpirvEntrypoint> frag = Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(frag);
    CORRADE_COMPARE(frag->name, "fra");
    CORRADE_COMPARE(frag->executionModel, SpvExecutionModelFragment);
    CORRADE_COMPARE(frag->interfaces.size(), 3);

    /* Only two entrypoints in this file */
    CORRADE_VERIFY(!Implementation::spirvNextEntrypoint(view));
}

void SpirvTest::nextEntrypointInvalidInstruction() {
    const UnsignedInt data[] {
        op(3, SpvOpMemoryModel), SpvAddressingModelLogical, SpvMemoryModelGLSL450,

        /* Should be 4 (missing name) */
        op(3, SpvOpEntryPoint), SpvExecutionModelVertex, 1
    };
    Containers::ArrayView<const UnsignedInt> view = data;

    CORRADE_VERIFY(!Implementation::spirvNextEntrypoint(view));
}

void SpirvTest::entrypointInterface() {
    Containers::Array<char> data = Utility::Directory::read(Utility::Directory::join(SHADERTOOLS_TEST_DIR, "SpirvTestFiles/entrypoint-interface.spv"));

    /* The file is a full SPIR-V, strip the header first */
    Containers::ArrayView<const UnsignedInt> view = Implementation::spirvData(data, data.size());
    CORRADE_VERIFY(view);

    Containers::Optional<Implementation::SpirvEntrypoint> vert = Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(vert);
    CORRADE_COMPARE(vert->interfaces.size(), 4);

    Implementation::SpirvEntrypointInterface vertInterface[4]{};
    Implementation::spirvEntrypointInterface(view, *vert, vertInterface);
    CORRADE_VERIFY(vertInterface[0].location); /* position */
    CORRADE_VERIFY(vertInterface[0].storageClass);
    CORRADE_COMPARE(*vertInterface[0].location, 0);
    CORRADE_COMPARE(*vertInterface[0].storageClass, SpvStorageClassInput);

    CORRADE_VERIFY(vertInterface[1].location); /* color */
    CORRADE_VERIFY(vertInterface[1].storageClass);
    CORRADE_COMPARE(*vertInterface[1].location, 1);
    CORRADE_COMPARE(*vertInterface[1].storageClass, SpvStorageClassInput);

    /* Verify that absence of location is handled properly */
    CORRADE_VERIFY(!vertInterface[2].location); /* gl_Position */
    CORRADE_VERIFY(vertInterface[2].storageClass);
    CORRADE_COMPARE(*vertInterface[2].storageClass, SpvStorageClassOutput);

    CORRADE_VERIFY(vertInterface[3].location); /* interpolatedColorOut */
    CORRADE_VERIFY(vertInterface[2].storageClass);
    CORRADE_COMPARE(*vertInterface[3].location, 0);
    CORRADE_COMPARE(*vertInterface[2].storageClass, SpvStorageClassOutput);

    Containers::Optional<Implementation::SpirvEntrypoint> frag = Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(frag);
    CORRADE_COMPARE(frag->interfaces.size(), 3);

    Implementation::SpirvEntrypointInterface fragInterface[3]{};
    Implementation::spirvEntrypointInterface(view, *frag, fragInterface);
    CORRADE_VERIFY(fragInterface[0].location); /* interpolatedColorIn */
    CORRADE_VERIFY(fragInterface[0].storageClass);
    CORRADE_COMPARE(*fragInterface[0].location, 0);
    CORRADE_COMPARE(*fragInterface[0].storageClass, SpvStorageClassInput);

    CORRADE_VERIFY(fragInterface[1].location); /* fragmentColor */
    CORRADE_VERIFY(fragInterface[1].storageClass);
    CORRADE_COMPARE(*fragInterface[1].location, 0);
    CORRADE_COMPARE(*fragInterface[1].storageClass, SpvStorageClassOutput);

    /* Verify that absence of storageClass is handled properly */
    CORRADE_VERIFY(fragInterface[2].location); /* unknownFragmentInterface */
    CORRADE_VERIFY(!fragInterface[2].storageClass);
    CORRADE_COMPARE(*fragInterface[2].location, 1);
}

void SpirvTest::entrypointInterfaceNothing() {
    const UnsignedInt data[] {
        op(3, SpvOpMemoryModel), SpvAddressingModelLogical, SpvMemoryModelGLSL450,

        op(4, SpvOpEntryPoint), SpvExecutionModelGLCompute, 1, '\0'
    };
    Containers::ArrayView<const UnsignedInt> view = data;

    Containers::Optional<Implementation::SpirvEntrypoint> comp = Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(comp);
    CORRADE_VERIFY(comp->interfaces.empty());

    Implementation::spirvEntrypointInterface(view, *comp, {});

    /* Well, it shouldn't crash */
    CORRADE_VERIFY(true);
}

}}}}

CORRADE_TEST_MAIN(Magnum::ShaderTools::Test::SpirvTest)
