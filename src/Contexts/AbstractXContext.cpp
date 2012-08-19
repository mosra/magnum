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

#include "AbstractXContext.h"

#include "ExtensionWrangler.h"

#define None 0L // redef Xlib nonsense

/* Mask for X events */
#define INPUT_MASK KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|StructureNotifyMask

using namespace std;

namespace Magnum { namespace Contexts {

AbstractXContext::AbstractXContext(AbstractGlInterface<Display*, VisualID, Window>* glInterface, int&, char**, const string& title, const Math::Vector2<GLsizei>& size): glInterface(glInterface), viewportSize(size), _redraw(true) {
    /* Get default X display */
    display = XOpenDisplay(0);

    /* Get visual ID */
    VisualID visualId = glInterface->getVisualId(display);

    /* Get visual info */
    XVisualInfo *visInfo, visTemplate;
    int visualCount;
    visTemplate.visualid = visualId;
    visInfo = XGetVisualInfo(display, VisualIDMask, &visTemplate, &visualCount);
    if(!visInfo) {
        Error() << "Cannot get X visual";
        exit(1);
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
    glInterface->createContext(window);

    /* Capture exposure, keyboard and mouse button events */
    XSelectInput(display, window, INPUT_MASK);

    /* Set OpenGL context as current */
    glInterface->makeCurrent();

    /* Initialize extension wrangler */
    ExtensionWrangler::initialize(glInterface->experimentalExtensionWranglerFeatures());
}

AbstractXContext::~AbstractXContext() {
    /* Shut down the interface */
    delete glInterface;

    /* Shut down X */
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int AbstractXContext::exec() {
    /* Show window */
    XMapWindow(display, window);

    /* Call viewportEvent for the first time */
    viewportEvent(viewportSize);

    while(true) {
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
                    Math::Vector2<int> size(event.xconfigure.width, event.xconfigure.height);
                    if(size != viewportSize) {
                        viewportSize = size;
                        viewportEvent(size);
                        _redraw = true;
                    }
                } break;

                /* Key/mouse events */
                case KeyPress:
                    keyPressEvent(static_cast<Key>(XLookupKeysym(&event.xkey, 0)), {event.xkey.x, event.xkey.y});
                    break;
                case KeyRelease:
                    keyReleaseEvent(static_cast<Key>(XLookupKeysym(&event.xkey, 0)), {event.xkey.x, event.xkey.y});
                    break;
                case ButtonPress:
                    mousePressEvent(static_cast<MouseButton>(event.xbutton.button), {event.xbutton.x, event.xbutton.y});
                    break;
                case ButtonRelease:
                    mouseReleaseEvent(static_cast<MouseButton>(event.xbutton.button), {event.xbutton.x, event.xbutton.y});
                    break;
            }
        }

        if(_redraw) {
            _redraw = false;
            drawEvent();
        } else Corrade::Utility::sleep(5);
    }

    return 0;
}

}}
