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
