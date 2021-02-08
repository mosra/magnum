#!/bin/bash

for i in $(ls *.spvasm); do
    magnum-shaderconverter $i ${i%asm}
done
