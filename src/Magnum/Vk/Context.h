#ifndef Magnum_Vk_Context_h
#define Magnum_Vk_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Vk::Context
 */

#include "vulkan.h"

#include <cstdlib>
#include <array>
#include <bitset>
#include <vector>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Tags.h"
#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"
#include "MagnumExternal/Optional/optional.hpp"

namespace Magnum { namespace Vk {

enum class Version: UnsignedInt {
    None = 0, /**< No version */
    Vulkan_1_0 = VK_API_VERSION, /**< Vulkan 1.0 */
};

/**
@brief Magnum context

Provides access to version and extension information. Instance available
through @ref Context::current() is automatically created during construction of
`*Application` classes in @ref Platform namespace. You can safely assume that
the instance is available during whole lifetime of `*Application` object. It's
also possible to create the context without using any `*Application` class
using @ref Platform::Context subclass, see @ref platform documentation for more
information.

## Command-line options

The context is configurable through command-line options, that are passed
either from the `Platform::*Application` classes or from the @ref Platform::Context
class. Usage:

    <application> [--magnum-help] ...

Arguments:

-   `...` -- main application arguments (see `-h` or `--help` for details)
-   `--magnum-help` -- display this help message and exit
*/
class MAGNUM_VK_EXPORT Context {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref flags(),
         *      @ref Platform::Sdl2Application::Configuration::setFlags() "Platform::*Application::Configuration::setFlags()"
         */
        enum class Flag: Int {
            EnableValidation = 1, /**< Enable validation layer */
        };

        /**
         * @brief Context flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Whether there is any current context
         *
         * @see @ref current()
         */
        static bool hasCurrent();

        /**
         * @brief Current context
         *
         * Expect that there is current context.
         * @see @ref hasCurrent()
         */
        static Context& current();

        Context(Flags flags);

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Move constructor */
        Context(Context&& other);

        ~Context();

        /** @brief Copying is not allowed */
        Context& operator=(const Context&) = delete;

        /** @brief Move assignment is not allowed */
        Context& operator=(Context&&) = delete;

        /** @brief Vulkan version */
        Version version() const { return _version; }

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Whether given Vulkan version is supported
         *
         * @see @ref MAGNUM_ASSERT_VERSION_SUPPORTED()
         */
        bool isVersionSupported(Version version) const;

        VkInstance vkInstance() {
            return _instance;
        }

    private:
        MAGNUM_VK_LOCAL static Context* _current;

        explicit Context(NoCreateT, Int argc, char** argv, void functionLoader());

        bool tryCreate();
        void create();

        void(*_functionLoader)();
        Version _version;
        Flags _flags;

        VkInstance _instance;
};

}}

#endif
