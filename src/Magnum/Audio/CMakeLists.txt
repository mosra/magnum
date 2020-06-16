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
find_package(OpenAL REQUIRED)

set(MagnumAudio_SRCS
    Audio.cpp
    Buffer.cpp
    BufferFormat.cpp
    Context.cpp
    Renderer.cpp
    Source.cpp)

set(MagnumAudio_GracefulAssert_SRCS
    AbstractImporter.cpp)

set(MagnumAudio_HEADERS
    AbstractImporter.h
    Audio.h
    Buffer.h
    BufferFormat.h
    Context.h
    Extensions.h
    Renderer.h
    Source.h

    visibility.h)

if(NOT CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
endif()

if(WITH_SCENEGRAPH)
    list(APPEND MagnumAudio_HEADERS
        Listener.h
        Playable.h
        PlayableGroup.h)

    list(APPEND MagnumAudio_SRCS
        Listener.cpp
        Playable.cpp
        PlayableGroup.cpp)
endif()

# Objects shared between main and test library
add_library(MagnumAudioObjects OBJECT
    ${MagnumAudio_SRCS}
    ${MagnumAudio_HEADERS})
target_include_directories(MagnumAudioObjects PUBLIC
    $<TARGET_PROPERTY:OpenAL::OpenAL,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Corrade::PluginManager,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumAudioObjects PRIVATE "MagnumAudioObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumAudioObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumAudioObjects PROPERTIES FOLDER "Magnum/Audio")

# Audio library
add_library(MagnumAudio ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumAudioObjects>
    ${MagnumAudio_GracefulAssert_SRCS})
target_include_directories(MagnumAudio PUBLIC ${OPENAL_INCLUDE_DIR})
set_target_properties(MagnumAudio PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Audio")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumAudio PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumAudio PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumAudio PUBLIC
    Magnum
    Corrade::PluginManager
    OpenAL::OpenAL)
if(WITH_SCENEGRAPH)
    target_link_libraries(MagnumAudio PUBLIC MagnumSceneGraph)
endif()

install(TARGETS MagnumAudio
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumAudio_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Audio)

# Copy the OpenAL DLL next to the place where all executables are stored to
# help people running the apps.
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CORRADE_TARGET_WINDOWS AND (OPENAL_DLL_DEBUG OR OPENAL_DLL_RELEASE))
    # Not exactly sure why, but in the OpenAL Soft binary distribution it's
    # named soft_oal.dll and needs to be renamed to OpenAL32.dll to work
    # correctly. Not the case with OpenAL Soft built by hand.
    if(OPENAL_DLL_DEBUG AND OPENAL_DLL_RELEASE)
        set(openal_dll_input $<$<CONFIG:Debug>:${OPENAL_DLL_DEBUG}>$<$<NOT:$<CONFIG:Debug>>:${OPENAL_DLL_RELEASE}>)
    elseif(OPENAL_DLL_DEBUG)
        set(openal_dll_input ${OPENAL_DLL_DEBUG})
    else()
        set(openal_dll_input ${OPENAL_DLL_RELEASE})
    endif()
    add_custom_command(TARGET MagnumAudio POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${openal_dll_input} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OpenAL32.dll)
endif()

# Magnum Audio target alias for superprojects
add_library(Magnum::Audio ALIAS MagnumAudio)

if(WITH_AL_INFO)
    add_executable(magnum-al-info al-info.cpp)
    target_link_libraries(magnum-al-info PRIVATE MagnumAudio)
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
        add_custom_command(TARGET magnum-al-info POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/WebApplication.css
                ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/WindowlessEmscriptenApplication.js
                $<TARGET_FILE_DIR:magnum-al-info>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/al-info.html
                $<TARGET_FILE_DIR:magnum-al-info>/magnum-al-info.html)
        install(FILES al-info.html DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info RENAME index.html)
        install(TARGETS magnum-al-info DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info)
        install(FILES
            ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/WebApplication.css
            ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/WindowlessEmscriptenApplication.js
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info)
        install(FILES
            $<TARGET_FILE_DIR:magnum-al-info>/magnum-al-info.js.mem
            $<TARGET_FILE_DIR:magnum-al-info>/magnum-al-info.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX}/magnum-al-info OPTIONAL)
    endif()

    # Magnum AL info target alias for superprojects
    add_executable(Magnum::al-info ALIAS magnum-al-info)
endif()

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumAudioTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumAudioObjects>
        ${MagnumAudio_GracefulAssert_SRCS})
    target_compile_definitions(MagnumAudioTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumAudio_EXPORTS")
    target_include_directories(MagnumAudioTestLib PUBLIC ${OPENAL_INCLUDE_DIR})
    set_target_properties(MagnumAudioTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumAudioTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumAudioTestLib PUBLIC
        Magnum
        Corrade::PluginManager
        OpenAL::OpenAL)
    if(WITH_SCENEGRAPH)
        target_link_libraries(MagnumAudioTestLib PUBLIC MagnumSceneGraph)
    endif()

    add_subdirectory(Test)
endif()
