install(FILES AbstractContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)

# GLUT context
if(NOT TARGET_GLES)
    find_package(GLUT)
    if(GLUT_FOUND)
        add_library(MagnumGlutContext STATIC GlutContext.cpp)
        install(FILES GlutContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
        install(TARGETS MagnumGlutContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    else()
        message(WARNING "GLUT library was not found. GLUT context library will not be generated.")
    endif()
endif()

# SDL2 context
find_package(SDL2)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    add_library(MagnumSdl2Context STATIC Sdl2Context.cpp)
    install(FILES Sdl2Context.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumSdl2Context DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
else()
    message(WARNING "SDL2 library was not found. SDL2 context library will not be generated.")
endif()
