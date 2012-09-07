# Extension wrangler
add_library(MagnumContextsExtensionWrangler OBJECT ExtensionWrangler.cpp)

set(MagnumContexts_HEADERS
    AbstractContext.h
    AbstractWindowContext.h
    ExtensionWrangler.h)
install(FILES ${MagnumContexts_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)

# GLUT context
if(WITH_GLUTCONTEXT)
    find_package(GLUT)
    if(GLUT_FOUND)
        add_library(MagnumGlutWindowContext STATIC
            GlutWindowContext.cpp
            $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
        install(FILES GlutWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumGlutWindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "GLUT library, required by GlutWindowContext, was not found. Set WITH_GLUTCONTEXT to OFF to skip building it.")
    endif()
endif()

# SDL2 context
if(WITH_SDL2CONTEXT)
    find_package(SDL2)
    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIR})
        add_library(MagnumSdl2WindowContext STATIC
            Sdl2WindowContext.cpp
            $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
        install(FILES Sdl2WindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumSdl2WindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "SDL2 library, required by Sdl2WindowContext, was not found. Set WITH_SDL2CONTEXT to OFF to skip building it.")
    endif()
endif()

# GLX context
if(WITH_GLXCONTEXT)
    set(NEED_ABSTRACTXCONTEXT 1)
    set(NEED_GLXINTERFACE 1)
    add_library(MagnumGlxWindowContext STATIC
        $<TARGET_OBJECTS:MagnumAbstractXWindowContext>
        $<TARGET_OBJECTS:MagnumGlxContext>
        $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
    install(FILES GlxWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumGlxWindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# X/EGL context
if(WITH_XEGLCONTEXT)
    set(NEED_ABSTRACTXCONTEXT 1)
    set(NEED_EGLINTERFACE 1)
    add_library(MagnumXEglWindowContext STATIC
        $<TARGET_OBJECTS:MagnumAbstractXWindowContext>
        $<TARGET_OBJECTS:MagnumEglContext>
        $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
    install(FILES XEglWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumXEglWindowContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Abstract X context
if(NEED_ABSTRACTXCONTEXT)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some contexts, was not found. Set WITH_*X*CONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumAbstractXWindowContext OBJECT AbstractXWindowContext.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumAbstractXWindowContext PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES AbstractXWindowContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()

# GLX interface
if(NEED_GLXINTERFACE)
    add_library(MagnumGlxContext OBJECT GlxContext.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumGlxContext PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES GlxContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()

# EGL interface
if(NEED_EGLINTERFACE)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some contexts, was not found. Set WITH_*EGL*CONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumEglContext OBJECT EglContext.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumEglContext PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES EglContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()
