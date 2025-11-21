# Pre-commit Hooks Setup Guide

This project uses [pre-commit](https://pre-commit.com/) to automatically enforce code quality standards before commits.

## What Gets Checked

Pre-commit hooks automatically run before each commit to:

✅ **Format code** - C++ (clang-format), Python (black), CMake
✅ **Lint files** - Shell scripts (shellcheck), Markdown (markdownlint)
✅ **Validate commits** - Conventional Commits format enforcement
✅ **Prevent issues** - Trailing whitespace, large files, merge conflicts, private keys
✅ **Maintain consistency** - Line endings (LF), file endings, JSON/YAML syntax

## Installation

### 1. Install pre-commit
```bash
# Using pip (recommended)
pip install pre-commit

# Or using apt (Ubuntu/Debian)
sudo apt install pre-commit

# Or using brew (macOS)
brew install pre-commit
```

### 2. Install hooks
```bash
cd /path/to/PiPinPP
pre-commit install
pre-commit install --hook-type commit-msg
```

### 3. (Optional) Run on all files
```bash
pre-commit run --all-files
```

## Usage

### Normal commits
Once installed, hooks run automatically:
```bash
git add src/pin.cpp
git commit -m "feat: add new GPIO feature"
```

If any hook fails, the commit is blocked and files are auto-fixed where possible.

### Skip hooks (emergency only)
```bash
git commit --no-verify -m "emergency fix"
```

⚠️ **Not recommended** - CI will still enforce these checks!

### Update hooks
```bash
pre-commit autoupdate
```

## Hook Configuration

Configuration is in `.pre-commit-config.yaml`. Hooks include:

### Code Formatting
- **clang-format** - Enforces Allman brace style, 4-space indent, 100-char limit
- **cmake-format** - Formats CMakeLists.txt files
- **black** - Formats Python scripts (PEP 8 compliant)

### Linting
- **shellcheck** - Validates shell scripts (scripts/build.sh, scripts/install.sh, scripts/*.sh)
- **markdownlint** - Checks Markdown formatting (except CHANGELOG.md)

### Validation
- **conventional-pre-commit** - Enforces commit message format:
  - `feat: description` - New features
  - `fix: description` - Bug fixes
  - `docs: description` - Documentation
  - `test: description` - Tests
  - `chore: description` - Maintenance

### Safety Checks
- **check-merge-conflict** - Detects unresolved merge conflicts
- **detect-private-key** - Prevents committing private keys
- **check-added-large-files** - Blocks files >500KB

## Commit Message Format

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Examples:**
```bash
git commit -m "feat(gpio): add INPUT_PULLDOWN support"
git commit -m "fix(pwm): resolve frequency calculation bug"
git commit -m "docs: update installation guide"
git commit -m "test(interrupts): add edge detection tests"
git commit -m "chore: bump version to 0.4.1"
```

**Valid types:**
- `feat` - New feature
- `fix` - Bug fix
- `docs` - Documentation only
- `style` - Code style (formatting, whitespace)
- `refactor` - Code restructuring (no behavior change)
- `perf` - Performance improvement
- `test` - Adding/fixing tests
- `build` - Build system changes
- `ci` - CI/CD changes
- `chore` - Maintenance tasks

## Troubleshooting

### Hook fails on commit
Check the error message - most hooks auto-fix issues:
```bash
git add .  # Stage auto-fixed files
git commit -m "feat: your message"
```

### clang-format not found
```bash
sudo apt install clang-format
# or
brew install clang-format
```

### Python hooks fail
Ensure Python 3 is installed:
```bash
python3 --version
pip install -r requirements-dev.txt  # If available
```

### Disable specific hook
Edit `.pre-commit-config.yaml` and comment out the hook:
```yaml
# - repo: https://github.com/pre-commit/mirrors-clang-format
#   rev: v18.1.8
#   hooks:
#     - id: clang-format
```

Then run `pre-commit install` again.

### Commit message rejected
Ensure message follows format: `type: description`

```bash
# ❌ Bad
git commit -m "updated code"

# ✅ Good
git commit -m "feat: add new feature"
git commit -m "fix: resolve bug #123"
```

## CI Integration

GitHub Actions workflows enforce the same checks:
- `format-check.yml` - Validates C++ formatting
- `ci.yml` - Runs all tests and checks

**Pre-commit hooks = faster feedback than waiting for CI!**

## Maintenance

### Update hook versions
```bash
pre-commit autoupdate
git add .pre-commit-config.yaml
git commit -m "chore: update pre-commit hooks"
```

### Clean cache
```bash
pre-commit clean
```

### Uninstall
```bash
pre-commit uninstall
pre-commit uninstall --hook-type commit-msg
```

## Benefits

✅ Catch formatting issues locally (not in PR review)
✅ Enforce consistent code style across contributors
✅ Prevent common mistakes (merge conflicts, large files)
✅ Validate commits before they reach CI
✅ Auto-fix most issues automatically

## Resources

- [Pre-commit Homepage](https://pre-commit.com/)
- [Conventional Commits](https://www.conventionalcommits.org/)
- [Supported Hooks](https://pre-commit.com/hooks.html)

---

**Questions?** Open an issue or check our [GitHub Discussions](https://github.com/Barbatos6669/PiPinPP/discussions)!
