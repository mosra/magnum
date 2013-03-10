#ifndef Magnum_Shaders_AbstractVectorShader_h
#define Magnum_Shaders_AbstractVectorShader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Shaders::AbstractVectorShader, typedef Magnum::Shaders::AbstractVectorShader2D, Magnum::Shaders::AbstractVectorShader3D
 */

#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for vector shaders

@see AbstractVectorShader2D, AbstractVectorShader3D
*/
template<UnsignedInt dimensions> class AbstractVectorShader: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions>::VectorType> Position;

        /** @brief Texture coordinates */
        typedef Attribute<1, Vector2> TextureCoordinates;

        enum: Int {
            VectorTextureLayer = 16 /**< Layer for vector texture */
        };

        virtual ~AbstractVectorShader() = 0;
};

template<UnsignedInt dimensions> inline AbstractVectorShader<dimensions>::~AbstractVectorShader() {}

/** @brief Base for two-dimensional text shaders */
typedef AbstractVectorShader<2> AbstractVectorShader2D;

/** @brief Base for three-dimensional text shader */
typedef AbstractVectorShader<3> AbstractVectorShader3D;

}}

#endif
