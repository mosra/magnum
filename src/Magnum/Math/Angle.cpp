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

#include "Angle.h"

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <algorithm> /** @todo get rid of this once StringView::find() exists */
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/TweakableParser.h>

namespace Corrade { namespace Utility {

std::pair<TweakableState, Magnum::Math::Deg<Magnum::Float>> TweakableParser<Magnum::Math::Deg<Magnum::Float>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    char* end;
    const Magnum::Float result = std::strtof(value.data(), &end);

    if(end == value.begin() || std::find(value.begin(), value.end(), '.') == value.end()) {
        Warning{} << "Utility::TweakableParser:" << value << "is not an angle literal";
        return {TweakableState::Recompile, {}};
    }

    if(!value.hasSuffix("_degf"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _degf";
        return {TweakableState::Recompile, {}};
    }

    if(end != value.end() - 5) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after an angle literal";
        return {TweakableState::Recompile, {}};
    }

    return {TweakableState::Success, Magnum::Math::Deg<Magnum::Float>{result}};
}

std::pair<TweakableState, Magnum::Math::Deg<Magnum::Double>> TweakableParser<Magnum::Math::Deg<Magnum::Double>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    char* end;
    const Magnum::Double result = std::strtod(value.data(), &end);

    if(end == value.begin() || std::find(value.begin(), value.end(), '.') == value.end()) {
        Warning{} << "Utility::TweakableParser:" << value << "is not an angle literal";
        return {TweakableState::Recompile, {}};
    }

    if(!value.hasSuffix("_deg"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _deg";
        return {TweakableState::Recompile, {}};
    }

    if(end != value.end() - 4) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after an angle literal";
        return {TweakableState::Recompile, {}};
    }

    return {TweakableState::Success, Magnum::Math::Deg<Magnum::Double>{result}};
}

std::pair<TweakableState, Magnum::Math::Rad<Magnum::Float>> TweakableParser<Magnum::Math::Rad<Magnum::Float>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    char* end;
    const Magnum::Float result = std::strtof(value.data(), &end);

    if(end == value.begin() || std::find(value.begin(), value.end(), '.') == value.end()) {
        Warning{} << "Utility::TweakableParser:" << value << "is not an angle literal";
        return {TweakableState::Recompile, {}};
    }

    if(!value.hasSuffix("_radf"_s)) {
        Warning{} << "Utility::TweakableParser:" << value.data() << "has an unexpected suffix, expected _radf";
        return {TweakableState::Recompile, {}};
    }

    if(end != value.end() - 5) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after an angle literal";
        return {TweakableState::Recompile, {}};
    }

    return {TweakableState::Success, Magnum::Math::Rad<Magnum::Float>{result}};
}

std::pair<TweakableState, Magnum::Math::Rad<Magnum::Double>> TweakableParser<Magnum::Math::Rad<Magnum::Double>>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    char* end;
    const Magnum::Double result = std::strtod(value.data(), &end);

    if(end == value.begin() || std::find(value.begin(), value.end(), '.') == value.end()) {
        Warning{} << "Utility::TweakableParser:" << value << "is not an angle literal";
        return {TweakableState::Recompile, {}};
    }

    if(!value.hasSuffix("_rad"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _rad";
        return {TweakableState::Recompile, {}};
    }

    if(end != value.end() - 4) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after an angle literal";
        return {TweakableState::Recompile, {}};
    }

    return {TweakableState::Success, Magnum::Math::Rad<Magnum::Double>{result}};
}

}}
#endif
