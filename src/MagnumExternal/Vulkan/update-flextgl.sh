#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 /path/to/flextGLgen.py"
    exit 1
fi

$1 -D . -t . extensions.txt
