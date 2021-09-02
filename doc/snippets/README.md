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

### triangulate.svg, scenedata-tree.svg, scenedata-dod.svg

Created by Inkscape from `doc/artwork/triangulate.svg` by saving as Optimized
SVG and:

-   cleaning up the `<svg>` header (removing `version`, `xmlns`)
-   converting to a `style=""`, *keeping* `viewBox`
-   adding `class="m-image"`
-   removing metadata, the background layer and all layers that have
    `display: none`
