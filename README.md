# MiniShell

MiniShell is a simple Unix-like command-line shell written in C.

This project is being built step by step to understand how real shells work internally using Linux system calls such as `fork()`, `execvp()`, and `waitpid()`.

The goal of this project is to build a small but well-structured shell that gradually supports command execution, built-in commands, redirection, pipes, background processes, command history, and signal handling.

---

## Features Implemented

* Displays a custom shell prompt
* Reads user input using `getline()`
* Parses simple commands and arguments
* Executes external Linux commands
* Creates child processes using `fork()`
* Executes programs using `execvp()`
* Waits for foreground commands using `waitpid()`
* Supports the `exit` command
* Handles empty input safely
* Uses dynamic memory allocation for command input and arguments

---

## Commands Tested

The current version supports simple commands such as:

```bash
ls
ls -l
pwd
date
whoami
echo hello
clear
exit
```

Example:

```bash
$ ./minishell
minishell> pwd
/home/user/minishell
minishell> echo hello
hello
minishell> ls
main.c  Makefile  README.md
minishell> exit
```

---

## Project Structure

```text
minishell/
├── main.c
├── Makefile
├── README.md
└── .gitignore
```

---

## How It Works

MiniShell follows the basic working model of a Unix shell:

```text
1. Print a prompt
2. Read user input
3. Parse the input into command and arguments
4. Create a child process using fork()
5. Execute the command in the child process using execvp()
6. Parent process waits for the child using waitpid()
7. Repeat until the user types exit
```

---

## Build Instructions

To compile the project, run:

```bash
make
```

This will create an executable named:

```bash
minishell
```

---

## Run Instructions

After building the project, run:

```bash
./minishell
```

You should see the shell prompt:

```bash
minishell>
```

Now you can type commands such as:

```bash
ls
pwd
echo hello
date
exit
```

---

## Clean Build Files

To remove the compiled executable, run:

```bash
make clean
```

---

## System Calls and Functions Used

| Function/System Call | Purpose                                             |
| -------------------- | --------------------------------------------------- |
| `getline()`          | Reads a complete line of input from the user        |
| `strtok()`           | Splits the input into command and arguments         |
| `malloc()`           | Dynamically allocates memory for argument storage   |
| `free()`             | Releases dynamically allocated memory               |
| `fork()`             | Creates a child process                             |
| `execvp()`           | Executes an external command                        |
| `waitpid()`          | Makes the parent process wait for the child process |
| `strcmp()`           | Compares command strings                            |
| `perror()`           | Prints system error messages                        |

---

## Important Concepts Learned

This project demonstrates fundamental concepts of system programming in C:

* Process creation
* Parent and child processes
* Command execution
* Linux system calls
* Dynamic memory management
* Basic command parsing
* Error handling
* Shell loop design

---

## Current Limitations

The current version is an early Phase 1 implementation.

It does not yet support:

* `cd` command
* Pipes using `|`
* Input redirection using `<`
* Output redirection using `>`
* Append redirection using `>>`
* Background processes using `&`
* Signal handling such as `Ctrl+C`
* Command history
* Quoted strings such as `"hello world"`

These features will be added in future phases.

---

## Roadmap

* [x] Basic shell prompt
* [x] Read user input
* [x] Parse simple commands
* [x] Execute external commands
* [x] Add `exit` command
* [ ] Add built-in commands like `cd`, `pwd`, `help`, and `clear`
* [ ] Add input and output redirection
* [ ] Add pipe support
* [ ] Add background process support
* [ ] Add command history
* [ ] Add signal handling
* [ ] Improve parser to handle quotes and special symbols
* [ ] Add modular project structure with separate source and header files

---

## Future Improvements

Planned improvements include:

* Better command parser
* Support for multiple pipes
* File redirection
* Built-in command system
* Persistent command history
* Signal handling using `sigaction()`
* Background job control
* Cleaner modular codebase
* Unit tests for parser and built-ins

---

## Example Session

```bash
$ make
gcc -Wall -Wextra -g main.c -o minishell

$ ./minishell
minishell> whoami
user
minishell> pwd
/home/user/minishell
minishell> echo MiniShell is running
MiniShell is running
minishell> ls
main.c  Makefile  README.md
minishell> exit
```

---

## Requirements

This project requires a Unix-like environment.

Recommended environments:

* Linux
* Ubuntu
* Ubuntu on WSL
* macOS

Not recommended:

* Native Windows CMD
* Native Windows PowerShell

This is because the project uses Unix-specific system calls such as `fork()`, `execvp()`, and `waitpid()`.

---

## Author

Satvik Miglani

---

## License

This project is currently for learning and portfolio development.
A license may be added later.
