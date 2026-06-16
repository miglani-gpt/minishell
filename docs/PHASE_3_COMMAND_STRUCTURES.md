# Phase 3: Proper Command Data Structures

Phase 3 separates parsed shell meaning from raw argument text.

Earlier phases converted input into a flat `char **args` array. That worked for a small shell, but it mixed real command arguments with shell syntax tokens such as `<`, `>`, `>>`, `|`, and `&`.

This phase introduces proper command structures so MiniShell can reason about commands, redirections, and command chains separately.

## What changed

- Added `include/command.h` and `src/command.c`.
- Added `t_command` to represent one command.
- Added `t_redirection` to represent input/output redirections attached to a command.
- Changed the parser so it now returns a `t_parsed_input` structure containing:
  - a linked list of commands,
  - the number of parsed commands,
  - whether the input should run in the background.
- Updated the executor to run structured commands instead of raw argument arrays.
- Updated redirection handling so redirection tokens are no longer removed from `argv` during execution.
- Updated pipe execution to use two structured command objects.
- Added Phase 3 tests for structured redirection and pipe-side ownership.

## New structures

```c
typedef enum e_redir_type
{
    REDIR_INPUT,
    REDIR_OUTPUT,
    REDIR_APPEND
} t_redir_type;

typedef struct s_redirection
{
    t_redir_type type;
    char *filename;
    struct s_redirection *next;
} t_redirection;

typedef struct s_command
{
    char **argv;
    int argc;
    int argv_capacity;
    t_redirection *redirections;
    struct s_command *next;
} t_command;
```

## Example

Input:

```bash
cat < input.txt | grep hello > output.txt
```

Phase 3 stores it as:

```text
Parsed input
├── is_background = 0
├── command_count = 2
└── commands
    ├── Command 1
    │   ├── argv = ["cat"]
    │   └── redirections
    │       └── REDIR_INPUT: input.txt
    └── Command 2
        ├── argv = ["grep", "hello"]
        └── redirections
            └── REDIR_OUTPUT: output.txt
```

## Why this matters

This structure is important because future phases need command metadata, not just strings.

It prepares MiniShell for:

- multi-pipe execution,
- cleaner redirection handling,
- better syntax validation,
- environment variable expansion,
- command status tracking,
- more precise memory cleanup.

## Current limitation

Phase 3 can parse more than two commands, but execution still supports only one pipe. Full N-pipe execution belongs to Phase 4.

So this is still intentionally rejected during execution:

```bash
cat file.txt | grep hello | wc -l
```

Expected message:

```text
minishell: multiple pipes are not supported yet
```

## Verification

Run:

```bash
make fclean
make
make test
```

Expected result after Phase 3:

```text
Passed: 33
Failed: 0
All tests passed.
```
