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

#include "SkinData.h"

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

template<UnsignedInt dimensions> SkinData<dimensions>::SkinData(Containers::Array<UnsignedInt>&& jointData, Containers::Array<MatrixTypeFor<dimensions, Float>>&& inverseBindMatrixData, const void* const importerState) noexcept: _jointData{std::move(jointData)}, _inverseBindMatrixData{std::move(inverseBindMatrixData)}, _importerState{importerState} {
    CORRADE_ASSERT(_jointData.size() == _inverseBindMatrixData.size(),
        "Trade::SkinData: joint and inverse bind matrix arrays have different size, got" << _jointData.size() << "and" << _inverseBindMatrixData.size(), );
}

template<UnsignedInt dimensions> SkinData<dimensions>::SkinData(const std::initializer_list<UnsignedInt> joints, const std::initializer_list<MatrixTypeFor<dimensions, Float>> inverseBindMatrices, const void* const importerState): SkinData{Implementation::initializerListToArrayWithDefaultDeleter(joints), Implementation::initializerListToArrayWithDefaultDeleter(inverseBindMatrices), importerState} {}

template<UnsignedInt dimensions> SkinData<dimensions>::SkinData(DataFlags, const Containers::ArrayView<const UnsignedInt> jointData, DataFlags, const Containers::ArrayView<const MatrixTypeFor<dimensions, Float>> inverseBindMatrixData, const void* const importerState) noexcept: SkinData<dimensions>{Containers::Array<UnsignedInt>{const_cast<UnsignedInt*>(jointData.data()), jointData.size(), reinterpret_cast<void(*)(UnsignedInt*, std::size_t)>(Implementation::nonOwnedArrayDeleter)}, Containers::Array<MatrixTypeFor<dimensions, Float>>{const_cast<MatrixTypeFor<dimensions, Float>*>(inverseBindMatrixData.data()), inverseBindMatrixData.size(), reinterpret_cast<void(*)(MatrixTypeFor<dimensions, Float>*, std::size_t)>(Implementation::nonOwnedArrayDeleter)}, importerState} {}

template<UnsignedInt dimensions> SkinData<dimensions>::SkinData(SkinData<dimensions>&&) noexcept = default;

template<UnsignedInt dimensions> SkinData<dimensions>& SkinData<dimensions>::operator=(SkinData<dimensions>&&) noexcept = default;

template<UnsignedInt dimensions> Containers::Array<UnsignedInt> SkinData<dimensions>::releaseJointData() {
    return std::move(_jointData);
}

template<UnsignedInt dimensions> Containers::Array<MatrixTypeFor<dimensions, Float>> SkinData<dimensions>::releaseInverseBindMatrixData() {
    return std::move(_inverseBindMatrixData);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_TRADE_EXPORT SkinData<2>;
template class MAGNUM_TRADE_EXPORT SkinData<3>;
#endif

}}
