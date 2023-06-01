The `checkerboard.png` is exported as `checkerboard.in.png` with Inkscape from
`Shaders/Test/TestFiles/checkerboard.svg` with:

-   the non-alpha checkerboard layer enabled
-   the bold text layer enabled
-   the uneven top right area selected
-   DPI set to 3, resulting in a 32x24 image (8x6 blocks)

The `checkerboard-odd.png` is exported as `checkerboard-odd.in.png` with the
even top right area selected and the size set to 28x28, resulting in 7x7
blocks.

Both are then passed through StbDxtImageConverter and subsequently decoded
back to `checkerboard.png` and `checkerboard-odd.png` with
`compress-format-blocks.py` to act as a ground truth, the output of the script
and its full invocation is pasted to the `ColorBatchTest.cpp`.

The remaining images are generated with `extract-interesting-blocks.py`, the
output of that script and its full invocation is again in `ColorBatchTest.cpp`.

Unlike in other tests, the images are PNGs and not TGAs. The full test process
requires `BcDecImageConverter` from the plugins repo anyway, so there's no
point in having larger TGAs.
