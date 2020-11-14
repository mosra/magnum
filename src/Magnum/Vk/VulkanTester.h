#ifndef Magnum_Vk_VulkanTester_h
#define Magnum_Vk_VulkanTester_h
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

/** @file
 * @brief Class @ref Magnum::Vk::VulkanTester
 * @m_since_latest
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_VK
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Queue.h"

namespace Magnum { namespace Vk {

/**
@brief Base class for Vulkan tests and benchmarks
@m_since_latest

Extends @ref Corrade::TestSuite::Tester with features for Vulkan testing and
benchmarking. Be sure to read its documentation first to have an overview of
the base features.

This class is built into a separate static library and only if
`WITH_VULKANTESTER` is enabled when building Magnum. To use it with CMake,
request the `VulkanTester` component of the `Magnum` package. Derive your test
class from this class instead of @ref Corrade::TestSuite::Tester and
either link to `Magnum::VulkanTester` target or add it to the `LIBRARIES`
section of the @ref corrade-cmake-add-test "corrade_add_test()" macro:

@code{.cmake}
find_package(Magnum REQUIRED VulkanTester)

# ...
corrade_add_test(YourTest YourTest.cpp LIBRARIES Magnum::VulkanTester)
@endcode

Additionally, if you're using Magnum as a CMake subproject, ensure it's enabled
as it's not built by default:

@code{.cmake}
set(WITH_OPENGLTESTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

See @ref building, @ref cmake and @ref testsuite for more information.

@section Vk-VulkanTester-device Vulkan device and instance creation

The class implicitly creates a Vulkan @ref Instance and @ref Device with
default layers and extensions and one graphics queue. These are then available
through @ref instance(), @ref device() and @ref queue() getters.

If you want to create a custom device, use the @ref VulkanTester(NoCreateT)
constructor. You can then move the device and queue instances to the getters to
make them available through common interfaces again. If you want to create a
custom instance as well, use the @ref VulkanTester(NoCreateT, NoCreateT)
variant.
*/
class VulkanTester: public TestSuite::Tester {
    public:
        /**
         * @brief Default constructor
         *
         * Creates an instance using implicit settings, picks a default device
         * and creates a graphics queue on that device. These are then exposed
         * through @ref instance(), @ref device() and @ref queue() getters.
         */
        explicit VulkanTester();

        /**
         * @brief Construct without creating a device
         *
         * Use the @ref instance() to pick and create a device. You can then
         * move it to @ref device() and @ref queue() to have them accessible
         * through common interfaces again.
         */
        explicit VulkanTester(NoCreateT);

        /**
         * @brief Construct without creating an instance or device
         *
         * Leaves the initialization completely on the user. You can move the
         * instances to @ref instance(), @ref device() and @ref queue() to have
         * them accessible through common interfaces.
         */
        explicit VulkanTester(NoCreateT, NoCreateT);

    protected:
        /**
         * @brief Vulkan instance
         *
         * In case the class was constructed using
         * @ref VulkanTester(NoCreateT, NoCreateT), this instance is initially
         * not created. Move a created instance onto it to make it useful.
         */
        Instance& instance() { return _instance; }

        /**
         * @brief Vulkan device
         *
         * In case the class was constructed using @ref VulkanTester(NoCreateT)
         * or @ref VulkanTester(NoCreateT, NoCreateT), this instance is
         * initially not created. Move a created instance onto it to make it
         * useful.
         */
        Device& device() { return _device; }

        /**
         * @brief Vulkan queue
         *
         * In case the calss was constructed using @ref VulkanTester(), the
         * queue corresponds to @ref DeviceProperties::pickQueueFamily() with
         * @ref QueueFlag::Graphics called on @ref Device::properties().
         *
         * In case the class was constructed using @ref VulkanTester(NoCreateT)
         * or @ref VulkanTester(NoCreateT, NoCreateT), this instance is
         * initially not created. Move a created instance onto it to make it
         * useful.
         */
        Queue& queue() { return _queue; }

    private:
        Instance _instance;
        Device _device;
        Queue _queue;
};

}}
#else
#error this header is available only in the Vulkan build
#endif

#endif
