# hsh

A simple UNIX command-line interpreter written in C.

## Description

`hsh` is a small shell inspired by `/bin/sh`. It reads commands from standard
input, parses their arguments and redirections, searches the `PATH`, creates
child processes with `fork()`, executes external programs with `execve()`,
and waits for them.

Beyond a single command, `hsh` also understands command lists (`;`, `&&`,
`||`), pipelines (`|`), output/input redirection (`>`, `>>`, `<`), and
here-documents (`<<`).

The shell implements the `exit`, `cd`, `env`, `setenv`, and `unsetenv`
builtins.

## Compilation

```sh
make
```

The executable produced is:

```text
./hsh
```

## Manual

The manual page is `man_1_hsh`. From the project directory it can be viewed
with:

```sh
man ./man_1_hsh
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

Terminates the shell. A numeric argument is used as the exit status
(taken modulo 256).

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
directory (`OLDPWD`) and prints the new directory. `PWD` and `OLDPWD` are
kept up to date after every successful change.

### env

Prints the shell environment.

```sh
env
```

### setenv

Creates a new environment variable, or overwrites the value of an existing
one.

```sh
setenv NAME value
```

### unsetenv

Removes an environment variable.

```sh
unsetenv NAME
```

## Redirections

```sh
ls > out.txt         # overwrite out.txt with the output of ls
ls >> out.txt         # append instead of overwriting
wc -l < out.txt       # read input from out.txt
cat << EOF
some text
EOF
```

A here-document (`<<`) reads lines from standard input until a line matching
the delimiter exactly is found, and feeds everything read as the standard
input of the command.

## Command lists and pipelines

```sh
ls ; pwd                   # run ls, then pwd, regardless of the result
make && ./hsh               # run ./hsh only if make succeeded
ls missing || echo failed   # run echo only if ls failed
ls | grep hsh | wc -l       # chain commands through pipes
```

`&&` and `||` short-circuit on the exit status of the previous command;
`;` always runs the next command. A single `|` chains commands through a
pipe, each running in its own process.

## Features

- Interactive and non-interactive modes
- Command arguments
- `PATH` command lookup
- `exit`, `cd`, `env`, `setenv`, `unsetenv`
- Output/append/input redirection (`>`, `>>`, `<`) and here-documents (`<<`)
- Command separators and logical operators (`;`, `&&`, `||`)
- Pipelines (`|`)
- EOF handling
- Command execution errors matching `sh`-style messages
- `PWD` and `OLDPWD` updates
- SIGINT handling

## Limitations

The project does not implement:

- quotes
- command substitution
- wildcard expansion
- variable expansion (`$VAR`)

## Authors

See `AUTHORS.md`.
