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

#include <string>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Vk/Shader.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

#include "configure.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ShaderVkTest: VulkanTester {
    explicit ShaderVkTest();

    void construct();
    void constructMove();

    void wrap();
};

ShaderVkTest::ShaderVkTest() {
    addTests({&ShaderVkTest::construct,
              &ShaderVkTest::constructMove,

              &ShaderVkTest::wrap});
}

void ShaderVkTest::construct() {
    Containers::Array<char> data = Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"));
    CORRADE_VERIFY(data);

    {
        Shader shader{device(), ShaderCreateInfo{data}};
        CORRADE_VERIFY(shader.handle());
        CORRADE_COMPARE(shader.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ShaderVkTest::constructMove() {
    Containers::Array<char> data = Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"));
    CORRADE_VERIFY(data);

    Shader a{device(), ShaderCreateInfo{data}};
    VkShaderModule handle = a.handle();

    Shader b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    Shader c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Shader>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Shader>::value);
}

void ShaderVkTest::wrap() {
    Containers::Array<char> data = Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"));
    CORRADE_VERIFY(data);

    VkShaderModule shader{};
    CORRADE_COMPARE(Result(device()->CreateShaderModule(device(),
        ShaderCreateInfo{data},
        nullptr, &shader)), Result::Success);

    auto wrapped = Shader::wrap(device(), shader, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), shader);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), shader);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyShaderModule(device(), shader, nullptr);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ShaderVkTest)
