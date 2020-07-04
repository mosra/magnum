#ifndef Magnum_Trade_CameraData_h
#define Magnum_Trade_CameraData_h
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
 * @brief Class @ref Magnum::Trade::CameraData
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Camera type

@see @ref CameraData
*/
enum class CameraType: UnsignedByte {
    Orthographic2D,         /**< 2D orthographic camera */
    Orthographic3D,         /**< 3D orthographic camera */
    Perspective3D           /**< 3D perspective camera */
};

/**
@brief Camera data

@see @ref AbstractImporter::camera(),
    @ref Math::Matrix4::perspectiveProjection(Rad<T>, T, T, T),
    @ref Math::Matrix4::perspectiveProjection(const Vector2<T>&, T, T),
    @ref Math::Matrix4::orthographicProjection(const Vector2<T>&, T, T),
    @ref Math::Matrix3::projection(const Vector2<T>&)
*/
class MAGNUM_TRADE_EXPORT CameraData {
    public:
        /**
         * @brief Construct a camera using a field of view
         * @param type              Camera type
         * @param fov               Horizontal field-of-view angle @f$ \theta @f$
         * @param aspectRatio       Horizontal:vertical aspect ratio @f$ a @f$
         * @param near              Near clipping plane @f$ n @f$
         * @param far               Far clipping plane @f$ f @f$. Set to
         *      @ref Constants::inf() for an infinite far plane.
         * @param importerState     Importer-specific state
         *
         * The constructor converts the @p fov and @p aspectRatio to near plane
         * size using the following formula and stores that: @f[
         *      \boldsymbol{s} = 2n \tan \left(\tfrac{\theta}{2} \right)
         *      \begin{pmatrix}
         *          1 \\
         *          \frac{1}{a}
         *      \end{pmatrix}
         * @f]
         *
         * The @p type parameter has to be @ref CameraType::Perspective3D, use
         * @ref CameraData(CameraType, const Vector2&, Float, Float, const void*)
         * for orthographic and 2D cameras instead.
         */
        explicit CameraData(CameraType type, Rad fov, Float aspectRatio, Float near, Float far, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct a camera using a projection size
         * @param type              Camera type
         * @param size              Size of the near clipping plane
         * @param near              Near clipping plane
         * @param far               Far clipping plane. Set to @ref Constants::inf()
         *      for an infinite far plane.
         * @param importerState     Importer-specific state
         *
         * For @ref CameraType::Orthographic2D, @p near and @p far is expected
         * to be @cpp 0.0f @ce.
         */
        explicit CameraData(CameraType type, const Vector2& size, Float near, Float far, const void* importerState = nullptr) noexcept;

        /** @brief Copying is not allowed */
        CameraData(const CameraData&) = delete;

        /** @brief Move constructor */
        CameraData(CameraData&&) noexcept = default;

        /** @brief Copying is not allowed */
        CameraData& operator=(const CameraData&) = delete;

        /** @brief Move assignment */
        CameraData& operator=(CameraData&&) noexcept = default;

        /** @brief Camera type */
        CameraType type() const { return _type; }

        /**
         * @brief Size of the near clipping plane
         *
         * For @ref CameraType::Perspective3D, this property is also available
         * through @ref fov() and @ref aspectRatio().
         */
        Vector2 size() const { return _size; }

        /**
         * @brief Field-of-view angle
         *
         * Expects that @ref type() is @ref CameraType::Perspective3D. The
         * value is calculated from @ref size() using the following formula: @f[
         *      \theta = 2 \arctan \left( \frac{s_x}{2n} \right)
         * @f]
         */
        Rad fov() const;

        /**
         * @brief Aspect ratio
         *
         * Similarly to @ref fov(), the value is calculated from @ref size().
         */
        Float aspectRatio() const { return _size.aspectRatio(); }

        /** @brief Near clipping plane */
        Float near() const { return _near; }

        /**
         * @brief Far clipping plane
         *
         * Can be set to infinity, in which case it denotes a lack of far
         * clipping plane.
         * @see @ref Constants::inf(), @ref Math::isInf()
         */
        Float far() const { return _far; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        CameraType _type;
        Vector2 _size;
        Float _near, _far;
        const void* _importerState;
};

/** @debugoperatorenum{CameraType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, CameraType value);

}}

#endif
