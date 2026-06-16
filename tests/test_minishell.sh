#!/usr/bin/env bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MINISHELL="$PROJECT_ROOT/minishell"
TEST_DIR="$PROJECT_ROOT/tests/test_files"

pass_count=0
fail_count=0

print_pass() {
    echo "[PASS] $1"
    pass_count=$((pass_count + 1))
}

print_fail() {
    echo "[FAIL] $1"
    echo "       Expected: $2"
    echo "       Got:      $3"
    fail_count=$((fail_count + 1))
}

run_shell() {
    printf "%s\nexit\n" "$1" | "$MINISHELL"
}

clean_prompt_output() {
    sed 's/minishell:[^>]*> //g' | sed 's/minishell> //g'
}

assert_contains() {
    local test_name="$1"
    local command="$2"
    local expected="$3"

    output="$(run_shell "$command" | clean_prompt_output)"

    if echo "$output" | grep -q "$expected"; then
        print_pass "$test_name"
    else
        print_fail "$test_name" "$expected" "$output"
    fi
}

assert_file_content() {
    local test_name="$1"
    local file="$2"
    local expected="$3"

    if [ ! -f "$file" ]; then
        print_fail "$test_name" "$expected" "file not found"
        return
    fi

    actual="$(cat "$file")"

    if [ "$actual" = "$expected" ]; then
        print_pass "$test_name"
    else
        print_fail "$test_name" "$expected" "$actual"
    fi
}

BUILD_BEFORE_TEST=1

if [ "${1:-}" = "--no-build" ]; then
    BUILD_BEFORE_TEST=0
fi

if [ "$BUILD_BEFORE_TEST" -eq 1 ]; then
    echo "Building MiniShell..."
    make -C "$PROJECT_ROOT" fclean >/dev/null
    make -C "$PROJECT_ROOT" >/dev/null
else
    echo "Using existing MiniShell build..."
fi

rm -rf "$TEST_DIR"
mkdir -p "$TEST_DIR"

echo
echo "Running MiniShell tests..."
echo

# Basic external command
assert_contains "external command: echo" "echo hello" "hello"

# Blank and whitespace-only input should not be executed as commands
output="$(printf "\n   \n\t\nexit\n" | "$MINISHELL" 2>&1 | clean_prompt_output)"
if echo "$output" | grep -qi "not found\|permission denied\|syntax error"; then
    print_fail "blank and whitespace-only input" "no command execution error" "$output"
else
    print_pass "blank and whitespace-only input"
fi

# Leading and trailing spaces should be ignored
assert_contains "leading and trailing whitespace" "   echo trimmed   " "trimmed"

# Tabs should behave like spaces between arguments
tab_command=$'echo\twith\ttabs'
assert_contains "tab-separated arguments" "$tab_command" "with tabs"

# CRLF input should be accepted cleanly
output="$(printf "echo crlf\r\nexit\r\n" | "$MINISHELL" 2>&1 | clean_prompt_output)"
if echo "$output" | grep -q "crlf"; then
    print_pass "CRLF line ending input"
else
    print_fail "CRLF line ending input" "crlf" "$output"
fi

# EOF / Ctrl+D should exit cleanly
output="$(printf "" | "$MINISHELL" 2>&1 | clean_prompt_output)"
if echo "$output" | grep -q "exit"; then
    print_pass "EOF exits cleanly"
else
    print_fail "EOF exits cleanly" "exit" "$output"
fi

# pwd built-in
assert_contains "built-in command: pwd" "pwd" "$PROJECT_ROOT"

# help built-in
assert_contains "built-in command: help" "help" "MiniShell"

# Output redirection >
run_shell "echo hello > $TEST_DIR/output.txt" >/dev/null
assert_file_content "output redirection overwrite" "$TEST_DIR/output.txt" "hello"

# Output redirection >>
run_shell "echo world >> $TEST_DIR/output.txt" >/dev/null
expected_append="$(printf "hello\nworld")"
assert_file_content "output redirection append" "$TEST_DIR/output.txt" "$expected_append"

# Input redirection
printf "apple\nbanana\nmango\n" > "$TEST_DIR/names.txt"
assert_contains "input redirection" "wc -l < $TEST_DIR/names.txt" "3"

# Combined input and output redirection
run_shell "sort < $TEST_DIR/names.txt > $TEST_DIR/sorted.txt" >/dev/null
expected_sorted="$(printf "apple\nbanana\nmango")"
assert_file_content "input plus output redirection" "$TEST_DIR/sorted.txt" "$expected_sorted"

# Single pipe
assert_contains "single pipe" "printf 'a\nb\nc\n' | wc -l" "3"

# Pipe plus redirection
run_shell "cat $TEST_DIR/names.txt | grep banana > $TEST_DIR/match.txt" >/dev/null
assert_file_content "pipe plus output redirection" "$TEST_DIR/match.txt" "banana"

# Background process
output="$(run_shell "sleep 1 &" | clean_prompt_output)"
if echo "$output" | grep -q "background pid"; then
    print_pass "background process"
else
    print_fail "background process" "background pid" "$output"
fi

# Quoted strings should stay as one argument
assert_contains "double-quoted string" "echo \"hello world\"" "hello world"
assert_contains "single-quoted string" "echo 'single quoted text'" "single quoted text"

# Empty quoted arguments should be preserved
assert_contains "empty quoted argument" "printf 'x%sx\\n' \"\"" "xx"

# Operators inside quotes should not be treated as shell operators
assert_contains "operator inside quotes" "echo \"left|right\"" "left|right"

# Operators should work without surrounding spaces
run_shell "echo nospace>$TEST_DIR/no_space_output.txt" >/dev/null
assert_file_content "no-space output redirection" "$TEST_DIR/no_space_output.txt" "nospace"

run_shell "echo appended>>$TEST_DIR/no_space_output.txt" >/dev/null
expected_no_space_append="$(printf "nospace\nappended")"
assert_file_content "no-space append redirection" "$TEST_DIR/no_space_output.txt" "$expected_no_space_append"

assert_contains "no-space input redirection" "wc -l<$TEST_DIR/names.txt" "3"
assert_contains "no-space pipe" "printf abc|wc -c" "3"

# Background marker should work even when attached to the previous word
output="$(run_shell "sleep 1&" | clean_prompt_output)"
if echo "$output" | grep -q "background pid"; then
    print_pass "no-space background process"
else
    print_fail "no-space background process" "background pid" "$output"
fi

# Parser should reject malformed syntax cleanly
output="$(printf "echo broken >\nexit\n" | "$MINISHELL" 2>&1 | clean_prompt_output)"
if echo "$output" | grep -q "expected file after '>'"; then
    print_pass "parser rejects missing redirection target"
else
    print_fail "parser rejects missing redirection target" "expected file after '>'" "$output"
fi

output="$(printf "echo \"unfinished\nexit\n" | "$MINISHELL" 2>&1 | clean_prompt_output)"
if echo "$output" | grep -q "unclosed"; then
    print_pass "parser rejects unclosed quote"
else
    print_fail "parser rejects unclosed quote" "unclosed quote error" "$output"
fi

echo
echo "Test summary:"
echo "Passed: $pass_count"
echo "Failed: $fail_count"

if [ "$fail_count" -ne 0 ]; then
    exit 1
fi

echo
echo "All tests passed."