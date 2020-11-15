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

#include "Version.h"

#include <cstdlib>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

Debug& operator<<(Debug& debug, const Version value) {
    if(!(debug.immediateFlags() & Debug::Flag::Packed))
        debug << "Vulkan";
    debug << versionMajor(value) << Debug::nospace << "." << Debug::nospace << versionMinor(value);
    if(const UnsignedInt patch = versionPatch(value))
        debug << Debug::nospace << "." << Debug::nospace << patch;
    return debug;
}

Version enumerateInstanceVersion() {
    if(!vkEnumerateInstanceVersion) return Version::Vk10;

    UnsignedInt version;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(vkEnumerateInstanceVersion(&version));
    return Version(version);
}

}}

namespace Corrade { namespace Utility {

using namespace Magnum;

Vk::Version ConfigurationValue<Vk::Version>::fromString(const Containers::StringView& stringValue, ConfigurationValueFlags) {
    /** @todo trim the string first, once Utility::String::trim() works for
        views */
    CORRADE_INTERNAL_ASSERT(stringValue.flags() & Containers::StringViewFlag::NullTerminated);

    char* end;
    const UnsignedInt major = std::strtoull(stringValue.data(), &end, 10);
    if(end == stringValue.begin() || end == stringValue.end() || *end != '.' || end + 1 == stringValue.end())
        return Vk::Version::None;

    const UnsignedInt minor = std::strtoull(end + 1, &end, 10);
    if(end != stringValue.end())
        return Vk::Version::None;

    return Vk::version(major, minor);
}

}}
