/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Color.h"

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <cstring>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Tweakable.h>
#endif

/* MSVC 2019 with the /permissive- flag crashes with the variadic template
   implementation of color literals, see header for details. The half-float
   ones are deinlined to avoid including Half.h in the header, so the include
   has to be here instead. */
#if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG_CL) && _MSC_VER >= 1920 && _MSC_VER < 1930
#include "Magnum/Math/Half.h"
#endif

namespace Magnum { namespace Math {

namespace {
    constexpr const char Hex[]{"0123456789abcdef"};
}

Debug& operator<<(Debug& debug, const Color3<UnsignedByte>& value) {
    /* Print an actual colored square if requested */
    if(debug.immediateFlags() & Debug::Flag::Color) {
        /* Pick a shade based on calculated lightness */
        const Float valueValue = value.value();
        const char* shade;
        if(valueValue <= 0.2f)      shade = "  ";
        else if(valueValue <= 0.4f) shade = "░░";
        else if(valueValue <= 0.6f) shade = "▒▒";
        else if(valueValue <= 0.8f) shade = "▓▓";
        else                        shade = "██";

        /* If ANSI colors are disabled, use just the shade */
        if(debug.immediateFlags() & Debug::Flag::DisableColors)
            return debug << shade;
        else {
            debug << "\033[38;2;";

            /* Disable space between values for everything after the initial
               value */
            const Debug::Flags previousFlags = debug.flags();
            debug.setFlags(previousFlags|Debug::Flag::NoSpace);

            /* Set both background and foreground, reset back after */
            debug << int(value.r()) << ";" << int(value.g()) << ";"
                << int(value.b()) << "m\033[48;2;" << int(value.r()) << ";" <<
                int(value.g()) << ";" << int(value.b()) << "m" << shade << "\033[0m";

            /* Reset original flags */
            debug.setFlags(previousFlags);
            return debug;
        }

    /* Otherwise print a CSS color */
    } else {
        char out[] = "#______";
        out[1] = Hex[(value.r() >> 4) & 0xf];
        out[2] = Hex[(value.r() >> 0) & 0xf];
        out[3] = Hex[(value.g() >> 4) & 0xf];
        out[4] = Hex[(value.g() >> 0) & 0xf];
        out[5] = Hex[(value.b() >> 4) & 0xf];
        out[6] = Hex[(value.b() >> 0) & 0xf];
        return debug << out;
    }
}

Debug& operator<<(Debug& debug, const Color4<UnsignedByte>& value) {
    /* Print an actual colored square if requested */
    if(debug.immediateFlags() & Debug::Flag::Color) {
        /* Pick a shade based on calculated lightness */
        const Float valueValue = value.value();
        const Float alpha = Math::unpack<Float>(value.a());
        const Float valueAlpha = valueValue*alpha;
        const char* shade;
        if(valueAlpha <= 0.2f)      shade = "  ";
        else if(valueAlpha <= 0.4f) shade = "░░";
        else if(valueAlpha <= 0.6f) shade = "▒▒";
        else if(valueAlpha <= 0.8f) shade = "▓▓";
        else                        shade = "██";

        /* If ANSI colors are disabled, use just the shade */
        if(debug.immediateFlags() & Debug::Flag::DisableColors)
            return debug << shade;
        else {
            debug << "\033[38;2;";

            /* Disable space between values for everything after the initial
               value */
            const Debug::Flags previousFlags = debug.flags();
            debug.setFlags(previousFlags|Debug::Flag::NoSpace);

            /* Print foreground color */
            debug << int(value.r()) << ";" << int(value.g()) << ";"
                << int(value.b()) << "m";

            /* If alpha is larger than perceived value, set also background */
            if(alpha > valueValue)
                debug << "\033[48;2;" << int(value.r()) << ";" <<
                int(value.g()) << ";" << int(value.b()) << "m";

            /* Print the shade and reset color back */
            debug << shade << "\033[0m";

            /* Reset original flags */
            debug.setFlags(previousFlags);
            return debug;
        }

    /* Otherwise print a CSS color */
    } else {
        char out[] = "#________";
        out[1] = Hex[(value.r() >> 4) & 0xf];
        out[2] = Hex[(value.r() >> 0) & 0xf];
        out[3] = Hex[(value.g() >> 4) & 0xf];
        out[4] = Hex[(value.g() >> 0) & 0xf];
        out[5] = Hex[(value.b() >> 4) & 0xf];
        out[6] = Hex[(value.b() >> 0) & 0xf];
        out[7] = Hex[(value.a() >> 4) & 0xf];
        out[8] = Hex[(value.a() >> 0) & 0xf];
        return debug << out;
    }
}

/* MSVC 2019 with the /permissive- flag crashes with the variadic template
   implementation, see header for details */
#if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG_CL) && _MSC_VER >= 1920 && _MSC_VER < 1930
namespace Literals { inline namespace ColorLiterals {

Color3<Half> operator""_rgbh(unsigned long long value) {
    return Color3<Half>{Color3<Float>::fromLinearRgbInt(value)};
}
Color3<Half> operator""_srgbh(unsigned long long value) {
    return Color3<Half>{Color3<Float>::fromSrgbInt(value)};
}
Color4<Half> operator""_rgbah(unsigned long long value) {
    return Color4<Half>{Color4<Float>::fromLinearRgbaInt(value)};
}
Color4<Half> operator""_srgbah(unsigned long long value) {
    return Color4<Half>{Color4<Float>::fromSrgbAlphaInt(value)};
}

}}
#endif

}}

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
namespace Corrade { namespace Utility {

Containers::Pair<TweakableState, Magnum::Math::Color3<Magnum::UnsignedByte>> TweakableParser<Magnum::Math::Color3<Magnum::UnsignedByte>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << value << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = value.hasSuffix("_srgb"_s);
    if(!isSrgb && !value.hasSuffix("_rgb"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _rgb or _srgb";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value.data(), &end, 16);

    if(end != value.end() - (isSrgb ? 5 : 4)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != std::size_t(isSrgb ? 13 : 12)) {
        Error{} << "Utility::TweakableParser:" << value << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    /* Both the _srgba and _rgba literals return the same value (but a
       different type) as they're meant mainly for self-documenting purposes.
       So here's no distinction either, and fromLinearRgbInt() is just
       splitting up the 24-bit integer to 8-bit parts. */
    return {TweakableState::Success,
        Magnum::Math::Color3<Magnum::UnsignedByte>::fromLinearRgbInt(result)};
}

Containers::Pair<TweakableState, Magnum::Math::Color4<Magnum::UnsignedByte>> TweakableParser<Magnum::Math::Color4<Magnum::UnsignedByte>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << value << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = value.hasSuffix("_srgba"_s);
    if(!isSrgb && !value.hasSuffix("_rgba"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _rgba or _srgba";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value.data(), &end, 16);

    if(end != value.end() - (isSrgb ? 6 : 5)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != std::size_t(isSrgb ? 16 : 15)) {
        Error{} << "Utility::TweakableParser:" << value << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    /* Both the _srgba and _rgba literals return the same value (but a
       different type) as they're meant mainly for self-documenting purposes.
       So here's no distinction either, and fromLinearRgbaInt() is just
       splitting up the 32-bit integer to 8-bit parts. */
    return {TweakableState::Success,
        Magnum::Math::Color4<Magnum::UnsignedByte>::fromLinearRgbaInt(result)};
}

Containers::Pair<TweakableState, Magnum::Math::Color3<Magnum::Float>> TweakableParser<Magnum::Math::Color3<Magnum::Float>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << value << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = value.hasSuffix("_srgbf"_s);
    if(!isSrgb && !value.hasSuffix("_rgbf"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _rgbf or _srgbf";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value.data(), &end, 16);

    if(end != value.end() - (isSrgb ? 6 : 5)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != std::size_t(isSrgb ? 14 : 13)) {
        Error{} << "Utility::TweakableParser:" << value << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    return {TweakableState::Success, isSrgb ?
        Magnum::Math::Color3<Magnum::Float>::fromSrgbInt(result) :
        Magnum::Math::Color3<Magnum::Float>::fromLinearRgbInt(result)};
}

Containers::Pair<TweakableState, Magnum::Math::Color4<Magnum::Float>> TweakableParser<Magnum::Math::Color4<Magnum::Float>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << value << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = value.hasSuffix("_srgbaf"_s);
    if(!isSrgb && !value.hasSuffix("_rgbaf"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _rgbaf or _srgbaf";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value.data(), &end, 16);

    if(end != value.end() - (isSrgb ? 7 : 6)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != (isSrgb ? 17 : 16)) {
        Error{} << "Utility::TweakableParser:" << value << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    return {TweakableState::Success, isSrgb ?
        Magnum::Math::Color4<Magnum::Float>::fromSrgbAlphaInt(result) :
        Magnum::Math::Color4<Magnum::Float>::fromLinearRgbaInt(result)};
}

}}
#endif
