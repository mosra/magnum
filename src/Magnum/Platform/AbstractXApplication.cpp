/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"

#include "Implementation/AbstractContextHandler.h"

/* Mask for X events */
#define INPUT_MASK KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|StructureNotifyMask

namespace Magnum { namespace Platform {

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): AbstractXApplication{contextHandler, arguments, NoCreate} {
    create(configuration, glConfiguration);
}

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, NoCreateT): _contextHandler{contextHandler}, _context{new GLContext{NoCreate, arguments.argc, arguments.argv}}, _flags{Flag::Redraw} {}

void AbstractXApplication::create() { create({}); }

void AbstractXApplication::create(const Configuration& configuration) {
    create(configuration, GLConfiguration{});
}

void AbstractXApplication::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration)) std::exit(1);
}

bool AbstractXApplication::tryCreate(const Configuration& configuration) {
    return tryCreate(configuration, GLConfiguration{});
}

bool AbstractXApplication::tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::AbstractXApplication::tryCreate(): context already created", false);

    _windowSize = configuration.size();

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
        Error() << "Platform::WindowlessGlxApplication::tryCreate(): cannot get X visual";
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
    _contextHandler->createContext(glConfiguration, _window);

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
    if(_window) XDestroyWindow(_display, _window);
    if(_display) XCloseDisplay(_display);
}

void AbstractXApplication::swapBuffers() {
    _contextHandler->swapBuffers();
}

int AbstractXApplication::exec() {
    /* If exit was requested directly in the constructor, exit immediately
       without calling anything else */
    if(_flags & Flag::Exit) return _exitCode;

    /* Show window */
    XMapWindow(_display, _window);

    while(mainLoopIteration()) {}

    return _exitCode;
}

bool AbstractXApplication::mainLoopIteration() {
    /* If exit was requested directly in the constructor, exit immediately
       without calling anything else */
    if(_flags & Flag::Exit) return false;

    XEvent event;

    /* Closed window */
    if(XCheckTypedWindowEvent(_display, _window, ClientMessage, &event) &&
        Atom(event.xclient.data.l[0]) == _deleteWindow) {
        return false;
    }

    while(XCheckWindowEvent(_display, _window, INPUT_MASK, &event)) {
        switch(event.type) {
            /* Window resizing */
            case ConfigureNotify: {
                Vector2i size(event.xconfigure.width, event.xconfigure.height);
                if(size != _windowSize) {
                    _windowSize = size;
                    ViewportEvent e{size};
                    viewportEvent(e);
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

    return !(_flags & Flag::Exit);
}

void AbstractXApplication::viewportEvent(ViewportEvent& event) {
    static_cast<void>(event);
}

void AbstractXApplication::keyPressEvent(KeyEvent&) {}
void AbstractXApplication::keyReleaseEvent(KeyEvent&) {}
void AbstractXApplication::mousePressEvent(MouseEvent&) {}
void AbstractXApplication::mouseReleaseEvent(MouseEvent&) {}
void AbstractXApplication::mouseMoveEvent(MouseMoveEvent&) {}

AbstractXApplication::GLConfiguration::GLConfiguration(): _version(GL::Version::None) {}
AbstractXApplication::GLConfiguration::~GLConfiguration() = default;

AbstractXApplication::Configuration::Configuration():
    _title("Magnum X Application"), _size(800, 600) {}
AbstractXApplication::Configuration::~Configuration() = default;

}}
