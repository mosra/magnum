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
set(CMAKE_FOLDER "Magnum/Test")

find_package(Corrade REQUIRED PluginManager)

corrade_add_test(BritishTest BritishTest.cpp LIBRARIES Magnum)
# Just to have the GL headers pulled in correctly. Shouldn't be needed in most
# cases as the headers are self-contained in MagnumExternal, but some platforms
# such as iOS may disagree.
if(MAGNUM_TARGET_GL)
    target_link_libraries(BritishTest PRIVATE MagnumGL)
endif()
corrade_add_test(ConverterUtilitiesTest ConverterUtilitiesTest.cpp LIBRARIES Magnum Corrade::PluginManager)
corrade_add_test(FileCallbackTest FileCallbackTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageTest ImageTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(ImageFlagsTest ImageFlagsTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageViewTest ImageViewTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(MeshTest MeshTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(PixelFormatTest PixelFormatTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(PixelStorageTest PixelStorageTest.cpp LIBRARIES Magnum)
corrade_add_test(ResourceManagerTest ResourceManagerTest.cpp LIBRARIES Magnum)
corrade_add_test(SamplerTest SamplerTest.cpp LIBRARIES MagnumTestLib)
# Prefixed with project name to avoid conflicts with TagsTest in Corrade
corrade_add_test(MagnumTagsTest TagsTest.cpp LIBRARIES Magnum)
corrade_add_test(TimelineTest TimelineTest.cpp LIBRARIES Magnum)

# Prefixed with project name to avoid conflicts with VersionTest in Corrade and
# other repos
corrade_add_test(MagnumVersionTest VersionTest.cpp LIBRARIES Magnum)

corrade_add_test(VertexFormatTest VertexFormatTest.cpp LIBRARIES MagnumTestLib)

if(MAGNUM_BUILD_DEPRECATED)
    corrade_add_test(ArrayTest ArrayTest.cpp LIBRARIES Magnum)
endif()

set_property(TARGET
    MeshTest
    PixelFormatTest
    ResourceManagerTest
    VertexFormatTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")
