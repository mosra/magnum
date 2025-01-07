set -e

../../../corrade/src/acme/acme.py MagnumMath.hpp --output ../../../magnum-singles
../../../corrade/src/acme/acme.py MagnumMathBatch.hpp --output ../../../magnum-singles
../../../corrade/src/acme/acme.py MagnumMeshTools.hpp --output ../../../magnum-singles
