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

#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Shader.h"
#include "Magnum/Vk/ShaderSet.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ShaderSetTest: TestSuite::Tester {
    explicit ShaderSetTest();

    void specializationConstructInt();
    void specializationConstructFloat();
    void specializationConstructBool();

    void construct();
    void constructCopy();
    void constructMove();

    void addShader();
    void addShaderEntrypointCopy();
    void addShaderEntrypointCopyReallocation();
    void addShaderSpecializations();
    void addShaderSpecializationsReallocation();
    void addShaderOwnershipTransfer();
    void addShaderTooManyStages();
};

ShaderSetTest::ShaderSetTest() {
    addTests({&ShaderSetTest::specializationConstructInt,
              &ShaderSetTest::specializationConstructFloat,
              &ShaderSetTest::specializationConstructBool,

              &ShaderSetTest::construct,
              &ShaderSetTest::constructCopy,
              &ShaderSetTest::constructMove,

              &ShaderSetTest::addShader,
              &ShaderSetTest::addShaderEntrypointCopy,
              &ShaderSetTest::addShaderEntrypointCopyReallocation,
              &ShaderSetTest::addShaderSpecializations,
              &ShaderSetTest::addShaderSpecializationsReallocation,
              &ShaderSetTest::addShaderOwnershipTransfer,
              &ShaderSetTest::addShaderTooManyStages});
}

using namespace Containers::Literals;

void ShaderSetTest::specializationConstructInt() {
    ShaderSpecialization spec{42, 133785};
    CORRADE_COMPARE(spec.id(), 42);
    CORRADE_COMPARE(spec.data(), 133785);
}

void ShaderSetTest::specializationConstructFloat() {
    ShaderSpecialization spec{42, 4.32f};
    CORRADE_COMPARE(spec.id(), 42);
    UnsignedInt data = spec.data();
    CORRADE_COMPARE(reinterpret_cast<Float&>(data), 4.32f);
}

void ShaderSetTest::specializationConstructBool() {
    ShaderSpecialization spec{42, true};
    CORRADE_COMPARE(spec.id(), 42);
    CORRADE_COMPARE(spec.data(), 1);
}

void ShaderSetTest::construct() {
    ShaderSet set;
    CORRADE_VERIFY(set.stages().empty());

    /* The actually meaningful test done in addShader() and friends */
}

void ShaderSetTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<ShaderSet>{});
    CORRADE_VERIFY(!std::is_copy_assignable<ShaderSet>{});
}

void ShaderSetTest::constructMove() {
    ShaderSet c;

    {
        ShaderSet a;
        a.addShader(ShaderStage::Geometry, reinterpret_cast<VkShaderModule>(0xdeadbeef), "main!"_s.except(1), {
            {42, 1.15f}
        });
        CORRADE_COMPARE(a.stages().size(), 1);
        CORRADE_COMPARE(a.stages()[0].pName, "main"_s);
        CORRADE_VERIFY(a.stages()[0].pSpecializationInfo);
        CORRADE_COMPARE(a.stages()[0].pSpecializationInfo->mapEntryCount, 1);
        CORRADE_VERIFY(a.stages()[0].pSpecializationInfo->pMapEntries);
        CORRADE_COMPARE(a.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 42);
        CORRADE_VERIFY(a.stages()[0].pSpecializationInfo->pData);
        CORRADE_COMPARE(*reinterpret_cast<const Float*>(a.stages()[0].pSpecializationInfo->pData), 1.15f);

        ShaderSet b = std::move(a);
        CORRADE_VERIFY(a.stages().empty());
        CORRADE_COMPARE(b.stages().size(), 1);
        CORRADE_COMPARE(b.stages()[0].pName, "main"_s);
        CORRADE_VERIFY(b.stages()[0].pSpecializationInfo);
        CORRADE_COMPARE(b.stages()[0].pSpecializationInfo->mapEntryCount, 1);
        CORRADE_VERIFY(b.stages()[0].pSpecializationInfo->pMapEntries);
        CORRADE_COMPARE(b.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 42);
        CORRADE_VERIFY(b.stages()[0].pSpecializationInfo->pData);
        CORRADE_COMPARE(*reinterpret_cast<const Float*>(b.stages()[0].pSpecializationInfo->pData), 1.15f);

        c = std::move(b);
        CORRADE_VERIFY(b.stages().empty());
    }

    /* Doing this in outer scope to verify that the internal state pointer got
       properly transferred as well and we're not referencing destroyed data */
    CORRADE_COMPARE(c.stages().size(), 1);
    CORRADE_COMPARE(c.stages()[0].pName, "main"_s);
    CORRADE_VERIFY(c.stages()[0].pSpecializationInfo);
    CORRADE_COMPARE(c.stages()[0].pSpecializationInfo->mapEntryCount, 1);
    CORRADE_VERIFY(c.stages()[0].pSpecializationInfo->pMapEntries);
    CORRADE_COMPARE(c.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 42);
    CORRADE_VERIFY(c.stages()[0].pSpecializationInfo->pData);
    CORRADE_COMPARE(*reinterpret_cast<const Float*>(c.stages()[0].pSpecializationInfo->pData), 1.15f);
}

void ShaderSetTest::addShader() {
    ShaderSet set;
    Containers::StringView entrypoint = "enterHere"_s;
    set.addShader(ShaderStage::Geometry, reinterpret_cast<VkShaderModule>(0xdeadbeef), entrypoint);
    CORRADE_COMPARE(set.stages().size(), 1);
    CORRADE_COMPARE(set.stages()[0].stage, VK_SHADER_STAGE_GEOMETRY_BIT);
    CORRADE_COMPARE(set.stages()[0].module, reinterpret_cast<VkShaderModule>(0xdeadbeef));
    /* The name should not be copied if it's null-terminated and global */
    CORRADE_COMPARE(set.stages()[0].pName, entrypoint.data());
    CORRADE_VERIFY(!set.stages()[0].pSpecializationInfo);
}

void ShaderSetTest::addShaderEntrypointCopy() {
    ShaderSet set;
    Containers::StringView entrypoint = "enterHere!"_s;
    set.addShader(ShaderStage{}, {}, entrypoint.except(1));
    CORRADE_COMPARE(set.stages().size(), 1);
    CORRADE_VERIFY(set.stages()[0].pName != entrypoint.data());
    CORRADE_COMPARE(set.stages()[0].pName, "enterHere"_s);
}

void ShaderSetTest::addShaderEntrypointCopyReallocation() {
    ShaderSet set;
    Containers::StringView entrypoint = "enterHere!"_s;
    set.addShader(ShaderStage{}, {}, entrypoint.except(1));
    CORRADE_COMPARE(set.stages().size(), 1);
    CORRADE_VERIFY(set.stages()[0].pName != entrypoint.data());
    CORRADE_COMPARE(set.stages()[0].pName, "enterHere"_s);

    /* After adding more stages, the original name pointers should be preserved
       -- no SSO strings getting reallocated but instead all copies allocated */
    const char* prev = set.stages()[0].pName;
    set.addShader(ShaderStage{}, {}, "huajajajaja"_s.prefix(5))
       .addShader(ShaderStage{}, {}, "ablablablab"_s.prefix(5));
    CORRADE_COMPARE(set.stages().size(), 3);
    CORRADE_COMPARE(set.stages()[0].pName, prev);
    CORRADE_COMPARE(set.stages()[0].pName, "enterHere"_s);
    CORRADE_COMPARE(set.stages()[1].pName, "huaja"_s);
    CORRADE_COMPARE(set.stages()[2].pName, "ablab"_s);
}

void ShaderSetTest::addShaderSpecializations() {
    ShaderSet set;
    set.addShader(ShaderStage{}, {}, "main"_s, {
        {42, 1.15f},
        {1, true},
        {13, -227}
    });
    CORRADE_COMPARE(set.stages().size(), 1);
    CORRADE_COMPARE(set.stages()[0].pName, "main"_s);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->mapEntryCount, 3);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pMapEntries);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 42);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[0].offset, 0);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[0].size, 4);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[1].constantID, 1);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[1].offset, 4);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[1].size, 4);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[2].constantID, 13);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[2].offset, 8);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[2].size, 4);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->dataSize, 4*3);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pData);
    CORRADE_COMPARE(*reinterpret_cast<const Float*>(set.stages()[0].pSpecializationInfo->pData), 1.15f);
    CORRADE_COMPARE(*(reinterpret_cast<const UnsignedInt*>(set.stages()[0].pSpecializationInfo->pData) + 1), 1);
    CORRADE_COMPARE(*(reinterpret_cast<const Int*>(set.stages()[0].pSpecializationInfo->pData) + 2), -227);
}

void ShaderSetTest::addShaderSpecializationsReallocation() {
    ShaderSet set;
    set.addShader(ShaderStage{}, {}, "main"_s, {
        {42, 1.15f}
    });
    CORRADE_COMPARE(set.stages().size(), 1);
    CORRADE_COMPARE(set.stages()[0].pName, "main"_s);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->mapEntryCount, 1);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pMapEntries);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 42);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pData);
    CORRADE_COMPARE(*reinterpret_cast<const Float*>(set.stages()[0].pSpecializationInfo->pData), 1.15f);

    const void* prevData = set.stages()[0].pSpecializationInfo->pData;
    const VkSpecializationMapEntry* prev = set.stages()[0].pSpecializationInfo->pMapEntries;

    set.addShader(ShaderStage{}, {}, "well"_s, {
        {1, true},
        {13, -227}
    });
    CORRADE_COMPARE(set.stages().size(), 2);

    /* Don't fail in this case -- the allocator is expected to be smarter than
       this test */
    if(set.stages()[0].pSpecializationInfo->pData == prevData)
        Warning{} << "No data reallocation happened.";
    if(set.stages()[0].pSpecializationInfo->pMapEntries == prev)
        Warning{} << "No entry map reallocation happened.";

    /* Same as above, everything should be kept */
    CORRADE_COMPARE(set.stages()[0].pName, "main"_s);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->mapEntryCount, 1);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pMapEntries);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 42);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pData);
    CORRADE_COMPARE(*reinterpret_cast<const Float*>(set.stages()[0].pSpecializationInfo->pData), 1.15f);

    /* New entries */
    CORRADE_COMPARE(set.stages()[1].pName, "well"_s);
    CORRADE_VERIFY(set.stages()[1].pSpecializationInfo);
    CORRADE_COMPARE(set.stages()[1].pSpecializationInfo->mapEntryCount, 2);
    CORRADE_VERIFY(set.stages()[1].pSpecializationInfo->pMapEntries);
    CORRADE_COMPARE(set.stages()[1].pSpecializationInfo->pMapEntries[0].constantID, 1);
    CORRADE_COMPARE(set.stages()[1].pSpecializationInfo->pMapEntries[1].constantID, 13);
    CORRADE_VERIFY(set.stages()[1].pSpecializationInfo->pData);
    CORRADE_COMPARE(*reinterpret_cast<const UnsignedInt*>(set.stages()[1].pSpecializationInfo->pData), 1);
    CORRADE_COMPARE(*(reinterpret_cast<const Int*>(set.stages()[1].pSpecializationInfo->pData) + 1), -227);
}

void ShaderSetTest::addShaderOwnershipTransfer() {
    Device device{NoCreate};
    auto shader = Shader::wrap(device, reinterpret_cast<VkShaderModule>(0xdeadbeef));

    ShaderSet set;
    set.addShader(ShaderStage::RayAnyHit, std::move(shader), "main"_s, {
        {13, 1227}
    });

    CORRADE_COMPARE(set.stages()[0].stage, VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
    CORRADE_COMPARE(set.stages()[0].pName, "main"_s);
    CORRADE_COMPARE(set.stages()[0].module, reinterpret_cast<VkShaderModule>(0xdeadbeef));
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->mapEntryCount, 1);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pMapEntries);
    CORRADE_COMPARE(set.stages()[0].pSpecializationInfo->pMapEntries[0].constantID, 13);
    CORRADE_VERIFY(set.stages()[0].pSpecializationInfo->pData);
    CORRADE_COMPARE(*reinterpret_cast<const UnsignedInt*>(set.stages()[0].pSpecializationInfo->pData), 1227);

    /* The shader should be moved away */
    CORRADE_VERIFY(!shader.handle());
}

void ShaderSetTest::addShaderTooManyStages() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    ShaderSet set;

    set.addShader({}, {}, {})
       .addShader({}, {}, {})
       .addShader({}, {}, {})
       .addShader({}, {}, {})
       .addShader({}, {}, {})
       .addShader({}, {}, {});

    std::ostringstream out;
    Error redirectError{&out};
    set.addShader({}, {}, {});
    CORRADE_COMPARE(out.str(), "Vk::ShaderSet::addShader(): too many stages, expected at most 6\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ShaderSetTest)
