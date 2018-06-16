#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
#             Vladimír Vondruš <mosra@centrum.cz>
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

# Headers
set(MagnumPlatform_HEADERS
    GLContext.h
    Platform.h
    Screen.h
    ScreenedApplication.h
    ScreenedApplication.hpp)

if(BUILD_DEPRECATED AND TARGET_GL)
    list(APPEND MagnumPlatform_HEADERS Context.h)
endif()

# Files to display in project view of IDEs only (filled in below)
set(MagnumPlatform_FILES )

install(FILES ${MagnumPlatform_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)

# Decide about platform-specific context for cross-platform toolkits
if((WITH_GLFWAPPLICATION OR WITH_GLUTAPPLICATION OR WITH_SDL2APPLICATION) AND TARGET_GL)
    if(CORRADE_TARGET_APPLE AND NOT MAGNUM_TARGET_GLES)
        set(NEED_CGLCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumCglContextObjects>)
    elseif(CORRADE_TARGET_WINDOWS AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
        set(NEED_WGLCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumWglContextObjects>)
    elseif(CORRADE_TARGET_UNIX AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
        set(NEED_GLXCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumGlxContextObjects>)
        # If the GLVND library (CMake 3.11+) was found and linked to, we need
        # to link to GLX explicitly. Otherwise (and also on all systems except
        # Linux) the transitive dependency to classic GL lib from MagnumGL is
        # enough. Can't use OpenGL_OpenGL_FOUND, because that one is set also
        # if GLVND is *not* found. WTF.
        if(OPENGL_opengl_LIBRARY)
            set(MagnumSomeContext_LIBRARY OpenGL::GLX)
        endif()
    elseif(MAGNUM_TARGET_GLES AND NOT CORRADE_TARGET_EMSCRIPTEN)
        set(NEED_EGLCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumEglContextObjects>)
        # We're linking to EGL explicitly, no need to bother with GLVND there
    endif()
endif()

# Android application
if(WITH_ANDROIDAPPLICATION)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL Android)
        message(FATAL_ERROR "AndroidApplication is available only when targeting Android. Set WITH_ANDROIDAPPLICATION to OFF to skip building it.")
    endif()

    set(NEED_EGLCONTEXT 1)

    set(MagnumAndroidApplication_SRCS
        AndroidApplication.cpp
        Implementation/Egl.cpp
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumAndroidApplication_HEADERS
        AndroidApplication.h)
    set(MagnumAndroidApplication_PRIVATE_HEADERS
        Implementation/Egl.h)

    add_library(MagnumAndroidApplicationGlue OBJECT
        "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    set_target_properties(MagnumAndroidApplicationGlue PROPERTIES
        CORRADE_USE_PEDANTIC_FLAGS OFF
        FOLDER "Magnum/Platform")

    add_library(MagnumAndroidApplication STATIC
        ${MagnumAndroidApplication_SRCS}
        ${MagnumAndroidApplication_HEADERS}
        ${MagnumAndroidApplication_PRIVATE_HEADERS}
        $<TARGET_OBJECTS:MagnumAndroidApplicationGlue>)
    target_include_directories(MagnumAndroidApplication PUBLIC
        "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/")
    set_target_properties(MagnumAndroidApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_link_libraries(MagnumAndroidApplication PUBLIC MagnumGL)

    install(FILES ${MagnumAndroidApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumAndroidApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum AndroidApplication target alias for superprojects
    add_library(Magnum::AndroidApplication ALIAS MagnumAndroidApplication)
endif()

# GLFW application
if(WITH_GLFWAPPLICATION)
    find_package(GLFW)
    if(NOT GLFW_FOUND)
        message(FATAL_ERROR "GLFW library, required by GlfwApplication, was not found. Set WITH_GLFWAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumGlfwApplication_SRCS GlfwApplication.cpp)
    set(MagnumGlfwApplication_HEADERS GlfwApplication.h)
    if(TARGET_GL)
        list(APPEND MagnumGlfwApplication_SRCS ${MagnumSomeContext_OBJECTS})
    endif()

    add_library(MagnumGlfwApplication STATIC
        ${MagnumGlfwApplication_SRCS}
        ${MagnumGlfwApplication_HEADERS})
    set_target_properties(MagnumGlfwApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_link_libraries(MagnumGlfwApplication PUBLIC Magnum GLFW::GLFW)
    if(TARGET_GL)
        target_link_libraries(MagnumGlfwApplication PUBLIC
            MagnumGL
            # need to link to GLX explicitly if using GLVND (CMake 3.11+)
            ${MagnumSomeContext_LIBRARY})
    endif()

    install(FILES ${MagnumGlfwApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlfwApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum GlfwApplication target alias for superprojects
    add_library(Magnum::GlfwApplication ALIAS MagnumGlfwApplication)
endif()

# GLUT application
if(WITH_GLUTAPPLICATION)
    find_package(GLUT)
    if(NOT GLUT_FOUND)
        message(FATAL_ERROR "GLUT library, required by GlutApplication, was not found. Set WITH_GLUTAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumGlutApplication_SRCS
        GlutApplication.cpp
        ${MagnumSomeContext_OBJECTS})
    set(MagnumGlutApplication_HEADERS GlutApplication.h)

    add_library(MagnumGlutApplication STATIC
        ${MagnumGlutApplication_SRCS}
        ${MagnumGlutApplication_HEADERS})
    set_target_properties(MagnumGlutApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_include_directories(MagnumGlutApplication PUBLIC ${GLUT_INCLUDE_DIR})
    target_link_libraries(MagnumGlutApplication PUBLIC
        MagnumGL
        ${GLUT_glut_LIBRARY}
        # need to link to GLX explicitly if using GLVND (CMake 3.11+)
        ${MagnumSomeContext_LIBRARY})

    install(FILES ${MagnumGlutApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlutApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum GlutApplication target alias for superprojects
    add_library(Magnum::GlutApplication ALIAS MagnumGlutApplication)
endif()

# SDL2 application
if(WITH_SDL2APPLICATION)
    find_package(SDL2)
    if(NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 library, required by Sdl2Application, was not found. Set WITH_SDL2APPLICATION to OFF to skip building it.")
    endif()

    set(MagnumSdl2Application_SRCS Sdl2Application.cpp)
    set(MagnumSdl2Application_HEADERS Sdl2Application.h)
    if(TARGET_GL)
        list(APPEND MagnumSdl2Application_SRCS ${MagnumSomeContext_OBJECTS})
    endif()

    add_library(MagnumSdl2Application STATIC
        ${MagnumSdl2Application_SRCS}
        ${MagnumSdl2Application_HEADERS})
    set_target_properties(MagnumSdl2Application PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_link_libraries(MagnumSdl2Application PUBLIC Magnum SDL2::SDL2)
    if(TARGET_GL)
        target_link_libraries(MagnumSdl2Application PUBLIC
            MagnumGL
            # need to link to GLX explicitly if using GLVND (CMake 3.11+)
            ${MagnumSomeContext_LIBRARY})
    endif()

    install(FILES ${MagnumSdl2Application_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumSdl2Application
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum Sdl2Application target alias for superprojects
    add_library(Magnum::Sdl2Application ALIAS MagnumSdl2Application)
endif()

# JavaScript and CSS stuff for Emscripten
if(CORRADE_TARGET_EMSCRIPTEN)
    set(MagnumEmscriptenApplication_FILES )
    if(WITH_SDL2APPLICATION)
        list(APPEND MagnumEmscriptenApplication_FILES EmscriptenApplication.js)
    endif()
    if(WITH_WINDOWLESSEGLAPPLICATION)
        list(APPEND MagnumEmscriptenApplication_FILES WindowlessEmscriptenApplication.js)
    endif()
    if(WITH_SDL2APPLICATION OR WITH_WINDOWLESSEGLAPPLICATION)
        list(APPEND MagnumEmscriptenApplication_FILES WebApplication.css)
        list(APPEND MagnumPlatform_FILES ${MagnumEmscriptenApplication_FILES})
        install(FILES ${MagnumEmscriptenApplication_FILES} DESTINATION ${MAGNUM_DATA_INSTALL_DIR})
    endif()
endif()

# GLX application
if(WITH_GLXAPPLICATION)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_GLXCONTEXTHANDLER 1)
    set(NEED_GLXCONTEXT 1)

    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some applications, was not found. Set WITH_*X*APPLICATION to OFF to skip building them.")
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
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib
    target_include_directories(MagnumGlxApplication PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(MagnumGlxApplication PUBLIC
        MagnumGL
        ${X11_LIBRARIES}
        # need to link to GLX explicitly if using GLVND (CMake 3.11+)
        ${MagnumSomeContext_LIBRARY})

    install(FILES ${MagnumGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum GlxApplication target alias for superprojects
    add_library(Magnum::GlxApplication ALIAS MagnumGlxApplication)
endif()

# X/EGL application
if(WITH_XEGLAPPLICATION)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_EGLCONTEXTHANDLER 1)
    set(NEED_EGLCONTEXT 1)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some applications, was not found. Set WITH_*EGL*APPLICATION to OFF to skip building them.")
    endif()

    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some applications, was not found. Set WITH_*X*APPLICATION to OFF to skip building them.")
    endif()

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
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
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
if(WITH_WINDOWLESSEGLAPPLICATION)
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
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    if(BUILD_STATIC_PIC)
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
if(WITH_WINDOWLESSGLXAPPLICATION)
    set(NEED_GLXCONTEXT 1)

    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some applications, was not found. Set WITH_*X*APPLICATION to OFF to skip building them.")
    endif()

    set(MagnumWindowlessGlxApplication_SRCS
        WindowlessGlxApplication.cpp
        $<TARGET_OBJECTS:MagnumGlxContextObjects>)
    set(MagnumWindowlessGlxApplication_HEADERS WindowlessGlxApplication.h)

    add_library(MagnumWindowlessGlxApplication STATIC
        ${MagnumWindowlessGlxApplication_SRCS}
        ${MagnumWindowlessGlxApplication_HEADERS})
    set_target_properties(MagnumWindowlessGlxApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    # X11 macros are a mess, disable warnings for C-style casts
    target_compile_options(MagnumWindowlessGlxApplication PRIVATE "-Wno-old-style-cast")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessGlxApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_include_directories(MagnumWindowlessGlxApplication PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(MagnumWindowlessGlxApplication PUBLIC
        MagnumGL
        ${X11_LIBRARIES}
        # need to link to GLX explicitly if using GLVND (CMake 3.11+)
        ${MagnumSomeContext_LIBRARY})

    install(FILES ${MagnumWindowlessGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessGlxApplication target alias for superprojects
    add_library(Magnum::WindowlessGlxApplication ALIAS MagnumWindowlessGlxApplication)
endif()

# Windowless iOS application
if(WITH_WINDOWLESSIOSAPPLICATION)
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
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    if(BUILD_STATIC_PIC)
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
if(WITH_WINDOWLESSWGLAPPLICATION)
    set(NEED_WGLCONTEXT 1)

    set(MagnumWindowlessWglApplication_SRCS
        WindowlessWglApplication.cpp
        $<TARGET_OBJECTS:MagnumWglContextObjects>)
    set(MagnumWindowlessWglApplication_HEADERS WindowlessWglApplication.h)

    add_library(MagnumWindowlessWglApplication STATIC
        ${MagnumWindowlessWglApplication_SRCS}
        ${MagnumWindowlessWglApplication_HEADERS})
    set_target_properties(MagnumWindowlessWglApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    target_compile_definitions(MagnumWindowlessWglApplication PRIVATE "UNICODE")
    if(BUILD_STATIC_PIC)
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

# Windowless Windows/EGL application
if(WITH_WINDOWLESSWINDOWSEGLAPPLICATION)
    set(NEED_EGLCONTEXT 1)

    set(MagnumWindowlessWindowsEglApplication_SRCS
        WindowlessWindowsEglApplication.cpp
        Implementation/Egl.cpp
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumWindowlessWindowsEglApplication_HEADERS
        WindowlessWindowsEglApplication.h)
    set(MagnumWindowlessWindowsEglApplication_PRIVATE_HEADERS
        Implementation/Egl.h)

    add_library(MagnumWindowlessWindowsEglApplication STATIC
        ${MagnumWindowlessWindowsEglApplication_SRCS}
        ${MagnumWindowlessWindowsEglApplication_HEADERS}
        ${MagnumWindowlessWindowsEglApplication_PRIVATE_HEADERS})
    set_target_properties(MagnumWindowlessWindowsEglApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumWindowlessWindowsEglApplication PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumWindowlessWindowsEglApplication PUBLIC MagnumGL EGL::EGL)

    install(FILES ${MagnumWindowlessWindowsEglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessWindowsEglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessWindowsEglApplication target alias for superprojects
    add_library(Magnum::WindowlessWindowsEglApplication ALIAS MagnumWindowlessWindowsEglApplication)
endif()

# Windowless CGL application
if(WITH_WINDOWLESSCGLAPPLICATION)
    set(NEED_CGLCONTEXT 1)

    set(MagnumWindowlessCglApplication_SRCS
        WindowlessCglApplication.cpp
        $<TARGET_OBJECTS:MagnumCglContextObjects>)
    set(MagnumWindowlessCglApplication_HEADERS WindowlessCglApplication.h)

    add_library(MagnumWindowlessCglApplication STATIC
        ${MagnumWindowlessCglApplication_SRCS}
        ${MagnumWindowlessCglApplication_HEADERS})
    set_target_properties(MagnumWindowlessCglApplication PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Platform")
    if(BUILD_STATIC_PIC)
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
    set_target_properties(MagnumAbstractXApplication PROPERTIES FOLDER "Magnum/Platform")

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
    set_target_properties(MagnumGlxContextHandler PROPERTIES FOLDER "Magnum/Platform")
endif()

# EGL context handler
if(NEED_EGLCONTEXTHANDLER)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some contexts, was not found. Set WITH_*EGL*APPLICATION to OFF to skip building them.")
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
    set_target_properties(MagnumEglContextHandler PROPERTIES FOLDER "Magnum/Platform")
endif()

# Platform-specific sources for context library
set(MagnumContext_SRCS )
if(NOT CORRADE_TARGET_IOS)
    list(APPEND MagnumContext_SRCS Implementation/OpenGLFunctionLoader.cpp)
endif()
if(NOT MAGNUM_TARGET_GLES)
    list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GL/flextGLPlatform.cpp)
elseif(MAGNUM_TARGET_GLES AND MAGNUM_TARGET_GLES2)
    if(CORRADE_TARGET_WINDOWS AND MAGNUM_TARGET_DESKTOP_GLES)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatformWindowsDesktop.cpp)
    elseif(CORRADE_TARGET_IOS)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatformIOS.cpp)
    else()
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatform.cpp)
    endif()
elseif(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_GLES2)
    if(CORRADE_TARGET_WINDOWS AND MAGNUM_TARGET_DESKTOP_GLES)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatformWindowsDesktop.cpp)
    elseif(CORRADE_TARGET_IOS)
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatformIOS.cpp)
    else()
        list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatform.cpp)
    endif()
endif()

# CGL context
if(NEED_CGLCONTEXT OR WITH_CGLCONTEXT)
    add_library(MagnumCglContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumCglContextObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumCglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties(MagnumCglContextObjects PROPERTIES FOLDER "Magnum/Platform")

    # Also create proper static library, if requested
    if(WITH_CGLCONTEXT)
        # CMake-generated XCode projects had some problems when library
        # consisted only of $<TARGET_OBJECTS> entries, thus compiling the
        # sources again in this case
        add_library(MagnumCglContext STATIC ${MagnumContext_SRCS})
        set_target_properties(MagnumCglContext PROPERTIES
            DEBUG_POSTFIX "-d"
            FOLDER "Magnum/Platform")
        if(BUILD_STATIC_PIC)
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
if(NEED_EGLCONTEXT OR WITH_EGLCONTEXT)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some contexts, was not found. Set WITH_*APPLICATION and/or WITH_EGLCONTEXT to OFF to skip building them.")
    endif()

    add_library(MagnumEglContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumEglContextObjects PUBLIC
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:EGL::EGL,INTERFACE_INCLUDE_DIRECTORIES>)
    target_compile_definitions(MagnumEglContextObjects PRIVATE "MAGNUM_PLATFORM_USE_EGL")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumEglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties(MagnumEglContextObjects PROPERTIES FOLDER "Magnum/Platform")

    # Also create proper static library, if requested
    if(WITH_EGLCONTEXT)
        add_library(MagnumEglContext STATIC
            $<TARGET_OBJECTS:MagnumEglContextObjects>
            ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
        set_target_properties(MagnumEglContext PROPERTIES
            DEBUG_POSTFIX "-d"
            FOLDER "Magnum/Platform")
        if(BUILD_STATIC_PIC)
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
if(NEED_GLXCONTEXT OR WITH_GLXCONTEXT)
    add_library(MagnumGlxContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumGlxContextObjects PUBLIC
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>
        ${X11_INCLUDE_DIR})
    target_compile_definitions(MagnumGlxContextObjects PRIVATE "MAGNUM_PLATFORM_USE_GLX")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumGlxContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties(MagnumGlxContextObjects PROPERTIES FOLDER "Magnum/Platform")

    # Also create proper static library, if requested
    if(WITH_GLXCONTEXT)
        add_library(MagnumGlxContext STATIC $<TARGET_OBJECTS:MagnumGlxContextObjects>)
        set_target_properties(MagnumGlxContext PROPERTIES
            DEBUG_POSTFIX "-d"
            FOLDER "Magnum/Platform")
        if(BUILD_STATIC_PIC)
            set_target_properties(MagnumGlxContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        target_include_directories(MagnumGlxContext PUBLIC ${X11_INCLUDE_DIR})
        target_link_libraries(MagnumGlxContext PUBLIC
            MagnumGL
            ${X11_LIBRARIES}
            ${MagnumSomeContext_LIBRARY})

        install(TARGETS MagnumGlxContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum GlxContext target alias for superprojects
        add_library(Magnum::GlxContext ALIAS MagnumGlxContext)
    endif()
endif()

# WGL context
if(NEED_WGLCONTEXT OR WITH_WGLCONTEXT)
    add_library(MagnumWglContextObjects OBJECT ${MagnumContext_SRCS})
    target_include_directories(MagnumWglContextObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumWglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties(MagnumWglContextObjects PROPERTIES FOLDER "Magnum/Platform")

    # Also create proper static library, if requested
    if(WITH_WGLCONTEXT)
        add_library(MagnumWglContext STATIC $<TARGET_OBJECTS:MagnumWglContextObjects>)
        set_target_properties(MagnumWglContext PROPERTIES
            DEBUG_POSTFIX "-d"
            FOLDER "Magnum/Platform")
        if(BUILD_STATIC_PIC)
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
if(WITH_GL_INFO)
    add_executable(magnum-gl-info gl-info.cpp)
    target_link_libraries(magnum-gl-info PRIVATE MagnumGL)
    if(MAGNUM_TARGET_HEADLESS OR CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessEglApplication)
        else()
            target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessGlxApplication)
        endif()
    elseif(CORRADE_TARGET_WINDOWS)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessWindowsEglApplication)
        else()
            target_link_libraries(magnum-gl-info PRIVATE MagnumWindowlessWglApplication)
        endif()
    else()
        message(FATAL_ERROR "magnum-gl-info is not available on this platform. Set WITH_GL_INFO to OFF to skip building it.")
    endif()
    set_target_properties(magnum-gl-info PROPERTIES FOLDER "Magnum/Platform")

    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        install(TARGETS magnum-gl-info DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
    endif()

    if(CORRADE_TARGET_IOS)
        set_target_properties(magnum-gl-info PROPERTIES
            MACOSX_BUNDLE ON
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    elseif(CORRADE_TARGET_EMSCRIPTEN)
        install(FILES gl-info.html DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info RENAME index.html)
        install(TARGETS magnum-gl-info DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info)
        install(FILES
            WebApplication.css
            WindowlessEmscriptenApplication.js
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info)
        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/magnum-gl-info.js.mem
            ${CMAKE_CURRENT_BINARY_DIR}/magnum-gl-info.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-gl-info OPTIONAL)
    endif()

    # Magnum info target alias for superprojects
    add_executable(Magnum::gl-info ALIAS magnum-gl-info)
endif()

# Force IDEs display also all header files and additional files in project view
add_custom_target(MagnumPlatform SOURCES ${MagnumPlatform_HEADERS} ${MagnumPlatform_FILES})
set_target_properties(MagnumPlatform PROPERTIES FOLDER "Magnum/Platform")

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
