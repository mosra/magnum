/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/Utility/System.h>

#include "Magnum/Platform/Context.h"
#include "Magnum/Version.h"

#include "Implementation/AbstractContextHandler.h"

/* Mask for X events */
#define INPUT_MASK KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|StructureNotifyMask

namespace Magnum { namespace Platform {

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, const Configuration& configuration): AbstractXApplication{contextHandler, arguments, NoCreate} {
    createContext(configuration);
}

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, NoCreateT): _contextHandler{contextHandler}, _context{new Context{NoCreate, arguments.argc, arguments.argv}}, _flags{Flag::Redraw} {}

void AbstractXApplication::createContext() { createContext({}); }

void AbstractXApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool AbstractXApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::AbstractXApplication::tryCreateContext(): context already created", false);

    _viewportSize = configuration.size();

    /* Get default X display */
    _display = XOpenDisplay(nullptr);

    /* Get visual ID */
    VisualID visualId = _contextHandler->getVisualId(_display);

    /* Get visual info */
    XVisualInfo *visInfo, visTemplate;
    int visualCount;
    visTemplate.visualid = visualId;
    visInfo = XGetVisualInfo(_display, VisualIDMask, &visTemplate, &visualCount);
    if(!visInfo) {
        Error() << "Platform::WindowlessGlxApplication::tryCreateContext(): cannot get X visual";
        return false;
    }

    /* Create X Window */
    Window root = RootWindow(_display, DefaultScreen(_display));
    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(_display, root, visInfo->visual, AllocNone);
    attr.event_mask = 0;
    unsigned long mask = CWBackPixel|CWBorderPixel|CWColormap|CWEventMask;
    _window = XCreateWindow(_display, root, 20, 20, configuration.size().x(), configuration.size().y(), 0, visInfo->depth, InputOutput, visInfo->visual, mask, &attr);
    XSetStandardProperties(_display, _window, configuration.title().data(), nullptr, 0, nullptr, 0, nullptr);
    XFree(visInfo);

    /* Be notified about closing the window */
    _deleteWindow = XInternAtom(_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(_display, _window, &_deleteWindow, 1);

    /* Create context */
    _contextHandler->createContext(configuration, _window);

    /* Capture exposure, keyboard and mouse button events */
    XSelectInput(_display, _window, INPUT_MASK);

    /* Set OpenGL context as current */
    _contextHandler->makeCurrent();

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

AbstractXApplication::~AbstractXApplication() {
    _context.reset();

    /* Shut down context handler */
    _contextHandler.reset();

    /* Shut down X */
    XDestroyWindow(_display, _window);
    XCloseDisplay(_display);
}

void AbstractXApplication::swapBuffers() {
    _contextHandler->swapBuffers();
}

int AbstractXApplication::exec() {
    /* Show window */
    XMapWindow(_display, _window);

    while(!(_flags & Flag::Exit)) {
        XEvent event;

        /* Closed window */
        if(XCheckTypedWindowEvent(_display, _window, ClientMessage, &event) &&
           Atom(event.xclient.data.l[0]) == _deleteWindow) {
            return 0;
        }

        while(XCheckWindowEvent(_display, _window, INPUT_MASK, &event)) {
            switch(event.type) {
                /* Window resizing */
                case ConfigureNotify: {
                    Vector2i size(event.xconfigure.width, event.xconfigure.height);
                    if(size != _viewportSize) {
                        _viewportSize = size;
                        viewportEvent(size);
                        _flags |= Flag::Redraw;
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

        if(_flags & Flag::Redraw) {
            _flags &= ~Flag::Redraw;
            drawEvent();
        } else Utility::System::sleep(5);
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
