#!/usr/bin/env python3

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

# Loads a font and saves sizes of all its glyphs to a binary file that's then
# used by AtlasBenchmark.cpp

import argparse
import array

from magnum import math, trade
from magnum import *

parser = argparse.ArgumentParser()
parser.add_argument('input')
parser.add_argument('output')
parser.add_argument('--max', type=int, default=1024)
parser.add_argument('--ratio', type=int, default=1)
args = parser.parse_args()

importer = trade.ImporterManager().load_and_instantiate('AnySceneImporter')
importer.open_file(args.input)

print("Importing {} images".format(importer.image2d_count))

sizes = []
total = 0
max = Vector2i()
for i in range(importer.image2d_count):
    image = importer.image2d(i)
    size = image.size/args.ratio;
    if (size > Vector2i(args.max)).any():
        continue

    sizes += list(size)
    total += size.product()
    max = math.max(size, max)

print("Remains {0} images not larger than {1}x{1}".format(len(sizes)//2, args.max))

print("Total area: {} ({:.1f}**2), max: {}x{}".format(total, math.sqrt(total), max.x, max.y))

with open(args.output, 'wb') as output:
    array.array('h', sizes).tofile(output)
