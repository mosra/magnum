#ifndef Magnum_Audio_Extensions_h
#define Magnum_Audio_Extensions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Namespace @ref Magnum::Audio::Extensions
 */

#include "MagnumExternal/OpenAL/extensions.h"

namespace Magnum { namespace Audio {

/**
@brief Compile-time information about OpenAL extensions

Each extension is a @cpp struct @ce named hierarchically by prefix, vendor and
extension name taken from list at @ref openal-support, for example
@cpp Audio::Extensions::ALC::SOFTX::HRTF @ce.

Each struct has the same public methods as @ref Extension class (currently just
@ref Extension::string() "string()"), but these structs are better suited for
compile-time decisions rather than @ref Extension instances. See
@ref Context::isExtensionSupported() for example usage.

This library depends on the [OpenAL](https://www.openal.org/) library and is
built if `WITH_AUDIO` is enabled when building Magnum. To use this library with
CMake, put [FindOpenAL.cmake](https://github.com/mosra/magnum/blob/master/modules/FindOpenAL.cmake)
into your `modules/` directory, request the `Audio` component of the `Magnum`
package and link to the `Magnum::Audio` target:

@code{.cmake}
find_package(Magnum REQUIRED Audio)

# ...
target_link_libraries(your-app PRIVATE Magnum::Audio)
@endcode

Additionally, if you're using Magnum as a CMake subproject, bundle the
[openal-soft repository](https://github.com/kcat/openal-soft) and do the
following *before* calling @cmake find_package() @ce to ensure it's enabled, as
the library is not built by default. If you want to use system-installed
OpenAL, omit the first part and point `CMAKE_PREFIX_PATH` to its installation
dir if necessary.

@code{.cmake}
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed if building dynamic libraries
set(LIBTYPE STATIC) # omit or set to SHARED if you want a shared OpenAL library
add_subdirectory(openal-soft EXCLUDE_FROM_ALL)

set(WITH_AUDIO ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

See @ref building and @ref cmake for more information.
@see @ref MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED()
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(index, prefix, vendor, extension)                        \
    struct extension {                                                      \
        enum: std::size_t { Index = index };                                \
        constexpr static const char* string() { return #prefix "_" #vendor "_" #extension; } \
    };

#define _extension_rev(index, prefix, vendor, extension)                    \
    struct extension {                                                      \
        enum: std::size_t { Index = index };                                \
        constexpr static const char* string() { return #prefix "_" #extension "_" #vendor; } \
    };

/* IMPORTANT: don't forget to add new extensions also in Context.cpp */
namespace AL {
    namespace EXT {
        _extension(1,AL,EXT,FLOAT32) // #???
        _extension(2,AL,EXT,DOUBLE) // #???
        _extension(3,AL,EXT,ALAW) // #???
        _extension(4,AL,EXT,MULAW) // #???
        _extension(5,AL,EXT,MCFORMATS) // #???
    }
    namespace SOFT {
        _extension(6,AL,SOFT,loop_points) // #???
    }
} namespace ALC {
    namespace EXT {
        _extension_rev(7,ALC,EXT,ENUMERATION) // #???
    }
    namespace SOFTX {
        _extension(8,ALC,SOFTX,HRTF) // #???
    }
    namespace SOFT {
        _extension(9,ALC,SOFT,HRTF) // #???
    }
}
#undef _extension
#undef _extension_rev
#endif

}

}}

#endif
