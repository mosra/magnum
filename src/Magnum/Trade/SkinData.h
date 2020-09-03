#ifndef Magnum_Trade_SkinData_h
#define Magnum_Trade_SkinData_h
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

/** @file
 * @brief Class @ref Magnum::Trade::SkinData
 * @m_since_latest
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Skin data
@m_since_latest

@see @ref SkinData2D, @ref SkinData3D, @ref AbstractImporter::skin2D(),
    @ref AbstractImporter::skin3D()
*/
template<UnsignedInt dimensions> class SkinData {
    public:
        /**
         * @brief Constructor
         * @param jointData         IDs of objects that act as joints
         * @param inverseBindMatrixData  Inverse bind matrix for each joint
         * @param importerState     Importer-specific state
         *
         * The @p jointData and @p inverseBindMatrixData arrays are expected to
         * have the same size.
         */
        explicit SkinData(Containers::Array<UnsignedInt>&& jointData, Containers::Array<MatrixTypeFor<dimensions, Float>>&& inverseBindMatrixData, const void* importerState = nullptr) noexcept;

        /** @overload */
        explicit SkinData(std::initializer_list<UnsignedInt> joints, std::initializer_list<MatrixTypeFor<dimensions, Float>> inverseBindMatrices, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned skin data
         * @param jointDataFlags    Ignored. Used only for a safer distinction
         *      from the owning constructor.
         * @param jointData         IDs of objects that act as joints
         * @param inverseBindMatrixDataFlags  Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param inverseBindMatrixData  Inverse bind matrix for each joint
         * @param importerState     Importer-specific state
         *
         * The @p jointData and @p inverseBindMatrixData arrays are expected to
         * have the same size.
         */
        explicit SkinData(DataFlags jointDataFlags, Containers::ArrayView<const UnsignedInt> jointData, DataFlags inverseBindMatrixDataFlags, Containers::ArrayView<const MatrixTypeFor<dimensions, Float>> inverseBindMatrixData, const void* importerState = nullptr) noexcept;

        /** @brief Copying is not allowed */
        SkinData(const SkinData<dimensions>&) = delete;

        /** @brief Move constructor */
        SkinData(SkinData<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        SkinData<dimensions>& operator=(const SkinData<dimensions>&) = delete;

        /** @brief Move assignment */
        SkinData<dimensions>& operator=(SkinData<dimensions>&& other) noexcept;

        /**
         * @brief Joint IDs
         *
         * IDs of objects that act as joints.
         * @see @ref AbstractImporter::object2D(),
         *      @ref AbstractImporter::object3D(), @ref releaseJointData()
         */
        Containers::ArrayView<const UnsignedInt> joints() const { return _jointData; }

        /**
         * @brief Inverse bind matrices
         *
         * Transforms each joint to the initial state for skinning to be
         * applied. The returned array has the same size as @ref joints().
         */
        Containers::ArrayView<const MatrixTypeFor<dimensions, Float>> inverseBindMatrices() const { return _inverseBindMatrixData; }

        /**
         * @brief Release joint data storage
         *
         * Releases the ownership of the joint ID array. The material then
         * behaves like if it has no joints.
         * @see @ref joints()
         */
        Containers::Array<UnsignedInt> releaseJointData();

        /**
         * @brief Release inverse bind matrix data storage
         *
         * Releases the ownership of the inverse bind matrix array. The
         * material then  behaves like if it has no matrices.
         * @see @ref inverseBindMatrices()
         */
        Containers::Array<MatrixTypeFor<dimensions, Float>> releaseInverseBindMatrixData();

        /** @brief Importer-specific state */
        const void* importerState() const { return _importerState; }

    private:
        /* For custom deleter checks. Not done in the constructors here because
           the restriction is pointless when used outside of plugin
           implementations. */
        friend AbstractImporter;

        /** @todo skeleton object ID? gltf has that but the use is unclear,
            this might help: https://github.com/KhronosGroup/glTF/pull/1747 */
        Containers::Array<UnsignedInt> _jointData;
        Containers::Array<MatrixTypeFor<dimensions, Float>> _inverseBindMatrixData;
        const void* _importerState;
};

/**
@brief Two-dimensional skin data
@m_since_latest

@see @ref AbstractImporter::skin2D()
*/
typedef SkinData<2> SkinData2D;

/**
@brief Three-dimensional skin data
@m_since_latest

@see @ref AbstractImporter::skin3D()
*/
typedef SkinData<3> SkinData3D;

}}

#endif
