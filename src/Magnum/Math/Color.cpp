/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>
#include <Corrade/Utility/Tweakable.h>
#endif

namespace Magnum { namespace Math {

namespace {
    constexpr const char Hex[]{"0123456789abcdef"};
}

Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color3<UnsignedByte>& value) {
    char out[] = "#______";
    out[1] = Hex[(value.r() >> 4) & 0xf];
    out[2] = Hex[(value.r() >> 0) & 0xf];
    out[3] = Hex[(value.g() >> 4) & 0xf];
    out[4] = Hex[(value.g() >> 0) & 0xf];
    out[5] = Hex[(value.b() >> 4) & 0xf];
    out[6] = Hex[(value.b() >> 0) & 0xf];
    return debug << out;
}

Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color4<UnsignedByte>& value) {
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

}}

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
namespace Corrade { namespace Utility {

std::pair<TweakableState, Magnum::Math::Color3<Magnum::UnsignedByte>> TweakableParser<Magnum::Math::Color3<Magnum::UnsignedByte>>::parse(const Containers::ArrayView<const char> value) {
    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = String::viewEndsWith(value, "_srgb");
    if(!isSrgb && !String::viewEndsWith(value, "_rgb")) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "has an unexpected suffix, expected _rgb or _srgb";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value, &end, 16);

    if(end != value.end() - (isSrgb ? 5 : 4)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << std::string{const_cast<const char*>(end), value.end()} <<  "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != (isSrgb ? 13 : 12)) {
        Error{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    return {TweakableState::Success,
        isSrgb ? Magnum::Math::Literals::operator "" _srgb(result) :
                 Magnum::Math::Literals::operator "" _rgb(result)};
}

std::pair<TweakableState, Magnum::Math::Color4<Magnum::UnsignedByte>> TweakableParser<Magnum::Math::Color4<Magnum::UnsignedByte>>::parse(const Containers::ArrayView<const char> value) {
    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = String::viewEndsWith(value, "_srgba");
    if(!isSrgb && !String::viewEndsWith(value, "_rgba")) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "has an unexpected suffix, expected _rgba or _srgba";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value, &end, 16);

    if(end != value.end() - (isSrgb ? 6 : 5)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << std::string{const_cast<const char*>(end), value.end()} <<  "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != (isSrgb ? 16 : 15)) {
        Error{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    return {TweakableState::Success,
        isSrgb ? Magnum::Math::Literals::operator "" _srgba(result) :
                 Magnum::Math::Literals::operator "" _rgba(result)};
}

std::pair<TweakableState, Magnum::Math::Color3<Magnum::Float>> TweakableParser<Magnum::Math::Color3<Magnum::Float>>::parse(const Containers::ArrayView<const char> value) {
    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = String::viewEndsWith(value, "_srgbf");
    if(!isSrgb && !String::viewEndsWith(value, "_rgbf")) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "has an unexpected suffix, expected _rgbf or _srgbf";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value, &end, 16);

    if(end != value.end() - (isSrgb ? 6 : 5)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << std::string{const_cast<const char*>(end), value.end()} <<  "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != (isSrgb ? 14 : 13)) {
        Error{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    return {TweakableState::Success,
        isSrgb ? Magnum::Math::Literals::operator "" _srgbf(result) :
                 Magnum::Math::Literals::operator "" _rgbf(result)};
}

std::pair<TweakableState, Magnum::Math::Color4<Magnum::Float>> TweakableParser<Magnum::Math::Color4<Magnum::Float>>::parse(const Containers::ArrayView<const char> value) {
    if(value.size() < 2 || value[0] != '0' || (value[1] != 'x' && value[1] != 'X')) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "is not a hexadecimal color literal";
        return {TweakableState::Recompile, {}};
    }

    const bool isSrgb = String::viewEndsWith(value, "_srgbaf");
    if(!isSrgb && !String::viewEndsWith(value, "_rgbaf")) {
        Warning{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "has an unexpected suffix, expected _rgbaf or _srgbaf";
        return {TweakableState::Recompile, {}};
    }

    char* end;
    const Magnum::UnsignedInt result = std::strtoul(value, &end, 16);

    if(end != value.end() - (isSrgb ? 7 : 6)) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << std::string{const_cast<const char*>(end), value.end()} <<  "after a color literal";
        return {TweakableState::Recompile, {}};
    }

    if(value.size() != (isSrgb ? 17 : 16)) {
        Error{} << "Utility::TweakableParser:" << std::string{value, value.size()} << "doesn't have expected number of digits";
        return {TweakableState::Error, {}};
    }

    return {TweakableState::Success,
        isSrgb ? Magnum::Math::Literals::operator "" _srgbaf(result) :
                 Magnum::Math::Literals::operator "" _rgbaf(result)};
}

}}
#endif
