/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "AbstractFramebuffer.h"

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/Implementation/ImageProperties.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif
#include "Magnum/GL/Context.h"
#include "Magnum/GL/CubeMapTexture.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/CubeMapTextureArray.h"
#endif
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/RectangleTexture.h"
#endif
#include "Magnum/GL/Texture.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#endif
#include "Magnum/GL/Implementation/FramebufferState.h"
#include "Magnum/GL/Implementation/RendererState.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum { namespace GL {

Vector2i AbstractFramebuffer::maxViewportSize() {
    Vector2i& value = Context::current().state().framebuffer.maxViewportSize;

    /* Get the value, if not already cached */
    if(value == Vector2i())
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, value.data());

    return value;
}

Int AbstractFramebuffer::maxDrawBuffers() {
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::draw_buffers>() &&
       !Context::current().isExtensionSupported<Extensions::NV::draw_buffers>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_buffers>())
        return 0;
    #endif
    #endif

    GLint& value = Context::current().state().framebuffer.maxDrawBuffers;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &value);
        #else
        glGetIntegerv(GL_MAX_DRAW_BUFFERS_EXT, &value);
        #endif
    }

    return value;
}

#ifndef MAGNUM_TARGET_GLES
Int AbstractFramebuffer::maxDualSourceDrawBuffers() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::blend_func_extended>())
        return 0;

    GLint& value = Context::current().state().framebuffer.maxDualSourceDrawBuffers;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &value);

    return value;
}
#endif

void AbstractFramebuffer::createIfNotAlready() {
    if(_flags & ObjectFlag::Created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glObjectLabel()) operate with IDs directly and they
       require the object to be created. Binding the framebuffer finally
       creates it. Also all EXT DSA functions implicitly create it. */
    bindInternal();
    CORRADE_INTERNAL_ASSERT(_flags & ObjectFlag::Created);
}

void AbstractFramebuffer::bind() {
    bindInternal(FramebufferTarget::Draw);
    setViewportInternal();
}

void AbstractFramebuffer::bindInternal(FramebufferTarget target) {
    #ifndef MAGNUM_TARGET_GLES2
    bindImplementationDefault(*this, target);
    #elif defined(MAGNUM_TARGET_WEBGL)
    static_cast<void>(target);
    bindImplementationSingle(*this);
    #else
    Context::current().state().framebuffer.bindImplementation(*this, target);
    #endif
}

#ifdef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::bindImplementationSingle(AbstractFramebuffer& self, FramebufferTarget) {
    Implementation::FramebufferState& state = Context::current().state().framebuffer;
    CORRADE_INTERNAL_ASSERT(state.readBinding == state.drawBinding);
    if(state.readBinding == self._id) return;

    state.readBinding = state.drawBinding = self._id;

    /* Binding the framebuffer finally creates it */
    self._flags |= ObjectFlag::Created;
    glBindFramebuffer(GL_FRAMEBUFFER, self._id);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
inline
#endif
void AbstractFramebuffer::bindImplementationDefault(AbstractFramebuffer& self, FramebufferTarget target) {
    Implementation::FramebufferState& state = Context::current().state().framebuffer;

    if(target == FramebufferTarget::Read) {
        if(state.readBinding == self._id) return;
        state.readBinding = self._id;
    } else if(target == FramebufferTarget::Draw) {
        if(state.drawBinding == self._id) return;
        state.drawBinding = self._id;
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* Binding the framebuffer finally creates it */
    self._flags |= ObjectFlag::Created;
    glBindFramebuffer(GLenum(target), self._id);
}

FramebufferTarget AbstractFramebuffer::bindInternal() {
    #ifndef MAGNUM_TARGET_GLES2
    return bindImplementationDefault(*this);
    #elif defined(MAGNUM_TARGET_WEBGL)
    return bindImplementationSingle(*this);
    #else
    return Context::current().state().framebuffer.bindInternalImplementation(*this);
    #endif
}

#ifdef MAGNUM_TARGET_GLES2
FramebufferTarget AbstractFramebuffer::bindImplementationSingle(AbstractFramebuffer& self) {
    Implementation::FramebufferState& state = Context::current().state().framebuffer;
    CORRADE_INTERNAL_ASSERT(state.readBinding == state.drawBinding);

    /* Bind the framebuffer, if not already */
    if(state.readBinding != self._id) {
        state.readBinding = state.drawBinding = self._id;

        /* Binding the framebuffer finally creates it */
        self._flags |= ObjectFlag::Created;
        glBindFramebuffer(GL_FRAMEBUFFER, self._id);
    }

    /* On ES2 w/o separate read/draw bindings the return value is used as a
       first parameter to glFramebufferRenderbuffer() etc. and so it needs to
       be unconditionally GL_FRAMEBUFFER. That value is not part of the public
       enum, though. */
    return FramebufferTarget(GL_FRAMEBUFFER);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
inline
#endif
FramebufferTarget AbstractFramebuffer::bindImplementationDefault(AbstractFramebuffer& self) {
    Implementation::FramebufferState& state = Context::current().state().framebuffer;

    /* Return target to which the framebuffer is already bound */
    if(state.readBinding == self._id)
        return FramebufferTarget::Read;
    if(state.drawBinding == self._id)
        return FramebufferTarget::Draw;

    /* Or bind it, if not already */
    state.readBinding = self._id;

    /* Binding the framebuffer finally creates it */
    self._flags |= ObjectFlag::Created;
    glBindFramebuffer(GLenum(FramebufferTarget::Read), self._id);
    return FramebufferTarget::Read;
}

PixelFormat AbstractFramebuffer::implementationColorReadFormat() {
    return PixelFormat(Context::current().state().framebuffer.implementationColorReadFormatTypeImplementation(*this, GL_IMPLEMENTATION_COLOR_READ_FORMAT));
}

PixelType AbstractFramebuffer::implementationColorReadType() {
    return PixelType(Context::current().state().framebuffer.implementationColorReadFormatTypeImplementation(*this, GL_IMPLEMENTATION_COLOR_READ_TYPE));
}

GLenum AbstractFramebuffer::implementationColorReadFormatTypeImplementationGlobal(AbstractFramebuffer& self, const GLenum what) {
    self.bindInternal(FramebufferTarget::Read);
    GLint formatType;
    glGetIntegerv(what, &formatType);
    return formatType;
}

#ifndef MAGNUM_TARGET_GLES
GLenum AbstractFramebuffer::implementationColorReadFormatTypeImplementationFramebuffer(AbstractFramebuffer& self, const GLenum what) {
    const FramebufferTarget target = self.bindInternal();
    GLint formatType;
    glGetFramebufferParameteriv(GLenum(target), what, &formatType);
    return formatType;
}

GLenum AbstractFramebuffer::implementationColorReadFormatTypeImplementationFramebufferDSA(AbstractFramebuffer& self, const GLenum what) {
    GLint formatType;
    glGetNamedFramebufferParameteriv(self._id, what, &formatType);
    return formatType;
}

GLenum AbstractFramebuffer::implementationColorReadFormatTypeImplementationFramebufferDSAMesa(AbstractFramebuffer& self, const GLenum what) {
    /* Mesa needs the framebuffer bound for read even with DSA. See the
       "mesa-implementation-color-read-format-dsa-explicit-binding" workaround
       for details. */
    self.bindInternal(FramebufferTarget::Read);
    return implementationColorReadFormatTypeImplementationFramebufferDSA(self, what);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractFramebuffer::blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, const FramebufferBlitMask mask, const FramebufferBlitFilter filter) {
    Context::current().state().framebuffer.blitImplementation(source, destination, sourceRectangle, destinationRectangle, mask, filter);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::blitImplementationDefault(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, const FramebufferBlitMask mask, const FramebufferBlitFilter filter) {
    source.bindInternal(FramebufferTarget::Read);
    destination.bindInternal(FramebufferTarget::Draw);
    glBlitFramebuffer(sourceRectangle.left(), sourceRectangle.bottom(), sourceRectangle.right(), sourceRectangle.top(), destinationRectangle.left(), destinationRectangle.bottom(), destinationRectangle.right(), destinationRectangle.top(), GLbitfield(mask), GLenum(filter));
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::blitImplementationDSA(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, const FramebufferBlitMask mask, const FramebufferBlitFilter filter) {
    glBlitNamedFramebuffer(source._id, destination._id, sourceRectangle.left(), sourceRectangle.bottom(), sourceRectangle.right(), sourceRectangle.top(), destinationRectangle.left(), destinationRectangle.bottom(), destinationRectangle.right(), destinationRectangle.top(), GLbitfield(mask), GLenum(filter));
}
#endif

#elif !defined(MAGNUM_TARGET_WEBGL)
void AbstractFramebuffer::blitImplementationANGLE(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, const FramebufferBlitMask mask, const FramebufferBlitFilter filter) {
    source.bindInternal(FramebufferTarget::Read);
    destination.bindInternal(FramebufferTarget::Draw);
    glBlitFramebufferANGLE(sourceRectangle.left(), sourceRectangle.bottom(), sourceRectangle.right(), sourceRectangle.top(), destinationRectangle.left(), destinationRectangle.bottom(), destinationRectangle.right(), destinationRectangle.top(), GLbitfield(mask), GLenum(filter));
}

void AbstractFramebuffer::blitImplementationNV(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, const FramebufferBlitMask mask, const FramebufferBlitFilter filter) {
    source.bindInternal(FramebufferTarget::Read);
    destination.bindInternal(FramebufferTarget::Draw);
    glBlitFramebufferNV(sourceRectangle.left(), sourceRectangle.bottom(), sourceRectangle.right(), sourceRectangle.top(), destinationRectangle.left(), destinationRectangle.bottom(), destinationRectangle.right(), destinationRectangle.top(), GLbitfield(mask), GLenum(filter));
}
#endif

Range2Di AbstractFramebuffer::viewport() const {
    /* For default framebuffer the viewport is stored inside the state tracker
       instead. See the _viewport variable docs for details. */
    return _id ? _viewport : Context::current().state().framebuffer.defaultViewport;
}

AbstractFramebuffer& AbstractFramebuffer::setViewport(const Range2Di& rectangle) {
    Implementation::FramebufferState& state = Context::current().state().framebuffer;

    CORRADE_INTERNAL_ASSERT(rectangle != Implementation::FramebufferState::DisengagedViewport);

    /* For default framebuffer the viewport is stored inside the state tracker
       instead. See the _viewport variable docs for details. */
    (_id ? _viewport : state.defaultViewport) = rectangle;

    /* Update the viewport if the framebuffer is currently bound */
    if(state.drawBinding == _id)
        setViewportInternal();

    return *this;
}

void AbstractFramebuffer::setViewportInternal() {
    Implementation::FramebufferState& state = Context::current().state().framebuffer;

    /* For default framebuffer the viewport is stored inside the state tracker
       instead. See the _viewport variable docs for details. */
    const Range2Di& viewport = _id ? _viewport : Context::current().state().framebuffer.defaultViewport;

    CORRADE_INTERNAL_ASSERT(viewport != Implementation::FramebufferState::DisengagedViewport);
    CORRADE_INTERNAL_ASSERT(state.drawBinding == _id);

    /* Already up-to-date, nothing to do */
    if(state.viewport == viewport)
        return;

    /* Update the state and viewport */
    state.viewport = viewport;
    glViewport(viewport.left(), viewport.bottom(), viewport.sizeX(), viewport.sizeY());
}

AbstractFramebuffer& AbstractFramebuffer::clear(const FramebufferClearMask mask) {
    bindInternal(FramebufferTarget::Draw);
    glClear(GLbitfield(mask));

    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
AbstractFramebuffer& AbstractFramebuffer::clearDepth(const Float depth) {
    Context::current().state().framebuffer.clearFImplementation(*this, GL_DEPTH, 0, &depth);
    return *this;
}

AbstractFramebuffer& AbstractFramebuffer::clearStencil(const Int stencil) {
    Context::current().state().framebuffer.clearIImplementation(*this, GL_STENCIL, 0, &stencil);
    return *this;
}

AbstractFramebuffer& AbstractFramebuffer::clearDepthStencil(const Float depth, const Int stencil) {
    Context::current().state().framebuffer.clearFIImplementation(*this, GL_DEPTH_STENCIL, depth, stencil);
    return *this;
}
#endif

void AbstractFramebuffer::read(const Range2Di& rectangle, const MutableImageView2D& image) {
    CORRADE_ASSERT(image.data().data() != nullptr || !rectangle.size().product(),
        "GL::AbstractFramebuffer::read(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == rectangle.size(),
        "GL::AbstractFramebuffer::read(): expected image view size" << rectangle.size() << "but got" << image.size(), );

    bindInternal(FramebufferTarget::Read);
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    #endif
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    (Context::current().state().framebuffer.readImplementation)(rectangle, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data().size(), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Magnum::Implementation::pixelStorageSkipOffsetFor(image, rectangle.size())
        #endif
        );
}

void AbstractFramebuffer::read(const Range2Di& rectangle, Image2D& image) {
    /* Reallocate only if needed */
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, rectangle.size());
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    /* Replace the storage, proxy to the function taking a view */
    image = Image2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), rectangle.size(), Utility::move(data), ImageFlags2D{}};
    read(rectangle, MutableImageView2D(image));
}

Image2D AbstractFramebuffer::read(const Range2Di& rectangle, Image2D&& image) {
    read(rectangle, image);
    return Utility::move(image);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::read(const Range2Di& rectangle, BufferImage2D& image, BufferUsage usage) {
    bindInternal(FramebufferTarget::Read);

    /* Reallocate only if needed */
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, rectangle.size());
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), rectangle.size(), {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), rectangle.size(), nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    (Context::current().state().framebuffer.readImplementation)(rectangle, image.format(), image.type(), dataSize, nullptr);
}

BufferImage2D AbstractFramebuffer::read(const Range2Di& rectangle, BufferImage2D&& image, BufferUsage usage) {
    read(rectangle, image, usage);
    return Utility::move(image);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copyImage(const Range2Di& rectangle, Texture1D& texture, const Int level, const TextureFormat internalFormat) {
    CORRADE_ASSERT(rectangle.sizeY() == 1, "GL::AbstractFramebuffer::copyImage(): height must be 1 for 1D textures", );
    bindInternal(FramebufferTarget::Read);
    texture.bindInternal();
    glCopyTexImage1D(GL_TEXTURE_1D, level, GLenum(internalFormat), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), 0);
}
#endif

void AbstractFramebuffer::copyImage(const Range2Di& rectangle, Texture2D& texture, const Int level, const TextureFormat internalFormat) {
    bindInternal(FramebufferTarget::Read);
    texture.bindInternal();
    glCopyTexImage2D(GL_TEXTURE_2D, level, GLenum(internalFormat), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), 0);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copyImage(const Range2Di& rectangle, RectangleTexture& texture, const TextureFormat internalFormat) {
    bindInternal(FramebufferTarget::Read);
    texture.bindInternal();
    glCopyTexImage2D(GL_TEXTURE_RECTANGLE, 0, GLenum(internalFormat), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), 0);
}
#endif

void AbstractFramebuffer::copyImage(const Range2Di& rectangle, CubeMapTexture& texture, const CubeMapCoordinate coordinate, const Int level, const TextureFormat internalFormat) {
    bindInternal(FramebufferTarget::Read);
    texture.bindInternal();
    glCopyTexImage2D(GLenum(coordinate), level, GLenum(internalFormat), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), 0);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copyImage(const Range2Di& rectangle, Texture1DArray& texture, const Int level, const TextureFormat internalFormat) {
    bindInternal(FramebufferTarget::Read);
    texture.bindInternal();
    glCopyTexImage2D(GL_TEXTURE_1D_ARRAY, level, GLenum(internalFormat), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), 0);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, Texture1D& texture, const Int level, const Int offset) {
    CORRADE_ASSERT(rectangle.sizeY() == 1, "GL::AbstractFramebuffer::copyImage(): height must be 1 for 1D textures", );
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub1DImplementation(rectangle, texture, level, offset);
}
#endif

void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, Texture2D& texture, const Int level, const Vector2i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub2DImplementation(rectangle, texture, GL_TEXTURE_2D, level, offset);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, RectangleTexture& texture, const Vector2i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub2DImplementation(rectangle, texture, GL_TEXTURE_RECTANGLE, 0, offset);
}
#endif

void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, CubeMapTexture& texture, const Int level, const Vector3i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySubCubeMapImplementation(rectangle, texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + offset.z(), level, offset.xy());
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, Texture3D& texture, const Int level, const Vector3i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub3DImplementation(rectangle, texture, level, offset);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, Texture1DArray& texture, const Int level, const Vector2i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub2DImplementation(rectangle, texture, GL_TEXTURE_1D_ARRAY, level, offset);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, Texture2DArray& texture, const Int level, const Vector3i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub3DImplementation(rectangle, texture, level, offset);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractFramebuffer::copySubImage(const Range2Di& rectangle, CubeMapTextureArray& texture, const Int level, const Vector3i& offset) {
    bindInternal(FramebufferTarget::Read);
    Context::current().state().framebuffer.copySub3DImplementation(rectangle, texture, level, offset);
}
#endif

void AbstractFramebuffer::invalidateImplementationNoOp(AbstractFramebuffer&, GLsizei, const GLenum* const) {}

void AbstractFramebuffer::invalidateImplementationDefault(AbstractFramebuffer& self, const GLsizei count, const GLenum* const attachments) {
    #ifndef MAGNUM_TARGET_GLES2
    glInvalidateFramebuffer(GLenum(self.bindInternal()), count, attachments);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
    glDiscardFramebufferEXT(GLenum(self.bindInternal()), count, attachments);
    #else
    static_cast<void>(self);
    static_cast<void>(count);
    static_cast<void>(attachments);
    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::invalidateImplementationDSA(AbstractFramebuffer& self, const GLsizei count, const GLenum* const attachments) {
    glInvalidateNamedFramebufferData(self._id, count, attachments);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::invalidateImplementationNoOp(AbstractFramebuffer&, GLsizei, const GLenum*, const Range2Di&) {}

void AbstractFramebuffer::invalidateImplementationDefault(AbstractFramebuffer& self, const GLsizei count, const GLenum* const attachments, const Range2Di& rectangle) {
    glInvalidateSubFramebuffer(GLenum(self.bindInternal()), count, attachments, rectangle.left(), rectangle.bottom(), rectangle.sizeX(), rectangle.sizeY());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::invalidateImplementationDSA(AbstractFramebuffer& self, const GLsizei count, const GLenum* const attachments, const Range2Di& rectangle) {
    glInvalidateNamedFramebufferSubData(self._id, count, attachments, rectangle.left(), rectangle.bottom(), rectangle.sizeX(), rectangle.sizeY());
}
#endif
#endif

GLenum AbstractFramebuffer::checkStatusImplementationDefault(AbstractFramebuffer& self, const FramebufferTarget target) {
    self.bindInternal(target);
    return glCheckFramebufferStatus(GLenum(target));
}

#ifdef MAGNUM_TARGET_GLES2
GLenum AbstractFramebuffer::checkStatusImplementationSingle(AbstractFramebuffer& self, FramebufferTarget) {
    self.bindInternal(FramebufferTarget{});
    return glCheckFramebufferStatus(GL_FRAMEBUFFER);
}
#endif

#ifndef MAGNUM_TARGET_GLES
GLenum AbstractFramebuffer::checkStatusImplementationDSA(AbstractFramebuffer& self, const FramebufferTarget target) {
    return glCheckNamedFramebufferStatus(self._id, GLenum(target));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::clearImplementationDefault(AbstractFramebuffer& self, const GLenum buffer, const GLint drawbuffer, const GLint* const value) {
    self.bindInternal(FramebufferTarget::Draw);
    glClearBufferiv(buffer, drawbuffer, value);
}

void AbstractFramebuffer::clearImplementationDefault(AbstractFramebuffer& self, const GLenum buffer, const GLint drawbuffer, const GLuint* const value) {
    self.bindInternal(FramebufferTarget::Draw);
    glClearBufferuiv(buffer, drawbuffer, value);
}

void AbstractFramebuffer::clearImplementationDefault(AbstractFramebuffer& self, const GLenum buffer, const GLint drawbuffer, const GLfloat* const value) {
    self.bindInternal(FramebufferTarget::Draw);
    glClearBufferfv(buffer, drawbuffer, value);
}

void AbstractFramebuffer::clearImplementationDefault(AbstractFramebuffer& self, const GLenum buffer, const GLfloat depth, const GLint stencil) {
    self.bindInternal(FramebufferTarget::Draw);
    glClearBufferfi(buffer, 0, depth, stencil);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::clearImplementationDSA(AbstractFramebuffer& self, const GLenum buffer, const GLint drawbuffer, const GLint* const value) {
    glClearNamedFramebufferiv(self._id, buffer, drawbuffer, value);
}

void AbstractFramebuffer::clearImplementationDSA(AbstractFramebuffer& self, const GLenum buffer, const GLint drawbuffer, const GLuint* const value) {
    glClearNamedFramebufferuiv(self._id, buffer, drawbuffer, value);
}

void AbstractFramebuffer::clearImplementationDSA(AbstractFramebuffer& self, const GLenum buffer, const GLint drawbuffer, const GLfloat* const value) {
    glClearNamedFramebufferfv(self._id, buffer, drawbuffer, value);
}

void AbstractFramebuffer::clearImplementationDSA(AbstractFramebuffer& self, const GLenum buffer, const GLfloat depth, const GLint stencil) {
    glClearNamedFramebufferfi(self._id, buffer, 0, depth, stencil);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::drawBuffersImplementationDefault(AbstractFramebuffer& self, GLsizei count, const GLenum* buffers) {
    self.bindInternal(FramebufferTarget::Draw);

    glDrawBuffers(count, buffers);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::drawBuffersImplementationDSA(AbstractFramebuffer& self, const GLsizei count, const GLenum* const buffers) {
    glNamedFramebufferDrawBuffers(self._id, count, buffers);
}
#endif
#else
void AbstractFramebuffer::drawBuffersImplementationEXT(AbstractFramebuffer& self, GLsizei count, const GLenum* buffers) {
    self.bindInternal(FramebufferTarget::Draw);
    glDrawBuffersEXT(count, buffers);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractFramebuffer::drawBuffersImplementationNV(AbstractFramebuffer& self, GLsizei count, const GLenum* buffers) {
    self.bindInternal(FramebufferTarget::Draw);
    glDrawBuffersNV(count, buffers);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::drawBufferImplementationDefault(AbstractFramebuffer& self, GLenum buffer) {
    self.bindInternal(FramebufferTarget::Draw);

    glDrawBuffer(buffer);
}

void AbstractFramebuffer::drawBufferImplementationDSA(AbstractFramebuffer& self, const GLenum buffer) {
    glNamedFramebufferDrawBuffer(self._id, buffer);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractFramebuffer::readBufferImplementationDefault(AbstractFramebuffer& self, GLenum buffer) {
    self.bindInternal(FramebufferTarget::Read);

    #ifndef MAGNUM_TARGET_GLES2
    glReadBuffer(buffer);
    #else
    glReadBufferNV(buffer);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::readBufferImplementationDSA(AbstractFramebuffer& self, const GLenum buffer) {
    glNamedFramebufferReadBuffer(self._id, buffer);
}
#endif

void AbstractFramebuffer::readImplementationDefault(const Range2Di& rectangle, const PixelFormat format, const PixelType type, const std::size_t, GLvoid* const data) {
    glReadPixels(rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), GLenum(format), GLenum(type), data);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractFramebuffer::readImplementationRobustness(const Range2Di& rectangle, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    #ifndef MAGNUM_TARGET_GLES
    glReadnPixelsARB(rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), GLenum(format), GLenum(type), dataSize, data);
    #else
    glReadnPixelsEXT(rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY(), GLenum(format), GLenum(type), dataSize, data);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copySub1DImplementationDefault(const Range2Di& rectangle, AbstractTexture& texture, const Int level, const Int offset) {
    texture.bindInternal();
    glCopyTexSubImage1D(texture._target, level, offset, rectangle.min().x(), rectangle.min().y(), rectangle.sizeX());
}

void AbstractFramebuffer::copySub1DImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, const Int level, const Int offset) {
    glCopyTextureSubImage1D(texture._id, level, offset, rectangle.min().x(), rectangle.min().y(), rectangle.sizeX());
}
#endif

void AbstractFramebuffer::copySub2DImplementationDefault(const Range2Di& rectangle, AbstractTexture& texture, const GLenum target, const Int level, const Vector2i& offset) {
    texture.bindInternal();
    glCopyTexSubImage2D(target, level, offset.x(), offset.y(), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copySub2DImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, const GLenum, const Int level, const Vector2i& offset) {
    glCopyTextureSubImage2D(texture._id, level, offset.x(), offset.y(), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY());
}

void AbstractFramebuffer::copySubCubeMapImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, const GLenum target, const Int level, const Vector2i& offset) {
    glCopyTextureSubImage3D(texture._id, level, offset.x(), offset.y(), target - GL_TEXTURE_CUBE_MAP_POSITIVE_X, rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY());
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void AbstractFramebuffer::copySub3DImplementationDefault(const Range2Di& rectangle, AbstractTexture& texture, const Int level, const Vector3i& offset) {
    texture.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glCopyTexSubImage3D
    #else
    glCopyTexSubImage3DOES
    #endif
        (texture._target, level, offset.x(), offset.y(), offset.z(), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::copySub3DImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, const Int level, const Vector3i& offset) {
    glCopyTextureSubImage3D(texture._id, level, offset.x(), offset.y(), offset.z(), rectangle.min().x(), rectangle.min().y(), rectangle.sizeX(), rectangle.sizeY());
}
#endif

}}
