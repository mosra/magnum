/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

#include "GlfwApplication.h"

#include <tuple>
#include <Corrade/Utility/String.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/Version.h"
#include "Magnum/Platform/Context.h"
#include "Magnum/Platform/ScreenedApplication.hpp"

namespace Magnum { namespace Platform {

GlfwApplication* GlfwApplication::_instance = nullptr;

#ifdef GLFW_TRUE
/* The docs say that it's the same, verify that just in case */
static_assert(GLFW_TRUE == true && GLFW_FALSE == false, "GLFW does not have sane bool values");
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
GlfwApplication::GlfwApplication(const Arguments& arguments): GlfwApplication{arguments, Configuration{}} {}
#endif

GlfwApplication::GlfwApplication(const Arguments& arguments, const Configuration& configuration): GlfwApplication{arguments, NoCreate} {
    createContext(configuration);
}

GlfwApplication::GlfwApplication(const Arguments& arguments, NoCreateT):
    _context{new Context{NoCreate, arguments.argc, arguments.argv}},
    _flags{Flag::Redraw}
{
    /* Save global instance */
    _instance = this;

    /* Init GLFW */
    glfwSetErrorCallback(staticErrorCallback);

    if(!glfwInit()) {
        Error() << "Could not initialize GLFW";
        std::exit(8);
    }
}

void GlfwApplication::createContext() { createContext({}); }

void GlfwApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool GlfwApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::GlfwApplication::tryCreateContext(): context already created", false);

    /* Window flags */
    GLFWmonitor* monitor = nullptr; /* Needed for setting fullscreen */
    if (configuration.windowFlags() >= Configuration::WindowFlag::Fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        glfwWindowHint(GLFW_AUTO_ICONIFY, configuration.windowFlags() >= Configuration::WindowFlag::AutoIconify);
    } else {
        const Configuration::WindowFlags& flags = configuration.windowFlags();
        glfwWindowHint(GLFW_RESIZABLE, flags >= Configuration::WindowFlag::Resizable);
        glfwWindowHint(GLFW_VISIBLE, !(flags >= Configuration::WindowFlag::Hidden));
        #ifdef GLFW_MAXIMIZED
        glfwWindowHint(GLFW_MAXIMIZED, flags >= Configuration::WindowFlag::Maximized);
        #endif
        glfwWindowHint(GLFW_FLOATING, flags >= Configuration::WindowFlag::Floating);
    }
    glfwWindowHint(GLFW_FOCUSED, configuration.windowFlags() >= Configuration::WindowFlag::Focused);

    /* Context window hints */
    glfwWindowHint(GLFW_SAMPLES, configuration.sampleCount());
    glfwWindowHint(GLFW_SRGB_CAPABLE, configuration.isSRGBCapable());

    const Configuration::Flags& flags = configuration.flags();
    #ifdef GLFW_CONTEXT_NO_ERROR
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, flags >= Configuration::Flag::NoError);
    #endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, flags >= Configuration::Flag::Debug);
    glfwWindowHint(GLFW_STEREO, flags >= Configuration::Flag::Stereo);

    /* Set context version, if requested */
    if(configuration.version() != Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(configuration.version());
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        #ifndef MAGNUM_TARGET_GLES
        if(configuration.version() >= Version::GL310) {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        #else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        #endif
    }

    /* Set context flags */
    _window = glfwCreateWindow(configuration.size().x(), configuration.size().y(), configuration.title().c_str(), monitor, nullptr);
    if(!_window) {
        Error() << "Platform::GlfwApplication::tryCreateContext(): cannot create context";
        glfwTerminate();
        return false;
    }

    /* Proceed with configuring other stuff that couldn't be done with window
       hints */
    if(configuration.windowFlags() >= Configuration::WindowFlag::Minimized)
        glfwIconifyWindow(_window);
    glfwSetInputMode(_window, GLFW_CURSOR, Int(configuration.cursorMode()));

    /* Set callbacks */
    glfwSetFramebufferSizeCallback(_window, staticViewportEvent);
    glfwSetKeyCallback(_window, staticKeyEvent);
    glfwSetCursorPosCallback(_window, staticMouseMoveEvent);
    glfwSetMouseButtonCallback(_window, staticMouseEvent);
    glfwSetScrollCallback(_window, staticMouseScrollEvent);
    glfwSetCharCallback(_window, staticTextInputEvent);

    glfwMakeContextCurrent(_window);

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

GlfwApplication::~GlfwApplication() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

Vector2i GlfwApplication::windowSize() {
    Vector2i size;
    glfwGetWindowSize(_window, &size.x(), &size.y());
    return size;
}

void GlfwApplication::setSwapInterval(const Int interval) {
    glfwSwapInterval(interval);
}

int GlfwApplication::exec() {
    while(!glfwWindowShouldClose(_window)) {
        if(_flags & Flag::Redraw) {
            _flags &= ~Flag::Redraw;
            drawEvent();
        }
        glfwPollEvents();
    }
    return 0;
}

void GlfwApplication::staticKeyEvent(GLFWwindow*, int key, int, int action, int mods) {
    KeyEvent e(static_cast<KeyEvent::Key>(key), {static_cast<InputEvent::Modifier>(mods)}, action == GLFW_REPEAT);

    if(action == GLFW_PRESS) {
        _instance->keyPressEvent(e);
    } else if(action == GLFW_RELEASE) {
        _instance->keyReleaseEvent(e);
    } else if(action == GLFW_REPEAT) {
        _instance->keyPressEvent(e);
    }
}

void GlfwApplication::staticMouseMoveEvent(GLFWwindow* window, double x, double y) {
    MouseMoveEvent e{Vector2i{Int(x), Int(y)}, KeyEvent::getCurrentGlfwModifiers(window)};
    _instance->mouseMoveEvent(e);
}

void GlfwApplication::staticMouseEvent(GLFWwindow*, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(_instance->_window, &x, &y);
    MouseEvent e(static_cast<MouseEvent::Button>(button), {Int(x), Int(y)}, {static_cast<InputEvent::Modifier>(mods)});

    if(action == GLFW_PRESS) {
        _instance->mousePressEvent(e);
    } else if(action == GLFW_RELEASE) {
        _instance->mouseReleaseEvent(e);
    } /* we don't handle GLFW_REPEAT */
}

void GlfwApplication::staticMouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
    MouseScrollEvent e(Vector2{Float(xoffset), Float(yoffset)}, KeyEvent::getCurrentGlfwModifiers(window));
    _instance->mouseScrollEvent(e);

    #ifdef MAGNUM_BUILD_DEPRECATED
    if(yoffset != 0.0) {
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #endif
        MouseEvent e1((yoffset > 0.0) ? MouseEvent::Button::WheelUp : MouseEvent::Button::WheelDown, {}, KeyEvent::getCurrentGlfwModifiers(window));
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
        _instance->mousePressEvent(e1);
    }
    #endif
}

void GlfwApplication::staticTextInputEvent(GLFWwindow*, unsigned int codepoint) {
    if(!(_instance->_flags & Flag::TextInputActive)) return;

    char utf8[4];
    const std::size_t size = Utility::Unicode::utf8(codepoint, utf8);
    TextInputEvent e{{utf8, size}};
    _instance->textInputEvent(e);
}

void GlfwApplication::staticErrorCallback(int, const char* description) {
    Error() << description;
}

auto GlfwApplication::KeyEvent::getCurrentGlfwModifiers(GLFWwindow* window) -> Modifiers {
    static_assert(GLFW_PRESS == true && GLFW_RELEASE == false,
        "GLFW press and release constants do not correspond to bool values");

    Modifiers mods;
    if(glfwGetKey(window, Int(Key::LeftShift)) || glfwGetKey(window, Int(Key::RightShift)))
        mods |= Modifier::Shift;
    if(glfwGetKey(window, Int(Key::LeftAlt)) || glfwGetKey(window, Int(Key::RightAlt)))
        mods |= Modifier::Alt;
    if(glfwGetKey(window, Int(Key::LeftCtrl)) || glfwGetKey(window, Int(Key::RightCtrl)))
        mods |= Modifier::Ctrl;
    if(glfwGetKey(window, Int(Key::RightSuper)))
        mods |= Modifier::Super;

    return mods;
}

void GlfwApplication::viewportEvent(const Vector2i&) {}
void GlfwApplication::keyPressEvent(KeyEvent&) {}
void GlfwApplication::keyReleaseEvent(KeyEvent&) {}
void GlfwApplication::mousePressEvent(MouseEvent&) {}
void GlfwApplication::mouseReleaseEvent(MouseEvent&) {}
void GlfwApplication::mouseMoveEvent(MouseMoveEvent&) {}
void GlfwApplication::mouseScrollEvent(MouseScrollEvent&) {}
void GlfwApplication::textInputEvent(TextInputEvent&) {}

GlfwApplication::Configuration::Configuration():
    _title{"Magnum GLFW Application"},
    _size{800, 600}, _sampleCount{0},
    _version{Version::None},
    _windowFlags{WindowFlag::Focused},
    _cursorMode{CursorMode::Normal} {}

GlfwApplication::Configuration::~Configuration() = default;

#if defined(DOXYGEN_GENERATING_OUTPUT) || GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 302
std::string GlfwApplication::KeyEvent::keyName(const Key key) {
    /* It can return null, so beware */
    return Utility::String::fromArray(glfwGetKeyName(int(key), 0));
}

std::string GlfwApplication::KeyEvent::keyName() const {
    return keyName(_key);
}
#endif

template class BasicScreen<GlfwApplication>;
template class BasicScreenedApplication<GlfwApplication>;

}}
