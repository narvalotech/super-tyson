# Disclaimer

The repo is used for "working in public".
The project itself is not yet usable, but feel free to browse the oft-changing code.

I will remove this disclaimer when the GUI MVP milestone (below) is done.

# What

Super-Tyson (ST) is a micro-ide for [micro lisp](http://www.ulisp.com/).

![Editor screenshot](super-tyson.png)

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

- [x] connect to device
- [x] evaluate s-exp
- [x] evaluate "region" (ie multiple s-exp)
- [x] evaluate file

## GUI MVP

GUI should be pluggable, as to have multiple backends.

Use a "server" architecture, where the server connects to the target and does
the comms and parsing. GUI should just present the data.

- [ ] serial connection dialog / bar
- [x] editor window
- [x] REPL window/terminal
- [x] send editor contents button

## Extra features

- [ ] save/restore memory to disk
- [ ] show all lambdas and variables
- [ ] "scratchpad" text area
- [ ] editor tabs
- [ ] wifi REPL
