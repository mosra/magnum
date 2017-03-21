#ifndef Magnum_Shaders_Vector_h
#define Magnum_Shaders_Vector_h
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
 * @brief Class @ref Magnum::Shaders::Vector, typedef @ref Magnum::Shaders::Vector2D, @ref Magnum::Shaders::Vector3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Vector shader

Renders vector art in plain grayscale form. See also @ref DistanceFieldVector
for more advanced effects. For rendering unchanged texture you can use the
@ref Flat shader. You need to provide @ref Position and @ref TextureCoordinates
attributes in your triangle mesh and call at least
@ref setTransformationProjectionMatrix(), @ref setColor() and
@ref setVectorTexture().

@image html shaders-vector.png
@image latex shaders-vector.png

## Example usage

Common mesh setup:
@code
struct Vertex {
    Vector2 position;
    Vector2 textureCoordinates;
};
Vertex data[] = { ... };

Buffer vertices;
vertices.setData(data, BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::Vector2D::Position{},
    Shaders::Vector2D::TextureCoordinates{});
@endcode

Common rendering setup:
@code
Matrix3 transformationMatrix, projectionMatrix;
Texture2D texture;

Shaders::Vector2D shader;
shader.setColor(Color3::fromHSV(216.0_degf, 0.85f, 1.0f))
    .setVectorTexture(texture)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
@endcode

@see @ref shaders, @ref Vector2D, @ref Vector3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Vector: public AbstractVector<dimensions> {
    public:
        explicit Vector();

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         */
        explicit Vector(NoCreateT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : AbstractVector<dimensions>{NoCreate}
            #endif
            {}

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        Vector& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
            AbstractShaderProgram::setUniform(_transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set background color
         * @return Reference to self (for method chaining)
         *
         * Default is transparent black.
         * @see @ref setColor()
         */
        Vector& setBackgroundColor(const Color4& color) {
            AbstractShaderProgram::setUniform(_backgroundColorUniform, color);
            return *this;
        }

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * @see @ref setBackgroundColor()
         */
        Vector& setColor(const Color4& color) {
            AbstractShaderProgram::setUniform(_colorUniform, color);
            return *this;
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Overloads to remove WTF-factor from method chaining order */
        Vector<dimensions>& setVectorTexture(Texture2D& texture) {
            AbstractVector<dimensions>::setVectorTexture(texture);
            return *this;
        }
        #endif

    private:
        Int _transformationProjectionMatrixUniform{0},
            _backgroundColorUniform{1},
            _colorUniform{2};
};

/** @brief Two-dimensional vector shader */
typedef Vector<2> Vector2D;

/** @brief Three-dimensional vector shader */
typedef Vector<3> Vector3D;

}}

#endif
