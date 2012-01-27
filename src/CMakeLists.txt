set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CORRADE_INCLUDE_DIR})

add_subdirectory(Math)
add_subdirectory(MeshTools)
add_subdirectory(Primitives)

set(Magnum_SRCS
    Object.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    Camera.cpp
    IndexedMesh.cpp
    Mesh.cpp
    Scene.cpp
    Shader.cpp
    SizeTraits.cpp
    Texture.cpp

    Trade/AbstractImporter.cpp

    Math/Math.cpp
)

add_library(Magnum SHARED ${Magnum_SRCS})
target_link_libraries(Magnum CorradePluginManager CorradeUtility ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

install(TARGETS Magnum DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
