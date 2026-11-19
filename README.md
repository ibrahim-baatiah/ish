# ish — Ibrahim Shell

A Unix shell built from scratch in C, as a learning project to understand how shells, processes, and the kernel actually work under the hood.

## Features

- Interactive REPL with a configurable, bash-style prompt (`PS1` escapes: `\u`, `\h`, `\w`, `\t`, `\$`, `\#`, variable expansion, octal escapes, and more)
- Line reading with a dynamically growing buffer (no fixed-size limits)
- Tokenizing / argument parsing
- Process execution via `fork()` / `execvp()` / `waitpid()`
- Built-in commands: `cd`, `exit`, `help` (run in the shell's own process, never forked)
- Script file execution: `./ish script.sh`, including shebang support and `#` comments
- I/O redirection: `<` (input) and `>` (output), including combined use in one command
- Shell variables: `NAME=value` assignment and `$NAME` expansion, with fallback to real environment variables (`$HOME`, `$PATH`, ...)
- Pipes: `cmd1 | cmd2 | cmd3`, any number of stages in series

## In progress

- Command history (listing/recall; real up-arrow terminal navigation is a bigger separate effort)

## Project layout

The code is split by concern, not left in one file:

| File | Responsibility |
|---|---|
| `main.c` | `main()`, the main read-execute loop |
| `input.c` | Reading lines, tokenizing |
| `builtins.c` | `cd`, `exit`, `help` |
| `exec.c` | Dispatch, `fork`/`execvp`/`waitpid` |
| `redirection.c` | `<` / `>` handling |
| `pipe.c` | Multi-stage pipelines |
| `vars.c` | Shell variable storage, lookup, `$` expansion |
| `prompt.c` | `PS1` prompt rendering |
| `ish.h` | Shared prototypes |

## Build

```sh
make
```
or manually:
```sh
gcc -o ish main.c input.c builtins.c exec.c vars.c pipe.c redirection.c prompt.c -Wall
```

## Usage

Interactive mode:
```sh
./ish
```

Run a script:
```sh
./ish myScript
```

Or make the script self-executing with a shebang pointing at `ish`, then run it directly:
```sh
#!/path/to/ish
pwd
ls
```
```sh
chmod +x myScript
./myScript
```

Set a shell variable and use it:
```sh
NAME=John
echo $NAME
```

Pipe commands together:
```sh
ls | grep c | wc -l
```

## Why

This project follows the classic ["Write a Shell in C"](https://brennan.io/2015/01/16/write-a-shell-in-c/) tutorial as a reference, implemented step by step to actually understand `fork`/`exec`, process control, file descriptors, and parsing — not to just copy the tutorial's code.

## References

- [Write a Shell in C — Stephen Brennan](https://brennan.io/2015/01/16/write-a-shell-in-c/)
- *The C Programming Language* (K&R)
- *Operating Systems: Three Easy Pieces* (OSTEP) — especially the chapters on system calls, processes, and scheduling
- [Let's Make a Shell! — Barry Brown (live stream)](https://www.youtube.com/watch?v=_pc7pVvAoCg)
- [xv6 #30: File Descriptors, Open Files, Pipes — hhp3](https://www.youtube.com/watch?v=URSS_BomPa8)
- [Simulating the pipe "|" operator in C — CodeVault](https://www.youtube.com/watch?v=6xbLgZpOBi8)
- [CS 162 HW2 — Pipes (UC Berkeley)](https://cs162.org/static/hw/hw-shell/docs/pipes/)
