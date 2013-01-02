#ifndef Magnum_Shaders_AbstractTextShader_h
#define Magnum_Shaders_AbstractTextShader_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Shaders::AbstractTextShader
 */

#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for text shaders

@see AbstractTextShader2D, AbstractTextShader3D
*/
template<std::uint8_t dimensions> class AbstractTextShader: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions>::PointType> Position;

        /** @brief Texture coordinates */
        typedef Attribute<1, Vector2> TextureCoordinates;

        /** @todoc Remove when Doxygen is sane */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        enum: GLint {
        #else
        static const GLint
        #endif
            FontTextureLayer = 16   /**< @brief Layer for font texture */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        };
        #else
        ;
        #endif

        ~AbstractTextShader() = 0;
};

template<std::uint8_t dimensions> inline AbstractTextShader<dimensions>::~AbstractTextShader() {}

/** @brief Base for two-dimensional text shaders */
typedef AbstractTextShader<2> AbstractTextShader2D;

/** @brief Base for three-dimensional text shader */
typedef AbstractTextShader<3> AbstractTextShader3D;

}}

#endif
