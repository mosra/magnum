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

#include "AbstractTexture.h"

#include "Context.h"
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
#define filter_or(filter, mipmap) \
    (static_cast<GLint>(AbstractTexture::Filter::filter)|static_cast<GLint>(AbstractTexture::Mipmap::mipmap))
static_assert((filter_or(NearestNeighbor, BaseLevel) == GL_NEAREST) &&
              (filter_or(NearestNeighbor, NearestLevel) == GL_NEAREST_MIPMAP_NEAREST) &&
              (filter_or(NearestNeighbor, LinearInterpolation) == GL_NEAREST_MIPMAP_LINEAR) &&
              (filter_or(LinearInterpolation, BaseLevel) == GL_LINEAR) &&
              (filter_or(LinearInterpolation, NearestLevel) == GL_LINEAR_MIPMAP_NEAREST) &&
              (filter_or(LinearInterpolation, LinearInterpolation) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#undef filter_or
#endif

GLint AbstractTexture::maxSupportedLayerCount() {
    return Context::current()->state()->texture->maxSupportedLayerCount;
}

#ifndef MAGNUM_TARGET_GLES
GLfloat AbstractTexture::maxSupportedAnisotropy() {
    GLfloat& value = Context::current()->state()->texture->maxSupportedAnisotropy;

    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);

    return value;
}
#endif

AbstractTexture::~AbstractTexture() {
    /* Remove all bindings */
    for(GLuint& binding: Context::current()->state()->texture->bindings)
        if(binding == _id) binding = 0;

    glDeleteTextures(1, &_id);
}

void AbstractTexture::bind(GLint layer) {
    Implementation::TextureState* const textureState = Context::current()->state()->texture;

    /* If already bound in given layer, nothing to do */
    if(textureState->bindings[layer] == _id) return;

    /* Change to given layer, if not already there */
    if(textureState->currentLayer != layer)
        glActiveTexture(GL_TEXTURE0 + (textureState->currentLayer = layer));

    /* Bind the texture to the layer */
    glBindTexture(_target, (textureState->bindings[layer] = _id));
}

AbstractTexture* AbstractTexture::setMinificationFilter(Filter filter, Mipmap mipmap) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE || mipmap == Mipmap::BaseLevel, "AbstractTexture: rectangle textures cannot have mipmaps", this);
    #endif

    bindInternal();
    glTexParameteri(_target, GL_TEXTURE_MIN_FILTER,
        static_cast<GLint>(filter)|static_cast<GLint>(mipmap));
    return this;
}

AbstractTexture* AbstractTexture::generateMipmap() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE, "AbstractTexture: rectangle textures cannot have mipmaps", this);
    #endif

    bindInternal();
    glGenerateMipmap(_target);
    return this;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractTexture::bindInternal() {
    Implementation::TextureState* const textureState = Context::current()->state()->texture;

    /* If the texture is already bound in current layer, nothing to do */
    if(textureState->bindings[textureState->currentLayer] == _id)
        return;

    /* Set internal layer as active if not already */
    const GLint internalLayer = textureState->maxSupportedLayerCount-1;
    if(textureState->currentLayer != internalLayer)
        glActiveTexture(GL_TEXTURE0 + (textureState->currentLayer = internalLayer));

    /* Bind the texture to internal layer, if not already */
    if(textureState->bindings[internalLayer] != _id)
        glBindTexture(_target, (textureState->bindings[internalLayer] = _id));
}
#endif

void AbstractTexture::initializeContextBasedFunctionality(Context* context) {
    Implementation::TextureState* const textureState = context->state()->texture;
    GLint& value = textureState->maxSupportedLayerCount;

    /* Get the value and resize bindings array */
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
    textureState->bindings.resize(value);
}

AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components components, AbstractTexture::ComponentType type) {
    #ifndef MAGNUM_TARGET_GLES
    #define internalFormatSwitch(c) switch(type) {                          \
        case ComponentType::UnsignedByte:                                   \
            internalFormat = GL_##c##8UI; break;                            \
        case ComponentType::Byte:                                           \
            internalFormat = GL_##c##8I; break;                             \
        case ComponentType::UnsignedShort:                                  \
            internalFormat = GL_##c##16UI; break;                           \
        case ComponentType::Short:                                          \
            internalFormat = GL_##c##16I; break;                            \
        case ComponentType::UnsignedInt:                                    \
            internalFormat = GL_##c##32UI; break;                           \
        case ComponentType::Int:                                            \
            internalFormat = GL_##c##32I; break;                            \
        case ComponentType::Half:                                           \
            internalFormat = GL_##c##16F; break;                            \
        case ComponentType::Float:                                          \
            internalFormat = GL_##c##32F; break;                            \
        case ComponentType::NormalizedUnsignedByte:                         \
            internalFormat = GL_##c##8; break;                              \
        case ComponentType::NormalizedByte:                                 \
            internalFormat = GL_##c##8_SNORM; break;                        \
        case ComponentType::NormalizedUnsignedShort:                        \
            internalFormat = GL_##c##16; break;                             \
        case ComponentType::NormalizedShort:                                \
            internalFormat = GL_##c##16_SNORM; break;                       \
    }
    #else
    #define internalFormatSwitch(c) switch(type) {                          \
        case ComponentType::UnsignedByte:                                   \
            internalFormat = GL_##c##8UI; break;                            \
        case ComponentType::Byte:                                           \
            internalFormat = GL_##c##8I; break;                             \
        case ComponentType::UnsignedShort:                                  \
            internalFormat = GL_##c##16UI; break;                           \
        case ComponentType::Short:                                          \
            internalFormat = GL_##c##16I; break;                            \
        case ComponentType::UnsignedInt:                                    \
            internalFormat = GL_##c##32UI; break;                           \
        case ComponentType::Int:                                            \
            internalFormat = GL_##c##32I; break;                            \
        case ComponentType::Half:                                           \
            internalFormat = GL_##c##16F; break;                            \
        case ComponentType::Float:                                          \
            internalFormat = GL_##c##32F; break;                            \
        case ComponentType::NormalizedUnsignedByte:                         \
            internalFormat = GL_##c##8; break;                              \
        case ComponentType::NormalizedByte:                                 \
            internalFormat = GL_##c##8_SNORM; break;                        \
    }
    #endif
    if(components == Components::Red)
        internalFormatSwitch(R)
    else if(components == Components::RedGreen)
        internalFormatSwitch(RG)
    else if(components == Components::RGB)
        internalFormatSwitch(RGB)
    else if(components == Components::RGBA)
        internalFormatSwitch(RGBA)
    #undef internalFormatSwitch
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractTexture::DataHelper<2>::setWrapping(AbstractTexture* texture, const Math::Vector<2, Wrapping>& wrapping) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(texture->_target != GL_TEXTURE_RECTANGLE || ((wrapping[0] == Wrapping::ClampToEdge || wrapping[0] == Wrapping::ClampToBorder) && (wrapping[0] == Wrapping::ClampToEdge || wrapping[1] == Wrapping::ClampToEdge)), "AbstractTexture: rectangle texture wrapping must either clamp to border or to edge", );
    #endif

    texture->bindInternal();
    glTexParameteri(texture->_target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    glTexParameteri(texture->_target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[1]));
}

void AbstractTexture::DataHelper<3>::setWrapping(AbstractTexture* texture, const Math::Vector<3, Wrapping>& wrapping) {
    texture->bindInternal();
    glTexParameteri(texture->_target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    glTexParameteri(texture->_target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[1]));
    #ifndef MAGNUM_TARGET_GLES
    glTexParameteri(texture->_target, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping[2]));
    #endif
}
#endif

}
