# ish

A Unix shell built from scratch in C, as a learning project to understand how shells actually work under the hood.

## Features

- Interactive REPL with prompt
- Line reading with dynamically growing buffer (no fixed-size limits)
- Tokenizing / argument parsing
- Process execution via `fork()` / `execvp()` / `waitpid()`
- Built-in commands: `cd`, `exit`, `help`
- Script file execution: `./ish script.sh`
- Comments in scripts (`#`)
- I/O redirection: `<` (input) and `>` (output), including combined use in one command

## In progress

- Pipes (`cmd1 | cmd2`)
- Shell variables (`VAR=value`, `$VAR` expansion)
- Command history

## Build

```sh
gcc -o ish main.c -Wall
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

## Why

This project follows the classic ["Write a Shell in C"](https://brennan.io/2015/01/16/write-a-shell-in-c/) tutorial as a reference, implemented step-by-step to actually understand `fork`/`exec`, process control, and parsing — not just copy the tutorial's code.

## References

- *The C Programming Language* (K&R)
- *Operating Systems: Three Easy Pieces* (OSTEP) — especially helpful for system calls and low-level understanding of processes, concurrency, and scheduling
