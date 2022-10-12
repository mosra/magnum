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

### triangulate.svg, scenedata-tree.svg, scenedata-dod.svg, line-*.svg

Created by Inkscape from `doc/artwork/triangulate.svg` and `line-*.svg` by
saving as Optimized SVG. On fresh installations you need the `scour` package
for it:

-   enabling all possible options in the dialog, saving
-   cleaning up the `<svg>` header (removing `version`, `xmlns`) in an editor
-   converting to a `style=""`, *keeping* `viewBox`
-   adding `class="m-image"`
-   removing all layers that have `display: none`

In case of the `line-quad-data-expansion*.svg` and
`line-quad-data-overlap-*.svg`, they're all generated from
`doc/artwork/line-quad-expansion.svg` and `doc/artwork/line-quad-overlap.svg`,
each time with different layers shown.

The `doc/artwork/line-quad-data-other.svg` is derived from
`doc/artwork/line-quad-data-neighbor.svg` by removing the "neighbor" layer and
making the canvas smaller, the `doc/artwork/line-quad-data.svg` is then derived
from `doc/artwork/line-quad-data-other.svg` by removing the "other" layer and
making the canvas smaller yet again.
