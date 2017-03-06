/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include "Sdl2Application.h"

#include <cstring>
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <tuple>
#else
#include <emscripten/emscripten.h>
#endif

#include "Magnum/Version.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Platform/Context.h"
#include "Magnum/Platform/ScreenedApplication.hpp"

namespace Magnum { namespace Platform {

namespace {

/*
 * Fix up the modifiers -- we want >= operator to work properly on Shift,
 * Ctrl, Alt, but SDL generates different event for left / right keys, thus
 * (modifiers >= Shift) would pass only if both left and right were pressed,
 * which is usually not what the developers wants.
 */
Sdl2Application::InputEvent::Modifiers fixedModifiers(Uint16 mod) {
    Sdl2Application::InputEvent::Modifiers modifiers(static_cast<Sdl2Application::InputEvent::Modifier>(mod));
    if(modifiers & Sdl2Application::InputEvent::Modifier::Shift) modifiers |= Sdl2Application::InputEvent::Modifier::Shift;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Ctrl) modifiers |= Sdl2Application::InputEvent::Modifier::Ctrl;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Alt) modifiers |= Sdl2Application::InputEvent::Modifier::Alt;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Super) modifiers |= Sdl2Application::InputEvent::Modifier::Alt;
    return modifiers;
}

}

Sdl2ApplicationWindow::Sdl2ApplicationWindow(Sdl2Application& application, NoCreateT): _application{application},
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _window{nullptr},
    #endif
    _windowFlags{WindowFlag::Redraw} {}

#ifndef CORRADE_TARGET_EMSCRIPTEN
Sdl2ApplicationWindow::Sdl2ApplicationWindow(Sdl2Application& application, const Sdl2ApplicationWindow::WindowConfiguration& configuration): Sdl2ApplicationWindow{application, NoCreate} {
    if(!tryCreateWindow(configuration)) std::exit(1);
}

Sdl2ApplicationWindow::Sdl2ApplicationWindow(Sdl2Application& application): Sdl2ApplicationWindow{application, WindowConfiguration{}} {}
#endif

Sdl2ApplicationWindow::~Sdl2ApplicationWindow() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    destroyWindow();
    #endif
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
bool Sdl2ApplicationWindow::tryCreateWindow(const WindowConfiguration& configuration) {
    CORRADE_INTERNAL_ASSERT(!_window);

    /* Create the window */
    if(!(_window = SDL_CreateWindow(
        #ifndef CORRADE_TARGET_IOS
        configuration.title().data(),
        #else
        nullptr,
        #endif
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        configuration.size().x(), configuration.size().y(),
        SDL_WINDOW_OPENGL|Uint32(configuration.windowFlags()))))
    {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create window:" << SDL_GetError();
        return false;
    }

    /* Add itself to the window list */
    const std::size_t windowId = SDL_GetWindowID(_window);
    CORRADE_INTERNAL_ASSERT(windowId <= _application._windows.size() + 2);
    for(std::size_t i = _application._windows.size(); i < windowId; ++i)
        _application._windows.push_back(nullptr);
    _application._windows.push_back(this);

    return true;
}

void Sdl2ApplicationWindow::destroyWindow() {
    /* Already done, nothing to do */
    if(!_window) return;

    /* Remove itself from the window list */
    const std::size_t id = SDL_GetWindowID(_window);
    CORRADE_INTERNAL_ASSERT(id < _application._windows.size());
    _application._windows[id] = nullptr;

    SDL_DestroyWindow(_window);
    _window = nullptr;
}
#endif

void Sdl2ApplicationWindow::viewportEvent(const Vector2i&) {}
void Sdl2ApplicationWindow::keyPressEvent(KeyEvent&) {}
void Sdl2ApplicationWindow::keyReleaseEvent(KeyEvent&) {}
void Sdl2ApplicationWindow::mousePressEvent(MouseEvent&) {}
void Sdl2ApplicationWindow::mouseReleaseEvent(MouseEvent&) {}
void Sdl2ApplicationWindow::mouseMoveEvent(MouseMoveEvent&) {}
void Sdl2ApplicationWindow::mouseScrollEvent(MouseScrollEvent&) {}
void Sdl2ApplicationWindow::textInputEvent(TextInputEvent&) {}
void Sdl2ApplicationWindow::textEditingEvent(TextEditingEvent&) {}

#ifdef CORRADE_TARGET_EMSCRIPTEN
Sdl2Application* Sdl2Application::_instance = nullptr;
void Sdl2Application::staticMainLoop() {
    _instance->mainLoop();
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Sdl2Application::Sdl2Application(const Arguments& arguments): Sdl2Application{arguments, Configuration{}} {}
#endif

Sdl2Application::Sdl2Application(const Arguments& arguments, const Configuration& configuration): Sdl2Application{arguments, nullptr} {
    createContext(configuration);
}

Sdl2Application::Sdl2Application(const Arguments& arguments, std::nullptr_t): Sdl2ApplicationWindow{*this, NoCreate}, _glContext{nullptr},
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _minimalLoopPeriod{0},
    #endif
    _context{new Context{NoCreate, arguments.argc, arguments.argv}}
{
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(!_instance, "Platform::Sdl2Application::Sdl2Application(): the instance is already created", );
    _instance = this;
    _windows[0] = this;
    #endif

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        std::exit(1);
    }
}

void Sdl2Application::createContext() { createContext({}); }

void Sdl2Application::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool Sdl2Application::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::Sdl2Application::tryCreateContext(): context already created", false);

    /* Enable double buffering and 24bt depth buffer */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Multisampling */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, configuration.sampleCount() > 1 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, configuration.sampleCount());

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* sRGB */
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, configuration.isSRGBCapable());
    #endif

    /* Hide the main window by default so we don't have distracting window
       blinking in case we have to destroy it again right away */
    WindowConfiguration hiddenConfiguration{configuration};
    hiddenConfiguration.setWindowFlags(hiddenConfiguration.windowFlags()|WindowConfiguration::WindowFlag::Hidden);

    /** @todo Remove when Emscripten has proper SDL2 support */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Set context version, if user-specified */
    if(configuration.version() != Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(configuration.version());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

        #ifndef MAGNUM_TARGET_GLES
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, configuration.version() >= Version::GL310 ?
            SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(configuration.flags()));

    /* Request usable version otherwise */
    } else {
        #ifndef MAGNUM_TARGET_GLES
        /* First try to create core context. This is needed mainly on OS X and
           Mesa, as support for recent OpenGL versions isn't implemented in
           compatibility contexts (which are the default). At least GL 3.2 is
           needed on OSX, at least GL 3.1 is needed on Mesa. Bite the bullet
           and try 3.1 also elsewhere. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #ifdef CORRADE_TARGET_APPLE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(configuration.flags())|SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        #else
        /* For ES the major context version is compile-time constant */
        #ifdef MAGNUM_TARGET_GLES3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #elif defined(MAGNUM_TARGET_GLES2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        #else
        #error unsupported OpenGL ES version
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif
    }

    /* Create the main window */
    if(!tryCreateWindow(hiddenConfiguration)) return false;

    /* Create context */
    _glContext = SDL_GL_CreateContext(_window);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1, if version is not
       user-specified and either core context creation fails or we are on
       binary NVidia/AMD drivers on Linux/Windows. Instead of creating forward-
       compatible context with highest available version, they force the
       version to the one specified, which is completely useless behavior. */
    #ifndef CORRADE_TARGET_APPLE
    constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
    #ifdef CORRADE_TARGET_WINDOWS
    constexpr static const char intelVendorString[] = "Intel";
    #endif
    constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
    const char* vendorString;
    #endif
    if(configuration.version() == Version::None && (!_glContext
        #ifndef CORRADE_TARGET_APPLE
        /* Sorry about the UGLY code, HOPEFULLY THERE WON'T BE MORE WORKAROUNDS */
        || (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
         #ifdef CORRADE_TARGET_WINDOWS
         std::strncmp(vendorString, intelVendorString, sizeof(intelVendorString)) == 0 ||
         #endif
         std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0)
         && !_context->isDriverWorkaroundDisabled("no-forward-compatible-core-context"))
        #endif
    )) {
        /* Don't print any warning when doing the NV workaround, because the
           bug will be there probably forever */
        if(!_glContext) Warning()
            << "Platform::Sdl2Application::tryCreateContext(): cannot create core context:"
            << SDL_GetError() << "(falling back to compatibility context)";
        else SDL_GL_DeleteContext(_glContext);

        destroyWindow();

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(configuration.flags()));

        if(!tryCreateWindow(hiddenConfiguration)) return false;

        /* Create compatibility context */
        _glContext = SDL_GL_CreateContext(_window);
    }
    #endif

    /* Cannot create context (or fallback compatibility context on desktop) */
    if(!_glContext) {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create context:" << SDL_GetError();
        destroyWindow();
        return false;
    }

    #ifdef CORRADE_TARGET_IOS
    /* iOS has zero initial GL_VIEWPORT size, get drawable size and put it back
       in so all other code can assume that the viewport is set to sane values.
       Fortunately on iOS we also don't have to load any function pointers so
       it's safe to do the glViewport() call as it is linked statically. */
    Vector2i drawableSize;
    SDL_GL_GetDrawableSize(_window, &drawableSize.x(), &drawableSize.y());
    glViewport(0, 0, drawableSize.x(), drawableSize.y());
    #endif
    #else
    /* Emscripten-specific initialization */
    if(!(_glContext = SDL_SetVideoMode(configuration.size().x(), configuration.size().y(), 24, SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF))) {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create context:" << SDL_GetError();
        return false;
    }
    #endif

    /* Destroy everything also when the Magnum context creation fails */
    if(!_context->tryCreate()) {
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_GL_DeleteContext(_glContext);
        SDL_DestroyWindow(_window);
        _window = nullptr;
        #else
        SDL_FreeSurface(_glContext);
        #endif
        return false;
    }

    _activeGlContextWindow = _window;

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Show the window once we are sure that everything is okay */
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Hidden))
        SDL_ShowWindow(_window);
    #endif

    /* Return true if the initialization succeeds */
    return true;
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
Vector2i Sdl2ApplicationWindow::windowSize() {
    Vector2i size;
    SDL_GetWindowSize(_window, &size.x(), &size.y());
    return size;
}
#endif

void Sdl2ApplicationWindow::swapBuffers() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_SwapWindow(_window);
    #else
    SDL_Flip(_application._glContext);
    #endif
}

Int Sdl2Application::swapInterval() const {
    return SDL_GL_GetSwapInterval();
}

bool Sdl2Application::setSwapInterval(const Int interval) {
    if(SDL_GL_SetSwapInterval(interval) == -1) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): cannot set swap interval:" << SDL_GetError();
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    if(SDL_GL_GetSwapInterval() != interval) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): swap interval setting ignored by the driver";
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    _flags |= Flag::VSyncEnabled;
    return true;
}

Sdl2Application::~Sdl2Application() {
    _context.reset();

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_DeleteContext(_glContext);
    /* Destroy all windows before calling SDL_Quit */
    for(Sdl2ApplicationWindow* w: _windows)
        if(w) w->destroyWindow();
    #else
    SDL_FreeSurface(_glContext);
    CORRADE_INTERNAL_ASSERT(_instance == this);
    _instance = nullptr;
    #endif
    SDL_Quit();
}

int Sdl2Application::exec() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    while(!(_flags & Flag::Exit)) mainLoop();
    #else
    emscripten_set_main_loop(staticMainLoop, 0, true);
    #endif
    return 0;
}

void Sdl2Application::exit() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _flags |= Flag::Exit;
    #else
    emscripten_cancel_main_loop();
    #endif
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::makeContextCurrent(Sdl2ApplicationWindow& window) {
    /* Only do it if it is not active already */
    if(_activeGlContextWindow != window._window) {
        SDL_GL_MakeCurrent(window._window, _glContext);
        _activeGlContextWindow = window._window;
        Context::current().resetState(Context::State::WindowSpecific);
    }
}
#endif

template<class ...Args> inline void Sdl2Application::callEventHandler(std::size_t windowId, void(Sdl2ApplicationWindow::*eventHandler)(Args...), Args&&... args) {
    if(!(windowId < _windows.size() && _windows[windowId])) {
        Debug() << "HUH" << windowId << _windows.size();
        return;
    }

    CORRADE_INTERNAL_ASSERT(windowId < _windows.size() && _windows[windowId]);

    /* Make GL context current on that window so the user is not restricted in
       which functions can be called */
    makeContextCurrent(*_windows[windowId]);
    (_windows[windowId]->*eventHandler)(std::forward<Args>(args)...);
}

void Sdl2Application::mainLoop() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    const UnsignedInt timeBefore = _minimalLoopPeriod ? SDL_GetTicks() : 0;
    #endif

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        #ifndef CORRADE_TARGET_IOS
                        /* Can't pass it directly because that would cause
                           ambiguous template parameter pack resolution */
                        const Vector2i drawableSize{event.window.data1, event.window.data2};
                        callEventHandler(event.window.windowID,
                            &Sdl2ApplicationWindow::viewportEvent,
                            drawableSize);
                        #else
                        /* On iOS the window event is in points and not pixels,
                           but we need pixels to call glViewport() properly */
                        Vector2i drawableSize;
                        SDL_GL_GetDrawableSize(_window, &drawableSize.x(), &drawableSize.y());
                        callEventHandler(event.window.windowID,
                            &Sdl2ApplicationWindow::viewportEvent,
                            drawableSize);
                        #endif
                        _windows[event.window.windowID]->_windowFlags |= WindowFlag::Redraw;
                    } break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        CORRADE_INTERNAL_ASSERT(event.window.windowID < _windows.size() && _windows[event.window.windowID]);
                        _windows[event.window.windowID]->_windowFlags |= WindowFlag::Redraw;
                        break;
                } break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent e(static_cast<KeyEvent::Key>(event.key.keysym.sym), fixedModifiers(event.key.keysym.mod), event.key.repeat != 0);
                callEventHandler(event.key.windowID,
                    event.type == SDL_KEYDOWN ? &Sdl2ApplicationWindow::keyPressEvent : &Sdl2ApplicationWindow::keyReleaseEvent,
                    e);
            } break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseEvent e(static_cast<MouseEvent::Button>(event.button.button), {event.button.x, event.button.y}
                    #ifndef CORRADE_TARGET_EMSCRIPTEN
                    , event.button.clicks
                    #endif
                    );
                callEventHandler(event.button.windowID,
                    event.type == SDL_MOUSEBUTTONDOWN ? &Sdl2ApplicationWindow::mousePressEvent : &Sdl2ApplicationWindow::mouseReleaseEvent,
                    e);
            } break;

            case SDL_MOUSEWHEEL: {
                MouseScrollEvent e{{Float(event.wheel.x), Float(event.wheel.y)}};
                callEventHandler(event.wheel.windowID,
                    &Sdl2ApplicationWindow::mouseScrollEvent,
                    e);

                #ifdef MAGNUM_BUILD_DEPRECATED
                if(event.wheel.y != 0) {
                    #ifdef __GNUC__
                    #pragma GCC diagnostic push
                    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                    #endif
                    MouseEvent e(event.wheel.y > 0 ? MouseEvent::Button::WheelUp : MouseEvent::Button::WheelDown, {event.wheel.x, event.wheel.y}
                        #ifndef CORRADE_TARGET_EMSCRIPTEN
                        , 0
                        #endif
                        );
                    #ifdef __GNUC__
                    #pragma GCC diagnostic pop
                    #endif
                    callEventHandler(event.wheel.windowID,
                        &Sdl2ApplicationWindow::mousePressEvent,
                        e);
                }
                #endif
            } break;

            case SDL_MOUSEMOTION: {
                MouseMoveEvent e({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel}, static_cast<MouseMoveEvent::Button>(event.motion.state));
                callEventHandler(event.motion.windowID,
                    &Sdl2ApplicationWindow::mouseMoveEvent,
                    e);
                break;
            }

            case SDL_MULTIGESTURE: {
                /* For some reason not specific to a window, so calling it on
                   the main window only. SDL2 omission? */
                MultiGestureEvent e({event.mgesture.x, event.mgesture.y}, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.numFingers);
                makeContextCurrent(*this);
                multiGestureEvent(e);
                break;
            }

            case SDL_TEXTINPUT: {
                TextInputEvent e{{event.text.text, std::strlen(event.text.text)}};
                callEventHandler(event.text.windowID,
                    &Sdl2ApplicationWindow::textInputEvent,
                    e);
            } break;

            case SDL_TEXTEDITING: {
                CORRADE_INTERNAL_ASSERT(event.edit.windowID < _windows.size() && _windows[event.edit.windowID]);
                TextEditingEvent e{{event.edit.text, std::strlen(event.text.text)}, event.edit.start, event.edit.length};
                callEventHandler(event.text.windowID,
                    &Sdl2ApplicationWindow::textEditingEvent,
                    e);
            } break;

            case SDL_QUIT:
                #ifndef CORRADE_TARGET_EMSCRIPTEN
                _flags |= Flag::Exit;
                #else
                emscripten_cancel_main_loop();
                #endif
                return;
        }
    }

    /* Tick event */
    if(!(_flags & Flag::NoTickEvent)) {
        makeContextCurrent(*this);
        tickEvent();
    }

    /* Draw event */
    bool somethingDrawn = false;
    for(std::size_t i = 0; i != _windows.size(); ++i) {
        if(!_windows[i] || !(_windows[i]->_windowFlags & WindowFlag::Redraw)) continue;

        _windows[i]->_windowFlags &= ~WindowFlag::Redraw;
        callEventHandler(i,
            &Sdl2ApplicationWindow::drawEvent);
        somethingDrawn = true;
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* If VSync is not enabled, delay to prevent CPU hogging (if set) */
    if(somethingDrawn) {
        if(!(_flags & Flag::VSyncEnabled) && _minimalLoopPeriod) {
            const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
            if(loopTime < _minimalLoopPeriod)
                SDL_Delay(_minimalLoopPeriod - loopTime);
        }

        return;
    }

    /* If not drawing anything, delay to prevent CPU hogging (if set) */
    if(_minimalLoopPeriod) {
        const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
        if(loopTime < _minimalLoopPeriod)
            SDL_Delay(_minimalLoopPeriod - loopTime);
    }

    /* Then, if the tick event doesn't need to be called periodically, wait
       indefinitely for next input event */
    if(_flags & Flag::NoTickEvent) SDL_WaitEvent(nullptr);
    #endif
}

void Sdl2Application::setMouseLocked(Sdl2ApplicationWindow* const window) {
    /** @todo Implement this in Emscripten */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowGrab(window ? window->_window : _window, window ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(window ? SDL_TRUE : SDL_FALSE);
    #else
    CORRADE_ASSERT(false, "Sdl2Application::setMouseLocked(): not implemented", );
    static_cast<void>(window);
    #endif
}

bool Sdl2Application::isTextInputActive() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    return SDL_IsTextInputActive();
    #else
    return !!(_flags & Flag::TextInputActive);
    #endif
}

void Sdl2Application::startTextInput() {
    SDL_StartTextInput();
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _flags |= Flag::TextInputActive;
    #endif
}

void Sdl2Application::stopTextInput() {
    SDL_StopTextInput();
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _flags &= ~Flag::TextInputActive;
    #endif
}

void Sdl2Application::setTextInputRect(const Range2Di& rect) {
    SDL_Rect r{rect.min().x(), rect.min().y(), rect.sizeX(), rect.sizeY()};
    SDL_SetTextInputRect(&r);
}

void Sdl2Application::tickEvent() {
    /* If this got called, the tick event is not implemented by user and thus
       we don't need to call it ever again */
    _flags |= Flag::NoTickEvent;
}

void Sdl2Application::multiGestureEvent(MultiGestureEvent&) {}

Sdl2ApplicationWindow::WindowConfiguration::WindowConfiguration():
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
    _title("Magnum SDL2 Application"),
    #endif
    #ifndef CORRADE_TARGET_IOS
    _size{800, 600},
    #else
    _size{}, /* SDL2 detects someting for us */
    #endif
    _windowFlags{} {}

Sdl2ApplicationWindow::WindowConfiguration::~WindowConfiguration() = default;

Sdl2Application::Configuration::Configuration():
    _sampleCount(0)
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _version(Version::None), _sRGBCapable{false}
    #endif
    {}

std::string Sdl2ApplicationWindow::KeyEvent::keyName(const Key key) {
    return SDL_GetKeyName(SDL_Keycode(key));
}

std::string Sdl2ApplicationWindow::KeyEvent::keyName() const {
    return keyName(_key);
}

Sdl2ApplicationWindow::InputEvent::Modifiers Sdl2ApplicationWindow::MouseEvent::modifiers() {
    if(_modifiersLoaded) return _modifiers;
    _modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

Sdl2ApplicationWindow::InputEvent::Modifiers Sdl2ApplicationWindow::MouseMoveEvent::modifiers() {
    if(_modifiersLoaded) return _modifiers;
    _modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

Sdl2ApplicationWindow::InputEvent::Modifiers Sdl2ApplicationWindow::MouseScrollEvent::modifiers() {
    if(_modifiersLoaded) return _modifiers;
    _modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

template class BasicScreen<Sdl2Application>;
template class BasicScreenedApplication<Sdl2Application>;

}}
