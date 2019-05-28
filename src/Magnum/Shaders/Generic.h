#ifndef Magnum_Shaders_GenericShader_h
#define Magnum_Shaders_GenericShader_h
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
 * @brief Struct @ref Magnum::Shaders::Generic, typedef @ref Magnum::Shaders::Generic2D, @ref Magnum::Shaders::Generic3D
 */

#include "Magnum/GL/Attribute.h"

namespace Magnum { namespace Shaders {

/**
@brief Generic shader definition

Definitions common for majority of shaders in the @ref Shaders namespace,
allowing mesh configured for a generic shader to be used with any of them.
See @ref shaders-generic for more information.
@see @ref shaders, @ref Generic2D, @ref Generic3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> struct Generic;
#else
template<UnsignedInt dimensions> struct Generic {
    /* Keep consistent with generic.glsl and the real definitions below */

    /**
     * @brief Vertex position
     *
     * @ref Magnum::Vector2 "Vector2" in 2D and @ref Magnum::Vector3 "Vector3"
     * in 3D.
     */
    typedef GL::Attribute<0, T> Position;

    /**
     * @brief 2D texture coordinates
     *
     * @ref Magnum::Vector2 "Vector2".
     */
    typedef GL::Attribute<1, Vector2> TextureCoordinates;

    /**
     * @brief Vertex normal
     *
     * @ref Magnum::Vector3 "Vector3", defined only in 3D.
     */
    typedef GL::Attribute<2, Vector3> Normal;

    /**
     * @brief Vertex tangent
     *
     * @ref Magnum::Vector3 "Vector3", defined only in 3D.
     */
    typedef GL::Attribute<4, Vector3> Tangent;

    /**
     * @brief Three-component vertex color.
     *
     * @ref Magnum::Color3. Use either this or the @ref Color4 attribute.
     */
    typedef GL::Attribute<3, Magnum::Color3> Color3;

    /**
     * @brief Four-component vertex color.
     *
     * @ref Magnum::Color4. Use either this or the @ref Color3 attribute.
     */
    typedef GL::Attribute<3, Magnum::Color4> Color4;

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @brief Vertex color
     * @deprecated Use @ref Color3 or @ref Color4 instead.
     */
    struct Color: GL::Attribute<3, Magnum::Color4> {
        /**
         * @brief Constructor
         * @param components    Component count
         * @param dataType      Type of passed data
         * @param dataOptions   Data options
         *
         * @deprecated Use @ref Color3 or @ref Color4 instead.
         */
        CORRADE_DEPRECATED("use Color3 or Color4 instead") constexpr explicit Color(Components components, DataType dataType = DataType::Float, DataOptions dataOptions = {});

        /** @brief @copybrief Color(Components, DataType, DataOptions)
         * @deprecated Use @ref Color3 or @ref Color4 instead.
         */
        CORRADE_DEPRECATED("use Color3 or Color4 instead") constexpr explicit Color(DataType dataType = DataType::Float, DataOptions dataOptions = {});
    };
    #endif
};
#endif

/** @brief Generic 2D shader definition */
typedef Generic<2> Generic2D;

/** @brief Generic 3D shader definition */
typedef Generic<3> Generic3D;

#ifndef DOXYGEN_GENERATING_OUTPUT
struct BaseGeneric {
    typedef GL::Attribute<1, Vector2> TextureCoordinates;
    typedef GL::Attribute<3, Magnum::Color3> Color3;
    typedef GL::Attribute<3, Magnum::Color4> Color4;

    #ifdef MAGNUM_BUILD_DEPRECATED
    struct Color: GL::Attribute<3, Magnum::Color4> {
        CORRADE_DEPRECATED("use Color3 or Color4 instead") constexpr explicit Color(Components components, DataType dataType = DataType::Float, DataOptions dataOptions = DataOptions()): Attribute<3, Magnum::Color4>{components, dataType, dataOptions} {}

        CORRADE_DEPRECATED("use Color3 or Color4 instead") constexpr explicit Color(DataType dataType = DataType::Float, DataOptions dataOptions = DataOptions()): Attribute<3, Magnum::Color4>{Components::Three, dataType, dataOptions} {}
    };
    #endif
};

template<> struct Generic<2>: BaseGeneric {
    typedef GL::Attribute<0, Vector2> Position;
};

template<> struct Generic<3>: BaseGeneric {
    typedef GL::Attribute<0, Vector3> Position;
    typedef GL::Attribute<2, Vector3> Normal;
    typedef GL::Attribute<4, Vector3> Tangent;
};
#endif

}}

#endif
