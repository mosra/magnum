#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

if(CORRADE_TARGET_EMSCRIPTEN)
    set(WAVAUDIOIMPORTER_TEST_DIR "")
else()
    set(WAVAUDIOIMPORTER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_test(WavAudioImporterTest WavImporterTest.cpp LIBRARIES MagnumWavAudioImporterTestLib)
target_include_directories(WavAudioImporterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# On Win32 we need to avoid dllimporting WavAudioImporter symbols, because it
# would search for the symbols in some DLL even though they were linked
# statically. However it apparently doesn't matter that they were dllexported
# when building the static library. EH.
if(WIN32)
    target_compile_definitions(WavAudioImporterTest PRIVATE "MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC")
endif()

if(CORRADE_TARGET_EMSCRIPTEN)
    emscripten_embed_file(WavAudioImporterTest unsupportedChannelCount.wav "/unsupportedChannelCount.wav")
    emscripten_embed_file(WavAudioImporterTest unsupportedFormat.wav "/unsupportedFormat.wav")
    emscripten_embed_file(WavAudioImporterTest wrongSignature.wav "/wrongSignature.wav")

    emscripten_embed_file(WavAudioImporterTest invalidPadding.wav "/invalidPadding.wav")
    emscripten_embed_file(WavAudioImporterTest invalidLength.wav "/invalidLength.wav")
    emscripten_embed_file(WavAudioImporterTest invalidDataChunk.wav "/invalidDataChunk.wav")
    emscripten_embed_file(WavAudioImporterTest invalidFactChunk.wav "/invalidFactChunk.wav")

    emscripten_embed_file(WavAudioImporterTest mono8.wav "/mono8.wav")
    emscripten_embed_file(WavAudioImporterTest mono8.wav "/mono8.wav")
    emscripten_embed_file(WavAudioImporterTest mono8junk.wav "/mono8junk.wav")
    emscripten_embed_file(WavAudioImporterTest mono8ALaw.wav "/mono8ALaw.wav")
    emscripten_embed_file(WavAudioImporterTest mono8MuLaw.wav "/mono8MuLaw.wav")
    emscripten_embed_file(WavAudioImporterTest mono16.wav "/mono16.wav")
    emscripten_embed_file(WavAudioImporterTest mono24.wav "/mono24.wav")

    emscripten_embed_file(WavAudioImporterTest stereo4.wav "/stereo4.wav")
    emscripten_embed_file(WavAudioImporterTest stereo8.wav "/stereo8.wav")
    emscripten_embed_file(WavAudioImporterTest stereo8ALaw.wav "/stereo8ALaw.wav")
    emscripten_embed_file(WavAudioImporterTest stereo8MuLaw.wav "/stereo8MuLaw.wav")
    emscripten_embed_file(WavAudioImporterTest stereo12.wav "/stereo12.wav")
    emscripten_embed_file(WavAudioImporterTest stereo16.wav "/stereo16.wav")
    emscripten_embed_file(WavAudioImporterTest stereo24.wav "/stereo24.wav")
    emscripten_embed_file(WavAudioImporterTest stereo32.wav "/stereo32.wav")

    emscripten_embed_file(WavAudioImporterTest mono32f.wav "/mono32f.wav")
    emscripten_embed_file(WavAudioImporterTest stereo32f.wav "/stereo32f.wav")
    emscripten_embed_file(WavAudioImporterTest stereo64f.wav "/stereo64f.wav")

    emscripten_embed_file(WavAudioImporterTest surround51Channel16.wav "/surround51Channel16.wav")
    emscripten_embed_file(WavAudioImporterTest surround71Channel24.wav "/surround71Channel24.wav")
endif()
