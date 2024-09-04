#!/usr/bin/env bash

set -eu

ninja -Cbuild

uart=/tmp/st/myid/uart

# It's ok if the FIFO already exists
set +eu

mkdir -p $(dirname ${uart})
mkfifo ${uart} 2>/dev/null

# Print result after the program has started
# Would be better to have an actual simlator program
sleep 0.5 && printf "14.5\n\"some text with\nnewlines\"\n12927>\n" > ${uart} &

# exec cat < ${uart} &
# exec cat > ${uart} &

echo "## Start program ##"
echo "(+ 1 2 (* 3 4))" | ./build/SuperTyson ${uart}
