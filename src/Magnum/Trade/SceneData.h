#ifndef Magnum_Trade_SceneData_h
#define Magnum_Trade_SceneData_h
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
 * @brief Class @ref Magnum::Trade::SceneData
 */

#include <string>
#include <vector>

#include "Magnum/Types.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Scene data

@see @ref AbstractImporter::scene()
*/
class MAGNUM_TRADE_EXPORT SceneData {
    public:
        /**
         * @brief Constructor
         * @param children2D        Two-dimensional child objects
         * @param children3D        Three-dimensional child objects
         * @param importerState     Importer-specific state
         */
        explicit SceneData(std::vector<UnsignedInt> children2D, std::vector<UnsignedInt> children3D, const void* importerState = nullptr);

        /** @brief Copying is not allowed */
        SceneData(const SceneData&) = delete;

        /** @brief Move constructor */
        SceneData(SceneData&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Copying is not allowed */
        SceneData& operator=(const SceneData&) = delete;

        /** @brief Move assignment */
        SceneData& operator=(SceneData&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Two-dimensional child objects */
        const std::vector<UnsignedInt>& children2D() const { return _children2D; }

        /** @brief Three-dimensional child objects */
        const std::vector<UnsignedInt>& children3D() const { return _children3D; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        std::vector<UnsignedInt> _children2D,
            _children3D;
        const void* _importerState;
};

}}

#endif
