#!/usr/bin/env sh

mkdir -p ~/.R

if [ "${COMPILER}" = "gcc-4.4" ]; then
    echo "CC=gcc-4.4 -std=gnu99"    >> ~/.R/Makevars
    echo "CXX=g++-4.4"              >> ~/.R/Makevars
    echo "CXX1X=g++-4.4 -std=c++0x" >> ~/.R/Makevars
fi

if [ "${COMPILER}" = "gcc" ]; then
    echo "CC=gcc -std=gnu99"    >> ~/.R/Makevars
    echo "CXX=g++"              >> ~/.R/Makevars
    echo "CXX1X=g++ -std=c++0x" >> ~/.R/Makevars
fi

if [ "${COMPILER}" = "clang" ]; then
    echo "CC=clang -std=gnu99"      >> ~/.R/Makevars
    echo "CXX=clang++"              >> ~/.R/Makevars
    echo "CXX1X=clang++ -std=c++0x" >> ~/.R/Makevars
fi

