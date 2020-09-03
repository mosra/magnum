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

find_package(Corrade REQUIRED PluginManager)

set(MagnumTrade_SRCS
    ArrayAllocator.cpp
    Data.cpp
    MeshObjectData2D.cpp
    MeshObjectData3D.cpp
    SceneData.cpp
    TextureData.cpp)

set(MagnumTrade_GracefulAssert_SRCS
    AbstractImageConverter.cpp
    AbstractImporter.cpp
    AbstractSceneConverter.cpp
    AnimationData.cpp
    CameraData.cpp
    FlatMaterialData.cpp
    ImageData.cpp
    LightData.cpp
    MaterialData.cpp
    MeshData.cpp
    ObjectData2D.cpp
    ObjectData3D.cpp
    PbrClearCoatMaterialData.cpp
    PbrMetallicRoughnessMaterialData.cpp
    PbrSpecularGlossinessMaterialData.cpp
    PhongMaterialData.cpp
    SkinData.cpp)

set(MagnumTrade_HEADERS
    AbstractImporter.h
    AbstractImageConverter.h
    AbstractSceneConverter.h
    AnimationData.h
    ArrayAllocator.h
    CameraData.h
    Data.h
    FlatMaterialData.h
    ImageData.h
    LightData.h
    MaterialData.h
    MaterialLayerData.h
    MeshData.h
    MeshObjectData2D.h
    MeshObjectData3D.h
    ObjectData2D.h
    ObjectData3D.h
    PbrClearCoatMaterialData.h
    PbrMetallicRoughnessMaterialData.h
    PbrSpecularGlossinessMaterialData.h
    PhongMaterialData.h
    SceneData.h
    SkinData.h
    TextureData.h
    Trade.h

    visibility.h)

set(MagnumTrade_PRIVATE_HEADERS
    Implementation/arrayUtilities.h
    Implementation/converterUtilities.h
    Implementation/materialAttributeProperties.hpp)

if(MAGNUM_BUILD_DEPRECATED)
    list(APPEND MagnumTrade_GracefulAssert_SRCS
        # These have to be here instead of in MagnumTrade_SRCS because they
        # include MeshData.h and call (and thus instantiate) various functions
        # with inline asserts. We need the linker to pick the variant with
        # graceful asserts for tests, and if there would be two different
        # copies, it may happen it picks the non-graceful-assert variant,
        # causing the tests to blow up. Happens only on the MSVC linker, but
        # let's be safe and do this everywhere.
        MeshData2D.cpp
        MeshData3D.cpp)
    list(APPEND MagnumTrade_HEADERS
        AbstractMaterialData.h

        MeshData2D.h
        MeshData3D.h)
endif()

if(NOT CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
endif()

# Objects shared between main and test library
add_library(MagnumTradeObjects OBJECT
    ${MagnumTrade_SRCS}
    ${MagnumTrade_HEADERS}
    ${MagnumTrade_PRIVATE_HEADERS})
target_include_directories(MagnumTradeObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Corrade::PluginManager,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumTradeObjects PRIVATE "MagnumTradeObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumTradeObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumTradeObjects PROPERTIES FOLDER "Magnum/Trade")

# Trade library
add_library(MagnumTrade ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumTradeObjects>
    ${MagnumTrade_GracefulAssert_SRCS})
set_target_properties(MagnumTrade PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Trade")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumTrade PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumTrade PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumTrade PUBLIC
    Magnum
    Corrade::PluginManager)

install(TARGETS MagnumTrade
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTrade_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Trade)

if(WITH_IMAGECONVERTER)
    find_package(Threads REQUIRED)

    add_executable(magnum-imageconverter imageconverter.cpp)
    target_link_libraries(magnum-imageconverter PRIVATE
        Magnum
        MagnumTrade
        # BasisImageConverter uses these, and linking pthread to just the
        # plugin doesn't work. See its documentation for details.
        Threads::Threads)
    set_target_properties(magnum-imageconverter PROPERTIES FOLDER "Magnum/Trade")

    install(TARGETS magnum-imageconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum imageconverter target alias for superprojects
    add_executable(Magnum::imageconverter ALIAS magnum-imageconverter)
endif()

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumTradeTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumTradeObjects>
        ${MagnumTrade_GracefulAssert_SRCS})
    target_compile_definitions(MagnumTradeTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumTrade_EXPORTS")
    set_target_properties(MagnumTradeTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumTradeTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumTradeTestLib
        Magnum
        Corrade::PluginManager)

    add_subdirectory(Test)
endif()

# Magnum Trade target alias for superprojects
add_library(Magnum::Trade ALIAS MagnumTrade)
