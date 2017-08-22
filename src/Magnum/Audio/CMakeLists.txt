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

find_package(OpenAL REQUIRED)

set(MagnumAudio_SRCS
    AbstractImporter.cpp
    Audio.cpp
    Buffer.cpp
    Context.cpp
    Renderer.cpp
    Source.cpp)

set(MagnumAudio_HEADERS
    AbstractImporter.h
    Audio.h
    Buffer.h
    Context.h
    Extensions.h
    Renderer.h
    Source.h

    visibility.h)

if(WITH_SCENEGRAPH)
    list(APPEND MagnumAudio_HEADERS
        Listener.h
        Playable.h
        PlayableGroup.h)

    list(APPEND MagnumAudio_SRCS
        Listener.cpp)
endif()

# Audio library
add_library(MagnumAudio ${SHARED_OR_STATIC}
    ${MagnumAudio_SRCS}
    ${MagnumAudio_HEADERS})
target_include_directories(MagnumAudio PUBLIC ${OPENAL_INCLUDE_DIR})
set_target_properties(MagnumAudio PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Audio")
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumAudio PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumAudio Magnum Corrade::PluginManager ${OPENAL_LIBRARY})
if(WITH_SCENEGRAPH)
    target_link_libraries(MagnumAudio MagnumSceneGraph)
endif()

install(TARGETS MagnumAudio
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumAudio_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Audio)

# Magnum Audio target alias for superprojects
add_library(Magnum::Audio ALIAS MagnumAudio)

if(WITH_AL_INFO)
    add_executable(magnum-al-info magnum-al-info.cpp)
    target_link_libraries(magnum-al-info MagnumAudio)
    set_target_properties(magnum-al-info PROPERTIES FOLDER "Magnum/Audio")

    if(NOT MAGNUM_TARGET_EMSCRIPTEN)
        install(TARGETS magnum-al-info DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
    endif()

    if(CORRADE_TARGET_IOS)
        set_target_properties(magnum-al-info PROPERTIES
            MACOSX_BUNDLE ON
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    elseif(CORRADE_TARGET_EMSCRIPTEN)
        install(FILES magnum-al-info.html DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info RENAME index.html)
        install(TARGETS magnum-al-info DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info)
        install(FILES
            ${CMAKE_SOURCE_DIR}/src/Magnum/Platform/WebApplication.css
            ${CMAKE_SOURCE_DIR}/src/Magnum/Platform/WindowlessEmscriptenApplication.js
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info)
        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/magnum-al-info.js.mem
            ${CMAKE_CURRENT_BINARY_DIR}/magnum-al-info.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info OPTIONAL)
    endif()

    # Magnum AL info target alias for superprojects
    add_executable(Magnum::al-info ALIAS magnum-al-info)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
