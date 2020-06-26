#ifndef Magnum_Trade_Data_h
#define Magnum_Trade_Data_h
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
 * @brief Enum @ref Magnum::Trade::DataFlag, enum set @ref Magnum::Trade::DataFlags
 * @m_since{2020,06}
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Data flag
@m_since{2020,06}

@see @ref DataFlags, @ref AnimationData::dataFlags(),
    @ref ImageData::dataFlags(), @ref MeshData::indexDataFlags(),
    @ref MeshData::vertexDataFlags()
*/
enum class DataFlag: UnsignedByte {
    /**
     * Data is owned by the instance. If this flag is not set, the instance
     * might be for example referencing a memory-mapped file or a constant
     * memory.
     */
    Owned = 1 << 0,

    /**
     * Data is mutable. If this flag is not set, the instance might be for
     * example referencing a readonly memory-mapped file or a constant memory.
     */
    Mutable = 2 << 0

    /** @todo owned by importer, owned by the GPU, ... */
};

/**
@debugoperatorenum{DataFlag}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, DataFlag value);

/**
@brief Data flags
@m_since{2020,06}

@see @ref AnimationData::dataFlags(), @ref ImageData::dataFlags(),
    @ref MeshData::indexDataFlags(), @ref MeshData::vertexDataFlags()
*/
typedef Containers::EnumSet<DataFlag> DataFlags;

CORRADE_ENUMSET_OPERATORS(DataFlags)

/**
@debugoperatorenum{DataFlags}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, DataFlags value);

namespace Implementation {
    /* Used internally by MeshData */
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(char*, std::size_t);
}

}}

#endif
