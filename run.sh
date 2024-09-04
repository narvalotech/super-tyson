#!/usr/bin/env bash

set -eu

ninja -Cbuild

echo "(+ 1 2 (* 3 4))" | ./build/SuperTyson
