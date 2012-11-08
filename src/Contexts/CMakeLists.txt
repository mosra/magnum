# Extension wrangler
add_library(MagnumContextsExtensionWrangler OBJECT ExtensionWrangler.cpp)

set(MagnumContexts_HEADERS
    AbstractContextHandler.h
    AbstractWindowContext.h
    ExtensionWrangler.h)
install(FILES ${MagnumContexts_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)

# GLUT window context
if(WITH_GLUTWINDOWCONTEXT)
    find_package(GLUT)
    if(GLUT_FOUND)
        add_library(MagnumGlutWindowContext STATIC
            GlutWindowContext.cpp
            $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
        install(FILES GlutWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumGlutWindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "GLUT library, required by GlutWindowContext, was not found. Set WITH_GLUTWINDOWCONTEXT to OFF to skip building it.")
    endif()
endif()

# SDL2 window context
if(WITH_SDL2WINDOWCONTEXT)
    find_package(SDL2)
    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIR})
        add_library(MagnumSdl2WindowContext STATIC
            Sdl2WindowContext.cpp
            $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
        install(FILES Sdl2WindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumSdl2WindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "SDL2 library, required by Sdl2WindowContext, was not found. Set WITH_SDL2WINDOWCONTEXT to OFF to skip building it.")
    endif()
endif()

# GLX window context
if(WITH_GLXWINDOWCONTEXT)
    set(NEED_ABSTRACTXWINDOWCONTEXT 1)
    set(NEED_GLXCONTEXT 1)
    add_library(MagnumGlxWindowContext STATIC
        $<TARGET_OBJECTS:MagnumAbstractXWindowContext>
        $<TARGET_OBJECTS:MagnumGlxContextHandler>
        $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
    install(FILES GlxWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumGlxWindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# X/EGL window context
if(WITH_XEGLWINDOWCONTEXT)
    set(NEED_ABSTRACTXWINDOWCONTEXT 1)
    set(NEED_EGLCONTEXT 1)
    add_library(MagnumXEglWindowContext STATIC
        $<TARGET_OBJECTS:MagnumAbstractXWindowContext>
        $<TARGET_OBJECTS:MagnumEglContextHandler>
        $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
    install(FILES XEglWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumXEglWindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Abstract X window context
if(NEED_ABSTRACTXWINDOWCONTEXT)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some contexts, was not found. Set WITH_*X*WINDOWCONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumAbstractXWindowContext OBJECT AbstractXWindowContext.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumAbstractXWindowContext PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES AbstractXWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()

# GLX context
if(NEED_GLXCONTEXT)
    add_library(MagnumGlxContextHandler OBJECT GlxContextHandler.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumGlxContextHandler PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES GlxContextHandler.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
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
    install(FILES EglContextHandler.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()
