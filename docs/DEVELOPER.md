# Developer Onboarding Guide

Welcome to PiPin++ internals! This guide outlines the workflow for contributors who want to build,
test, and ship changes confidently.

## 1. Environment Setup

```bash
git clone https://github.com/Barbatos6669/PiPinPP.git
cd PiPinPP
./install.sh              # Simplest route (builds libgpiod 2.2.1+ automatically)
```

Need a custom build? Install dependencies manually:
- CMake ≥ 3.16
- GCC ≥ 10 or Clang ≥ 12 (C++17)
- libgpiod 2.2.1+ (v1.x is incompatible)
- pkg-config, ninja or make

## 2. Recommended Workflow

1. `./build.sh --clean --debug` for a fresh debug build.
2. `cmake --build build` or `cmake --build build --target <target>` for incremental work.
3. `cd build && ctest --output-on-failure` before opening a PR.
4. Run relevant examples on real hardware when touching GPIO/driver code.

## 3. Formatting & Style

- Follow the Allman brace style, 4-space indentation, and naming rules in `docs/CODE_STANDARDS.md`.
- Prefer modern C++ (RAII, smart pointers). Avoid raw `new`/`delete`.
- Use `clang-format` with the provided `.clang-format` file (or run `cmake --build build --target format` once available).
- Document new public APIs with full Doxygen blocks (`@brief`, `@param`, `@returns`, `@throws`, `@example`).

## 4. Testing Stack

| Test Type | Command | Notes |
|-----------|---------|-------|
| Unit (default) | `cd build && ctest` | Runs 600+ GoogleTest cases, hardware-free |
| Selected | `ctest -R gtest_wire` | Filter by suite name |
| Coverage | `cmake -S . -B build -DPIPINPP_ENABLE_LOGGING=OFF -DENABLE_COVERAGE=ON` then `cmake --build build --target coverage` | Generates `build/coverage_html/index.html` |
| Examples | `cmake --build build --target example_<name>` | Ensures docs/tutorials stay correct |

## 5. Submitting Changes

1. Ensure `git status` is clean except for intentional edits.
2. Update docs/examples/tests alongside code.
3. Run `clang-tidy`/`cppcheck` if introducing lower-level changes.
4. Open a PR targeting `main`, fill in the template, and link related issues.

## 6. Mocking & Headless Tips

- Use `BUILD_TESTS=ON` (default) to keep mocks enabled; hardware-specific tests auto-skip when `/dev/gpiochip0` is absent.
- For CI or x86 dev boxes, rely on libgpiod’s mock chips via `gpiod_sim` (documented in `docs/validation/`).
- Wrap slow hardware loops with `GTEST_SKIP()` guards referencing the relevant issue/requirement.

## 7. Helpful Scripts

| Script | Purpose |
|--------|---------|
| `build.sh --clean --debug` | One-liner to rebuild from scratch |
| `install.sh` | Ensures libgpiod 2.2.1+ and installs to `/usr/local` |
| `scripts/format.sh` *(coming soon)* | Project-wide formatting |
| `scripts/run_examples.sh` | Batch-builds (and optionally runs) examples; supports `--filter`, `--execute`, `--jobs` |

## 8. Resources

- [CODE_STANDARDS.md](CODE_STANDARDS.md)
- [.github/CONTRIBUTING.md](../.github/CONTRIBUTING.md)
- [API_REFERENCE.md](API_REFERENCE.md)
- [docs/tutorials/](tutorials/README.md)

Thank you for leveling up PiPin++!
