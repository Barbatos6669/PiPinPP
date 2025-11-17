"""
Setup script for PyPiPinPP - Python bindings for PiPinPP GPIO library
"""

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import os
import subprocess

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""
    def __str__(self):
        import pybind11
        return pybind11.get_include()

def pkgconfig(*packages, **kw):
    """Use pkg-config to get compiler flags"""
    flag_map = {'-I': 'include_dirs', '-L': 'library_dirs', '-l': 'libraries'}
    
    cmd = ['pkg-config', '--cflags', '--libs'] + list(packages)
    try:
        output = subprocess.check_output(cmd).decode('utf-8')
    except subprocess.CalledProcessError:
        print(f"Warning: pkg-config failed for {packages}")
        return kw
    
    for token in output.split():
        if token[:2] in flag_map:
            kw.setdefault(flag_map.get(token[:2]), []).append(token[2:])
        else:
            kw.setdefault('extra_link_args', []).append(token)
    return kw

# PiPinPP library paths
pipinpp_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '../..'))
pipinpp_include = os.path.join(pipinpp_root, 'include')
pipinpp_src = os.path.join(pipinpp_root, 'src')

# Collect all PiPinPP source files
pipinpp_sources = [
    os.path.join(pipinpp_src, 'ArduinoCompat.cpp'),
    os.path.join(pipinpp_src, 'pin.cpp'),
    os.path.join(pipinpp_src, 'pwm.cpp'),
    os.path.join(pipinpp_src, 'interrupts.cpp'),
    os.path.join(pipinpp_src, 'Wire.cpp'),
    os.path.join(pipinpp_src, 'SPI.cpp'),
    os.path.join(pipinpp_src, 'Serial.cpp'),
    os.path.join(pipinpp_src, 'platform.cpp'),
    os.path.join(pipinpp_src, 'event_pwm.cpp'),
]

# Extension module
ext_modules = [
    Extension(
        'pypipinpp',
        sources=['pypipinpp.cpp'] + pipinpp_sources,
        include_dirs=[
            get_pybind_include(),
            pipinpp_include,
        ],
        language='c++',
        **pkgconfig('libgpiod')
    ),
]

class BuildExt(build_ext):
    """Custom build extension to set C++17 standard"""
    def build_extensions(self):
        opts = ['-std=c++17', '-O3']
        
        # Platform-specific optimizations
        if sys.platform == 'linux':
            opts.append('-march=native')
        
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)

# Read long description from README
long_description = """
# PyPiPinPP - Python Bindings for PiPinPP

Arduino-compatible GPIO library for Raspberry Pi in Python.

## Features

- **Arduino API Compatibility**: Use familiar functions like `pinMode()`, `digitalWrite()`, `analogWrite()`
- **Modern & Fast**: Built on libgpiod v2 with C++ performance
- **Comprehensive**: GPIO, PWM, I²C (Wire), SPI, Serial/UART support
- **Thread-Safe**: All operations protected by internal mutexes
- **Cross-Platform**: Works on Raspberry Pi 3/4/5, Orange Pi, and other SBCs
- **Production-Ready**: 73.5% test coverage, actively maintained

## Quick Start

```python
import pypipinpp as gpio

# Blink an LED
gpio.pinMode(17, gpio.OUTPUT)
while True:
    gpio.digitalWrite(17, gpio.HIGH)
    gpio.delay(1000)
    gpio.digitalWrite(17, gpio.LOW)
    gpio.delay(1000)
```

## Installation

```bash
pip install pypipinpp
```

For development:

```bash
git clone https://github.com/Barbatos6669/PiPinPP
cd PiPinPP/bindings/python
pip install -e .
```

## Requirements

- Python 3.7+
- libgpiod >= 2.2.1
- Raspberry Pi (or compatible SBC)
- User must be in `gpio` group

## Documentation

Full documentation: https://github.com/Barbatos6669/PiPinPP

## Examples

### Digital I/O
```python
import pypipinpp as gpio

# Button input
gpio.pinMode(18, gpio.INPUT_PULLUP)
if gpio.digitalRead(18) == gpio.LOW:
    print("Button pressed!")
```

### PWM
```python
import pypipinpp as gpio

# Fade LED
gpio.pinMode(17, gpio.OUTPUT)
for brightness in range(256):
    gpio.analogWrite(17, brightness)
    gpio.delay(10)
```

### I²C Communication
```python
from pypipinpp import Wire

# Scan for I2C devices
Wire.begin()
devices = Wire.scan()
print(f"Found devices: {devices}")

# Read from sensor
Wire.beginTransmission(0x68)
Wire.write(0x75)  # WHO_AM_I register
Wire.endTransmission(False)
Wire.requestFrom(0x68, 1)
device_id = Wire.read()
print(f"Device ID: 0x{device_id:02x}")
```

### SPI Communication
```python
from pypipinpp import SPI

SPI.begin()
SPI.setClockDivider(SPI.SPI_CLOCK_DIV8)
response = SPI.transfer(0x42)
print(f"Received: 0x{response:02x}")
```

### Serial/UART
```python
from pypipinpp import Serial

# Talk to Arduino
Serial.begin(9600, "/dev/ttyUSB0")
Serial.println("Hello Arduino!")
gpio.delay(100)
if Serial.available():
    response = Serial.readStringUntil('\\n')
    print(f"Arduino says: {response}")
```

### Interrupts
```python
import pypipinpp as gpio

def button_handler():
    print("Button pressed!")

gpio.pinMode(18, gpio.INPUT_PULLUP)
gpio.attachInterrupt(18, button_handler, gpio.FALLING)

# Keep program running
while True:
    gpio.delay(1000)
```

## License

MIT License - see LICENSE file for details.

## Credits

PiPinPP C++ library by Barbatos6669  
Python bindings using pybind11
"""

setup(
    name='pypipinpp',
    version='0.4.0',
    author='Barbatos6669',
    author_email='',  # Add email if desired
    description='Arduino-compatible GPIO library for Raspberry Pi (Python bindings)',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/Barbatos6669/PiPinPP',
    project_urls={
        'Documentation': 'https://github.com/Barbatos6669/PiPinPP/blob/main/docs/API_REFERENCE.md',
        'Source': 'https://github.com/Barbatos6669/PiPinPP',
        'Tracker': 'https://github.com/Barbatos6669/PiPinPP/issues',
    },
    ext_modules=ext_modules,
    cmdclass={'build_ext': BuildExt},
    install_requires=['pybind11>=2.6.0'],
    setup_requires=['pybind11>=2.6.0'],
    python_requires='>=3.7',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Libraries',
        'Topic :: System :: Hardware',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Operating System :: POSIX :: Linux',
        'Environment :: Console',
    ],
    keywords='raspberry-pi gpio arduino embedded iot maker hardware sbc',
    zip_safe=False,
)
