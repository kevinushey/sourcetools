#!/usr/bin/env sh

mkdir -p ~/.R
echo "CC=${CC}"   >> ~/.R/Makevars
echo "CXX=${CXX}" >> ~/.R/Makevars
