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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Vk/Integration.h"

namespace Magnum { namespace Vk { namespace Test {

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void vkOffset2D();
    void vkOffset3D();
    void vkExtent2D();
    void vkExtent3D();
    void vkExtent2DSigned();
    void vkExtent3DSigned();
    void vkClearColorValueFloat();
    void vkClearColorValueSigned();
    void vkClearColorValueUnsigned();
    void vkClearColorValue3();
    void vkViewport();
    void vkRect2D();
    void vkClearRect();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vkOffset2D,
              &IntegrationTest::vkOffset3D,
              &IntegrationTest::vkExtent2D,
              &IntegrationTest::vkExtent3D,
              &IntegrationTest::vkExtent2DSigned,
              &IntegrationTest::vkExtent3DSigned,
              &IntegrationTest::vkClearColorValueFloat,
              &IntegrationTest::vkClearColorValueSigned,
              &IntegrationTest::vkClearColorValueUnsigned,
              &IntegrationTest::vkClearColorValue3,
              &IntegrationTest::vkViewport,
              &IntegrationTest::vkRect2D,
              &IntegrationTest::vkClearRect});
}

void IntegrationTest::vkOffset2D() {
    Vector2i a{1, -2};
    VkOffset2D b(a);
    Vector2i c(b);
    CORRADE_COMPARE(b.x, 1);
    CORRADE_COMPARE(b.y, -2);
    CORRADE_COMPARE(c, (Vector2i{1, -2}));

    constexpr Vector2i ca{1, -2};
    constexpr VkOffset2D cb(ca);
    constexpr Vector2i cc(cb);
    CORRADE_COMPARE(cb.x, 1);
    CORRADE_COMPARE(cb.y, -2);
    CORRADE_COMPARE(cc, (Vector2i{1, -2}));
}

void IntegrationTest::vkOffset3D() {
    Vector3i a{-3, 2, 1};
    VkOffset3D b(a);
    Vector3i c(b);
    CORRADE_COMPARE(b.x, -3);
    CORRADE_COMPARE(b.y, 2);
    CORRADE_COMPARE(b.z, 1);
    CORRADE_COMPARE(c, (Vector3i{-3, 2, 1}));

    constexpr Vector3i ca{-3, 2, 1};
    constexpr VkOffset3D cb(ca);
    constexpr Vector3i cc(cb);
    CORRADE_COMPARE(cb.x, -3);
    CORRADE_COMPARE(cb.y, 2);
    CORRADE_COMPARE(cb.z, 1);
    CORRADE_COMPARE(cc, (Vector3i{-3, 2, 1}));
}

void IntegrationTest::vkExtent2D() {
    Vector2ui a{3526872522, 2};
    VkExtent2D b(a);
    Vector2ui c(b);
    CORRADE_COMPARE(b.width, 3526872522);
    CORRADE_COMPARE(b.height, 2);
    CORRADE_COMPARE(c, (Vector2ui{3526872522, 2}));

    constexpr Vector2ui ca{3526872522, 2};
    constexpr VkExtent2D cb(ca);
    constexpr Vector2ui cc(cb);
    CORRADE_COMPARE(cb.width, 3526872522);
    CORRADE_COMPARE(cb.height, 2);
    CORRADE_COMPARE(cc, (Vector2ui{3526872522, 2}));
}

void IntegrationTest::vkExtent3D() {
    Vector3ui a{3, 3526872522, 1};
    VkExtent3D b(a);
    Vector3ui c(b);
    CORRADE_COMPARE(b.width, 3);
    CORRADE_COMPARE(b.height, 3526872522);
    CORRADE_COMPARE(b.depth, 1);
    CORRADE_COMPARE(c, (Vector3ui{3, 3526872522, 1}));

    constexpr Vector3ui ca{3, 3526872522, 1};
    constexpr VkExtent3D cb(ca);
    constexpr Vector3ui cc(cb);
    CORRADE_COMPARE(cb.width, 3);
    CORRADE_COMPARE(cb.height, 3526872522);
    CORRADE_COMPARE(cb.depth, 1);
    CORRADE_COMPARE(cc, (Vector3ui{3, 3526872522, 1}));
}

void IntegrationTest::vkExtent2DSigned() {
    Vector2i a{1526872125, 2};
    VkExtent2D b(a);
    Vector2i c(b);
    CORRADE_COMPARE(b.width, 1526872125);
    CORRADE_COMPARE(b.height, 2);
    CORRADE_COMPARE(c, (Vector2i{1526872125, 2}));

    constexpr Vector2i ca{1526872125, 2};
    constexpr VkExtent2D cb(ca);
    constexpr Vector2i cc(cb);
    CORRADE_COMPARE(cb.width, 1526872125);
    CORRADE_COMPARE(cb.height, 2);
    CORRADE_COMPARE(cc, (Vector2i{1526872125, 2}));
}

void IntegrationTest::vkExtent3DSigned() {
    Vector3i a{3, 2, 1526872125};
    VkExtent3D b(a);
    Vector3i c(b);
    CORRADE_COMPARE(b.width, 3);
    CORRADE_COMPARE(b.height, 2);
    CORRADE_COMPARE(b.depth, 1526872125);
    CORRADE_COMPARE(c, (Vector3i{3, 2, 1526872125}));

    constexpr Vector3i ca{3, 2, 1526872125};
    constexpr VkExtent3D cb(ca);
    constexpr Vector3i cc(cb);
    CORRADE_COMPARE(cb.width, 3);
    CORRADE_COMPARE(cb.height, 2);
    CORRADE_COMPARE(cb.depth, 1526872125);
    CORRADE_COMPARE(cc, (Vector3i{3, 2, 1526872125}));
}

void IntegrationTest::vkClearColorValueFloat() {
    Color4 a{0.3f, 0.7f, 0.1f, 0.88f};
    VkClearColorValue b(a);
    Color4 c(b);
    CORRADE_COMPARE(b.float32[0], 0.3f);
    CORRADE_COMPARE(b.float32[1], 0.7f);
    CORRADE_COMPARE(b.float32[2], 0.1f);
    CORRADE_COMPARE(b.float32[3], 0.88f);
    CORRADE_COMPARE(c, (Color4{0.3f, 0.7f, 0.1f, 0.88f}));

    /** @todo test constexpr once/if possible */
}

void IntegrationTest::vkClearColorValueSigned() {
    Vector4i a{13, -42, 1337, 1526872125};
    VkClearColorValue b(a);
    Vector4i c(b);
    CORRADE_COMPARE(b.int32[0], 13);
    CORRADE_COMPARE(b.int32[1], -42);
    CORRADE_COMPARE(b.int32[2], 1337);
    CORRADE_COMPARE(b.int32[3], 1526872125);
    CORRADE_COMPARE(c, (Vector4i{13, -42, 1337, 1526872125}));

    /** @todo test constexpr once/if possible */
}

void IntegrationTest::vkClearColorValueUnsigned() {
    Vector4ui a{13, 42, 1337, 3526872522};
    VkClearColorValue b(a);
    Vector4ui c(b);
    CORRADE_COMPARE(b.uint32[0], 13);
    CORRADE_COMPARE(b.uint32[1], 42);
    CORRADE_COMPARE(b.uint32[2], 1337);
    CORRADE_COMPARE(b.uint32[3], 3526872522);
    CORRADE_COMPARE(c, (Vector4ui{13, 42, 1337, 3526872522}));

    /** @todo test constexpr once/if possible */
}

void IntegrationTest::vkClearColorValue3() {
    Color3 a{0.3f, 0.7f, 0.88f};
    VkClearColorValue b(a);
    CORRADE_COMPARE(b.float32[0], 0.3f);
    CORRADE_COMPARE(b.float32[1], 0.7f);
    CORRADE_COMPARE(b.float32[2], 0.88f);
    CORRADE_COMPARE(b.float32[3], 1.0f);

    /** @todo test constexpr once/if possible */

    /* Conversion the ohter way not allowed */
    CORRADE_VERIFY((std::is_constructible<Color4, VkClearColorValue>::value));
    CORRADE_VERIFY(!(std::is_constructible<Color3, VkClearColorValue>::value));
}

void IntegrationTest::vkViewport() {
    Range3D a = Range3D::fromSize({3.0f, 2.5f, -1.0f}, {2.7f, 0.3f, 1.1f});
    VkViewport b(a);
    Range3D c(b);
    CORRADE_COMPARE(b.x, 3.0f);
    CORRADE_COMPARE(b.y, 2.5f);
    CORRADE_COMPARE(b.minDepth, -1.0f);
    CORRADE_COMPARE(b.width, 2.7f);
    CORRADE_COMPARE(b.height, 0.3f);
    CORRADE_COMPARE(b.maxDepth, 0.1f);
    CORRADE_COMPARE(c, Range3D::fromSize({3.0f, 2.5f, -1.0f}, {2.7f, 0.3f, 1.1f}));

    constexpr Range3D ca{{3.0f, 2.5f, -1.0f}, {5.7f, 2.8f, 0.1f}};
    constexpr VkViewport cb(ca);
    constexpr Range3D cc(cb);
    CORRADE_COMPARE(cb.x, 3.0f);
    CORRADE_COMPARE(cb.y, 2.5f);
    CORRADE_COMPARE(cb.minDepth, -1.0f);
    CORRADE_COMPARE(cb.width, 2.7f);
    CORRADE_COMPARE(cb.height, 0.3f);
    CORRADE_COMPARE(cb.maxDepth, 0.1f);
    CORRADE_COMPARE(cc, Range3D::fromSize({3.0f, 2.5f, -1.0f}, {2.7f, 0.3f, 1.1f}));
}

void IntegrationTest::vkRect2D() {
    Range2Di a = Range2Di::fromSize({3, -2}, {23, 45});
    VkRect2D b(a);
    Range2Di c(b);
    CORRADE_COMPARE(b.offset.x, 3);
    CORRADE_COMPARE(b.offset.y, -2);
    CORRADE_COMPARE(b.extent.width, 23);
    CORRADE_COMPARE(b.extent.height, 45);
    CORRADE_COMPARE(c, Range2Di::fromSize({3, -2}, {23, 45}));

    constexpr Range2Di ca{{3, -2}, {26, 43}};
    constexpr VkRect2D cb(ca);
    constexpr Range2Di cc(cb);
    CORRADE_COMPARE(cb.offset.x, 3);
    CORRADE_COMPARE(cb.offset.y, -2);
    CORRADE_COMPARE(cb.extent.width, 23);
    CORRADE_COMPARE(cb.extent.height, 45);
    CORRADE_COMPARE(cc, Range2Di::fromSize({3, -2}, {23, 45}));
}

void IntegrationTest::vkClearRect() {
    Range3Di a = Range3Di::fromSize({3, -2, 1}, {23, 45, 7});
    VkClearRect b(a);
    Range3Di c(b);
    CORRADE_COMPARE(b.rect.offset.x, 3);
    CORRADE_COMPARE(b.rect.offset.y, -2);
    CORRADE_COMPARE(b.baseArrayLayer, 1);
    CORRADE_COMPARE(b.rect.extent.width, 23);
    CORRADE_COMPARE(b.rect.extent.height, 45);
    CORRADE_COMPARE(b.layerCount, 7);
    CORRADE_COMPARE(c, Range3Di::fromSize({3, -2, 1}, {23, 45, 7}));

    constexpr Range3Di ca{{3, -2, 1}, {26, 43, 8}};
    constexpr VkClearRect cb(ca);
    constexpr Range3Di cc(cb);
    CORRADE_COMPARE(cb.rect.offset.x, 3);
    CORRADE_COMPARE(cb.rect.offset.y, -2);
    CORRADE_COMPARE(cb.baseArrayLayer, 1);
    CORRADE_COMPARE(cb.rect.extent.width, 23);
    CORRADE_COMPARE(cb.rect.extent.height, 45);
    CORRADE_COMPARE(cb.layerCount, 7);
    CORRADE_COMPARE(cc, Range3Di::fromSize({3, -2, 1}, {23, 45, 7}));
}

}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::IntegrationTest)
