# PiPinPP Scripts

This directory contains utility scripts for maintaining the PiPinPP project.

## check_markdown_links.py

Validates all internal markdown links in the documentation.

### Usage

```bash
python3 scripts/check_markdown_links.py
```

### What it does

- Scans all `.md` files in the `docs/` directory and `README.md`
- Extracts all markdown links (format: `[text](url)`)
- Ignores links inside code blocks (to avoid false positives)
- Checks that all internal file links point to existing files
- Skips external links (http://, https://), mailto links, and anchor-only links

### Exit codes

- `0` - All links are valid
- `1` - One or more broken links found

### Example output

```
================================================================================
MARKDOWN LINK VERIFICATION REPORT
================================================================================

Total markdown files checked: 22
Total links found: 155
Broken links found: 0

✅ All local links are valid!
```

### CI Integration

This script runs automatically in GitHub Actions:
- On every push/PR (via `.github/workflows/ci.yml`)
- On documentation builds (via `.github/workflows/documentation.yml`)

### Development

When adding new documentation or modifying existing docs, run this script locally
before committing to catch broken links early:

```bash
# From repository root
python3 scripts/check_markdown_links.py
```

If broken links are found, the script will show:
- Which file contains the broken link
- The line number of the link
- What the link text and URL are
- Where the link was expected to point

### Implementation details

- Written in Python 3 (standard library only, no dependencies)
- Uses regex to parse markdown links
- Resolves relative paths using `pathlib`
- Handles edge cases like anchor links (#section) and external URLs

## run_examples.sh

Batch-builds every example target (`example_<name>`) and can optionally run the resulting binaries.

### Usage

```bash
# Build everything
scripts/run_examples.sh

# Build only PWM-related examples and run them after build
scripts/run_examples.sh --filter "pwm*" --execute

# List detected examples without building
scripts/run_examples.sh --list
```

Key options:
- `-x, --execute` – run binaries after building (output appended to the log)
- `-f, --filter <glob>` – select specific example directories (e.g., `pwm*`, `i2c_*`)
- `-l, --list` – print the example names and exit
- `-j, --jobs <N>` – pass `--parallel N` to the `cmake --build` invocation

Environment variables:
- `BUILD_DIR` – override the build directory (default: `build`)
- `GENERATOR_ARGS` – optional CMake generator flags (e.g., `-G Ninja`)
- `RUN_EXAMPLES_LOG` – log file path (default: `build/example_builds.log`)
- `JOBS` – alternative way to set parallelism

### What it does

1. Configures CMake (if `CMakeCache.txt` is missing) with `BUILD_EXAMPLES=ON`
2. Discovers directories inside `examples/` (respecting filters)
3. Builds the matching `example_<folder>` target
4. Optionally executes each binary
5. Logs stdout/stderr to `build/example_builds.log`

### Exit codes
- `0` – all targets (and optional runs) succeeded
- `1` – one or more builds or executions failed (see the log)

### CI Integration

Ideal for nightly jobs, hardware smoke tests, or PR checks that need to ensure tutorial code stays
healthy. Pair with `--execute` on a development Pi to keep runtime regressions in check.
