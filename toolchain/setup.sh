#!/bin/env bash

set -e

SCRIPT_ROOT=$(dirname -- "$( readlink -f -- "$0"; )"; )

source toolchain.sh

pushd $SCRIPT_ROOT

mkdir -p build/binutils build/gcc

pushd build/binutils

../../binutils-2.44/configure --target="$TARGET" --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j8
make install

popd

pushd build/gcc

../../gcc-15.1.0/configure --target "$TARGET" --prefix "$PREFIX" --disable-nls --enable-languages=c --without-headers --disable-hosted-libstdcxx
make -j8 all-gcc
make -j8 all-target-libgcc
make -j8 all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3

popd

popd
