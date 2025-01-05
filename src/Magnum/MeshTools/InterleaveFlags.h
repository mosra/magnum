#ifndef Magnum_MeshTools_InterleaveFlags_h
#define Magnum_MeshTools_InterleaveFlags_h
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
 * @brief Enum @ref Magnum::MeshTools::InterleaveFlag, enum set @ref Magnum::MeshTools::InterleaveFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace MeshTools {

/**
@brief Interleaving behavior flag
@m_since_latest

@see @ref InterleaveFlags,
    @ref interleavedLayout(const Trade::MeshData&, UnsignedInt, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags),
    @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags),
    @ref concatenate(const Containers::Iterable<const Trade::MeshData>&, InterleaveFlags)
*/
enum class InterleaveFlag: UnsignedInt {
    /**
     * If the mesh is already interleaved, preserves existing layout of the
     * attributes as well as any padding or aliasing among them, keeping the
     * original stride and only removing the initial offset. This can also
     * preserve attributes with an implementation-specific @ref VertexFormat.
     *
     * If not set or if the mesh is not interleaved to begin with, a tightly
     * packed stride is calculated from vertex format sizes of all attributes,
     * removing all padding. In that case an implementation-specific
     * @ref VertexFormat can't be used for any attribute.
     */
    PreserveInterleavedAttributes = 1 << 0,

    /**
     * If a mesh is indexed, makes @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags)
     * preserve the index buffer even if it's not tightly packed. Since such
     * data layouts are not commonly supported by GPU APIs, this flag is not
     * set by default.
     *
     * If not set and the index buffer is strided, a tightly packed copy with
     * the same index type is allocated for the output, dropping also any
     * padding before or after the original index view. In such case however,
     * the index type is not allowed to be implementation-specific.
     *
     * Has no effect when passed to @ref interleavedLayout(const Trade::MeshData&, UnsignedInt, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags) "interleavedLayout()"
     * as that function doesn't preserve the index buffer. Has no effect when
     * passed to @ref concatenate(const Containers::Iterable<const Trade::MeshData>&, InterleaveFlags) "concatenate()"
     * as that function allocates a new combined index buffer anyway.
     * @see @ref isMeshIndexTypeImplementationSpecific()
     */
    PreserveStridedIndices = 1 << 1
};

/**
@brief Interleaving behavior flags
@m_since_latest

@see @ref interleavedLayout(const Trade::MeshData&, UnsignedInt, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags),
    @ref interleave(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>, InterleaveFlags),
    @ref concatenate(const Containers::Iterable<const Trade::MeshData>&, InterleaveFlags)
*/
typedef Containers::EnumSet<InterleaveFlag> InterleaveFlags;

CORRADE_ENUMSET_OPERATORS(InterleaveFlags)

}}

#endif
