# Platform Support Matrix

PiPin++ targets Raspberry Pi boards with the 40-pin GPIO header. This matrix summarizes what has
been verified, what is pending, and how to report new data.

## Tested Boards

| Board | Status | Notes |
|-------|--------|-------|
| Raspberry Pi 5 | ✅ Supported | Uses `/dev/i2c-20` by default; platform detector auto-switches |
| Raspberry Pi 4 Model B (1–8 GB) | ✅ Supported | Reference device for CI + examples |
| Raspberry Pi 400 | ✅ Supported | Same silicon as Pi 4; keyboard chassis makes the header vertical |
| Raspberry Pi 3 Model B/B+ | ✅ Supported | Same GPIO numbering; lower performance but fully compatible |
| Raspberry Pi 2 Model B | ✅ Supported | Works with libgpiod 2.2.1; CPU-bound PWM >400 Hz |
| Raspberry Pi Zero 2 W | ✅ Supported | Recommend heatsink when running sustained PWM |
| Raspberry Pi Zero W (original) | ⚠️ Reduced performance | Single-core CPU limits software PWM throughput |
| Raspberry Pi Compute Module 4 | ✅ Supported | Verify carrier board pin muxing |
| Raspberry Pi Compute Module 3 | ⚠️ Needs confirmation | Same BCM2837 as Pi 3; please report results |

## Untested / Experimental

| Platform | Status | Notes |
|----------|--------|-------|
| Orange Pi 5 / Rockchip | 🚧 Planned | Needs new pin maps + libgpiod cross-build |
| NVIDIA Jetson Nano | 🚧 Planned | Different GPIO driver stack (Tegra) |
| BeagleBone Black | 🚧 Planned | Requires libgpiod 2.x and custom numbering |
| Khadas VIM4 | 🚧 Planned | Would use Amlogic GPIO controller; help wanted |

## Operating Systems

| OS | Status | Notes |
|----|--------|-------|
| Raspberry Pi OS (64-bit) Bookworm | ✅ | Primary target; `install.sh` builds libgpiod 2.2.1 automatically |
| Raspberry Pi OS (32-bit) Bookworm | ✅ | Works, but CPU-bound tasks saturate earlier |
| Ubuntu Server 22.04/24.04 for Pi | ✅ | Remove distro `libgpiod-dev` before running `install.sh` |
| Debian Bullseye | ⚠️ | Ships libgpiod v1.x; run `install.sh --libgpiod-only` first |
| Manjaro ARM | ⚠️ | Works if you install `gpiod` 2.x from community repos; please report results |
| DietPi | ⚠️ | Minimal image—install `build-essential` + `cmake` manually |

## Containers, VMs, and CI

- **Headless x86 CI**: Works with `gpiod_sim` and `PIPINPP_SKIP_HARDWARE_TESTS=ON`; run only unit tests.
- **Containers/chroots**: Bind-mount `/dev/gpiochip*` and `/dev/i2c-*`; ensure the container user is part of the `gpio` group.
- **WSL/VirtualBox**: 🛑 Not supported—Windows hypervisors do not expose GPIO/I2C devices.

## libgpiod Compatibility

| Environment | Tested libgpiod | Notes |
|-------------|-----------------|-------|
| PiPin++ 0.4.0 | 2.2.1 (preferred), 2.0.2 | v2 APIs mandatory for interrupts/PWM |
| PiPin++ 0.3.7 | 2.0.x | Transitional release before platform detector landed |
| PiPin++ ≤0.3.0 | 1.6.3 (legacy) | Deprecated – upgrade to ≥0.4.0 |
| Headless CI (x86) | 2.2.1 (custom build) | Use `gpiod_sim` and skip hardware-only tests |

## Hardware Notes

- GPIO pins 0/1 (I2C) and 14/15 (UART) are marked "reserved"; PiPin++ allows them but prints warnings.
- Hardware PWM pins (GPIO12/13/18/19) provide the lowest jitter for servos and LEDs.
- Pi 5 exposes a voltage-select jumper—leave it at 3.3 V; PiPin++ assumes 3.3 V logic.
- Carrier boards sometimes remap pins—validate against their schematics.
- Run `pipinpp doctor` + `pipinpp info` to confirm detected chips, bus numbers, and permissions before deploying.

## Verification Checklist

Use this flow when bringing PiPin++ to a new combo of board + OS:

1. `pipinpp doctor` – confirms `/dev/gpiochip*` access and permission groups.
2. `pipinpp info` – shows detected board, GPIO chip, and I2C bus mapping.
3. `scripts/run_examples.sh --filter basic_led --execute` – verifies digital write path.
4. `scripts/run_examples.sh --filter i2c_scanner --execute` – optional, validates I2C bus selection.
5. `ctest -R gtest_pin` – ensures unit tests pass with your kernel/libgpiod combo.
6. Capture thermal data if you plan to drive PWM/interrupt-heavy workloads for long periods.

Document the results (board, OS image, kernel, libgpiod version, pass/fail notes) so we can merge
them into the matrix.

## Contributing Platform Data

1. Run through the verification checklist.
2. Record `uname -a`, `lsb_release -d` (or `/etc/os-release`), and `gpiodetect -V` output.
3. List any extra steps (custom device tree overlays, kernel modules, voltage tweaks).
4. Open a GitHub issue using the "Platform Support" template or send a PR updating this file.

The more data we collect, the easier it is for makers to know what hardware/software stacks work
out of the box.
