Common shader test files
------------------------

The `*-texture.tga` files are exported quadrants from `checkerboard.svg` with
various layers enabled, saved either as three-channel or four-channel PNG of
128x128 pixels and then converted to TGA via `magnum-imageconverter` (so the
shader tests don't need any external plugins) --- if everything is correct, the
RGB files will be 48 kB and the RGBA ones 64 kB.

The `vector.tga` file is exported from `magnum/doc/generated/vector.svg` as
128x128, converted to a single-channel format via ImageMagick:

```sh
convert vector.png -flatten -colorspace Gray vector-r.png
```

and then to a single-channel (16 kB) TGA via `magnum-imageconverter`. Then,
this file is converted to a SDF via

```sh
magnum-distancefieldconverter --output-size "64 64" --radius 16 vector.tga vector-distancefield.tga
```
