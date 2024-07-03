#!/usr/bin/python3

import argparse
from typing import List, Tuple

from corrade import containers
from magnum import *
from magnum import math, trade

parser = argparse.ArgumentParser()
parser.add_argument('input')
parser.add_argument('output')
parser.add_argument('--offset', type=int, default=0)
parser.add_argument('--count', type=int, default=4)
args = parser.parse_args()

importer_manager = trade.ImporterManager()
importer_manager.metadata('DdsImporter').configuration['assumeYUpZBackward'] = True
importer_manager.metadata('KtxImporter').configuration['assumeOrientation'] = 'ruo'
importer = importer_manager.load_and_instantiate('AnyImageImporter')
importer.open_file(args.input)

image = importer.image2d(0)

assert image.is_compressed, "expected a compressed image"

print(f"format: {image.compressed_format}")

block_size = image.compressed_format.block_size
assert image.size % block_size.xy == Vector2i(0), "expected whole blocks"

block_data_size = image.compressed_format.block_data_size
# TODO use image.blocks once it exists
blocks = containers.StridedArrayView1D(image.data).expanded(0, (
    image.size.y//block_size.y,
    image.size.x//block_size.x,
    block_data_size
))

if image.compressed_format.name.startswith('BC'):
    decoder_name = 'BcDecImageConverter'
elif image.compressed_format.name.startswith('ETC') or \
     image.compressed_format.name.startswith('EAC'):
    decoder_name = 'EtcDecImageConverter'
else: assert False, f"unsupported format {image.compressed_format}"

converter_manager = trade.ImageConverterManager()
decoder = converter_manager.load_and_instantiate(decoder_name)
decoded = decoder.convert(image)
assert decoded.size == image.size
# TODO some better way to get this directly from pixels? i.e., want to access
#   the bytes and not the high-level type
# assert decoded.pixels.is_contiguous
decoded_pixel_size = decoded.format.size
decoded_pixel_data = containers.StridedArrayView1D(decoded.data).expanded(0, (
    image.size.y,
    image.size.x,
    decoded_pixel_size
))

block_count = decoded.size/block_size.xy

pixels = decoded.pixels

if decoded.format.channel_count == 1:
    default_min = math.inf
    default_max = -math.inf
    sum_ = lambda i: i
elif decoded.format.channel_count == 2:
    default_min = Vector2(math.inf)
    default_max = Vector2(-math.inf)
    sum_ = lambda i: i.sum()
elif decoded.format.channel_count == 3:
    default_min = Vector3(math.inf)
    default_max = Vector3(-math.inf)
    sum_ = lambda i: i.sum()
elif decoded.format.channel_count == 4:
    default_min = Vector4(math.inf)
    default_max = Vector4(-math.inf)
    sum_ = lambda i: i.sum()

block_ranges: List[Tuple[int, int, float]] = []

# This is extremely slow. Pybind11 ugh what are you doing?!
for y in range(block_count.y):
    oy = y*block_size.y
    for x in range(block_count.x):
        ox = x*block_size.x
        min_ = default_min
        max_ = default_max
        for by in range(block_size.y):
            for bx in range(block_size.x):
                min_ = math.min(pixels[oy + by, ox + bx], min_)
                max_ = math.max(pixels[oy + by, ox + bx], max_)

        block_ranges += [(x, y, sum_(max_ - min_))]

# Get most interesting blocks
most_interesting_blocks = list(reversed(sorted(block_ranges, key=lambda block: block[2])))[args.offset:(args.offset + args.count)]

print("    char blocks[]{")
for x, y, range_ in most_interesting_blocks:
    print("        /* [{}, {}], {:1.3f} */\n        {},".format(
        x, y, range_,
        ', '.join(["'\\x{:02x}'".format(blocks[y, x, i]) for i in range(block_data_size)])
    ))
print("    };")

# Combine the decoded pixels from the most interesting blocks
data = bytearray(b'\0'*(args.count*block_size.x*block_size.y*decoded_pixel_size))
offset = 0
for x, y, range_ in most_interesting_blocks:
    oy = y*block_size.y
    ox = x*block_size.x
    for by in range(block_size.y):
        for bx in range(block_size.x):
            for i in range(decoded_pixel_size):
                data[offset] = decoded_pixel_data[oy + by, ox + bx, i]
                offset += 1
assert offset == len(data)

# Write them to a file
converter = converter_manager.load_and_instantiate('AnyImageConverter')
converter.convert_to_file(ImageView2D(
    decoded.format,
    (block_size.x, block_size.y*args.count),
    data), args.output)
