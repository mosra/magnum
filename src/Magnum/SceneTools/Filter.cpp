/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Filter.h"

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/Optional.h>

#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

Trade::SceneData filterFields(const Trade::SceneData& scene, const Containers::BitArrayView fieldsToKeep) {
    CORRADE_ASSERT(fieldsToKeep.size() == scene.fieldCount(),
        "SceneTools::filterFields(): expected" << scene.fieldCount() << "bits but got" << fieldsToKeep.size(), (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    Containers::Array<Trade::SceneFieldData> filtered{ValueInit, fieldsToKeep.count()};
    std::size_t offset = 0;
    /** @todo some "iterate set bits" API */
    for(std::size_t i = 0; i != fieldsToKeep.size(); ++i) {
        if(!fieldsToKeep[i]) continue;
        filtered[offset++] = scene.fieldData(i);
    }
    CORRADE_INTERNAL_ASSERT(offset == filtered.size());

    return Trade::SceneData{scene.mappingType(), scene.mappingBound(),
        {}, scene.data(), std::move(filtered)};
}

Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, const Containers::ArrayView<const Trade::SceneField> fields) {
    Containers::BitArray fieldsToKeep{DirectInit, scene.fieldCount(), false};
    for(const Trade::SceneField field: fields) {
        if(const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field))
            fieldsToKeep.set(*fieldId);
    }
    return filterFields(scene, fieldsToKeep);
}

Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields) {
    return filterOnlyFields(scene, Containers::arrayView(fields));
}

Trade::SceneData filterExceptFields(const Trade::SceneData& scene, const Containers::ArrayView<const Trade::SceneField> fields) {
    Containers::BitArray fieldsToKeep{DirectInit, scene.fieldCount(), true};
    for(const Trade::SceneField field: fields) {
        if(const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field))
            fieldsToKeep.reset(*fieldId);
    }
    return filterFields(scene, fieldsToKeep);
}

Trade::SceneData filterExceptFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields) {
    return filterExceptFields(scene, Containers::arrayView(fields));
}

}}
