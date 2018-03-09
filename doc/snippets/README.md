Snippets that possibly generate output for Magnum documentation
###############################################################

### "Getting started" image

Displayed by the `getting-started` executable. Run the app and take screenshot
using Spectacle (including decorations, 880x707). Similarly for the gray
version. The resulting files should be resized to half. `pngcrush` to them for
smaller file sizes:

    pngcrush -ow getting-started.png
    pngcrush -ow getting-started-blue.png

The output printed by the application can be used to update the example output
in `doc/getting-started.dox`.
