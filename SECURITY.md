# Security Policy

## Supported Versions

PiPinPP security updates and support for different versions:

| Version | Supported          | Notes                                    |
| ------- | ------------------ | ---------------------------------------- |
| 0.2.x   | :white_check_mark: | Current stable release, actively supported |
| 0.1.x   | :warning:          | Legacy support, critical fixes only     |
| < 0.1   | :x:                | No longer supported                      |

**Recommendation**: Always use the latest 0.2.x release for new projects.

## Security Considerations for GPIO Libraries

### Hardware-Level Security
- **Root Access Required**: PiPinPP requires root privileges for GPIO access via libgpiod
- **Hardware Damage Risk**: Incorrect GPIO usage can damage Raspberry Pi hardware
- **Electrical Safety**: Always verify voltage levels before connecting external devices

### Software Security Best Practices
- **Input Validation**: All pin numbers are validated (0-27 range for Raspberry Pi)
- **Resource Management**: RAII patterns prevent GPIO resource leaks
- **Error Handling**: Comprehensive exception handling prevents undefined behavior

### Known Security Considerations
- **Privilege Escalation**: Applications using PiPinPP typically run as root
- **Pin Conflicts**: Multiple processes accessing the same GPIO pins can cause conflicts
- **Hardware State**: GPIO pins retain state between program runs

## Reporting a Vulnerability

### How to Report
For security vulnerabilities in PiPinPP, please report them responsibly:

1. **Discord** (Primary): Contact maintainers directly on our Discord server for all security issues
2. **GitHub Security**: Use GitHub's private vulnerability reporting feature for detailed documentation
3. **GitHub Issues**: Only for non-sensitive security discussions (after coordinating on Discord)

### What to Include
When reporting a vulnerability, please provide:
- **Description**: Clear description of the vulnerability
- **Reproduction**: Steps to reproduce the issue
- **Impact**: Potential security impact and affected versions
- **Environment**: Hardware setup, OS version, PiPinPP version

### Response Timeline
- **Initial Response**: Within 48 hours of report
- **Investigation**: Security team will investigate within 1 week
- **Fix Timeline**: Critical vulnerabilities patched within 2 weeks
- **Disclosure**: Public disclosure after fix is released and users notified

### Security Update Process
1. **Patch Development**: Security fixes developed in private repository
2. **Testing**: Patches tested on multiple Raspberry Pi configurations
3. **Release**: Security updates released as patch versions (e.g., 0.2.1)
4. **Notification**: Users notified via GitHub releases and Discord

## Security Best Practices for Users

### Safe GPIO Usage
```cpp
// Good: Proper error handling and resource management
try {
    Pin led(17, PinDirection::OUTPUT);
    led.write(true);
    // Pin automatically cleaned up when going out of scope
} catch (const std::exception& e) {
    std::cerr << "GPIO Error: " << e.what() << std::endl;
}

// Avoid: Direct memory access or bypassing PiPinPP validation
```

### Permission Management
- **Run with minimal privileges**: Only use sudo when necessary
- **Validate pin assignments**: Double-check pin numbers before deployment
- **Monitor GPIO usage**: Be aware of other processes using GPIO pins

### Hardware Protection
- **Use current limiting resistors**: Protect GPIO pins from overcurrent
- **Verify voltage levels**: Ensure 3.3V logic compatibility
- **Implement safety shutoffs**: Add emergency stop mechanisms for motor control

### Development Security
- **Code Review**: Review GPIO code for potential hardware damage
- **Testing**: Test on development hardware before production deployment
- **Documentation**: Document pin assignments and electrical requirements

## Responsible Disclosure

We encourage responsible disclosure of security vulnerabilities. We commit to:

- **Acknowledgment**: Crediting security researchers (if desired)
- **Communication**: Keeping reporters informed of fix progress
- **Transparency**: Publishing security advisories for resolved issues
- **Improvement**: Using reports to improve overall library security

## Security Contact

- **Primary**: Discord server (for all security communications and urgent issues)
- **Documentation**: GitHub Security Advisories (for formal vulnerability tracking)
- **Public Discussion**: GitHub Issues (for non-sensitive security topics only)

*Last Updated: October 31, 2025*
