# Phase 1: Strong Input Handling

Phase 1 hardens MiniShell's interactive input path without changing the shell's feature set.

The purpose of this phase is simple: the shell should not behave badly when the user enters empty input, extra spaces, tabs, Windows-style line endings, or EOF.

---

## What Changed

### 1. Empty input is ignored safely

The shell now handles blank lines without sending empty commands to the executor.

Examples:

```bash
minishell:/path> 
minishell:/path>       
minishell:/path>
```

The shell simply prints a fresh prompt.

---

### 2. Leading and trailing whitespace is trimmed

Commands like this:

```bash
   echo hello   
```

are normalized before parsing, so they behave like:

```bash
echo hello
```

---

### 3. Tabs are treated as argument separators

The parser now treats standard shell whitespace as separators:

```text
space
horizontal tab
carriage return
vertical tab
form feed
```

This means commands like this work correctly:

```bash
echo	 hello
```

---

### 4. CRLF input is handled

The input reader removes both Unix and Windows line endings.

Supported endings:

```text
\n
\r\n
```

This makes piped test input more reliable across platforms.

---

### 5. Ctrl+D exits cleanly

When the user sends EOF using Ctrl+D, MiniShell now exits cleanly and prints:

```text
exit
```

This is closer to traditional shell behavior.

---

### 6. getline errors are reported

If `getline()` fails because of an actual input error, MiniShell reports it with:

```text
minishell: getline: <system error>
```

EOF is not treated as an error.

---

## Files Changed

```text
src/shell.c
src/parser.c
tests/test_minishell.sh
docs/PHASE_1_STRONG_INPUT_HANDLING.md
README.md
```

---

## Testing

Run:

```bash
make test
```

Phase 1 adds tests for:

```text
blank input
whitespace-only input
leading and trailing spaces
tab-separated arguments
CRLF line endings
EOF / Ctrl+D behavior
```

---

## What Phase 1 Does Not Try to Solve

This phase does not replace the parser with a full lexer. These are still future phases:

```text
quoted strings
operators without spaces, such as echo hello>file.txt
multi-pipe commands
variable expansion
signal handling
```

Those belong in later phases because they require a proper lexer and command structure.
