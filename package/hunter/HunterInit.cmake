#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Pascal Thomet <pthomet@gmail.com>
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

set(_HUNTER_GATE_FILE ${CMAKE_CURRENT_BINARY_DIR}/HunterGate.cmake)
if(NOT EXISTS ${_HUNTER_GATE_FILE})
    file(DOWNLOAD
        https://raw.githubusercontent.com/hunter-packages/gate/c4e5b3e4d1c97fb46ae86af7f59baac95f7f1cf0/cmake/HunterGate.cmake
        ${_HUNTER_GATE_FILE}
        EXPECTED_HASH SHA1=b59156338f5722d6ef84321c8ccef5ecf2ac24e6
        STATUS _HUNTER_GATE_FILE_STATUS)
    # If download fails, remove the bad file so it tries to download it again
    # next time
    list(GET _HUNTER_GATE_FILE_STATUS 0 _HUNTER_GATE_FILE_STATUS_CODE)
    if(NOT _HUNTER_GATE_FILE_STATUS_CODE EQUAL 0)
        file(REMOVE ${_HUNTER_GATE_FILE})
        message(FATAL_ERROR "Error downloading HunterGate.cmake. Please retry the CMake configure step.")
    endif()
endif()

include(${_HUNTER_GATE_FILE})
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.65.tar.gz"
    SHA1 "00e252171605c290390a3cc90a372beeeb406d3c")
