set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -fvisibility=hidden")

if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/magnumConfigure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CORRADE_INCLUDE_DIR})

# Files shared between main library and unit test library
set(Magnum_SRCS
    AbstractImage.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    BufferedTexture.cpp
    Camera.cpp
    Framebuffer.cpp
    IndexedMesh.cpp
    Light.cpp
    Mesh.cpp
    Query.cpp
    Renderbuffer.cpp
    Shader.cpp
    SizeTraits.cpp
    TypeTraits.cpp

    Trade/AbstractImporter.cpp
    Trade/MeshData.cpp
)
add_library(MagnumObjects OBJECT ${Magnum_SRCS})

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Math.cpp
)
add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
set_target_properties(MagnumObjects MagnumMathObjects PROPERTIES COMPILE_FLAGS "-DMagnumObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

# Files compiled with different flags for main library and unit test library
set(Magnum_GracefulAssert_SRCS
    Object.cpp
)

# Main library
add_library(Magnum SHARED
    $<TARGET_OBJECTS:MagnumObjects>
    $<TARGET_OBJECTS:MagnumMathObjects>
    ${Magnum_GracefulAssert_SRCS}
)
target_link_libraries(Magnum ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
if(NOT TARGET_GLES)
    target_link_libraries(Magnum ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
else()
    target_link_libraries(Magnum ${OPENGLES2_LIBRARY})
endif()
install(TARGETS Magnum DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

# Install also configure file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

add_subdirectory(Contexts)
add_subdirectory(Math)
add_subdirectory(MeshTools)
add_subdirectory(Physics)
add_subdirectory(Primitives)
add_subdirectory(Shaders)

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    add_library(MagnumTestLib SHARED
        $<TARGET_OBJECTS:MagnumObjects>
        ${Magnum_GracefulAssert_SRCS}
    )
    set_target_properties(MagnumTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumTestLib ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
    if(NOT TARGET_GLES)
        target_link_libraries(MagnumTestLib ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
    else()
        target_link_libraries(MagnumTestLib ${OPENGLES2_LIBRARY})
    endif()

    include_directories(${QT_INCLUDE_DIR})
    add_subdirectory(Test)
endif()
