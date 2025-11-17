---
name: Platform Support Request
about: Request or report testing on new hardware platform
title: '[PLATFORM] '
labels: platform, testing
assignees: ''
---

## 🖥️ Platform Support Request

**Hardware Platform**
<!-- e.g., Orange Pi 5, BeagleBone AI, Jetson Nano -->
- **Board**: 
- **OS**: (e.g., Ubuntu 22.04, Armbian, Debian Bullseye)
- **Kernel Version**: (output of `uname -r`)
- **libgpiod Version**: (output of `pkg-config --modversion libgpiod`)

**Current Status**
- [ ] Not tested yet
- [ ] Partially working (specify what works/fails below)
- [ ] Fully working (please share test results!)
- [ ] Not working (error details below)

**Test Results**
```bash
# Run test suite and paste results
cd build && ctest --output-on-failure
```

**GPIO Chip Information**
```bash
# Run and paste output
gpiodetect
gpioinfo
```

**I2C/SPI Bus Information**
```bash
# Paste output
ls -l /dev/i2c-* /dev/spidev*
```

**Issues Encountered**
<!-- What doesn't work? Error messages? -->

**Pin Mapping Differences**
<!-- Does this platform use different GPIO numbering? -->

**Workarounds Discovered**
<!-- Any fixes or configuration changes needed? -->

**Are you willing to help test?**
- [ ] Yes, I have hardware and can run tests
- [ ] Yes, I can provide platform-specific documentation
- [ ] Yes, I can create CI runner for this platform

**Additional Information**
<!-- Links to datasheets, pinout diagrams, etc. -->
