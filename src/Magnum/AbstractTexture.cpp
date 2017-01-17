/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include "AbstractTexture.h"

#include <Corrade/Containers/Array.h>

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif
#include "Magnum/Array.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

#ifndef MAGNUM_TARGET_WEBGL
#include "Implementation/DebugState.h"
#endif
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
Float AbstractTexture::maxLodBias() {
    GLfloat& value = Context::current().state().texture->maxLodBias;

    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &value);

    return value;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Int AbstractTexture::maxColorSamples() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().texture->maxColorSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &value);

    return value;
}

Int AbstractTexture::maxDepthSamples() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().texture->maxDepthSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &value);

    return value;
}

Int AbstractTexture::maxIntegerSamples() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().texture->maxIntegerSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &value);

    return value;
}
#endif

void AbstractTexture::unbind(const Int textureUnit) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* If given texture unit is already unbound, nothing to do */
    if(textureState.bindings[textureUnit].second == 0) return;

    /* Unbind the texture, reset state tracker */
    Context::current().state().texture->unbindImplementation(textureUnit);
    textureState.bindings[textureUnit] = {0, 0};
}

void AbstractTexture::unbindImplementationDefault(const GLint textureUnit) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* Activate given texture unit if not already active, update state tracker */
    if(textureState.currentTextureUnit != textureUnit)
        glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = textureUnit));

    CORRADE_INTERNAL_ASSERT(textureState.bindings[textureUnit].first != 0);
    glBindTexture(textureState.bindings[textureUnit].first, 0);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::unbindImplementationMulti(const GLint textureUnit) {
    constexpr static GLuint zero = 0;
    glBindTextures(textureUnit, 1, &zero);
}

void AbstractTexture::unbindImplementationDSA(const GLint textureUnit) {
    CORRADE_INTERNAL_ASSERT(Context::current().state().texture->bindings[textureUnit].first != 0);
    glBindTextureUnit(textureUnit, 0);
}

void AbstractTexture::unbindImplementationDSAEXT(const GLint textureUnit) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    CORRADE_INTERNAL_ASSERT(textureState.bindings[textureUnit].first != 0);
    glBindMultiTextureEXT(GL_TEXTURE0 + textureUnit, textureState.bindings[textureUnit].first, 0);
}
#endif

void AbstractTexture::unbind(const Int firstTextureUnit, const std::size_t count) {
    /* State tracker is updated in the implementations */
    Context::current().state().texture->bindMultiImplementation(firstTextureUnit, {nullptr, count});
}

/** @todoc const std::initializer_list makes Doxygen grumpy */
void AbstractTexture::bind(const Int firstTextureUnit, std::initializer_list<AbstractTexture*> textures) {
    /* State tracker is updated in the implementations */
    Context::current().state().texture->bindMultiImplementation(firstTextureUnit, {textures.begin(), textures.size()});
}

void AbstractTexture::bindImplementationFallback(const GLint firstTextureUnit, const Containers::ArrayView<AbstractTexture* const> textures) {
    for(std::size_t i = 0; i != textures.size(); ++i)
        textures && textures[i] ? textures[i]->bind(firstTextureUnit + i) : unbind(firstTextureUnit + i);
}

#ifndef MAGNUM_TARGET_GLES
/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void AbstractTexture::bindImplementationMulti(const GLint firstTextureUnit, Containers::ArrayView<AbstractTexture* const> textures) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* Create array of IDs and also update bindings in state tracker */
    /** @todo VLAs */
    Containers::Array<GLuint> ids{textures ? textures.size() : 0};
    bool different = false;
    for(std::size_t i = 0; i != textures.size(); ++i) {
        const GLuint id = textures && textures[i] ? textures[i]->_id : 0;

        if(textures) {
            if(textures[i]) textures[i]->createIfNotAlready();
            ids[i] = id;
        }

        if(textureState.bindings[firstTextureUnit + i].second != id) {
            different = true;
            textureState.bindings[firstTextureUnit + i].second = id;
        }
    }

    /* Avoid doing the binding if there is nothing different */
    if(different) glBindTextures(firstTextureUnit, textures.size(), ids);
}
#endif

#ifndef MAGNUM_TARGET_GLES
Int AbstractTexture::compressedBlockDataSize(const GLenum target, const TextureFormat format) {
    return (Context::current().state().texture->compressedBlockDataSizeImplementation)(target, format);
}

Int AbstractTexture::compressedBlockDataSizeImplementationDefault(const GLenum target, const TextureFormat format) {
    GLint value;
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_SIZE, 1, &value);
    return value;
}

Int AbstractTexture::compressedBlockDataSizeImplementationBitsWorkaround(const GLenum target, const TextureFormat format) {
    /* NVidia (358.16) reports the value in bits instead of bytes */
    return compressedBlockDataSizeImplementationDefault(target, format)/8;
}
#endif

AbstractTexture::AbstractTexture(GLenum target): _target{target}, _flags{ObjectFlag::DeleteOnDestruction} {
    (this->*Context::current().state().texture->createImplementation)();
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

void AbstractTexture::createImplementationDefault() {
    glGenTextures(1, &_id);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::createImplementationDSA() {
    glCreateTextures(_target, 1, &_id);
    _flags |= ObjectFlag::Created;
}
#endif

AbstractTexture::~AbstractTexture() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    /* Remove all bindings */
    for(auto& binding: Context::current().state().texture->bindings) {
        /* MSVC 2015 needs the parentheses around */
        if(binding.second == _id) binding = {0, 0};
    }

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Remove all image bindings */
    for(auto& binding: Context::current().state().texture->imageBindings) {
        /* MSVC 2015 needs the parentheses around */
        if(std::get<0>(binding) == _id) binding = {};
    }
    #endif

    glDeleteTextures(1, &_id);
}

void AbstractTexture::createIfNotAlready() {
    if(_flags & ObjectFlag::Created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glBindTextures() or glObjectLabel()) operate with IDs
       directly and they require the object to be created. Binding the texture
       to desired target finally creates it. Also all EXT DSA functions
       implicitly create it. */
    bindInternal();
    CORRADE_INTERNAL_ASSERT(_flags & ObjectFlag::Created);
}

#ifndef MAGNUM_TARGET_WEBGL
std::string AbstractTexture::label() {
    createIfNotAlready();
    return Context::current().state().debug->getLabelImplementation(GL_TEXTURE, _id);
}

AbstractTexture& AbstractTexture::setLabelInternal(const Containers::ArrayView<const char> label) {
    createIfNotAlready();
    Context::current().state().debug->labelImplementation(GL_TEXTURE, _id, label);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::unbindImage(const Int imageUnit) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* If already unbound in given image unit, nothing to do */
    if(std::get<0>(textureState.imageBindings[imageUnit]) == 0) return;

    /* Update state tracker, bind the texture to the unit */
    std::get<0>(textureState.imageBindings[imageUnit]) = 0;
    glBindImageTexture(imageUnit, 0, 0, false, 0, GL_READ_ONLY, GL_R8);
}

#ifndef MAGNUM_TARGET_GLES
/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void AbstractTexture::bindImagesInternal(const Int firstImageUnit, Containers::ArrayView<AbstractTexture* const> textures) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* Create array of IDs and also update bindings in state tracker */
    Containers::Array<GLuint> ids{textures ? textures.size() : 0};
    bool different = false;
    for(std::size_t i = 0; i != textures.size(); ++i) {
        const std::tuple<GLuint, GLint, GLboolean, GLint, GLenum> state = textures && textures[i] ?
            std::tuple<GLuint, GLint, GLboolean, GLint, GLenum>(textures[i]->_id, 0, true, 0, GL_READ_WRITE) :
            std::tuple<GLuint, GLint, GLboolean, GLint, GLenum>(0, 0, false, 0, GL_READ_ONLY);

        if(textures) {
            if(textures[i]) {
                textures[i]->createIfNotAlready();
            }
            ids[i] = std::get<0>(state);
        }

        if(textureState.imageBindings[firstImageUnit + i] != state) {
            different = true;
            textureState.imageBindings[firstImageUnit + i] = state;
        }
    }

    /* Avoid doing the binding if there is nothing different */
    if(different) glBindImageTextures(firstImageUnit, textures.size(), ids);
}
#endif

void AbstractTexture::bindImageInternal(const Int imageUnit, const Int level, const bool layered, const Int layer, const ImageAccess access, const ImageFormat format) {
    Implementation::TextureState& textureState = *Context::current().state().texture;
    const std::tuple<GLuint, GLint, GLboolean, GLint, GLenum> state{_id, level, layered, layer, GLenum(access)};

    /* If already bound in given texture unit, nothing to do */
    if(textureState.imageBindings[imageUnit] == state) return;

    /* Update state tracker, bind the texture to the unit */
    textureState.imageBindings[imageUnit] = state;
    glBindImageTexture(imageUnit, _id, level, layered, layer, GLenum(access), GLenum(format));
}
#endif

void AbstractTexture::bind(Int textureUnit) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* If already bound in given texture unit, nothing to do */
    if(textureState.bindings[textureUnit].second == _id) return;

    /* Update state tracker, bind the texture to the unit */
    textureState.bindings[textureUnit] = {_target, _id};
    (this->*textureState.bindImplementation)(textureUnit);
}

void AbstractTexture::bindImplementationDefault(GLint textureUnit) {
    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* Activate given texture unit if not already active, update state tracker */
    if(textureState.currentTextureUnit != textureUnit)
        glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = textureUnit));

    /* Binding the texture finally creates it */
    _flags |= ObjectFlag::Created;
    glBindTexture(_target, _id);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::bindImplementationMulti(GLint textureUnit) {
    createIfNotAlready();
    glBindTextures(textureUnit, 1, &_id);
}

void AbstractTexture::bindImplementationDSA(const GLint textureUnit) {
    glBindTextureUnit(textureUnit, _id);
}

void AbstractTexture::bindImplementationDSAEXT(GLint textureUnit) {
    _flags |= ObjectFlag::Created;
    glBindMultiTextureEXT(GL_TEXTURE0 + textureUnit, _target, _id);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setBaseLevel(Int level) {
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_BASE_LEVEL, level);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::setMaxLevel(Int level) {
    (this->*Context::current().state().texture->parameteriImplementation)(
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_MAX_LEVEL
        #else
        GL_TEXTURE_MAX_LEVEL_APPLE
        #endif
    , level);
}
#endif

void AbstractTexture::setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap) {
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_MIN_FILTER, GLint(filter)|GLint(mipmap));
}

void AbstractTexture::setMagnificationFilter(const Sampler::Filter filter) {
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_MAG_FILTER, GLint(filter));
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setMinLod(const Float lod) {
    (this->*Context::current().state().texture->parameterfImplementation)(GL_TEXTURE_MIN_LOD, lod);
}

void AbstractTexture::setMaxLod(const Float lod) {
    (this->*Context::current().state().texture->parameterfImplementation)(GL_TEXTURE_MAX_LOD, lod);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::setLodBias(const Float bias) {
    (this->*Context::current().state().texture->parameterfImplementation)(GL_TEXTURE_LOD_BIAS, bias);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
void AbstractTexture::setBorderColor(const Color4& color) {
    (this->*Context::current().state().texture->parameterfvImplementation)(
        #ifndef MAGNUM_TARGET_GLES
        GL_TEXTURE_BORDER_COLOR,
        #else
        GL_TEXTURE_BORDER_COLOR_EXT,
        #endif
        color.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setBorderColor(const Vector4ui& color) {
    (this->*Context::current().state().texture->parameterIuivImplementation)(
        #ifndef MAGNUM_TARGET_GLES
        GL_TEXTURE_BORDER_COLOR,
        #else
        GL_TEXTURE_BORDER_COLOR_EXT,
        #endif
        color.data());
}

void AbstractTexture::setBorderColor(const Vector4i& color) {
    (this->*Context::current().state().texture->parameterIivImplementation)(
        #ifndef MAGNUM_TARGET_GLES
        GL_TEXTURE_BORDER_COLOR,
        #else
        GL_TEXTURE_BORDER_COLOR_EXT,
        #endif
        color.data());
}
#endif
#endif

void AbstractTexture::setMaxAnisotropy(const Float anisotropy) {
    (this->*Context::current().state().texture->setMaxAnisotropyImplementation)(anisotropy);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractTexture::setSRGBDecode(bool decode) {
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_SRGB_DECODE_EXT,
        decode ? GL_DECODE_EXT : GL_SKIP_DECODE_EXT);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::setSwizzleInternal(const GLint r, const GLint g, const GLint b, const GLint a) {
    #ifndef MAGNUM_TARGET_GLES
    const GLint rgba[] = {r, g, b, a};
    (this->*Context::current().state().texture->parameterivImplementation)(GL_TEXTURE_SWIZZLE_RGBA, rgba);
    #else
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_SWIZZLE_R, r);
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_SWIZZLE_G, g);
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_SWIZZLE_B, b);
    (this->*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_SWIZZLE_A, a);
    #endif
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::setCompareMode(const Sampler::CompareMode mode) {
    (this->*Context::current().state().texture->parameteriImplementation)(
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_COMPARE_MODE
        #else
        GL_TEXTURE_COMPARE_MODE_EXT
        #endif
        , GLenum(mode));
}

void AbstractTexture::setCompareFunction(const Sampler::CompareFunction function) {
    (this->*Context::current().state().texture->parameteriImplementation)(
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_COMPARE_FUNC
        #else
        GL_TEXTURE_COMPARE_FUNC_EXT
        #endif
        , GLenum(function));
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::setDepthStencilMode(const Sampler::DepthStencilMode mode) {
    (this->*Context::current().state().texture->parameteriImplementation)(GL_DEPTH_STENCIL_TEXTURE_MODE, GLenum(mode));
}
#endif

void AbstractTexture::invalidateImage(const Int level) {
    (this->*Context::current().state().texture->invalidateImageImplementation)(level);
}

void AbstractTexture::generateMipmap() {
    (this->*Context::current().state().texture->mipmapImplementation)();
}

void AbstractTexture::mipmapImplementationDefault() {
    bindInternal();
    glGenerateMipmap(_target);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::mipmapImplementationDSA() {
    glGenerateTextureMipmap(_id);
}

void AbstractTexture::mipmapImplementationDSAEXT() {
    _flags |= ObjectFlag::Created;
    glGenerateTextureMipmapEXT(_id, _target);
}
#endif

void AbstractTexture::bindInternal() {
    /* Using glBindTextures() here is meaningless, because the non-DSA
       functions need to have the texture bound in *currently active* unit,
       so we would need to call glActiveTexture() afterwards anyway. */

    Implementation::TextureState& textureState = *Context::current().state().texture;

    /* If the texture is already bound in current unit, nothing to do */
    if(textureState.bindings[textureState.currentTextureUnit].second == _id)
        return;

    /* Set internal unit as active if not already, update state tracker */
    CORRADE_INTERNAL_ASSERT(textureState.maxTextureUnits > 1);
    const GLint internalTextureUnit = textureState.maxTextureUnits-1;
    if(textureState.currentTextureUnit != internalTextureUnit)
        glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = internalTextureUnit));

    /* Bind the texture to internal unit if not already, update state tracker */
    if(textureState.bindings[internalTextureUnit].second == _id) return;
    textureState.bindings[internalTextureUnit] = {_target, _id};

    /* Binding the texture finally creates it */
    _flags |= ObjectFlag::Created;
    glBindTexture(_target, _id);
}

#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
PixelFormat AbstractTexture::imageFormatForInternalFormat(const TextureFormat internalFormat) {
    switch(internalFormat) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Red:
        case TextureFormat::R8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R16:
        case TextureFormat::R16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16F:
        case TextureFormat::R32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRed:
        case TextureFormat::CompressedRedRgtc1:
        case TextureFormat::CompressedSignedRedRgtc1:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::CompressedR11Eac:
        case TextureFormat::CompressedSignedR11Eac:
        #endif
            return PixelFormat::Red;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8UI:
        case TextureFormat::R8I:
        case TextureFormat::R16UI:
        case TextureFormat::R16I:
        case TextureFormat::R32UI:
        case TextureFormat::R32I:
            return PixelFormat::RedInteger;
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RG:
        case TextureFormat::RG8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RG16:
        case TextureFormat::RG16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRG:
        case TextureFormat::CompressedRGRgtc2:
        case TextureFormat::CompressedSignedRGRgtc2:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::CompressedRG11Eac:
        case TextureFormat::CompressedSignedRG11Eac:
        #endif
            return PixelFormat::RG;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG8UI:
        case TextureFormat::RG8I:
        case TextureFormat::RG16UI:
        case TextureFormat::RG16I:
        case TextureFormat::RG32UI:
        case TextureFormat::RG32I:
            return PixelFormat::RGInteger;
        #endif

        case TextureFormat::RGB:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RGB8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB16:
        case TextureFormat::RGB16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R3B3G2:
        case TextureFormat::RGB4:
        case TextureFormat::RGB5:
        #endif
        case TextureFormat::RGB565:
        #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        case TextureFormat::RGB10:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB12:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R11FG11FB10F:
        case TextureFormat::RGB9E5:
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SRGB:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGB:
        case TextureFormat::CompressedRGBBptcUnsignedFloat:
        case TextureFormat::CompressedRGBBptcSignedFloat:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::CompressedRGB8Etc2:
        case TextureFormat::CompressedSRGB8Etc2:
        #endif
        case TextureFormat::CompressedRGBS3tcDxt1:
            return PixelFormat::RGB;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB8UI:
        case TextureFormat::RGB8I:
        case TextureFormat::RGB16UI:
        case TextureFormat::RGB16I:
        case TextureFormat::RGB32UI:
        case TextureFormat::RGB32I:
            return PixelFormat::RGBInteger;
        #endif

        case TextureFormat::RGBA:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RGBA8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA16:
        case TextureFormat::RGBA16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA2:
        #endif
        case TextureFormat::RGBA4:
        case TextureFormat::RGB5A1:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RGB10A2:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA12:
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SRGBAlpha:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8Alpha8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBA:
        case TextureFormat::CompressedRGBABptcUnorm:
        case TextureFormat::CompressedSRGBAlphaBptcUnorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::CompressedRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedSRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedRGBA8Etc2Eac:
        case TextureFormat::CompressedSRGB8Alpha8Etc2Eac:
        #endif
        case TextureFormat::CompressedRGBAS3tcDxt1:
        case TextureFormat::CompressedRGBAS3tcDxt3:
        case TextureFormat::CompressedRGBAS3tcDxt5:
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::CompressedRGBAAstc4x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc4x4:
        case TextureFormat::CompressedRGBAAstc5x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x4:
        case TextureFormat::CompressedRGBAAstc5x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x5:
        case TextureFormat::CompressedRGBAAstc6x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x5:
        case TextureFormat::CompressedRGBAAstc6x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x6:
        case TextureFormat::CompressedRGBAAstc8x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x5:
        case TextureFormat::CompressedRGBAAstc8x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x6:
        case TextureFormat::CompressedRGBAAstc8x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x8:
        case TextureFormat::CompressedRGBAAstc10x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x5:
        case TextureFormat::CompressedRGBAAstc10x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x6:
        case TextureFormat::CompressedRGBAAstc10x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x8:
        case TextureFormat::CompressedRGBAAstc10x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x10:
        case TextureFormat::CompressedRGBAAstc12x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x10:
        case TextureFormat::CompressedRGBAAstc12x12:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x12:
        #endif
            return PixelFormat::RGBA;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA8UI:
        case TextureFormat::RGBA8I:
        case TextureFormat::RGBA16UI:
        case TextureFormat::RGBA16I:
        case TextureFormat::RGBA32UI:
        case TextureFormat::RGBA32I:
        case TextureFormat::RGB10A2UI:
            return PixelFormat::RGBAInteger;
        #endif

        #ifdef MAGNUM_TARGET_GLES2
        case TextureFormat::Luminance:
            return PixelFormat::Luminance;
        case TextureFormat::LuminanceAlpha:
            return PixelFormat::LuminanceAlpha;
        #endif

        case TextureFormat::DepthComponent:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::DepthComponent16:
        case TextureFormat::DepthComponent24:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::DepthComponent32:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::DepthComponent32F:
        #endif
            return PixelFormat::DepthComponent;

        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::StencilIndex8:
            return PixelFormat::StencilIndex;
        #endif

        case TextureFormat::DepthStencil:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Depth24Stencil8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::Depth32FStencil8:
        #endif
            return PixelFormat::DepthStencil;
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

PixelType AbstractTexture::imageTypeForInternalFormat(const TextureFormat internalFormat) {
    switch(internalFormat) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Red:
        case TextureFormat::RG:
        #endif
        case TextureFormat::RGB:
        case TextureFormat::RGBA:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case TextureFormat::Luminance:
        case TextureFormat::LuminanceAlpha:
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SRGB:
        case TextureFormat::SRGBAlpha:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8:
        case TextureFormat::SRGB8Alpha8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA2: /**< @todo really? */
        case TextureFormat::CompressedRed:
        case TextureFormat::CompressedRG:
        case TextureFormat::CompressedRGB:
        case TextureFormat::CompressedRGBA:
        case TextureFormat::CompressedRedRgtc1:
        case TextureFormat::CompressedRGRgtc2:
        case TextureFormat::CompressedRGBABptcUnorm:
        case TextureFormat::CompressedSRGBAlphaBptcUnorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::CompressedRGB8Etc2:
        case TextureFormat::CompressedSRGB8Etc2:
        case TextureFormat::CompressedRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedSRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedRGBA8Etc2Eac:
        case TextureFormat::CompressedSRGB8Alpha8Etc2Eac:
        case TextureFormat::CompressedR11Eac:
        case TextureFormat::CompressedSignedR11Eac:
        case TextureFormat::CompressedRG11Eac:
        case TextureFormat::CompressedSignedRG11Eac:
        #endif
        case TextureFormat::CompressedRGBS3tcDxt1:
        case TextureFormat::CompressedRGBAS3tcDxt1:
        case TextureFormat::CompressedRGBAS3tcDxt3:
        case TextureFormat::CompressedRGBAS3tcDxt5:
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::CompressedRGBAAstc4x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc4x4:
        case TextureFormat::CompressedRGBAAstc5x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x4:
        case TextureFormat::CompressedRGBAAstc5x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x5:
        case TextureFormat::CompressedRGBAAstc6x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x5:
        case TextureFormat::CompressedRGBAAstc6x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x6:
        case TextureFormat::CompressedRGBAAstc8x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x5:
        case TextureFormat::CompressedRGBAAstc8x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x6:
        case TextureFormat::CompressedRGBAAstc8x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x8:
        case TextureFormat::CompressedRGBAAstc10x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x5:
        case TextureFormat::CompressedRGBAAstc10x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x6:
        case TextureFormat::CompressedRGBAAstc10x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x8:
        case TextureFormat::CompressedRGBAAstc10x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x10:
        case TextureFormat::CompressedRGBAAstc12x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x10:
        case TextureFormat::CompressedRGBAAstc12x12:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x12:
        #endif
            return PixelType::UnsignedByte;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8Snorm:
        case TextureFormat::RG8Snorm:
        case TextureFormat::RGB8Snorm:
        case TextureFormat::RGBA8Snorm:
        case TextureFormat::R8I:
        case TextureFormat::RG8I:
        case TextureFormat::RGB8I:
        case TextureFormat::RGBA8I:
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedSignedRedRgtc1:
        case TextureFormat::CompressedSignedRGRgtc2:
        #endif
            return PixelType::Byte;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R16:
        case TextureFormat::RG16:
        case TextureFormat::RGB16:
        case TextureFormat::RGBA16:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16UI:
        case TextureFormat::RG16UI:
        case TextureFormat::RGB16UI:
        case TextureFormat::RGBA16UI:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB12: /**< @todo really? */
        #endif
        case TextureFormat::RGBA4: /**< @todo really? */
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA12: /**< @todo really? */
        #endif
            return PixelType::UnsignedShort;

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R16Snorm:
        case TextureFormat::RG16Snorm:
        case TextureFormat::RGB16Snorm:
        case TextureFormat::RGBA16Snorm:
        #endif
        case TextureFormat::R16I:
        case TextureFormat::RG16I:
        case TextureFormat::RGB16I:
        case TextureFormat::RGBA16I:
            return PixelType::Short;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            return PixelType::HalfFloat;

        case TextureFormat::R32UI:
        case TextureFormat::RG32UI:
        case TextureFormat::RGB32UI:
        case TextureFormat::RGBA32UI:
            return PixelType::UnsignedInt;

        case TextureFormat::R32I:
        case TextureFormat::RG32I:
        case TextureFormat::RGB32I:
        case TextureFormat::RGBA32I:
            return PixelType::Int;

        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBBptcUnsignedFloat:
        case TextureFormat::CompressedRGBBptcSignedFloat:
        #endif
            return PixelType::Float;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R3B3G2:
            return PixelType::UnsignedByte332;
        case TextureFormat::RGB4:
            return PixelType::UnsignedShort4444;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB5:
        #endif
        case TextureFormat::RGB5A1:
            return PixelType::UnsignedShort5551;

        case TextureFormat::RGB565:
            return PixelType::UnsignedShort565;

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::RGB10:
        #endif
        case TextureFormat::RGB10A2:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB10A2UI:
        #endif
            return PixelType::UnsignedInt2101010Rev; /**< @todo Rev for all? */
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R11FG11FB10F:
            return PixelType::UnsignedInt10F11F11FRev;
        case TextureFormat::RGB9E5:
            return PixelType::UnsignedInt5999Rev;
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::DepthComponent16:
            return PixelType::UnsignedShort;
        #endif

        case TextureFormat::DepthComponent:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::DepthComponent24:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::DepthComponent32:
        #endif
            return PixelType::UnsignedInt;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::DepthComponent32F:
            return PixelType::Float;
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::StencilIndex8:
            return PixelType::UnsignedByte;
        #endif

        case TextureFormat::DepthStencil:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Depth24Stencil8:
        #endif
            return PixelType::UnsignedInt248;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::Depth32FStencil8:
            return PixelType::Float32UnsignedInt248Rev;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, GLint value) {
    bindInternal();
    glTexParameteri(_target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(const GLenum parameter, const GLint value) {
    glTextureParameteri(_id, parameter, value);
}

void AbstractTexture::parameterImplementationDSAEXT(GLenum parameter, GLint value) {
    _flags |= ObjectFlag::Created;
    glTextureParameteriEXT(_id, _target, parameter, value);
}
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, GLfloat value) {
    bindInternal();
    glTexParameterf(_target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(const GLenum parameter, const GLfloat value) {
    glTextureParameterf(_id, parameter, value);
}

void AbstractTexture::parameterImplementationDSAEXT(GLenum parameter, GLfloat value) {
    _flags |= ObjectFlag::Created;
    glTextureParameterfEXT(_id, _target, parameter, value);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::parameterImplementationDefault(GLenum parameter, const GLint* values) {
    bindInternal();
    glTexParameteriv(_target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(const GLenum parameter, const GLint* const values) {
    glTextureParameteriv(_id, parameter, values);
}

void AbstractTexture::parameterImplementationDSAEXT(GLenum parameter, const GLint* values) {
    _flags |= ObjectFlag::Created;
    glTextureParameterivEXT(_id, _target, parameter, values);
}
#endif
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, const GLfloat* values) {
    bindInternal();
    glTexParameterfv(_target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(const GLenum parameter, const GLfloat* const values) {
    glTextureParameterfv(_id, parameter, values);
}

void AbstractTexture::parameterImplementationDSAEXT(GLenum parameter, const GLfloat* values) {
    _flags |= ObjectFlag::Created;
    glTextureParameterfvEXT(_id, _target, parameter, values);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::parameterIImplementationDefault(GLenum parameter, const GLuint* values) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES
    glTexParameterIuiv
    #else
    glTexParameterIuivEXT
    #endif
        (_target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterIImplementationDSA(const GLenum parameter, const GLuint* const values) {
    glTextureParameterIuiv(_id, parameter, values);
}

void AbstractTexture::parameterIImplementationDSAEXT(GLenum parameter, const GLuint* values) {
    _flags |= ObjectFlag::Created;
    glTextureParameterIuivEXT(_id, _target, parameter, values);
}
#endif

void AbstractTexture::parameterIImplementationDefault(GLenum parameter, const GLint* values) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES
    glTexParameterIiv
    #else
    glTexParameterIivEXT
    #endif
        (_target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterIImplementationDSA(const GLenum parameter, const GLint* const values) {
    glTextureParameterIiv(_id, parameter, values);
}

void AbstractTexture::parameterIImplementationDSAEXT(GLenum parameter, const GLint* values) {
    _flags |= ObjectFlag::Created;
    glTextureParameterIivEXT(_id, _target, parameter, values);
}
#endif
#endif

void AbstractTexture::setMaxAnisotropyImplementationNoOp(GLfloat) {}

void AbstractTexture::setMaxAnisotropyImplementationExt(GLfloat anisotropy) {
    (this->*Context::current().state().texture->parameterfImplementation)(GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::getLevelParameterImplementationDefault(const GLint level, const GLenum parameter, GLint* const values) {
    bindInternal();
    glGetTexLevelParameteriv(_target, level, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getLevelParameterImplementationDSA(const GLint level, const GLenum parameter, GLint* const values) {
    glGetTextureLevelParameteriv(_id, level, parameter, values);
}

void AbstractTexture::getLevelParameterImplementationDSAEXT(const GLint level, const GLenum parameter, GLint* const values) {
    _flags |= ObjectFlag::Created;
    glGetTextureLevelParameterivEXT(_id, _target, level, parameter, values);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationFallback(const GLsizei levels, const TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    const PixelFormat format = imageFormatForInternalFormat(internalFormat);
    const PixelType type = imageTypeForInternalFormat(internalFormat);

    for(GLsizei level = 0; level != levels; ++level)
        DataHelper<1>::setImage(*this, level, internalFormat,
            ImageView1D{format, type, Math::max(Math::Vector<1, GLsizei>(1), size >> level)});
}

void AbstractTexture::storageImplementationDefault(GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    bindInternal();
    glTexStorage1D(_target, levels, GLenum(internalFormat), size[0]);
}

void AbstractTexture::storageImplementationDSA(const GLsizei levels, const TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    glTextureStorage1D(_id, levels, GLenum(internalFormat), size[0]);
}

void AbstractTexture::storageImplementationDSAEXT(GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    _flags |= ObjectFlag::Created;
    glTextureStorage1DEXT(_id, _target, levels, GLenum(internalFormat), size[0]);
}
#endif

#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
void AbstractTexture::storageImplementationFallback(const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    const PixelFormat format = imageFormatForInternalFormat(internalFormat);
    const PixelType type = imageTypeForInternalFormat(internalFormat);

    /* Common code for classic types */
    #ifndef MAGNUM_TARGET_GLES
    if(_target == GL_TEXTURE_2D || _target == GL_TEXTURE_RECTANGLE)
    #else
    if(_target == GL_TEXTURE_2D)
    #endif
    {
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<2>::setImage(*this, level, internalFormat,
                ImageView2D{format, type, Math::max(Vector2i(1), size >> level)});

    /* Cube map additionally needs to specify all faces */
    } else if(_target == GL_TEXTURE_CUBE_MAP) {
        for(GLsizei level = 0; level != levels; ++level) {
            for(GLenum face: {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Z})
                DataHelper<2>::setImage(*this, face, level, internalFormat,
                    ImageView2D{format, type, Math::max(Vector2i(1), size >> level)});
        }

    #ifndef MAGNUM_TARGET_GLES
    /* Array texture is not scaled in "layer" dimension */
    } else if(_target == GL_TEXTURE_1D_ARRAY) {
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<2>::setImage(*this, level, internalFormat,
                ImageView2D{format, type, Vector2i{Math::max(1, size.x() >> level), size.y()}});
    #endif

    /* No other targets are available */
    } else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::storageImplementationDefault(GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage2D(_target, levels, GLenum(internalFormat), size.x(), size.y());
    #elif !defined(CORRADE_TARGET_NACL)
    glTexStorage2DEXT(_target, levels, GLenum(internalFormat), size.x(), size.y());
    #else
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    glTextureStorage2D(_id, levels, GLenum(internalFormat), size.x(), size.y());
}

void AbstractTexture::storageImplementationDSAEXT(GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
    _flags |= ObjectFlag::Created;
    glTextureStorage2DEXT(_id, _target, levels, GLenum(internalFormat), size.x(), size.y());
}
#endif

#if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
void AbstractTexture::storageImplementationFallback(GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    const PixelFormat format = imageFormatForInternalFormat(internalFormat);
    const PixelType type = imageTypeForInternalFormat(internalFormat);

    /* Common code for classic type */
    #ifndef MAGNUM_TARGET_GLES2
    if(_target == GL_TEXTURE_3D)
    #else
    if(_target == GL_TEXTURE_3D_OES)
    #endif
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<3>::setImage(*this, level, internalFormat,
                ImageView3D{format, type, Math::max(Vector3i(1), size >> level)});

    #ifndef MAGNUM_TARGET_GLES2
    /* Array texture is not scaled in "layer" dimension */
    #ifndef MAGNUM_TARGET_GLES
    else if(_target == GL_TEXTURE_2D_ARRAY || _target == GL_TEXTURE_CUBE_MAP_ARRAY)
    #else
    else if(_target == GL_TEXTURE_2D_ARRAY)
    #endif
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<3>::setImage(*this, level, internalFormat,
                ImageView3D{format, type, Vector3i{Math::max(Vector2i{1}, size.xy() >> level), size.z()}});
    #endif

    /* No other targets are available */
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::storageImplementationDefault(GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage3D(_target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    #elif !defined(CORRADE_TARGET_NACL)
    glTexStorage3DEXT(_target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    #else
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(const GLsizei levels, const TextureFormat internalFormat, const Vector3i& size) {
    glTextureStorage3D(_id, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
}

void AbstractTexture::storageImplementationDSAEXT(GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    _flags |= ObjectFlag::Created;
    glTextureStorage3DEXT(_id, _target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationFallback(const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    bindInternal();
    glTexImage2DMultisample(_target, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::storageMultisampleImplementationDefault(const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    bindInternal();
    glTexStorage2DMultisample(_target, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationDSA(const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    glTextureStorage2DMultisample(_id, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}

void AbstractTexture::storageMultisampleImplementationDSAEXT(const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    _flags |= ObjectFlag::Created;
    glTextureStorage2DMultisampleEXT(_id, _target, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}

void AbstractTexture::storageMultisampleImplementationFallback(const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    bindInternal();
    glTexImage3DMultisample(_target, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::storageMultisampleImplementationDefault(const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES
    glTexStorage3DMultisample
    #else
    glTexStorage3DMultisampleOES
    #endif
        (_target, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationDSA(const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    glTextureStorage3DMultisample(_id, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}

void AbstractTexture::storageMultisampleImplementationDSAEXT(const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    _flags |= ObjectFlag::Created;
    glTextureStorage3DMultisampleEXT(_id, _target, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getImageImplementationDefault(const GLint level, const PixelFormat format, const PixelType type, const std::size_t, GLvoid* const data) {
    bindInternal();
    glGetTexImage(_target, level, GLenum(format), GLenum(type), data);
}

void AbstractTexture::getCompressedImageImplementationDefault(const GLint level, std::size_t, GLvoid* const data) {
    bindInternal();
    glGetCompressedTexImage(_target, level, data);
}

void AbstractTexture::getImageImplementationDSA(const GLint level, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    glGetTextureImage(_id, level, GLenum(format), GLenum(type), dataSize, data);
}

void AbstractTexture::getCompressedImageImplementationDSA(const GLint level, const std::size_t dataSize, GLvoid* const data) {
    glGetCompressedTextureImage(_id, level, dataSize, data);
}

void AbstractTexture::getImageImplementationDSAEXT(const GLint level, const PixelFormat format, const PixelType type, const std::size_t, GLvoid* const data) {
    _flags |= ObjectFlag::Created;
    glGetTextureImageEXT(_id, _target, level, GLenum(format), GLenum(type), data);
}

void AbstractTexture::getCompressedImageImplementationDSAEXT(const GLint level, std::size_t, GLvoid* const data) {
    _flags |= ObjectFlag::Created;
    glGetCompressedTextureImageEXT(_id, _target, level, data);
}

void AbstractTexture::getImageImplementationRobustness(const GLint level, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    bindInternal();
    glGetnTexImageARB(_target, level, GLenum(format), GLenum(type), dataSize, data);
}

void AbstractTexture::getCompressedImageImplementationRobustness(const GLint level, const std::size_t dataSize, GLvoid* const data) {
    bindInternal();
    glGetnCompressedTexImageARB(_target, level, dataSize, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDefault(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, PixelFormat format, PixelType type, const GLvoid* data) {
    bindInternal();
    glTexSubImage1D(_target, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDefault(const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    bindInternal();
    glCompressedTexSubImage1D(_target, level, offset[0], size[0], GLenum(format), dataSize, data);
}

void AbstractTexture::subImageImplementationDSA(const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const PixelFormat format, const PixelType type, const GLvoid* const data) {
    glTextureSubImage1D(_id, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSA(const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage1D(_id, level, offset[0], size[0], GLenum(format), dataSize, data);
}

void AbstractTexture::subImageImplementationDSAEXT(GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, PixelFormat format, PixelType type, const GLvoid* data) {
    _flags |= ObjectFlag::Created;
    glTextureSubImage1DEXT(_id, _target, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSAEXT(const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    _flags |= ObjectFlag::Created;
    glCompressedTextureSubImage1DEXT(_id, _target, level, offset[0], size[0], GLenum(format), dataSize, data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data) {
    bindInternal();
    glTexSubImage2D(_target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDefault(const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    bindInternal();
    glCompressedTexSubImage2D(_target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), dataSize, data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(const GLint level, const Vector2i& offset, const Vector2i& size, const PixelFormat format, const PixelType type, const GLvoid* const data) {
    glTextureSubImage2D(_id, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSA(const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage2D(_id, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), dataSize, data);
}

void AbstractTexture::subImageImplementationDSAEXT(GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data) {
    _flags |= ObjectFlag::Created;
    glTextureSubImage2DEXT(_id, _target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSAEXT(GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    _flags |= ObjectFlag::Created;
    glCompressedTextureSubImage2DEXT(_id, _target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), dataSize, data);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::subImageImplementationDefault(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexSubImage3D(_target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
    #elif !defined(CORRADE_TARGET_NACL)
    glTexSubImage3DOES(_target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
    #else
    static_cast<void>(level);
    static_cast<void>(offset);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}

void AbstractTexture::compressedSubImageImplementationDefault(const GLint level, const Vector3i& offset, const Vector3i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glCompressedTexSubImage3D(_target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
    #elif !defined(CORRADE_TARGET_NACL)
    glCompressedTexSubImage3DOES(_target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
    #else
    static_cast<void>(level);
    static_cast<void>(offset);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(data);
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(const GLint level, const Vector3i& offset, const Vector3i& size, const PixelFormat format, const PixelType type, const GLvoid* const data) {
    glTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSA(const GLint level, const Vector3i& offset, const Vector3i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
}

void AbstractTexture::subImageImplementationDSAEXT(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data) {
    _flags |= ObjectFlag::Created;
    glTextureSubImage3DEXT(_id, _target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSAEXT(const GLint level, const Vector3i& offset, const Vector3i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    _flags |= ObjectFlag::Created;
    glCompressedTextureSubImage3DEXT(_id, _target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
}
#endif

void AbstractTexture::invalidateImageImplementationNoOp(GLint) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateImageImplementationARB(GLint level) {
    createIfNotAlready();
    glInvalidateTexImage(_id, level);
}
#endif

void AbstractTexture::invalidateSubImageImplementationNoOp(GLint, const Vector3i&, const Vector3i&) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateSubImageImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size) {
    createIfNotAlready();
    glInvalidateTexSubImage(_id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z());
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> void AbstractTexture::image(GLint level, Image<dimensions>& image) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    const std::size_t dataSize = Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    (this->*Context::current().state().texture->getImageImplementation)(level, image.format(), image.type(), data.size(), data);
    image.setData(image.storage(), image.format(), image.type(), size, std::move(data));
}

template void MAGNUM_EXPORT AbstractTexture::image<1>(GLint, Image<1>&);
template void MAGNUM_EXPORT AbstractTexture::image<2>(GLint, Image<2>&);
template void MAGNUM_EXPORT AbstractTexture::image<3>(GLint, Image<3>&);

template<UnsignedInt dimensions> void AbstractTexture::image(GLint level, BufferImage<dimensions>& image, BufferUsage usage) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    const std::size_t dataSize = Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    (this->*Context::current().state().texture->getImageImplementation)(level, image.format(), image.type(), dataSize, nullptr);
}

template void MAGNUM_EXPORT AbstractTexture::image<1>(GLint, BufferImage<1>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::image<2>(GLint, BufferImage<2>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::image<3>(GLint, BufferImage<3>&, BufferUsage);

template<UnsignedInt dimensions> void AbstractTexture::compressedImage(const GLint level, CompressedImage<dimensions>& image) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    GLint textureDataSize;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &textureDataSize);
    const std::size_t dataSize = Implementation::compressedImageDataSizeFor(image, size, textureDataSize);
    GLint format;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    (this->*Context::current().state().texture->getCompressedImageImplementation)(level, data.size(), data);
    image.setData(image.storage(), CompressedPixelFormat(format), size, std::move(data));
}

template void MAGNUM_EXPORT AbstractTexture::compressedImage<1>(GLint, CompressedImage<1>&);
template void MAGNUM_EXPORT AbstractTexture::compressedImage<2>(GLint, CompressedImage<2>&);
template void MAGNUM_EXPORT AbstractTexture::compressedImage<3>(GLint, CompressedImage<3>&);

template<UnsignedInt dimensions> void AbstractTexture::compressedImage(const GLint level, CompressedBufferImage<dimensions>& image, BufferUsage usage) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    GLint textureDataSize;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &textureDataSize);
    const std::size_t dataSize = Implementation::compressedImageDataSizeFor(image, size, textureDataSize);
    GLint format;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), CompressedPixelFormat(format), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    (this->*Context::current().state().texture->getCompressedImageImplementation)(level, dataSize, nullptr);
}

template void MAGNUM_EXPORT AbstractTexture::compressedImage<1>(GLint, CompressedBufferImage<1>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::compressedImage<2>(GLint, CompressedBufferImage<2>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::compressedImage<3>(GLint, CompressedBufferImage<3>&, BufferUsage);

template<UnsignedInt dimensions> void AbstractTexture::subImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>& image) {
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const std::size_t dataSize = Implementation::imageDataSizeFor(image, size);
    const Vector3i paddedOffset = Vector3i::pad(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    glGetTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), GLenum(image.format()), GLenum(image.type()), data.size(), data);
    image.setData(image.storage(), image.format(), image.type(), size, std::move(data));
}

template void MAGNUM_EXPORT AbstractTexture::subImage<1>(GLint, const Range1Di&, Image<1>&);
template void MAGNUM_EXPORT AbstractTexture::subImage<2>(GLint, const Range2Di&, Image<2>&);
template void MAGNUM_EXPORT AbstractTexture::subImage<3>(GLint, const Range3Di&, Image<3>&);

template<UnsignedInt dimensions> void AbstractTexture::subImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>& image, const BufferUsage usage) {
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const std::size_t dataSize = Implementation::imageDataSizeFor(image, size);
    const Vector3i paddedOffset = Vector3i::pad(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    glGetTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), GLenum(image.format()), GLenum(image.type()), dataSize, nullptr);
}

template void MAGNUM_EXPORT AbstractTexture::subImage<1>(GLint, const Range1Di&, BufferImage<1>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::subImage<2>(GLint, const Range2Di&, BufferImage<2>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::subImage<3>(GLint, const Range3Di&, BufferImage<3>&, BufferUsage);

template<UnsignedInt dimensions> std::size_t AbstractTexture::compressedSubImageSize(TextureFormat format, const Math::Vector<dimensions, Int>& size) {
    /* Amount of blocks in given range (rounded up) multiplied by block
       data size. The user is responsible for proper block-aligned size. */
    const Math::Vector<dimensions, Int> blockSize = DataHelper<dimensions>::compressedBlockSize(_target, format);
    return ((size + blockSize - Math::Vector<dimensions, Int>{1})/blockSize).product()*
        compressedBlockDataSize(_target, format);
}

template<UnsignedInt dimensions> void AbstractTexture::compressedSubImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, CompressedImage<dimensions>& image) {
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const Vector3i paddedOffset = Vector3i::pad(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);
    GLint format;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    const std::size_t dataSize = Implementation::compressedImageDataSizeFor(image, size, compressedSubImageSize<dimensions>(TextureFormat(format), size));

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    glGetCompressedTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), data.size(), data);
    image.setData(CompressedPixelFormat(format), size, std::move(data));
}

template void MAGNUM_EXPORT AbstractTexture::compressedSubImage<1>(GLint, const Range1Di&, CompressedImage<1>&);
template void MAGNUM_EXPORT AbstractTexture::compressedSubImage<2>(GLint, const Range2Di&, CompressedImage<2>&);
template void MAGNUM_EXPORT AbstractTexture::compressedSubImage<3>(GLint, const Range3Di&, CompressedImage<3>&);

template<UnsignedInt dimensions> void AbstractTexture::compressedSubImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, CompressedBufferImage<dimensions>& image, const BufferUsage usage) {
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const Vector3i paddedOffset = Vector3i::pad(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);
    GLint format;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    const std::size_t dataSize = Implementation::compressedImageDataSizeFor(image, size, compressedSubImageSize<dimensions>(TextureFormat(format), size));

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), CompressedPixelFormat(format), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    image.storage().applyPack();
    glGetCompressedTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), dataSize, nullptr);
}

template void MAGNUM_EXPORT AbstractTexture::compressedSubImage<1>(GLint, const Range1Di&, CompressedBufferImage<1>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::compressedSubImage<2>(GLint, const Range2Di&, CompressedBufferImage<2>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::compressedSubImage<3>(GLint, const Range3Di&, CompressedBufferImage<3>&, BufferUsage);
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
Math::Vector<1, GLint> AbstractTexture::DataHelper<1>::compressedBlockSize(const GLenum target, const TextureFormat format) {
    GLint value;
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_WIDTH, 1, &value);
    return value;
}

Vector2i AbstractTexture::DataHelper<2>::compressedBlockSize(const GLenum target, const TextureFormat format) {
    Vector2i value{Math::NoInit};
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_WIDTH, 1, &value.x());
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT, 1, &value.y());
    return value;
}

Vector3i AbstractTexture::DataHelper<3>::compressedBlockSize(const GLenum target, const TextureFormat format) {
    /** @todo use real value when OpenGL has proper queries for 3D compression formats */
    return Vector3i{DataHelper<2>::compressedBlockSize(target, format), 1};
}
#endif

#ifndef MAGNUM_TARGET_GLES
Math::Vector<1, GLint> AbstractTexture::DataHelper<1>::imageSize(AbstractTexture& texture, const GLint level) {
    Math::Vector<1, GLint> value;
    (texture.*Context::current().state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_WIDTH, &value[0]);
    return value;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Vector2i AbstractTexture::DataHelper<2>::imageSize(AbstractTexture& texture, const GLint level) {
    const Implementation::TextureState& state = *Context::current().state().texture;

    Vector2i value;
    (texture.*state.getLevelParameterivImplementation)(level, GL_TEXTURE_WIDTH, &value[0]);
    (texture.*state.getLevelParameterivImplementation)(level, GL_TEXTURE_HEIGHT, &value[1]);
    return value;
}

Vector3i AbstractTexture::DataHelper<3>::imageSize(AbstractTexture& texture, const GLint level) {
    const Implementation::TextureState& state = *Context::current().state().texture;

    Vector3i value;
    (texture.*state.getLevelParameterivImplementation)(level, GL_TEXTURE_WIDTH, &value[0]);
    (texture.*state.getLevelParameterivImplementation)(level, GL_TEXTURE_HEIGHT, &value[1]);
    (texture.*state.getLevelParameterivImplementation)(level, GL_TEXTURE_DEPTH, &value[2]);
    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setStorage(AbstractTexture& texture, const GLsizei levels, const TextureFormat internalFormat, const Math::Vector< 1, GLsizei >& size) {
    (texture.*Context::current().state().texture->storage1DImplementation)(levels, internalFormat, size);
}
#endif

void AbstractTexture::DataHelper<2>::setStorage(AbstractTexture& texture, const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    (texture.*Context::current().state().texture->storage2DImplementation)(levels, internalFormat, size);
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setStorage(AbstractTexture& texture, const GLsizei levels, const TextureFormat internalFormat, const Vector3i& size) {
    (texture.*Context::current().state().texture->storage3DImplementation)(levels, internalFormat, size);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::DataHelper<2>::setStorageMultisample(AbstractTexture& texture, const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    (texture.*Context::current().state().texture->storage2DMultisampleImplementation)(samples, internalFormat, size, fixedSampleLocations);
}

void AbstractTexture::DataHelper<3>::setStorageMultisample(AbstractTexture& texture, const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    (texture.*Context::current().state().texture->storage3DMultisampleImplementation)(samples, internalFormat, size, fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, const ImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    texture.bindInternal();
    glTexImage1D(texture._target, level, GLint(internalFormat), image.size()[0], 0, GLenum(image.format()), GLenum(image.type()), image.data());
}

void AbstractTexture::DataHelper<1>::setCompressedImage(AbstractTexture& texture, const GLint level, const CompressedImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    texture.bindInternal();
    glCompressedTexImage1D(texture._target, level, GLenum(image.format()), image.size()[0], 0, Implementation::occupiedCompressedImageDataSize(image, image.data().size()), image.data());
}

void AbstractTexture::DataHelper<1>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, BufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    texture.bindInternal();
    glTexImage1D(texture._target, level, GLint(internalFormat), image.size()[0], 0, GLenum(image.format()), GLenum(image.type()), nullptr);
}

void AbstractTexture::DataHelper<1>::setCompressedImage(AbstractTexture& texture, const GLint level, CompressedBufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    texture.bindInternal();
    glCompressedTexImage1D(texture._target, level, GLenum(image.format()), image.size()[0], 0, Implementation::occupiedCompressedImageDataSize(image, image.dataSize()), nullptr);
}

void AbstractTexture::DataHelper<1>::setSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, const ImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->subImage1DImplementation)(level, offset, image.size(), image.format(), image.type(), image.data());
}

void AbstractTexture::DataHelper<1>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, const CompressedImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->compressedSubImage1DImplementation)(level, offset, image.size(), image.format(), image.data(), Implementation::occupiedCompressedImageDataSize(image, image.data().size()));
}

void AbstractTexture::DataHelper<1>::setSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, BufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->subImage1DImplementation)(level, offset, image.size(), image.format(), image.type(), nullptr);
}

void AbstractTexture::DataHelper<1>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, CompressedBufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->compressedSubImage1DImplementation)(level, offset, image.size(), image.format(), nullptr, Implementation::occupiedCompressedImageDataSize(image, image.dataSize()));
}
#endif

void AbstractTexture::DataHelper<2>::setImage(AbstractTexture& texture, const GLenum target, const GLint level, const TextureFormat internalFormat, const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    image.storage().applyUnpack();
    texture.bindInternal();
    glTexImage2D(target, level, GLint(internalFormat), image.size().x(), image.size().y(), 0, GLenum(image.format()), GLenum(image.type()), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Implementation::pixelStorageSkipOffset(image)
        #endif
        );
}

void AbstractTexture::DataHelper<2>::setCompressedImage(AbstractTexture& texture, const GLenum target, const GLint level, const CompressedImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    texture.bindInternal();
    glCompressedTexImage2D(target, level, GLenum(image.format()), image.size().x(), image.size().y(), 0, Implementation::occupiedCompressedImageDataSize(image, image.data().size()), image.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<2>::setImage(AbstractTexture& texture, const GLenum target, const GLint level, const TextureFormat internalFormat, BufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    texture.bindInternal();
    glTexImage2D(target, level, GLint(internalFormat), image.size().x(), image.size().y(), 0, GLenum(image.format()), GLenum(image.type()), nullptr);
}

void AbstractTexture::DataHelper<2>::setCompressedImage(AbstractTexture& texture, const GLenum target, const GLint level, CompressedBufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    texture.bindInternal();
    glCompressedTexImage2D(target, level, GLenum(image.format()), image.size().x(), image.size().y(), 0, Implementation::occupiedCompressedImageDataSize(image, image.dataSize()), nullptr);
}
#endif

void AbstractTexture::DataHelper<2>::setSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->subImage2DImplementation)(level, offset, image.size(), image.format(), image.type(), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Implementation::pixelStorageSkipOffset(image)
        #endif
        );
}

void AbstractTexture::DataHelper<2>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, const CompressedImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    (texture.*Context::current().state().texture->compressedSubImage2DImplementation)(level, offset, image.size(), image.format(), image.data(), Implementation::occupiedCompressedImageDataSize(image, image.data().size()));
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<2>::setSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, BufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->subImage2DImplementation)(level, offset, image.size(), image.format(), image.type(), nullptr);
}

void AbstractTexture::DataHelper<2>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, CompressedBufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    (texture.*Context::current().state().texture->compressedSubImage2DImplementation)(level, offset, image.size(), image.format(), nullptr, Implementation::occupiedCompressedImageDataSize(image, image.dataSize()));
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, const ImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    image.storage().applyUnpack();
    texture.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexImage3D(texture._target, level, GLint(internalFormat), image.size().x(), image.size().y(), image.size().z(), 0, GLenum(image.format()), GLenum(image.type()), image.data());
    #elif !defined(CORRADE_TARGET_NACL)
    glTexImage3DOES(texture._target, level, GLint(internalFormat), image.size().x(), image.size().y(), image.size().z(), 0, GLenum(image.format()), GLenum(image.type()), image.data() + Implementation::pixelStorageSkipOffset(image));
    #else
    static_cast<void>(level);
    static_cast<void>(internalFormat);
    static_cast<void>(image);
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}

void AbstractTexture::DataHelper<3>::setCompressedImage(AbstractTexture& texture, const GLint level, const CompressedImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    texture.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glCompressedTexImage3D(texture._target, level, GLenum(image.format()), image.size().x(), image.size().y(), image.size().z(), 0, Implementation::occupiedCompressedImageDataSize(image, image.data().size()), image.data());
    #elif !defined(CORRADE_TARGET_NACL)
    glCompressedTexImage3DOES(texture._target, level, GLenum(image.format()), image.size().x(), image.size().y(), image.size().z(), 0, Implementation::occupiedCompressedImageDataSize(image, image.data().size()), image.data());
    #else
    static_cast<void>(level);
    static_cast<void>(image);
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<3>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, BufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    texture.bindInternal();
    glTexImage3D(texture._target, level, GLint(internalFormat), image.size().x(), image.size().y(), image.size().z(), 0, GLenum(image.format()), GLenum(image.type()), nullptr);
}

void AbstractTexture::DataHelper<3>::setCompressedImage(AbstractTexture& texture, const GLint level, CompressedBufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    texture.bindInternal();
    glCompressedTexImage3D(texture._target, level, GLenum(image.format()), image.size().x(), image.size().y(), image.size().z(), 0, Implementation::occupiedCompressedImageDataSize(image, image.dataSize()), nullptr);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, const ImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->subImage3DImplementation)(level, offset, image.size(), image.format(), image.type(), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Implementation::pixelStorageSkipOffset(image)
        #endif
        );
}

void AbstractTexture::DataHelper<3>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, const CompressedImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    (texture.*Context::current().state().texture->compressedSubImage3DImplementation)(level, offset, image.size(), image.format(), image.data(), Implementation::occupiedCompressedImageDataSize(image, image.data().size()));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<3>::setSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, BufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    image.storage().applyUnpack();
    (texture.*Context::current().state().texture->subImage3DImplementation)(level, offset, image.size(), image.format(), image.type(), nullptr);
}

void AbstractTexture::DataHelper<3>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, CompressedBufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    #ifndef MAGNUM_TARGET_GLES
    /* Pixel storage is completely ignored for compressed images on ES, no need
       to reset anything */
    image.storage().applyUnpack();
    #endif
    (texture.*Context::current().state().texture->compressedSubImage3DImplementation)(level, offset, image.size(), image.format(), nullptr, Implementation::occupiedCompressedImageDataSize(image, image.dataSize()));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::invalidateSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size) {
    (texture.*Context::current().state().texture->invalidateSubImageImplementation)(level, {offset[0], 0, 0}, {size[0], 1, 1});
}
#endif

void AbstractTexture::DataHelper<2>::invalidateSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, const Vector2i& size) {
    (texture.*Context::current().state().texture->invalidateSubImageImplementation)(level, {offset, 0}, {size, 1});
}

void AbstractTexture::DataHelper<3>::invalidateSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, const Vector3i& size) {
    (texture.*Context::current().state().texture->invalidateSubImageImplementation)(level, offset, size);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setWrapping(AbstractTexture& texture, const Array1D<Sampler::Wrapping>& wrapping) {
    (texture.*Context::current().state().texture->parameteriImplementation)(GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
}
#endif

void AbstractTexture::DataHelper<2>::setWrapping(AbstractTexture& texture, const Array2D<Sampler::Wrapping>& wrapping) {
    const Implementation::TextureState& state = *Context::current().state().texture;

    (texture.*state.parameteriImplementation)(GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
    (texture.*state.parameteriImplementation)(GL_TEXTURE_WRAP_T, GLint(wrapping.y()));
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setWrapping(AbstractTexture& texture, const Array3D<Sampler::Wrapping>& wrapping) {
    const Implementation::TextureState& state = *Context::current().state().texture;

    (texture.*state.parameteriImplementation)(GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
    (texture.*state.parameteriImplementation)(GL_TEXTURE_WRAP_T, GLint(wrapping.y()));
    #ifndef MAGNUM_TARGET_GLES2
    (texture.*state.parameteriImplementation)(GL_TEXTURE_WRAP_R, GLint(wrapping.z()));
    #else
    (texture.*state.parameteriImplementation)(GL_TEXTURE_WRAP_R_OES, GLint(wrapping.z()));
    #endif
}
#endif
#endif

}
