# hsh

A simple UNIX command-line interpreter written in C.

## Description

`hsh` is a small shell inspired by `/bin/sh`. It reads commands from standard
input, parses their arguments, searches the `PATH`, creates child processes with
`fork()`, executes external programs with `execve()`, and waits for them.

The shell also implements the `exit`, `cd`, and `env` builtins.

## Compilation

```sh
make
```

The executable produced is:

```text
./hsh
```

## Manual

The manual page is `hsh.1`. From the project directory it can be viewed with:

```sh
man -l hsh.1
```

## Usage

Interactive:

```sh
./hsh
($) ls -l
($) pwd
($) cd /tmp
($) pwd
($) exit
```

Non-interactive:

```sh
echo "ls -l" | ./hsh
cat commands.txt | ./hsh
```

## Builtins

### exit

Terminates the shell. A numeric argument is used as the exit status.

```sh
exit
exit 42
```

### cd

Changes the current working directory.

```sh
cd /tmp
cd
cd -
```

Without an argument, `cd` uses `HOME`. `cd -` changes to the previous
directory and prints the new directory.

### env

Prints the shell environment.

```sh
env
```

## Features

- Interactive and non-interactive modes
- Command arguments
- `PATH` command lookup
- `exit`, `cd`, and `env`
- EOF handling
- Basic command execution errors
- `PWD` and `OLDPWD` updates
- SIGINT handling

## Limitations

The project does not implement:

- pipes
- redirections
- quotes
- command substitution
- wildcard expansion
- logical operators

## Authors

See `AUTHORS.md`.
