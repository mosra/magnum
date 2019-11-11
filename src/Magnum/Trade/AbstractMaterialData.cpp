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

#include "AbstractMaterialData.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace Trade {

AbstractMaterialData::AbstractMaterialData(AbstractMaterialData&&) noexcept = default;

AbstractMaterialData& AbstractMaterialData::operator=(AbstractMaterialData&&) noexcept = default;

AbstractMaterialData::AbstractMaterialData(const MaterialType type, const Flags flags, const MaterialAlphaMode alphaMode, const Float alphaMask, const void* const importerState) noexcept: _type{type}, _alphaMode{alphaMode}, _flags{flags}, _alphaMask{alphaMask}, _importerState{importerState} {}

AbstractMaterialData::~AbstractMaterialData() = default;

Debug& operator<<(Debug& debug, const MaterialType value) {
    debug << "Trade::MaterialType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialType::value: return debug << "::" #value;
        _c(Phong)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const AbstractMaterialData::Flag value) {
    debug << "Trade::AbstractMaterialData::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AbstractMaterialData::Flag::value: return debug << "::" #value;
        _c(DoubleSided)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const AbstractMaterialData::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::AbstractMaterialData::Flags{}", {
        AbstractMaterialData::Flag::DoubleSided
    });
}

Debug& operator<<(Debug& debug, const MaterialAlphaMode value) {
    debug << "Trade::MaterialAlphaMode" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialAlphaMode::value: return debug << "::" #value;
        _c(Opaque)
        _c(Mask)
        _c(Blend)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

}}
