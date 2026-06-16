# Phase 2: Lexer and Parser Rewrite

Phase 2 replaces the old `strtok()`-based parser with a proper lexer and parser layer.

The old parser could split simple commands by whitespace, but it did not understand shell operators unless spaces were placed around them. It also could not preserve quoted strings as a single argument.

The new flow is:

```text
raw input line
    ↓
lexer
    ↓
token linked list
    ↓
parser validation
    ↓
char **args for the existing executor
```

This phase intentionally keeps the executor interface compatible. The parser still produces `char **args`, so the current redirection, pipe, background, built-in, and external execution code can continue working.

---

## New Files

```text
include/lexer.h
src/lexer.c
docs/PHASE_2_LEXER_PARSER.md
```

---

## Changed Files

```text
include/parser.h
src/parser.c
src/shell.c
Makefile
tests/test_minishell.sh
README.md
```

---

## Token Types

The lexer now recognizes these token types:

```c
typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_REDIR_APPEND,
    TOKEN_BACKGROUND
} t_token_type;
```

---

## Supported Parsing Improvements

### Operators no longer need spaces

These now work:

```bash
echo hello>output.txt
echo hello>>output.txt
wc -l<input.txt
printf abc|wc -c
sleep 1&
```

The lexer turns operators into separate tokens even when attached to words.

Example:

```bash
echo hello>output.txt
```

becomes:

```text
WORD(echo)
WORD(hello)
REDIR_OUT(>)
WORD(output.txt)
```

---

### Quoted strings stay together

These now work:

```bash
echo "hello world"
echo 'single quoted text'
```

The parser passes the quoted content as one argument, without the surrounding quote characters.

Example:

```bash
echo "hello world"
```

becomes:

```text
argv[0] = echo
argv[1] = hello world
```

---

### Operators inside quotes are preserved as normal text

This works correctly:

```bash
echo "left|right"
```

The `|` is not treated as a pipe because it is inside quotes.

---

### Empty quoted arguments are preserved

This works:

```bash
printf 'x%sx\n' ""
```

The empty quotes become a real empty string argument.

---

## Parser Validation

The parser now rejects malformed syntax before execution.

Examples:

```bash
| wc -l
echo hello |
echo hello >
echo "unfinished
sleep 1 & echo done
```

These produce syntax errors instead of being passed blindly to the executor.

---

## Memory Ownership

The parser now returns a structure instead of a raw `char **`:

```c
typedef struct s_parsed_input
{
    char **args;
    char **owned_values;
    int owned_count;
} t_parsed_input;
```

Why this is useful:

* `args` can still be passed to the old executor.
* `owned_values` keeps ownership of every allocated argument string.
* Even if the executor mutates `args`, cleanup remains safe.

The shell loop now calls:

```c
t_parsed_input parsed = parse_input(line);

if (parsed.args != NULL)
{
    execute_command(parsed.args);
}

free_parsed_input(&parsed);
```

---

## What This Phase Does Not Add Yet

Phase 2 does not add a full command AST yet. That comes in Phase 3.

Still limited:

* only one pipe is supported by the executor
* environment variable expansion is not implemented yet
* escape sequences such as `\"` are not fully handled by the lexer
* here-documents are not implemented
* advanced shell grammar such as `&&`, `||`, and `;` is not implemented

---

## Test Coverage Added

Phase 2 increases automated tests from 15 to 26.

New tests cover:

```text
double quotes
single quotes
empty quoted arguments
operators inside quotes
no-space output redirection
no-space append redirection
no-space input redirection
no-space pipe
no-space background process
missing redirection target syntax error
unclosed quote syntax error
```

Run:

```bash
make test
```

Expected result:

```text
Passed: 26
Failed: 0
All tests passed.
```
