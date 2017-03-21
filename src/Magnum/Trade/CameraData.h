#ifndef Magnum_Trade_CameraData_h
#define Magnum_Trade_CameraData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

/** @file
 * @brief Class @ref Magnum::Trade::CameraData
 */

#include <string>

#include "Magnum/Math/Angle.h"
#include "Magnum/Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief 3D camera data
*/
class CameraData {
    public:
        /**
         * @brief Constructor
         * @param fov               Field-of-view angle
         * @param near              Near clipping plane
         * @param far               Far clipping plane
         * @param importerState     Importer-specific state
         *
         * If `NaN` is specified for any parameter, default value is used
         * instead, which is `35.0_degf` for @p fov, `0.01f` for @p near and
         * `100.0f` for @p far.
         */
        explicit CameraData(Rad fov, Float near, Float far, const void* importerState = nullptr) noexcept;

        /** @brief Copying is not allowed */
        CameraData(const CameraData&) = delete;

        /** @brief Move constructor */
        CameraData(CameraData&&) noexcept = default;

        /** @brief Copying is not allowed */
        CameraData& operator=(const CameraData&) = delete;

        /** @brief Move assignment */
        CameraData& operator=(CameraData&&) noexcept = default;

        /** @brief Field-of-view angle */
        Rad fov() const { return _fov; }

        /** @brief Near clipping plane */
        Float near() const { return _near; }

        /** @brief Far clipping plane */
        Float far() const { return _far; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        Rad _fov;
        Float _near, _far;
        const void* _importerState;
};

inline CameraData::CameraData(const Rad fov, const Float near, const Float far, const void* const importerState) noexcept:
    _fov{fov != fov ? Rad{Deg{35.0f}} : fov},
    _near{near != near ? 0.01f : near},
    _far{far != far ? 100.0f : far},
    _importerState{importerState} {}

}}

#endif
