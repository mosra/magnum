#!/usr/bin/env python3

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

# Not included in the repository, download it from
# https://raw.githubusercontent.com/BIDS/colormap/master/colormaps.py
from colormaps import _magma_data, _plasma_data, _inferno_data, _viridis_data

from typing import List, Tuple

def colors(data: List[Tuple[float, float, float]]):
    for i, color in enumerate(data):
        if i and i % 4 == 0: print(",\n    ", end='')
        elif i: print(", ", end='')

        # Seems like the values are already in sRGB -- applying the sRGB curve
        # on those with Color3(color).to_srgb_int() washes them out compared to
        # the 'official' rendering
        srgb = [int(c*255) for c in color]
        print('{{{:>3}, {:>3}, {:>3}}}'.format(srgb[0], srgb[1], srgb[2]), end='')
    print('')

print("/* Generated with Implementation/colormap-to-srgb.py */")
print("constexpr UnsignedByte Magma[][3] = {\n    ", end='')
colors(_magma_data)
print("};\n")

print("/* Generated with Implementation/colormap-to-srgb.py */")
print("constexpr UnsignedByte Plasma[][3] = {\n    ", end='')
colors(_plasma_data)
print("};\n")

print("/* Generated with Implementation/colormap-to-srgb.py */")
print("constexpr UnsignedByte Inferno[][3] = {\n    ", end='')
colors(_inferno_data)
print("};\n")

print("/* Generated with Implementation/colormap-to-srgb.py */")
print("constexpr UnsignedByte Viridis[][3] = {\n    ", end='')
colors(_viridis_data)
print("};\n")
