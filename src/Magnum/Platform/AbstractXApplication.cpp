/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "AbstractXApplication.h"

#include <Corrade/Utility/utilities.h>

#include "Magnum/Platform/Context.h"
#include "Magnum/Version.h"

#include "Implementation/AbstractContextHandler.h"

/* Mask for X events */
#define INPUT_MASK KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|StructureNotifyMask

namespace Magnum { namespace Platform {

/** @todo Delegating constructor when support for GCC 4.6 is dropped */

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments&, const Configuration& configuration): contextHandler(contextHandler), c(nullptr), flags(Flag::Redraw) {
    createContext(configuration);
}

#ifndef CORRADE_GCC45_COMPATIBILITY
AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments&, std::nullptr_t)
#else
AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments&, void*)
#endif
    : contextHandler(contextHandler), c(nullptr), flags(Flag::Redraw) {}

void AbstractXApplication::createContext() { createContext({}); }

void AbstractXApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool AbstractXApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(!c, "AbstractXApplication::tryCreateContext(): context already created", false);

    viewportSize = configuration.size();

    /* Get default X display */
    display = XOpenDisplay(nullptr);

    /* Get visual ID */
    VisualID visualId = contextHandler->getVisualId(display);

    /* Get visual info */
    XVisualInfo *visInfo, visTemplate;
    int visualCount;
    visTemplate.visualid = visualId;
    visInfo = XGetVisualInfo(display, VisualIDMask, &visTemplate, &visualCount);
    if(!visInfo) {
        Error() << "Platform::WindowlessGlxApplication::tryCreateContext(): cannot get X visual";
        return false;
    }

    /* Create X Window */
    Window root = RootWindow(display, DefaultScreen(display));
    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(display, root, visInfo->visual, AllocNone);
    attr.event_mask = 0;
    unsigned long mask = CWBackPixel|CWBorderPixel|CWColormap|CWEventMask;
    window = XCreateWindow(display, root, 20, 20, configuration.size().x(), configuration.size().y(), 0, visInfo->depth, InputOutput, visInfo->visual, mask, &attr);
    XSetStandardProperties(display, window, configuration.title().data(), nullptr, 0, nullptr, 0, nullptr);
    XFree(visInfo);

    /* Be notified about closing the window */
    deleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(display, window, &deleteWindow, 1);

    /* Create context */
    contextHandler->createContext(configuration, window);

    /* Capture exposure, keyboard and mouse button events */
    XSelectInput(display, window, INPUT_MASK);

    /* Set OpenGL context as current */
    contextHandler->makeCurrent();

    c = new Platform::Context;
    return true;
}

AbstractXApplication::~AbstractXApplication() {
    delete c;

    /* Shut down context handler */
    delete contextHandler;

    /* Shut down X */
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void AbstractXApplication::swapBuffers() {
    contextHandler->swapBuffers();
}

int AbstractXApplication::exec() {
    /* Show window */
    XMapWindow(display, window);

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
                    Vector2i size(event.xconfigure.width, event.xconfigure.height);
                    if(size != viewportSize) {
                        viewportSize = size;
                        viewportEvent(size);
                        flags |= Flag::Redraw;
                    }
                } break;

                /* Key/mouse events */
                case KeyPress:
                case KeyRelease: {
                    KeyEvent e(static_cast<KeyEvent::Key>(XLookupKeysym(&event.xkey, 0)), static_cast<InputEvent::Modifier>(event.xkey.state), {event.xkey.x, event.xkey.y});
                    event.type == KeyPress ? keyPressEvent(e) : keyReleaseEvent(e);
                } break;
                case ButtonPress:
                case ButtonRelease: {
                    MouseEvent e(static_cast<MouseEvent::Button>(event.xbutton.button), static_cast<InputEvent::Modifier>(event.xkey.state), {event.xbutton.x, event.xbutton.y});
                    event.type == ButtonPress ? mousePressEvent(e) : mouseReleaseEvent(e);
                } break;

                /* Mouse move events */
                case MotionNotify: {
                    MouseMoveEvent e(static_cast<InputEvent::Modifier>(event.xmotion.state), {event.xmotion.x, event.xmotion.y});
                    mouseMoveEvent(e);
                } break;
            }
        }

        if(flags & Flag::Redraw) {
            flags &= ~Flag::Redraw;
            drawEvent();
        } else Utility::sleep(5);
    }

    return 0;
}

void AbstractXApplication::viewportEvent(const Vector2i&) {}
void AbstractXApplication::keyPressEvent(KeyEvent&) {}
void AbstractXApplication::keyReleaseEvent(KeyEvent&) {}
void AbstractXApplication::mousePressEvent(MouseEvent&) {}
void AbstractXApplication::mouseReleaseEvent(MouseEvent&) {}
void AbstractXApplication::mouseMoveEvent(MouseMoveEvent&) {}

AbstractXApplication::Configuration::Configuration(): _title("Magnum X Application"), _size(800, 600), _version(Version::None) {}
AbstractXApplication::Configuration::~Configuration() = default;

}}
