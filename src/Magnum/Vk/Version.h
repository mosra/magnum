#ifndef Magnum_Vk_Version_h
#define Magnum_Vk_Version_h
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
 * @brief Enum @ref Magnum::Vk::Version, function @ref Magnum::Vk::version(), @ref Magnum::Vk::versionMajor(), @ref Magnum::Vk::versionMinor(), @ref Magnum::Vk::versionPatch()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Vulkan version
@m_since_latest

@see @ref version(), @ref versionMajor(), @ref versionMinor(), @ref versionPatch()
*/
enum class Version: UnsignedInt {
    None = 0xffffffffu,             /**< Unspecified */

    /* Not using VK_MAKE_VERSION() because it would warn about C-style casts */
    Vk10 = (1 << 22) | (0 << 12),   /**< Vulkan 1.0 */
    Vk11 = (1 << 22) | (1 << 12),   /**< Vulkan 1.1 */
    Vk12 = (1 << 22) | (2 << 12)    /**< Vulkan 1.2 */
};

/**
@brief Create a version from components
@m_since_latest

Equivalent to @def_vk_keyword{MAKE_VERSION}, but without C-style casts.
*/
constexpr Version version(UnsignedInt major, UnsignedInt minor, UnsignedInt patch = 0) {
    return Version((major << 22) | (minor << 12) | patch);
}

/**
@brief Extract major version number from a packed representation
@m_since_latest

Equivalent to @def_vk_keyword{VERSION_MAJOR}, but without C-style casts.
*/
constexpr UnsignedInt versionMajor(Version version) {
    return UnsignedInt(version) >> 22;
}

/**
@brief Extract minor version number from a packed representation
@m_since_latest

Equivalent to @def_vk_keyword{VERSION_MINOR}, but without C-style casts.
*/
constexpr UnsignedInt versionMinor(Version version) {
    return (UnsignedInt(version) >> 12) & 0x3ff;
}

/**
@brief Extract minor version number from a packed representation
@m_since_latest

Equivalent to @def_vk_keyword{VERSION_PATCH}, but without C-style casts.
*/
constexpr UnsignedInt versionPatch(Version version) {
    return UnsignedInt(version) & 0xfff;
}

/**
@brief Whether a version is smaller than the other
@m_since_latest
*/
constexpr bool operator<(Version a, Version b) {
    return UnsignedInt(a) < UnsignedInt(b);
}

/**
@brief Whether a version is smaller than or equal to the other
@m_since_latest
*/
constexpr bool operator<=(Version a, Version b) {
    return UnsignedInt(a) <= UnsignedInt(b);
}

/**
@brief Whether a version is greater than or equal to the other
@m_since_latest
*/
constexpr bool operator>=(Version a, Version b) {
    return UnsignedInt(a) >= UnsignedInt(b);
}

/**
@brief Whether a version is greater than the other
@m_since_latest
*/
constexpr bool operator>(Version a, Version b) {
    return UnsignedInt(a) > UnsignedInt(b);
}

/**
@debugoperatorenum{Version}
@m_since_latest

Prints the version as @cb{.shell-session} <major>.<minor>.<patch> @ce, or just
as @cb{.shell-session} <major>.<minor> @ce if patch is zero.
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, Version value);

/**
@brief Enumerate version supported by the instance
@m_since_latest

Note that the @fn_vk{EnumerateInstanceVersion} function isn't available in
Vulkan 1.0 and thus is fetched at runtime via @fn_vk{GetInstanceProcAddr}
called in a static constructor. On Vulkan 1.0 always returns
@ref Version::Vk10, in newer versions the returned version number contains also
patch information.
@see @ref versionMajor(), @ref versionMinor(), @ref versionPatch(),
    @fn_vk_keyword{EnumerateInstanceVersion}
*/
MAGNUM_VK_EXPORT Version enumerateInstanceVersion();

}}

namespace Corrade { namespace Utility {

/**
@configurationvalue{Magnum::Vk::Version}
@m_since_latest

Parses a value in the form of `<major>.<minor>`, returns
@ref Magnum::Vk::Version::None on error. Saving a value to string is not
implemented.
*/
template<> struct ConfigurationValue<Magnum::Vk::Version> {
    ConfigurationValue() = delete;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    MAGNUM_VK_EXPORT static Magnum::Vk::Version fromString(const Containers::StringView& stringValue, ConfigurationValueFlags);
    #endif
};

}}

#endif
