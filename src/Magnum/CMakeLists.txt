#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

# Generate configure header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# Generate version header. If Git is found and this is a Git working copy,
# extract values from there, otherwise use just MAGNUM_VERSION_YEAR/MONTH that
# are set in project root CMakeLists.
find_package(Git)
if(Git_FOUND)
    # Match only tags starting with `v`, always use the long format so we have
    # a commit hash also on a tagged version
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --match "v*" --long
        OUTPUT_VARIABLE MAGNUM_VERSION_STRING
        RESULT_VARIABLE _MAGNUM_VERSION_RESULT
        # Otherwise this gets executed in the build dir, which might be inside
        # a totally different Git working copy
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(MAGNUM_VERSION_STRING MATCHES "^v([0-9][0-9][0-9][0-9])\\.0?([0-9][0-9]?)-([0-9]+)-g([a-f0-9]+)$")
        set(MAGNUM_VERSION_YEAR ${CMAKE_MATCH_1})
        set(MAGNUM_VERSION_MONTH ${CMAKE_MATCH_2})
        set(MAGNUM_VERSION_COMMIT ${CMAKE_MATCH_3})
        # Otherwise if commit is 0, it would #undef the variable
        set(MAGNUM_VERSION_COMMIT " ${MAGNUM_VERSION_COMMIT}")
        set(MAGNUM_VERSION_HASH ${CMAKE_MATCH_4})
    elseif(_MAGNUM_VERSION_RESULT EQUAL 0)
        message(WARNING "Can't match Git version from ${MAGNUM_VERSION_STRING}")
    endif()
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# Files shared between main library and unit test library
set(Magnum_SRCS
    FileCallback.cpp
    PixelStorage.cpp
    Resource.cpp
    Sampler.cpp
    Timeline.cpp)

set(Magnum_GracefulAssert_SRCS
    Image.cpp
    ImageView.cpp
    Mesh.cpp
    PixelFormat.cpp
    VertexFormat.cpp

    Animation/Player.cpp
    Animation/Interpolation.cpp)

set(Magnum_HEADERS
    AbstractResourceLoader.h
    Array.h
    British.h
    DimensionTraits.h
    FileCallback.h
    Image.h
    ImageView.h
    Magnum.h
    Mesh.h
    PixelFormat.h
    PixelStorage.h
    Resource.h
    ResourceManager.h
    Sampler.h
    Tags.h
    Timeline.h
    Types.h
    VertexFormat.h
    visibility.h)

set(Magnum_PRIVATE_HEADERS
    Implementation/ImageProperties.h

    Implementation/converterUtilities.h
    Implementation/meshIndexTypeMapping.hpp
    Implementation/meshPrimitiveMapping.hpp
    Implementation/compressedPixelFormatMapping.hpp
    Implementation/pixelFormatMapping.hpp
    Implementation/vertexFormatMapping.hpp)

# Functionality specific to static Windows builds
if(CORRADE_TARGET_WINDOWS AND NOT CORRADE_TARGET_WINDOWS_RT AND MAGNUM_BUILD_STATIC)
    list(APPEND Magnum_SRCS Implementation/WindowsWeakSymbol.cpp)
    list(APPEND Magnum_PRIVATE_HEADERS Implementation/WindowsWeakSymbol.h)
endif()

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Angle.cpp
    Math/Color.cpp
    Math/Half.cpp
    Math/Packing.cpp
    Math/instantiation.cpp)

set(MagnumMath_GracefulAssert_SRCS
    Math/Functions.cpp
    Math/PackingBatch.cpp)

# Objects shared between main and math test library
add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})
target_include_directories(MagnumMathObjects PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    $<TARGET_PROPERTY:Corrade::Utility,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumMathObjects PRIVATE "MagnumMathObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumMathObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumMathObjects PROPERTIES FOLDER "Magnum/Math")

# Objects shared between main and test library
add_library(MagnumObjects OBJECT
    ${Magnum_SRCS}
    ${Magnum_HEADERS}
    ${Magnum_PRIVATE_HEADERS})
target_include_directories(MagnumObjects PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    $<TARGET_PROPERTY:Corrade::Utility,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumObjects PRIVATE "MagnumObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumObjects PROPERTIES FOLDER "Magnum")

# Main library
add_library(Magnum ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumMathObjects>
    $<TARGET_OBJECTS:MagnumObjects>
    ${MagnumMath_GracefulAssert_SRCS}
    ${Magnum_GracefulAssert_SRCS})
set_target_properties(Magnum PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum")
if(NOT BUILD_STATIC)
    set_target_properties(Magnum PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(Magnum PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(Magnum PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
target_link_libraries(Magnum PUBLIC
    Corrade::Utility)

install(TARGETS Magnum
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${Magnum_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
    DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

add_subdirectory(Animation)
add_subdirectory(Math)
add_subdirectory(Platform)

if(WITH_AUDIO)
    add_subdirectory(Audio)
endif()

if(WITH_DEBUGTOOLS)
    add_subdirectory(DebugTools)
endif()

if(WITH_GL)
    add_subdirectory(GL)
endif()

if(WITH_MESHTOOLS)
    add_subdirectory(MeshTools)
endif()

if(WITH_PRIMITIVES)
    add_subdirectory(Primitives)
endif()

if(WITH_SCENEGRAPH)
    add_subdirectory(SceneGraph)
endif()

if(WITH_SHADERS)
    add_subdirectory(Shaders)
endif()

if(WITH_SHADERTOOLS)
    add_subdirectory(ShaderTools)
endif()

if(WITH_TEXT)
    add_subdirectory(Text)
endif()

if(WITH_TEXTURETOOLS)
    add_subdirectory(TextureTools)
endif()

if(WITH_TRADE)
    add_subdirectory(Trade)
endif()

if(WITH_VK)
    add_subdirectory(Vk)
endif()

if(BUILD_TESTS)
    # Math library with graceful assert for testing
    add_library(MagnumMathTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumMathObjects>
        ${MagnumMath_GracefulAssert_SRCS})
    target_include_directories(MagnumMathTestLib PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src)
    target_compile_definitions(MagnumMathTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "Magnum_EXPORTS")
    set_target_properties(MagnumMathTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Math")
    target_link_libraries(MagnumMathTestLib Corrade::Utility)

    # Library with graceful assert for testing
    add_library(MagnumTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumMathObjects>
        $<TARGET_OBJECTS:MagnumObjects>
        ${Magnum_GracefulAssert_SRCS})
    target_include_directories(MagnumTestLib PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src)
    target_compile_definitions(MagnumTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "Magnum_EXPORTS")
    set_target_properties(MagnumTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumTestLib PUBLIC Corrade::Utility)

    add_subdirectory(Test)
endif()

# Magnum library target alias and configure file for superprojects
add_library(Magnum::Magnum ALIAS Magnum)
set(_MAGNUM_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/configure.h CACHE INTERNAL "")
