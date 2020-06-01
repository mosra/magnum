/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/* Kept consistent with Generic.h (tested in ShadersGenericTest) */

#define POSITION_ATTRIBUTE_LOCATION 0
#define TEXTURECOORDINATES_ATTRIBUTE_LOCATION 1
#define COLOR_ATTRIBUTE_LOCATION 2
#define TANGENT_ATTRIBUTE_LOCATION 3
#define BITANGENT_ATTRIBUTE_LOCATION 4 /* also ObjectId */
#define OBJECT_ID_ATTRIBUTE_LOCATION 4 /* also Bitangent */
#define NORMAL_ATTRIBUTE_LOCATION 5

#define TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION 8
#define NORMAL_MATRIX_ATTRIBUTE_LOCATION 12
#define TEXTURE_OFFSET_ATTRIBUTE_LOCATION 15

/* Outputs */
#define COLOR_OUTPUT_ATTRIBUTE_LOCATION 0
#define OBJECT_ID_OUTPUT_ATTRIBUTE_LOCATION 1
