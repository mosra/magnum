#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
#             Vladimír Vondruš <mosra@centrum.cz>
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

set(MagnumTrade_HEADERS
    AbstractImporter.h
    AbstractImageConverter.h
    AbstractMaterialData.h
    CameraData.h
    ImageData.h
    LightData.h
    MeshData2D.h
    MeshData3D.h
    MeshObjectData2D.h
    MeshObjectData3D.h
    ObjectData2D.h
    ObjectData3D.h
    PhongMaterialData.h
    SceneData.h
    TextureData.h
    Trade.h)

# Force IDEs to display all header files in project view
add_custom_target(MagnumTrade SOURCES ${MagnumTrade_HEADERS})
set_target_properties(MagnumTrade PROPERTIES FOLDER "Magnum/Trade")

install(FILES ${MagnumTrade_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Trade)

if(WITH_IMAGECONVERTER)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/imageconverterConfigure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/imageconverterConfigure.h)

    add_executable(magnum-imageconverter imageconverter.cpp)
    target_include_directories(magnum-imageconverter PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(magnum-imageconverter Magnum)
    set_target_properties(magnum-imageconverter PROPERTIES FOLDER "Magnum/Trade")

    install(TARGETS magnum-imageconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum imageconverter target alias for superprojects
    add_executable(Magnum::imageconverter ALIAS magnum-imageconverter)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
