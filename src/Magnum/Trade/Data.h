#ifndef Magnum_Trade_Data_h
#define Magnum_Trade_Data_h
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
 * @brief Enum @ref Magnum::Trade::DataFlag, enum set @ref Magnum::Trade::DataFlags
 * @m_since{2020,06}
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Data flag
@m_since{2020,06}

Used to describe data contained in various classes returned from
@ref AbstractImporter interfaces and also data passed internally in the
importer itself.
@see @ref DataFlags, @ref AnimationData::dataFlags(),
    @ref ImageData::dataFlags(), @ref MaterialData::attributeDataFlags(),
    @ref MaterialData::layerDataFlags(), @ref MeshData::indexDataFlags(),
    @ref MeshData::vertexDataFlags(), @ref SceneData::dataFlags(),
    @ref AbstractImporter::doOpenData()
*/
enum class DataFlag: UnsignedByte {
    /**
     * Data is owned by the instance, meaning it stays in scope for as long as
     * the instance. If neither @ref DataFlag::Owned,
     * @ref DataFlag::ExternallyOwned nor @ref DataFlag::Global is set, the
     * data is considered to be just a temporary allocation and no assumptions
     * about its lifetime can be made.
     */
    Owned = 1 << 0,

    /**
     * Data has an owner external to the instance, for example a memory-mapped
     * file or a constant memory. In general the data lifetime exceeds lifetime
     * of the instance wrapping it. If neither @ref DataFlag::Owned,
     * @ref DataFlag::ExternallyOwned nor @ref DataFlag::Global is set, the
     * data is considered to be just a temporary allocation and no assumptions
     * about its lifetime can be made.
     * @m_since_latest
     */
    ExternallyOwned = 1 << 3,

    /**
     * Data is global, for example stored in static memory, so guaranteed to
     * never go out of scope. Usually such data are not @ref DataFlag::Mutable.
     * If neither @ref DataFlag::Owned, @ref DataFlag::ExternallyOwned nor
     * @ref DataFlag::Global is set, the data is considered to be just a
     * temporary allocation and no assumptions about its lifetime can be made.
     * @m_since_latest
     */
    Global = 1 << 4,

    /**
     * Data is mutable. If this flag is not set, the instance might be for
     * example referencing a readonly memory-mapped file or a constant memory.
     */
    Mutable = 1 << 2

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
    /* Used internally by AnimationData, MaterialData, MeshData, SceneData,
       SkinData -- we need them to be exported symbols in the Trade library and
       not e.g. lambdas in order to ensure that data originating from
       dynamically-loaded plugins don't contain pointers to deleter functions
       contained inside the plugin binary, leading to a dangling function
       pointer call if the array gets destructed after the plugin was unloaded.

       All variants below do the same (which is nothing), but because
       reinterpret_cast<>'ing function pointers triggers a UBSan complaint,
       they are separate symbols. On MSVC /OPT:ICF *could* merge them, on other
       compilers it might result in unnecessary duplicated symbols, but being
       UBSan-clean is the bigger priority here. */
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(char*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(AnimationTrackData*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(MeshAttributeData*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(MaterialAttributeData*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(UnsignedInt*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(Matrix3*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(Matrix4*, std::size_t);
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(SceneFieldData*, std::size_t);
}

}}

#endif
