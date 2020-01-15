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

#include "Interleave.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

bool isInterleaved(const Trade::MeshData& data) {
    /* There is nothing, so yes it is (because there is nothing we could do
       to make it interleaved anyway) */
    if(!data.attributeCount()) return true;

    const UnsignedInt stride = data.attributeStride(0);
    std::size_t minOffset = data.attributeOffset(0);
    std::size_t maxOffset = minOffset;
    for(UnsignedInt i = 1; i != data.attributeCount(); ++i) {
        if(data.attributeStride(i) != stride) return false;

        const std::size_t offset = data.attributeOffset(i);
        minOffset = Math::min(minOffset, offset);
        maxOffset = Math::max(maxOffset, offset + vertexFormatSize(data.attributeFormat(i)));
    }

    return maxOffset - minOffset <= stride;
}

}}
