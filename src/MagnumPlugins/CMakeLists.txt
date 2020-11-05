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

# Wrapper for creating given plugin type
macro(add_plugin plugin_name debug_install_dirs release_install_dirs metadata_file)
    string(TOUPPER ${plugin_name} PLUGIN_NAME)
    if(NOT MAGNUM_${PLUGIN_NAME}_BUILD_STATIC)
        corrade_add_plugin(${plugin_name} "${debug_install_dirs}" "${release_install_dirs}" ${metadata_file} ${ARGN})
        set_target_properties(${plugin_name} ${plugin_name}-metadata PROPERTIES FOLDER "MagnumPlugins/${plugin_name}")
    else()
        corrade_add_static_plugin(${plugin_name} "${release_install_dirs}" ${metadata_file} ${ARGN})
        set_target_properties(${plugin_name} ${plugin_name}-dependencies PROPERTIES FOLDER "MagnumPlugins/${plugin_name}")
    endif()
endmacro()

if(WITH_ANYAUDIOIMPORTER)
    add_subdirectory(AnyAudioImporter)
endif()

if(WITH_ANYIMAGEIMPORTER)
    add_subdirectory(AnyImageImporter)
endif()

if(WITH_ANYIMAGECONVERTER)
    add_subdirectory(AnyImageConverter)
endif()

if(WITH_ANYSCENECONVERTER)
    add_subdirectory(AnySceneConverter)
endif()

if(WITH_ANYSCENEIMPORTER)
    add_subdirectory(AnySceneImporter)
endif()

if(WITH_ANYSHADERCONVERTER)
    add_subdirectory(AnyShaderConverter)
endif()

if(WITH_MAGNUMFONT)
    add_subdirectory(MagnumFont)
endif()

if(WITH_MAGNUMFONTCONVERTER)
    add_subdirectory(MagnumFontConverter)
endif()

if(WITH_OBJIMPORTER)
    add_subdirectory(ObjImporter)
endif()

if(WITH_TGAIMAGECONVERTER)
    add_subdirectory(TgaImageConverter)
endif()

if(WITH_TGAIMPORTER)
    add_subdirectory(TgaImporter)
endif()

if(WITH_WAVAUDIOIMPORTER)
    add_subdirectory(WavAudioImporter)
endif()
