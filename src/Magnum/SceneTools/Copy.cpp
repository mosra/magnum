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

#include "Copy.h"

#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

Trade::SceneData reference(const Trade::SceneData& scene) {
    return Trade::SceneData{scene.mappingType(), scene.mappingBound(),
        {}, scene.data(), Trade::sceneFieldDataNonOwningArray(scene.fieldData())};
}

Trade::SceneData mutableReference(Trade::SceneData& scene) {
    CORRADE_ASSERT(scene.dataFlags() & Trade::DataFlag::Mutable,
        "SceneTools::mutableReference(): data not mutable",
        (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, {}, {}}));

    return Trade::SceneData{scene.mappingType(), scene.mappingBound(),
        Trade::DataFlag::Mutable, scene.mutableData(), Trade::sceneFieldDataNonOwningArray(scene.fieldData())};
}

Trade::SceneData copy(const Trade::SceneData& scene) {
    return copy(Trade::SceneData{scene.mappingType(), scene.mappingBound(),
        {}, scene.data(),
        Trade::sceneFieldDataNonOwningArray(scene.fieldData()),
        scene.importerState()});
}

Trade::SceneData copy(Trade::SceneData&& scene) {
    /* Transfer data if they're owned and mutable, allocate a copy otherwise.
       Save also the original data view for new field pointer calculation. */
    const Containers::ArrayView<const char> originalData = scene.data();
    Containers::Array<char> data;
    if(scene.dataFlags() >= (Trade::DataFlag::Mutable|Trade::DataFlag::Owned))
        data = scene.releaseData();
    else {
        data = Containers::Array<char>{NoInit, originalData.size()};
        Utility::copy(originalData, data);
    }

    /* There's no way to know if field data are owned until we release them and
       check the deleter, but releasing them makes it impossible to use the
       convenience SceneData APIs, so we have to do it the hard way. */
    Containers::Array<Trade::SceneFieldData> originalFieldData = scene.releaseFieldData();

    /* If the field data are owned *and* the data weren't copied, we can reuse
       the original array in its entirety */
    Containers::Array<Trade::SceneFieldData> fieldData;
    if(!originalFieldData.deleter() && (scene.dataFlags() & Trade::DataFlag::Owned)) {
        fieldData = Utility::move(originalFieldData);

    /* Otherwise we have to allocate a new one and re-route the fields to a
       potentially different data array */
    /** @todo could theoretically also just modify the array in-place if it has
        a default deleter, but would need to pay attention to not copy items
        to themselves and such */
    } else {
        /* Using DefaultInit so the array has a default deleter and isn't
           problematic to use in plugins */
        fieldData = Containers::Array<Trade::SceneFieldData>{DefaultInit, originalFieldData.size()};
        for(std::size_t i = 0; i != originalFieldData.size(); ++i) {
            const Trade::SceneFieldData& originalField = originalFieldData[i];

            /* If the field is offset-only, copy it directly, yay! */
            if(originalField.flags() & Trade::SceneFieldFlag::OffsetOnly)
                fieldData[i] = originalField;

            /* Otherwise there's a bunch of special cases based on its type */
            else {
                const Trade::SceneMappingType mappingType = originalField.mappingType();
                const Containers::StridedArrayView1D<const void> mappingView{
                    data, data.data() + (static_cast<const char*>(originalField.mappingData().data()) - originalData.data()),
                    originalField.size(),
                    originalField.mappingData().stride()};

                const Trade::SceneFieldType fieldType = originalField.fieldType();
                if(fieldType == Trade::SceneFieldType::Bit) {
                    if(originalField.fieldArraySize() == 0) {
                        const Containers::StridedBitArrayView1D fieldView{
                            /** @todo explicit construction from an ArrayView?! */
                            Containers::BitArrayView{data.data(), 0, data.size()*8},
                            data.data() + (static_cast<const char*>(originalField.fieldBitData().data()) - originalData.data()),
                            originalField.fieldBitData().offset(),
                            originalField.size(),
                            originalField.fieldBitData().stride()[0]};
                        fieldData[i] = Trade::SceneFieldData{originalField.name(),
                            mappingType, mappingView,
                            fieldView, originalField.flags()};
                    } else {
                        const Containers::StridedBitArrayView2D fieldView{
                            /** @todo explicit construction from an ArrayView?! */
                            Containers::BitArrayView{data.data(), 0, data.size()*8},
                            data.data() + (static_cast<const char*>(originalField.fieldBitData().data()) - originalData.data()),
                            originalField.fieldBitData().offset(),
                            {originalField.size(), originalField.fieldArraySize()},
                            originalField.fieldBitData().stride()};
                        fieldData[i] = Trade::SceneFieldData{originalField.name(),
                            mappingType, mappingView,
                            fieldView, originalField.flags()};
                    }
                } else {
                    const Containers::StridedArrayView1D<const void> fieldView{
                        data, data.data() + (static_cast<const char*>(originalField.fieldData().data()) - originalData.data()),
                        originalField.size(),
                        originalField.fieldData().stride()};

                    if(Trade::Implementation::isSceneFieldTypeString(fieldType)) {
                        fieldData[i] = Trade::SceneFieldData{originalField.name(),
                            mappingType, mappingView,
                            data.data() + (originalField.stringData() - originalData.data()),
                            fieldType, fieldView,
                            originalField.flags()};
                    } else {
                        fieldData[i] = Trade::SceneFieldData{originalField.name(),
                            mappingType, mappingView,
                            fieldType, fieldView,
                            originalField.fieldArraySize(), originalField.flags()};
                    }
                }
            }
        }
    }

    return Trade::SceneData{scene.mappingType(), scene.mappingBound(),
        Utility::move(data), Utility::move(fieldData), scene.importerState()};
}

}}
