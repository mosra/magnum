#ifndef Magnum_Shaders_GenericShader_h
#define Magnum_Shaders_GenericShader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Magnum/Attribute.h"

namespace Magnum { namespace Shaders {

/**
@brief Generic shader definition

Definitions common for majority of shaders in @ref Shaders namespace, allowing
mesh configured for the generic shader to be used with any of them. See
@ref shaders-generic for more information.

@see @ref shaders, @ref Generic2D, @ref Generic3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> struct Generic;
#else
template<UnsignedInt dimensions> struct Generic {
    /**
     * @brief Vertex position
     *
     * @ref Vector2 in 2D and @ref Vector3 in 3D.
     */
    typedef Attribute<0, T> Position;

    /**
     * @brief 2D texture coordinates
     *
     * @ref Vector2.
     */
    typedef Attribute<1, Vector2> TextureCoordinates;

    /**
     * @brief Vertex normal
     *
     * @ref Vector3, defined only in 3D.
     */
    typedef Attribute<2, Vector3> Normal;

    /**
     * @brief Vertex color
     *
     * @ref Color4, however defaults to @ref Color3 if @ref MAGNUM_BUILD_DEPRECATED
     * is defined. See the constructor documentation for more information.
     */
    struct Color: Attribute<3, Color4> {
        /**
         * @brief Constructor
         * @param components    Component count
         * @param dataType      Type of passed data
         * @param dataOptions   Data options
         */
        constexpr explicit Color(Components components, DataType dataType = DataType::Float, DataOptions dataOptions = {});

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief Color(Components, DataType, DataOptions)
         * @deprecated Use @ref Color(Components, DataType, DataOptions) instead.
         */
        CORRADE_DEPRECATED("use Color(Components, DataType, DataOptions) instead") constexpr explicit Color(DataType dataType = DataType::Float, DataOptions dataOptions = {});
        #endif
    };
};
#endif

/** @brief Generic 2D shader definition */
typedef Generic<2> Generic2D;

/** @brief Generic 3D shader definition */
typedef Generic<3> Generic3D;

#ifndef DOXYGEN_GENERATING_OUTPUT
struct BaseGeneric {
    typedef Attribute<1, Vector2> TextureCoordinates;

    struct Color: Attribute<3, Color4> {
        constexpr explicit Color(Components components, DataType dataType = DataType::Float, DataOptions dataOptions = DataOptions()): Attribute<3, Color4>{components, dataType, dataOptions} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_DEPRECATED("use Color(Components, DataType, DataOptions) instead") constexpr explicit Color(DataType dataType = DataType::Float, DataOptions dataOptions = DataOptions()): Attribute<3, Color4>{Components::Three, dataType, dataOptions} {}
        #endif
    };
};

template<> struct Generic<2>: BaseGeneric {
    typedef Attribute<0, Vector2> Position;
};

template<> struct Generic<3>: BaseGeneric {
    typedef Attribute<0, Vector3> Position;
    typedef Attribute<2, Vector3> Normal;
};
#endif

}}

#endif
