include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(Math)

set(Magnum_SRCS
    AbstractObject.cpp
    AbstractShaderProgram.cpp
    Camera.cpp
    Mesh.cpp
    Scene.cpp
    Shader.cpp
)

add_library(Magnum SHARED ${Magnum_SRCS})
target_link_libraries(Magnum ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
