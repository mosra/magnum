#ifndef Magnum_MeshTools_Reference_h
#define Magnum_MeshTools_Reference_h
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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Function @ref Magnum::MeshTools::reference(), @ref Magnum::MeshTools::owned()
 * @m_deprecated_since_latest Use @ref Magnum/MeshTools/Copy.h instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/MeshTools/Copy.h"

#ifndef _MAGNUM_NO_DEPRECATED_MESHTOOLS_REFERENCE
CORRADE_DEPRECATED_FILE("use Magnum/MeshTools/Copy.h instead")
#endif

namespace Magnum { namespace MeshTools {

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Create an owned @ref Trade::MeshData
@m_deprecated_since_latest Use @ref copy(const Trade::MeshData&) instead.
*/
CORRADE_DEPRECATED("use copy(Trade::MeshData&&) instead") MAGNUM_MESHTOOLS_EXPORT Trade::MeshData owned(const Trade::MeshData& mesh);

/**
@brief Create an owned @ref Trade::MeshData, if not already
@m_deprecated_since_latest Use @ref copy(Trade::MeshData&&) instead.
*/
CORRADE_DEPRECATED("use copy(Trade::MeshData&&) instead") MAGNUM_MESHTOOLS_EXPORT Trade::MeshData owned(Trade::MeshData&& mesh);
#endif

}}
#else
#error use Magnum/MeshTools/Copy.h instead
#endif

#endif
