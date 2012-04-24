set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CORRADE_INCLUDE_DIR})

add_subdirectory(Math)
add_subdirectory(MeshTools)
add_subdirectory(Physics)
add_subdirectory(Primitives)
add_subdirectory(Shaders)

set(Magnum_SRCS
    Object.cpp
    AbstractImage.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    Camera.cpp
    Framebuffer.cpp
    IndexedMesh.cpp
    Light.cpp
    Mesh.cpp
    Query.cpp
    Renderbuffer.cpp
    Scene.cpp
    Shader.cpp
    SizeTraits.cpp
    TypeTraits.cpp

    Trade/AbstractImporter.cpp
    Trade/MeshData.cpp

    Math/Math.cpp
)

add_library(Magnum SHARED ${Magnum_SRCS})

if(WIN32)
    set_target_properties(Magnum PROPERTIES COMPILE_FLAGS -DMAGNUM_EXPORTING)
endif()

target_link_libraries(Magnum ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

install(TARGETS Magnum DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    add_library(MagnumTestLib SHARED ${Magnum_SRCS})
    if(WIN32)
        set_target_properties(MagnumTestLib PROPERTIES COMPILE_FLAGS -DMAGNUM_EXPORTING)
    endif()
    set_target_properties(MagnumTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumTestLib ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

    include_directories(${QT_INCLUDE_DIR})
    add_subdirectory(Test)
endif()
