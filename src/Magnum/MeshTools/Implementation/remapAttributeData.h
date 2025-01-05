#ifndef Magnum_MeshTools_Implementation_remapAttributeData_h
#define Magnum_MeshTools_Implementation_remapAttributeData_h
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

#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Implementation {

/* Common helper used by various MeshTools algorithms.

   Remaps MeshAttributeData to be relative to the passed vertexData array,
   which is assumed to be exactly the same as the original except in different
   memory location. All properties including array size are kept but
   offset-only attributes are changed to absolute and attributes with
   (placeholder) zero vertex count are changed to actual vertex count. */
inline Trade::MeshAttributeData remapAttributeData(const Trade::MeshAttributeData& attribute, const UnsignedInt vertexCount, const Containers::ArrayView<const char> originalVertexData, Containers::ArrayView<const char> vertexData) {
    return Trade::MeshAttributeData{
        attribute.name(),
        attribute.format(),
        Containers::StridedArrayView1D<const void>{
            vertexData,
            vertexData.data() + attribute.offset(originalVertexData),
            vertexCount,
            attribute.stride()},
        attribute.arraySize(),
        attribute.morphTargetId()};
}

}}}

#endif
