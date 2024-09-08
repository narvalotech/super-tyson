# scratchpad

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

# Workflow

-> TDD?
- need to simulate interaction with fake target
- need to learn and integrate + maintain unit-tests
-> could be faster to test manually for now

# REPL sample

example interaction (over serial):

```
uLisp 4.5
12927> (+ (* 3 4) 2.5)
14.5
"some text-with
newlines"

12927>

--- exit ---
```

# notes on GUI

Resizing in the X direction doesn't work.

Weirdly enough, the window is resizable in both direction when no buttons are
added to the layout. It's probably something to do with the button element
itself adding a maxsize constraint.

# next steps

GUI

- fix resizing issue (see above)
- add connection status indicator (statusline maybe?)
- add serial port selector widget
- highlight matching parens
