#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2022 Guillaume Jacquemin <williamjcm@users.noreply.github.com>
#   Copyright © 2024 Andy Maloney <asmaloney@gmail.com>
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

import sys
import gdb, gdb.printing
import struct

class MagnumTypePrinter:
    """Base class for all pretty-printers"""

    def __init__(self, val: gdb.Value):
        self.val = val
        self.enabled: bool = True

    def to_string(self):
        return f"{self.val.type}"

class MagnumImage(MagnumTypePrinter):
    """Prints a Magnum::(Compressed)Image(View)"""

    def to_string(self):
        return f"{self.val.type} of size {self.val['_size']} and format {self.val['_format']}"

class MagnumResource(MagnumTypePrinter):
    """Prints a Resource"""

    def to_string(self):
        return f"Resource key {self.val['_key']} (state: {self.val['_state']})"

class MagnumResourceKey(MagnumTypePrinter):
    """Prints a ResourceKey"""

    def to_string(self):
        key = str()
        for i in range(8):
            key += "{:02x}".format(int(self.val['_digest'][i]) & 0xff)
        return "".join(key)

class MagnumTimeline(MagnumTypePrinter):
    """Prints a Timeline"""

    def to_string(self):
        if not bool(self.val['running']):
            return f"{self.val.type} (not running)"
        return f"{self.val.type} (running; previous frame time: {self.val['_previousFrameDuration']}s)"

class MagnumTrack(MagnumTypePrinter):
    """Prints an Animation::Track"""

    class Iterator:
        def __init__(self, val: gdb.Value, fields: list[gdb.Field]):
            self.val = val
            self.fields = fields
            self.index = 0

        def __iter__(self):
            return self

        def __next__(self):
            index = self.index
            self.index = self.index + 1
            if index == len(self.fields):
                raise StopIteration
            return (f"[{self.fields[index].name.split('_')[1]}]", self.val[self.fields[index]])

    def children(self):
        return self.Iterator(self.val, self.val.type.fields())

    def to_string(self):
        return f"{self.val.type} with {self.val['_data']['_size']} keyframes"

class MagnumAngle(MagnumTypePrinter):
    """Prints a Math::Deg or Rad"""

    def to_string(self):
        return f"{self.val['_value']}"

class MagnumBezier(MagnumTypePrinter):
    """Prints a Math::Bezier"""

    class Iterator:
        def __init__(self, data: gdb.Value, order: int):
            self.data = data
            self.order = order
            self.count: int = 0

        def __iter__(self):
            return self

        def __next__(self):
            count = self.count
            self.count = self.count + 1
            if count == self.order + 1:
                raise StopIteration
            return (f"[{count}]", self.data[count])

    def __init__(self, val: gdb.Value):
        template_arguments = val.type.strip_typedefs().name.split('<')[1].split('>')[0].split(',')
        self.order = int(template_arguments[0].strip())
        self.dimensions = int(template_arguments[1].strip())
        super(MagnumBezier, self).__init__(val)

    def children(self):
        return self.Iterator(self.val['_data'], self.order)

    def to_string(self):
        return f"{self.order}-order, {self.dimensions}-dimensional Bézier curve"

class MagnumBitVector(MagnumTypePrinter):
    """Prints a Math::BitVector"""

    class Iterator:
        def __init__(self, data: gdb.Value, size: int):
            self.data = data
            self.size = size
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            count = self.count
            self.count = self.count + 1
            if count == self.size:
                raise StopIteration
            return (f"[{count}]", bool((self.data[int(count/8)] >> count%8) & 0x01))

    def __init__(self, val: gdb.Value):
        self.size = int(val.type.strip_typedefs().name.split('<')[1].split('>')[0])
        super(MagnumBitVector, self).__init__(val)

    def children(self):
        return self.Iterator(self.val['_data'], self.size)

    def to_string(self):
        return f"BitVector of size {self.size}"

class MagnumComplex(MagnumTypePrinter):
    """Prints a Math::Complex"""

    def to_string(self):
        return f"{self.val['_real']} + i{self.val['_imaginary']}"

class MagnumCubicHermite(MagnumTypePrinter):
    """Prints a Math::CubicHermite"""

    def to_string(self):
        return f"Cubic Hermite curve with spline point {self.val['_point']}, in-tangent {self.val['_inTangent']}," \
               f" and out-tangent {self.val['_outTangent']}"

class MagnumDual(MagnumTypePrinter):
    """Prints a Math::Dual{Complex,Quaternion}"""

    def to_string(self):
        return f"({self.val['_real']}) + \u03F5({self.val['_dual']})"

class MagnumFrustum(MagnumTypePrinter):
    """Prints a Math::Frustum"""

    class Iterator:
        LABELS = ["left", "right", "bottom", "top", "near", "far"]
        SIZE = len(LABELS)

        def __init__(self, data: gdb.Value):
            self.data = data
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            count = self.count
            self.count = self.count + 1
            if count == self.SIZE:
                raise StopIteration
            return (f"[{self.LABELS[count]}]", self.data[count])

    def children(self):
        return self.Iterator(self.val['_data'])

class MagnumHalf(MagnumTypePrinter):
    """Prints a Math::Half"""

    def to_string(self):
        data = int(self.val['_data']).to_bytes(2, byteorder=sys.byteorder)
        return f"{struct.unpack('e', data)[0]}"

class MagnumMatrix(MagnumTypePrinter):
    """Prints a Math::(Rectangular)Matrix"""

    class Iterator:
        def __init__(self, data: gdb.Value, rows: int):
            self.data = data
            self.rows = rows
            self.row = 0

        def __iter__(self):
            return self

        def __next__(self):
            row = self.row
            self.row = self.row + 1
            if row == self.rows:
                raise StopIteration
            return (f"[{row}]", self.data[row])

    def __init__(self, val: gdb.Value):
        try:
            # (Rectangular)Matrix
            self.rows = int(val.type.strip_typedefs().name.split('<')[1].split(',')[0].strip())
        except ValueError:
            # Matrix{3,4}
            self.rows = int(val.type.strip_typedefs().name.split('<')[0][-1])

        if "RectangularMatrix" in val.type.strip_typedefs().name:
            self.columns = int(val.type.strip_typedefs().name.split('<')[1].split(',')[1].strip())
        else:
            self.columns = self.rows

        super(MagnumMatrix, self).__init__(val)

    def children(self):
        return self.Iterator(self.val['_data'], self.rows)

    def to_string(self):
        return f"Matrix with {self.rows} rows and {self.columns} columns"

class MagnumQuaternion(MagnumTypePrinter):
    """Prints a Math::Quaternion"""

    def to_string(self):
        return f"[{self.val['_vector']}, {self.val['_scalar']}]"

class MagnumRange(MagnumTypePrinter):
    """Prints a Math::Range({2,3}D)"""

    def to_string(self):
        return f"Range (min: {self.val['_min']}, max: {self.val['_max']})"

class MagnumUnit(MagnumTypePrinter):
    """Prints a Math::Unit"""

    def to_string(self):
        return f"{self.val['_value']}"

class MagnumVectorColor(MagnumTypePrinter):
    """
    Prints a Math::Vector({2,3,4}).

    Also prints Math::Color{3,4}, because they inherit from the vectors.
    """

    def __init__(self, val: gdb.Value):
        try:
            # Vector2/3/4 and Color3/4
            self.dimensions = int(val.type.strip_typedefs().name.split('<')[0][-1])
        except ValueError:
            # Vector<dimensions, T>, used in matrices
            self.dimensions = int(val.type.strip_typedefs().name.split('<')[1].split(',')[0].strip())
        super(MagnumVectorColor, self).__init__(val)

    def to_string(self):
        components = []
        for i in range(self.dimensions):
            components.append(str(self.val['_data'][i]))
        return f"({', '.join(components)})"

magnum_printers: gdb.printing.RegexpCollectionPrettyPrinter | None = None

def build_magnum_printer():
    global magnum_printers

    if magnum_printers is not None:
        return

    magnum_printers = gdb.printing.RegexpCollectionPrettyPrinter("Magnum")
    magnum_printers.add_printer("CompressedImage",
                                "^Magnum::CompressedImage<\d+, .*>$",
                                MagnumImage)
    magnum_printers.add_printer("CompressedImageView",
                                "^Magnum::CompressedImageView<\d+, .*>$",
                                MagnumImage)
    magnum_printers.add_printer("Image",
                                "^Magnum::Image<\d+, .*>$",
                                MagnumImage)
    magnum_printers.add_printer("ImageView",
                                "^Magnum::ImageView<\d+, .*>$",
                                MagnumImage)
    magnum_printers.add_printer("Resource",
                                "^Magnum::Resource<.*, .*>$",
                                MagnumResource)
    magnum_printers.add_printer("ResourceKey",
                                "^Magnum::ResourceKey$",
                                MagnumResourceKey)
    magnum_printers.add_printer("Timeline",
                                "^Magnum::Timeline$",
                                MagnumTimeline)
    magnum_printers.add_printer("Animation::Track",
                                "^Magnum::Animation::Track<.*>$",
                                MagnumTrack)
    magnum_printers.add_printer("Math::Bezier",
                                "^Magnum::Math::Bezier<\d+, \d+, .*>$",
                                MagnumBezier)
    magnum_printers.add_printer("Math::BitVector",
                                "^Magnum::Math::BitVector<\d+>$",
                                MagnumBitVector)
    magnum_printers.add_printer("Math::Color3",
                                "^Magnum::Math::Color3<.*>$",
                                MagnumVectorColor)
    magnum_printers.add_printer("Math::Color4",
                                "^Magnum::Math::Color4<.*>$",
                                MagnumVectorColor)
    magnum_printers.add_printer("Math::Complex",
                                "^Magnum::Math::Complex<.*>$",
                                MagnumComplex)
    magnum_printers.add_printer("Math::CubicHermite",
                                "^Magnum::Math::CubicHermite<.*>$",
                                MagnumCubicHermite)
    magnum_printers.add_printer("Math::Deg",
                                "^Magnum::Math::Deg<.*>$",
                                MagnumAngle)
    magnum_printers.add_printer("Math::DualComplex",
                                "^Magnum::Math::DualComplex<.*>$",
                                MagnumDual)
    magnum_printers.add_printer("Math::DualQuaternion",
                                "^Magnum::Math::DualQuaternion<.*>$",
                                MagnumDual)
    magnum_printers.add_printer("Math::Frustum",
                                "^Magnum::Math::Frustum<.*>$",
                                MagnumFrustum)
    magnum_printers.add_printer("Math::Half",
                                "^Magnum::Math::Half$",
                                MagnumHalf)
    magnum_printers.add_printer("Math::Matrix",
                                "^Magnum::Math::Matrix<\d+, .*>$",
                                MagnumMatrix)
    magnum_printers.add_printer("Math::Matrix3",
                                "^Magnum::Math::Matrix3<.*>$",
                                MagnumMatrix)
    magnum_printers.add_printer("Math::Matrix4",
                                "^Magnum::Math::Matrix4<.*>$",
                                MagnumMatrix)
    magnum_printers.add_printer("Math::Quaternion",
                                "^Magnum::Math::Quaternion<.*>$",
                                MagnumQuaternion)
    magnum_printers.add_printer("Math::Rad",
                                "^Magnum::Math::Rad<.*>$",
                                MagnumAngle)
    magnum_printers.add_printer("Math::Range",
                                "^Magnum::Math::Range<\d+, .*>$",
                                MagnumRange)
    magnum_printers.add_printer("Math::Range2D",
                                "^Magnum::Math::Range2D<.*>$",
                                MagnumRange)
    magnum_printers.add_printer("Math::Range3D",
                                "^Magnum::Math::Range3D<.*>$",
                                MagnumRange)
    magnum_printers.add_printer("Math::RectangularMatrix",
                                "^Magnum::Math::RectangularMatrix<\d+, \d+, .*>$",
                                MagnumMatrix)
    magnum_printers.add_printer("Math::Unit",
                                "^Magnum::Math::Unit<.*>$",
                                MagnumUnit)
    magnum_printers.add_printer("Math::Vector",
                                "^Magnum::Math::Vector<\d+,.*>$",
                                MagnumVectorColor)
    magnum_printers.add_printer("Math::Vector2",
                                "^Magnum::Math::Vector2<.*>$",
                                MagnumVectorColor)
    magnum_printers.add_printer("Math::Vector3",
                                "^Magnum::Math::Vector3<.*>$",
                                MagnumVectorColor)
    magnum_printers.add_printer("Math::Vector4",
                                "^Magnum::Math::Vector4<.*>$",
                                MagnumVectorColor)

def register_magnum_printers(obj: gdb.Objfile | gdb.Progspace):
    if obj is None:
        obj = gdb

    global magnum_printers

    if magnum_printers is None:
        build_magnum_printer()

    gdb.printing.register_pretty_printer(obj, magnum_printers)
