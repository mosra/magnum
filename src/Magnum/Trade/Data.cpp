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

#include "Data.h"

#include <Corrade/Containers/EnumSet.hpp>

namespace Magnum { namespace Trade {

Debug& operator<<(Debug& debug, const DataFlag value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::DataFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case DataFlag::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
        _c(Owned)
        _c(ExternallyOwned)
        _c(Global)
        _c(Mutable)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const DataFlags value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Trade::DataFlags{}", {
        DataFlag::Owned,
        DataFlag::ExternallyOwned,
        DataFlag::Global,
        DataFlag::Mutable});
}

namespace Implementation {
    void nonOwnedArrayDeleter(char*, std::size_t) {}
    void nonOwnedArrayDeleter(AnimationTrackData*, std::size_t) {}
    void nonOwnedArrayDeleter(MeshAttributeData*, std::size_t) {}
    void nonOwnedArrayDeleter(MaterialAttributeData*, std::size_t) {}
    void nonOwnedArrayDeleter(UnsignedInt*, std::size_t) {}
    void nonOwnedArrayDeleter(Matrix3*, std::size_t) {}
    void nonOwnedArrayDeleter(Matrix4*, std::size_t) {}
    void nonOwnedArrayDeleter(SceneFieldData*, std::size_t) {}
}

}}
