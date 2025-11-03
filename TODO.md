# PiPinPP v0.3.0 Todo List

This checklist tracks the planned work for v0.3.0. Use it locally in VS Code, or run the helper script in `scripts/` to open GitHub issues from these items.

Legend: [easy] quick win · [medium] moderate · [hard] larger feature

## Easy

- ✅ Thread-safety for ArduinoCompat [easy]
  - Protect globalPins map with a mutex
  - Document thread-safety notes
  - Add minimal concurrent access test

- ✅ Tidy HIGH/LOW constants [easy]
    - Replaced #define macros with constexpr bool for type safety
    - Kept simple global scope for Arduino compatibility
    - No namespace needed - HIGH/LOW don't conflict with std library
    - Updated TODO.md to mark task as complete"

- ✅ Logging + build options [easy]
  - Minimal logging macros behind `PIPINPP_ENABLE_LOGGING` and `PIPINPP_LOG_LEVEL`
  - Remove `std::cout` from library code
  - Add `-Wall -Wextra -Wpedantic` and optional `PIPINPP_WARNINGS_AS_ERRORS`

- ✅ Repo discoverability [easy]
  - Set GitHub topics: raspberry-pi, gpio, cpp, arduino, libgpiod, embedded
  - Add README badges: build, release, license
  - GitHub Pages configured (awaiting v0.3.0 merge to main)

- ✅ Docs and wiki updates [easy]
  - Pin numbering guide (BCM vs physical) - See docs/API_REFERENCE.md
  - Permissions guide (gpio group / udev) - See INSTALL.md
  - TOC anchors added and docs synced to wiki structure

## Medium

- ✅ Timing functions [medium]
  - Implemented `millis()`, `micros()`, `delay()`, and `delayMicroseconds()`
  - Using `std::chrono::steady_clock` for monotonic timing
  - Comprehensive unit tests for accuracy, monotonicity and bounds
  - All functions tested and verified accurate to ±1µs
  - Completed in v0.3.1

- ✅ Custom exceptions [medium]
  - `PinError`, `InvalidPinError`, `GpioAccessError`
  - Replace generic throws and improve error messages
  - Document error model
  - Completed in v0.3.2

- ✅ GitHub Actions CI [medium]
  - Build Debug/Release, run tests
  - pkg-config smoke test
  - Optional CodeQL; add badges
  - Completed in v0.3.3

- ✅ Unit tests (GoogleTest) [medium]
  - Integrated GoogleTest v1.14.0 framework
  - 40 comprehensive tests (timing, exceptions, Pin operations)
  - Automatic test discovery and professional reporting
  - Hardware tests skip gracefully in CI environments
  - Completed in v0.3.4

- [ ] New examples [medium]
  - `interrupt_button`, `pwm_led_fade`, `timing_benchmark` (with wiring notes)

- [ ] CMake packaging polish [medium]
  - Shared library option
  - Install `PiPinPPConfig.cmake` and export targets
  - Verify both CMake and pkg-config consumption paths

## Hard

- [ ] Implement interrupts API [hard]
  - `attachInterrupt(pin, mode, callback)` and `detachInterrupt(pin)`
  - Support RISING/FALLING/CHANGE via libgpiod events
  - Internal polling/epoll thread with safe shutdown
  - Example + documentation

- [ ] Add PWM (analogWrite) [hard]
  - Software PWM backend (configurable frequency/duty) first; document jitter
  - LED fade example; roadmap to hardware PWM backend

- [ ] Namespace core API [hard]
  - Wrap Pin, enums, helpers in `pipinpp::`
  - Provide transitional `using` declarations to avoid breaking users
  - Update headers and docs

---

## Create GitHub Issues (optional)

A helper script is provided to open GitHub issues and a `v0.3.0` milestone using GitHub CLI.

Requirements:
- GitHub CLI installed and authenticated: `gh auth login`
- Push access to the repository

Run:
```bash
./scripts/create_issues_v0_3_0.sh
```

This will:
- Create a `v0.3.0` milestone
- Open one issue per todo and assign them to the milestone
