#ifndef Magnum_Shaders_Flat_h
#define Magnum_Shaders_Flat_h
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
 * @brief Class @ref Magnum::Shaders::Flat, typedef @ref Magnum::Shaders::Flat2D, @ref Magnum::Shaders::Flat3D
 */

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class FlatFlag: UnsignedByte { Textured = 1 << 0 };
    typedef Containers::EnumSet<FlatFlag> FlatFlags;
}

/**
@brief Flat shader

Draws whole mesh with given unshaded color or texture. For colored mesh you
need to provide @ref Position attribute in your triangle mesh and call at least
@ref setTransformationProjectionMatrix() and @ref setColor().

If you want to use texture, you need to provide also @ref TextureCoordinates
attribute. Pass @ref Flag::Textured to constructor and then at render time
don't forget to set also the texture via @ref setTexture(). The texture is
multipled by the color, which is by default set to fully opaque white if
texturing is enabled.

For coloring the texture based on intensity you can use the @ref Vector shader.

@image html shaders-flat.png
@image latex shaders-flat.png

## Example usage

### Colored mesh

Common mesh setup:
@code
struct Vertex {
    Vector3 position;
};
Vertex data[] = { ... };

Buffer vertices;
vertices.setData(data, BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::Flat3D::Position{});
@endcode

Common rendering setup:
@code
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::Flat3D shader;
shader.setColor(Color3::fromHSV(216.0_degf, 0.85f, 1.0f))
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
@endcode

### Textured mesh

Common mesh setup:
@code
struct Vertex {
    Vector3 position;
    Vector2 textureCoordinates;
};
Vertex data[] = { ... };

Buffer vertices;
vertices.setData(data, BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::Flat3D::Position{},
    Shaders::Flat3D::TextureCoordinates{});
@endcode

Common rendering setup:
@code
Matrix4 transformationMatrix, projectionMatrix;
Texture2D texture;

Shaders::Flat3D shader{Shaders::Flat3D::Textured};
shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .setTexture(texture);

mesh.draw(shader);
@endcode

@see @ref shaders, @ref Flat2D, @ref Flat3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Flat: public AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute", @ref Vector2 in 2D,
         * @ref Vector3 in 3D.
         */
        typedef typename Generic<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute", @ref Vector2. Used only if
         * @ref Flag::Textured is set.
         */
        typedef typename Generic<dimensions>::TextureCoordinates TextureCoordinates;

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            Textured = 1 << 0   /**< The shader uses texture instead of color */
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        typedef Implementation::FlatFlag Flag;
        typedef Implementation::FlatFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit Flat(Flags flags = {});

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
        explicit Flat(NoCreateT) noexcept: AbstractShaderProgram{NoCreate} {}

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        Flat<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
            setUniform(_transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::Textured is set, default value is `{1.0f, 1.0f, 1.0f}`
         * and the color will be multiplied with texture.
         * @see @ref setTexture()
         */
        Flat<dimensions>& setColor(const Color4& color){
            setUniform(_colorUniform, color);
            return *this;
        }

        /**
         * @brief Set texture
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Textured is set.
         * @see @ref setColor()
         */
        Flat<dimensions>& setTexture(Texture2D& texture);

    private:
        Flags _flags;
        Int _transformationProjectionMatrixUniform{0},
            _colorUniform{1};
};

/** @brief 2D flat shader */
typedef Flat<2> Flat2D;

/** @brief 3D flat shader */
typedef Flat<3> Flat3D;

CORRADE_ENUMSET_OPERATORS(Implementation::FlatFlags)

}}

#endif
