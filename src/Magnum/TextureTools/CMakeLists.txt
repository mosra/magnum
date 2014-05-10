#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
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

corrade_add_resource(MagnumTextureTools_RCS resources.conf)

set(MagnumTextureTools_SRCS
    Atlas.cpp
    DistanceField.cpp
    ${MagnumTextureTools_RCS})

set(MagnumTextureTools_HEADERS
    Atlas.h
    DistanceField.h

    magnumTextureToolsVisibility.h)

if(BUILD_STATIC)
    set(MagnumTextureTools_HEADERS ${MagnumTextureTools_HEADERS} magnumTextureToolsResourceImport.hpp)
endif()

add_library(MagnumTextureTools ${SHARED_OR_STATIC} ${MagnumTextureTools_SRCS})
set_target_properties(MagnumTextureTools PROPERTIES DEBUG_POSTFIX "-d")
if(BUILD_STATIC_PIC)
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumTextureTools PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
endif()
target_link_libraries(MagnumTextureTools Magnum)

if(WITH_DISTANCEFIELDCONVERTER)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/distancefieldconverterConfigure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/distancefieldconverterConfigure.h)

    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    add_executable(magnum-distancefieldconverter distancefieldconverter.cpp)

    if(UNIX AND NOT TARGET_GLES)
        target_link_libraries(magnum-distancefieldconverter MagnumTextureTools Magnum MagnumWindowlessGlxApplication ${X11_LIBRARIES})
    elseif(WIN32)
        target_link_libraries(magnum-distancefieldconverter MagnumTextureTools MagnumWindowlessWglApplication Magnum)
    else()
        message(FATAL_ERROR "magnum-distancefieldconverter is not available on this platform. Set WITH_DISTANCEFIELDCONVERTER to OFF to suppress this warning.")
    endif()

    install(TARGETS magnum-distancefieldconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
endif()

install(TARGETS MagnumTextureTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTextureTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/TextureTools)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
