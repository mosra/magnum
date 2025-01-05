#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018, 2019 Jonathan Hale <squareys@googlemail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/Platform")

# Code shared by more application/context implementations
set(MagnumPlatform_SRCS )

set(MagnumPlatform_HEADERS
    Gesture.h
    Platform.h
    Screen.h
    ScreenedApplication.h
    ScreenedApplication.hpp)

set(MagnumPlatform_PRIVATE_HEADERS )

# DPI scaling queries and related stuff for EmscriptenApplication,
# Sdl2Application and GlfwApplication
if(MAGNUM_WITH_EMSCRIPTENAPPLICATION OR MAGNUM_WITH_GLFWAPPLICATION OR MAGNUM_WITH_SDL2APPLICATION)
    # List of libraries to link when using the MagnumPlatformObjects target
    # TODO: use target_link_libraries() when we are on a CMake version that
    # supports it (3.12?)
    set(MagnumPlatform_LINK_LIBRARIES )
    set(MagnumPlatform_COMPILE_DEFINITIONS )

    list(APPEND MagnumPlatform_PRIVATE_HEADERS Implementation/DpiScaling.h)
    if(CORRADE_TARGET_APPLE)
        # We can't build both DpiScaling.cpp and DpiScaling.mm as they both
        # result in DpiScaling.o and Xcode/CMake gets confused, so including
        # the cpp from the mm instead
        list(APPEND MagnumPlatform_SRCS Implementation/DpiScaling.mm)
    else()
        list(APPEND MagnumPlatform_SRCS Implementation/DpiScaling.cpp)
    endif()

    add_library(MagnumPlatformObjects OBJECT
        ${MagnumPlatform_SRCS}
        ${MagnumPlatform_HEADERS}
        ${MagnumPlatform_PRIVATE_HEADERS})
    target_include_directories(MagnumPlatformObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumPlatformObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Use the Foundation framework on Apple to query the DPI awareness
    if(CORRADE_TARGET_APPLE)
        find_library(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY Foundation)
        find_path(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_INCLUDE_DIR NAMES NSBundle.h)
        mark_as_advanced(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY
            _MAGNUM_APPLE_FOUNDATION_FRAMEWORK_INCLUDE_DIR)
        list(APPEND MagnumPlatform_LINK_LIBRARIES ${_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY})
        target_include_directories(MagnumPlatformObjects PRIVATE ${_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_INCLUDE_DIR})

    # If there is X11, ask it for DPI
    elseif(CORRADE_TARGET_UNIX)
        find_package(X11)
        if(X11_FOUND)
            # Not linking to X11, we dlopen() instead
            target_include_directories(MagnumPlatformObjects PRIVATE ${X11_X11_INCLUDE_PATH})
            target_compile_definitions(MagnumPlatformObjects PUBLIC "_MAGNUM_PLATFORM_USE_X11")
            list(APPEND MagnumPlatform_LINK_LIBRARIES ${CMAKE_DL_LIBS})
            list(APPEND MagnumPlatform_COMPILE_DEFINITIONS "_MAGNUM_PLATFORM_USE_X11")
        endif()
    endif()
endif()

# Files to display in project view of IDEs only (filled in below)
set(MagnumPlatform_FILES )

if(MAGNUM_TARGET_GL)
    list(APPEND MagnumPlatform_HEADERS GLContext.h)

    # Decide about platform-specific context for cross-platform toolkits
    if(MAGNUM_WITH_GLFWAPPLICATION OR MAGNUM_WITH_SDL2APPLICATION)
        # On WebGL windowless apps don't use anything from EGL, only windowless
        # do. On iOS it's EAGL, which isn't really EGL (and thus
        # MAGNUM_TARGET_EGL isn't set), but we handle it inside EglContext as
        # well. TODO make EaglContext for iOS to prepare for true EGL (such as
        # with ANGLE or Zink)
        if((MAGNUM_TARGET_EGL AND NOT MAGNUM_TARGET_WEBGL) OR CORRADE_TARGET_IOS)
            set(NEED_EGLCONTEXT 1)
            set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumEglContextObjects>)
            find_package(EGL REQUIRED)
            set(MagnumSomeContext_LIBRARY EGL::EGL)
        elseif(CORRADE_TARGET_APPLE)
            set(NEED_CGLCONTEXT 1)
            set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumCglContextObjects>)
        elseif(CORRADE_TARGET_WINDOWS)
            set(NEED_WGLCONTEXT 1)
            set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumWglContextObjects>)
        elseif(CORRADE_TARGET_UNIX)
            set(NEED_GLXCONTEXT 1)
            set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumGlxContextObjects>)
        endif()

        # If the GLVND library (CMake 3.11+) was found and linked to, we need
        # to link to GLX explicitly. Otherwise (and also on all systems except
        # Linux) the transitive dependency to classic GL lib from MagnumGL is
        # enough. Can't use OpenGL_OpenGL_FOUND, because that one is set also
        # if GLVND is *not* found. WTF. Also can't just check for
        # OPENGL_opengl_LIBRARY because that's set even if OpenGL_GL_PREFERENCE
        # is explicitly set to LEGACY.
        if(CORRADE_TARGET_UNIX AND NOT MAGNUM_TARGET_EGL AND OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
            set(MagnumSomeContext_LIBRARY OpenGL::GLX)
        endif()
    endif()
endif()

install(FILES ${MagnumPlatform_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)

# Android application
if(MAGNUM_WITH_ANDROIDAPPLICATION)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL Android)
        message(FATAL_ERROR "AndroidApplication is available only when targeting Android. Set MAGNUM_WITH_ANDROIDAPPLICATION to OFF to skip building it.")
    endif()

    find_package(EGL REQUIRED)
    set(NEED_EGLCONTEXT 1)

    set(MagnumAndroidApplication_SRCS
        AndroidApplication.cpp
        Implementation/Egl.cpp
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumAndroidApplication_HEADERS
        AndroidApplication.h)
    set(MagnumAndroidApplication_PRIVATE_HEADERS
        Implementation/Egl.h)

    enable_language(C)
    add_library(MagnumAndroidApplicationGlue OBJECT
        "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    set_target_properties(MagnumAndroidApplicationGlue PROPERTIES
        CORRADE_USE_PEDANTIC_FLAGS OFF)

    add_library(MagnumAndroidApplication STATIC
        ${MagnumAndroidApplication_SRCS}
        ${MagnumAndroidApplication_HEADERS}
        ${MagnumAndroidApplication_PRIVATE_HEADERS}
        $<TARGET_OBJECTS:MagnumAndroidApplicationGlue>)
    target_include_directories(MagnumAndroidApplication PUBLIC
        "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/")
    set_target_properties(MagnumAndroidApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_link_libraries(MagnumAndroidApplication PUBLIC
        MagnumGL
        android
        EGL::EGL)

    install(FILES ${MagnumAndroidApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumAndroidApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum AndroidApplication target alias for superprojects
    add_library(Magnum::AndroidApplication ALIAS MagnumAndroidApplication)
endif()

# JavaScript and CSS stuff for Emscripten. Installed unconditionally so it can
# be also used by people who hate the Application classes
if(CORRADE_TARGET_EMSCRIPTEN)
    set(MagnumEmscriptenApplication_FILES
        EmscriptenApplication.js
        WindowlessEmscriptenApplication.js
        WebApplication.css)

    list(APPEND MagnumPlatform_FILES ${MagnumEmscriptenApplication_FILES})
    install(FILES ${MagnumEmscriptenApplication_FILES} DESTINATION ${MAGNUM_DATA_INSTALL_DIR})
endif()

# Emscripten-specific nightmares for EmscriptenApplication and Sdl2Application
if(CORRADE_TARGET_EMSCRIPTEN AND (MAGNUM_WITH_EMSCRIPTENAPPLICATION OR MAGNUM_WITH_SDL2APPLICATION))
    # This could also set -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
    # always and require only 1.38.27 (where this option got added), but that
    # version is from February 2019 and I don't think anyone actively uses it
    # anymore.
    if(EMSCRIPTEN_VERSION VERSION_LESS 1.39.5)
        message(SEND_ERROR "Emscripten 1.39.5+, which enables DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR by default, is required to build EmscriptenApplication and Sdl2Application. Found version ${EMSCRIPTEN_VERSION}.")
    endif()

    # Patch the Platform.js file to contain only __deps that match given
    # Emscripten version. No, there's no known better way. See the comment in
    # the file itself for details.
    if(EMSCRIPTEN_VERSION VERSION_LESS 3.1.35)
        set(MagnumPlatform_EMSCRIPTEN_3135_ONLY "//") # Haha
    else()
        set(MagnumPlatform_EMSCRIPTEN_3135_ONLY "/* Emscripten 3.1.35+ */")
    endif()
    if(EMSCRIPTEN_VERSION VERSION_LESS 2.0.10)
        set(MagnumPlatform_EMSCRIPTEN_2010_ONLY "//") # Haha
    else()
        set(MagnumPlatform_EMSCRIPTEN_2010_ONLY "/* Emscripten 2.0.10+ */")
    endif()
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Platform.js.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Platform.js)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Platform.js DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR} RENAME MagnumPlatform.js)
endif()

# Emscripten application
if(MAGNUM_WITH_EMSCRIPTENAPPLICATION)
    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        message(FATAL_ERROR "EmscriptenApplication is available only when targeting Emscripten. Set MAGNUM_WITH_EMSCRIPTENAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumEmscriptenApplication_SRCS
        $<TARGET_OBJECTS:MagnumPlatformObjects>
        EmscriptenApplication.cpp)
    set(MagnumEmscriptenApplication_HEADERS
        EmscriptenApplication.h)

    add_library(MagnumEmscriptenApplication STATIC
        ${MagnumEmscriptenApplication_SRCS}
        ${MagnumEmscriptenApplication_HEADERS}
        ${MagnumEmscriptenApplication_PRIVATE_HEADERS})
    set_target_properties(MagnumEmscriptenApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    # TODO: use MagnumPlatformObjects instead of ${MagnumPlatform_*} when
    # CMake supports it
    target_link_libraries(MagnumEmscriptenApplication PUBLIC Magnum
        ${MagnumPlatform_LINK_LIBRARIES})
    target_compile_definitions(MagnumEmscriptenApplication PRIVATE
        ${MagnumPlatform_COMPILE_DEFINITIONS})
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumEmscriptenApplication PUBLIC MagnumGL)
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        # TODO switch to target_link_options() and SHELL: once we require CMake
        #   3.13 unconditionally
        target_link_libraries(MagnumEmscriptenApplication PUBLIC "--js-library ${CMAKE_CURRENT_BINARY_DIR}/Platform.js")
        # TODO this adds the dependency only on 3.13+ and only for Make/Ninja,
        #   any better option?
        set_property(TARGET MagnumEmscriptenApplication APPEND PROPERTY INTERFACE_LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Platform.js)
    endif()

    install(FILES ${MagnumEmscriptenApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumEmscriptenApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum EmscriptenApplication target alias for superprojects
    add_library(Magnum::EmscriptenApplication ALIAS MagnumEmscriptenApplication)
endif()

# GLFW application
if(MAGNUM_WITH_GLFWAPPLICATION)
    find_package(GLFW)
    if(NOT GLFW_FOUND)
        message(FATAL_ERROR "GLFW library, required by GlfwApplication, was not found. Set MAGNUM_WITH_GLFWAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumGlfwApplication_SRCS
        $<TARGET_OBJECTS:MagnumPlatformObjects>
        GlfwApplication.cpp)
    set(MagnumGlfwApplication_HEADERS GlfwApplication.h)
    if(MAGNUM_TARGET_GL)
        list(APPEND MagnumGlfwApplication_SRCS ${MagnumSomeContext_OBJECTS})
    endif()

    add_library(MagnumGlfwApplication STATIC
        ${MagnumGlfwApplication_SRCS}
        ${MagnumGlfwApplication_HEADERS})
    set_target_properties(MagnumGlfwApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumGlfwApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    # TODO: use MagnumPlatformObjects instead of ${MagnumPlatform_*} when
    # CMake supports it
    target_link_libraries(MagnumGlfwApplication PUBLIC Magnum GLFW::GLFW
        ${MagnumPlatform_LINK_LIBRARIES})
    target_compile_definitions(MagnumGlfwApplication PRIVATE
        ${MagnumPlatform_COMPILE_DEFINITIONS})
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumGlfwApplication PUBLIC
            MagnumGL
            ${MagnumSomeContext_LIBRARY})
    endif()

    install(FILES ${MagnumGlfwApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlfwApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Copy the GLFW DLL next to the place where all executables are stored to
    # help people running the apps
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CORRADE_TARGET_WINDOWS AND (GLFW_DLL_DEBUG OR GLFW_DLL_RELEASE))
        if(GLFW_DLL_DEBUG AND GLFW_DLL_RELEASE)
            set(glfw_dll_input $<$<CONFIG:Debug>:${GLFW_DLL_DEBUG}>$<$<NOT:$<CONFIG:Debug>>:${GLFW_DLL_RELEASE}>)
        elseif(GLFW_DLL_DEBUG)
            set(glfw_dll_input ${GLFW_DLL_DEBUG})
        else()
            set(glfw_dll_input ${GLFW_DLL_RELEASE})
        endif()
        add_custom_command(TARGET MagnumGlfwApplication POST_BUILD
            # The directory might not always exist before (e.g. if creating a
            # static build) and copy_if_different would just name the file
            # `bin` instead of creating such directory
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${glfw_dll_input} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()

    # Magnum GlfwApplication target alias for superprojects
    add_library(Magnum::GlfwApplication ALIAS MagnumGlfwApplication)
endif()

# SDL2 application
if(MAGNUM_WITH_SDL2APPLICATION)
    find_package(SDL2)
    if(NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 library, required by Sdl2Application, was not found. Set MAGNUM_WITH_SDL2APPLICATION to OFF to skip building it.")
    endif()

    set(MagnumSdl2Application_SRCS
        $<TARGET_OBJECTS:MagnumPlatformObjects>
        Sdl2Application.cpp)
    set(MagnumSdl2Application_HEADERS Sdl2Application.h)
    if(MAGNUM_TARGET_GL)
        list(APPEND MagnumSdl2Application_SRCS ${MagnumSomeContext_OBJECTS})
    endif()

    add_library(MagnumSdl2Application STATIC
        ${MagnumSdl2Application_SRCS}
        ${MagnumSdl2Application_HEADERS})
    set_target_properties(MagnumSdl2Application PROPERTIES
        DEBUG_POSTFIX "-d")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumSdl2Application PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    # TODO: use MagnumPlatformObjects instead of ${MagnumPlatform_*} when
    # CMake supports it
    target_link_libraries(MagnumSdl2Application PUBLIC Magnum SDL2::SDL2
        ${MagnumPlatform_LINK_LIBRARIES})
    target_compile_definitions(MagnumSdl2Application PRIVATE
        ${MagnumPlatform_COMPILE_DEFINITIONS})
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumSdl2Application PUBLIC
            MagnumGL
            ${MagnumSomeContext_LIBRARY})
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        # TODO switch to target_link_options() and SHELL: once we require CMake
        #   3.13 unconditionally
        target_link_libraries(MagnumSdl2Application PUBLIC "--js-library ${CMAKE_CURRENT_BINARY_DIR}/Platform.js")
        # TODO this adds the dependency only on 3.13+ and only for Make/Ninja,
        #   any better option?
        set_property(TARGET MagnumSdl2Application APPEND PROPERTY INTERFACE_LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Platform.js)
    endif()

    install(FILES ${MagnumSdl2Application_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumSdl2Application
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Copy the SDL2 DLL next to the place where all executables are stored to
    # help people running the apps
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CORRADE_TARGET_WINDOWS AND (SDL2_DLL_DEBUG OR SDL2_DLL_RELEASE))
        if(SDL2_DLL_DEBUG AND SDL2_DLL_RELEASE)
            set(sdl2_dll_input $<$<CONFIG:Debug>:${SDL2_DLL_DEBUG}>$<$<NOT:$<CONFIG:Debug>>:${SDL2_DLL_RELEASE}>)
        elseif(SDL2_DLL_DEBUG)
            set(sdl2_dll_input ${SDL2_DLL_DEBUG})
        else()
            set(sdl2_dll_input ${SDL2_DLL_RELEASE})
        endif()
        add_custom_command(TARGET MagnumSdl2Application POST_BUILD
            # The directory might not always exist before (e.g. if creating a
            # static build) and copy_if_different would just name the file
            # `bin` instead of creating such directory
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${sdl2_dll_input} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()

    # Magnum Sdl2Application target alias for superprojects
    add_library(Magnum::Sdl2Application ALIAS MagnumSdl2Application)
endif()

# GLX application
if(MAGNUM_WITH_GLXAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "GlxApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_GLXCONTEXTHANDLER 1)
    set(NEED_GLXCONTEXT 1)

    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some applications, was not found. Set MAGNUM_WITH_*X*APPLICATION to OFF to skip building them.")
    endif()

    # If we're targeting GLES and EGL, only the libGLES library is looked for
    # by default. Here we need libGLX from GLVND in addition. If GLVND is not
    # available however (or CMake < 3.10 is used), fail, as GLX symbols are
    # only in libGL in that case, which would conflict with libGLES.
    if(MAGNUM_TARGET_GLES AND MAGNUM_TARGET_EGL)
        find_package(OpenGL)
        if(NOT OpenGL_FOUND OR NOT OPENGL_opengl_LIBRARY OR NOT OpenGL_GL_PREFERENCE STREQUAL GLVND)
            message(SEND_ERROR "Without GLVND, GlxApplication is available only if MAGNUM_TARGET_EGL is disabled. Use XEglApplication on EGL instead.")
        endif()
    endif()

    set(MagnumGlxApplication_SRCS
        GlxApplication.cpp
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumGlxContextHandler>
        $<TARGET_OBJECTS:MagnumGlxContextObjects>)
    set(MagnumGlxApplication_HEADERS GlxApplication.h)

    add_library(MagnumGlxApplication STATIC
        ${MagnumGlxApplication_SRCS}
        ${MagnumGlxApplication_HEADERS})
    set_target_properties(MagnumGlxApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_include_directories(MagnumGlxApplication PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(MagnumGlxApplication PUBLIC
        MagnumGL
        ${X11_LIBRARIES})
    # If the GLVND library (CMake 3.10+) was found and linked to, we need to
    # link to GLX explicitly. Otherwise (and also on all systems except Linux)
    # the transitive dependency to classic GL lib from MagnumGL is enough.
    # Can't use OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
    # *not* found. WTF. Also can't just check for OPENGL_opengl_LIBRARY because
    # that's set even if OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
    #
    # See also the logic above for preventing libGL and libGLES conflicts. If
    # there's no conflict, this branch is taken and libGLX is linked.
    if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
        target_link_libraries(MagnumGlxApplication PUBLIC OpenGL::GLX)
    endif()

    install(FILES ${MagnumGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum GlxApplication target alias for superprojects
    add_library(Magnum::GlxApplication ALIAS MagnumGlxApplication)
endif()

# X/EGL application
if(MAGNUM_WITH_XEGLAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "XEglApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_EGLCONTEXTHANDLER 1)
    set(NEED_EGLCONTEXT 1)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some applications, was not found. Set MAGNUM_WITH_*EGL*APPLICATION to OFF to skip building them.")
    endif()

    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some applications, was not found. Set MAGNUM_WITH_*X*APPLICATION to OFF to skip building them.")
    endif()

    # Unlike with GlxApplication above, here no conflict between libGLES and
    # libGL can happen -- we only either link libGLES and libEGL or libGL and
    # libEGL.

    set(MagnumXEglApplication_SRCS
        XEglApplication.cpp
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumEglContextHandler>
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumXEglApplication_HEADERS XEglApplication.h)

    add_library(MagnumXEglApplication STATIC
        ${MagnumXEglApplication_SRCS}
        ${MagnumXEglApplication_HEADERS})
    set_target_properties(MagnumXEglApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_include_directories(MagnumXEglApplication PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(MagnumXEglApplication PUBLIC MagnumGL ${X11_LIBRARIES} EGL::EGL)

    install(FILES ${MagnumXEglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumXEglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum XEglApplication target alias for superprojects
    add_library(Magnum::XEglApplication ALIAS MagnumXEglApplication)
endif()

# Windowless EGL application
if(MAGNUM_WITH_WINDOWLESSEGLAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "WindowlessEglApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    set(NEED_EGLCONTEXT 1)

    set(MagnumWindowlessEglApplication_SRCS
        WindowlessEglApplication.cpp
        Implementation/Egl.cpp
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumWindowlessEglApplication_HEADERS
        WindowlessEglApplication.h)
    set(MagnumWindowlessEglApplication_PRIVATE_HEADERS
        Implementation/Egl.h)

    add_library(MagnumWindowlessEglApplication STATIC
        ${MagnumWindowlessEglApplication_SRCS}
        ${MagnumWindowlessEglApplication_HEADERS}
        ${MagnumWindowlessEglApplication_PRIVATE_HEADERS})
    set_target_properties(MagnumWindowlessEglApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessEglApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumWindowlessEglApplication PUBLIC MagnumGL EGL::EGL)

    install(FILES ${MagnumWindowlessEglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessEglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessEglApplication target alias for superprojects
    add_library(Magnum::WindowlessEglApplication ALIAS MagnumWindowlessEglApplication)
endif()

# Windowless GLX application
if(MAGNUM_WITH_WINDOWLESSGLXAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "WindowlessGlxApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    set(NEED_GLXCONTEXT 1)

    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some applications, was not found. Set MAGNUM_WITH_*X*APPLICATION to OFF to skip building them.")
    endif()

    # If we're targeting GLES and EGL, only the libGLES library is looked for
    # by default. Here we need libGLX from GLVND in addition. If GLVND is not
    # available however (or CMake < 3.10 is used), fail, as GLX symbols are
    # only in libGL in that case, which would conflict with libGLES.
    if(MAGNUM_TARGET_GLES AND MAGNUM_TARGET_EGL)
        find_package(OpenGL)
        if(NOT OpenGL_FOUND OR NOT OPENGL_opengl_LIBRARY OR NOT OpenGL_GL_PREFERENCE STREQUAL GLVND)
            message(SEND_ERROR "Without GLVND, WindowlessGlxApplication is available only if MAGNUM_TARGET_EGL is disabled. Use WindowlessEglApplication on EGL instead.")
        endif()
    endif()

    set(MagnumWindowlessGlxApplication_SRCS
        WindowlessGlxApplication.cpp
        $<TARGET_OBJECTS:MagnumGlxContextObjects>)
    set(MagnumWindowlessGlxApplication_HEADERS WindowlessGlxApplication.h)

    add_library(MagnumWindowlessGlxApplication STATIC
        ${MagnumWindowlessGlxApplication_SRCS}
        ${MagnumWindowlessGlxApplication_HEADERS})
    set_target_properties(MagnumWindowlessGlxApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    # X11 macros are a mess, disable warnings for C-style casts
    target_compile_options(MagnumWindowlessGlxApplication PRIVATE "-Wno-old-style-cast")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessGlxApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_include_directories(MagnumWindowlessGlxApplication PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(MagnumWindowlessGlxApplication PUBLIC
        MagnumGL
        ${X11_LIBRARIES})
    # If the GLVND library (CMake 3.11+) was found and linked to, we need to
    # link to GLX explicitly. Otherwise (and also on all systems except Linux)
    # the transitive dependency to classic GL lib from MagnumGL is enough.
    # Can't use OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
    # *not* found. WTF. Also can't just check for OPENGL_opengl_LIBRARY because
    # that's set even if OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
    if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
        target_link_libraries(MagnumWindowlessGlxApplication PUBLIC OpenGL::GLX)
    endif()

    install(FILES ${MagnumWindowlessGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessGlxApplication target alias for superprojects
    add_library(Magnum::WindowlessGlxApplication ALIAS MagnumWindowlessGlxApplication)
endif()

# Windowless iOS application
if(MAGNUM_WITH_WINDOWLESSIOSAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "WindowlessIosApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    set(NEED_EGLCONTEXT 1)

    # We need to link to Foundation framework to use ObjC
    find_library(_MAGNUM_IOS_FOUNDATION_FRAMEWORK_LIBRARY Foundation)
    mark_as_advanced(_MAGNUM_IOS_FOUNDATION_FRAMEWORK_LIBRARY)

    set(MagnumWindowlessIosApplication_SRCS
        WindowlessIosApplication.mm
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumWindowlessIosApplication_HEADERS
        WindowlessIosApplication.h)

    add_library(MagnumWindowlessIosApplication STATIC
        ${MagnumWindowlessIosApplication_SRCS}
        ${MagnumWindowlessIosApplication_HEADERS})
    set_target_properties(MagnumWindowlessIosApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessIosApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumWindowlessIosApplication
        MagnumGL
        EGL::EGL
        ${_MAGNUM_IOS_FOUNDATION_FRAMEWORK_LIBRARY})

    install(FILES ${MagnumWindowlessIosApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessIosApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessIosApplication target alias for superprojects
    add_library(Magnum::WindowlessIosApplication ALIAS MagnumWindowlessIosApplication)
endif()

# Windowless WGL application
if(MAGNUM_WITH_WINDOWLESSWGLAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "WindowlessWglApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    set(NEED_WGLCONTEXT 1)

    set(MagnumWindowlessWglApplication_SRCS
        WindowlessWglApplication.cpp
        $<TARGET_OBJECTS:MagnumWglContextObjects>)
    set(MagnumWindowlessWglApplication_HEADERS WindowlessWglApplication.h)

    add_library(MagnumWindowlessWglApplication STATIC
        ${MagnumWindowlessWglApplication_SRCS}
        ${MagnumWindowlessWglApplication_HEADERS})
    set_target_properties(MagnumWindowlessWglApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumWindowlessWglApplication PRIVATE "UNICODE")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessWglApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumWindowlessWglApplication PUBLIC MagnumGL)

    install(FILES ${MagnumWindowlessWglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessWglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessWglApplication target alias for superprojects
    add_library(Magnum::WindowlessWglApplication ALIAS MagnumWindowlessWglApplication)
endif()

# Windowless CGL application
if(MAGNUM_WITH_WINDOWLESSCGLAPPLICATION)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "WindowlessCglApplication is available only if MAGNUM_TARGET_GL is enabled")
    endif()
    if(MAGNUM_TARGET_GLES)
        message(SEND_ERROR "WindowlessCglApplication is not available if MAGNUM_TARGET_GLES is enabled")
    endif()

    set(NEED_CGLCONTEXT 1)

    set(MagnumWindowlessCglApplication_SRCS
        WindowlessCglApplication.cpp
        $<TARGET_OBJECTS:MagnumCglContextObjects>)
    set(MagnumWindowlessCglApplication_HEADERS WindowlessCglApplication.h)

    add_library(MagnumWindowlessCglApplication STATIC
        ${MagnumWindowlessCglApplication_SRCS}
        ${MagnumWindowlessCglApplication_HEADERS})
    set_target_properties(MagnumWindowlessCglApplication PROPERTIES
        DEBUG_POSTFIX "-d")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessCglApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumWindowlessCglApplication PUBLIC MagnumGL)

    install(FILES ${MagnumWindowlessCglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessCglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessCglApplication library for superprojects
    add_library(Magnum::WindowlessCglApplication ALIAS MagnumWindowlessCglApplication)
endif()

# Abstract X application
if(NEED_ABSTRACTXAPPLICATION)
    set(MagnumAbstractXApplication_SRCS AbstractXApplication.cpp)
    set(MagnumAbstractXApplication_HEADERS AbstractXApplication.h)

    add_library(MagnumAbstractXApplication OBJECT
        ${MagnumAbstractXApplication_SRCS}
        ${MagnumAbstractXApplication_HEADERS})
    target_include_directories(MagnumAbstractXApplication PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    # X11 macros are a mess, disable warnings for C-style casts
    target_compile_options(MagnumAbstractXApplication PRIVATE "-Wno-old-style-cast")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumAbstractXApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
endif()

# GLX context handler
if(NEED_GLXCONTEXTHANDLER)
    set(MagnumGlxContextHandler_SRCS
        Implementation/GlxContextHandler.cpp)
    set(MagnumGlxContextHandler_PRIVATE_HEADERS
        Implementation/AbstractContextHandler.h
        Implementation/GlxContextHandler.h)

    add_library(MagnumGlxContextHandler OBJECT
        ${MagnumGlxContextHandler_SRCS}
        ${MagnumGlxContextHandler_PRIVATE_HEADERS})
    target_include_directories(MagnumGlxContextHandler PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
    # X11 macros are a mess, disable warnings for C-style casts
    target_compile_options(MagnumGlxContextHandler PRIVATE "-Wno-old-style-cast")
    # Assuming that PIC is not needed because this is part of Application lib,
    # which is always linked to the executable and not to any intermediate
    # shared lib
endif()

# EGL context handler
if(NEED_EGLCONTEXTHANDLER)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some contexts, was not found. Set MAGNUM_WITH_*EGL*APPLICATION to OFF to skip building them.")
    endif()

    set(MagnumEglContextHandler_SRCS
        Implementation/EglContextHandler.cpp
        Implementation/Egl.cpp)
    set(MagnumEglContextHandler_PRIVATE_HEADERS
        Implementation/AbstractContextHandler.h
        Implementation/EglContextHandler.h
        Implementation/Egl.h)

    add_library(MagnumEglContextHandler OBJECT
        ${MagnumEglContextHandler_SRCS}
        ${MagnumEglContextHandler_PRIVATE_HEADERS})
    target_include_directories(MagnumEglContextHandler PUBLIC
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:EGL::EGL,INTERFACE_INCLUDE_DIRECTORIES>)
    # X11 macros are a mess, disable warnings for C-style casts
    target_compile_options(MagnumEglContextHandler PRIVATE "-Wno-old-style-cast")
    # Assuming that PIC is not needed because this is part of Application lib,
    # which is always linked to the executable and not to any intermediate
    # shared lib
endif()

# Platform-specific sources for context library
set(MagnumContext_SRCS )
if(NOT CORRADE_TARGET_IOS)
    list(APPEND MagnumContext_SRCS Implementation/OpenGLFunctionLoader.cpp)
endif()
if(NOT MAGNUM_TARGET_GLES)
    list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GL/flextGLPlatform.cpp)
elseif(MAGNUM_TARGET_GLES AND MAGNUM_TARGET_GLES2)
    if(CORRADE_TARGET_WINDOWS AND NOT MAGNUM_TARGET_EGL)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatformWindowsDesktop.cpp)
    elseif(CORRADE_TARGET_IOS)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatformIOS.cpp)
    else()
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatform.cpp)
    endif()
elseif(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_GLES2)
    if(CORRADE_TARGET_WINDOWS AND NOT MAGNUM_TARGET_EGL)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatformWindowsDesktop.cpp)
    elseif(CORRADE_TARGET_IOS)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatformIOS.cpp)
    else()
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatform.cpp)
    endif()
endif()

# CGL context
if(NEED_CGLCONTEXT OR MAGNUM_WITH_CGLCONTEXT)
    add_library(MagnumCglContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumCglContextObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumCglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(MAGNUM_WITH_CGLCONTEXT)
        if(NOT MAGNUM_TARGET_GL)
            message(SEND_ERROR "CglContext is available only if MAGNUM_TARGET_GL is enabled")
        endif()

        add_library(MagnumCglContext STATIC
            $<TARGET_OBJECTS:MagnumCglContextObjects>
            ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
        set_target_properties(MagnumCglContext PROPERTIES
            DEBUG_POSTFIX "-d")
        if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
            set_target_properties(MagnumCglContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        target_link_libraries(MagnumCglContext PUBLIC MagnumGL)

        install(TARGETS MagnumCglContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum CglContext target alias for superprojects
        add_library(Magnum::CglContext ALIAS MagnumCglContext)
    endif()
endif()

# EGL context
if(NEED_EGLCONTEXT OR MAGNUM_WITH_EGLCONTEXT)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some contexts, was not found. Set MAGNUM_WITH_*APPLICATION and/or MAGNUM_WITH_EGLCONTEXT to OFF to skip building them.")
    endif()

    add_library(MagnumEglContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumEglContextObjects PUBLIC
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:EGL::EGL,INTERFACE_INCLUDE_DIRECTORIES>)
    target_compile_definitions(MagnumEglContextObjects PRIVATE "MAGNUM_PLATFORM_USE_EGL")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumEglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(MAGNUM_WITH_EGLCONTEXT)
        if(NOT MAGNUM_TARGET_GL)
            message(SEND_ERROR "EglContext is available only if MAGNUM_TARGET_GL is enabled")
        endif()

        add_library(MagnumEglContext STATIC
            $<TARGET_OBJECTS:MagnumEglContextObjects>
            ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
        set_target_properties(MagnumEglContext PROPERTIES
            DEBUG_POSTFIX "-d")
        if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
            set_target_properties(MagnumEglContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        target_link_libraries(MagnumEglContext PUBLIC MagnumGL EGL::EGL)

        install(TARGETS MagnumEglContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum EglContext target alias for superprojects
        add_library(Magnum::EglContext ALIAS MagnumEglContext)
    endif()
endif()

# GLX context
if(NEED_GLXCONTEXT OR MAGNUM_WITH_GLXCONTEXT)
    add_library(MagnumGlxContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumGlxContextObjects PUBLIC
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>
        ${X11_INCLUDE_DIR})
    target_compile_definitions(MagnumGlxContextObjects PRIVATE "MAGNUM_PLATFORM_USE_GLX")
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumGlxContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(MAGNUM_WITH_GLXCONTEXT)
        if(NOT MAGNUM_TARGET_GL)
            message(SEND_ERROR "GlxContext is available only if MAGNUM_TARGET_GL is enabled")
        endif()

        add_library(MagnumGlxContext STATIC $<TARGET_OBJECTS:MagnumGlxContextObjects>)
        set_target_properties(MagnumGlxContext PROPERTIES
            DEBUG_POSTFIX "-d")
        if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
            set_target_properties(MagnumGlxContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        target_include_directories(MagnumGlxContext PUBLIC ${X11_INCLUDE_DIR})
        target_link_libraries(MagnumGlxContext PUBLIC
            MagnumGL
            ${X11_LIBRARIES})
        # If the GLVND library (CMake 3.11+) was found and linked to, we need
        # to link to GLX explicitly. Otherwise (and also on all systems except
        # Linux) the transitive dependency to classic GL lib from MagnumGL is
        # enough. Can't use OpenGL_OpenGL_FOUND, because that one is set also
        # if GLVND is *not* found. WTF. Also can't just check for
        # OPENGL_opengl_LIBRARY because that's set even if OpenGL_GL_PREFERENCE
        # is explicitly set to LEGACY.
        if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
            target_link_libraries(MagnumGlxContext PUBLIC OpenGL::GLX)
        endif()

        install(TARGETS MagnumGlxContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum GlxContext target alias for superprojects
        add_library(Magnum::GlxContext ALIAS MagnumGlxContext)
    endif()
endif()

# WGL context
if(NEED_WGLCONTEXT OR MAGNUM_WITH_WGLCONTEXT)
    add_library(MagnumWglContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumWglContextObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumWglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(MAGNUM_WITH_WGLCONTEXT)
        if(NOT MAGNUM_TARGET_GL)
            message(SEND_ERROR "WglContext is available only if MAGNUM_TARGET_GL is enabled")
        endif()

        add_library(MagnumWglContext STATIC $<TARGET_OBJECTS:MagnumWglContextObjects>)
        set_target_properties(MagnumWglContext PROPERTIES
            DEBUG_POSTFIX "-d")
        if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
            set_target_properties(MagnumWglContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        target_link_libraries(MagnumWglContext PUBLIC MagnumGL)

        install(TARGETS MagnumWglContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum WglContext target alias for superprojects
        add_library(Magnum::WglContext ALIAS MagnumWglContext)
    endif()
endif()

# Magnum GL Info
if(MAGNUM_WITH_GL_INFO)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "magnum-gl-info is available only if MAGNUM_TARGET_GL is enabled")
    endif()

    find_package(Corrade REQUIRED Main)

    add_executable(magnum-gl-info gl-info.cpp)
    target_link_libraries(magnum-gl-info PRIVATE
        Corrade::Main
        MagnumGL)
    if(MAGNUM_TARGET_EGL)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessGlxApplication)
    elseif(CORRADE_TARGET_WINDOWS)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessWglApplication)
    else()
        message(FATAL_ERROR "magnum-gl-info is not available on this platform. Set MAGNUM_WITH_GL_INFO to OFF to skip building it.")
    endif()

    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        install(TARGETS magnum-gl-info DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
    endif()

    if(CORRADE_TARGET_IOS)
        set_target_properties(magnum-gl-info PROPERTIES
            MACOSX_BUNDLE ON
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    elseif(CORRADE_TARGET_EMSCRIPTEN)
        add_custom_command(TARGET magnum-gl-info POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/WebApplication.css
                ${CMAKE_CURRENT_SOURCE_DIR}/WindowlessEmscriptenApplication.js
                $<TARGET_FILE_DIR:magnum-gl-info>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/gl-info.html
                $<TARGET_FILE_DIR:magnum-gl-info>/magnum-gl-info.html)
        install(FILES gl-info.html DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info RENAME index.html)
        install(TARGETS magnum-gl-info DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info)
        install(FILES
            WebApplication.css
            WindowlessEmscriptenApplication.js
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info)
        install(FILES
            $<TARGET_FILE_DIR:magnum-gl-info>/magnum-gl-info.js.mem
            $<TARGET_FILE_DIR:magnum-gl-info>/magnum-gl-info.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info OPTIONAL)
    endif()

    # Magnum info target alias for superprojects
    add_executable(Magnum::gl-info ALIAS magnum-gl-info)
endif()

# Force IDEs display also all header files and additional files in project view
add_custom_target(MagnumPlatform SOURCES ${MagnumPlatform_HEADERS} ${MagnumPlatform_FILES})

if(MAGNUM_BUILD_TESTS)
    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()
