#ifndef Magnum_MeshTools_Copy_h
#define Magnum_MeshTools_Copy_h
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

/** @file
 * @brief Function @ref Magnum::MeshTools::copy(), @ref Magnum::MeshTools::reference(), @ref Magnum::MeshTools::mutableReference()
 * @m_since_latest
 */

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Make an owned copy of the mesh
@m_since_latest

Allocates a copy of @ref Trade::MeshData::indexData(),
@relativeref{Trade::MeshData,vertexData()} and
@relativeref{Trade::MeshData,attributeData()} and returns a new mesh with them.
All other properties such as the primitive or importer state are passed through
unchanged, the data layout isn't changed in any way. The resulting
@ref Trade::MeshData::indexDataFlags() and
@relativeref{Trade::MeshData,vertexDataFlags()} are always
@ref Trade::DataFlag::Owned and @ref Trade::DataFlag::Mutable. Attributes that
were offset-only before are kept offset-only, others have offsets recalculated
against the newly-allocated vertex data.
@see @ref copy(Trade::MeshData&&), @ref reference(), @ref mutableReference(),
    @ref meshtools-helpers
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData copy(const Trade::MeshData& mesh);

/**
@brief Make a mesh with owned data
@m_since_latest

If @ref Trade::MeshData::indexDataFlags() or
@relativeref{Trade::MeshData,vertexDataFlags()} are not
@ref Trade::DataFlag::Owned and @ref Trade::DataFlag::Mutable or the attribute
data don't have the default deleter, allocates a copy of
@ref Trade::MeshData::indexData(),
@relativeref{Trade::MeshData,vertexData()} or
@relativeref{Trade::MeshData,attributeData()}, otherwise transfers their
ownership. The resulting data are always owned and mutable, the data layout
isn't changed in any way. Attributes that were offset-only before are kept
offset-only, others have offsets recalculated against the
potentially-newly-allocated vertex data.
@see @ref reference(), @ref mutableReference(), @ref meshtools-helpers
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData copy(Trade::MeshData&& mesh);

/**
@brief Create an immutable reference on a @ref Trade::MeshData
@m_since{2020,06}

The returned instance has empty @ref Trade::MeshData::indexDataFlags() and
@ref Trade::MeshData::vertexDataFlags() and references attribute data from the
@p mesh as well. The function performs no allocation or data copy. Use
@ref copy() for an inverse operation.
@see @ref mutableReference(), @ref meshtools-helpers
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData reference(const Trade::MeshData& mesh);

/**
@brief Create a mutable reference on a @ref Trade::MeshData
@m_since{2020,06}

The returned instance has @ref Trade::MeshData::indexDataFlags() and
@ref Trade::MeshData::vertexDataFlags() set to @ref Trade::DataFlag::Mutable.
The function performs no allocation or data copy. Use @ref copy() for an
inverse operation. Expects that @p mesh is mutable.
@see @ref reference(), @ref meshtools-helpers
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData mutableReference(Trade::MeshData& mesh);

}}

#endif
