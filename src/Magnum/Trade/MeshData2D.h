#ifndef Magnum_Trade_MeshData2D_h
#define Magnum_Trade_MeshData2D_h
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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Class @ref Magnum::Trade::MeshData2D
 * @m_deprecated_since{2020,06} Use @ref Magnum/Trade/MeshData.h and the
 *      @ref Magnum::Trade::MeshData "MeshData" class instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>

#include "Magnum/Trade/MeshData.h"

#ifndef _MAGNUM_NO_DEPRECATED_MESHDATA
CORRADE_DEPRECATED_FILE("use Magnum/Trade/MeshData.h and the MeshData class instead")
#endif

namespace Magnum { namespace Trade {

/**
@brief Two-dimensional mesh data

Provides access to mesh data and additional information, such as primitive
type.

It's possible to use @ref MeshTools::transformPointsInPlace() and
@ref MeshTools::transformVectorsInPlace() to do transformations on the stored
vertex data. For example, baking a particular translation + rotation + scaling
directly to vertex positions:

@snippet MagnumTrade.cpp MeshData2D-transform

@m_deprecated_since{2020,06} Use @ref MeshData instead.

@see @ref AbstractImporter::mesh2D(), @ref MeshData3D
*/
class CORRADE_DEPRECATED("use MeshData instead") MAGNUM_TRADE_EXPORT MeshData2D {
    public:
        /**
         * @brief Constructor
         * @param primitive         Primitive
         * @param indices           Index array or empty array, if the mesh is
         *      not indexed
         * @param positions         Position arrays. At least one position
         *      array should be present.
         * @param textureCoords2D   Two-dimensional texture coordinate arrays,
         *      if present
         * @param colors            Vertex color arrays, if present
         * @param importerState     Importer-specific state
         */
        explicit MeshData2D(MeshPrimitive primitive, std::vector<UnsignedInt> indices, std::vector<std::vector<Vector2>> positions, std::vector<std::vector<Vector2>> textureCoords2D, std::vector<std::vector<Color4>> colors, const void* importerState = nullptr);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Construct from @ref MeshData
         * @m_deprecated_since{2020,06} Use @ref MeshData directly instead.
         */
        /* No data moving can take place because std::vector is damn shitty
           regarding memory ownership transfer, so it can well be a copy. */
        CORRADE_DEPRECATED("use MeshData directly instead") /*implicit*/ MeshData2D(const MeshData& other);
        #endif

        /** @brief Copying is not allowed */
        MeshData2D(const MeshData2D&) = delete;

        /** @brief Move constructor */
        MeshData2D(MeshData2D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        ~MeshData2D();

        /** @brief Copying is not allowed */
        MeshData2D& operator=(const MeshData2D&) = delete;

        /** @brief Move assignment */
        MeshData2D& operator=(MeshData2D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Primitive */
        MeshPrimitive primitive() const { return _primitive; }

        /** @brief Whether the mesh is indexed */
        bool isIndexed() const { return !_indices.empty(); }

        /**
         * @brief Indices
         *
         * @see @ref isIndexed()
         */
        std::vector<UnsignedInt>& indices();
        const std::vector<UnsignedInt>& indices() const; /**< @overload */

        /**
         * @brief Count of position arrays
         *
         * There is always at least one.
         */
        UnsignedInt positionArrayCount() const { return _positions.size(); }

        /**
         * @brief Positions
         * @param id    Position array ID
         *
         * @see @ref positionArrayCount()
         */
        std::vector<Vector2>& positions(UnsignedInt id);
        const std::vector<Vector2>& positions(UnsignedInt id) const; /**< @overload */

        /** @brief Whether the data contain any 2D texture coordinates */
        bool hasTextureCoords2D() const { return !_textureCoords2D.empty(); }

        /** @brief Count of 2D texture coordinate arrays */
        UnsignedInt textureCoords2DArrayCount() const { return _textureCoords2D.size(); }

        /**
         * @brief 2D texture coordinates
         * @param id    Texture coordinate array ID
         *
         * @see @ref textureCoords2DArrayCount()
         */
        std::vector<Vector2>& textureCoords2D(UnsignedInt id);
        const std::vector<Vector2>& textureCoords2D(UnsignedInt id) const; /**< @overload */

        /** @brief Whether the data contain any vertex colors */
        bool hasColors() const { return !_colors.empty(); }

        /** @brief Count of color arrays */
        UnsignedInt colorArrayCount() const { return _colors.size(); }

        /**
         * @brief Vertex colors
         * @param id    Vertex color array ID
         *
         * @see @ref colorArrayCount()
         */
        std::vector<Color4>& colors(UnsignedInt id);
        const std::vector<Color4>& colors(UnsignedInt id) const; /**< @overload */

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        MeshPrimitive _primitive;
        std::vector<UnsignedInt> _indices;
        std::vector<std::vector<Vector2>> _positions;
        std::vector<std::vector<Vector2>> _textureCoords2D;
        std::vector<std::vector<Color4>> _colors;
        const void* _importerState;
};

}}
#else
#error use Magnum/Trade/MeshData.h and the MeshData class instead
#endif

#endif
