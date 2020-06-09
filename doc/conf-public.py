# Inherit everything from the local config
import os, sys; sys.path.append(os.path.dirname(os.path.realpath(__file__)))
from conf import *

DOXYFILE = 'Doxyfile-public'

assert "Python API" in LINKS_NAVBAR2[3][0]
LINKS_NAVBAR2[3] = ("<a href=\"https://doc.magnum.graphics/python/\">Python API</a>", [])

STYLESHEETS = [
    'https://fonts.googleapis.com/css?family=Source+Sans+Pro:400,400i,600,600i%7CSource+Code+Pro:400,400i,600&subset=latin-ext',
    'https://static.magnum.graphics/m-dark.compiled.css',
    'https://static.magnum.graphics/m-dark.documentation.compiled.css'
]

SEARCH_DOWNLOAD_BINARY = True
SEARCH_BASE_URL = "https://doc.magnum.graphics/magnum/"
SEARCH_EXTERNAL_URL = "https://google.com/search?q=site:doc.magnum.graphics+Magnum+{query}"

FAVICON = 'https://doc.magnum.graphics/favicon.ico'
