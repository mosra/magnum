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

#include "AbstractTexture.h"

#include "Buffer.h"
#include "BufferImage.h"
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
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::GetLevelParameterivImplementation AbstractTexture::getLevelParameterivImplementation =
    &AbstractTexture::getLevelParameterImplementationDefault;
#endif
AbstractTexture::MipmapImplementation AbstractTexture::mipmapImplementation =
    &AbstractTexture::mipmapImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::Storage1DImplementation AbstractTexture::storage1DImplementation =
    &AbstractTexture::storageImplementationDefault;
#endif
AbstractTexture::Storage2DImplementation AbstractTexture::storage2DImplementation =
    &AbstractTexture::storageImplementationDefault;
AbstractTexture::Storage3DImplementation AbstractTexture::storage3DImplementation =
    &AbstractTexture::storageImplementationDefault;
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

AbstractTexture::InvalidateImplementation AbstractTexture::invalidateImplementation = &AbstractTexture::invalidateImplementationNoOp;
AbstractTexture::InvalidateSubImplementation AbstractTexture::invalidateSubImplementation = &AbstractTexture::invalidateSubImplementationNoOp;

#ifndef DOXYGEN_GENERATING_OUTPUT

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
#define filter_or(filter, mipmap) \
    (static_cast<GLint>(AbstractTexture::Filter::filter)|static_cast<GLint>(AbstractTexture::Mipmap::mipmap))
static_assert((filter_or(Nearest, Base) == GL_NEAREST) &&
              (filter_or(Nearest, Nearest) == GL_NEAREST_MIPMAP_NEAREST) &&
              (filter_or(Nearest, Linear) == GL_NEAREST_MIPMAP_LINEAR) &&
              (filter_or(Linear, Base) == GL_LINEAR) &&
              (filter_or(Linear, Nearest) == GL_LINEAR_MIPMAP_NEAREST) &&
              (filter_or(Linear, Linear) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#undef filter_or
#endif

Int AbstractTexture::maxSupportedLayerCount() {
    return Context::current()->state()->texture->maxSupportedLayerCount;
}

Float AbstractTexture::maxSupportedAnisotropy() {
    GLfloat& value = Context::current()->state()->texture->maxSupportedAnisotropy;

    /** @todo Re-enable when extension header is available */
    #ifndef MAGNUM_TARGET_GLES
    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
    #endif

    return value;
}

void AbstractTexture::destroy() {
    /* Moved out */
    if(!_id) return;

    /* Remove all bindings */
    for(GLuint& binding: Context::current()->state()->texture->bindings)
        if(binding == _id) binding = 0;

    glDeleteTextures(1, &_id);
}

void AbstractTexture::move() {
    _id = 0;
}

AbstractTexture::~AbstractTexture() { destroy(); }

AbstractTexture::AbstractTexture(AbstractTexture&& other): _target(other._target), _id(other._id) {
    other.move();
}

AbstractTexture& AbstractTexture::operator=(AbstractTexture&& other) {
    destroy();

    _target = other._target;
    _id = other._id;

    other.move();
    return *this;
}

void AbstractTexture::bind(Int layer) {
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
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE || mipmap == Mipmap::Base, "AbstractTexture: rectangle textures cannot have mipmaps", this);
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
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDSA;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSA;
        storage1DImplementation = &AbstractTexture::storageImplementationDSA;
        storage2DImplementation = &AbstractTexture::storageImplementationDSA;
        storage3DImplementation = &AbstractTexture::storageImplementationDSA;
        image1DImplementation = &AbstractTexture::imageImplementationDSA;
        image2DImplementation = &AbstractTexture::imageImplementationDSA;
        image3DImplementation = &AbstractTexture::imageImplementationDSA;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage2DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDSA;
    }

    if(context->isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>()) {
        Debug() << "AbstractTexture: using" << Extensions::GL::ARB::invalidate_subdata::string() << "features";

        invalidateImplementation = &AbstractTexture::invalidateImplementationARB;
        invalidateSubImplementation = &AbstractTexture::invalidateSubImplementationARB;
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
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getLevelParameterImplementationDefault(GLenum target, GLint level, GLenum parameter, GLint* values) {
    bindInternal();
    glGetTexLevelParameteriv(target, level, parameter, values);
}

void AbstractTexture::getLevelParameterImplementationDSA(GLenum target, GLint level, GLenum parameter, GLint* values) {
    glGetTextureLevelParameterivEXT(_id, target, level, parameter, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDefault(GLenum target, GLsizei levels, AbstractTexture::InternalFormat internalFormat, const Math::Vector< 1, GLsizei >& size) {
    bindInternal();
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage1D(target, levels, GLenum(internalFormat), size[0]);
    #else
    //glTexStorage2DEXT(target, levels, GLenum(internalFormat), size.x(), size.y());
    static_cast<void>(target);
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    #endif
}

void AbstractTexture::storageImplementationDSA(GLenum target, GLsizei levels, AbstractTexture::InternalFormat internalFormat, const Math::Vector< 1, GLsizei >& size) {
    glTextureStorage1DEXT(_id, target, levels, GLenum(internalFormat), size[0]);
}
#endif

void AbstractTexture::storageImplementationDefault(GLenum target, GLsizei levels, AbstractTexture::InternalFormat internalFormat, const Vector2i& size) {
    bindInternal();
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage2D(target, levels, GLenum(internalFormat), size.x(), size.y());
    #else
    //glTexStorage2DEXT(target, levels, GLenum(internalFormat), size.x(), size.y());
    static_cast<void>(target);
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(GLenum target, GLsizei levels, AbstractTexture::InternalFormat internalFormat, const Vector2i& size) {
    glTextureStorage2DEXT(_id, target, levels, GLenum(internalFormat), size.x(), size.y());
}
#endif

void AbstractTexture::storageImplementationDefault(GLenum target, GLsizei levels, AbstractTexture::InternalFormat internalFormat, const Vector3i& size) {
    bindInternal();
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage3D(target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    #else
    //glTexStorage3DEXT(target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    static_cast<void>(target);
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(GLenum target, GLsizei levels, AbstractTexture::InternalFormat internalFormat, const Vector3i& size) {
    glTextureStorage3DEXT(_id, target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
}

void AbstractTexture::imageImplementationDefault(GLenum target, GLint level, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexImage1D(target, level, static_cast<GLint>(internalFormat), size[0], 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

void AbstractTexture::imageImplementationDSA(GLenum target, GLint level, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureImage1DEXT(_id, target, level, GLint(internalFormat), size[0], 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::imageImplementationDefault(GLenum target, GLint level, InternalFormat internalFormat, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexImage2D(target, level, GLint(internalFormat), size.x(), size.y(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDSA(GLenum target, GLint level, InternalFormat internalFormat, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureImage2DEXT(_id, target, level, GLint(internalFormat), size.x(), size.y(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::imageImplementationDefault(GLenum target, GLint level, InternalFormat internalFormat, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    /** @todo Get some extension wrangler instead to avoid linker errors to glTexImage3D() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexImage3D(target, level, GLint(internalFormat), size.x(), size.y(), size.z(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    #else
    static_cast<void>(target);
    static_cast<void>(level);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDSA(GLenum target, GLint level, InternalFormat internalFormat, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureImage3DEXT(_id, target, level, GLint(internalFormat), size.x(), size.y(), size.z(), 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDefault(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexSubImage1D(target, level, offset[0], size[0], static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

void AbstractTexture::subImageImplementationDSA(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureSubImage1DEXT(_id, target, level, offset[0], size[0], static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    glTexSubImage2D(target, level, offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureSubImage2DEXT(_id, target, level, offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    bindInternal();
    /** @todo Get some extension wrangler instead to avoid linker errors to glTexSubImage3D() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexSubImage3D(target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    #else
    static_cast<void>(target);
    static_cast<void>(level);
    static_cast<void>(offset);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data) {
    glTextureSubImage3DEXT(_id, target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}
#endif

void AbstractTexture::invalidateImplementationNoOp(GLint) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateImplementationARB(GLint level) {
    glInvalidateTexImage(_id, level);
}
#endif

void AbstractTexture::invalidateSubImplementationNoOp(GLint, const Vector3i&, const Vector3i&) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateSubImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size) {
    glInvalidateTexSubImage(_id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z());
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES2
namespace Implementation {
    template<UnsignedInt dimensions> const GLvoid* ImageHelper<BufferImage<dimensions>>::dataOrPixelUnpackBuffer(BufferImage<dimensions>* image) {
        image->buffer()->bind(Buffer::Target::PixelUnpack);
        return nullptr;
    }

    template struct ImageHelper<BufferImage1D>;
    template struct ImageHelper<BufferImage2D>;
    template struct ImageHelper<BufferImage3D>;
}
#endif

#ifndef MAGNUM_TARGET_GLES
Math::Vector<1, GLint> AbstractTexture::DataHelper<1>::imageSize(AbstractTexture* texture, GLenum target, GLint level) {
    Math::Vector<1, GLint> value;
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_WIDTH, &value[0]);
    return value;
}

Vector2i AbstractTexture::DataHelper<2>::imageSize(AbstractTexture* texture, GLenum target, GLint level) {
    Vector2i value;
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_WIDTH, &value[0]);
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_HEIGHT, &value[1]);
    return value;
}

Vector3i AbstractTexture::DataHelper<3>::imageSize(AbstractTexture* texture, GLenum target, GLint level) {
    Vector3i value;
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_WIDTH, &value[0]);
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_HEIGHT, &value[1]);
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_DEPTH, &value[2]);
    return value;
}
#endif

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
