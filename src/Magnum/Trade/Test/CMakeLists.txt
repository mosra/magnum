#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/Trade/Test")

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(TRADE_TEST_DIR ".")
    set(TRADE_TEST_OUTPUT_DIR "./write")
else()
    set(TRADE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(TRADE_TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Executable testing is implemented on Unix platforms only at the moment, so
# don't even provide the filename elsewhere.
if(MAGNUM_WITH_IMAGECONVERTER AND CORRADE_TARGET_UNIX)
    set(IMAGECONVERTER_EXECUTABLE_FILENAME $<TARGET_FILE:magnum-imageconverter>)
endif()
if(NOT MAGNUM_BUILD_PLUGINS_STATIC)
    if(MAGNUM_WITH_ANYIMAGEIMPORTER AND NOT MAGNUM_ANYIMAGEIMPORTER_BUILD_STATIC)
        set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
    endif()
    if(MAGNUM_WITH_ANYIMAGECONVERTER AND NOT MAGNUM_ANYIMAGECONVERTER_BUILD_STATIC)
        set(ANYIMAGECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageConverter>)
    endif()
    if(MAGNUM_WITH_TGAIMAGECONVERTER AND NOT MAGNUM_TGAIMAGECONVERTER_BUILD_STATIC)
        set(TGAIMAGECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImageConverter>)
    endif()
endif()

# First replace ${} variables, then $<> generator expressions
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

corrade_add_test(TradeAbstractImageConverterTest AbstractImageConverterTest.cpp LIBRARIES MagnumTradeTestLib)
target_include_directories(TradeAbstractImageConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

corrade_add_test(TradeAbstractImporterTest AbstractImporterTest.cpp
    LIBRARIES MagnumTradeTestLib
    FILES file.bin)
target_include_directories(TradeAbstractImporterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

corrade_add_test(TradeAbstractSceneConverterTest AbstractSceneConverterTest.cpp
    LIBRARIES MagnumTradeTestLib)
target_include_directories(TradeAbstractSceneConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

corrade_add_test(TradeAnimationDataTest AnimationDataTest.cpp LIBRARIES MagnumTradeTestLib)
# In Emscripten 3.1.27, the stack size was reduced from 5 MB (!) to 64 kB:
#   https://github.com/emscripten-core/emscripten/pull/18191
# One case in this test uses 65 kB of stack space, need to increase it a bit to
# not make it crash.
if(CORRADE_TARGET_EMSCRIPTEN AND NOT EMSCRIPTEN_VERSION VERSION_LESS 3.1.27)
    set_property(TARGET TradeAnimationDataTest APPEND_STRING PROPERTY LINK_FLAGS " -s STACK_SIZE=128kB")
endif()

corrade_add_test(TradeCameraDataTest CameraDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradeDataTest DataTest.cpp LIBRARIES MagnumTrade)
corrade_add_test(TradeFlatMaterialDataTest FlatMaterialDataTest.cpp LIBRARIES MagnumTradeTestLib)

corrade_add_test(TradeImageConverterImplementa___Test ImageConverterImplementationTest.cpp
    LIBRARIES MagnumTrade
    FILES
        ImageConverterImplementationTestFiles/info.txt)
target_include_directories(TradeImageConverterImplementa___Test PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
if(MAGNUM_WITH_ANYIMAGEIMPORTER)
    if(MAGNUM_BUILD_PLUGINS_STATIC OR MAGNUM_ANYIMAGEIMPORTER_BUILD_STATIC)
        target_link_libraries(TradeImageConverterImplementa___Test PRIVATE AnyImageImporter)
    else()
        # So the plugins get properly built when building the test
        add_dependencies(TradeImageConverterImplementa___Test AnyImageImporter)
    endif()
endif()
if(MAGNUM_WITH_ANYIMAGECONVERTER)
    if(MAGNUM_BUILD_PLUGINS_STATIC OR MAGNUM_ANYIMAGECONVERTER_BUILD_STATIC)
        target_link_libraries(TradeImageConverterImplementa___Test PRIVATE AnyImageConverter)
    else()
        # So the plugins get properly built when building the test
        add_dependencies(TradeImageConverterImplementa___Test AnyImageConverter)
    endif()
endif()
if(MAGNUM_WITH_TGAIMAGECONVERTER)
    if(MAGNUM_BUILD_PLUGINS_STATIC OR MAGNUM_TGAIMAGECONVERTER_BUILD_STATIC)
        target_link_libraries(TradeImageConverterImplementa___Test PRIVATE TgaImageConverter)
    else()
        # So the plugins get properly built when building the test
        add_dependencies(TradeImageConverterImplementa___Test TgaImageConverter)
    endif()
endif()

# Executable testing is implemented on Unix platforms only at the moment
if(CORRADE_TARGET_UNIX AND NOT CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT)
    corrade_add_test(TradeImageConverterTest ImageConverterTest.cpp
        LIBRARIES
            MagnumTrade
            # Link the same static plugins as for the magnum-imageconverter
            # executable so plugin existence checks are consistent between the two
            ${MAGNUM_IMAGECONVERTER_STATIC_PLUGINS}
        FILES
            ImageConverterTestFiles/info-data.txt
            ImageConverterTestFiles/info-data-ignored-output.txt
            ImageConverterTestFiles/info-converter.txt
            ImageConverterTestFiles/info-importer.txt
            ImageConverterTestFiles/info-importer-ignored-input-output.txt
            ImageConverterTestFiles/file.tga)
    target_include_directories(TradeImageConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(MAGNUM_WITH_IMAGECONVERTER)
        add_dependencies(TradeImageConverterTest magnum-imageconverter)
    endif()
endif()

corrade_add_test(TradeImageDataTest ImageDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradeLightDataTest LightDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradeMaterialDataTest MaterialDataTest.cpp LIBRARIES MagnumTradeTestLib)

corrade_add_test(TradeMeshDataTest MeshDataTest.cpp LIBRARIES MagnumTradeTestLib)
# In Emscripten 3.1.27, the stack size was reduced from 5 MB (!) to 64 kB:
#   https://github.com/emscripten-core/emscripten/pull/18191
# One case in this test uses 65 kB of stack space, need to increase it a bit to
# not make it crash.
if(CORRADE_TARGET_EMSCRIPTEN AND NOT EMSCRIPTEN_VERSION VERSION_LESS 3.1.27)
    set_property(TARGET TradeMeshDataTest APPEND_STRING PROPERTY LINK_FLAGS " -s STACK_SIZE=128kB")
endif()

corrade_add_test(TradePbrClearCoatMaterialDataTest PbrClearCoatMaterialDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradePbrMetallicRoughnessMate___Test PbrMetallicRoughnessMaterialDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradePbrSpecularGlossinessMat___Test PbrSpecularGlossinessMaterialDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradePhongMaterialDataTest PhongMaterialDataTest.cpp LIBRARIES MagnumTradeTestLib)

corrade_add_test(TradeSceneDataTest SceneDataTest.cpp LIBRARIES MagnumTradeTestLib)
# In Emscripten 3.1.27, the stack size was reduced from 5 MB (!) to 64 kB:
#   https://github.com/emscripten-core/emscripten/pull/18191
# One case in this test uses 65 kB of stack space, need to increase it a bit to
# not make it crash.
if(CORRADE_TARGET_EMSCRIPTEN AND NOT EMSCRIPTEN_VERSION VERSION_LESS 3.1.27)
    set_property(TARGET TradeSceneDataTest APPEND_STRING PROPERTY LINK_FLAGS " -s STACK_SIZE=128kB")
endif()

corrade_add_test(TradeSkinDataTest SkinDataTest.cpp LIBRARIES MagnumTradeTestLib)
corrade_add_test(TradeTextureDataTest TextureDataTest.cpp LIBRARIES MagnumTrade)

set_property(TARGET
    TradeAnimationDataTest
    TradeMaterialDataTest
    TradeMeshDataTest
    TradeSceneDataTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

if(MAGNUM_BUILD_DEPRECATED)
    corrade_add_test(TradeMeshData2DTest MeshData2DTest.cpp LIBRARIES MagnumTrade)
    corrade_add_test(TradeMeshData3DTest MeshData3DTest.cpp LIBRARIES MagnumTrade)
    corrade_add_test(TradeObjectData2DTest ObjectData2DTest.cpp LIBRARIES MagnumTradeTestLib)
    corrade_add_test(TradeObjectData3DTest ObjectData3DTest.cpp LIBRARIES MagnumTradeTestLib)
endif()
