#!/usr/bin/python3

import argparse

from corrade import containers
from magnum import *
from magnum import trade

parser = argparse.ArgumentParser()
parser.add_argument('input')
parser.add_argument('output')
args = parser.parse_args()

importer_manager = trade.ImporterManager()
importer = importer_manager.load_and_instantiate('StbImageImporter')
# Don't want the alpha in order to have just 64-bit blocks
importer.configuration['forceChannelCount'] = 3
importer.open_file(args.input)

image = importer.image2d(0)
print(f"format: {image.format}")

converter_manager = trade.ImageConverterManager()
dxt_encoder = converter_manager.load_and_instantiate('StbDxtImageConverter')
dxt_encoder.configuration['highQuality'] = True

compressed_image = dxt_encoder.convert(image)
print(f"compressed format: {compressed_image.compressed_format}")

block_size = compressed_image.compressed_format.block_size
block_data_size = compressed_image.compressed_format.block_data_size
assert compressed_image.size % block_size.xy == Vector2i(0), "expected whole blocks"
# TODO use image.blocks once it exists
blocks = containers.StridedArrayView1D(compressed_image.data).expanded(0, (
    compressed_image.size.y//block_size.y,
    compressed_image.size.x//block_size.x,
    block_data_size
))

print("    char blocks[]{")
for y in range(compressed_image.size.y//block_size.y):
    for x in range(compressed_image.size.x//block_size.x):
        print("        {},".format(
            ', '.join(["'\\x{:02x}'".format(blocks[y, x, i]) for i in range(block_data_size)])
        ))
print("    };")

decoder = converter_manager.load_and_instantiate('BcDecImageConverter')
decoded_image = decoder.convert(compressed_image)

png_converter = converter_manager.load_and_instantiate('AnyImageConverter')
png_converter.convert_to_file(decoded_image, args.output)
