#!/bin/bash
set -eu

# Hack to allow calling this script from it's dir.
if [ -f precompute_radiance_transfer.pasprogram ]; then
  cd ../../../../
fi

# Call this from ../../../../ (or just use `make examples').

fpc -dRELEASE @kambi.cfg vrml/opengl/examples/radiance_transfer/precompute_radiance_transfer.pasprogram
