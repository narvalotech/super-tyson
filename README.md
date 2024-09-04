# What

Super-Tyson (ST) is a micro-ide for [micro lisp](http://www.ulisp.com/).

# Why

Needed a project to learn me some C++.
Also ulisp is lacking a small IDE, something like micropython's [Thonny](https://thonny.org/).

# How (to use)

``` sh
# configure
cmake -GNinja -Bbuild -S.

# build
ninja -C build

# run
./build/SuperTyson
```

# Plan

## CLI MVP

- [ ] connect to device
- [ ] evaluate s-exp
- [ ] evaluate "region" (ie multiple s-exp)
- [ ] evaluate file

## GUI MVP

GUI should be pluggable, as to have multiple backends.

Use a "server" architecture, where the server connects to the target and does
the comms and parsing. GUI should just present the data.

- [ ] serial connection dialog / bar
- [ ] editor window
- [ ] REPL window/terminal
- [ ] send editor contents button

## Extra features

- [ ] save/restore memory to disk
- [ ] show all lambdas and variables
- [ ] "scratchpad" text area
- [ ] editor tabs
- [ ] wifi REPL
