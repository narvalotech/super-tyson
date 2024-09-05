#!/usr/bin/env bash

set -eu

ninja -Cbuild

echo "(+ 1 2 (* 3 4))" | ./build/SuperTyson
# To sniff traffic:
# socat -r /dev/fd/0 -R /dev/fd/0 -d0 pty,raw,echo=0,link=/tmp/ttyV0 /dev/ttyACM0,raw,b115200,echo=0
