#!/bin/bash

set -e

# in -> bin
for i in *.bin.in; do
    ../../../../../../magnum-plugins/src/MagnumPlugins/GltfImporter/Test/in2bin.py ${i}
done
