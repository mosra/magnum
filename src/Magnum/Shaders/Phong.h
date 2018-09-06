#ifndef Magnum_Shaders_Phong_h
#define Magnum_Shaders_Phong_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong shader

Uses ambient, diffuse and specular color or texture. For colored mesh you need
to provide the @ref Position and @ref Normal attributes in your triangle mesh.
By default, the shader renders the mesh with a white color in an identity
transformation. Use @ref setTransformationMatrix(), @ref setNormalMatrix(),
@ref setProjectionMatrix(), @ref setLightPosition() and others to configure
the shader.

If you want to use textures, you need to provide also @ref TextureCoordinates
attribute. Pass appropriate @ref Flags to constructor and then at render time
don't forget to also call appropriate subset of @ref bindAmbientTexture(),
@ref bindDiffuseTexture() and @ref bindSpecularTexture() (or the combined
@ref bindTextures()). The texture is multipled by the color, which is by
default set to fully opaque white for enabled textures.

@image html shaders-phong.png

@section Shaders-Phong-usage Example usage

@subsection Shaders-Phong-usage-colored Colored mesh

Common mesh setup:

@snippet MagnumShaders.cpp Phong-usage-colored1

Common rendering setup:

@snippet MagnumShaders.cpp Phong-usage-colored2

@subsection Shaders-Phong-usage-texture Diffuse and specular texture

Common mesh setup:

@snippet MagnumShaders.cpp Phong-usage-texture1

Common rendering setup:

@snippet MagnumShaders.cpp Phong-usage-texture2

@subsection Shaders-Phong-usage-alpha Alpha blending and masking

Enable @ref Flag::AlphaMask and tune @ref setAlphaMask() for simple
binary alpha-masked drawing that doesn't require depth sorting or blending
enabled. Note that this feature is implemented using the GLSL @glsl discard @ce
operation which is known to have considerable performance impact on some
platforms. With proper depth sorting and blending you'll usually get much
better performance and output quality.

For general alpha-masked drawing you need to provide ambient texture with alpha
channel and set alpha channel of diffuse/specular color to @cpp 0.0f @ce so
only ambient alpha will be taken into account. If you have diffuse texture
combined with the alpha mask, you can use that texture for both ambient and
diffuse part and then separate the alpha like this:

@snippet MagnumShaders.cpp Phong-usage-alpha

@see @ref shaders
*/
class MAGNUM_SHADERS_EXPORT Phong: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef Generic3D::Position Position;

        /**
         * @brief Normal direction
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef Generic3D::Normal Normal;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2", used only if at least one of
         * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture and
         * @ref Flag::SpecularTexture is set.
         */
        typedef Generic3D::TextureCoordinates TextureCoordinates;

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            /**
             * Multiply ambient color with a texture.
             * @see @ref setAmbientColor(), @ref setAmbientTexture()
             */
            AmbientTexture = 1 << 0,

            /**
             * Multiply diffuse color with a texture.
             * @see @ref setDiffuseColor(), @ref setDiffuseTexture()
             */
            DiffuseTexture = 1 << 1,

            /**
             * Multiply specular color with a texture.
             * @see @ref setSpecularColor(), @ref setSpecularTexture()
             */
            SpecularTexture = 1 << 2,

            /**
             * Enable alpha masking. If the combined fragment color has an
             * alpha less than the value specified with @ref setAlphaMask(),
             * given fragment is discarded.
             *
             * This uses the @glsl discard @ce operation which is known to have
             * considerable performance impact on some platforms. While useful
             * for cheap alpha masking that doesn't require depth sorting,
             * with proper depth sorting and blending you'll usually get much
             * better performance and output quality.
             */
            AlphaMask = 1 << 3
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Flags
         * @param lightCount    Count of light sources
         */
        explicit Phong(Flags flags = {}, UnsignedInt lightCount = 1);

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
        explicit Phong(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        Phong(const Phong&) = delete;

        /** @brief Move constructor */
        Phong(Phong&&) noexcept = default;

        /** @brief Copying is not allowed */
        Phong& operator=(const Phong&) = delete;

        /** @brief Move assignment */
        Phong& operator=(Phong&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /** @brief Light count */
        UnsignedInt lightCount() const { return _lightCount; }

        /**
         * @brief Set ambient color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::AmbientTexture is set, default value is
         * @cpp 0xffffffff_rgbaf @ce and the color will be multiplied with
         * ambient texture, otherwise default value is @cpp 0x00000000_rgbaf @ce.
         * @see @ref bindAmbientTexture()
         */
        Phong& setAmbientColor(const Color4& color) {
            setUniform(_ambientColorUniform, color);
            return *this;
        }

        /**
         * @brief Bind an ambient texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AmbientTexture
         * enabled.
         * @see @ref bindTextures(), @ref setAmbientColor()
         */
        Phong& bindAmbientTexture(GL::Texture2D& texture);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief bindAmbientTexture()
         * @deprecated Use @ref bindAmbientTexture() instead.
         */
        CORRADE_DEPRECATED("use bindAmbientTexture() instead") Phong& setAmbientTexture(GL::Texture2D& texture) {
            return bindAmbientTexture(texture);
        }
        #endif

        /**
         * @brief Set diffuse color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         * @see @ref bindDiffuseTexture()
         */
        Phong& setDiffuseColor(const Color4& color) {
            setUniform(_diffuseColorUniform, color);
            return *this;
        }

        /**
         * @brief Bind a diffuse texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::DiffuseTexture
         * enabled.
         * @see @ref bindTextures(), @ref setDiffuseColor()
         */
        Phong& bindDiffuseTexture(GL::Texture2D& texture);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief bindDiffuseTexture()
         * @deprecated Use @ref bindDiffuseTexture() instead.
         */
        CORRADE_DEPRECATED("use bindDiffuseTexture() instead") Phong& setDiffuseTexture(GL::Texture2D& texture) {
            return bindDiffuseTexture(texture);
        }
        #endif

        /**
         * @brief Set specular color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. Color will be multiplied
         * with specular texture if @ref Flag::SpecularTexture is set. If you
         * want to have a fully diffuse material, set specular color to
         * @cpp 0x000000ff_rgbaf @ce.
         * @see @ref bindSpecularTexture()
         */
        Phong& setSpecularColor(const Color4& color) {
            setUniform(_specularColorUniform, color);
            return *this;
        }

        /**
         * @brief Bind a specular texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::SpecularTexture
         * enabled.
         * @see @ref bindTextures(), @ref setSpecularColor()
         */
        Phong& bindSpecularTexture(GL::Texture2D& texture);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief bindSpecularTexture()
         * @deprecated Use @ref bindSpecularTexture() instead.
         */
        CORRADE_DEPRECATED("use bindSpecularTexture() instead") Phong& setSpecularTexture(GL::Texture2D& texture) {
            return bindSpecularTexture(texture);
        }
        #endif

        /**
         * @brief Bind textures
         * @return Reference to self (for method chaining)
         *
         * A particular texture has effect only if particular texture flag from
         * @ref Phong::Flag "Flag" is set, you can use @cpp nullptr @ce for the
         * rest. Expects that the shader was created with at least one of
         * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture or
         * @ref Flag::SpecularTexture enabled. More efficient than setting each
         * texture separately.
         * @see @ref bindAmbientTexture(), @ref bindDiffuseTexture(),
         *      @ref bindSpecularTexture()
         */
        Phong& bindTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief bindTextures()
         * @deprecated Use @ref bindTextures() instead.
         */
        CORRADE_DEPRECATED("use bindTextures() instead") Phong& setTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular) {
            return bindTextures(ambient, diffuse, specular);
        }
        #endif

        /**
         * @brief Set shininess
         * @return Reference to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * Initial value is @cpp 80.0f @ce.
         */
        Phong& setShininess(Float shininess) {
            setUniform(_shininessUniform, shininess);
            return *this;
        }

        /**
         * @brief Set alpha mask value
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AlphaMask
         * enabled. Fragments with alpha values smaller than the mask value
         * will be discarded. Initial value is @cpp 0.5f @ce. See the flag
         * documentation for further information.
         */
        Phong& setAlphaMask(Float mask);

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         *
         * You need to set also @ref setNormalMatrix() with a corresponding
         * value. Initial value is an identity matrix.
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
         * must be done in the shader anyway. You need to set also
         * @ref setTransformationMatrix() with a corresponding value. Initial
         * value is an identity matrix.
         */
        Phong& setNormalMatrix(const Matrix3x3& matrix) {
            setUniform(_normalMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix (i.e., an orthographic
         * projection of the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$
         * cube).
         */
        Phong& setProjectionMatrix(const Matrix4& matrix) {
            setUniform(_projectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set light positions
         * @return Reference to self (for method chaining)
         *
         * Initial values are zero vectors --- that will in most cases cause
         * the object to be rendered black (or in the ambient color), as the
         * lights are is inside of it. Expects that the size of the @p lights
         * array is the same as @ref lightCount().
         * @see @ref setLightPosition(UnsignedInt, const Vector3&),
         *      @ref setLightPosition(const Vector3&)
         */
        Phong& setLightPositions(Containers::ArrayView<const Vector3> lights);

        /** @overload */
        Phong& setLightPositions(std::initializer_list<Vector3> lights) {
            return setLightPositions({lights.begin(), lights.size()});
        }

        /**
         * @brief Set position for given light
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setLightPosition() updates just a single light position.
         * Expects that @p id is less than @ref lightCount().
         * @see @ref setLightPosition(const Vector3&)
         */
        Phong& setLightPosition(UnsignedInt id, const Vector3& position);

        /**
         * @brief Set light position
         * @return Reference to self (for method chaining)
         *
         * Convenience alternative to @ref setLightPositions() when there is
         * just one light.
         * @see @ref setLightPosition(UnsignedInt, const Vector3&)
         */
        Phong& setLightPosition(const Vector3& position) {
            return setLightPositions({&position, 1});
        }

        /**
         * @brief Set light colors
         * @return Reference to self (for method chaining)
         *
         * Initial values are @cpp 0xffffffff_rgbaf @ce. Expects that the size
         * of the @p colors array is the same as @ref lightCount().
         */
        Phong& setLightColors(Containers::ArrayView<const Color4> colors);

        /** @overload */
        Phong& setLightColors(std::initializer_list<Color4> colors) {
            return setLightColors({colors.begin(), colors.size()});
        }

        /**
         * @brief Set position for given light
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setLightColors() updates just a single light color.
         * Expects that @p id is less than @ref lightCount().
         * @see @ref setLightColor(const Color4&)
         */
        Phong& setLightColor(UnsignedInt id, const Color4& color);

        /**
         * @brief Set light color
         * @return Reference to self (for method chaining)
         *
         * Convenience alternative to @ref setLightColors() when there is just
         * one light.
         * @see @ref setLightColor(UnsignedInt, const Color4&)
         */
        Phong& setLightColor(const Color4& color) {
            return setLightColors({&color, 1});
        }

    private:
        Flags _flags;
        UnsignedInt _lightCount;
        Int _transformationMatrixUniform{0},
            _projectionMatrixUniform{1},
            _normalMatrixUniform{2},
            _ambientColorUniform{4},
            _diffuseColorUniform{5},
            _specularColorUniform{6},
            _shininessUniform{7},
            _alphaMaskUniform{8},
            _lightPositionsUniform{9},
            _lightColorsUniform; /* 9 + lightCount, set in the constructor */
};

/** @debugoperatorclassenum{Phong,Phong::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, Phong::Flag value);

/** @debugoperatorclassenum{Phong,Phong::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, Phong::Flags value);

CORRADE_ENUMSET_OPERATORS(Phong::Flags)

}}

#endif
