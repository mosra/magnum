#!/bin/bash

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 /path/to/flextGLgen.py"
    exit 1
fi

cd GL
$1 -D . -t . extensions.txt
cd ..

cd GLES2
$1 -D . -t . extensions.txt
$1 -D . -t Emscripten/ Emscripten/extensions.txt
cd ..

cd GLES3
$1 -D . -t . extensions.txt
$1 -D . -t Emscripten/ Emscripten/extensions.txt
cd ..
