# Extension wrangler
add_library(MagnumContextsExtensionWrangler OBJECT ExtensionWrangler.cpp)

set(MagnumContexts_HEADERS
    AbstractContext.h
    AbstractGlInterface.h
    ExtensionWrangler.h)
install(FILES ${MagnumContexts_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)

# GLUT context
if(WITH_GLUTCONTEXT)
    find_package(GLUT)
    if(GLUT_FOUND)
        add_library(MagnumGlutContext STATIC
            GlutContext.cpp
            $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
        install(FILES GlutContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumGlutContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "GLUT library, required by GlutContext, was not found. Set WITH_GLUTCONTEXT to OFF to skip building it.")
    endif()
endif()

# SDL2 context
if(WITH_SDL2CONTEXT)
    find_package(SDL2)
    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIR})
        add_library(MagnumSdl2Context STATIC
            Sdl2Context.cpp
            $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
        install(FILES Sdl2Context.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumSdl2Context DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(FATAL_ERROR "SDL2 library, required by Sdl2Context, was not found. Set WITH_SDL2CONTEXT to OFF to skip building it.")
    endif()
endif()

# X/EGL context
if(WITH_XEGLCONTEXT)
    set(NEED_ABSTRACTXCONTEXT 1)
    set(NEED_EGLINTERFACE 1)
    add_library(MagnumXEglContext STATIC
        $<TARGET_OBJECTS:MagnumAbstractXContext>
        $<TARGET_OBJECTS:MagnumEglInterface>
        $<TARGET_OBJECTS:MagnumContextsExtensionWrangler>)
    install(FILES XEglContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumXEglContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
endif()

# Abstract X context
if(NEED_ABSTRACTXCONTEXT)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 library, required by some contexts, was not found. Set WITH_*X*CONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumAbstractXContext OBJECT AbstractXContext.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumAbstractXContext PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES AbstractXContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()

# EGL interface
if(NEED_EGLINTERFACE)
    find_package(EGL)
    if(NOT EGL_FOUND)
        message(FATAL_ERROR "EGL library, required by some contexts, was not found. Set WITH_*EGL*CONTEXT to OFF to skip building them.")
    endif()
    add_library(MagnumEglInterface OBJECT EglInterface.cpp)
    # X11 macros are a mess, disable warnings for C-style casts
    set_target_properties(MagnumEglInterface PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
    install(FILES EglInterface.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
endif()
