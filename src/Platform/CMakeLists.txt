# Extension wrangler
add_library(MagnumPlatformExtensionWrangler OBJECT ExtensionWrangler.cpp)

set(MagnumPlatform_HEADERS
    AbstractContextHandler.h
    ExtensionWrangler.h)
install(FILES ${MagnumPlatform_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)

# GLUT application
if(WITH_GLUTAPPLICATION)
    find_package(GLUT)
    if(GLUT_FOUND)
        add_library(MagnumGlutApplication STATIC
            GlutApplication.cpp
            $<TARGET_OBJECTS:MagnumPlatformExtensionWrangler>)
        install(FILES GlutApplication.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
        install(TARGETS MagnumGlutApplication DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "GLUT library, required by GlutApplication, was not found. Set WITH_GLUTWINDOWCONTEXT to OFF to skip building it.")
    endif()
endif()

# SDL2 application
if(WITH_SDL2APPLICATION)
    find_package(SDL2)
    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIR})
        add_library(MagnumSdl2Application STATIC
            Sdl2Application.cpp
            $<TARGET_OBJECTS:MagnumPlatformExtensionWrangler>)
        install(FILES Sdl2Application.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
        install(TARGETS MagnumSdl2Application DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "SDL2 library, required by Sdl2Application, was not found. Set WITH_SDL2WINDOWCONTEXT to OFF to skip building it.")
    endif()
endif()

# GLX application
if(WITH_GLXAPPLICATION)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_GLXCONTEXT 1)
    add_library(MagnumGlxApplication STATIC
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumGlxContextHandler>
        $<TARGET_OBJECTS:MagnumPlatformExtensionWrangler>)
    install(FILES GlxApplication.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumGlxApplication DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# X/EGL window context
if(WITH_XEGLWINDOWCONTEXT)
    set(NEED_ABSTRACTXAPPLICATION 1)
    set(NEED_EGLCONTEXT 1)
    add_library(MagnumXEglApplication STATIC
        $<TARGET_OBJECTS:MagnumAbstractXApplication>
        $<TARGET_OBJECTS:MagnumEglContextHandler>
        $<TARGET_OBJECTS:MagnumPlatformExtensionWrangler>)
    install(FILES XEglApplication.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
    install(TARGETS MagnumXEglApplication DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Abstract X application
if(NEED_ABSTRACTXAPPLICATION)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some contexts, was not found. Set WITH_*X*WINDOWCONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumAbstractXApplication OBJECT AbstractXApplication.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumAbstractXApplication PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES AbstractXApplication.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
endif()

# GLX context
if(NEED_GLXCONTEXT)
    add_library(MagnumGlxContextHandler OBJECT GlxContextHandler.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumGlxContextHandler PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES GlxContextHandler.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
endif()

# EGL context
if(NEED_EGLCONTEXT)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some window contexts, was not found. Set WITH_*EGL*WINDOWCONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumEglContextHandler OBJECT EglContextHandler.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumEglContextHandler PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES EglContextHandler.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Platform)
endif()
