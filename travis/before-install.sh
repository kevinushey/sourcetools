#!/usr/bin/env bash

mkdir -p ~/.R

read -r -d '' MAKEVARS << EOF
CC=${CC} -std=gnu99
CXX=${CXX}
CXX1X=${CXX} -std=c++0x
EOF

echo "${MAKEVARS}" > ~/.R/Makevars
