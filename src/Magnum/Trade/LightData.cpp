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

#include "LightData.h"

namespace Magnum { namespace Trade {

using namespace Math::Literals;

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Vector3& attenuation, const Float range, const Rad innerConeAngle, const Rad outerConeAngle, const void* const importerState) noexcept: _type{type}, _color{color}, _intensity{intensity}, _attenuation{attenuation}, _range{range}, _innerConeAngle{innerConeAngle}, _outerConeAngle{outerConeAngle}, _importerState{importerState} {
    CORRADE_ASSERT(_type != Type::Spot || (Deg(_innerConeAngle) >= 0.0_degf && _innerConeAngle <= _outerConeAngle && Deg(_outerConeAngle) <= 360.0_degf),
        "Trade::LightData: spot light inner and outer cone angles have to be in range [0°, 360°] and inner not larger than outer but got" << Deg(_innerConeAngle) << "and" << Deg(_outerConeAngle), );
    CORRADE_ASSERT(_type == Type::Spot || (Math::equal(Deg(_innerConeAngle),360.0_degf) && Math::equal(Deg(_outerConeAngle), 360.0_degf)),
        "Trade::LightData: cone angles have to be 360° for lights that aren't spot but got" << Deg(_innerConeAngle) << "and" << Deg(_outerConeAngle), );
    CORRADE_ASSERT((_type != Type::Ambient && _type != Type::Directional) || (_attenuation == Vector3{1.0f, 0.0f, 0.0f}),
        "Trade::LightData: attenuation has to be (1, 0, 0) for an ambient or directional light but got" << _attenuation, );
    CORRADE_ASSERT((_type != Type::Ambient && _type != Type::Directional) || _range == Constants::inf(),
        "Trade::LightData: range has to be infinity for an ambient or directional light but got" << _range, );
}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Vector3& attenuation, const Float range, const void* const importerState) noexcept: LightData{type, color, intensity, attenuation, range,
    type == Type::Spot ? 0.0_degf : 360.0_degf,
    type == Type::Spot ? 90.0_degf : 360.0_degf,
    importerState} {}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Vector3& attenuation, const Rad innerConeAngle, const Rad outerConeAngle, const void* const importerState) noexcept: LightData{type, color, intensity, attenuation, Constants::inf(), innerConeAngle, outerConeAngle, importerState} {}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Vector3& attenuation, const void* const importerState) noexcept: LightData{type, color, intensity, attenuation, Constants::inf(), importerState} {}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Float range, const Rad innerConeAngle, const Rad outerConeAngle, const void* const importerState) noexcept: LightData{type, color, intensity,
    type == Type::Ambient || type == Type::Directional ?
        Vector3{1.0f, 0.0f, 0.0f} : Vector3{1.0f, 0.0f, 1.0f},
    range, innerConeAngle, outerConeAngle, importerState} {}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Float range, const void* const importerState) noexcept: LightData{type, color, intensity,
    type == Type::Ambient || type == Type::Directional ?
        Vector3{1.0f, 0.0f, 0.0f} : Vector3{1.0f, 0.0f, 1.0f},
    range, importerState} {}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const Rad innerConeAngle, const Rad outerConeAngle, const void* const importerState) noexcept: LightData{type, color, intensity, Constants::inf(), innerConeAngle, outerConeAngle, importerState} {}

LightData::LightData(const Type type, const Color3& color, const Float intensity, const void* const importerState) noexcept: LightData{type, color, intensity, Constants::inf(), importerState} {}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const LightData::Type value) {
    debug << "Trade::LightData::Type" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case LightData::Type::value: return debug << "::" #value;
        _c(Ambient)
        _c(Directional)
        _c(Point)
        _c(Spot)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}
#endif

}}
