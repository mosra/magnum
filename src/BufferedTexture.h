#ifndef Magnum_BufferedTexture_h
#define Magnum_BufferedTexture_h
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
 * @brief Class Magnum::BufferedTexture
 */

#include "Renderbuffer.h"
#include "Buffer.h"

namespace Magnum {

/**
@brief Buffered texture

This texture is, unlike classic textures such as Texture or CubeMapTexture,
used as simple data source, without any unneccessary interpolation and
wrapping methods. Texture data are stored in buffer and after binding the
buffer to the texture using setBuffer(), you can fill the buffer at any time
using data setting functions in Buffer itself.

When using buffered texture in the shader, use `samplerBuffer` and fetch the
data using integer coordinates in `texelFetch()`.
*/
class BufferedTexture {
    BufferedTexture(const BufferedTexture& other) = delete;
    BufferedTexture(BufferedTexture&& other) = delete;
    BufferedTexture& operator=(const BufferedTexture& other) = delete;
    BufferedTexture& operator=(BufferedTexture&& other) = delete;

    public:
        /** @copydoc Renderbuffer::Components */
        typedef Renderbuffer::Components Components;

        /** @copydoc Renderbuffer::ComponentType */
        typedef Renderbuffer::ComponentType ComponentType;

        /**
         * @brief Constructor
         * @param layer     %Texture layer (number between 0 and 31)
         */
        inline BufferedTexture(GLint layer = 0): _layer(layer) {
            glActiveTexture(GL_TEXTURE0 + layer);
            glGenTextures(1, &texture);
        }

        /** @copydoc AbstractTexture::~AbstractTexture() */
        inline virtual ~BufferedTexture() {
            glDeleteTextures(1, &texture);
        }

        /** @copydoc AbstractTexture::layer() */
        inline GLint layer() const { return _layer; }

        /** @copydoc AbstractTexture::bind() */
        inline void bind() const {
            glActiveTexture(GL_TEXTURE0 + _layer);
            glBindTexture(GL_TEXTURE_BUFFER, texture);
        }

        /**
         * @brief Set texture buffer
         * @param components    Component count
         * @param type          Data type per component
         * @param buffer        %Buffer with data
         *
         * Binds given buffer to this texture. The buffer itself can be then
         * filled with data of proper format at any time using Buffer own data
         * setting functions.
         */
        void setBuffer(Components components, ComponentType type, Buffer* buffer) {
            bind();
            glTexBuffer(GL_TEXTURE_BUFFER, components|type, buffer->id());
        }

    private:
        GLint _layer;
        GLuint texture;
};

}

#endif
