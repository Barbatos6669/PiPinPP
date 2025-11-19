# Contributing to PiPin++

Welcome! We're excited to have you contribute to **PiPin++**, a modern C++ GPIO library for Raspberry Pi aimed at making hardware projects easier for Arduino makers. Whether you're a seasoned developer or a first-time contributor, your ideas, code, and feedback are valued here.

---

## 🎯 Ways to Contribute

### For Everyone
- ⭐ **Star the repository** to show support
- 🐛 **Report bugs** using our [bug report template](https://github.com/Barbatos6669/PiPinPP/issues/new?template=bug_report.md)
- 💡 **Request features** using our [feature request template](https://github.com/Barbatos6669/PiPinPP/issues/new?template=feature_request.md)
- 📖 **Improve documentation** - fix typos, clarify instructions, add examples
- 💬 **Answer questions** in issues and discussions
- 🎓 **Write tutorials** - share your projects and learning experiences

### For Developers
- 🔧 **Fix bugs** - check [open issues labeled "bug"](https://github.com/Barbatos6669/PiPinPP/labels/bug)
- ✨ **Add features** - see our [roadmap](https://github.com/Barbatos6669/PiPinPP/wiki/Roadmap)
- 🧪 **Improve tests** - increase coverage, add edge cases
- ⚡ **Optimize performance** - profile and improve critical paths
- 🖥️ **Port to new platforms** - test on different Pi models or boards

### First-Time Contributors
Look for issues labeled [`good first issue`](https://github.com/Barbatos6669/PiPinPP/labels/good%20first%20issue) - these are perfect for getting started!

---

## 🚀 Quick Start Guide

### 1. Fork & Clone
```bash
# Fork the repository on GitHub, then:
git clone https://github.com/YOUR-USERNAME/PiPinPP.git
cd PiPinPP
```

### 2. Set Up Development Environment
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config git

# Build libgpiod 2.2.1 (required)
./install.sh

# Build PiPinPP
./build.sh --clean --debug

# Run tests
cd build && ctest --output-on-failure
```

### 3. Create a Branch
```bash
git checkout -b feature/my-awesome-feature
# or
git checkout -b fix/issue-123
```

### 4. Make Changes
- Keep changes focused and atomic
- Follow our [Code Standards](https://github.com/Barbatos6669/PiPinPP/wiki/Code-Standards)
- Write clear commit messages (see below)

### 5. Test Your Changes
```bash
# Build
./build.sh

# Run all tests
cd build && ctest --output-on-failure

# Test specific functionality
./examples/example_your_feature

# Run on real hardware when possible
```

### 6. Submit a Pull Request
```bash
git push origin feature/my-awesome-feature
```
Then open a PR on GitHub using our [PR template](https://github.com/Barbatos6669/PiPinPP/blob/main/.github/PULL_REQUEST_TEMPLATE.md)

---

## 📋 Code Standards

### C++ Style
- **C++17** standard (modern features encouraged)
- **Allman brace style** (opening braces on new lines)
- **4-space indentation** (no tabs)
- **100 character line limit**
- **Clear naming**: `camelCase` for functions/variables, `PascalCase` for classes

### Documentation
- **Doxygen comments** for all public APIs
- Include `@brief`, `@param`, `@return`, `@throws`, `@example`
- Update `docs/API_REFERENCE.md` for user-facing changes
- Add examples for new features

### Testing
- **Unit tests** for all new functionality (GoogleTest)
- **Hardware tests** when possible (mark with `GTEST_SKIP()` if no hardware)
- Aim for >70% code coverage
- Test edge cases and error conditions

### Commit Messages
```
type(scope): Short description (50 chars max)

Longer explanation if needed (wrap at 72 chars).
Include motivation and contrast with previous behavior.

Fixes #123
```

**Types**: `feat`, `fix`, `docs`, `test`, `refactor`, `perf`, `chore`

**Examples**:
```
feat(Wire): add I2C multi-master support
fix(Pin): correct pull-up resistor configuration
docs(tutorial): add SPI device walkthrough
test(interrupts): add edge-case timing tests
```

See [Code Standards](https://github.com/Barbatos6669/PiPinPP/wiki/Code-Standards) for complete details.

---

## 🐛 Reporting Issues

### Bug Reports
Use our [bug report template](https://github.com/Barbatos6669/PiPinPP/issues/new?template=bug_report.md) and include:
- **Environment**: Pi model, OS, PiPinPP version, libgpiod version
- **Steps to reproduce**: Minimal code example
- **Expected vs actual behavior**
- **Error messages**: Full output/stack traces
- **Troubleshooting done**: Run `pipinpp doctor` first

### Feature Requests
Use our [feature request template](https://github.com/Barbatos6669/PiPinPP/issues/new?template=feature_request.md) and include:
- **Problem it solves**: What limitation does this address?
- **Proposed API**: Show example usage
- **Arduino compatibility**: Similar Arduino features?
- **Willingness to help**: Can you implement/test/document?

---

## 🔄 Pull Request Process

### Before Submitting
1. ✅ **Code compiles** without warnings (`./build.sh`)
2. ✅ **Tests pass** (`cd build && ctest`)
3. ✅ **Code follows standards** (check [Code Standards](https://github.com/Barbatos6669/PiPinPP/wiki/Code-Standards))
4. ✅ **Documentation updated** (API docs, tutorials, examples)
5. ✅ **Commit messages clear** (follow format above)

### Review Process
- Maintainers will review within 1-3 days
- Address feedback constructively
- Keep PRs focused on one feature/fix
- Be patient - quality over speed!

### After Merge
- Your contribution will be in the next release
- You'll be added to CONTRIBUTORS.md
- Consider helping review other PRs!

---

## 🎓 Learning Resources

### Getting Started
- [Developer Guide](https://github.com/Barbatos6669/PiPinPP/wiki/Developer-Guide) - Environment setup
- [Architecture Overview](https://github.com/Barbatos6669/PiPinPP/wiki/API-Reference#architecture) - How PiPinPP works
- [Tutorial Index](https://github.com/Barbatos6669/PiPinPP/wiki/Tutorials-Index) - Learn by example

### Key Components
- **ArduinoCompat**: Arduino-style API layer (`pinMode`, `digitalWrite`, etc.)
- **Pin class**: Object-oriented GPIO control with RAII
- **Wire/SPI/Serial**: Communication protocol implementations
- **Platform detection**: Auto-detect Pi model and peripherals

### Tools
- `pipinpp doctor` - Check permissions and setup
- `pipinpp info` - Show platform details
- `scripts/run_examples.sh` - Batch test examples

---

## 🤝 Community & Conduct

### Code of Conduct
- **Be respectful and welcoming** to all contributors
- **Assume good intentions** - we're all learning
- **Help others succeed** - answer questions, share knowledge
- **Focus on constructive feedback**
- Review our full [Code of Conduct](CODE_OF_CONDUCT.md)

### Getting Help
- 📖 **Documentation**: Start with the [Wiki](https://github.com/Barbatos6669/PiPinPP/wiki)
- 🐛 **Bugs**: Open an [issue](https://github.com/Barbatos6669/PiPinPP/issues)
- 💬 **Questions**: Use [GitHub Discussions](https://github.com/Barbatos6669/PiPinPP/discussions)
- 🚀 **Show & Tell**: Share your projects in Discussions!

---

## 🙏 Thank You!

Every contribution makes PiPinPP better for makers worldwide. Whether you fix a typo, add a feature, or answer a question - **you're making a difference**.

**Special thanks to all our contributors!** 🎉

---

**Happy making!** 🚀
