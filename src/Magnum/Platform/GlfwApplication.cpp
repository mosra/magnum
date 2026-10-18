/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016, 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Konstantinos Chatzilygeroudis <costashatz@gmail.com>
    Copyright © 2019, 2020 Marco Melorio <m.melorio@icloud.com>
    Copyright © 2022 Andreas Leroux <andreas.leroux@epitech.eu>

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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Unicode.h>
#include <Corrade/Utility/System.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/Time.h"
#include "Magnum/Platform/ScreenedApplication.hpp"
#include "Magnum/Platform/Implementation/DpiScaling.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Version.h"
#endif

namespace Magnum { namespace Platform {

using namespace Containers::Literals;
using namespace Math::Literals;

#ifdef GLFW_TRUE
/* The docs say that it's the same, verify that just in case */
static_assert(GLFW_TRUE == true && GLFW_FALSE == false, "GLFW does not have sane bool values");
#endif

enum class GlfwApplication::Flag: UnsignedByte {
    Redraw = 1 << 0,
    TextInputActive = 1 << 1,
    VSyncEnabled = 1 << 2,
    NoTickEvent = 1 << 3,
    Exit = 1 << 4
};

GlfwApplication::GlfwApplication(const Arguments& arguments): GlfwApplication{arguments, Configuration{}} {}

GlfwApplication::GlfwApplication(const Arguments& arguments, const Configuration& configuration): GlfwApplication{arguments, NoCreate} {
    create(configuration);
}

#ifdef MAGNUM_TARGET_GL
GlfwApplication::GlfwApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): GlfwApplication{arguments, NoCreate} {
    create(configuration, glConfiguration);
}
#endif

namespace {

/* GLFW before version 3.4 is compiled either for X11 or for Wayland, and
   reports that in the version string. Version 3.4 adds runtime platform
   selection and convenience queries. Emulate the newer API on older versions
   for convenience. */
#if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 304
#define GLFW_PLATFORM_X11 1
#define GLFW_PLATFORM_WAYLAND 2
int glfwGetPlatform() {
    const Containers::StringView platform = glfwGetVersionString();
    if(platform.contains("X11"_s))
        return GLFW_PLATFORM_X11;
    if(platform.contains("Wayland"_s))
        return GLFW_PLATFORM_WAYLAND;
    return {};
}
#endif

}

GlfwApplication::GlfwApplication(const Arguments& arguments, NoCreateT):
    _flags{Flag::Redraw}
{
    Utility::Arguments args{Implementation::windowScalingArguments()};
    #ifdef MAGNUM_TARGET_GL
    /** @todo this means GL::Context is constructed even with Contextless
        present, do we actually want that at all? It's just for argument
        parsing (which affect GL context creation), so maybe separate that from
        the GL::Context somehow? Chicken and egg... */
    _context.emplace(NoCreate, args, arguments.argc, arguments.argv);
    #else
    /** @todo this is duplicated here, in Sdl2Application and in
        EmscriptenApplication, figure out a nice non-duplicated way to handle
        this */
    args.addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .setFromEnvironment("log")
        .parse(arguments.argc, arguments.argv);
    #endif

    /* GLFW before 3.3 doesn't have glfwGetError(), use glfwSetErrorCallback()
       even before glfwInit() in that case. It'll be on two lines but better
       than getting no reason at all. */
    #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303
    glfwSetErrorCallback([](int, const char* const description) {
        Error{} << "Platform::GlfwApplication:" << description;
    });
    #endif

    /* Init GLFW */
    #ifdef CORRADE_TARGET_APPLE
    /* Don't change current working directory to Resources/ in the app bundle
       on Apple platforms. Not sure why this would be done only on a single
       platform of all, I guess it was for compatibility with SDL1 (and then
       SDL2+ doesn't do this anymore but in GLFW it stays)?
        https://wiki.libsdl.org/SDL2/README/macos#working-directory */
    glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, false);
    #endif
    if(!glfwInit()) {
        /* On GLFW before 3.3 the error is printed by the callback that's set
           up above */
        #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303
        Error{} << "Platform::GlfwApplication: could not initialize GLFW";
        #else
        const char* error = nullptr;
        CORRADE_INTERNAL_ASSERT_OUTPUT(glfwGetError(&error) != GLFW_NO_ERROR && error);
        Error{} << "Platform::GlfwApplication: could not initialize GLFW:" << error;
        #endif
        std::exit(8);
    }

    /* Set error callback for further errors. On GLFW 3.3+ done after init so
       we don't print the error message twice. */
    #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
    glfwSetErrorCallback([](int, const char* const description) {
        Error{} << "Platform::GlfwApplication:" << description;
    });
    #endif

    /* Save command-line arguments */
    if(args.value("log") == "verbose")
        _verboseLog = true;
    const Containers::StringView dpiScaling = args.value<Containers::StringView>("dpi-scaling");
    #if defined(CORRADE_TARGET_UNIX) && !defined(CORRADE_TARGET_APPLE)
    const bool isWayland = glfwGetPlatform() == GLFW_PLATFORM_WAYLAND;
    #endif
    if(dpiScaling == "default"_s)
        /* Here we don't need to resolve to Virtual or Framebuffer on Wayland
           as the value will get overriden by the configuration-supplied
           value */
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Default;
    #if defined(CORRADE_TARGET_APPLE) || defined(CORRADE_TARGET_UNIX)
    else if(dpiScaling == "framebuffer"_s
        #ifndef CORRADE_TARGET_APPLE
        /* On Wayland framebuffer scaling is the only option. If not running
           on Wayland, don't recognize --magnum-dpi-scaling framebuffer at
           all. */
        && isWayland
        #endif
    )
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Framebuffer;
    #endif
    #ifndef CORRADE_TARGET_APPLE
    /* If running on a Wayland backend where framebuffer scaling is the only
       option, these two aren't allowed there */
    else if(dpiScaling == "virtual"_s
        #ifdef CORRADE_TARGET_UNIX
        && !isWayland
        #endif
    )
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Virtual;
    else if(dpiScaling == "physical"_s
        #ifdef CORRADE_TARGET_UNIX
        && !isWayland
        #endif
    )
        _commandLineDpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Physical;
    #endif
    else {
        if(dpiScaling.containsAny(" \t\n"_s))
            _commandLineDpiScaling = args.value<Vector2>("dpi-scaling");
        else
            _commandLineDpiScaling = Vector2{args.value<Float>("dpi-scaling")};
        /* If e.g. `virtual` is passed on macOS, it falls back to parsing it as
           a number, resulting in zero, which then leads to nasty assertions or
           other misbehavior. Catch that early and exit. */
        if(Math::equal(_commandLineDpiScaling.product(), 0.0f)) {
            Error{} << "Platform::GlfwApplication: invalid DPI scaling" << dpiScaling;
            std::exit(2);
        }
    }
}

void GlfwApplication::create() {
    create(Configuration{});
}

void GlfwApplication::create(const Configuration& configuration) {
    if(!tryCreate(configuration))
        std::exit(1);
}

#ifdef MAGNUM_TARGET_GL
void GlfwApplication::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration))
        std::exit(1);
}
#endif

Vector2 GlfwApplication::dpiScaling(const Configuration& configuration) {
    return dpiScalingInternal(configuration.dpiScalingPolicy(), configuration.dpiScaling());
}

Vector2 GlfwApplication::dpiScalingInternal(const Implementation::GlfwDpiScalingPolicy configurationDpiScalingPolicy, const Vector2& configurationDpiScaling, const bool silentLog) const {
    std::ostream* const output = silentLog ? nullptr : Debug::output();
    std::ostream* const verbose = _verboseLog && !silentLog ? Debug::output() : nullptr;

    /* Print a helpful warning in case some extra steps are needed for HiDPI
       support. Do that even before parsing the command-line- or
       configuration-supplied values to let the user know those might not
       behave as expected. */
    #ifdef CORRADE_TARGET_APPLE
    if(!Implementation::isAppleBundleHiDpiEnabled()) {
        Warning{output} << "Platform::GlfwApplication: warning: the executable is not a HiDPI-enabled app bundle";
    }
    #endif

    /* Use values from the configuration only if not overridden on command line
       to something non-default. In any case explicit scaling has a precedence
       before the policy. */
    #ifndef CORRADE_TARGET_APPLE
    Implementation::GlfwDpiScalingPolicy dpiScalingPolicy{};
    #endif
    #if defined(CORRADE_TARGET_UNIX) && !defined(CORRADE_TARGET_APPLE)
    const int glfwPlatform = glfwGetPlatform();
    #endif
    if(!_commandLineDpiScaling.isZero()) {
        Debug{verbose} << "Platform::GlfwApplication: user-defined DPI scaling" << _commandLineDpiScaling;
        return _commandLineDpiScaling;
    } else if(_commandLineDpiScalingPolicy != Implementation::GlfwDpiScalingPolicy::Default) {
        #ifndef CORRADE_TARGET_APPLE
        /* On non-Apple Unix the constructor guaranteed this to be Framebuffer
           if on Wayland */
        dpiScalingPolicy = _commandLineDpiScalingPolicy;
        #ifdef CORRADE_TARGET_UNIX
        CORRADE_INTERNAL_ASSERT(glfwPlatform != GLFW_PLATFORM_WAYLAND || dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Framebuffer);
        #endif
        #endif
    } else if(!configurationDpiScaling.isZero()) {
        Debug{verbose} << "Platform::GlfwApplication: app-defined DPI scaling" << configurationDpiScaling;
        return configurationDpiScaling;
    } else {
        #ifndef CORRADE_TARGET_APPLE
        dpiScalingPolicy = configurationDpiScalingPolicy;
        #ifdef CORRADE_TARGET_UNIX
        if(glfwPlatform == GLFW_PLATFORM_WAYLAND) {
            /* By default, the policy is set to Default (heh), not Framebuffer,
               don't warn in that case. Compared to --magnum-dpi-scaling this
               is not a hard error as it's generally too annoing for the app
               code to check whether X11 or Wayland is used before actually
               creating the window */
            if(dpiScalingPolicy != Implementation::GlfwDpiScalingPolicy::Framebuffer &&
               dpiScalingPolicy != Implementation::GlfwDpiScalingPolicy::Default)
            {
                CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Virtual || dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Physical);
                Warning{output} << "Platform::GlfwApplication:" << (dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Virtual ? "virtual" : "physical") << "DPI scaling not supported on this platform, falling back to framebuffer scaling";
            }
            dpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Framebuffer;
        } else {
            /* Conversely, if we're not on Wayland, warn if attempting to use
               framebuffer scaling and fall back to virtual */
            if(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Framebuffer) {
                Warning{output} << "Platform::GlfwApplication: framebuffer DPI scaling not supported on this platform, falling back to virtual scaling";
                dpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Virtual;
            }
            if(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Default)
                dpiScalingPolicy = Implementation::GlfwDpiScalingPolicy::Virtual;
        }
        #endif
        #else
        static_cast<void>(configurationDpiScalingPolicy);
        #endif
    }

    /* On Windows GLFW is advertising the application to be DPI-aware on its
       own even without supplying an explicit manifest -- https://github.com/glfw/glfw/blob/089ea9af227fdffdf872348923e1c12682e63029/src/win32_init.c#L564-L569
       If, for some reason, the app is still not DPI-aware, tell that to the
       user explicitly and don't even attempt to query the value as neither
       virtual nor physical scaling would behave as expected. This is done only
       after parsing the command-line- and configuration-supplied values so a
       custom value can still be supplied if desired. */
    #if defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
    if(!Implementation::isWindowsAppDpiAware()) {
        Warning{output} << "Platform::GlfwApplication: your application is not set as DPI-aware, DPI scaling won't be used";
        return Vector2{1.0f};
    }
    #endif

    /* There's no choice on Apple, it's all controlled by the plist file. So
       unless someone specified custom scaling via config or command-line
       above, return the default. */
    #ifdef CORRADE_TARGET_APPLE
    return Vector2{1.0f};

    /* Otherwise there's a choice between virtual and physical DPI scaling, or
       possibly only framebuffer scaling if we're on Wayland because things are
       always made more complex than they need to be, ugh */
    #else
    #if defined(CORRADE_TARGET_UNIX) && !defined(CORRADE_TARGET_APPLE)
    if(glfwPlatform == GLFW_PLATFORM_WAYLAND) {
        CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Framebuffer);
        Debug{verbose} << "Platform::GlfwApplication: using framebuffer DPI scaling on Wayland";
        return Vector2{1.0f};
    }
    #endif

    /* On non-Wayland Unix and Windows try to get virtual DPI scaling first, if
       supported and requested */
    if(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Virtual) {
        /* If we're on GLFW < 3.3 where we cannot use
           glfwGetMonitorContentScale(), X11 was found at compile time and the
           session is actually running on X11, do a manual query of Xft.dpi.
           Checking for X11 is done with glfwGetPlatform(), not using
           XDG_SESSION_TYPE because one can be on Wayland (with
           XDG_SESSION_TYPE=wayland) but set
           `glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);` to force use of
           XWayland. */
        #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303 && defined(_MAGNUM_PLATFORM_USE_X11)
        if(glfwPlatform == GLFW_PLATFORM_X11) {
            const Vector2 dpiScaling{Implementation::x11DpiScaling(output)};
            if(!dpiScaling.isZero()) {
                Debug{verbose} << "Platform::GlfwApplication: virtual DPI scaling" << dpiScaling.x();
                return dpiScaling;
            }
        }
        #endif

        /* On Unix with GLFW 3.3+ we can use glfwGetMonitorContentScale() that
           returns a correct value for both X11 and Wayland, same for non-RT
           Windows as long as the app is DPI-aware, which was checked above.

           On Unix with GLFW < 3.3, if running X11, the Xft.dpi branch above
           was taken, and if running Wayland or Windows, the below "sorry, not
           implemented" message is printed instead. */
        #if (defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))) && GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
        GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
        Vector2 dpiScaling;
        glfwGetMonitorContentScale(monitor, &dpiScaling.x(), &dpiScaling.y());
        Debug{verbose} << "Platform::GlfwApplication: virtual DPI scaling" << dpiScaling;
        return dpiScaling;

        /* Otherwise ¯\_(ツ)_/¯. In particular, this gets printed for GLFW < 3.3
           on Wayland or on Windows. */
        #else
        Warning{output} << "Platform::GlfwApplication: sorry, virtual DPI scaling not implemented on this platform, falling back to physical DPI scaling";
        #endif
    }

    /* At this point, either the virtual DPI query failed, isn't implemented or
       a physical DPI scaling is requested */
    CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Virtual || dpiScalingPolicy == Implementation::GlfwDpiScalingPolicy::Physical);

    /* Physical DPI scaling. Enable only on Linux + X11 (where it gets the
       usually very-off value) and on non-RT Windows (where it calculates it
       from actual monitor dimensions). */
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
    GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* const mode = glfwGetVideoMode(monitor);
    /* On X11, XRandR might return zero values, which would then cause a
       division by zero here. GLFW 3.3.1+ works around that by assuming 96 DPI:
        https://github.com/glfw/glfw/commit/7c33fb22fdd55af5417a48c8594fc495fb1f81a2
       Subsequently, the same issue was fixed for Wayland in 3.3.3:
        https://github.com/glfw/glfw/pull/1784 */
    Vector2i monitorSize;
    glfwGetMonitorPhysicalSize(monitor, &monitorSize.x(), &monitorSize.y());
    CORRADE_INTERNAL_ASSERT(!monitorSize.isZero());
    auto dpi = Vector2{Vector2i{mode->width, mode->height}*25.4f/Vector2{monitorSize}};
    const Vector2 dpiScaling{dpi/96.0f};
    Debug{verbose} << "Platform::GlfwApplication: physical DPI scaling" << dpiScaling;
    return dpiScaling;

    /* Not implemented otherwise */
    #else
    Warning{output} << "Platform::GlfwApplication: sorry, physical DPI scaling not implemented on this platform, falling back to no scaling";
    return Vector2{1.0f};
    #endif
    #endif
}

void GlfwApplication::setWindowTitle(const Containers::StringView title) {
    glfwSetWindowTitle(_window, Containers::String::nullTerminatedView(title).data());
}

void GlfwApplication::setWindowIcon(const ImageView2D& image) {
    setWindowIcon({&image, 1});
}

namespace {

/** @todo use some optimized RGB -> RGBA utility from Math/ColorBatch.h for
    this once it exists, in particular one that makes use of contiguous rows
    for optimization */
template<class T> inline void packPixels(const Containers::StridedArrayView2D<const T>& in, const Containers::StridedArrayView2D<Color4ub>& out) {
    for(std::size_t row = 0; row != in.size()[0]; ++row)
        for(std::size_t col = 0; col != in.size()[1]; ++col)
            out[row][col] = in[row][col];
}

}

void GlfwApplication::setWindowIcon(const Containers::ArrayView<const ImageView2D> images) {
    /* Calculate the total size needed to allocate first so we don't allocate
       a ton of tiny arrays */
    std::size_t size = 0;
    for(const ImageView2D& image: images)
        size += sizeof(GLFWimage) + 4*image.size().product();
    Containers::Array<char> data{NoInit, size};

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
        else CORRADE_ASSERT_UNREACHABLE("Platform::GlfwApplication::setWindowIcon(): unexpected format" << image.format(), );

        /* Specify the image metadata */
        glfwImages[i].width = image.size().x();
        glfwImages[i].height = image.size().y();
        glfwImages[i].pixels = reinterpret_cast<unsigned char*>(target.data());

        ++i;
        offset += target.size();
    }

    glfwSetWindowIcon(_window, glfwImages.size(), glfwImages.data());
}

void GlfwApplication::setWindowIcon(std::initializer_list<ImageView2D> images) {
    setWindowIcon(Containers::arrayView(images));
}

/* Returns a monitor pointer that gets subsequently used for enabling
   fullscreen, or nullptr if fullscreen isn't meant to be enabled. Strange API.
   For a fullscreen window the windowSizeToUse gets overwritten with actual
   screen size. */
GLFWmonitor* GlfwApplication::setupWindowFlags(const WindowFlags flags, Vector2i& windowSizeToUse) {
    glfwWindowHint(GLFW_DECORATED, !(flags >= WindowFlag::Borderless));
    glfwWindowHint(GLFW_RESIZABLE, flags >= WindowFlag::Resizable);
    glfwWindowHint(GLFW_VISIBLE, !(flags >= WindowFlag::Hidden));
    glfwWindowHint(GLFW_MAXIMIZED, flags >= WindowFlag::Maximized);
    glfwWindowHint(GLFW_FLOATING, flags >= WindowFlag::AlwaysOnTop);

    /* Fullscreen setup. GLFW, would it kill you to have a builtin API for
       desktop fullscreen? I really don't feel like it's my responsibility to
       deal with all these.*/
    GLFWmonitor* const monitor = flags >= WindowFlag::Fullscreen ?
        glfwGetPrimaryMonitor() : nullptr;
    if(flags >= WindowFlag::FullscreenDesktop) {
        const GLFWvidmode* const mode = glfwGetVideoMode(monitor);
        /* Save the window size supplied from configuration to reuse later when
           going back from fullscreen in clearWindowFlags() */
        _windowPositionToRestore = {};
        _windowSizeToRestore = windowSizeToUse;
        windowSizeToUse = {mode->width, mode->height};
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    }

    return monitor;
}

void GlfwApplication::toggleWindowFlags(
    #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303
    const char* const messagePrefix,
    #endif
    const WindowFlags flags, const bool enabled
) {
    /* FullscreenDesktop is a superset of Fullscreen. Altogether, this whole
       fullscreen thing is so needlessly complicated for no reason... GLFW,
       could you just give me glfwSetWindowFullscreen() or something like SDL
       has, and deal with all this nonsense internally? */
    if(flags >= WindowFlag::Fullscreen) {
        if(enabled) {
            /* The way the glfwSetWindowMonitor() API is designed throws away
               all information about previous window position or size, which
               means we have to cache those. Do this only if the values are not
               saved already so e.g. calling addWindowFlags(Fullscreen) twice
               in a row doesn't cause the previous values to be overwritten
               with the screen size. */
            if(_windowSizeToRestore.isZero() && _windowPositionToRestore.isZero()) {
                glfwGetWindowSize(_window, &_windowSizeToRestore.x(), &_windowSizeToRestore.y());
                glfwGetWindowPos(_window, &_windowPositionToRestore.x(), &_windowPositionToRestore.y());
            }
            /* Unfortunately there isn't a way to get the screen on which an
               existing window is (glfwGetWindowMonitor() returns null for
               non-fullscreen windows, FFS), so toggling fullscreen may
               actually cause the window to jump onto some other display
               entirely. Sigh. */
            GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
            if(flags >= WindowFlag::FullscreenDesktop) {
                const GLFWvidmode* const mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            } else {
                glfwSetWindowMonitor(_window, monitor, 0, 0, _windowSizeToRestore.x(), _windowSizeToRestore.y(), GLFW_DONT_CARE);
            }
        } else {
            /* If disabling fullscreen, restore back to the previously cached
               position and size. Clear the cached values so they get correctly
               remembered above next time fullscreen is enabled. */
            /** @todo when the window is already created as fullscreen, such as
                with PlatformGlfwApplicationTest --fullscreen-desktop, toggling
                it back to a regular window doesn't actually use the window
                size saved in setupWindowFlags() but the window is maximized;
                one has to call glfwRestoreWindow() to un-maximize it, which
                still causes it to have a strange size, and then only a call to
                glfwSetWindowSize() makes it the actual desired size ... WTF */
            glfwSetWindowMonitor(_window, nullptr, _windowPositionToRestore.x(), _windowPositionToRestore.y(), _windowSizeToRestore.x(), _windowSizeToRestore.y(), GLFW_DONT_CARE);
            _windowSizeToRestore = {};
            _windowPositionToRestore = {};
        }
    }

    #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
    if(flags >= WindowFlag::Borderless)
        glfwSetWindowAttrib(_window, GLFW_DECORATED, !enabled);
    if(flags >= WindowFlag::Resizable)
        glfwSetWindowAttrib(_window, GLFW_RESIZABLE, enabled);
    #endif
    if(flags >= WindowFlag::Hidden)
        enabled ? glfwHideWindow(_window) : glfwShowWindow(_window);
    if(flags >= WindowFlag::Maximized)
        enabled ? glfwMaximizeWindow(_window) : glfwRestoreWindow(_window);
    if(flags >= WindowFlag::Minimized)
        enabled ? glfwIconifyWindow(_window) : glfwRestoreWindow(_window);
    #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
    if(flags >= WindowFlag::AlwaysOnTop)
        glfwSetWindowAttrib(_window, GLFW_FLOATING, enabled);
    #endif

    #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303
    if(flags & (WindowFlag::Borderless|
                WindowFlag::Resizable|
                WindowFlag::AlwaysOnTop))
        Warning{} << messagePrefix << "toggling WindowFlag::Borderless, Resizable or AlwaysOnTop is not supported";
    #endif
}

void GlfwApplication::addWindowFlags(const WindowFlags flags) {
    toggleWindowFlags(
        #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303
        "Platform::GlfwApplication::addWindowFlags():",
        #endif
        flags, true);
}

void GlfwApplication::clearWindowFlags(const WindowFlags flags) {
    toggleWindowFlags(
        #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR < 303
        "Platform::GlfwApplication::clearWindowFlags():",
        #endif
        flags, false);
}

bool GlfwApplication::tryCreate(const Configuration& configuration) {
    #ifdef MAGNUM_TARGET_GL
    if(!(configuration.flags() & Configuration::Flag::Contextless)) {
        return tryCreate(configuration, GLConfiguration{});
    }
    #endif

    CORRADE_ASSERT(!_window, "Platform::GlfwApplication::tryCreate(): window already created", false);

    /* Save DPI scaling values from configuration for future use in viewport
       events, scale window based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    _dpiScaling = dpiScalingInternal(_configurationDpiScalingPolicy, _configurationDpiScaling);
    /*mutable*/ Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

    /* Setup window flags, the monitor pointer is used below to enable
       fullscreen and the window size gets updated if desktop fullscreen is
       requested */
    GLFWmonitor* const monitor = setupWindowFlags(configuration.windowFlags(), scaledWindowSize);

    /* Disable implicit GL context creation */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    /* Create the window. The title is internally stored as a
       Containers::String so it should be null-terminated always. */
    CORRADE_INTERNAL_ASSERT(configuration.title().flags() & Containers::StringViewFlag::NullTerminated);
    _window = glfwCreateWindow(scaledWindowSize.x(), scaledWindowSize.y(), configuration.title().data(), monitor, nullptr);
    if(!_window) {
        Error() << "Platform::GlfwApplication::tryCreate(): cannot create window";
        glfwTerminate();
        return false;
    }

    /* Proceed with configuring other stuff that couldn't be done with window
       hints */
    if(configuration.windowFlags() >= WindowFlag::Minimized)
        glfwIconifyWindow(_window);
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    glfwSetInputMode(_window, GLFW_CURSOR, Int(configuration.cursorMode()));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    return true;
}

#ifdef MAGNUM_TARGET_GL
bool GlfwApplication::tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    CORRADE_ASSERT(!(configuration.flags() & Configuration::Flag::Contextless),
        "Platform::GlfwApplication::tryCreate(): cannot pass Configuration::Flag::Contextless when creating an OpenGL context", false);
    CORRADE_ASSERT(!_window && _context->version() == GL::Version::None,
        "Platform::GlfwApplication::tryCreate(): window with OpenGL context already created", false);

    /* Save DPI scaling values from configuration for future use in viewport
       events, scale window based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    _dpiScaling = dpiScalingInternal(_configurationDpiScalingPolicy, _configurationDpiScaling);
    /*mutable*/ Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

    /* Setup window flags, the monitor pointer is used below to enable
       fullscreen and the window size gets updated if desktop fullscreen is
       requested */
    GLFWmonitor* const monitor = setupWindowFlags(configuration.windowFlags(), scaledWindowSize);

    /* Framebuffer setup */
    glfwWindowHint(GLFW_RED_BITS, glConfiguration.colorBufferSize().r());
    glfwWindowHint(GLFW_GREEN_BITS, glConfiguration.colorBufferSize().g());
    glfwWindowHint(GLFW_BLUE_BITS, glConfiguration.colorBufferSize().b());
    glfwWindowHint(GLFW_ALPHA_BITS, glConfiguration.colorBufferSize().a());
    glfwWindowHint(GLFW_DEPTH_BITS, glConfiguration.depthBufferSize());
    glfwWindowHint(GLFW_STENCIL_BITS, glConfiguration.stencilBufferSize());
    glfwWindowHint(GLFW_SAMPLES, glConfiguration.sampleCount());
    glfwWindowHint(GLFW_SRGB_CAPABLE, glConfiguration.isSrgbCapable());

    /* Request debug context if GpuValidation is enabled either via the
       configuration or via command-line */
    GLConfiguration::Flags glFlags = glConfiguration.flags();
    if((glFlags & GLConfiguration::Flag::GpuValidation) || (_context->configurationFlags() & GL::Context::Configuration::Flag::GpuValidation))
        glFlags |= GLConfiguration::Flag::Debug;
    else if((glFlags & GLConfiguration::Flag::GpuValidationNoError) || (_context->configurationFlags() & GL::Context::Configuration::Flag::GpuValidationNoError))
        glFlags |= GLConfiguration::Flag::NoError;

    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, glFlags >= GLConfiguration::Flag::NoError);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, glFlags >= GLConfiguration::Flag::Debug);
    glfwWindowHint(GLFW_STEREO, glFlags >= GLConfiguration::Flag::Stereo);

    /* Set context version, if requested */
    if(glConfiguration.version() != GL::Version::None) {
        const Containers::Pair<Int, Int> versionMajorMinor = version(glConfiguration.version());
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajorMinor.first());
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMajorMinor.second());
        #ifndef MAGNUM_TARGET_GLES
        if(glConfiguration.version() >= GL::Version::GL320) {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, glFlags >= GLConfiguration::Flag::ForwardCompatible);
        }
        #else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        #endif
        #ifdef MAGNUM_TARGET_EGL /* Force EGL if desired */
        /** @todo fails on NVidia X11: https://github.com/glfw/glfw/issues/2365 */
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
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
        #ifdef MAGNUM_TARGET_GLES2
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        #endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        #endif
        #ifdef MAGNUM_TARGET_EGL /* Force EGL if desired */
        /** @todo fails on NVidia X11: https://github.com/glfw/glfw/issues/2365 */
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
        #endif
    }

    /* Create window. Hide it by default so we don't have distracting window
       blinking in case we have to destroy it again right away. If the creation
       succeeds, make the context current so we can query GL_VENDOR below. */
    glfwWindowHint(GLFW_VISIBLE, false);
    /* The title is internally stored as a Containers::String so it should be
       null-terminated always */
    CORRADE_INTERNAL_ASSERT(configuration.title().flags() & Containers::StringViewFlag::NullTerminated);
    if((_window = glfwCreateWindow(scaledWindowSize.x(), scaledWindowSize.y(), configuration.title().data(), monitor, nullptr)))
        glfwMakeContextCurrent(_window);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1, if version is not
       user-specified and either core context creation fails or we are on
       binary NVidia/AMD drivers on Linux/Windows or Intel Windows drivers.
       Instead of creating forward-compatible context with highest available
       version, they force the version to the one specified, which is
       completely useless behavior. */
    #ifndef CORRADE_TARGET_APPLE
    Containers::StringView vendorString;
    #endif
    if(glConfiguration.version() == GL::Version::None && (!_window
        #ifndef CORRADE_TARGET_APPLE
        /* Sorry about the UGLY code, HOPEFULLY THERE WON'T BE MORE WORKAROUNDS */
        || (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        (vendorString == "NVIDIA Corporation"_s ||
         #ifdef CORRADE_TARGET_WINDOWS
         vendorString == "Intel"_s ||
         #endif
         vendorString == "ATI Technologies Inc."_s)
        && !_context->isDriverWorkaroundDisabled("no-forward-compatible-core-context"_s))
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
        /* Discard the ForwardCompatible flag for the fallback. Having it set
           makes the fallback context creation fail on Mesa's Zink (which is
           just 2.1) and I assume on others as well. */
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);

        /* StringViewFlag::NullTerminated asserted above already, internally
           it's stored as a Containers::String so it should be always */
        _window = glfwCreateWindow(scaledWindowSize.x(), scaledWindowSize.y(), configuration.title().data(), monitor, nullptr);
    }
    #endif

    if(!_window) {
        Error() << "Platform::GlfwApplication::tryCreate(): cannot create a window with OpenGL context";
        return false;
    }

    /* Proceed with configuring other stuff that couldn't be done with window
       hints */
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    glfwSetInputMode(_window, GLFW_CURSOR, Int(configuration.cursorMode()));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /* If exit() was called before the window got created, be sure to propagate
       it */
    glfwSetWindowShouldClose(_window, !!(_flags & Flag::Exit));

    /* Make the final context current */
    glfwMakeContextCurrent(_window);

    /* Destroy everything when the Magnum context creation fails */
    if(!_context->tryCreate(glConfiguration)) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }

    /* Show the window once we are sure that everything is okay. This
       apparently causes a minimized window to get restored, so perform the
       minimization only after this step, not before. */
    if(!(configuration.windowFlags() & WindowFlag::Hidden))
        glfwShowWindow(_window);
    if(configuration.windowFlags() >= WindowFlag::Minimized)
        glfwIconifyWindow(_window);

    /* Return true if the initialization succeeds */
    return true;
}
#endif

namespace {

GlfwApplication::Pointer buttonToPointer(const int button) {
    switch(button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            return GlfwApplication::Pointer::MouseLeft;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return GlfwApplication::Pointer::MouseMiddle;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return GlfwApplication::Pointer::MouseRight;
        case GLFW_MOUSE_BUTTON_4:
            return GlfwApplication::Pointer::MouseButton4;
        case GLFW_MOUSE_BUTTON_5:
            return GlfwApplication::Pointer::MouseButton5;
        case GLFW_MOUSE_BUTTON_6:
            return GlfwApplication::Pointer::MouseButton6;
        case GLFW_MOUSE_BUTTON_7:
            return GlfwApplication::Pointer::MouseButton7;
        case GLFW_MOUSE_BUTTON_8:
            return GlfwApplication::Pointer::MouseButton8;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}

GlfwApplication::Pointers currentGlfwPointers(GLFWwindow* const window) {
    GlfwApplication::Pointers pointers;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseLeft;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseMiddle;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseRight;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseButton4;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseButton5;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_6) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseButton6;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_7) == GLFW_PRESS)
        pointers |= GlfwApplication::Pointer::MouseButton7;
    return pointers;
}

}

void GlfwApplication::setupCallbacks() {
    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowCloseCallback(_window, [](GLFWwindow* const window){
        ExitEvent e;
        static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window))->exitEvent(e);
        if(!e.isAccepted())
            glfwSetWindowShouldClose(window, false);
    });
    glfwSetWindowRefreshCallback(_window, [](GLFWwindow* const window){
        /* Properly redraw after the window is restored from minimized state */
        static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window))->drawEvent();
    });
    #ifdef MAGNUM_TARGET_GL
    glfwSetFramebufferSizeCallback
    #else
    glfwSetWindowSizeCallback
    #endif
    (_window, [](GLFWwindow* const window, const int w, const int h) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));
        /* Refresh the DPI scaling value, as it might have changed as a side
           effect of the window size change. Suppress the log output as any
           warnings got likely already printed during startup. */
        app._dpiScaling = app.dpiScalingInternal(app._configurationDpiScalingPolicy, app._configurationDpiScaling, /*silentLog*/ true);
        #ifdef MAGNUM_TARGET_GL
        ViewportEvent e{app.windowSize(), {w, h}, app._dpiScaling};
        #else
        ViewportEvent e{{w, h}, app._dpiScaling};
        #endif
        app.viewportEvent(e);
    });
    glfwSetWindowFocusCallback(_window, [](GLFWwindow* const window, const int focused) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        FocusEvent e;
        focused ? app.focusEvent(e) : app.blurEvent(e);
    });
    glfwSetKeyCallback(_window, [](GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        KeyEvent e(Key(key), scancode, Modifiers{mods}, action == GLFW_REPEAT);

        if(action == GLFW_PRESS || action == GLFW_REPEAT)
            app.keyPressEvent(e);
        else if(action == GLFW_RELEASE)
            app.keyReleaseEvent(e);
    });
    glfwSetMouseButtonCallback(_window, [](GLFWwindow* const window, const int button, const int action, const int mods) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        const Pointer pointer = buttonToPointer(button);
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        const Vector2 position{Float(x), Float(y)};

        /* If an additional mouse button was pressed or some buttons are still
           left pressed after a release, call a move event instead */
        const Pointers pointers = currentGlfwPointers(window);
        if((action == GLFW_PRESS && (pointers & ~pointer)) ||
           (action == GLFW_RELEASE && pointers)) {
            PointerMoveEvent e{window, pointer, position, {}};
            /* We had to query the pointers already and get the modifiers in
               the callback, set them directly instead of having them lazily
               populated later */
            e._pointers = pointers;
            e._modifiers = Modifiers{mods};
            app.pointerMoveEvent(e);
        } else {
            PointerEvent e{pointer, position, Modifiers{mods}};
            if(action == GLFW_PRESS) /* we don't handle GLFW_REPEAT */
                app.pointerPressEvent(e);
            else if(action == GLFW_RELEASE)
                app.pointerReleaseEvent(e);
        }
    });
    glfwSetCursorPosCallback(_window, [](GLFWwindow* const window, const double x, const double y) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));
        /* Avoid bogus offset at first -- report 0 when the event is called for
           the first time */
        const Vector2 position{Float(x), Float(y)};
        PointerMoveEvent e{window, {}, position,
            Math::isNan(app._previousMouseMovePosition).all() ? Vector2{} :
            position - app._previousMouseMovePosition};
        app._previousMouseMovePosition = position;
        app.pointerMoveEvent(e);
    });
    glfwSetScrollCallback(_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ScrollEvent e{window, {Float(xoffset), Float(yoffset)}};
        static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window))->scrollEvent(e);
    });
    glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int codepoint) {
        auto& app = *static_cast<GlfwApplication*>(glfwGetWindowUserPointer(window));

        if(!(app._flags & Flag::TextInputActive))
            return;

        /* One extra byte to ensure it gets always null-terminated */
        char utf8[4 + 1]{};
        const std::size_t size = Utility::Unicode::utf8(codepoint, Containers::staticArrayView(utf8).prefix<4>());
        TextInputEvent e{{utf8, size, Containers::StringViewFlag::NullTerminated}};
        app.textInputEvent(e);
    });
}

GlfwApplication::~GlfwApplication() {
    #ifdef MAGNUM_TARGET_GL
    /* Destroy Magnum context first to avoid it potentially accessing the
       now-destroyed GL context after */
    _context = Containers::NullOpt;
    #endif

    glfwDestroyWindow(_window);
    for(auto& cursor: _cursors)
        glfwDestroyCursor(cursor);
    glfwTerminate();
}

Containers::StringView GlfwApplication::keyName(const Key key, const UnsignedInt scancode) const {
    return glfwGetKeyName(int(key), scancode);
}

#if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
Containers::Optional<UnsignedInt> GlfwApplication::keyToScanCode(const Key key) const {
    const int scancode = glfwGetKeyScancode(int(key));
    if(scancode == -1)
        return {};
    return UnsignedInt(scancode);
}
#endif

Vector2i GlfwApplication::windowSize() const {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::windowSize(): no window opened", {});

    Vector2i size;
    glfwGetWindowSize(_window, &size.x(), &size.y());
    return size;
}

void GlfwApplication::setWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::setWindowSize(): no window opened", );

    const Vector2i newSize = _dpiScaling*size;
    glfwSetWindowSize(_window, newSize.x(), newSize.y());
}

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

#ifdef MAGNUM_TARGET_GL
Vector2i GlfwApplication::framebufferSize() const {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::framebufferSize(): no window opened", {});

    Vector2i size;
    glfwGetFramebufferSize(_window, &size.x(), &size.y());
    return size;
}

void GlfwApplication::setSwapInterval(const Int interval) {
    glfwSwapInterval(interval);

    /* Remember whether VSync is enabled for mainLoopIteration() to use
       minimal loop period or not. Unlike SDL2 where it's possible to check
       whether the VSync was actually set, here it's purely hope-based.
       Sorry. */
    if(interval)
        _flags |= Flag::VSyncEnabled;
    else _flags &= ~Flag::VSyncEnabled;
}
#endif

void GlfwApplication::setMinimalLoopPeriod(const Nanoseconds time) {
    CORRADE_ASSERT(time >= 0_nsec,
        "Platform::Sdl2Application::setMinimalLoopPeriod(): expected non-negative time, got" << time, );
    _minimalLoopPeriodNanoseconds = Long(time);
}

void GlfwApplication::redraw() { _flags |= Flag::Redraw; }

int GlfwApplication::exec() {
    while(mainLoopIteration()) {}

    return _exitCode;
}

bool GlfwApplication::mainLoopIteration() {
    /* If exit was requested directly in the constructor, exit immediately
       without calling anything else */
    if(_flags & Flag::Exit || glfwWindowShouldClose(_window))
        return false;

    CORRADE_ASSERT(_window, "Platform::GlfwApplication::mainLoopIteration(): no window opened", {});

    /*
        If callbacks are not set up yet, do that. Can't be done inside
        tryCreate() because:

        1.  On Windows, GLFW fires a viewport event already when creating the
            window, which means viewportEvent() gets called even before the
            constructor exits. That's not a problem if the window is created
            implicitly (because derived class vtable is not setup yet and so
            the call goes into the base class no-op viewportEvent()), but when
            calling create() / tryCreate() from user constructor, this might
            lead to crashes as things touched by viewportEvent() might not be
            initialized yet.
        2.  On macOS, GLFW might sometimes (hard to reproduce) trigger a draw
            event when creating the window. That's even worse than on Windows
            because this leads to pure virtual drawEvent() getting called and
            the application aborting due to a pure virtual method call in case
            GL context is created implicitly by the base class constructor (at
            which point the vtable pointers for the derived class are not set
            up yet).
    */
    if(glfwGetWindowUserPointer(_window) != this)
        setupCallbacks();

    const Nanoseconds timeBefore = _minimalLoopPeriodNanoseconds ? glfwGetTime()*1.0_sec : Nanoseconds{};

    glfwPollEvents();

    /* Tick event */
    if(!(_flags & Flag::NoTickEvent))
        tickEvent();

    /* Draw event */
    if(_flags & Flag::Redraw) {
        _flags &= ~Flag::Redraw;
        drawEvent();

        /* If VSync is not enabled, delay to prevent CPU hogging (if set) */
        if(!(_flags & Flag::VSyncEnabled) && _minimalLoopPeriodNanoseconds) {
            const Nanoseconds loopTime = glfwGetTime()*1.0_sec - timeBefore;
            if(loopTime < _minimalLoopPeriodNanoseconds*1_nsec)
                Utility::System::sleep((_minimalLoopPeriodNanoseconds*1_nsec - loopTime)/1.0_msec);
        }

        return !(_flags & Flag::Exit || glfwWindowShouldClose(_window));
    }

    /* If not drawing anything, delay to prevent CPU hogging (if set) */
    if(_minimalLoopPeriodNanoseconds) {
        const Nanoseconds loopTime = glfwGetTime()*1.0_sec - timeBefore;
        if(loopTime < _minimalLoopPeriodNanoseconds*1_nsec)
            Utility::System::sleep((_minimalLoopPeriodNanoseconds*1_nsec - loopTime)/1.0_msec);
    }

    /* Then, if the tick event doesn't need to be called periodically, wait
       indefinitely for next input event */
    if(_flags & Flag::NoTickEvent)
        glfwWaitEvents();
    return !(_flags & Flag::Exit || glfwWindowShouldClose(_window));
}

void GlfwApplication::exit(int exitCode) {
    _flags |= Flag::Exit;
    _exitCode = exitCode;

    /* If the window is already created, tell GLFW that it should close. If
       not, this is done in tryCreate() once the window is created */
    if(_window)
        glfwSetWindowShouldClose(_window, true);
}

bool GlfwApplication::isKeyPressed(const Key key) {
    /* Documentation says GLFW_KEY_UNKNOWN is not valid for glfwGetKey() */
    if(key == Key::Unknown)
        return false;
    return glfwGetKey(_window, int(key)) == GLFW_PRESS;
}

namespace {

constexpr Int CursorMap[] {
    GLFW_ARROW_CURSOR,
    GLFW_IBEAM_CURSOR,
    GLFW_CROSSHAIR_CURSOR,
    #ifdef GLFW_RESIZE_NWSE_CURSOR
    GLFW_RESIZE_NWSE_CURSOR,
    GLFW_RESIZE_NESW_CURSOR,
    #endif
    GLFW_HRESIZE_CURSOR,
    GLFW_VRESIZE_CURSOR,
    #ifdef GLFW_RESIZE_NWSE_CURSOR
    GLFW_RESIZE_ALL_CURSOR,
    GLFW_NOT_ALLOWED_CURSOR,
    #endif
    GLFW_HAND_CURSOR
};

}

void GlfwApplication::setCursor(Cursor cursor) {
    CORRADE_ASSERT(_window, "Platform::GlfwApplication::setCursor(): no window opened", );

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

    /* The second condition could be a static assert but it doesn't let me
       because "this pointer only accessible in a constexpr function". Thanks
       for nothing, C++. */
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(_cursors) && Containers::arraySize(_cursors) == Containers::arraySize(CursorMap));

    if(!_cursors[UnsignedInt(cursor)])
        _cursors[UnsignedInt(cursor)] = glfwCreateStandardCursor(CursorMap[UnsignedInt(cursor)]);

    glfwSetCursor(_window, _cursors[UnsignedInt(cursor)]);
}

GlfwApplication::Cursor GlfwApplication::cursor() {
    return _cursor;
}

void GlfwApplication::exitEvent(ExitEvent& event) {
    event.setAccepted();
}

void GlfwApplication::tickEvent() {
    /* If this got called, the tick event is not implemented by user and thus
       we don't need to call it ever again */
    _flags |= Flag::NoTickEvent;
}

void GlfwApplication::viewportEvent(ViewportEvent&) {}
void GlfwApplication::focusEvent(FocusEvent&) {}
void GlfwApplication::blurEvent(FocusEvent&) {}
void GlfwApplication::keyPressEvent(KeyEvent&) {}
void GlfwApplication::keyReleaseEvent(KeyEvent&) {}

#ifdef MAGNUM_BUILD_DEPRECATED
namespace {

CORRADE_IGNORE_DEPRECATED_PUSH
GlfwApplication::MouseEvent::Button pointerToButton(const GlfwApplication::Pointer pointer) {
    switch(pointer) {
        case GlfwApplication::Pointer::MouseLeft:
            return GlfwApplication::MouseEvent::Button::Left;
        case GlfwApplication::Pointer::MouseMiddle:
            return GlfwApplication::MouseEvent::Button::Middle;
        case GlfwApplication::Pointer::MouseRight:
            return GlfwApplication::MouseEvent::Button::Right;
        case GlfwApplication::Pointer::MouseButton4:
            return GlfwApplication::MouseEvent::Button::Button4;
        case GlfwApplication::Pointer::MouseButton5:
            return GlfwApplication::MouseEvent::Button::Button5;
        case GlfwApplication::Pointer::MouseButton6:
            return GlfwApplication::MouseEvent::Button::Button6;
        case GlfwApplication::Pointer::MouseButton7:
            return GlfwApplication::MouseEvent::Button::Button7;
        case GlfwApplication::Pointer::MouseButton8:
            return GlfwApplication::MouseEvent::Button::Button8;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}
CORRADE_IGNORE_DEPRECATED_POP

}
#endif

void GlfwApplication::pointerPressEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseEvent mouseEvent{pointerToButton(event.pointer()), Vector2i{Math::round(event.position())}, event.modifiers()};
    mousePressEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void GlfwApplication::mousePressEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void GlfwApplication::pointerReleaseEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseEvent mouseEvent{pointerToButton(event.pointer()), Vector2i{Math::round(event.position())}, event.modifiers()};
    mouseReleaseEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void GlfwApplication::mouseReleaseEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void GlfwApplication::pointerMoveEvent(PointerMoveEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    const Vector2i roundedPosition{Math::round(event.position())};

    /* If the event is due to some button being additionally pressed or one
       button from a larger set being released, delegate to a press/release
       event instead */
    if(event.pointer()) {
        /* GLFW reports either a move or a press/release, so there shouldn't be
           any move in this case */
        CORRADE_INTERNAL_ASSERT(event.relativePosition() == Vector2{});
        MouseEvent mouseEvent{pointerToButton(*event.pointer()),
            roundedPosition, event.modifiers()};
        event.pointers() >= *event.pointer() ?
            mousePressEvent(mouseEvent) : mouseReleaseEvent(mouseEvent);
    } else {
        /* Can't do just Math::round(event.relativePosition()) because if the
           previous position was 4.6 and the new 5.3, they both round to 5 but
           the relativePosition is 0.6 and rounds to 1. Conversely, if it'd be
           5.3 and 5.6, the positions round to 5 and 6 but relative position
           stays 0. */
        const Vector2i previousRoundedPosition{Math::round(event.position() - event.relativePosition())};
        /* Call the event only if the integer values actually changed */
        if(roundedPosition != previousRoundedPosition) {
            MouseMoveEvent mouseEvent{_window, roundedPosition, roundedPosition - previousRoundedPosition};
            mouseMoveEvent(mouseEvent);
        }
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void GlfwApplication::mouseMoveEvent(MouseMoveEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void GlfwApplication::scrollEvent(ScrollEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseScrollEvent mouseEvent{_window, event.offset()};
    mouseScrollEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void GlfwApplication::mouseScrollEvent(MouseScrollEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void GlfwApplication::textInputEvent(TextInputEvent&) {}

bool GlfwApplication::isTextInputActive() const {
    return !!(_flags & Flag::TextInputActive);
}

void GlfwApplication::startTextInput() {
    _flags |= Flag::TextInputActive;
}

void GlfwApplication::stopTextInput() {
    _flags &= ~Flag::TextInputActive;
}

Containers::StringView GlfwApplication::clipboardText() {
    return glfwGetClipboardString(_window);
}

void GlfwApplication::setClipboardText(const Containers::StringView text) {
    glfwSetClipboardString(_window, Containers::String::nullTerminatedView(text).data());
}

#ifdef MAGNUM_TARGET_GL
GlfwApplication::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 8}, _depthBufferSize{24}, _stencilBufferSize{0},
    _sampleCount{0}, _version{GL::Version::None}, _srgbCapable{false}
{
    #ifndef MAGNUM_TARGET_GLES
    addFlags(Flag::ForwardCompatible);
    #endif
}
#endif

GlfwApplication::Configuration::Configuration():
    _title{Containers::String::nullTerminatedGlobalView("Magnum GLFW Application"_s)},
    _size{800, 600},
    _dpiScalingPolicy{DpiScalingPolicy::Default} {}

GlfwApplication::Configuration::~Configuration() = default;

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::StringView GlfwApplication::KeyEvent::keyName(const GlfwApplication::Key key) {
    return glfwGetKeyName(int(key), 0);
}
#endif

Containers::StringView GlfwApplication::KeyEvent::keyName() const {
    return glfwGetKeyName(Int(_key), _scancode);
}

GlfwApplication::Pointers GlfwApplication::PointerMoveEvent::pointers() {
    if(!_pointers)
        _pointers = currentGlfwPointers(_window);

    return *_pointers;
}

namespace {

GlfwApplication::Modifiers currentGlfwModifiers(GLFWwindow* window) {
    static_assert(GLFW_PRESS == true && GLFW_RELEASE == false,
        "GLFW press and release constants do not correspond to bool values");

    GlfwApplication::Modifiers mods;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
        mods |= GlfwApplication::Modifier::Shift;
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
        mods |= GlfwApplication::Modifier::Ctrl;
    if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
        mods |= GlfwApplication::Modifier::Alt;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SUPER) ||
       glfwGetKey(window, GLFW_KEY_RIGHT_SUPER))
        mods |= GlfwApplication::Modifier::Super;

    return mods;
}

}

GlfwApplication::Modifiers GlfwApplication::PointerMoveEvent::modifiers() {
    if(!_modifiers)
        _modifiers = currentGlfwModifiers(_window);
    return *_modifiers;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
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

GlfwApplication::Modifiers GlfwApplication::MouseMoveEvent::modifiers() {
    if(!_modifiers)
        _modifiers = currentGlfwModifiers(_window);
    return *_modifiers;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

GlfwApplication::Modifiers GlfwApplication::ScrollEvent::modifiers() {
    if(!_modifiers)
        _modifiers = currentGlfwModifiers(_window);
    return *_modifiers;
}
Vector2 GlfwApplication::ScrollEvent::position() {
    if(!_position) {
        Vector2d position;
        glfwGetCursorPos(_window, &position.x(), &position.y());
        _position = Vector2{position};
    }
    return *_position;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Vector2i GlfwApplication::MouseScrollEvent::position() {
    if(!_position) {
        Vector2d position;
        glfwGetCursorPos(_window, &position.x(), &position.y());
        _position = Vector2i{position};
    }

    return *_position;
}

GlfwApplication::Modifiers GlfwApplication::MouseScrollEvent::modifiers() {
    if(!_modifiers)
        _modifiers = currentGlfwModifiers(_window);
    return *_modifiers;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

/* On MSVC 2017, deprecation warning suppression doesn't work on virtual
   function overrides, so ScreenedApplication overriding mousePressEvent(),
   mouseReleaseEvent() mouseMoveEvent() and mouseScrollEvent() causes warnings.
   Disable them at a higher level instead. */
#if defined(MAGNUM_BUILD_DEPRECATED) && defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG) && _MSC_VER < 1920
CORRADE_IGNORE_DEPRECATED_PUSH
#endif
template class BasicScreen<GlfwApplication>;
template class BasicScreenedApplication<GlfwApplication>;
#if defined(MAGNUM_BUILD_DEPRECATED) && defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG) && _MSC_VER < 1920
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}
