#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2015 Jonathan Hale <squareys@googlemail.com>
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

if(CORRADE_TARGET_EMSCRIPTEN)
    set(AUDIO_TEST_DIR "")
else()
    set(AUDIO_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_test(AudioAbstractImporterTest AbstractImporterTest.cpp
    LIBRARIES MagnumAudioTestLib
    FILES file.bin)
target_include_directories(AudioAbstractImporterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
corrade_add_test(AudioBufferFormatTest BufferFormatTest.cpp LIBRARIES MagnumAudio)
corrade_add_test(AudioContextTest ContextTest.cpp LIBRARIES MagnumAudio)
corrade_add_test(AudioRendererTest RendererTest.cpp LIBRARIES MagnumAudio)
corrade_add_test(AudioSourceTest SourceTest.cpp LIBRARIES MagnumAudio)

set_target_properties(
    AudioAbstractImporterTest
    AudioBufferFormatTest
    AudioContextTest
    AudioRendererTest
    AudioSourceTest
    PROPERTIES FOLDER "Magnum/Audio/Test")

if(BUILD_AL_TESTS)
    corrade_add_test(AudioBufferALTest BufferALTest.cpp LIBRARIES MagnumAudio)
    corrade_add_test(AudioContextALTest ContextALTest.cpp LIBRARIES MagnumAudio)
    corrade_add_test(AudioRendererALTest RendererALTest.cpp LIBRARIES MagnumAudio)
    corrade_add_test(AudioSourceALTest SourceALTest.cpp LIBRARIES MagnumAudio)

    set_target_properties(
        AudioBufferALTest
        AudioContextALTest
        AudioRendererALTest
        AudioSourceALTest
        PROPERTIES FOLDER "Magnum/Audio/Test")

    if(WITH_SCENEGRAPH)
        corrade_add_test(AudioListenerALTest ListenerALTest.cpp LIBRARIES MagnumSceneGraph MagnumAudio)
        corrade_add_test(AudioPlayableALTest PlayableALTest.cpp LIBRARIES MagnumSceneGraph MagnumAudio)

        set_target_properties(
            AudioListenerALTest
            AudioPlayableALTest
            PROPERTIES FOLDER "Magnum/Audio/Test")
    endif()

    if(MAGNUM_BUILD_STATIC AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_WINDOWS_RT)
        add_library(AudioGlobalStateAcrossLibrariesLibrary SHARED
            GlobalStateAcrossLibrariesLibrary.cpp)
        target_link_libraries(AudioGlobalStateAcrossLibrariesLibrary PUBLIC MagnumAudio)

        corrade_add_test(AudioGlobalStateAcrossLibra___ALTest
            GlobalStateAcrossLibrariesALTest.cpp
            LIBRARIES AudioGlobalStateAcrossLibrariesLibrary)
        set_target_properties(
            AudioGlobalStateAcrossLibrariesLibrary
            AudioGlobalStateAcrossLibra___ALTest
            PROPERTIES FOLDER "Magnum/Audio/Test")
    endif()
endif()
