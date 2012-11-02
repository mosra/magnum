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
#include "Extensions.h"
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

AbstractTexture::BindImplementation AbstractTexture::bindImplementation =
    &AbstractTexture::bindImplementationDefault;
AbstractTexture::ParameteriImplementation AbstractTexture::parameteriImplementation =
    &AbstractTexture::parameterImplementationDefault;
AbstractTexture::ParameterfImplementation AbstractTexture::parameterfImplementation =
    &AbstractTexture::parameterImplementationDefault;
AbstractTexture::ParameterfvImplementation AbstractTexture::parameterfvImplementation =
    &AbstractTexture::parameterImplementationDefault;
AbstractTexture::MipmapImplementation AbstractTexture::mipmapImplementation =
    &AbstractTexture::mipmapImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::Image1DImplementation AbstractTexture::image1DImplementation =
    &AbstractTexture::imageImplementationDefault;
#endif
AbstractTexture::Image2DImplementation AbstractTexture::image2DImplementation =
    &AbstractTexture::imageImplementationDefault;
AbstractTexture::Image3DImplementation AbstractTexture::image3DImplementation =
    &AbstractTexture::imageImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::SubImage1DImplementation AbstractTexture::subImage1DImplementation =
    &AbstractTexture::subImageImplementationDefault;
#endif
AbstractTexture::SubImage2DImplementation AbstractTexture::subImage2DImplementation =
    &AbstractTexture::subImageImplementationDefault;
AbstractTexture::SubImage3DImplementation AbstractTexture::subImage3DImplementation =
    &AbstractTexture::subImageImplementationDefault;

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

GLfloat AbstractTexture::maxSupportedAnisotropy() {
    GLfloat& value = Context::current()->state()->texture->maxSupportedAnisotropy;

    /** @todo Re-enable when extension header is available */
    #ifndef MAGNUM_TARGET_GLES
    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
    #endif

    return value;
}

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

    (this->*bindImplementation)(layer);
}

void AbstractTexture::bindImplementationDefault(GLint layer) {
    Implementation::TextureState* const textureState = Context::current()->state()->texture;

    /* Change to given layer, if not already there */
    if(textureState->currentLayer != layer)
        glActiveTexture(GL_TEXTURE0 + (textureState->currentLayer = layer));

    /* Bind the texture to the layer */
    glBindTexture(_target, (textureState->bindings[layer] = _id));
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::bindImplementationDSA(GLint layer) {
    glBindMultiTextureEXT(GL_TEXTURE0 + layer, _target, (Context::current()->state()->texture->bindings[layer] = _id));
}
#endif

AbstractTexture* AbstractTexture::setMinificationFilter(Filter filter, Mipmap mipmap) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE || mipmap == Mipmap::BaseLevel, "AbstractTexture: rectangle textures cannot have mipmaps", this);
    #endif

    (this->*parameteriImplementation)(GL_TEXTURE_MIN_FILTER,
        static_cast<GLint>(filter)|static_cast<GLint>(mipmap));
    return this;
}

AbstractTexture* AbstractTexture::generateMipmap() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE, "AbstractTexture: rectangle textures cannot have mipmaps", this);
    #endif

    (this->*mipmapImplementation)();
    return this;
}

void AbstractTexture::mipmapImplementationDefault() {
    bindInternal();
    glGenerateMipmap(_target);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::mipmapImplementationDSA() {
    glGenerateTextureMipmapEXT(_id, _target);
}
#endif

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

    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractTexture: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        bindImplementation = &AbstractTexture::bindImplementationDSA;
        parameteriImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDSA;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSA;
        image1DImplementation = &AbstractTexture::imageImplementationDSA;
        image2DImplementation = &AbstractTexture::imageImplementationDSA;
        image3DImplementation = &AbstractTexture::imageImplementationDSA;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage2DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDSA;
    }
    #endif
}

void AbstractTexture::parameterImplementationDefault(GLenum parameter, GLint value) {
    bindInternal();
    glTexParameteri(_target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(GLenum parameter, GLint value) {
    glTextureParameteriEXT(_id, _target, parameter, value);
}
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, GLfloat value) {
    bindInternal();
    glTexParameterf(_target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(GLenum parameter, GLfloat value) {
    glTextureParameterfEXT(_id, _target, parameter, value);
}
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, const GLfloat* values) {
    bindInternal();
    glTexParameterfv(_target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(GLenum parameter, const GLfloat* values) {
    glTextureParameterfvEXT(_id, _target, parameter, values);
}

void AbstractTexture::imageImplementationDefault(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexImage1D(target, mipLevel, static_cast<GLint>(internalFormat), size[0], 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

void AbstractTexture::imageImplementationDSA(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureImage1DEXT(_id, target, mipLevel, GLint(internalFormat), size[0], 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::imageImplementationDefault(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexImage2D(target, mipLevel, GLint(internalFormat), size.x(), size.y(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDSA(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureImage2DEXT(_id, target, mipLevel, GLint(internalFormat), size.x(), size.y(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::imageImplementationDefault(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    /** @todo Get some extension wrangler instead to avoid linker errors to glTexImage3D() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexImage3D(target, mipLevel, GLint(internalFormat), size.x(), size.y(), size.z(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    #else
    static_cast<void>(target);
    static_cast<void>(mipLevel);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDSA(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureImage3DEXT(_id, target, mipLevel, GLint(internalFormat), size.x(), size.y(), size.z(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDefault(GLenum target, GLint mipLevel, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexSubImage1D(target, mipLevel, offset[0], size[0], static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

void AbstractTexture::subImageImplementationDSA(GLenum target, GLint mipLevel, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureSubImage1DEXT(_id, target, mipLevel, offset[0], size[0], static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLenum target, GLint mipLevel, const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexSubImage2D(target, mipLevel, offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(GLenum target, GLint mipLevel, const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureSubImage2DEXT(_id, target, mipLevel, offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLenum target, GLint mipLevel, const Vector3i& offset, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    /** @todo Get some extension wrangler instead to avoid linker errors to glTexSubImage3D() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexSubImage3D(target, mipLevel, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    #else
    static_cast<void>(target);
    static_cast<void>(mipLevel);
    static_cast<void>(offset);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(GLenum target, GLint mipLevel, const Vector3i& offset, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureSubImage3DEXT(_id, target, mipLevel, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractTexture::DataHelper<2>::setWrapping(AbstractTexture* texture, const Array2D<Wrapping>& wrapping) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(texture->_target != GL_TEXTURE_RECTANGLE || ((wrapping.x() == Wrapping::ClampToEdge || wrapping.x() == Wrapping::ClampToBorder) && (wrapping.y() == Wrapping::ClampToEdge || wrapping.y() == Wrapping::ClampToEdge)), "AbstractTexture: rectangle texture wrapping must either clamp to border or to edge", );
    #endif

    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.x()));
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping.y()));
}

void AbstractTexture::DataHelper<3>::setWrapping(AbstractTexture* texture, const Array3D<Wrapping>& wrapping) {
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.x()));
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping.y()));
    #ifndef MAGNUM_TARGET_GLES
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping.z()));
    #endif
}
#endif

}
