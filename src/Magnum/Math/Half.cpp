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

#include "Magnum/Math/Half.h"

#include <iomanip>
#include <sstream>
#include <Corrade/Utility/DebugStl.h>

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <algorithm> /** @todo get rid of this once StringView::find() exists */
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/TweakableParser.h>
#endif

namespace Magnum { namespace Math {

Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, Half value) {
    std::ostringstream out;
    /* Wikipedia says it's 3 or 4 decimal places:
       https://en.wikipedia.org/wiki/Half-precision_floating-point_format */
    out << std::setprecision(4) << Float(value);
    return debug << out.str();
}

}}

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
namespace Corrade { namespace Utility {

std::pair<TweakableState, Magnum::Math::Half> TweakableParser<Magnum::Math::Half>::parse(const Containers::StringView value) {
    using namespace Containers::Literals;

    char* end;
    const Magnum::Float result = std::strtof(value.data(), &end);

    if(end == value.begin() || std::find(value.begin(), value.end(), '.') == value.end()) {
        Warning{} << "Utility::TweakableParser:" << value << "is not a half literal";
        return {TweakableState::Recompile, {}};
    }

    if(!value.hasSuffix("_h"_s)) {
        Warning{} << "Utility::TweakableParser:" << value << "has an unexpected suffix, expected _h";
        return {TweakableState::Recompile, {}};
    }

    if(end != value.end() - 2) {
        Warning{} << "Utility::TweakableParser: unexpected characters" << value.suffix(end) << "after a half literal";
        return {TweakableState::Recompile, {}};
    }

    return {TweakableState::Success, Magnum::Math::Half{result}};
}

}}
#endif
