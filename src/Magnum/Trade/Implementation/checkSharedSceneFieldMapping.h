#ifndef Magnum_Trade_Implementation_checkSharedSceneFieldMapping_h
#define Magnum_Trade_Implementation_checkSharedSceneFieldMapping_h
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

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Trade/SceneData.h"

/* Used by SceneData and SceneTools::combineFields() internals */

namespace Magnum { namespace Trade { namespace Implementation {

/* A prefix of the arrays get filled by findSharedSceneFields() in whatever
   order the fields appear in SceneData. Position of the first ~UnsignedInt{}
   element indicates how many were found, all elements after are ~UnsignedInt{}
   as well. */
struct SharedSceneFieldIds {
    UnsignedInt trs[3]{
        ~UnsignedInt{},
        ~UnsignedInt{},
        ~UnsignedInt{}
    };
    UnsignedInt meshMaterial[2]{
        ~UnsignedInt{},
        ~UnsignedInt{}
    };
};

inline SharedSceneFieldIds findSharedSceneFields(const Containers::ArrayView<const SceneFieldData> fields) {
    std::size_t trsFieldOffset = 0;
    std::size_t meshMaterialOffset = 0;
    SharedSceneFieldIds out;
    for(std::size_t i = 0; i != fields.size(); ++i) {
        const SceneFieldData& field = fields[i];
        if(field.name() == SceneField::Translation)
            out.trs[trsFieldOffset++] = i;
        else if(field.name() == SceneField::Rotation)
            out.trs[trsFieldOffset++] = i;
        else if(field.name() == SceneField::Scaling)
            out.trs[trsFieldOffset++] = i;
        else if(field.name() == SceneField::Mesh)
            out.meshMaterial[meshMaterialOffset++] = i;
        else if(field.name() == SceneField::MeshMaterial)
            out.meshMaterial[meshMaterialOffset++] = i;
    }

    CORRADE_INTERNAL_ASSERT(trsFieldOffset <= Containers::arraySize(out.trs));
    CORRADE_INTERNAL_ASSERT(meshMaterialOffset <= Containers::arraySize(out.meshMaterial));

    return out;
}

#ifndef CORRADE_NO_ASSERT
inline bool checkFieldMappingDataMatch(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    messagePrefix
    #endif
    , Containers::ArrayView<const UnsignedInt> fieldIds, Containers::ArrayView<const void> data, const Containers::ArrayView<const SceneFieldData> fields)
{
    /* Going through all fields and comparing to the first one, aborting if the
       field ID is unset (as all other will be as well). If no fields are
       present, the first is already unset, which means it'll break right at
       the start, never accessing `fields[~UnsignedInt{}]`. */
    for(const UnsignedInt fieldId: fieldIds) {
        if(fieldId == ~UnsignedInt{})
            break;

        const Trade::SceneFieldData& a = fields[fieldIds[0]];
        const Trade::SceneFieldData& b = fields[fieldId];
        const Containers::StridedArrayView1D<const void> aData = a.mappingData(data);
        const Containers::StridedArrayView1D<const void> bData = b.mappingData(data);
        CORRADE_ASSERT(aData.data() == bData.data() && aData.size() == bData.size() && aData.stride() == bData.stride(),
            messagePrefix << b.name() << "mapping data {" << Debug::nospace << bData.data() << Debug::nospace << "," << bData.size() << Debug::nospace << "," << bData.stride() << Debug::nospace << "} is different from" << a.name() << "mapping data {" << Debug::nospace << aData.data() << Debug::nospace << "," << aData.size() << Debug::nospace << "," << aData.stride() << Debug::nospace << "}", false);
    }

    return true;
}

inline bool checkSharedSceneFieldMapping(const char* messagePrefix, const SharedSceneFieldIds& fieldIds, Containers::ArrayView<const void> data, const Containers::ArrayView<const SceneFieldData> fields) {
    for(const Containers::ArrayView<const UnsignedInt> i: {
        /* All present TRS fields should share the same object mapping */
        Containers::arrayView(fieldIds.trs),
        /* Mesh and materials also */
        Containers::arrayView(fieldIds.meshMaterial)
    }) {
        if(!checkFieldMappingDataMatch(messagePrefix, i, data, fields))
            return false;
    }

    return true;
}
#endif

}}}

#endif
