#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
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

# Android application
if(WITH_ANDROIDAPPLICATION)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL Android)
        message(FATAL_ERROR "AndroidApplication is available only when targeting Android. Set WITH_ANDROIDAPPLICATION to OFF to skip building it.")
    endif()

    include_directories(${ANDROID_NATIVE_APP_GLUE_INCLUDE_DIR})

    set(MagnumAndroidApplication_SRCS
        AndroidApplication.cpp
        Implementation/Egl.cpp)
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
    install(FILES ${MagnumAndroidApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumAndroidApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# GLUT application
if(WITH_GLUTAPPLICATION)
    find_package(GLUT)
    if(NOT GLUT_FOUND)
        message(FATAL_ERROR "GLUT library, required by GlutApplication, was not found. Set WITH_GLUTAPPLICATION to OFF to skip building it.")
    endif()

    set(MagnumGlutApplication_SRCS GlutApplication.cpp)
    set(MagnumGlutApplication_HEADERS GlutApplication.h)

    add_library(MagnumGlutApplication STATIC
        ${MagnumGlutApplication_SRCS}
        ${MagnumGlutApplication_HEADERS})
    set_target_properties(MagnumGlutApplication PROPERTIES DEBUG_POSTFIX "-d")
    install(FILES ${MagnumGlutApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlutApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# SDL2 application
if(WITH_SDL2APPLICATION)
    find_package(SDL2)
    if(NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 library, required by Sdl2Application, was not found. Set WITH_SDL2APPLICATION to OFF to skip building it.")
    endif()

    include_directories(${SDL2_INCLUDE_DIR})

    set(MagnumSdl2Application_SRCS Sdl2Application.cpp)
    set(MagnumSdl2Application_HEADERS Sdl2Application.h)

    add_library(MagnumSdl2Application STATIC
        ${MagnumSdl2Application_SRCS}
        ${MagnumSdl2Application_HEADERS})
    set_target_properties(MagnumSdl2Application PROPERTIES DEBUG_POSTFIX "-d")
    install(FILES ${MagnumSdl2Application_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumSdl2Application
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
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
    install(FILES ${MagnumWindowlessNaClApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessNaClApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
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

    set(MagnumGlxApplication_SRCS
        GlxApplication.cpp
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumGlxContextHandler>)
    set(MagnumGlxApplication_HEADERS GlxApplication.h)

    add_library(MagnumGlxApplication STATIC
        ${MagnumGlxApplication_SRCS}
        ${MagnumGlxApplication_HEADERS})
    set_target_properties(MagnumGlxApplication PROPERTIES DEBUG_POSTFIX "-d")
    install(FILES ${MagnumGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# X/EGL application
if(WITH_XEGLAPPLICATION)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_EGLCONTEXTHANDLER 1)

    set(MagnumXEglApplication_SRCS
        XEglApplication.cpp
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumEglContextHandler>)
    set(MagnumXEglApplication_HEADERS XEglApplication.h)

    add_library(MagnumXEglApplication STATIC
        ${MagnumXEglApplication_SRCS}
        ${MagnumXEglApplication_HEADERS})
    set_target_properties(MagnumXEglApplication PROPERTIES DEBUG_POSTFIX "-d")
    install(FILES ${MagnumXEglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumXEglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

if(WITH_WINDOWLESSGLXAPPLICATION OR NEED_ABSTRACTXAPPLICATION)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some contexts, was not found. Set WITH_*X*APPLICATION to OFF to skip building them.")
    endif()
endif()

# Windowless GLX application
if(WITH_WINDOWLESSGLXAPPLICATION)
    set(MagnumWindowlessGlxApplication_SRCS WindowlessGlxApplication.cpp)
    set(MagnumWindowlessGlxApplication_HEADERS WindowlessGlxApplication.h)

    add_library(MagnumWindowlessGlxApplication STATIC
        ${MagnumWindowlessGlxApplication_SRCS}
        ${MagnumWindowlessGlxApplication_HEADERS})
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumWindowlessGlxApplication PROPERTIES
        COMPILE_FLAGS "-Wno-old-style-cast"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessGlxApplication Magnum ${X11_LIBRARIES})
    install(FILES ${MagnumWindowlessGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessGlxApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Windowless WGL application
if(WITH_WINDOWLESSWGLAPPLICATION)
    set(MagnumWindowlessWglApplication_SRCS WindowlessWglApplication.cpp)
    set(MagnumWindowlessWglApplication_HEADERS WindowlessWglApplication.h)

    add_library(MagnumWindowlessWglApplication STATIC
        ${MagnumWindowlessWglApplication_SRCS}
        ${MagnumWindowlessWglApplication_HEADERS})
    set_target_properties(MagnumWindowlessWglApplication PROPERTIES
        COMPILE_FLAGS "-DUNICODE"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessWglApplication Magnum)
    install(FILES ${MagnumWindowlessWglApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessWglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Windowless CGL application
if(WITH_WINDOWLESSCGLAPPLICATION)
    set(MagnumWindowlessCglApplication_SRCS WindowlessCglApplication.cpp)
    set(MagnumWindowlessCglApplication_HEADERS WindowlessCglApplication.h)

    add_library(MagnumWindowlessCglApplication STATIC
        ${MagnumWindowlessCglApplication_SRCS}
        ${MagnumWindowlessCglApplication_HEADERS})
    set_target_properties(MagnumWindowlessCglApplication PROPERTIES DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumWindowlessCglApplication Magnum)
    install(FILES ${MagnumWindowlessGlxApplication_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumWindowlessCglApplication
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
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
endif()

# EGL context handler
if(NEED_EGLCONTEXTHANDLER)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some window contexts, was not found. Set WITH_*EGL*APPLICATION to OFF to skip building them.")
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
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumEglContextHandler PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
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
        target_link_libraries(magnum-info MagnumWindowlessWglApplication)
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
