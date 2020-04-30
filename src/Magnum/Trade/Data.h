#ifndef Magnum_Trade_Data_h
#define Magnum_Trade_Data_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Struct @ref Magnum::Trade::DataChunkHeader, enum @ref Magnum::Trade::DataFlag, @ref Magnum::Trade::DataChunkSignature, @ref Magnum::Trade::DataChunkType, enum set @ref Magnum::Trade::DataFlags, function @ref Magnum::Trade::isDataChunk(), @ref Magnum::Trade::dataChunkHeaderDeserialize(), @ref Magnum::Trade::dataChunkHeaderSerializeInto()
 * @m_since_latest
 */

#include <cstddef>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Data flag
@m_since_latest

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
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, DataFlag value);

/**
@brief Data flags
@m_since_latest

@see @ref AnimationData::dataFlags(), @ref ImageData::dataFlags(),
    @ref MeshData::indexDataFlags(), @ref MeshData::vertexDataFlags()
*/
typedef Containers::EnumSet<DataFlag> DataFlags;

CORRADE_ENUMSET_OPERATORS(DataFlags)

/**
@debugoperatorenum{DataFlags}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, DataFlags value);

/**
@brief Memory-mappable data chunk type
@m_since_latest

A [FourCC](https://en.wikipedia.org/wiki/FourCC)-like identifier of the data
contained in the chunk. Used together with @ref DataChunkHeader::typeVersion to
identify version of a particular chunk type.

@section Trade-DataChunkType-custom Custom data chunk types

All identifiers starting with an uppercase leter are reserved for Magnum
itself, custom application-specific data types should use a lowercase first
letter instead. Casing of the three remaining characters doesn't have any
specified effect in the current version of the header. It doesn't need to be
alphanumeric either, but for additional versioning of a particular chunk type
it's recommended to use @ref DataChunkHeader::typeVersion, keeping the chunk
type FourCC clearly recognizable.

@see @ref blob
*/
enum class DataChunkType: UnsignedInt {
    /**
     * Serialized @ref MeshData. The letters `Mesh`.
     *
     * Current version is @cpp 0 @ce.
     */
    Mesh = Utility::Endianness::fourCC('M', 'e', 's', 'h'),

    #if 0
    /* None of these used yet, here just to lay out the naming scheme */
    Animation = Utility::Endianness::fourCC('A', 'n', 'i', 'm'),
    Camera = Utility::Endianness::fourCC('C', 'a', 'm', 0),
    Image1D = Utility::Endianness::fourCC('I', 'm', 'g', '1'),
    Image2D = Utility::Endianness::fourCC('I', 'm', 'g', '2'),
    Image3D = Utility::Endianness::fourCC('I', 'm', 'g', '3'),
    Light = Utility::Endianness::fourCC('L', 'i', 'g', 't'),
    Material = Utility::Endianness::fourCC('M', 't', 'l', 0),
    Scene = Utility::Endianness::fourCC('S', 'c', 'n', 0),
    Texture = Utility::Endianness::fourCC('T', 'e', 'x', 0)
    #endif
};

/**
@debugoperatorenum{DataChunkType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, DataChunkType value);

/**
@brief Memory-mappable data chunk signature
@m_since_latest

Reads as `BLOB` letters for a Little-Endian 64 bit data chunk. For Big-Endian
the order is reversed (thus `BOLB`), 32-bit data have the `L` letter lowercase.
@see @ref blob, @ref DataChunkHeader::signature
*/
enum class DataChunkSignature: UnsignedInt {
    /** Little-Endian 32-bit data. The letters `BlOB`. */
    Little32 = Utility::Endianness::fourCC('B', 'l', 'O', 'B'),

    /** Little-Endian 64-bit data. The letters `BLOB`. */
    Little64 = Utility::Endianness::fourCC('B', 'L', 'O', 'B'),

    /** Big-Endian 32-bit data. The letters `BOlB`. */
    Big32 = Utility::Endianness::fourCC('B', 'O', 'l', 'B'),

    /** Big-Endian 64-bit data. The letters `BOLB`. */
    Big64 = Utility::Endianness::fourCC('B', 'O', 'L', 'B'),

    /** Signature matching this platform. Alias to one of the above. */
    Current
        #ifndef DOXYGEN_GENERATING_OUTPUT
        =
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        sizeof(std::size_t) == 8 ? Little64 : Little32
        #else
        sizeof(std::size_t) == 8 ? Big64 : Big32
        #endif
        #endif
};

/**
@debugoperatorenum{DataChunkSignature}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, DataChunkSignature value);

/**
@brief Header for memory-mappable data chunks
@m_since_latest

See @ref blob for an introduction.

Since the goal of the serialization format is to be a direct equivalent to the
in-memory data layout, there's four different variants of the header based on
whether it's running on a 32-bit or 64-bit system and whether the machine is
Little- or Big-Endian. A 64-bit variant of the header has 24 bytes to support
data larger than 4 GB, the 32-bit variant is 20 bytes. Apart from the @ref size
member, the header is designed to contain the same amount of information on
both, and its size is chosen so the immediately following data can be aligned
to either 4 or 8 bytes without needing to add extra padding.

The header contents are as follows, vaguely inspired by the
[PNG file header](https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_header).
All fields except @ref typeVersion and @ref size (marked with
@m_class{m-label m-primary} **E**) are stored in an endian-independent way,
otherwise the endian matches the signature field.

@m_class{m-row m-container-inflate}

@parblock

@m_class{m-fullwidth}

Byte offset | Byte size&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Member | Contents
----------- | --------- | ------------- | -------------------------------------
0 | 1 | @ref DataChunkHeader::version "version" | Header version. Has the high bit set to prevent the file from being detected as text. Currently set to @cpp 128 @ce.
1           | 1         | @ref eolUnix  | Unix EOL (LF, @cpp '\x0a' @ce), to detect unwanted Unix-to-DOS line ending conversion
2           | 2         | @ref eolDos   | DOS EOL (CR+LF, @cpp '\x0d', '\x0a' @ce), to detect unwanted DOS-to-Unix line ending conversion
4           | 4         | @ref signature | File signature. Differs based on bitness and endianness, see @ref DataChunkSignature for more information.
8           | 2         | @ref zero     | Two zero bytes (@cpp '\x00', '\x00' @ce), to prevent the data from being treated and copied as a null-terminated (wide) string.
10 | 2 @m_class{m-label m-primary} **E** | @ref typeVersion | Data chunk type version. Use is chunk-specific, see @ref DataChunkType for more information.
12          | 4         | @ref type     | Data chunk type, see @ref DataChunkType for more information
16 | 4 or 8 @m_class{m-label m-primary} **E** | @ref size | Data chunk size, including the header size. Stored in size matching the signature field.

@endparblock

For a particular header variant the first 10 bytes is static and thus can be
used for file validation. After the header are directly the chunk data. For performance reasons it's recommended to have the data padded to be a multiple
of 4 or 8 bytes to ensure the immediately following chunk is correctly aligned
as well, but it's not a strict recommendation and not enforced in any way in
current version of the format.

Current version of the header doesn't have any checksum field in order to make
it easy to modify the data in-place, this might change in the future.
@see @ref DataChunkSignature, @ref DataChunkType, @ref isDataChunk(),
    @ref dataChunkHeaderDeserialize(), @ref dataChunkHeaderSerializeInto()
*/
struct DataChunkHeader {
    UnsignedByte version;           /**< @brief Header version */
    char eolUnix[1];                /**< @brief Unix EOL */
    char eolDos[2];                 /**< @brief Dos EOL */
    DataChunkSignature signature;   /**< @brief Signature */
    UnsignedShort zero;             /**< @brief Two zero bytes */
    UnsignedShort typeVersion;      /**< @brief Chunk type version */
    DataChunkType type;             /**< @brief Chunk type */
    std::size_t size;               /**< @brief Chunk size */
};

/**
@brief Check if given data blob is a valid data chunk
@m_since_latest

Returns @cpp true @ce if @p data is a valid @ref DataChunkHeader, matches
current platform and @p data is large enough to contain the whole chunk,
@cpp false @ce otherwise. The function doesn't print any diagnostic messages on
validation failure, use @ref dataChunkHeaderDeserialize() instead if you need
to know why.
@see @ref blob
*/
MAGNUM_TRADE_EXPORT bool isDataChunk(Containers::ArrayView<const void> data);

/**
@brief Try to deserialize a data chunk from a memory-mappable representation
@m_since_latest

Checks that @p data is large enough to contain a valid data chunk, validates
the header and then returns @p data reinterpreted as a @ref DataChunkHeader
pointer. On failure prints an error message and returns @cpp nullptr @ce.
@see @ref blob, @ref isDataChunk(), @ref dataChunkHeaderSerializeInto()
*/
MAGNUM_TRADE_EXPORT const DataChunkHeader* dataChunkHeaderDeserialize(Containers::ArrayView<const void> data);

/**
@brief Serialize a data chunk header into existing array
@param[out] out         Where to write the output
@param[out] type        Data chunk type
@param[out] typeVersion Data chunk type version
@return  Number of bytes written. Same as size of @ref DataChunkHeader.
@m_since_latest

Expects that @p data is at least the size of @ref DataChunkHeader. Fills in
@ref DataChunkHeader::typeVersion and @ref DataChunkHeader::type with passed
values used in constructor, and @ref DataChunkHeader::size with @p data size.

@see @ref blob, @ref dataChunkHeaderDeserialize()
*/
MAGNUM_TRADE_EXPORT std::size_t dataChunkHeaderSerializeInto(Containers::ArrayView<char> out, DataChunkType type, UnsignedShort typeVersion);

namespace Implementation {
    /* Used internally by MeshData */
    MAGNUM_TRADE_EXPORT void nonOwnedArrayDeleter(char*, std::size_t);
}

}}

#endif
