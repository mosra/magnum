#ifndef Magnum_Trade_MeshObjectData2D_h
#define Magnum_Trade_MeshObjectData2D_h
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
 * @brief Class @ref Magnum::Trade::MeshObjectData2D
 */

#include "Magnum/Trade/ObjectData2D.h"

namespace Magnum { namespace Trade {

/**
@brief Two-dimensional mesh object data

Provides access to material information for given mesh instance.
@see @ref AbstractImporter::object2D(), @ref MeshObjectData3D
*/
class MAGNUM_TRADE_EXPORT MeshObjectData2D: public ObjectData2D {
    public:
        /**
         * @brief Construct with combined transformation
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instance          Instance ID
         * @param material          Material ID or `-1`
         * @param skin              Skin ID or `-1`
         * @param importerState     Importer-specific state
         *
         * Creates object with mesh instance type.
         */
        explicit MeshObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, UnsignedInt instance, Int material, Int skin, const void* importerState = nullptr);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief MeshObjectData2D(std::vector<UnsignedInt>, const Matrix3&, UnsignedInt, Int, Int, const void*)
         * @m_deprecated_since_latest Use @ref MeshObjectData2D(std::vector<UnsignedInt>, const Matrix3&, UnsignedInt, Int, Int, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use MeshObjectData2D(std::vector<UnsignedInt>, const Matrix4&, UnsignedInt, Int, Int, const void*) instead") MeshObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, UnsignedInt instance, Int material, const void* importerState = nullptr): MeshObjectData2D{std::move(children), transformation, instance, material, -1, importerState} {}
        #endif

        /**
         * @brief Construct with separate transformations
         * @param children          Child objects
         * @param translation       Translation (relative to parent)
         * @param rotation          Rotation (relative to parent)
         * @param scaling           Scaling (relative to parent)
         * @param instance          Instance ID
         * @param material          Material ID or `-1`
         * @param skin              Skin ID or `-1`
         * @param importerState     Importer-specific state
         *
         * Creates object with mesh instance type.
         */
        explicit MeshObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, UnsignedInt instance, Int material, Int skin, const void* importerState = nullptr);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief MeshObjectData2D(std::vector<UnsignedInt>, const Vector2&, const Complex&, const Vector2&, UnsignedInt, Int, Int, const void*)
         * @m_deprecated_since_latest Use @ref MeshObjectData2D(std::vector<UnsignedInt>, const Vector2&, const Complex&, const Vector2&, UnsignedInt, Int, Int, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use MeshObjectData2D(std::vector<UnsignedInt>, const Vector2&, const Complex&, const Vector2&, UnsignedInt, Int, Int, const void*) instead") MeshObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, UnsignedInt instance, Int material, const void* importerState = nullptr): MeshObjectData2D{std::move(children), translation, rotation, scaling, instance, material, -1, importerState} {}
        #endif

        /** @brief Copying is not allowed */
        MeshObjectData2D(const MeshObjectData2D&) = delete;

        /** @brief Move constructor */
        MeshObjectData2D(MeshObjectData2D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            = default;

        /** @brief Copying is not allowed */
        MeshObjectData2D& operator=(const MeshObjectData2D&) = delete;

        /** @brief Move assignment */
        MeshObjectData2D& operator=(MeshObjectData2D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            = default;

        /**
         * @brief Material ID
         *
         * Returns `-1` if the object has no material assigned.
         * @see @ref AbstractImporter::material()
         */
        Int material() const { return _material; }

        /**
         * @brief Skin ID
         * @m_since_latest
         *
         * Returns `-1` if the object has no skin assigned.
         * @see @ref AbstractImporter::skin2D()
         */
        Int skin() const { return _skin; }

    private:
        Int _material, _skin;
};

}}

#endif
