# scratchpad

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
- add "Run selection" button
- highlight matching parens
- allow evaluating more than 1 s-exp
