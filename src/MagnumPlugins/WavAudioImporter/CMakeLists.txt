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

find_package(OpenAL REQUIRED)

include_directories(${OPENAL_INCLUDE_DIR})

set(WavAudioImporter_SRCS
    WavImporter.cpp)

set(WavAudioImporter_HEADERS
    WavHeader.h
    WavImporter.h)

add_library(WavAudioImporterObjects OBJECT ${WavAudioImporter_SRCS})
set_target_properties(WavAudioImporterObjects PROPERTIES COMPILE_FLAGS "-DWavAudioImporterObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

add_plugin(WavAudioImporter ${MAGNUM_PLUGINS_AUDIOIMPORTER_INSTALL_DIR}
    WavAudioImporter.conf
    $<TARGET_OBJECTS:WavAudioImporterObjects>
    pluginRegistration.cpp)
target_link_libraries(WavAudioImporter Magnum MagnumAudio)

install(FILES ${WavAudioImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)

if(BUILD_TESTS)
    add_library(WavAudioImporterTestLib STATIC $<TARGET_OBJECTS:WavAudioImporterObjects>)
    target_link_libraries(WavAudioImporterTestLib Magnum MagnumAudio)

    # On Windows we need to install first and then run the tests to avoid "DLL
    # not found" hell, thus we need to install this too
    if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
        install(TARGETS WavAudioImporterTestLib
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    endif()

    add_subdirectory(Test)
endif()
