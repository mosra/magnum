#ifndef Magnum_Shaders_Phong_h
#define Magnum_Shaders_Phong_h
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
 * @brief Class @ref Magnum::Shaders::Phong
 */

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong shader

Uses ambient, diffuse and specular color or texture. For colored mesh you need
to provide @ref Position and @ref Normal attributes in your triangle mesh and
call at least @ref setTransformationMatrix(), @ref setNormalMatrix(),
@ref setProjectionMatrix(), @ref setDiffuseColor() and @ref setLightPosition().

If you want to use textures, you need to provide also @ref TextureCoordinates
attribute. Pass appropriate @ref Flags to constructor and then at render time
don't forget to also call appropriate subset of @ref setAmbientTexture(),
@ref setDiffuseTexture() and @ref setSpecularTexture(). The texture is
multipled by the color, which is by default set to fully opaque white for
enabled textures.

@image html shaders-phong.png
@image latex shaders-phong.png

## Example usage

### Colored mesh

Common mesh setup:
@code
struct Vertex {
    Vector3 position;
    Vector3 normal;
};
Vertex data[] = { ... };

Buffer vertices;
vertices.setData(data, BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::Phong::Position{},
    Shaders::Phong::Normal{});
@endcode

Common rendering setup:
@code
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::Phong shader;
shader.setDiffuseColor(Color4::fromHSV(216.0_degf, 0.85f, 1.0f))
    .setShininess(200.0f)
    .setLightPosition({5.0f, 5.0f, 7.0f})
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.rotation())
    .setProjectionMatrix(projectionMatrix);

mesh.draw(shader);
@endcode

### Diffuse and specular texture

Common mesh setup:
@code
struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoordinates;
};
Vertex data[] = { ... };

Buffer vertices;
vertices.setData(data, BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::Phong::Position{},
    Shaders::Phong::Normal{},
    Shaders::Phong::TextureCoordinates{});
@endcode

Common rendering setup:
@code
Matrix4 transformationMatrix, projectionMatrix;
Texture2D diffuseTexture, specularTexture;

Shaders::Phong shader{Shaders::Phong::DiffuseTexture|
                      Shaders::Phong::SpecularTexture};
shader.setTextures(nullptr, &diffuseTexture, &specularTexture)
    .setLightPosition({5.0f, 5.0f, 7.0f})
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.rotation())
    .setProjectionMatrix(projectionMatrix);

mesh.draw(shader);
@endcode

### Alpha-masked drawing

For general alpha-masked drawing you need to provide ambient texture with alpha
channel and set alpha channel of diffuse/specular color to `0.0f` so only
ambient alpha will be taken into account. If you have diffuse texture combined
with the alpha mask, you can use that texture for both ambient and diffuse
part and then separate the alpha like this:
@code
Shaders::Phong shader{Shaders::Phong::AmbientTexture|
                      Shaders::Phong::DiffuseTexture};
shader.setTextures(&diffuseAlphaTexture, &diffuseAlphaTexture, nullptr)
    .setAmbientColor({0.0f, 0.0f, 0.0f, 1.0f})
    .setDiffuseColor(Color4{diffuseRgb, 0.0f})
    .setSpecularColor(Color4{specularRgb, 0.0f});
@endcode

@see @ref shaders
*/
class MAGNUM_SHADERS_EXPORT Phong: public AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute", @ref Vector3.
         */
        typedef Generic3D::Position Position;

        /**
         * @brief Normal direction
         *
         * @ref shaders-generic "Generic attribute", @ref Vector3.
         */
        typedef Generic3D::Normal Normal;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute", @ref Vector2, used only if
         * at least one of @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture
         * and @ref Flag::SpecularTexture is set.
         */
        typedef Generic3D::TextureCoordinates TextureCoordinates;

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            AmbientTexture = 1 << 0,    /**< The shader uses ambient texture instead of color */
            DiffuseTexture = 1 << 1,    /**< The shader uses diffuse texture instead of color */
            SpecularTexture = 1 << 2    /**< The shader uses specular texture instead of color */
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit Phong(Flags flags = {});

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
        explicit Phong(NoCreateT) noexcept: AbstractShaderProgram{NoCreate} {}

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set ambient color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::AmbientTexture is set, default value is
         * `{1.0f, 1.0f, 1.0f, 1.0f}` and the color will be multiplied with
         * ambient texture, otherwise default value is `{0.0f, 0.0f, 0.0f, 1.0f}`.
         * @see @ref setAmbientTexture()
         */
        Phong& setAmbientColor(const Color4& color) {
            setUniform(_ambientColorUniform, color);
            return *this;
        }

        /**
         * @brief Set ambient texture
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::AmbientTexture is set.
         * @see @ref setTextures(), @ref setAmbientColor()
         */
        Phong& setAmbientTexture(Texture2D& texture);

        /**
         * @brief Set diffuse color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::DiffuseTexture is set, default value is
         * `{1.0f, 1.0f, 1.0f, 1.0f}` and the color will be multiplied with
         * diffuse texture.
         * @see @ref setDiffuseTexture()
         */
        Phong& setDiffuseColor(const Color4& color) {
            setUniform(_diffuseColorUniform, color);
            return *this;
        }

        /**
         * @brief Set diffuse texture
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::DiffuseTexture is set.
         * @see @ref setTextures(), @ref setDiffuseColor()
         */
        Phong& setDiffuseTexture(Texture2D& texture);

        /**
         * @brief Set specular color
         * @return Reference to self (for method chaining)
         *
         * Default value is `{1.0f, 1.0f, 1.0f, 1.0f}`. Color will be
         * multiplied with specular texture if @ref Flag::SpecularTexture is
         * set.
         * @see @ref setSpecularTexture()
         */
        Phong& setSpecularColor(const Color4& color) {
            setUniform(_specularColorUniform, color);
            return *this;
        }

        /**
         * @brief Set specular texture
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::SpecularTexture is set.
         * @see @ref setTextures(), @ref setSpecularColor()
         */
        Phong& setSpecularTexture(Texture2D& texture);

        /**
         * @brief Set textures
         * @return Reference to self (for method chaining)
         *
         * A particular texture has effect only if particular texture flag from
         * @ref Phong::Flag "Flag" is set, you can use `nullptr` for the rest.
         * More efficient than setting each texture separately.
         * @see @ref setAmbientTexture(), @ref setDiffuseTexture(),
         *      @ref setSpecularTexture()
         */
        Phong& setTextures(Texture2D* ambient, Texture2D* diffuse, Texture2D* specular);

        /**
         * @brief Set shininess
         * @return Reference to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * If not set, default value is `80.0f`.
         */
        Phong& setShininess(Float shininess) {
            setUniform(_shininessUniform, shininess);
            return *this;
        }

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         */
        Phong& setTransformationMatrix(const Matrix4& matrix) {
            setUniform(_transformationMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set normal matrix
         * @return Reference to self (for method chaining)
         *
         * The matrix doesn't need to be normalized, as the renormalization
         * must be done in the shader anyway.
         */
        Phong& setNormalMatrix(const Matrix3x3& matrix) {
            setUniform(_normalMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         */
        Phong& setProjectionMatrix(const Matrix4& matrix) {
            setUniform(_projectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set light position
         * @return Reference to self (for method chaining)
         */
        Phong& setLightPosition(const Vector3& light) {
            setUniform(_lightUniform, light);
            return *this;
        }

        /**
         * @brief Set light color
         * @return Reference to self (for method chaining)
         *
         * If not set, default value is `{1.0f, 1.0f, 1.0f, 1.0f}`.
         */
        Phong& setLightColor(const Color4& color) {
            setUniform(_lightColorUniform, color);
            return *this;
        }

    private:
        Flags _flags;
        Int _transformationMatrixUniform{0},
            _projectionMatrixUniform{1},
            _normalMatrixUniform{2},
            _lightUniform{3},
            _ambientColorUniform{4},
            _diffuseColorUniform{5},
            _specularColorUniform{6},
            _lightColorUniform{7},
            _shininessUniform{8};
};

CORRADE_ENUMSET_OPERATORS(Phong::Flags)

}}

#endif
