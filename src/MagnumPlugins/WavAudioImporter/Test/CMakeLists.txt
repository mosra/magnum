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

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(WAVAUDIOIMPORTER_TEST_DIR ".")
else()
    set(WAVAUDIOIMPORTER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# CMake before 3.8 has broken $<TARGET_FILE*> expressions for iOS (see
# https://gitlab.kitware.com/cmake/cmake/merge_requests/404) and since Corrade
# doesn't support dynamic plugins on iOS, this sorta works around that. Should
# be revisited when updating Travis to newer Xcode (xcode7.3 has CMake 3.6).
if(NOT MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
    set(WAVAUDIOIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:WavAudioImporter>)
endif()

# First replace ${} variables, then $<> generator expressions
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

corrade_add_test(WavAudioImporterTest WavImporterTest.cpp
    LIBRARIES MagnumAudio
    FILES
        unsupportedChannelCount.wav
        unsupportedFormat.wav
        wrongSignature.wav

        invalidPadding.wav
        invalidLength.wav
        invalidDataChunk.wav
        invalidFactChunk.wav

        zeroSamples.wav

        # The *be.wav files are created with artisanal bit-crafting from the
        # little-endian counterparts and a good portion of wishful thinking
        # (I can only hope those are correct), because ffpeg can't create them
        # on its own:
        #
        # $ ffmpeg -i mono16.wav -f wav -acodec pcm_s16be mono16be.wav
        # ...
        # [wav @ 0x7fbdd301a200] pcm_s32be codec not supported in WAVE format
        # Could not write header for output file #0 (incorrect codec parameters ?): Function not implemented

        mono4.wav
        mono8.wav
        mono8junk.wav
        mono8ALaw.wav
        mono8MuLaw.wav
        mono16.wav
        mono16be.wav
        mono24.wav

        stereo4.wav
        stereo8.wav
        stereo8ALaw.wav
        stereo8MuLaw.wav
        stereo12.wav
        stereo16.wav
        stereo24.wav
        stereo32.wav

        mono32f.wav
        mono32fbe.wav
        stereo32f.wav
        stereo64f.wav
        stereo64fbe.wav

        surround51Channel16.wav
        surround71Channel24.wav)
target_include_directories(WavAudioImporterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
if(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
    target_link_libraries(WavAudioImporterTest PRIVATE WavAudioImporter)
else()
    # So the plugins get properly built when building the test
    add_dependencies(WavAudioImporterTest WavAudioImporter)
endif()
corrade_add_test(WavAudioImporterWavHeaderTest
    WavHeaderTest.cpp
    $<TARGET_OBJECTS:WavAudioImporterObjects>
    LIBRARIES MagnumAudio)
set_target_properties(
    WavAudioImporterTest
    WavAudioImporterWavHeaderTest
    PROPERTIES FOLDER "MagnumPlugins/WavAudioImporter/Test")
if(CORRADE_BUILD_STATIC AND NOT MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
    # CMake < 3.4 does this implicitly, but 3.4+ not anymore (see CMP0065).
    # That's generally okay, *except if* the build is static, the executable
    # uses a plugin manager and needs to share globals with the plugins (such
    # as output redirection and so on).
    set_target_properties(WavAudioImporterTest PROPERTIES ENABLE_EXPORTS ON)
endif()
