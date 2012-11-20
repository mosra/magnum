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

#include "NaClApplication.h"

#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/lib/gl/gles2/gl2ext_ppapi.h>

#include "Context.h"

namespace Magnum { namespace Platform {

NaClApplication::NaClApplication(PP_Instance instance, const Math::Vector2<GLsizei>& size): Instance(instance), Graphics3DClient(this), viewportSize(size) {
    int32_t attributes[] = {
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
        PP_GRAPHICS3DATTRIB_SAMPLES, 0,
        PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
        PP_GRAPHICS3DATTRIB_WIDTH, size.x(),
        PP_GRAPHICS3DATTRIB_HEIGHT, size.y(),
        PP_GRAPHICS3DATTRIB_NONE
    };

    graphics = new pp::Graphics3D(this, attributes);
    if(graphics->is_null()) {
        Error() << "Platform::NaClApplication::NaClApplication(): cannot create graphics";
        exit(1);
    }
    if(!BindGraphics(*graphics)) {
        Error() << "Platform::NaClApplication::NaClApplication(): cannot bind graphics";
        exit(1);
    }

    glSetCurrentContextPPAPI(graphics->pp_resource());

    c = new Context;

    /* Enable input handling for mouse and keyboard */
    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE|PP_INPUTEVENT_CLASS_WHEEL);
    RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);
}

NaClApplication::~NaClApplication() {
    delete c;
    delete graphics;
}

void NaClApplication::DidChangeView(const pp::View& view) {
    Math::Vector2<GLsizei> size(view.GetRect().width(), view.GetRect().height());

    /* Canvas resized */
    if(viewportSize != size) {
        graphics->ResizeBuffers(size.x(), size.y());
        viewportSize = size;
        flags |= Flag::ViewportUpdated;
    }

    /* Update viewport, if changed */
    if(!(flags & Flag::ViewportUpdated)) {
        flags &= ~Flag::ViewportUpdated;
        viewportEvent(size);
    }

    drawEvent();
}

bool NaClApplication::HandleInputEvent(const pp::InputEvent& event) {
    Flags tmpFlags = flags;

    switch(event.GetType()) {
        case PP_INPUTEVENT_TYPE_KEYDOWN:
        case PP_INPUTEVENT_TYPE_KEYUP: {
            pp::KeyboardInputEvent keyEvent(event);
            KeyEvent e(static_cast<KeyEvent::Key>(keyEvent.GetKeyCode()), static_cast<InputEvent::Modifier>(keyEvent.GetModifiers()));
            event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
            if(!e.isAccepted()) return false;
            break;
        }

        case PP_INPUTEVENT_TYPE_MOUSEDOWN:
        case PP_INPUTEVENT_TYPE_MOUSEUP: {
            pp::MouseInputEvent mouseEvent(event);
            MouseEvent e(static_cast<MouseEvent::Button>(mouseEvent.GetButton()), {mouseEvent.GetPosition().x(), mouseEvent.GetPosition().y()}, static_cast<InputEvent::Modifier>(mouseEvent.GetModifiers()));
            event.GetType() == PP_INPUTEVENT_TYPE_MOUSEDOWN ? mousePressEvent(e) : mouseReleaseEvent(e);
            if(!e.isAccepted()) return false;
            break;
        }

        case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
            pp::MouseInputEvent mouseEvent(event);
            MouseMoveEvent e({mouseEvent.GetPosition().x(), mouseEvent.GetPosition().y()}, static_cast<InputEvent::Modifier>(mouseEvent.GetModifiers()));
            mouseMoveEvent(e);
            if(!e.isAccepted()) return false;
            break;
        }

        default: return false;
    }

    /* Assume everything is properly sequential here */
    CORRADE_INTERNAL_ASSERT((tmpFlags & Flag::SwapInProgress) == (flags & Flag::SwapInProgress));

    /* Redraw, if it won't be handled after swap automatically */
    if((flags & Flag::Redraw) && !(flags & Flag::SwapInProgress)) {
        flags &= ~Flag::Redraw;
        drawEvent();
    }

    return true;
}

void NaClApplication::swapBuffers() {
    /* Swap already in progress, do nothing */
    if(flags & Flag::SwapInProgress) return;

    /* Swap buffers and call swapCallback() when done */
    flags |= Flag::SwapInProgress;
    graphics->SwapBuffers(pp::CompletionCallback(&swapCallback, this));
}

void NaClApplication::swapCallback(void* applicationInstance, std::int32_t) {
    NaClApplication* instance = static_cast<NaClApplication*>(applicationInstance);
    instance->flags &= ~Flag::SwapInProgress;

    /* Redraw, if requested */
    if(instance->flags & Flag::Redraw) {
        instance->flags &= ~Flag::Redraw;
        instance->drawEvent();
    }
}

}}
