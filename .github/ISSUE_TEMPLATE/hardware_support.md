---
name: Hardware Compatibility Request
about: Request support for new Raspberry Pi models or single-board computers
title: '[HARDWARE] '
labels: hardware, enhancement
assignees: ''
---

## 🔧 Hardware Compatibility Request

**What hardware do you want supported?**
- Board Name: <!-- e.g., Raspberry Pi 5, Orange Pi 5, Rock Pi 4 -->
- Manufacturer: <!-- e.g., Raspberry Pi Foundation, Xunlong, Radxa -->
- Product Link: <!-- URL to product page -->

**Is this hardware officially supported by libgpiod?**
- [ ] Yes, confirmed working
- [ ] Partially (some features work)
- [ ] Unknown
- [ ] No, requires custom GPIO driver

---

## 📋 Hardware Specifications

**GPIO Details:**
- Number of GPIO pins: 
- GPIO chip device: <!-- e.g., /dev/gpiochip0, /dev/gpiochip4 -->
- Voltage levels: <!-- e.g., 3.3V, 1.8V -->
- Pin numbering scheme: <!-- BCM, physical, WiringPi? -->

**SoC Information:**
- Processor: <!-- e.g., BCM2711, RK3588 -->
- Architecture: <!-- ARM64, ARM32 -->
- Linux kernel version: <!-- e.g., 6.1.21 -->

**Peripherals:**
- [ ] I2C buses (how many?)
- [ ] SPI buses (how many?)
- [ ] PWM channels (hardware PWM count?)
- [ ] UART ports

---

## 🧪 Testing Availability

**Do you have access to this hardware?**
- [ ] Yes, I can test changes
- [ ] Yes, and I can contribute code
- [ ] No, requesting based on planned purchase
- [ ] No, reporting on behalf of community

**Testing commitment:**
- [ ] I can test pull requests within a week
- [ ] I can run automated tests
- [ ] I can test specific features (list below)
- [ ] I can provide remote access (via VPN/SSH)
- [ ] I can donate hardware to the project

**Specific features to test:**
- [ ] Basic GPIO (digitalWrite/digitalRead)
- [ ] PWM functionality
- [ ] I2C communication
- [ ] SPI communication
- [ ] Interrupts
- [ ] Timing functions
- [ ] All features

---

## 📚 Documentation

**Available resources:**
- Datasheet/Manual: <!-- Link to official documentation -->
- GPIO pinout diagram: <!-- Link to pinout -->
- Existing libraries: <!-- Any working GPIO libraries? -->
- Community forums: <!-- Links to support forums -->

**Known differences from Raspberry Pi:**
<!-- e.g., different I2C bus numbers, alternate pin functions, voltage differences -->



---

## 🎯 Use Case

**Why do you need this hardware supported?**
<!-- Describe your project or use case -->



**Current workaround:**
<!-- Are you using a different library? Manual GPIO access? -->



**Priority for your project:**
- [ ] Critical - project blocked without it
- [ ] High - would significantly improve project
- [ ] Medium - nice to have
- [ ] Low - future consideration

---

## 🔍 Additional Information

**Similar hardware already supported:**
<!-- e.g., "Similar to Raspberry Pi 4 but with more GPIO pins" -->



**Estimated effort:**
<!-- If you know the technical details, estimate complexity -->
- [ ] Trivial - just needs testing/documentation
- [ ] Easy - minor code adjustments
- [ ] Moderate - some new features needed
- [ ] Difficult - significant refactoring required
- [ ] Unknown

**Related issues/PRs:**
<!-- Link to related discussions -->

