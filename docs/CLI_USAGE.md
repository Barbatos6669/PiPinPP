# PiPin++ CLI (`pipinpp`) Guide

The PiPin++ command-line interface lets you test pins, scan I2C buses, and run quick diagnostics
without writing C++ code.

## Building the CLI

```bash
cmake -S . -B build -DBUILD_CLI=ON
cmake --build build --target pipinpp_cli
sudo cmake --install build   # optional, installs to /usr/local/bin/pipinpp
```

After installation the binary is available as `pipinpp` (or run `./build/bin/pipinpp`).

## Quick Examples

```bash
pipinpp info                 # Show detected board + defaults
pipinpp mode 17 out          # Configure GPIO17 as output
pipinpp write 17 1           # Drive GPIO17 high
pipinpp blink 17 200         # Blink GPIO17 at 200 ms intervals
pipinpp i2c scan             # List devices on /dev/i2c-1 (auto-detected)
pipinpp i2c read 0x76 0xD0   # Read register 0xD0 from a BMP280
pipinpp spi test             # Perform a loopback test on /dev/spidev0.0
pipinpp benchmark            # Measure GPIO toggle throughput
pipinpp monitor 23 5         # Watch GPIO23 changes every 5 ms
pipinpp doctor               # Run environment diagnostics
```

Run `pipinpp help` anytime to see the full command matrix.

## Command Reference

| Category | Command | Description |
|----------|---------|-------------|
| Info | `pipinpp info` | Prints platform detection, default GPIO chip, I2C bus, and hints |
| Info | `pipinpp version` | Shows CLI + library versions |
| GPIO | `pipinpp mode <pin> <in|out|up|down>` | Sets pin direction/pulls |
| GPIO | `pipinpp read <pin>` | Reads pin state (exits with that value) |
| GPIO | `pipinpp write <pin> <0|1>` | Forces pin low/high |
| GPIO | `pipinpp toggle <pin>` | Toggles current pin state |
| GPIO | `pipinpp blink <pin> [interval_ms]` | Blinks until Ctrl+C |
| PWM | `pipinpp pwm <pin> <0-255>` | Uses `analogWrite()` for quick fades |
| I2C | `pipinpp i2c scan` | Draws an address table of active devices |
| I2C | `pipinpp i2c read <addr> <reg>` | Reads one register (hex accepted) |
| I2C | `pipinpp i2c write <addr> <reg> <value>` | Writes a register |
| SPI | `pipinpp spi test` | Simple loopback timing test |
| SPI | `pipinpp spi send <byte>` | Sends a raw byte |
| Diagnostics | `pipinpp benchmark` | Measures digitalWrite toggle speed |
| Diagnostics | `pipinpp test` | Runs self-tests bundled with the CLI |
| Diagnostics | `pipinpp monitor <pin> [interval_ms]` | Streams pin transitions with timestamps |
| Diagnostics | `pipinpp doctor` | Checks permissions, gpio group membership, detected platform |

## Tips
- Most commands require GPIO permissions. Either run via `sudo` or add yourself to the `gpio` group.
- I2C/SPI commands assume the standard Pi buses. Override by exporting `PIPINPP_I2C_BUS`/`PIPINPP_SPI_DEVICE`.
- Combine with shell scripts for quick lab tooling (e.g., `watch -n1 pipinpp read 4`).
- The CLI uses the same Arduino-compatible API as your C++ code, so behavior mirrors the library.
