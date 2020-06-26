#ifndef Magnum_MeshTools_Reference_h
#define Magnum_MeshTools_Reference_h
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
 * @brief Function @ref Magnum::MeshTools::reference(), @ref Magnum::MeshTools::owned()
 * @m_since{2020,06}
 */

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Create an immutable reference on a @ref Trade::MeshData
@m_since{2020,06}

The returned instance has empty @ref Trade::MeshData::indexDataFlags() and
@ref Trade::MeshData::vertexDataFlags() and references attribute data from the
@p data as well. The function performs no allocation or data copy. Use
@ref owned() for an inverse operation.
@see @ref mutableReference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData reference(const Trade::MeshData& data);

/**
@brief Create a mutable reference on a @ref Trade::MeshData
@m_since{2020,06}

The returned instance has @ref Trade::MeshData::indexDataFlags() and
@ref Trade::MeshData::vertexDataFlags() set to @ref Trade::DataFlag::Mutable.
The function performs no allocation or data copy. Use @ref owned() for an
inverse operation. Expects that @p data is mutable.
@see @ref reference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData mutableReference(Trade::MeshData& data);

/**
@brief Create an owned @ref Trade::MeshData
@m_since{2020,06}

The returned instance owns its index, vertex and attribute data --- both
@ref Trade::MeshData::indexDataFlags() and
@ref Trade::MeshData::vertexDataFlags() have @ref Trade::DataFlag::Mutable and
@ref Trade::DataFlag::Owned set. This function unconditionally does an
allocation and a copy even if the @p data is already owned, use
@ref owned(Trade::MeshData&&) to make an owned copy only if the instance isn't
already owned.
@see @ref reference(), @ref mutableReference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData owned(const Trade::MeshData& data);

/**
@brief Create an owned @ref Trade::MeshData, if not already
@m_since{2020,06}

The returned instance owns its index, vertex and attribute data --- both
@ref Trade::MeshData::indexDataFlags() and
@ref Trade::MeshData::vertexDataFlags() have @ref Trade::DataFlag::Mutable and
@ref Trade::DataFlag::Owned set. Index, vertex and attribute data that are
already owned are simply moved to the output, otherwise the data get copied
into newly allocated arrays.
@see @ref reference(), @ref mutableReference()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData owned(Trade::MeshData&& data);

}}

#endif
