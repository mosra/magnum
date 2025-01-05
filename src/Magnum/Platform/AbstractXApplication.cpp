/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019, 2020 Konstantinos Chatzilygeroudis <costashatz@gmail.com>

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

#include "Implementation/AbstractContextHandler.h"

/* Mask for X events */
#define INPUT_MASK KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|StructureNotifyMask

namespace Magnum { namespace Platform {

using namespace Containers::Literals;

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): AbstractXApplication{contextHandler, arguments, NoCreate} {
    create(configuration, glConfiguration);
}

AbstractXApplication::AbstractXApplication(Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, NoCreateT): _contextHandler{contextHandler}, _context{InPlaceInit, NoCreate, arguments.argc, arguments.argv}, _flags{Flag::Redraw} {}

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
    return _context->tryCreate(glConfiguration);
}

AbstractXApplication::~AbstractXApplication() {
    /* Destroy Magnum context first to avoid it potentially accessing the
       now-destroyed GL context after */
    _context = Containers::NullOpt;

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

namespace {

AbstractXApplication::Pointer buttonToPointer(const unsigned int button) {
    switch(button) {
        case 1 /*Button1*/:
            return AbstractXApplication::Pointer::MouseLeft;
        case 2 /*Button2*/:
            return AbstractXApplication::Pointer::MouseMiddle;
        case 3 /*Button3*/:
            return AbstractXApplication::Pointer::MouseRight;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}

AbstractXApplication::Pointers buttonsToPointers(const unsigned int state) {
    AbstractXApplication::Pointers pointers;
    if(state & Button1Mask)
        pointers |= AbstractXApplication::Pointer::MouseLeft;
    if(state & Button2Mask)
        pointers |= AbstractXApplication::Pointer::MouseMiddle;
    if(state & Button3Mask)
        pointers |= AbstractXApplication::Pointer::MouseRight;
    return pointers;
}

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
                KeyEvent e{Key(XLookupKeysym(&event.xkey, 0)), event.xkey.state, {event.xkey.x, event.xkey.y}};
                event.type == KeyPress ? keyPressEvent(e) : keyReleaseEvent(e);
            } break;
            case ButtonPress:
            case ButtonRelease: {
                /* Expose wheel as a scroll event, consistently with all other
                   applications */
                if(event.xbutton.button == 4 /*Button4*/ ||
                   event.xbutton.button == 5 /*Button5*/) {
                    ScrollEvent e{Vector2::yAxis(event.xbutton.button == 4 ? 1.0f : -1.0f), {Float(event.xbutton.x), Float(event.xbutton.y)}, event.xbutton.state};
                    /* It reports both press and release. Fire the scroll event
                       just on press. */
                    if(event.type == ButtonPress)
                        scrollEvent(e);
                } else {
                    const Pointer pointer = buttonToPointer(event.xbutton.button);
                    Pointers pointers = buttonsToPointers(event.xbutton.state);
                    /* Compared to other toolkits, the `pointers` don't include
                       the currently pressed button on press yet, and still
                       include the currently released button on release. Make
                       it consistent. */
                    if(event.type == ButtonPress) {
                        CORRADE_INTERNAL_ASSERT(!(pointers & pointer));
                        pointers |= pointer;
                    } else {
                        CORRADE_INTERNAL_ASSERT(pointers & pointer);
                        pointers &= ~pointer;
                    }

                    /* If an additional mouse button was pressed or some other
                       buttons are still left pressed after a release, call a
                       move event instead */
                    if((event.type == ButtonPress && (pointers & ~pointer)) ||
                       (event.type == ButtonRelease && pointers)) {
                        /* As we are patching up the set of currently pressed
                           pointers, the move event can't just figure that
                           out from the state */
                        PointerMoveEvent e{pointer, pointers,
                            {Float(event.xbutton.x), Float(event.xbutton.y)},
                            event.xbutton.state};
                        pointerMoveEvent(e);
                    } else {
                        PointerEvent e(pointer,
                            {Float(event.xbutton.x), Float(event.xbutton.y)},
                            event.xbutton.state);
                        event.type == ButtonPress ?
                            pointerPressEvent(e) : pointerReleaseEvent(e);
                    }
                }
            } break;

            /* Mouse move events */
            case MotionNotify: {
                /* Because for the move-from-press/release above we're patching
                   up the set of pressed pointers, we need to explicitly pass
                   it in here as well. No need to patch anything in this case
                   tho -- the set should be up-to-date. */
                PointerMoveEvent e({}, buttonsToPointers(event.xmotion.state),
                    {Float(event.xmotion.x), Float(event.xmotion.y)},
                    event.xmotion.state);
                pointerMoveEvent(e);
            } break;
        }
    }

    if(_flags & Flag::Redraw) {
        _flags &= ~Flag::Redraw;
        drawEvent();
    } else Utility::System::sleep(5);

    return !(_flags & Flag::Exit);
}

void AbstractXApplication::viewportEvent(ViewportEvent&) {}
void AbstractXApplication::keyPressEvent(KeyEvent&) {}
void AbstractXApplication::keyReleaseEvent(KeyEvent&) {}

#ifdef MAGNUM_BUILD_DEPRECATED
namespace {

CORRADE_IGNORE_DEPRECATED_PUSH
AbstractXApplication::MouseEvent::Button pointerToButton(const AbstractXApplication::Pointer pointer) {
    switch(pointer) {
        case AbstractXApplication::Pointer::MouseLeft:
            return AbstractXApplication::MouseEvent::Button::Left;
        case AbstractXApplication::Pointer::MouseMiddle:
            return AbstractXApplication::MouseEvent::Button::Middle;
        case AbstractXApplication::Pointer::MouseRight:
            return AbstractXApplication::MouseEvent::Button::Right;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}
CORRADE_IGNORE_DEPRECATED_POP

}
#endif

void AbstractXApplication::pointerPressEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    /* The positions are reported in integers in the first place, no need to
       round anything */
    MouseEvent mouseEvent{pointerToButton(event.pointer()), event._modifiers, Vector2i{event.position()}};
    mousePressEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void AbstractXApplication::mousePressEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void AbstractXApplication::pointerReleaseEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    /* The positions are reported in integers in the first place, no need to
       round anything */
    MouseEvent mouseEvent{pointerToButton(event.pointer()), event._modifiers, Vector2i{event.position()}};
    mouseReleaseEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void AbstractXApplication::mouseReleaseEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void AbstractXApplication::pointerMoveEvent(PointerMoveEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    /* If the event is due to some button being additionally pressed or one
       button from a larger set being released, delegate to a press/release
       event instead */
    if(event.pointer()) {
        MouseEvent mouseEvent{pointerToButton(*event.pointer()), event._modifiers,
            Vector2i{event.position()}};
        event.pointers() >= *event.pointer() ?
            mousePressEvent(mouseEvent) : mouseReleaseEvent(mouseEvent);
    } else {
        MouseMoveEvent mouseEvent{event._modifiers, Vector2i{event.position()}};
        mouseMoveEvent(mouseEvent);
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void AbstractXApplication::mouseMoveEvent(MouseMoveEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void AbstractXApplication::scrollEvent(ScrollEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    /* The positions are reported in integers in the first place, no need to
       round anything */
    MouseEvent e{event.offset().y() > 0.0f ? MouseEvent::Button::WheelUp : MouseEvent::Button::WheelDown, event._modifiers, Vector2i{event.position()}};
    mousePressEvent(e);
    mouseReleaseEvent(e);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

AbstractXApplication::GLConfiguration::GLConfiguration(): _version(GL::Version::None) {}
AbstractXApplication::GLConfiguration::~GLConfiguration() = default;

AbstractXApplication::Configuration::Configuration():
    _title{Containers::String::nullTerminatedGlobalView("Magnum X Application"_s)},
    _size(800, 600) {}
AbstractXApplication::Configuration::~Configuration() = default;

AbstractXApplication::Pointers AbstractXApplication::KeyEvent::pointers() const {
    return buttonsToPointers(_modifiers);
}

AbstractXApplication::Pointers AbstractXApplication::ScrollEvent::pointers() const {
    return buttonsToPointers(_modifiers);
}

}}
