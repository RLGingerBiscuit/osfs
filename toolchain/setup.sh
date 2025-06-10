#!/bin/env bash

set -e

SCRIPT_ROOT=$(dirname -- "$(readlink -f -- "$0")")

source env.sh

pushd "$SCRIPT_ROOT"

# Define the versions and URLs for binutils and gcc
BINUTILS_VERSION="2.44"
BINUTILS_URL="https://sourceware.org/pub/binutils/releases/binutils-${BINUTILS_VERSION}.tar.xz"
GCC_VERSION="15.1.0"
GCC_URL="https://sourceware.org/pub/gcc/releases/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"

# Create directories for tarballs and extracted sources
mkdir -p tarballs
mkdir -p sources

# Download binutils if not already present
if [ ! -f "tarballs/binutils-${BINUTILS_VERSION}.tar.xz" ]; then
    echo "Downloading binutils-${BINUTILS_VERSION}.tar.xz..."
    wget -c -q --show-progress "$BINUTILS_URL" -P tarballs
else
    echo "binutils-${BINUTILS_VERSION}.tar.xz already exists. Skipping download."
fi

# Download gcc if not already present
if [ ! -f "tarballs/gcc-${GCC_VERSION}.tar.xz" ]; then
    echo "Downloading gcc-${GCC_VERSION}.tar.xz..."
    wget -c -q --show-progress "$GCC_URL" -P tarballs
else
    echo "gcc-${GCC_VERSION}.tar.xz already exists. Skipping download."
fi

# Extract binutils if not already extracted
if [ ! -d "sources/binutils-${BINUTILS_VERSION}" ]; then
    echo "Extracting binutils-${BINUTILS_VERSION}.tar.xz..."
    tar -xf "tarballs/binutils-${BINUTILS_VERSION}.tar.xz" -C sources
else
    echo "binutils-${BINUTILS_VERSION} already extracted. Skipping extraction."
fi

# Extract gcc if not already extracted
if [ ! -d "sources/gcc-${GCC_VERSION}" ]; then
    echo "Extracting gcc-${GCC_VERSION}.tar.xz..."
    tar -xf "tarballs/gcc-${GCC_VERSION}.tar.xz" -C sources
else
    echo "gcc-${GCC_VERSION} already extracted. Skipping extraction."
fi

# Create directories for builds
mkdir -p build/binutils build/gcc

# Configure and build binutils
pushd "build/binutils"

../../sources/binutils-${BINUTILS_VERSION}/configure --target="$TARGET" --prefix="$PREFIX" \
    --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install

popd # "build/binutils"

# Configure and build gcc
pushd "build/gcc"

../../sources/gcc-${GCC_VERSION}/configure --target "$TARGET" --prefix "$PREFIX" \
    --disable-nls --enable-languages=c --without-headers \
    --disable-hosted-libstdcxx
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
# C++ support (need to enable language above)
# make -j$(nproc) all-target-libstdc++-v3
# make install-target-libstdc++-v3

popd # "build/gcc"

popd # "$SCRIPT_ROOT"

