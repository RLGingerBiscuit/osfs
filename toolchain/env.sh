#!/bin/env bash

SCRIPT_ROOT=$(dirname -- "$(readlink -f -- "${BASH_SOURCE[0]}")")

export PREFIX="$SCRIPT_ROOT/opt"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
