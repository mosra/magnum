/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/Math/Range.h"
#include "Magnum/Platform/ScreenedApplication.hpp"
#include "Magnum/Platform/Implementation/dpiScaling.hpp"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"
#endif

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

Sdl2Application::Sdl2Application(const Arguments& arguments): Sdl2Application{arguments, Configuration{}} {}

Sdl2Application::Sdl2Application(const Arguments& arguments, const Configuration& configuration): Sdl2Application{arguments, NoCreate} {
    create(configuration);
}

#ifdef MAGNUM_TARGET_GL
Sdl2Application::Sdl2Application(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): Sdl2Application{arguments, NoCreate} {
    create(configuration, glConfiguration);
}
#endif

Sdl2Application::Sdl2Application(const Arguments& arguments, NoCreateT):
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _minimalLoopPeriod{0},
    #endif
    #ifdef MAGNUM_TARGET_GL
    _glContext{nullptr},
    #endif
    _flags{Flag::Redraw}
{
    Utility::Arguments args{Implementation::windowScalingArguments()};
    #ifdef MAGNUM_TARGET_GL
    _context.reset(new GLContext{NoCreate, args, arguments.argc, arguments.argv});
    #else
    args.parse(arguments.argc, arguments.argv);
    #endif

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        std::exit(1);
    }

    /* Save command-line arguments */
    if(args.value("log") == "verbose") _verboseLog = true;
    const std::string dpiScaling = args.value("dpi-scaling");
    if(dpiScaling == "default")
        _commandLineDpiScalingPolicy = Implementation::DpiScalingPolicy::Default;
    #ifdef CORRADE_TARGET_APPLE
    else if(dpiScaling == "framebuffer")
        _commandLineDpiScalingPolicy = Implementation::DpiScalingPolicy::Framebuffer;
    #endif
    #ifndef CORRADE_TARGET_APPLE
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    else if(dpiScaling == "virtual")
        _commandLineDpiScalingPolicy = Implementation::DpiScalingPolicy::Virtual;
    #endif
    else if(dpiScaling == "physical")
        _commandLineDpiScalingPolicy = Implementation::DpiScalingPolicy::Physical;
    #endif
    else if(dpiScaling.find_first_of(" \t\n") != std::string::npos)
        _commandLineDpiScaling = args.value<Vector2>("dpi-scaling");
    else
        _commandLineDpiScaling = Vector2{args.value<Float>("dpi-scaling")};
}

void Sdl2Application::create() {
    create(Configuration{});
}

void Sdl2Application::create(const Configuration& configuration) {
    if(!tryCreate(configuration)) std::exit(1);
}

#ifdef MAGNUM_TARGET_GL
void Sdl2Application::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration)) std::exit(1);
}
#endif

Vector2 Sdl2Application::dpiScaling(const Configuration& configuration) const {
    std::ostream* verbose = _verboseLog ? Debug::output() : nullptr;

    /* Print a helpful warning in case some extra steps are needed for HiDPI
       support */
    #ifdef CORRADE_TARGET_APPLE
    if(!Implementation::isAppleBundleHiDpiEnabled())
        Warning{} << "Platform::Sdl2Application: warning: the executable is not a HiDPI-enabled app bundle";
    #elif defined(CORRADE_TARGET_WINDOWS)
    /** @todo */
    #endif

    /* Use values from the configuration only if not overriden on command line.
       In any case explicit scaling has a precedence before the policy. */
    Implementation::DpiScalingPolicy dpiScalingPolicy{};
    if(!_commandLineDpiScaling.isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: user-defined DPI scaling" << _commandLineDpiScaling.x();
        return _commandLineDpiScaling;
    } else if(UnsignedByte(_commandLineDpiScalingPolicy)) {
        dpiScalingPolicy = _commandLineDpiScalingPolicy;
    } else if(!configuration.dpiScaling().isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: app-defined DPI scaling" << _commandLineDpiScaling.x();
        return configuration.dpiScaling();
    } else {
        dpiScalingPolicy = configuration.dpiScalingPolicy();
    }

    /* There's no choice on Apple, it's all controlled by the plist file. So
       unless someone specified custom scaling via config or command-line
       above, return the default. */
    #ifdef CORRADE_TARGET_APPLE
    return Vector2{1.0f};

    /* Otherwise there's a choice between virtual and physical DPI scaling */
    #else
    /* Try to get virtual DPI scaling first, if supported and requested */
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    if(dpiScalingPolicy == Implementation::DpiScalingPolicy::Virtual) {
        /* Use Xft.dpi on X11 */
        #ifdef _MAGNUM_PLATFORM_USE_X11
        const Vector2 dpiScaling{Implementation::x11DpiScaling()};
        if(!dpiScaling.isZero()) {
            Debug{verbose} << "Platform::Sdl2Application: virtual DPI scaling" << dpiScaling.x();
            return dpiScaling;
        }

        /* Otherwise ¯\_(ツ)_/¯ */
        #else
        Debug{verbose} << "Platform::Sdl2Application: sorry, virtual DPI scaling not implemented on this platform yet";
        return Vector2{1.0f};
        #endif
    }
    #endif

    /* At this point, either the virtual DPI query failed or a physical DPI
       scaling is requested */
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::DpiScalingPolicy::Virtual || dpiScalingPolicy == Implementation::DpiScalingPolicy::Physical);
    #else
    CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::DpiScalingPolicy::Physical);
    #endif

    /* Take device pixel ratio on Emscripten */
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    const Vector2 dpiScaling{Implementation::emscriptenDpiScaling()};
    Debug{verbose} << "Platform::Sdl2Application: physical DPI scaling" << dpiScaling.x();
    return dpiScaling;

    /* Take display DPI elsewhere. Enable only on Linux for now, I need to
       test this properly on Windows first. Also only since SDL 2.0.4. */
    #elif defined(CORRADE_TARGET_UNIX) && SDL_VERSION_ATLEAST(2, 0, 4)
    Vector2 dpi;
    if(SDL_GetDisplayDPI(0, nullptr, &dpi.x(), &dpi.y()) == 0) {
        const Vector2 dpiScaling{dpi/96.0f};
        Debug{verbose} << "Platform::Sdl2Application: physical DPI scaling" << dpiScaling;
        return dpiScaling;
    }

    Warning{} << "Platform::Sdl2Application: can't get physical display DPI, falling back to no scaling:" << SDL_GetError();
    return Vector2{1.0f};

    /* Not implemented otherwise */
    #else
    Debug{verbose} << "Platform::Sdl2Application: sorry, physical DPI scaling not implemented on this platform yet";
    return Vector2{1.0f};
    #endif
    #endif
}

bool Sdl2Application::tryCreate(const Configuration& configuration) {
    #ifdef MAGNUM_TARGET_GL
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Contextless))
        return tryCreate(configuration, GLConfiguration{});
    #endif

    /* Scale window based on DPI */
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Create window */
    if(!(_window = SDL_CreateWindow(
        #ifndef CORRADE_TARGET_IOS
        configuration.title().data(),
        #else
        nullptr,
        #endif
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        scaledWindowSize.x(), scaledWindowSize.y(),
        SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags() & ~Configuration::WindowFlag::Contextless))))
    {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
        return false;
    }
    #else
    /* Emscripten-specific initialization */
    if(!(_glContext = SDL_SetVideoMode(scaledWindowSize.x(), scaledWindowSize.y(), 24, SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF))) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
        return false;
    }
    #endif

    return true;
}

#ifdef MAGNUM_TARGET_GL
bool Sdl2Application::tryCreate(const Configuration& configuration, const GLConfiguration&
    #ifndef MAGNUM_BUILD_DEPRECATED
    glConfiguration
    #else
    _glConfiguration
    #endif
) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    GLConfiguration glConfiguration{_glConfiguration};
    CORRADE_IGNORE_DEPRECATED_PUSH
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(configuration.flags() && !glConfiguration.flags())
        glConfiguration.setFlags(configuration.flags());
    if(configuration.version() != GL::Version::None && glConfiguration.version() == GL::Version::None)
        glConfiguration.setVersion(configuration.version());
    #endif
    if(configuration.sampleCount() && !glConfiguration.sampleCount())
        glConfiguration.setSampleCount(configuration.sampleCount());
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(configuration.isSRGBCapable() && !glConfiguration.isSRGBCapable())
        glConfiguration.setSRGBCapable(configuration.isSRGBCapable());
    #endif
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::Sdl2Application::tryCreate(): context already created", false);

    /* Enable double buffering, set up buffer sizes */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, glConfiguration.colorBufferSize().r());
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, glConfiguration.colorBufferSize().g());
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, glConfiguration.colorBufferSize().b());
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, glConfiguration.colorBufferSize().a());
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, glConfiguration.depthBufferSize());
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, glConfiguration.stencilBufferSize());

    /* Multisampling */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, glConfiguration.sampleCount() > 1 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, glConfiguration.sampleCount());

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* sRGB */
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, glConfiguration.isSRGBCapable());
    #endif

    /* Scale window based on DPI */
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

    /** @todo Remove when Emscripten has proper SDL2 support */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Set context version, if user-specified */
    if(glConfiguration.version() != GL::Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(glConfiguration.version());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

        #ifndef MAGNUM_TARGET_GLES
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glConfiguration.version() >= GL::Version::GL310 ?
            SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(glConfiguration.flags()));

    /* Request usable version otherwise */
    } else {
        #ifndef MAGNUM_TARGET_GLES
        /* First try to create core context. This is needed mainly on macOS and
           Mesa, as support for recent OpenGL versions isn't implemented in
           compatibility contexts (which are the default). At least GL 3.2 is
           needed on macOS, at least GL 3.1 is needed on Mesa. Bite the bullet
           and try 3.1 also elsewhere. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #ifdef CORRADE_TARGET_APPLE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(glConfiguration.flags())|SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
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

    /* Create window. Hide it by default so we don't have distracting window
       blinking in case we have to destroy it again right away */
    if(!(_window = SDL_CreateWindow(
        #ifndef CORRADE_TARGET_IOS
        configuration.title().data(),
        #else
        nullptr,
        #endif
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        scaledWindowSize.x(), scaledWindowSize.y(),
        SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN|SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags()))))
    {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
        return false;
    }

    /* Create context */
    _glContext = SDL_GL_CreateContext(_window);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1, if version is not
       user-specified and either core context creation fails or we are on
       binary NVidia/AMD drivers on Linux/Windows or Intel Windows drivers.
       Instead of creating forward-compatible context with highest available
       version, they force the version to the one specified, which is
       completely useless behavior. */
    #ifndef CORRADE_TARGET_APPLE
    constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
    #ifdef CORRADE_TARGET_WINDOWS
    constexpr static const char intelVendorString[] = "Intel";
    #endif
    constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
    const char* vendorString;
    #endif
    if(glConfiguration.version() == GL::Version::None && (!_glContext
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
        /* Don't print any warning when doing the workaround, because the bug
           will be there probably forever */
        if(!_glContext) Warning()
            << "Platform::Sdl2Application::tryCreate(): cannot create core context:"
            << SDL_GetError() << "(falling back to compatibility context)";
        else SDL_GL_DeleteContext(_glContext);

        SDL_DestroyWindow(_window);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(glConfiguration.flags()));

        if(!(_window = SDL_CreateWindow(configuration.title().data(),
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            scaledWindowSize.x(), scaledWindowSize.y(),
            SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN|SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags()&~Configuration::WindowFlag::Contextless))))
        {
            Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
            return false;
        }

        /* Create compatibility context */
        _glContext = SDL_GL_CreateContext(_window);
    }
    #endif

    /* Cannot create context (or fallback compatibility context on desktop) */
    if(!_glContext) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create context:" << SDL_GetError();
        SDL_DestroyWindow(_window);
        _window = nullptr;
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
    if(!(_glContext = SDL_SetVideoMode(scaledWindowSize.x(), scaledWindowSize.y(), 24, SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF))) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create context:" << SDL_GetError();
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

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Show the window once we are sure that everything is okay */
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Hidden))
        SDL_ShowWindow(_window);
    #endif

    /* Return true if the initialization succeeds */
    return true;
}
#endif

Vector2i Sdl2Application::windowSize() const {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    Vector2i size;
    SDL_GetWindowSize(_window, &size.x(), &size.y());
    return size;
    #else
    return {_glContext->w, _glContext->h};
    #endif
}

Vector2i Sdl2Application::framebufferSize() const {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    Vector2i size;
    SDL_GL_GetDrawableSize(_window, &size.x(), &size.y());
    return size;
    #else
    return {_glContext->w, _glContext->h};
    #endif
}

void Sdl2Application::swapBuffers() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_SwapWindow(_window);
    #else
    SDL_Flip(_glContext);
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
    #ifdef MAGNUM_TARGET_GL
    _context.reset();

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_DeleteContext(_glContext);
    #else
    SDL_FreeSurface(_glContext);
    #endif
    #endif

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_DestroyWindow(_window);
    #endif
    SDL_Quit();
}

int Sdl2Application::exec() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    while(!(_flags & Flag::Exit)) mainLoopIteration();
    #else
    emscripten_set_main_loop_arg([](void* arg) {
        static_cast<Sdl2Application*>(arg)->mainLoopIteration();
    }, this, 0, true);
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

void Sdl2Application::mainLoopIteration() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    const UnsignedInt timeBefore = _minimalLoopPeriod ? SDL_GetTicks() : 0;
    #endif

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        ViewportEvent e{{event.window.data1, event.window.data2}, framebufferSize(), _dpiScaling};
                        /** @todo handle also WM_DPICHANGED events when a window is moved between displays with different DPI */
                        viewportEvent(e);
                        _flags |= Flag::Redraw;
                    } break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        _flags |= Flag::Redraw;
                        break;
                } break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent e(static_cast<KeyEvent::Key>(event.key.keysym.sym), fixedModifiers(event.key.keysym.mod), event.key.repeat != 0);
                event.type == SDL_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
            } break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseEvent e(static_cast<MouseEvent::Button>(event.button.button), {event.button.x, event.button.y}
                    #ifndef CORRADE_TARGET_EMSCRIPTEN
                    , event.button.clicks
                    #endif
                    );
                event.type == SDL_MOUSEBUTTONDOWN ? mousePressEvent(e) : mouseReleaseEvent(e);
            } break;

            case SDL_MOUSEWHEEL: {
                MouseScrollEvent e{{Float(event.wheel.x), Float(event.wheel.y)}};
                mouseScrollEvent(e);

                #ifdef MAGNUM_BUILD_DEPRECATED
                if(event.wheel.y != 0) {
                    #ifdef __GNUC__
                    #pragma GCC diagnostic push
                    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                    #endif
                    MouseEvent ew(event.wheel.y > 0 ? MouseEvent::Button::WheelUp : MouseEvent::Button::WheelDown, {event.wheel.x, event.wheel.y}
                        #ifndef CORRADE_TARGET_EMSCRIPTEN
                        , 0
                        #endif
                        );
                    #ifdef __GNUC__
                    #pragma GCC diagnostic pop
                    #endif
                    mousePressEvent(ew);
                }
                #endif
            } break;

            case SDL_MOUSEMOTION: {
                MouseMoveEvent e({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel}, static_cast<MouseMoveEvent::Button>(event.motion.state));
                mouseMoveEvent(e);
                break;
            }

            case SDL_MULTIGESTURE: {
                MultiGestureEvent e({event.mgesture.x, event.mgesture.y}, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.numFingers);
                multiGestureEvent(e);
                break;
            }

            case SDL_TEXTINPUT: {
                TextInputEvent e{{event.text.text, std::strlen(event.text.text)}};
                textInputEvent(e);
            } break;

            case SDL_TEXTEDITING: {
                TextEditingEvent e{{event.edit.text, std::strlen(event.text.text)}, event.edit.start, event.edit.length};
                textEditingEvent(e);
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
    if(!(_flags & Flag::NoTickEvent)) tickEvent();

    /* Draw event */
    if(_flags & Flag::Redraw) {
        _flags &= ~Flag::Redraw;
        drawEvent();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /* If VSync is not enabled, delay to prevent CPU hogging (if set) */
        if(!(_flags & Flag::VSyncEnabled) && _minimalLoopPeriod) {
            const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
            if(loopTime < _minimalLoopPeriod)
                SDL_Delay(_minimalLoopPeriod - loopTime);
        }
        #endif

        return;
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
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

void Sdl2Application::setMouseLocked(bool enabled) {
    /** @todo Implement this in Emscripten */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowGrab(_window, enabled ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    #else
    CORRADE_ASSERT(false, "Sdl2Application::setMouseLocked(): not implemented", );
    static_cast<void>(enabled);
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

void Sdl2Application::viewportEvent(ViewportEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    viewportEvent(event.framebufferSize());
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void Sdl2Application::viewportEvent(const Vector2i&) {}
#endif

void Sdl2Application::keyPressEvent(KeyEvent&) {}
void Sdl2Application::keyReleaseEvent(KeyEvent&) {}
void Sdl2Application::mousePressEvent(MouseEvent&) {}
void Sdl2Application::mouseReleaseEvent(MouseEvent&) {}
void Sdl2Application::mouseMoveEvent(MouseMoveEvent&) {}
void Sdl2Application::mouseScrollEvent(MouseScrollEvent&) {}
void Sdl2Application::multiGestureEvent(MultiGestureEvent&) {}
void Sdl2Application::textInputEvent(TextInputEvent&) {}
void Sdl2Application::textEditingEvent(TextEditingEvent&) {}

#ifdef MAGNUM_TARGET_GL
Sdl2Application::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 0}, _depthBufferSize{24}, _stencilBufferSize{0},
    _sampleCount(0)
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _version(GL::Version::None), _sRGBCapable{false}
    #endif
    {}

Sdl2Application::GLConfiguration::~GLConfiguration() = default;
#endif

Sdl2Application::Configuration::Configuration():
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
    _title("Magnum SDL2 Application"),
    #endif
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _size{640, 480},
    #elif !defined(CORRADE_TARGET_IOS)
    _size{800, 600},
    #else
    _size{}, /* SDL2 detects someting for us */
    #endif
    _dpiScalingPolicy{DpiScalingPolicy::Default}
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    , _sampleCount(0)
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _version(GL::Version::None), _sRGBCapable{false}
    #endif
    #endif
    {}

Sdl2Application::Configuration::~Configuration() = default;

std::string Sdl2Application::KeyEvent::keyName(const Key key) {
    return SDL_GetKeyName(SDL_Keycode(key));
}

std::string Sdl2Application::KeyEvent::keyName() const {
    return keyName(_key);
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseEvent::modifiers() {
    if(_modifiersLoaded) return _modifiers;
    _modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseMoveEvent::modifiers() {
    if(_modifiersLoaded) return _modifiers;
    _modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseScrollEvent::modifiers() {
    if(_modifiersLoaded) return _modifiers;
    _modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

template class BasicScreen<Sdl2Application>;
template class BasicScreenedApplication<Sdl2Application>;

}}
