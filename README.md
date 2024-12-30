# My_Bash--Simple_UNIX_Shell
Laboratorio desarrollado grupalmente del cursado de Sistema Operativo - FAMAF

## Description
MyBash is a simple Unix shell (command-line interpreter) implemented in C. It provides basic functionalities for executing commands in foreground and background, input and output redirection, and command piping. The goal of this project is to understand the mechanisms of concurrency and communication provided by UNIX and to implement a shell that reflects the architecture and internal structure of communication and concurrency primitives.

## Objectives
- Utilize the coarse-grained concurrency and communication mechanisms provided by UNIX.
- Understand that a command-line interpreter reflects the architecture and internal structure of communication and concurrency primitives.
- Implement a simple command-line interpreter (shell).
- Follow good programming practices such as code style, abstract data types (ADTs), unit testing, black box testing, defensive programming, and the use of debugging and memory tools.

## Features
- Execution of commands in foreground and background.
- Input and output redirection.
- Command piping.
- Internal commands: `cd`, `help`, and `exit`.
- Support for exiting with CTRL-D (End of Transmission character).
- Robustness against incomplete or invalid inputs.

## Examples
MyBash is capable of correctly executing the following examples:
```
ls -l mybash.c
ls 1 2 3 4 5 6 7 8 9 10 11 12 ... 194
wc -l > out < in
/usr/bin/xeyes &
ls | wc -l
```

## Implementation Details
- Usage of opaque abstract data types (ADTs).
- Memory management to prevent memory leaks.
- Following good programming practices such as coding style, modularization, proper commenting, meaningful variable names, idiomatic consistency, etc.

Please note that this implementation adheres to basic functionality and may not support all advanced features found in a full-fledged shell like Bash.
