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

#include "Renderer.h"

#include "Color.h"
#include "Math/Geometry/Rectangle.h"
#include "Context.h"
#include "Extensions.h"
#include "Implementation/State.h"
#include "Implementation/RendererState.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES
Renderer::ClearDepthfImplementation Renderer::clearDepthfImplementation = &Renderer::clearDepthfImplementationDefault;
#else
Renderer::ClearDepthfImplementation Renderer::clearDepthfImplementation = &Renderer::clearDepthfImplementationES;
#endif
#ifndef MAGNUM_TARGET_GLES3
Renderer::GraphicsResetStatusImplementation Renderer::graphicsResetStatusImplementation = &Renderer::graphicsResetStatusImplementationDefault;
#endif

void Renderer::setFeature(const Feature feature, const bool enabled) {
    enabled ? glEnable(GLenum(feature)) : glDisable(GLenum(feature));
}

void Renderer::setHint(const Hint target, const HintMode mode) {
    glHint(GLenum(target), GLenum(mode));
}

void Renderer::setClearColor(const Color4& color) {
    glClearColor(color.r(), color.g(), color.b(), color.a());
}

#ifndef MAGNUM_TARGET_GLES
void Renderer::setClearDepth(const Double depth) {
    glClearDepth(depth);
}
#endif

void Renderer::setClearStencil(const Int stencil) {
    glClearStencil(stencil);
}

void Renderer::setFrontFace(const FrontFace mode) {
    glFrontFace(GLenum(mode));
}

void Renderer::setFaceCullingMode(const PolygonFacing mode) {
    glCullFace(GLenum(mode));
}

#ifndef MAGNUM_TARGET_GLES
void Renderer::setProvokingVertex(const ProvokingVertex mode) {
    glProvokingVertex(GLenum(mode));
}

void Renderer::setPolygonMode(const PolygonMode mode) {
    glPolygonMode(GL_FRONT_AND_BACK, GLenum(mode));
}
#endif

void Renderer::setPolygonOffset(const Float factor, const Float units) {
    glPolygonOffset(factor, units);
}

void Renderer::setLineWidth(const Float width) {
    glLineWidth(width);
}

#ifndef MAGNUM_TARGET_GLES
void Renderer::setPointSize(const Float size) {
    glPointSize(size);
}
#endif

void Renderer::setScissor(const Rectanglei& rectangle) {
    glScissor(rectangle.left(), rectangle.bottom(), rectangle.width(), rectangle.height());
}

void Renderer::setStencilFunction(const PolygonFacing facing, const StencilFunction function, const Int referenceValue, const UnsignedInt mask) {
    glStencilFuncSeparate(GLenum(facing), GLenum(function), referenceValue, mask);
}

void Renderer::setStencilFunction(const StencilFunction function, const Int referenceValue, const UnsignedInt mask) {
    glStencilFunc(GLenum(function), referenceValue, mask);
}

void Renderer::setStencilOperation(const PolygonFacing facing, const StencilOperation stencilFail, const StencilOperation depthFail, const StencilOperation depthPass) {
    glStencilOpSeparate(GLenum(facing), GLenum(stencilFail), GLenum(depthFail), GLenum(depthPass));
}

void Renderer::setStencilOperation(const StencilOperation stencilFail, const StencilOperation depthFail, const StencilOperation depthPass) {
    glStencilOp(GLenum(stencilFail), GLenum(depthFail), GLenum(depthPass));
}

void Renderer::setDepthFunction(const DepthFunction function) {
    glDepthFunc(GLenum(function));
}

void Renderer::setColorMask(const GLboolean allowRed, const GLboolean allowGreen, const GLboolean allowBlue, const GLboolean allowAlpha) {
    glColorMask(allowRed, allowGreen, allowBlue, allowAlpha);
}

void Renderer::setDepthMask(const GLboolean allow) {
    glDepthMask(allow);
}

void Renderer::setStencilMask(const PolygonFacing facing, const UnsignedInt allowBits) {
    glStencilMaskSeparate(GLenum(facing), allowBits);
}

void Renderer::setStencilMask(const UnsignedInt allowBits) {
    glStencilMask(allowBits);
}

void Renderer::setBlendEquation(const BlendEquation equation) {
    glBlendEquation(GLenum(equation));
}

void Renderer::setBlendEquation(const BlendEquation rgb, const BlendEquation alpha) {
    glBlendEquationSeparate(GLenum(rgb), GLenum(alpha));
}

void Renderer::setBlendFunction(const BlendFunction source, const BlendFunction destination) {
    glBlendFunc(GLenum(source), GLenum(destination));
}

void Renderer::setBlendFunction(const BlendFunction sourceRgb, const BlendFunction destinationRgb, const BlendFunction sourceAlpha, const BlendFunction destinationAlpha) {
    glBlendFuncSeparate(GLenum(sourceRgb), GLenum(destinationRgb), GLenum(sourceAlpha), GLenum(destinationAlpha));
}

void Renderer::setBlendColor(const Color4& color) {
    glBlendColor(color.r(), color.g(), color.b(), color.a());
}

#ifndef MAGNUM_TARGET_GLES
void Renderer::setLogicOperation(const LogicOperation operation) {
    glLogicOp(GLenum(operation));
}
#endif

#ifndef MAGNUM_TARGET_GLES3
Renderer::ResetNotificationStrategy Renderer::resetNotificationStrategy() {
    ResetNotificationStrategy& strategy = Context::current()->state()->renderer->resetNotificationStrategy;

    if(strategy == ResetNotificationStrategy()) {
        #ifndef MAGNUM_TARGET_GLES
        glGetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, reinterpret_cast<GLint*>(&strategy));
        #else
        glGetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_EXT, reinterpret_cast<GLint*>(&strategy));
        #endif
    }

    return strategy;
}
#endif

void Renderer::initializeContextBasedFunctionality(Context* context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::ARB::ES2_compatibility>()) {
        Debug() << "Renderer: using" << Extensions::GL::ARB::ES2_compatibility::string() << "features";

        clearDepthfImplementation = &Renderer::clearDepthfImplementationES;
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::ARB::robustness>())
    #else
    if(context->isExtensionSupported<Extensions::GL::EXT::robustness>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        Debug() << "Renderer: using" << Extensions::GL::ARB::robustness::string() << "features";
        #else
        Debug() << "Renderer: using" << Extensions::GL::EXT::robustness::string() << "features";
        #endif

        graphicsResetStatusImplementation = &Renderer::graphicsResetStatusImplementationRobustness;
    }
    #else
    static_cast<void>(context);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Renderer::clearDepthfImplementationDefault(const GLfloat depth) {
    glClearDepth(depth);
}
#endif

void Renderer::clearDepthfImplementationES(const GLfloat depth) {
    glClearDepthf(depth);
}

#ifndef MAGNUM_TARGET_GLES3
Renderer::GraphicsResetStatus Renderer::graphicsResetStatusImplementationDefault() {
    return GraphicsResetStatus::NoError;
}

Renderer::GraphicsResetStatus Renderer::graphicsResetStatusImplementationRobustness() {
    /** @todo Enable when extension wrangler for ES is available */
    #ifndef MAGNUM_TARGET_GLES
    return GraphicsResetStatus(glGetGraphicsResetStatusARB());
    #else
    //return GraphicsResetStatus(glGetGraphicsResetStatusEXT());
    CORRADE_INTERNAL_ASSERT(false);
    #endif
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, const Renderer::Error value) {
    switch(value) {
        #define _c(value) case Renderer::Error::value: return debug << "Renderer::Error::" #value;
        _c(NoError)
        _c(InvalidEnum)
        _c(InvalidValue)
        _c(InvalidOperation)
        _c(InvalidFramebufferOperation)
        _c(OutOfMemory)
        #ifndef MAGNUM_TARGET_GLES3
        _c(StackUnderflow)
        _c(StackOverflow)
        #endif
        #undef _c
    }

    return debug << "Renderer::Error::(invalid)";
}

#ifndef MAGNUM_TARGET_GLES3
Debug operator<<(Debug debug, const Renderer::ResetNotificationStrategy value) {
    switch(value) {
        #define _c(value) case Renderer::ResetNotificationStrategy::value: return debug << "Renderer::ResetNotificationStrategy::" #value;
        _c(NoResetNotification)
        _c(LoseContextOnReset)
        #undef _c
    }

    return debug << "Renderer::ResetNotificationStrategy::(invalid)";
}

Debug operator<<(Debug debug, const Renderer::GraphicsResetStatus value) {
    switch(value) {
        #define _c(value) case Renderer::GraphicsResetStatus::value: return debug << "Renderer::GraphicsResetStatus::" #value;
        _c(NoError)
        _c(GuiltyContextReset)
        _c(InnocentContextReset)
        _c(UnknownContextReset)
        #undef _c
    }

    return debug << "Renderer::ResetNotificationStrategy::(invalid)";
}
#endif
#endif

}
