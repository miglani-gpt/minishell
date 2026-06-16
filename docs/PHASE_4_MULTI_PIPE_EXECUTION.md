# Phase 4: Multi-Pipe Execution

Phase 4 replaces the old two-command-only pipe executor with a general pipeline executor that can run any foreground command chain produced by the Phase 3 parser.

## What changed

- Replaced the old left/right pipe implementation with an N-command pipeline loop.
- `execute_piped_commands()` now receives the parsed command count.
- The executor now sends every pipeline with two or more commands to the pipe module.
- The pipe module creates `command_count - 1` pipes.
- Every command in the chain gets its own child process.
- Each child connects its standard input/output to the correct neighboring pipe.
- Parent process closes all pipe descriptors and waits for every child.
- Built-ins can now run inside pipeline children.
- Existing command-attached redirections still work inside pipelines.

## Supported examples

```bash
ls | wc -l
printf 'a\nb\nc\n' | cat | wc -l
cat README.md | grep MiniShell | wc -l
cat < input.txt | grep hello | wc -l > count.txt
printf abc|cat|wc -c
pwd | wc -l
```

## Pipeline descriptor rules

For a command chain like:

```bash
cmd1 | cmd2 | cmd3 | cmd4
```

MiniShell creates three pipes:

```text
pipe 0: cmd1 stdout -> cmd2 stdin
pipe 1: cmd2 stdout -> cmd3 stdin
pipe 2: cmd3 stdout -> cmd4 stdin
```

The child setup is:

```text
First command:
  stdout -> pipe[0][1]

Middle command:
  stdin  <- previous pipe read end
  stdout -> next pipe write end

Last command:
  stdin  <- previous pipe read end
```

After `dup2()`, every child closes all original pipe file descriptors. The parent also closes all pipe file descriptors after forking all children.

## Redirection behavior inside pipelines

Redirections are applied after pipe descriptors are connected. This matches the practical shell rule that explicit file redirection can override the pipeline side of a command.

Example:

```bash
cat < names.txt | grep mango | wc -c > mango_count.txt
```

The first command reads from `names.txt`; the final command writes to `mango_count.txt`.

Another example:

```bash
printf pipe | wc -c < right_input.txt
```

The right command's input redirection overrides its pipe input, so `wc` reads from `right_input.txt`.

## Current limitation

Background pipelines are still intentionally rejected:

```bash
printf a | cat | wc -c &
```

MiniShell prints a clear message instead of trying to run this partially:

```text
minishell: background execution not supported for pipelines yet
```

Background pipeline support belongs in a later job-control/background-process phase.

## Verification

Run:

```bash
make fclean
make
make test
```

Expected result after Phase 4:

```text
Passed: 38
Failed: 0
All tests passed.
```
