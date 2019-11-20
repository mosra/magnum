/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <cstring>
#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/String.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Platform/ScreenedApplication.hpp"
#include "Magnum/Platform/Implementation/DpiScaling.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"
#endif

namespace Magnum { namespace Platform {

#ifdef GLFW_TRUE
/* The docs say that it's the same, verify that just in case */
static_assert(GLFW_TRUE == true && GLFW_FALSE == false, "GLFW does not have sane bool values");
#endif

GlfwApplication::GlfwApplication(const Arguments& arguments): GlfwApplication{arguments, Configuration{}} {}

GlfwApplication::GlfwApplication(const Arguments& arguments, const Configuration& configuration): GlfwApplication{arguments, NoCreate} {
    create(configuration);
}

#ifdef MAGNUM_TARGET_GL
GlfwApplication::GlfwApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): GlfwApplication{arguments, NoCreate} {
    create(configuration, glConfiguration);
}
#endif

GlfwApplication::GlfwApplication(const Arguments& arguments, NoCreateT):
    _flags{Flag::Redraw}
{
    Utility::Arguments args{Implementation::windowScalingArguments()};
    #ifdef MAGNUM_TARGET_GL
    _context.reset(new GLContext{NoCreate, args, arguments.argc, arguments.argv});
    #else
    /** @todo this is duplicated here and in Sdl2Application, figure out a nice
        non-duplicated way to handle this */
    args.addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .setFromEnvironment("log")
        .parse(arguments.argc, arguments.argv);
    #endif

    /* Init GLFW */
    glfwSetErrorCallback([](int, const char* const description) {
        Error{} << description;
    });

    if(!glfwInit()) {
        Error() << "Could not initialize GLFW";
        std::exit(8);
    }

    /* Save command-line arguments */
    if(args.value("log") == "verbose") _verboseLog = true;
    const std::string dpiScaling = args.value("dpi-scaling");
    if(dpiScaling == "default")
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Default;
    #ifdef CORRADE_TARGET_APPLE
    else if(dpiScaling == "framebuffer")
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Framebuffer;
    #else
    else if(dpiScaling == "virtual")
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Virtual;
    else if(dpiScaling == "physical")
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Physical;
    #endif
    else if(dpiScaling.find_first_of(" \t\n") != std::string::npos)
        _commandLineDpiScaling = args.value<Vector2>("dpi-scaling");
    else
        _commandLineDpiScaling = Vector2{args.value<Float>("dpi-scaling")};
}

void GlfwApplication::create() {
    create(Configuration{});
}

void GlfwApplication::create(const Configuration& configuration) {
    if(!tryCreate(configuration)) std::exit(1);
}

#ifdef MAGNUM_TARGET_GL
void GlfwApplication::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration)) std::exit(1);
}
#endif

Vector2 GlfwApplication::dpiScaling(const Configuration& configuration) const {
    std::ostream* verbose = _verboseLog ? Debug::output() : nullptr;

    /* Print a helpful warning in case some extra steps are needed for HiDPI
       support */
    #ifdef CORRADE_TARGET_APPLE
    if(!Implementation::isAppleBundleHiDpiEnabled())
        Warning{} << "Platform::GlfwApplication: warning: the executable is not a HiDPI-enabled app bundle";
    #elif defined(CORRADE_TARGET_WINDOWS)
    /** @todo */
    #endif

    /* Use values from the configuration only if not overriden on command line.
       In any case explicit scaling has a precedence before the policy. */
    Implementation::GlfwDpiScalingPolicy dpiScalingPolicy{};
    if(!_commandLineDpiScaling.isZero()) {
        Debug{verbose} << "Platform::GlfwApplication: user-defined DPI scaling" << _commandLineDpiScaling.x();
        return _commandLineDpiScaling;
    } else if(UnsignedByte(_commandLineDpiScalingPolicy)) {
        dpiScalingPolicy = _commandLineDpiScalingPolicy;
    } else if(!configuration.dpiScaling().isZero()) {
        Debug{verbose} << "Platform::GlfwApplication: app-defined DPI scaling" << _commandLineDpiScaling.x();
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
    /** @todo Revisit this for GLFW 3.3 -- https://github.com/glfw/glfw/issues/677 */
    if(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Virtual) {
        /* Use Xft.dpi on X11 */
        #ifdef _MAGNUM_PLATFORM_USE_X11
        const Vector2 dpiScaling{Implementation::x11DpiScaling()};
        if(!dpiScaling.isZero()) {
            Debug{verbose} << "Platform::GlfwApplication: virtual DPI scaling" << dpiScaling.x();
            return dpiScaling;
        }

        /* Check for DPI awareness on non-RT Windows and then ask for DPI. GLFW
           is advertising the application to be DPI-aware on its own even
           without supplying an explicit manifest --
           https://github.com/glfw/glfw/blob/089ea9af227fdffdf872348923e1c12682e63029/src/win32_init.c#L564-L569
           If, for some reason, the app is still not DPI-aware, tell that to
           the user explicitly and don't even attempt to query the value if the
           app is not DPI aware. If it's desired to get the DPI value
           unconditionally, the user should use physical DPI scaling instead. */
        #elif defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
        if(!Implementation::isWindowsAppDpiAware()) {
            Warning{verbose} << "Platform::GlfwApplication: your application is not set as DPI-aware, DPI scaling won't be used";
            return Vector2{1.0f};
        }
        GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* const mode = glfwGetVideoMode(monitor);
        Vector2i monitorSize;
        glfwGetMonitorPhysicalSize(monitor, &monitorSize.x(), &monitorSize.y());
        if(monitorSize.isZero()) {
            Warning{verbose} << "Platform::GlfwApplication: the physical monitor size is zero? DPI scaling won't be used";
            return Vector2{1.0f};
        }
        auto dpi = Vector2{Vector2i{mode->width, mode->height}*25.4f/Vector2{monitorSize}};
        const Vector2 dpiScaling{dpi/96.0f};
        Debug{verbose} << "Platform::GlfwApplication: virtual DPI scaling" << dpiScaling;
        return dpiScaling;

        /* Otherwise ¯\_(ツ)_/¯ */
        #else
        Debug{verbose} << "Platform::GlfwApplication: sorry, virtual DPI scaling not implemented on this platform yet, falling back to physical DPI scaling";
        #endif
    }

    /* At this point, either the virtual DPI query failed or a physical DPI
       scaling is requested */
    CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Virtual || dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Physical);

    /* Take display DPI elsewhere. Enable only on Linux (where it gets the
       usually very-off value from X11) and on non-RT Windows (where it takes
       the UI scale value like with virtual DPI scaling, but without checking
       for DPI awareness first). */
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
    GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* const mode = glfwGetVideoMode(monitor);
    Vector2i monitorSize;
    glfwGetMonitorPhysicalSize(monitor, &monitorSize.x(), &monitorSize.y());
    if(monitorSize.isZero()) {
        Warning{verbose} << "Platform::GlfwApplication: the physical monitor size is zero? DPI scaling won't be used";
        return Vector2{1.0f};
    }
    auto dpi = Vector2{Vector2i{mode->width, mode->height}*25.4f/Vector2{monitorSize}};
    const Vector2 dpiScaling{dpi/96.0f};
    Debug{verbose} << "Platform::GlfwApplication: physical DPI scaling" << dpiScaling;
    return dpiScaling;

    /* Not implemented otherwise */
    #else
    Debug{verbose} << "Platform::GlfwApplication: sorry, physical DPI scaling not implemented on this platform yet";
    return Vector2{1.0f};
    #endif
    #endif
}

void GlfwApplication::setWindowTitle(const std::string& title) {
    glfwSetWindowTitle(_window, title.data());
}

#if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 302
void GlfwApplication::setWindowIcon(const ImageView2D& image) {
    setWindowIcon({image});
}

namespace {

template<class T> inline void packPixels(const Containers::StridedArrayView2D<const T>& in, const Containers::StridedArrayView2D<Color4ub>& out) {
    for(std::size_t row = 0; row != in.size()[0]; ++row)
        for(std::size_t col = 0; col != in.size()[1]; ++col)
            out[row][col] = in[row][col];
}

}

void GlfwApplication::setWindowIcon(std::initializer_list<ImageView2D> images) {
    /* Calculate the total size needed to allocate first so we don't allocate
       a ton of tiny arrays */
    std::size_t size = 0;
    for(const ImageView2D& image: images)
        size += sizeof(GLFWimage) + 4*image.size().product();
    Containers::Array<char> data{size};

    /* Pack array of GLFWimages and pixel data together into the memory
       allocated above */
    std::size_t offset = images.size()*sizeof(GLFWimage);
    Containers::ArrayView<GLFWimage> glfwImages = Containers::arrayCast<GLFWimage>(data.prefix(offset));
    std::size_t i = 0;
    for(const ImageView2D& image: images) {
        /* Copy and tightly pack pixels. GLFW doesn't allow arbitrary formats
           or strides (for subimages and/or Y flip), so we have to copy */
        Containers::ArrayView<char> target = data.slice(offset, offset + 4*image.size().product());
        auto out = Containers::StridedArrayView2D<Color4ub>{
            Containers::arrayCast<Color4ub>(target),
            {std::size_t(image.size().y()),
             std::size_t(image.size().x())}}.flipped<0>();
        /** @todo handle sRGB differently? */
        if(image.format() == PixelFormat::RGB8Snorm ||
           image.format() == PixelFormat::RGB8Unorm)
            packPixels(image.pixels<Color3ub>(), out);
        else if(image.format() == PixelFormat::RGBA8Snorm ||
                image.format() == PixelFormat::RGBA8Unorm)
            packPixels(image.pixels<Color4ub>(), out);
        else CORRADE_ASSERT(false, "Platform::GlfwApplication::setWindowIcon(): unexpected format" << image.format(), );

        /* Specify the image metadata */
        glfwImages[i].width = image.size().x();
        glfwImages[i].height = image.size().y();
        glfwImages[i].pixels = reinterpret_cast<unsigned char*>(target.data());

        ++i;
        offset += target.size();
    }

    glfwSetWindowIcon(_window, glfwImages.size(), glfwImages);
}
#endif

bool GlfwApplication::tryCreate(const Configuration& configuration) {
    #ifdef MAGNUM_TARGET_GL
    #ifdef GLFW_NO_API
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Contextless))
    #endif
    {
        return tryCreate(configuration, GLConfiguration{});
    }
    #endif

    CORRADE_ASSERT(!_window, "Platform::GlfwApplication::tryCreate(): window already created", false);

    /* Scale window based on DPI */
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

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

    #ifdef GLFW_NO_API
    /* Disable implicit GL context creation */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    #endif

    /* Create the window */
    _window = glfwCreateWindow(scaledWindowSize.x(), scaledWindowSize.y(), configuration.title().c_str(), monitor, nullptr);
    if(!_window) {
        Error() << "Platform::GlfwApplication::tryCreate(): cannot create window";
        glfwTerminate();
        return false;
    }

    /* Proceed with configuring other stuff that couldn't be done with window
       hints */
    if(configuration.windowFlags() >= Configuration::WindowFlag::Minimized)
        glfwIconifyWindow(_window);
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    glfwSetInputMode(_window, GLFW_CURSOR, Int(configuration.cursorMode()));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /* Set callbacks */
    setupCallbacks();

    return true;
}

namespace {

GlfwApplication::InputEvent::Modifiers currentGlfwModifiers(GLFWwindow* window) {
    static_assert(GLFW_PRESS == true && GLFW_RELEASE == false,
        "GLFW press and release constants do not correspond to bool values");

    GlfwApplication::InputEvent::Modifiers mods;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
        mods |= GlfwApplication::InputEvent::Modifier::Shift;
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
        mods |= GlfwApplication::InputEvent::Modifier::Ctrl;
    if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
        mods |= GlfwApplication::InputEvent::Modifier::Alt;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SUPER) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_SUPER))
        mods |= GlfwApplication::InputEvent::Modifier::Super;

    return mods;
}

}

#ifdef MAGNUM_TARGET_GL
bool GlfwApplication::tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    CORRADE_ASSERT(!_window && _context->version() == GL::Version::None, "Platform::GlfwApplication::tryCreate(): window with OpenGL context already created", false);

    /* Scale window based on DPI */
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

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

    /* Framebuffer setup */
    glfwWindowHint(GLFW_RED_BITS, glConfiguration.colorBufferSize().r());
    glfwWindowHint(GLFW_GREEN_BITS, glConfiguration.colorBufferSize().g());
    glfwWindowHint(GLFW_BLUE_BITS, glConfiguration.colorBufferSize().b());
    glfwWindowHint(GLFW_ALPHA_BITS, glConfiguration.colorBufferSize().a());
    glfwWindowHint(GLFW_DEPTH_BITS, glConfiguration.depthBufferSize());
    glfwWindowHint(GLFW_STENCIL_BITS, glConfiguration.stencilBufferSize());
    glfwWindowHint(GLFW_SAMPLES, glConfiguration.sampleCount());
    glfwWindowHint(GLFW_SRGB_CAPABLE, glConfiguration.isSrgbCapable());

    /* Request debug context if --magnum-gpu-validation is enabled */
    GLConfiguration::Flags glFlags = glConfiguration.flags();
    if(_context->internalFlags() & GL::Context::InternalFlag::GpuValidation)
        glFlags |= GLConfiguration::Flag::Debug;

    #ifdef GLFW_CONTEXT_NO_ERROR
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, glFlags >= GLConfiguration::Flag::NoError);
    #endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, glFlags >= GLConfiguration::Flag::Debug);
    glfwWindowHint(GLFW_STEREO, glFlags >= GLConfiguration::Flag::Stereo);

    /* Set context version, if requested */
    if(glConfiguration.version() != GL::Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(glConfiguration.version());
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        #ifndef MAGNUM_TARGET_GLES
        if(glConfiguration.version() >= GL::Version::GL320) {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, glFlags >= GLConfiguration::Flag::ForwardCompatible);
        }
        #else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        #endif

    /* Request usable version otherwise */
    } else {
        #ifndef MAGNUM_TARGET_GLES
        /* First try to create core context. This is needed mainly on macOS and
           Mesa, as support for recent OpenGL versions isn't implemented in
           compatibility contexts (which are the default). Unlike SDL2, GLFW
           requires at least version 3.2 to be able to request a core profile. */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, glFlags >= GLConfiguration::Flag::ForwardCompatible);
        #else
        /* For ES the major context version is compile-time constant */
        #ifdef MAGNUM_TARGET_GLES3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        #elif defined(MAGNUM_TARGET_GLES2)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        #else
        #error unsupported OpenGL ES version
        #endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        #endif
    }

    /* Create window. Hide it by default so we don't have distracting window
       blinking in case we have to destroy it again right away. If the creation
       succeeds, make the context current so we can query GL_VENDOR below.
       If we are on Wayland, this is causing a segfault; a blinking window is
       acceptable in this case. */
    constexpr const char waylandString[] = "wayland";
    const char* xdgSessionType = std::getenv("XDG_SESSION_TYPE");
    if(!xdgSessionType || std::strncmp(xdgSessionType, waylandString, sizeof(waylandString)) != 0)
        glfwWindowHint(GLFW_VISIBLE, false);
    else if(_verboseLog)
        Warning{} << "Platform::GlfwApplication: Wayland detected, GL context has to be created with the window visible and may cause flicker on startup";
    if((_window = glfwCreateWindow(scaledWindowSize.x(), scaledWindowSize.y(), configuration.title().c_str(), monitor, nullptr)))
        glfwMakeContextCurrent(_window);

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
    if(glConfiguration.version() == GL::Version::None && (!_window
        #ifndef CORRADE_TARGET_APPLE
        /* If context creation fails *really bad*, glGetString() may actually
           return nullptr. Check for that to avoid crashes deep inside
           strncmp(). Sorry about the UGLY code, HOPEFULLY THERE WON'T BE MORE
           WORKAROUNDS */
        || (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        vendorString && (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
         #ifdef CORRADE_TARGET_WINDOWS
         std::strncmp(vendorString, intelVendorString, sizeof(intelVendorString)) == 0 ||
         #endif
         std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0)
         && !_context->isDriverWorkaroundDisabled("no-forward-compatible-core-context"))
         #endif
    )) {
        /* Don't print any warning when doing the workaround, because the bug
           will be there probably forever */
        if(!_window) Warning{}
            << "Platform::GlfwApplication::tryCreate(): cannot create a window with core OpenGL context, falling back to compatibility context";
        else glfwDestroyWindow(_window);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
        /** @todo or keep the fwcompat? */
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);

        _window = glfwCreateWindow(scaledWindowSize.x(), scaledWindowSize.y(), configuration.title().c_str(), monitor, nullptr);
    }
    #endif

    if(!_window) {
        Error() << "Platform::GlfwApplication::tryCreate(): cannot create a window with OpenGL context";
        return false;
    }

    /* Proceed with configuring other stuff that couldn't be done with window
       hints */
    if(configuration.windowFlags() >= Configuration::WindowFlag::Minimized)
        glfwIconifyWindow(_window);
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    glfwSetInputMode(_window, GLFW_CURSOR, Int(configuration.cursorMode()));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /* Set callbacks */
    setupCallbacks();

    /* Make the final context current */
    glfwMakeContextCurrent(_window);

    /* Destroy everything when the Magnum context creation fails */
    if(!_context->tryCreate()) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }

    /* Show the window once we are sure that everything is okay */
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Hidden))
        glfwShowWindow(_window);

    /* Return true if the initialization succeeds */
    return true;
}
#endif

void GlfwApplication::setupCallbacks() {
    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowCloseCallback(_window, [](GLFWwindow* const window){
        ExitEvent e;
        static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window))->exitEvent(e);
        if(!e.isAccepted()) glfwSetWindowShouldClose(window, false);
    });
    glfwSetWindowRefreshCallback(_window, [](GLFWwindow* const window){
        /* Properly redraw after the window is restored from minimized state */
        static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window))->drawEvent();
    });
    #ifdef MAGNUM_TARGET_GL
    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* const window, const int w, const int h) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));
        ViewportEvent e{app.windowSize(), {w, h}, app.dpiScaling()};
        app.viewportEvent(e);
    });
    #else
    glfwSetWindowSizeCallback(_window, [](GLFWwindow* const window, const int w, const int h) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));
        ViewportEvent e{{w, h}, app.dpiScaling()};
        app.viewportEvent(e);
    });
    #endif
    glfwSetKeyCallback(_window, [](GLFWwindow* const window, const int key, int, const int action, const int mods) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        KeyEvent e(static_cast<KeyEvent::Key>(key), {static_cast<InputEvent::Modifier>(mods)}, action == GLFW_REPEAT);

        if(action == GLFW_PRESS || action == GLFW_REPEAT)
            app.keyPressEvent(e);
        else if(action == GLFW_RELEASE)
            app.keyReleaseEvent(e);
    });
    glfwSetMouseButtonCallback(_window, [](GLFWwindow* const window, const int button, const int action, const int mods) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        double x, y;
        glfwGetCursorPos(window, &x, &y);
        MouseEvent e(static_cast<MouseEvent::Button>(button), {Int(x), Int(y)}, {static_cast<InputEvent::Modifier>(mods)});

        if(action == GLFW_PRESS) /* we don't handle GLFW_REPEAT */
            app.mousePressEvent(e);
        else if(action == GLFW_RELEASE)
            app.mouseReleaseEvent(e);
    });
    glfwSetCursorPosCallback(_window, [](GLFWwindow* const window, const double x, const double y) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));
        /* Avoid bogus offset at first -- report 0 when the event is called for
           the first time */
        Vector2i position{Int(x), Int(y)};
        MouseMoveEvent e{window, position,
            app._previousMouseMovePosition == Vector2i{-1} ? Vector2i{} :
            position - app._previousMouseMovePosition};
        app._previousMouseMovePosition = position;
        app.mouseMoveEvent(e);
    });
    glfwSetScrollCallback(_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        MouseScrollEvent e(window, Vector2{Float(xoffset), Float(yoffset)});
        static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window))->mouseScrollEvent(e);
    });
    glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int codepoint) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        if(!(app._flags & Flag::TextInputActive)) return;

        char utf8[4]{};
        const std::size_t size = Utility::Unicode::utf8(codepoint, utf8);
        TextInputEvent e{{utf8, size}};
        app.textInputEvent(e);
    });
}

GlfwApplication::~GlfwApplication() {
    glfwDestroyWindow(_window);
    for(auto& cursor: _cursors)
        glfwDestroyCursor(cursor);
    glfwTerminate();
}

Vector2i GlfwApplication::windowSize() const {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::windowSize(): no window opened", {});

    Vector2i size;
    glfwGetWindowSize(_window, &size.x(), &size.y());
    return size;
}

#if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 302
void GlfwApplication::setMinWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::setMinWindowSize(): no window opened", );

    const Vector2i newSize = _dpiScaling*size;
    glfwSetWindowSizeLimits(_window, newSize.x(), newSize.y(), _maxWindowSize.x(), _maxWindowSize.y());
    _minWindowSize = newSize;
}

void GlfwApplication::setMaxWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::setMaxWindowSize(): no window opened", );

    const Vector2i newSize = _dpiScaling*size;
    glfwSetWindowSizeLimits(_window, _minWindowSize.x(), _minWindowSize.y(), newSize.x(), newSize.y());
    _maxWindowSize = newSize;
}
#endif

#ifdef MAGNUM_TARGET_GL
Vector2i GlfwApplication::framebufferSize() const {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::framebufferSize(): no window opened", {});

    Vector2i size;
    glfwGetFramebufferSize(_window, &size.x(), &size.y());
    return size;
}
#endif

void GlfwApplication::setSwapInterval(const Int interval) {
    glfwSwapInterval(interval);
}

int GlfwApplication::exec() {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::exec(): no window opened", {});

    while(mainLoopIteration()) {}

    return _exitCode;
}

bool GlfwApplication::mainLoopIteration() {
    if(_flags & Flag::Redraw) {
        _flags &= ~Flag::Redraw;
        drawEvent();
    }
    glfwPollEvents();

    return !glfwWindowShouldClose(_window);
}

namespace {

constexpr Int CursorMap[] {
    GLFW_ARROW_CURSOR,
    GLFW_IBEAM_CURSOR,
    GLFW_CROSSHAIR_CURSOR,
    GLFW_HRESIZE_CURSOR,
    GLFW_VRESIZE_CURSOR,
    GLFW_HAND_CURSOR
};

}

void GlfwApplication::setCursor(Cursor cursor) {
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(_cursors));

    _cursor = cursor;

    if(cursor == Cursor::Hidden) {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        return;
    } else if(cursor == Cursor::HiddenLocked) {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        return;
    } else {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if(!_cursors[UnsignedInt(cursor)])
        _cursors[UnsignedInt(cursor)] = glfwCreateStandardCursor(CursorMap[UnsignedInt(cursor)]);

    glfwSetCursor(_window, _cursors[UnsignedInt(cursor)]);
}

GlfwApplication::Cursor GlfwApplication::cursor() {
    return _cursor;
}

auto GlfwApplication::MouseMoveEvent::buttons() -> Buttons {
    if(!_buttons) {
        _buttons = Buttons{};
        for(const Int button: {GLFW_MOUSE_BUTTON_LEFT,
                               GLFW_MOUSE_BUTTON_MIDDLE,
                               GLFW_MOUSE_BUTTON_RIGHT}) {
            if(glfwGetMouseButton(_window, button) == GLFW_PRESS)
                *_buttons |= Button(1 << button);
        }
    }

    return *_buttons;
}

auto GlfwApplication::MouseMoveEvent::modifiers() -> Modifiers {
    if(!_modifiers) _modifiers = currentGlfwModifiers(_window);
    return *_modifiers;
}

Vector2i GlfwApplication::MouseScrollEvent::position() {
    if(!_position) {
        Vector2d position;
        glfwGetCursorPos(_window, &position.x(), &position.y());
        _position = Vector2i{position};
    }

    return *_position;
}

auto GlfwApplication::MouseScrollEvent::modifiers() -> Modifiers {
    if(!_modifiers) _modifiers = currentGlfwModifiers(_window);
    return *_modifiers;
}

void GlfwApplication::exitEvent(ExitEvent& event) {
    event.setAccepted();
}

void GlfwApplication::viewportEvent(ViewportEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    viewportEvent(event.windowSize());
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void GlfwApplication::viewportEvent(const Vector2i&) {}
#endif

void GlfwApplication::keyPressEvent(KeyEvent&) {}
void GlfwApplication::keyReleaseEvent(KeyEvent&) {}
void GlfwApplication::mousePressEvent(MouseEvent&) {}
void GlfwApplication::mouseReleaseEvent(MouseEvent&) {}
void GlfwApplication::mouseMoveEvent(MouseMoveEvent&) {}
void GlfwApplication::mouseScrollEvent(MouseScrollEvent&) {}
void GlfwApplication::textInputEvent(TextInputEvent&) {}

#ifdef MAGNUM_TARGET_GL
GlfwApplication::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 0}, _depthBufferSize{24}, _stencilBufferSize{0},
    _sampleCount{0}, _version{GL::Version::None},
    #ifndef MAGNUM_TARGET_GLES
    _flags{Flag::ForwardCompatible},
    #else
    _flags{},
    #endif
    _srgbCapable{false} {}

GlfwApplication::GLConfiguration::~GLConfiguration() = default;
#endif

GlfwApplication::Configuration::Configuration():
    _title{"Magnum GLFW Application"},
    _size{800, 600},
    _windowFlags{WindowFlag::Focused},
    _dpiScalingPolicy{DpiScalingPolicy::Default} {}

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
