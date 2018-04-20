#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

corrade_add_test(ArrayTest ArrayTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageTest ImageTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(ImageViewTest ImageViewTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(MeshTest MeshTest.cpp LIBRARIES Magnum)
corrade_add_test(PixelFormatTest PixelFormatTest.cpp LIBRARIES MagnumTestLib)
target_compile_definitions(PixelFormatTest PRIVATE "CORRADE_GRACEFUL_ASSERT")
corrade_add_test(PixelStorageTest PixelStorageTest.cpp LIBRARIES Magnum)
corrade_add_test(ResourceManagerTest ResourceManagerTest.cpp LIBRARIES Magnum)
target_compile_definitions(ResourceManagerTest PRIVATE "CORRADE_GRACEFUL_ASSERT")
corrade_add_test(SamplerTest SamplerTest.cpp LIBRARIES MagnumTestLib)

add_library(ResourceManagerLocalInstanceTestLib ${SHARED_OR_STATIC} ResourceManagerLocalInstanceTestLib.cpp)
target_link_libraries(ResourceManagerLocalInstanceTestLib Magnum)
if(NOT BUILD_STATIC)
    target_compile_definitions(ResourceManagerLocalInstanceTestLib PRIVATE "ResourceManagerLocalInstanceTestLib_EXPORTS")
endif()
corrade_add_test(ResourceManagerLocalInstanceTest ResourceManagerLocalInstanceTest.cpp LIBRARIES Magnum ResourceManagerLocalInstanceTestLib)

corrade_add_test(TagsTest TagsTest.cpp LIBRARIES Magnum)

set_target_properties(
    ArrayTest
    ImageTest
    ImageViewTest
    MeshTest
    PixelFormatTest
    PixelStorageTest
    ResourceManagerTest
    ResourceManagerLocalInstanceTestLib
    ResourceManagerLocalInstanceTest
    TagsTest
    PROPERTIES FOLDER "Magnum/Test")
