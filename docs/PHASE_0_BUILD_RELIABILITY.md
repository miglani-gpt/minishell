# Phase 0: Repo Cleanup and Build Reliability

This phase does not change MiniShell features. It makes the repository easier to build, test, clean, and share.

## What changed

- Reworked the Makefile into named targets.
- Added object-file compilation into `build/`.
- Added `.PHONY` targets to avoid name conflicts.
- Added `fclean`, `re`, `debug`, `sanitize`, `check`, and `help` targets.
- Changed `make test` to run the test script with `bash`, so the executable bit is not required after zipping or sharing.
- Updated the test script with a `--no-build` mode to avoid rebuilding twice during `make test`.
- Expanded `.gitignore` for build outputs, debug files, editor files, local environment files, and test artifacts.
- Added `.editorconfig` for consistent formatting across editors.

## Commands

```bash
make
make run
make test
make sanitize
make clean
make fclean
make re
make help
```

## Verification

After applying this phase, these commands should work:

```bash
make fclean
make
make test
```

Expected test result:

```text
Passed: 10
Failed: 0
All tests passed.
```
