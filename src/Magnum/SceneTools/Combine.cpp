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

#include "Combine.h"

#include "Magnum/SceneTools/Implementation/combine.h"

namespace Magnum { namespace SceneTools {

Trade::SceneData combineFields(const Trade::SceneMappingType mappingType, const UnsignedLong mappingBound, const Containers::ArrayView<const Trade::SceneFieldData> fields) {
    /* See the function documentation for details why is it inlined in a helper
       header */
    /** @todo move everything here once that's dropped */
    return Implementation::combineFields(mappingType, mappingBound, fields);
}

Trade::SceneData combineFields(const Trade::SceneMappingType mappingType, const UnsignedLong mappingBound, const std::initializer_list<Trade::SceneFieldData> fields) {
    return combineFields(mappingType, mappingBound, Containers::arrayView(fields));
}

Trade::SceneData combineFields(const Trade::SceneData& scene) {
    /* Can't just pass scene.fieldData() directly as those can be offset-only */
    Containers::Array<Trade::SceneFieldData> fields{NoInit, scene.fieldCount()};
    for(std::size_t i = 0; i != fields.size(); ++i)
        fields[i] = scene.fieldData(i);
    return combineFields(scene.mappingType(), scene.mappingBound(), fields);
}

}}
