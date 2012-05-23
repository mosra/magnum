install(FILES AbstractContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)

# GLUT context
find_package(GLUT)
if(GLUT_FOUND)
    add_library(MagnumGlutContext STATIC GlutContext.cpp)
    install(FILES GlutContext.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Contexts)
    install(TARGETS MagnumGlutContext DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
else()
    message(STATUS "GLUT library was not found. GLUT context library will not be generated.")
endif()
