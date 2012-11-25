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

#include "AbstractXApplication.h"

#include <Utility/utilities.h>

#include "Context.h"
#include "ExtensionWrangler.h"

#define None 0L // redef Xlib nonsense

/* Mask for X events */
#define INPUT_MASK KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|StructureNotifyMask

using namespace std;

namespace Magnum { namespace Platform {

AbstractXApplication::AbstractXApplication(AbstractContextHandler<Display*, VisualID, Window>* contextHandler, int&, char**, const string& title, const Math::Vector2<GLsizei>& size): contextHandler(contextHandler), viewportSize(size), flags(Flag::Redraw) {
    /* Get default X display */
    display = XOpenDisplay(0);

    /* Get visual ID */
    VisualID visualId = contextHandler->getVisualId(display);

    /* Get visual info */
    XVisualInfo *visInfo, visTemplate;
    int visualCount;
    visTemplate.visualid = visualId;
    visInfo = XGetVisualInfo(display, VisualIDMask, &visTemplate, &visualCount);
    if(!visInfo) {
        Error() << "Cannot get X visual";
        ::exit(1);
    }

    /* Create X Window */
    Window root = RootWindow(display, DefaultScreen(display));
    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(display, root, visInfo->visual, AllocNone);
    attr.event_mask = 0;
    unsigned long mask = CWBackPixel|CWBorderPixel|CWColormap|CWEventMask;
    window = XCreateWindow(display, root, 20, 20, size.x(), size.y(), 0, visInfo->depth, InputOutput, visInfo->visual, mask, &attr);
    XSetStandardProperties(display, window, title.c_str(), 0, None, 0, 0, 0);
    XFree(visInfo);

    /* Be notified about closing the window */
    deleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(display, window, &deleteWindow, 1);

    /* Create context */
    contextHandler->createContext(window);

    /* Capture exposure, keyboard and mouse button events */
    XSelectInput(display, window, INPUT_MASK);

    /* Set OpenGL context as current */
    contextHandler->makeCurrent();

    /* Initialize extension wrangler */
    ExtensionWrangler::initialize(contextHandler->experimentalExtensionWranglerFeatures());

    c = new Context;
}

AbstractXApplication::~AbstractXApplication() {
    delete c;

    /* Shut down context handler */
    delete contextHandler;

    /* Shut down X */
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int AbstractXApplication::exec() {
    /* Show window */
    XMapWindow(display, window);

    /* Call viewportEvent for the first time */
    viewportEvent(viewportSize);

    while(!(flags & Flag::Exit)) {
        XEvent event;

        /* Closed window */
        if(XCheckTypedWindowEvent(display, window, ClientMessage, &event) &&
           Atom(event.xclient.data.l[0]) == deleteWindow) {
            return 0;
        }

        while(XCheckWindowEvent(display, window, INPUT_MASK, &event)) {
            switch(event.type) {
                /* Window resizing */
                case ConfigureNotify: {
                    Math::Vector2<GLsizei> size(event.xconfigure.width, event.xconfigure.height);
                    if(size != viewportSize) {
                        viewportSize = size;
                        viewportEvent(size);
                        flags |= Flag::Redraw;
                    }
                } break;

                /* Key/mouse events */
                case KeyPress:
                    keyPressEvent(static_cast<Key>(XLookupKeysym(&event.xkey, 0)), static_cast<Modifier>(event.xkey.state), {event.xkey.x, event.xkey.y});
                    break;
                case KeyRelease:
                    keyReleaseEvent(static_cast<Key>(XLookupKeysym(&event.xkey, 0)), static_cast<Modifier>(event.xkey.state), {event.xkey.x, event.xkey.y});
                    break;
                case ButtonPress:
                    mousePressEvent(static_cast<MouseButton>(event.xbutton.button), static_cast<Modifier>(event.xkey.state), {event.xbutton.x, event.xbutton.y});
                    break;
                case ButtonRelease:
                    mouseReleaseEvent(static_cast<MouseButton>(event.xbutton.button), static_cast<Modifier>(event.xkey.state), {event.xbutton.x, event.xbutton.y});
                    break;

                /* Mouse move events */
                case MotionNotify:
                    mouseMotionEvent(static_cast<Modifier>(event.xmotion.state), {event.xmotion.x, event.xmotion.y});
                    break;
            }
        }

        if(flags & Flag::Redraw) {
            flags &= ~Flag::Redraw;
            drawEvent();
        } else Corrade::Utility::sleep(5);
    }

    return 0;
}

}}
