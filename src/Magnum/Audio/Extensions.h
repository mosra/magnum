#ifndef Magnum_Audio_Extensions_h
#define Magnum_Audio_Extensions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>
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

Each extension is `struct` named hierarchically by prefix, vendor and
extension name taken from list at @ref openal-support, for example
`ALC::SOFTX::HRTF`.

Each struct has the same public methods as @ref Extension class
(@ref Extension::requiredVersion() "requiredVersion()",
@ref Extension::coreVersion() "coreVersion()" and @ref Extension::string() "string()"),
but these structs are better suited for compile-time decisions rather than
@ref Extension instances. See @ref Context::isExtensionSupported() for example
usage.

This namespace is not built by default. It is built if `WITH_AUDIO` is
enabled when building Magnum. To use this library, you need to request
`Audio` component of `Magnum` package in CMake and link to `Magnum::Audio`
target. See @ref building and @ref cmake for more information. Additional
plugins are enabled separately, see particular `*Importer` class documentation,
@ref building-plugins, @ref cmake-plugins and @ref plugins for more
information.
@see @ref MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED()
@todo Manual indices for extensions, this has gaps
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(prefix, vendor, extension) \
    struct extension {                                                      \
        enum: std::size_t { Index = __LINE__-1 };                                \
        constexpr static const char* string() { return #prefix "_" #vendor "_" #extension; } \
    };

#define _extension_rev(prefix, vendor, extension) \
    struct extension {                                                      \
        enum: std::size_t { Index = __LINE__-1 };                                \
        constexpr static const char* string() { return #prefix "_" #extension "_" #vendor; } \
    };

/* IMPORTANT: don't forget to add new extensions also in Context.cpp */
namespace AL {
    #line 1
    namespace EXT {
        _extension(AL,EXT,FLOAT32) // #???
        _extension(AL,EXT,DOUBLE) // #???
        _extension(AL,EXT,ALAW) // #???
        _extension(AL,EXT,MULAW) // #???
        _extension(AL,EXT,MCFORMATS) // #???
    }
} namespace ALC {
    namespace EXT {
        _extension_rev(ALC,EXT,ENUMERATION) // #???
    }
    namespace SOFTX {
        _extension(ALC,SOFTX,HRTF) // #???
    }
    namespace SOFT {
        _extension(ALC,SOFT,HRTF) // #???
    }
}
#undef _extension
#undef _extension_rev
#endif

}

}}

#endif
