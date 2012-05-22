set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -fvisibility=hidden")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CORRADE_INCLUDE_DIR})

# On 64bit Linux with GCC flag -fPIC is needed for building some static libraries
if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_COMPILER_IS_GNUCC AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(USE_FPIC ON)
else()
    set(USE_FPIC OFF)
endif()

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

if(USE_FPIC)
    set_target_properties(MagnumObjects PROPERTIES COMPILE_FLAGS -fPIC)
    set_target_properties(MagnumMathObjects PROPERTIES COMPILE_FLAGS -fPIC)
endif()

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
target_link_libraries(Magnum ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
install(TARGETS Magnum DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

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
    target_link_libraries(MagnumTestLib ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

    include_directories(${QT_INCLUDE_DIR})
    add_subdirectory(Test)
endif()
