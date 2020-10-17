#!/usr/bin/env python3

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2020 janos <janos.meny@googlemail.com>
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

from numpy import genfromtxt

def format(filename):
    data = genfromtxt(filename, delimiter=',')
    formatted = ''
    data = data[1:,1:].astype(int)
    formatted = ''

    for i in range(256//4):
        line = '    '
        for j in range(4):
            row = data[4*i+j]
            line += '{{{:>3}, {:>3}, {:>3}}}, '.format(row[0], row[1], row[2])
        formatted = formatted + line + '\n'

    # Strip trailing comma and newline
    return formatted[:-3]

# The two CSV files taken from https://www.kennethmoreland.com/color-advice/

print("/* Generated with Implementation/cool-warm.py */")
print("constexpr UnsignedByte CoolWarmSmooth[][3] = {\n", end='')
print(format('smooth-cool-warm-table-byte-0256.csv'))
print("};\n")

print("/* Generated with Implementation/cool-warm.py */")
print("constexpr UnsignedByte CoolWarmBent[][3] = {\n", end='')
print(format('bent-cool-warm-table-byte-0256.csv'))
print("};\n")
