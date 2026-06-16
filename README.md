# MiniShell

MiniShell is a modular Unix-like command-line shell written in C.

This project is built to understand how traditional Unix shells work internally using low-level Linux system calls such as `fork()`, `execvp()`, `waitpid()`, `pipe()`, `dup2()`, `open()`, and `chdir()`.

The goal of MiniShell is to build a small but well-structured shell that supports command execution, built-in commands, input/output redirection, multi-pipe command chaining, background processes, and automated testing.

---

## Features

### Core Shell

* Custom shell prompt
* Displays the current working directory in the prompt
* Reads user input using `getline()`
* Handles EOF / Ctrl+D cleanly
* Trims leading and trailing whitespace before execution
* Treats tabs and other standard whitespace as argument separators
* Parses simple commands and arguments
* Executes external Linux commands
* Handles empty input safely
* Exits cleanly using the `exit` command

### Built-in Commands

MiniShell currently supports the following built-in commands:

| Command          | Description                          |
| ---------------- | ------------------------------------ |
| `cd [directory]` | Change the current working directory |
| `pwd`            | Print the current working directory  |
| `help`           | Display MiniShell help information   |
| `clear`          | Clear the terminal screen            |
| `exit`           | Exit the shell                       |

### External Command Execution

MiniShell can run standard Linux commands such as:

```bash
ls
ls -l
date
whoami
echo hello
cat README.md
wc -l README.md
```

External commands are executed using the traditional Unix process model:

```text
fork()  -> create child process
execvp() -> replace child process with requested program
waitpid() -> parent waits for foreground process
```

### Input and Output Redirection

MiniShell supports output redirection:

```bash
echo hello > output.txt
echo world >> output.txt
ls -l > files.txt
pwd > path.txt
```

Supported output operators:

| Operator | Meaning                                                  |
| -------- | -------------------------------------------------------- |
| `>`      | Redirect output to a file and overwrite existing content |
| `>>`     | Redirect output to a file and append to existing content |

MiniShell also supports input redirection:

```bash
wc -l < names.txt
sort < names.txt
cat < README.md
```

Combined input and output redirection is also supported:

```bash
sort < names.txt > sorted.txt
wc -l < names.txt >> count.txt
```

### Multi-Pipe Support

MiniShell supports pipe chains with two or more commands:

```bash
ls | wc -l
ls src | grep .c
cat README.md | wc -l
cat README.md | grep MiniShell
cat README.md | grep MiniShell | wc -l
printf 'a\nb\nc\n' | cat | wc -l
```

Pipelines with output redirection are also supported:

```bash
cat README.md | grep MiniShell > matches.txt
cat README.md | grep MiniShell | wc -l > match_count.txt
```

Pipelines with input redirection are supported:

```bash
cat < README.md | wc -l
cat < README.md | grep MiniShell | wc -l
```

### Background Processes

MiniShell supports simple background processes using `&`:

```bash
sleep 10 &
echo hello > bg_test.txt &
```

When a command is run in the background, the shell immediately returns to the prompt and displays the background process ID.

Example:

```bash
minishell:/home/user/minishell> sleep 10 &
[background pid: 12345]
minishell:/home/user/minishell>
```

---

## Project Structure

```text
minishell/
├── include/
│   ├── background.h
│   ├── builtins.h
│   ├── executor.h
│   ├── parser.h
│   ├── pipes.h
│   ├── redirection.h
│   └── shell.h
│
├── src/
│   ├── background.c
│   ├── builtins.c
│   ├── executor.c
│   ├── main.c
│   ├── parser.c
│   ├── pipes.c
│   ├── redirection.c
│   └── shell.c
│
├── tests/
│   └── test_minishell.sh
│
├── docs/
│   ├── PHASE_0_BUILD_RELIABILITY.md
│   └── PHASE_1_STRONG_INPUT_HANDLING.md
│
├── Makefile
├── README.md
├── .editorconfig
└── .gitignore
```

---

## Module Responsibilities

| File                | Responsibility                                         |
| ------------------- | ------------------------------------------------------ |
| `src/main.c`        | Starts the shell                                       |
| `src/shell.c`       | Handles prompt, input reading, and the main shell loop |
| `src/parser.c`      | Parses user input into command arguments               |
| `src/executor.c`    | Decides how commands should be executed                |
| `src/builtins.c`    | Handles built-in commands                              |
| `src/redirection.c` | Handles input and output redirection                   |
| `src/pipes.c`       | Handles two-command and multi-command pipeline execution |
| `src/background.c`  | Detects background process syntax using `&`            |

---

## System Calls and C Library Functions Used

| Function/System Call | Purpose                                                       |
| -------------------- | ------------------------------------------------------------- |
| `getline()`          | Reads a complete line of input from the user                  |
| `memmove()`           | Normalizes input after trimming leading whitespace            |
| `strlen()`            | Measures input length during cleanup                          |
| `strtok()`           | Splits user input into tokens                                 |
| `malloc()`           | Dynamically allocates memory                                  |
| `free()`             | Releases dynamically allocated memory                         |
| `fork()`             | Creates a child process                                       |
| `execvp()`           | Executes an external command                                  |
| `waitpid()`          | Waits for foreground processes or checks background processes |
| `pipe()`             | Creates communication channels between pipeline processes      |
| `dup()`              | Saves a copy of a file descriptor                             |
| `dup2()`             | Redirects standard input or output                            |
| `open()`             | Opens files for redirection                                   |
| `close()`            | Closes file descriptors                                       |
| `chdir()`            | Changes the current working directory                         |
| `getcwd()`           | Gets the current working directory                            |
| `getenv()`           | Reads environment variables such as `HOME`                    |
| `perror()`           | Prints system error messages                                  |

---

## Phase Documentation

Detailed upgrade notes are available in:

```text
docs/PHASE_0_BUILD_RELIABILITY.md
docs/PHASE_1_STRONG_INPUT_HANDLING.md
```

---

## Build Instructions

To compile MiniShell, run:

```bash
make
```

This creates an executable named:

```bash
minishell
```

Useful build targets:

| Command | Purpose |
| ------- | ------- |
| `make` | Build the shell |
| `make run` | Build and run the shell |
| `make test` | Build and run automated tests |
| `make debug` | Rebuild with debug settings |
| `make sanitize` | Rebuild with AddressSanitizer and UBSan |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and executable |
| `make re` | Full rebuild |
| `make help` | Show available build targets |

Phase 0 build-reliability notes are documented in:

```bash
docs/PHASE_0_BUILD_RELIABILITY.md
```

---

## Run Instructions

After building the project, run:

```bash
./minishell
```

You should see a prompt similar to:

```bash
minishell:/home/user/minishell>
```

---

## Clean Build Files

To remove the compiled executable, run:

```bash
make clean
```

---

## Run Automated Tests

MiniShell includes automated integration tests.

To run the tests:

```bash
make test
```

The Makefile runs the test script with `bash`, so tests still work even if executable permissions are lost while sharing the project as a zip file.

The test script can also be run directly:

```bash
bash tests/test_minishell.sh
```

The tests check features such as:

* External command execution
* Built-in commands
* Output redirection
* Append redirection
* Input redirection
* Combined input and output redirection
* Quote-aware parsing
* Operators without surrounding spaces
* Single-pipe execution
* Multi-pipe execution
* Pipeline redirection behavior
* Built-ins inside pipelines
* Background process detection

Example test output:

```text
Running MiniShell tests...

[PASS] external command: echo
[PASS] built-in command: pwd
[PASS] built-in command: help
[PASS] output redirection overwrite
[PASS] output redirection append
[PASS] input redirection
[PASS] input plus output redirection
[PASS] single pipe
[PASS] pipe plus output redirection
[PASS] multi-pipe command chain
[PASS] no-space multi-pipe command chain
[PASS] built-in inside pipeline
[PASS] multi-pipe plus output redirection
[PASS] background process

Test summary:
Passed: 38
Failed: 0

All tests passed.
```

---

## Example Usage

```bash
$ make
gcc -Wall -Wextra -g -Iinclude src/main.c src/shell.c src/parser.c src/executor.c src/builtins.c src/redirection.c src/pipes.c src/background.c -o minishell

$ ./minishell
minishell:/home/user/minishell> pwd
/home/user/minishell

minishell:/home/user/minishell> echo hello
hello

minishell:/home/user/minishell> echo hello > output.txt

minishell:/home/user/minishell> cat output.txt
hello

minishell:/home/user/minishell> echo world >> output.txt

minishell:/home/user/minishell> cat output.txt
hello
world

minishell:/home/user/minishell> wc -l < output.txt
2

minishell:/home/user/minishell> ls src | grep .c | wc -l
10

minishell:/home/user/minishell> cat < README.md | grep MiniShell | wc -l > count.txt

minishell:/home/user/minishell> cat count.txt
12

minishell:/home/user/minishell> sleep 10 &
[background pid: 12345]

minishell:/home/user/minishell> exit
```

---

## Current Limitations

MiniShell is still under development. The current version has the following limitations:

* Background pipelines are not fully supported yet.

Simple background commands work:

```bash
sleep 10 &
```

But background pipelines are intentionally rejected with a clear message:

```bash
ls | grep .c | wc -l &
```

* Finished background processes are not yet fully cleaned using `SIGCHLD`.

This will be handled in a future signal-handling/background-process phase.

* Environment variable expansion is not implemented yet.

Not yet supported:

```bash
echo $HOME
echo "$PATH"
echo $?
```

* Advanced shell grammar is not implemented yet.

Not yet supported:

```bash
cmd1 && cmd2
cmd1 || cmd2
cmd1; cmd2
cat << EOF
```

---

## Roadmap

* [x] Basic shell prompt
* [x] Read user input
* [x] Parse simple commands
* [x] Execute external commands
* [x] Add built-in commands
* [x] Add modular project structure
* [x] Add output redirection using `>`
* [x] Add append redirection using `>>`
* [x] Add input redirection using `<`
* [x] Support combined input and output redirection
* [x] Add single pipe support using `|`
* [x] Add support for multiple pipes
* [x] Add simple background process support using `&`
* [x] Add automated integration tests
* [ ] Add signal handling for `Ctrl+C`
* [ ] Add `SIGCHLD` handling for background process cleanup
* [ ] Add command history
* [x] Improve parser to handle symbols without spaces
* [x] Add support for quoted strings
* [ ] Add support for background pipelines
* [x] Add more unit tests for parser and command handling

---

## Learning Outcomes

This project demonstrates core concepts of Linux system programming and C development:

* Unix process model
* Parent and child processes
* Process creation using `fork()`
* Program execution using `execvp()`
* Foreground and background process handling
* File descriptor manipulation
* Input and output redirection
* Inter-process communication using pipes
* Built-in shell command design
* Modular C project organization
* Makefile-based build system
* Bash-based integration testing
* Error handling in system-level programs

---

## Requirements

MiniShell requires a Unix-like environment.

Recommended environments:

* Linux
* Ubuntu
* Ubuntu on WSL
* macOS

Not recommended:

* Native Windows Command Prompt
* Native Windows PowerShell

This project uses Unix-specific system calls such as `fork()`, `execvp()`, `waitpid()`, `pipe()`, and `dup2()`.

---

## Author

Satvik Miglani

---

## License

This project is currently created for learning and portfolio development.

A license may be added later.
