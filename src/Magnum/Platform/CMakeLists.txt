#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
    Context.h
    Platform.h
    Screen.h
    ScreenedApplication.h
    ScreenedApplication.hpp)

# Files to display in project view of IDEs only (filled in below)
set(MagnumPlatform_FILES )

install(FILES ${MagnumPlatform_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)

# Decide about platform-specific context for cross-platform toolkits
if(WITH_GLUTAPPLICATION OR WITH_SDL2APPLICATION)
    if(CORRADE_TARGET_APPLE AND NOT MAGNUM_TARGET_GLES)
        set(NEED_CGLCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumCglContextObjects>)
    elseif(CORRADE_TARGET_WINDOWS AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
        set(NEED_WGLCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumWglContextObjects>)
    elseif(CORRADE_TARGET_UNIX AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
        set(NEED_GLXCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumGlxContextObjects>)
    elseif(MAGNUM_TARGET_GLES AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_NACL)
        set(NEED_EGLCONTEXT 1)
        set(MagnumSomeContext_OBJECTS $<TARGET_OBJECTS:MagnumEglContextObjects>)
    endif()
endif()

# Android application
if(WITH_ANDROIDAPPLICATION)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL Android)
        message(FATAL_ERROR "AndroidApplication is available only when targeting Android. Set WITH_ANDROIDAPPLICATION to OFF to skip building it.")
    endif()

    set(NEED_EGLCONTEXT 1)

    include_directories(${ANDROID_NATIVE_APP_GLUE_INCLUDE_DIR})

    set(MagnumAndroidApplication_SRCS
        AndroidApplication.cpp
        Implementation/Egl.cpp
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumAndroidApplication_HEADERS
        AndroidApplication.h)
    set(MagnumAndroidApplication_PRIVATE_HEADERS
        Implementation/Egl.h)

    add_library(MagnumAndroidApplication STATIC
        ${MagnumAndroidApplication_SRCS}
        ${MagnumAndroidApplication_HEADERS}
        ${MagnumAndroidApplication_PRIVATE_HEADERS}
        ${ANDROID_NATIVE_APP_GLUE_SRC})
    set_target_properties(MagnumAndroidApplication PROPERTIES DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumAndroidApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumAndroidApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum AndroidApplication library for superprojects
    set(MAGNUM_ANDROIDAPPLICATION_LIBRARY MagnumAndroidApplication CACHE INTERNAL "")
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
    set_target_properties(MagnumGlutApplication PROPERTIES DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumGlutApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlutApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum GlutApplication library for superprojects
    set(MAGNUM_GLUTAPPLICATION_LIBRARY MagnumGlutApplication CACHE INTERNAL "")
endif()

# SDL2 application
if(WITH_SDL2APPLICATION)
    find_package(SDL2)
    if(NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 library, required by Sdl2Application, was not found. Set WITH_SDL2APPLICATION to OFF to skip building it.")
    endif()

    include_directories(${SDL2_INCLUDE_DIR})

    set(MagnumSdl2Application_SRCS
        Sdl2Application.cpp
        ${MagnumSomeContext_OBJECTS})
    set(MagnumSdl2Application_HEADERS Sdl2Application.h)

    add_library(MagnumSdl2Application STATIC
        ${MagnumSdl2Application_SRCS}
        ${MagnumSdl2Application_HEADERS})
    set_target_properties(MagnumSdl2Application PROPERTIES DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumSdl2Application_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumSdl2Application
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum Sdl2Application library for superprojects
    set(MAGNUM_SDL2APPLICATION_LIBRARY MagnumSdl2Application CACHE INTERNAL "")
endif()

# NaCl application
if(WITH_NACLAPPLICATION)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL NaCl)
        message(FATAL_ERROR "NaClApplication is available only when targeting Google Chrome Native Client. Set WITH_NACLAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumNaClApplication_SRCS NaClApplication.cpp)
    set(MagnumNaClApplication_HEADERS NaClApplication.h)

    add_library(MagnumNaClApplication STATIC
        ${MagnumNaClApplication_SRCS}
        ${MagnumNaClApplication_HEADERS})
    set_target_properties(MagnumNaClApplication PROPERTIES DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumNaClApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumNaClApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Windowless NaCl application
if(WITH_WINDOWLESSNACLAPPLICATION)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL NaCl)
        message(FATAL_ERROR "WindowlessNaClApplication is available only when targeting Google Chrome Native Client. Set WITH_WINDOWLESSNACLAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumWindowlessNaClApplication_SRCS WindowlessNaClApplication.cpp)
    set(MagnumWindowlessNaClApplication_HEADERS WindowlessNaClApplication.h)

    add_library(MagnumWindowlessNaClApplication STATIC
        ${MagnumWindowlessNaClApplication_SRCS}
        ${MagnumWindowlessNaClApplication_HEADERS})
    set_target_properties(MagnumWindowlessNaClApplication PROPERTIES DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessNaClApplication Magnum ppapi_cpp ppapi)
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumWindowlessNaClApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessNaClApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessNaClApplication library for superprojects
    set(MAGNUM_WINDOWLESSNACLAPPLICATION_LIBRARY MagnumWindowlessNaClApplication CACHE INTERNAL "")
endif()

# JavaScript and CSS stuff for NaCl
if(WITH_NACLAPPLICATION OR WITH_WINDOWLESSNACLAPPLICATION)
    set(MagnumNaClApplication_FILES
        NaClApplication.js
        WebApplication.css)
    list(APPEND MagnumPlatform_FILES ${MagnumNaClApplication_FILES})
    install(FILES ${MagnumNaClApplication_FILES} DESTINATION ${MAGNUM_DATA_INSTALL_DIR})
endif()

# JavaScript and CSS stuff for Emscripten
if(WITH_SDL2APPLICATION AND CORRADE_TARGET_EMSCRIPTEN)
    set(MagnumSdl2Application_FILES
        EmscriptenApplication.js
        WebApplication.css)
    list(APPEND MagnumPlatform_FILES ${MagnumSdl2Application_FILES})
    install(FILES ${MagnumSdl2Application_FILES} DESTINATION ${MAGNUM_DATA_INSTALL_DIR})
endif()

# GLX application
if(WITH_GLXAPPLICATION)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_GLXCONTEXTHANDLER 1)
    set(NEED_GLXCONTEXT 1)

    set(MagnumGlxApplication_SRCS
        GlxApplication.cpp
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumGlxContextHandler>
        $<TARGET_OBJECTS:MagnumGlxContextObjects>)
    set(MagnumGlxApplication_HEADERS GlxApplication.h)

    add_library(MagnumGlxApplication STATIC
        ${MagnumGlxApplication_SRCS}
        ${MagnumGlxApplication_HEADERS})
    set_target_properties(MagnumGlxApplication PROPERTIES DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum GlxApplication library for superprojects
    set(MAGNUM_GLXAPPLICATION_LIBRARY MagnumGlxApplication CACHE INTERNAL "")
endif()

# X/EGL application
if(WITH_XEGLAPPLICATION)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_EGLCONTEXTHANDLER 1)
    set(NEED_EGLCONTEXT 1)

    set(MagnumXEglApplication_SRCS
        XEglApplication.cpp
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumEglContextHandler>
        $<TARGET_OBJECTS:MagnumEglContextObjects>)
    set(MagnumXEglApplication_HEADERS XEglApplication.h)

    add_library(MagnumXEglApplication STATIC
        ${MagnumXEglApplication_SRCS}
        ${MagnumXEglApplication_HEADERS})
    set_target_properties(MagnumXEglApplication PROPERTIES DEBUG_POSTFIX "-d")
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumXEglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumXEglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum XEglApplication library for superprojects
    set(MAGNUM_XEGLAPPLICATION_LIBRARY MagnumXEglApplication CACHE INTERNAL "")
endif()

if(WITH_WINDOWLESSGLXAPPLICATION OR NEED_ABSTRACTXAPPLICATION)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some contexts, was not found. Set WITH_*X*APPLICATION to OFF to skip building them.")
    endif()
endif()

# Windowless GLX application
if(WITH_WINDOWLESSGLXAPPLICATION)
    set(NEED_GLXCONTEXT 1)

    set(MagnumWindowlessGlxApplication_SRCS
        WindowlessGlxApplication.cpp
        $<TARGET_OBJECTS:MagnumGlxContextObjects>)
    set(MagnumWindowlessGlxApplication_HEADERS WindowlessGlxApplication.h)

    add_library(MagnumWindowlessGlxApplication STATIC
        ${MagnumWindowlessGlxApplication_SRCS}
        ${MagnumWindowlessGlxApplication_HEADERS})
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumWindowlessGlxApplication PROPERTIES
        COMPILE_FLAGS "-Wno-old-style-cast"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessGlxApplication Magnum ${X11_LIBRARIES})
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumWindowlessGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessGlxApplication library for superprojects
    set(MAGNUM_WINDOWLESSGLXAPPLICATION_LIBRARY MagnumWindowlessGlxApplication CACHE INTERNAL "")
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
        COMPILE_FLAGS "-DUNICODE"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessWglApplication Magnum)
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumWindowlessWglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessWglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessWglApplication library for superprojects
    set(MAGNUM_WINDOWLESSWGLAPPLICATION_LIBRARY MagnumWindowlessWglApplication CACHE INTERNAL "")
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
        COMPILE_FLAGS "-DUNICODE"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessWindowsEglApplication Magnum ${EGL_LIBRARY})
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumWindowlessWindowsEglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessWindowsEglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessWindowsEglApplication library for superprojects
    set(MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_LIBRARY MagnumWindowlessWindowsEglApplication CACHE INTERNAL "")
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
    set_target_properties(MagnumWindowlessCglApplication PROPERTIES DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessCglApplication Magnum)
    # Assuming that PIC is not needed because the Application lib is always
    # linked to the executable and not to any intermediate shared lib

    install(FILES ${MagnumWindowlessGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessCglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum WindowlessCglApplication library for superprojects
    set(MAGNUM_WINDOWLESSCGLAPPLICATION_LIBRARY MagnumWindowlessCglApplication CACHE INTERNAL "")
endif()

# Abstract X application
if(NEED_ABSTRACTXAPPLICATION)
    set(MagnumAbstractXApplication_SRCS AbstractXApplication.cpp)
    set(MagnumAbstractXApplication_HEADERS AbstractXApplication.h)

    add_library(MagnumAbstractXApplication OBJECT
        ${MagnumAbstractXApplication_SRCS}
        ${MagnumAbstractXApplication_HEADERS})
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumAbstractXApplication PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
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
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumGlxContextHandler PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    # Assuming that PIC is not needed because this is part of Application lib,
    # which is always linked to the executable and not to any intermediate
    # shared lib
endif()

# EGL context handler
if(NEED_EGLCONTEXTHANDLER)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some window contexts, was not found. Set WITH_*EGL*APPLICATION to OFF to skip building them.")
    endif()

    include_directories(${EGL_INCLUDE_DIR})

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
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumEglContextHandler PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    # Assuming that PIC is not needed because this is part of Application lib,
    # which is always linked to the executable and not to any intermediate
    # shared lib
endif()

# Platform-specific sources for context library
set(MagnumContext_SRCS Implementation/OpenGLFunctionLoader.cpp)
if(NOT MAGNUM_TARGET_GLES)
    list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GL/flextGLPlatform.cpp)
elseif(MAGNUM_TARGET_GLES AND MAGNUM_TARGET_GLES2)
    list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES2/flextGLPlatform.cpp)
elseif(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_GLES2)
    list(APPEND MagnumContext_SRCS ../../MagnumExternal/OpenGL/GLES3/flextGLPlatform.cpp)
endif()

# CGL context
if(NEED_CGLCONTEXT OR WITH_CGLCONTEXT)
    add_library(MagnumCglContextObjects OBJECT ${MagnumContext_SRCS})
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumCglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(WITH_CGLCONTEXT)
        # CMake-generated XCode projects had some problems when library
        # consisted only of $<TARGET_OBJECTS> entries, thus compiling the
        # sources again in this case
        add_library(MagnumCglContext STATIC ${MagnumContext_SRCS})
        set_target_properties(MagnumCglContext PROPERTIES DEBUG_POSTFIX "-d")
        if(BUILD_STATIC_PIC)
            set_target_properties(MagnumCglContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()

        install(TARGETS MagnumCglContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum CglContext library for superprojects
        set(MAGNUM_CGLCONTEXT_LIBRARY MagnumCglContext CACHE INTERNAL "")
    endif()
endif()

# EGL context
if(NEED_EGLCONTEXT OR WITH_EGLCONTEXT)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some window contexts, was not found. Set WITH_*APPLICATION and/or WITH_EGLCONTEXT to OFF to skip building them.")
    endif()

    include_directories(${EGL_INCLUDE_DIR})

    add_library(MagnumEglContextObjects OBJECT ${MagnumContext_SRCS})
    set_target_properties(MagnumEglContextObjects PROPERTIES COMPILE_DEFINITIONS "MAGNUM_PLATFORM_USE_EGL")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumEglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(WITH_EGLCONTEXT)
        add_library(MagnumEglContext STATIC $<TARGET_OBJECTS:MagnumEglContextObjects>)
        set_target_properties(MagnumEglContext PROPERTIES DEBUG_POSTFIX "-d")
        if(BUILD_STATIC_PIC)
            set_target_properties(MagnumEglContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()

        install(TARGETS MagnumEglContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum EglContext library for superprojects
        set(MAGNUM_EGLCONTEXT_LIBRARY MagnumEglContext CACHE INTERNAL "")
    endif()
endif()

# GLX context
if(NEED_GLXCONTEXT OR WITH_GLXCONTEXT)
    add_library(MagnumGlxContextObjects OBJECT ${MagnumContext_SRCS})
    set_target_properties(MagnumGlxContextObjects PROPERTIES COMPILE_DEFINITIONS "MAGNUM_PLATFORM_USE_GLX")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumGlxContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(WITH_GLXCONTEXT)
        add_library(MagnumGlxContext STATIC $<TARGET_OBJECTS:MagnumGlxContextObjects>)
        set_target_properties(MagnumGlxContext PROPERTIES DEBUG_POSTFIX "-d")
        if(BUILD_STATIC_PIC)
            set_target_properties(MagnumGlxContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()

        install(TARGETS MagnumGlxContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum GlxContext library for superprojects
        set(MAGNUM_GLXCONTEXT_LIBRARY MagnumGlxContext CACHE INTERNAL "")
    endif()
endif()

# WGL context
if(NEED_WGLCONTEXT OR WITH_WGLCONTEXT)
    add_library(MagnumWglContextObjects OBJECT ${MagnumContext_SRCS})
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumWglContextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # Also create proper static library, if requested
    if(WITH_GLXCONTEXT)
        add_library(MagnumWglContext STATIC $<TARGET_OBJECTS:MagnumWglContextObjects>)
        set_target_properties(MagnumWglContext PROPERTIES DEBUG_POSTFIX "-d")
        if(BUILD_STATIC_PIC)
            set_target_properties(MagnumWglContext PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()

        install(TARGETS MagnumWglContext
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

        # Magnum WglContext library for superprojects
        set(MAGNUM_WGLCONTEXT_LIBRARY MagnumWglContext CACHE INTERNAL "")
    endif()
endif()

# Magnum Info
if(WITH_MAGNUMINFO)
    add_executable(magnum-info magnum-info.cpp)
    if(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-info MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_NACL)
        target_link_libraries(magnum-info MagnumWindowlessNaClApplication)
    elseif(CORRADE_TARGET_UNIX)
        target_link_libraries(magnum-info MagnumWindowlessGlxApplication)
    elseif(CORRADE_TARGET_WINDOWS)
        if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-info MagnumWindowlessWglApplication)
        else()
            target_link_libraries(magnum-info MagnumWindowlessWindowsEglApplication)
        endif()
    else()
        message(FATAL_ERROR "magnum-info is not available on this platform. Set WITH_MAGNUMINFO to OFF to skip building it.")
    endif()
    target_link_libraries(magnum-info Magnum)

    install(TARGETS magnum-info DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
    if(CORRADE_TARGET_NACL)
        install(FILES magnum-info-nacl.html DESTINATION ${MAGNUM_DATA_INSTALL_DIR} RENAME magnum-info.html)
        install(FILES magnum-info-nacl.nmf DESTINATION ${MAGNUM_DATA_INSTALL_DIR} RENAME magnum-info.nmf)
        list(APPEND MagnumPlatform_FILES magnum-info-nacl.html magnum-info-nacl.nmf)
    endif()
endif()

# Force IDEs display also all header files and additional files in project view
add_custom_target(MagnumPlatform SOURCES ${MagnumPlatform_HEADERS} ${MagnumPlatform_FILES})
