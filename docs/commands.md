# ActionStroke

This document lists all available commands for ActionStroke macros, along with their arguments, types, and descriptions.

## Table of Contents

- [Commands](#command-syntax)
- [MoveMouse](#movemouse)
- [MoveMouseTo](#movemouseto)
- [Sleep](#sleep)

---

## Command Syntax

Commands follow this general pattern:
COMMAND_NAME arg1=value arg2=value ...

---

### MoveMouse

*Set the mouse position to a given location on the main screen.*

```text
    MoveMouse (x) (y)
    x (int) -> X cooridinate
    y (int) -> Y coordinate
```

---

### MoveMouseTo

*Move the mouse to a given location from the current location on the main screen.*

```text
    MoveMouseTo (x) (y) (ms)
    x (int)  -> X cooridinate
    y (int)  -> Y coordinate
    ms (int) -> Time in ms to travel to the given x,y location
```

---

### Sleep

*Wait for n miliseconds.*

```text
    Sleep (ms)
    ms (int) -> Time in ms to wait
```
