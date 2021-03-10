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
#include "Magnum/Vk/ShaderCreateInfo.h"
#include "Magnum/Vk/Implementation/spirvPatching.h"

#include "configure.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ShaderTest: TestSuite::Tester {
    explicit ShaderTest();

    void spirvPatchSwiftShaderConflictingMultiEntrypointLocations();
    void spirvPatchSwiftShaderConflictingMultiEntrypointLocationsTooManyEntrypoints();
    void spirvPatchSwiftShaderConflictingMultiEntrypointLocationsOnlyOneEntrypoint();
    void spirvPatchSwiftShaderConflictingMultiEntrypointLocationsNoInterfaces();

    void createInfoConstruct();
    void createInfoConstructTransferOwnership();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();
    void createInfoConstructCopy();
    void createInfoConstructMove();

    void constructNoCreate();
    void constructCopy();
};

ShaderTest::ShaderTest() {
    addTests({&ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocations,
              &ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocationsTooManyEntrypoints,
              &ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocationsOnlyOneEntrypoint,
              &ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocationsNoInterfaces,

              &ShaderTest::createInfoConstruct,
              &ShaderTest::createInfoConstructTransferOwnership,
              &ShaderTest::createInfoConstructNoInit,
              &ShaderTest::createInfoConstructFromVk,
              &ShaderTest::createInfoConstructCopy,
              &ShaderTest::createInfoConstructMove,

              &ShaderTest::constructNoCreate,
              &ShaderTest::constructCopy});
}

void ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocations() {
    Containers::Array<char> data = Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "ShaderTestFiles/vert-frag.spv"));

    /* The file is a full SPIR-V, strip the header first */
    const Containers::ArrayView<const UnsignedInt> spirv = ShaderTools::Implementation::spirvData(data, data.size());
    CORRADE_VERIFY(spirv);

    Containers::ArrayView<const UnsignedInt> view = spirv;
    Containers::Optional<ShaderTools::Implementation::SpirvEntrypoint> vert = ShaderTools::Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(vert);
    CORRADE_COMPARE(vert->name, "ver");
    CORRADE_COMPARE(vert->interfaces.size(), 7);

    Containers::Optional<ShaderTools::Implementation::SpirvEntrypoint> frag = ShaderTools::Implementation::spirvNextEntrypoint(view);
    CORRADE_VERIFY(frag);
    CORRADE_COMPARE(frag->name, "fra");
    CORRADE_COMPARE(frag->interfaces.size(), 5);

    ShaderTools::Implementation::SpirvEntrypointInterface vertInterfaces[7]{};
    ShaderTools::Implementation::spirvEntrypointInterface(view, *vert, vertInterfaces);
    CORRADE_VERIFY(vertInterfaces[0].location); /* position */
    CORRADE_COMPARE(*vertInterfaces[0].location, 0);
    CORRADE_VERIFY(vertInterfaces[1].location); /* color */
    CORRADE_COMPARE(*vertInterfaces[1].location, 1);
    CORRADE_VERIFY(vertInterfaces[3].location); /* interpolatedColorOut */
    CORRADE_COMPARE(*vertInterfaces[3].location, 0);
    CORRADE_VERIFY(vertInterfaces[4].location); /* interpolatedTexCoordsOut */
    CORRADE_COMPARE(*vertInterfaces[4].location, 1);
    CORRADE_VERIFY(vertInterfaces[5].location); /* interpolatedNormalOut */
    CORRADE_COMPARE(*vertInterfaces[5].location, 2);
    CORRADE_VERIFY(vertInterfaces[6].location); /* unused */
    CORRADE_COMPARE(*vertInterfaces[6].location, 3);

    ShaderTools::Implementation::SpirvEntrypointInterface fragInterfaces[5]{};
    ShaderTools::Implementation::spirvEntrypointInterface(view, *frag, fragInterfaces);
    CORRADE_VERIFY(fragInterfaces[0].location); /* interpolatedColorIn */
    CORRADE_COMPARE(*fragInterfaces[0].location, 0);
    CORRADE_VERIFY(fragInterfaces[1].location); /* interpolatedTexCoordsIn */
    CORRADE_COMPARE(*fragInterfaces[1].location, 1);
    CORRADE_VERIFY(fragInterfaces[2].location); /* interpolatedNormalIn */
    CORRADE_COMPARE(*fragInterfaces[2].location, 2);
    CORRADE_VERIFY(fragInterfaces[3].location); /* fragmentColor */
    CORRADE_COMPARE(*fragInterfaces[3].location, 0);
    CORRADE_VERIFY(fragInterfaces[4].location); /* weight */
    CORRADE_COMPARE(*fragInterfaces[4].location, 1);

    CORRADE_VERIFY(Implementation::spirvPatchSwiftShaderConflictingMultiEntrypointLocations(spirv));
    CORRADE_COMPARE(*vertInterfaces[0].location, 0);
    CORRADE_COMPARE(*vertInterfaces[1].location, 1);
    CORRADE_COMPARE(*vertInterfaces[3].location, 4); /* changed */
    CORRADE_COMPARE(*vertInterfaces[4].location, 5); /* changed */
    CORRADE_COMPARE(*vertInterfaces[5].location, 2); /* kept, no conflict */
    CORRADE_COMPARE(*vertInterfaces[6].location, 3); /* kept, no conflict */

    CORRADE_COMPARE(*fragInterfaces[0].location, 4); /* changed */
    CORRADE_COMPARE(*fragInterfaces[1].location, 5); /* changed */
    CORRADE_COMPARE(*fragInterfaces[2].location, 2); /* kept, no conflict */
    CORRADE_COMPARE(*fragInterfaces[3].location, 0);
    CORRADE_COMPARE(*fragInterfaces[4].location, 1);
}

UnsignedInt op(UnsignedInt length, SpvOp op) {
    return length << 16 | op;
}

void ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocationsTooManyEntrypoints() {
    UnsignedInt data[] {
        op(6, SpvOpEntryPoint), SpvExecutionModelVertex, 1, '\0', 4, 5,
        op(4, SpvOpEntryPoint), SpvExecutionModelFragment, 2, '\0',
        op(6, SpvOpEntryPoint), SpvExecutionModelFragment, 3, '\0', 7, 8,
    };

    /* There's three entrypoints, skip to avoid breaking something we don't
       understand */
    CORRADE_VERIFY(!Implementation::spirvPatchSwiftShaderConflictingMultiEntrypointLocations(data));
}

void ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocationsOnlyOneEntrypoint() {
    UnsignedInt data[] {
        op(6, SpvOpEntryPoint), SpvExecutionModelVertex, 1, '\0', 4, 5,
    };

    /* There's just one entrypoint, the bug doesn't affect this case */
    CORRADE_VERIFY(!Implementation::spirvPatchSwiftShaderConflictingMultiEntrypointLocations(data));
}

void ShaderTest::spirvPatchSwiftShaderConflictingMultiEntrypointLocationsNoInterfaces() {
    UnsignedInt data[] {
        op(4, SpvOpEntryPoint), SpvExecutionModelVertex, 1, '\0',
        op(4, SpvOpEntryPoint), SpvExecutionModelFragment, 2, '\0'
    };

    /* There's no interfaces and thus nothing to do, but the function should
       succeed and not crash */
    CORRADE_VERIFY(Implementation::spirvPatchSwiftShaderConflictingMultiEntrypointLocations(data));
}

void ShaderTest::createInfoConstruct() {
    const UnsignedInt data[] { 0xdead, 0xbee5, 0xbaba };

    ShaderCreateInfo info{data};
    CORRADE_COMPARE(info->flags, 0);
    CORRADE_COMPARE(info->pCode, data);
    CORRADE_COMPARE(info->codeSize, 3*4);
}

Int destructorCallCount = 0;
std::size_t destructedSize = 0;
void* destructedData = nullptr;

void ShaderTest::createInfoConstructTransferOwnership() {
    destructorCallCount = 0;
    destructedData = nullptr;
    destructedSize = 0;
    UnsignedInt data[] { 0xdead, 0xbee5, 0xbaba };

    {
        ShaderCreateInfo info{Containers::Array<UnsignedInt>{data, 3, [](UnsignedInt* data, std::size_t size) {
            /* This is called in a destructor and exceptions aren't allowed in
               a destructor, which means we can't use CORRADE_COMPARE() */
            destructedData = data;
            destructedSize = size;
            ++destructorCallCount;
        }}};

        CORRADE_COMPARE(info->pCode, data);
        CORRADE_COMPARE(info->codeSize, 3*4);
        CORRADE_COMPARE(destructorCallCount, 0);
    }

    CORRADE_COMPARE(destructorCallCount, 1);
    CORRADE_COMPARE(destructedData, static_cast<void*>(data));
    CORRADE_COMPARE(destructedSize, 3);
}

void ShaderTest::createInfoConstructNoInit() {
    {
        ShaderCreateInfo info{NoInit};
        info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        new(&info) ShaderCreateInfo{NoInit};
        CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
    }

    /* The deleter should be zero-init'd and thus no function called on
       destruction */

    CORRADE_VERIFY(std::is_nothrow_constructible<ShaderCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, ShaderCreateInfo>::value);
}

void ShaderTest::createInfoConstructFromVk() {
    VkShaderModuleCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    ShaderCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void ShaderTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<ShaderCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<ShaderCreateInfo>{});
}

void ShaderTest::createInfoConstructMove() {
    destructorCallCount = 0;
    destructedData = nullptr;
    destructedSize = 0;
    UnsignedInt data[] { 0xdead, 0xbee5, 0xbaba };

    {
        ShaderCreateInfo a{Containers::Array<UnsignedInt>{data, 3, [](UnsignedInt* data, std::size_t size) {
            /* This is called in a destructor and exceptions aren't allowed in
               a destructor, which means we can't use CORRADE_COMPARE() */
            destructedData = data;
            destructedSize = size;
            ++destructorCallCount;
        }}};
        CORRADE_COMPARE(destructorCallCount, 0);

        /* Besides the deleter, the original code pointer and size should also
           be cleared because it will inevitably become dangling */
        ShaderCreateInfo b = std::move(a);
        CORRADE_VERIFY(!a->pCode);
        CORRADE_COMPARE(a->codeSize, 0);
        CORRADE_COMPARE(b->pCode, data);
        CORRADE_COMPARE(b->codeSize, 3*4);
        CORRADE_COMPARE(destructorCallCount, 0);

        ShaderCreateInfo c{NoInit};
        c->pCode = &data[1];
        c->codeSize = 2;
        c = std::move(b);
        /* It just swaps, so the moved-from instance doesn't have the code
           pointer cleared */
        CORRADE_COMPARE(b->pCode, &data[1]);
        CORRADE_COMPARE(b->codeSize, 2);
        CORRADE_COMPARE(c->pCode, data);
        CORRADE_COMPARE(c->codeSize, 3*4);
        CORRADE_COMPARE(destructorCallCount, 0);
    }

    CORRADE_COMPARE(destructorCallCount, 1);
    CORRADE_COMPARE(destructedData, static_cast<void*>(data));
    CORRADE_COMPARE(destructedSize, 3);
}

void ShaderTest::constructNoCreate() {
    {
        Shader shader{NoCreate};
        CORRADE_VERIFY(!shader.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, Shader>::value);
}

void ShaderTest::constructCopy() {
    CORRADE_VERIFY(!std::is_constructible<Shader, const Shader&>{});
    CORRADE_VERIFY(!std::is_assignable<Shader, const Shader&>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ShaderTest)
