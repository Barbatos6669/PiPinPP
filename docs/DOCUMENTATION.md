# PiPinPP Documentation

This directory contains the auto-generated API documentation for PiPinPP.

## 📚 View Online Documentation

**Live Documentation:** https://barbatos6669.github.io/PiPinPP/

Documentation is automatically generated from source code comments using Doxygen and published to GitHub Pages on every release.

## 🔨 Generate Documentation Locally

### Prerequisites
```bash
sudo apt-get install doxygen graphviz
```

### Generate
```bash
# From project root
doxygen Doxyfile

# Open in browser
xdg-open docs/html/index.html
```

## 📖 Documentation Structure

- **Classes:** Complete API reference for all classes
- **Files:** Source file documentation
- **Examples:** Code samples and usage patterns
- **Modules:** Grouped functionality (GPIO, timing, Arduino-inspired API)

## 🔄 Auto-Update

Documentation is automatically updated when:
- ✅ New code is pushed to `main`
- ✅ New version is tagged (e.g., `v0.3.3`)
- ✅ Manually triggered via GitHub Actions

See `.github/workflows/documentation.yml` for the automation workflow.

## 📝 Contributing to Documentation

We use Doxygen-style comments:

```cpp
/**
 * @brief Brief description of the function
 * 
 * Detailed description with more context.
 * Can span multiple lines.
 * 
 * @param pin GPIO pin number (BCM numbering)
 * @param mode Pin mode (INPUT, OUTPUT, etc.)
 * @return true if successful, false otherwise
 * 
 * @code
 * Pin led(17, PinDirection::OUTPUT);
 * led.write(true);  // Turn on LED
 * @endcode
 */
bool setupPin(int pin, PinMode mode);
```

## 🔗 Quick Links

- [API Reference](API_REFERENCE.md) - Markdown API docs
- [Wiki Home](WIKI_HOME.md) - Getting started guide
- [GitHub Repository](https://github.com/Barbatos6669/PiPinPP)
