## [0.5.0](https://github.com/Barbatos6669/PiPinPP/compare/v0.4.0...v0.5.0) (2025-11-21)


### Features

* add auto wiki sync workflow, improve issue/PR templates, enhance badges and CONTRIBUTING.md ([2bb7d3f](https://github.com/Barbatos6669/PiPinPP/commit/2bb7d3f221a9b3035498c70c095fe82a68ca9112))
* Add complete Python bindings (pypipinpp) ([43aff1e](https://github.com/Barbatos6669/PiPinPP/commit/43aff1e5941bc6864263cee692ccacbc0043e4fd))
* Add comprehensive 2026 roadmap to 'Super Champ' status ([a98e1d9](https://github.com/Barbatos6669/PiPinPP/commit/a98e1d9ed72ff62d86435a0762e862e306722f70))
* add comprehensive automation workflows and pre-commit hooks ([c6786f9](https://github.com/Barbatos6669/PiPinPP/commit/c6786f955fb4a72e7f5f636797624734e623442c))
* add comprehensive developer tooling and automation ([45bde78](https://github.com/Barbatos6669/PiPinPP/commit/45bde786773fa2bfd951392f7418c8cf4f20fa9c))
* add GitHub automation workflows and code style tooling ([115a409](https://github.com/Barbatos6669/PiPinPP/commit/115a4092178ced385b4e6e86af05cd977a78adec))
* Add Hardware PWM support (v0.3.13) ([4dbd014](https://github.com/Barbatos6669/PiPinPP/commit/4dbd0141bc17f38a34fc6e04f6849c92c41335fa))
* add issue templates, dependabot, citations, and enhance README ([3484983](https://github.com/Barbatos6669/PiPinPP/commit/348498349e2dac3f3e3cea51eec14187a369093f))
* enhance build system, add PR validation workflow, improve .gitignore ([249ce0c](https://github.com/Barbatos6669/PiPinPP/commit/249ce0c128cde13049cababbaa6d321807b5e807))
* Implement EventPWM with hybrid timing (70-85% CPU reduction) ([f668a14](https://github.com/Barbatos6669/PiPinPP/commit/f668a1423251874f6d79563e33f33da2ad10ed7e)), closes [#1](https://github.com/Barbatos6669/PiPinPP/issues/1)
* Platform Detection & Abstraction (Phase 2) ([addd6a2](https://github.com/Barbatos6669/PiPinPP/commit/addd6a278bbc984909d59e929e4b7db88eca79c6))
* **serial,spi:** Add Arduino-compatible print formats + 39 extended tests ([7b7fd94](https://github.com/Barbatos6669/PiPinPP/commit/7b7fd946cde07e4b60d284c1b0aadfb7156f356b))
* **testing:** Add comprehensive code coverage infrastructure ([5f1321a](https://github.com/Barbatos6669/PiPinPP/commit/5f1321acdc5dbff97adbd8cd96d45b58a2f0cf09)), closes [#6](https://github.com/Barbatos6669/PiPinPP/issues/6) [#5](https://github.com/Barbatos6669/PiPinPP/issues/5) [#7](https://github.com/Barbatos6669/PiPinPP/issues/7)
* **tools:** Add pipinpp command-line interface tool ([9d56d34](https://github.com/Barbatos6669/PiPinPP/commit/9d56d34ebe80cfdba48563e22c7e3e5efc97fa39)), closes [#8](https://github.com/Barbatos6669/PiPinPP/issues/8)


### Bug Fixes

* add libgpiod build step to docs workflow ([52acf10](https://github.com/Barbatos6669/PiPinPP/commit/52acf10f2dbe45b044b61a7c6d686b26d5df80f1))
* changelog generator version file handling ([4d00dc5](https://github.com/Barbatos6669/PiPinPP/commit/4d00dc5201fcff5e4bfd2580f7f9f70c2885c4a9))
* **cli:** Suppress unused parameter warning in signal handler ([e0dc098](https://github.com/Barbatos6669/PiPinPP/commit/e0dc09845022b8dd7363785af44d1b66bf65466d))
* correct broken Security link in README and improve link checker error handling ([37f565e](https://github.com/Barbatos6669/PiPinPP/commit/37f565e20a91c8cbf1b34e0367f7d88c769824aa))
* correct CMake coverage option name in docs workflow ([169b8d6](https://github.com/Barbatos6669/PiPinPP/commit/169b8d65c29a4d57d7118a43735a7579ec6ffc8c))
* correct PlatformInfo method call and CONTRIBUTING.md link ([7f6ea0d](https://github.com/Barbatos6669/PiPinPP/commit/7f6ea0d70689a1d22657068cc4a9267ab17beb6d))
* expand hardware test exclusions in docs workflow ([0b85c65](https://github.com/Barbatos6669/PiPinPP/commit/0b85c65ade43632a292bd5581dca6f64efeb5a1f))
* Keep ifdef wrapper for modeStr to avoid unused variable warning ([a16e8e0](https://github.com/Barbatos6669/PiPinPP/commit/a16e8e09cbcff53f2ff898900e23680b7dc078f2))
* prevent workflow failures on Dependabot PRs and missing wiki ([c48fd24](https://github.com/Barbatos6669/PiPinPP/commit/c48fd24c9199331e5ccd81c422cf92774bff5a40))
* Remove redundant -lgpiod from pkg-config Libs field ([7a2e205](https://github.com/Barbatos6669/PiPinPP/commit/7a2e2059a0e464602045514ee4cc11803b4e45cf))
* remove unsupported lcov flags for Ubuntu 22.04 compatibility ([f8989ba](https://github.com/Barbatos6669/PiPinPP/commit/f8989bae10d8076aa1d3841fcaebb6b4bbae5175))
* remove wiki submodule and add to gitignore ([5bff579](https://github.com/Barbatos6669/PiPinPP/commit/5bff5792f8f427b7ee21ced39e884fb32c89624d))
* separate test execution from coverage generation in docs workflow ([e241cdd](https://github.com/Barbatos6669/PiPinPP/commit/e241cdd9d966193d911208b1847fd7bf136ebd9f))
* skip hardware-dependent tests in docs-and-coverage workflow ([97255b9](https://github.com/Barbatos6669/PiPinPP/commit/97255b939d6c40fa57a93e04230ed35ddc4d2ffc))
* **tests:** Fix overflow and unused variable warnings in tests ([b59042d](https://github.com/Barbatos6669/PiPinPP/commit/b59042d3979d6e27c3d77b5ede1278310214f9b3))

## [0.3.10](https://github.com/Barbatos6669/PiPinPP/compare/v0.3.9...v0.3.10) (2025-11-09)


### Bug Fixes

* Add /dev/ prefix to GPIO chip path in interrupts ([a59a0a7](https://github.com/Barbatos6669/PiPinPP/commit/a59a0a7675ea4b16578ff70cdcf4eeac719059ff))

## [0.3.9](https://github.com/Barbatos6669/PiPinPP/compare/v0.3.8...v0.3.9) (2025-11-09)

## [0.3.8](https://github.com/Barbatos6669/PiPinPP/compare/v0.3.7...v0.3.8) (2025-11-08)


### Bug Fixes

* Preserve VERSION variable when sourcing /etc/os-release ([c19dc10](https://github.com/Barbatos6669/PiPinPP/commit/c19dc102830d32971670ca0b055ea9a013dbc20e))

## [0.3.7](https://github.com/Barbatos6669/PiPinPP/compare/v0.3.6...v0.3.7) (2025-11-06)


### Features

* Add Advanced I/O functions ([57963d1](https://github.com/Barbatos6669/PiPinPP/commit/57963d1ff8b743cac432945530ff9f381c1d897c))
* Add comprehensive Advanced I/O demonstration example ([f3c42cd](https://github.com/Barbatos6669/PiPinPP/commit/f3c42cd779be8ab06b9f0570ce189bf0058691a6))

## [0.3.5](https://github.com/Barbatos6669/PiPinPP/compare/v0.3.4...v0.3.5) (2025-11-05)


### Features

* Add extended math functions (v0.3.2) ([fb1d7e1](https://github.com/Barbatos6669/PiPinPP/commit/fb1d7e174616cfd377ea65c8581587ad6dac5259))
* Add one-command installation script ([0f586f0](https://github.com/Barbatos6669/PiPinPP/commit/0f586f088c4a8d36fec35faf679af150296794c5))
* Add pip-style one-line installation for v0.3.2 ([a707c97](https://github.com/Barbatos6669/PiPinPP/commit/a707c9736b406498d4ace658c27cbf890c8bc705))
* Add trigonometry constants for v0.3.3 ([b5f6a52](https://github.com/Barbatos6669/PiPinPP/commit/b5f6a524f5059366c8d224e154d8ddb37b3a6937))
* Add v0.3.4 utility functions (random, bits/bytes) ([3cf42c2](https://github.com/Barbatos6669/PiPinPP/commit/3cf42c2f8eac691674edefefc6e227d9abce727a))


### Bug Fixes

* Handle return values from read/write syscalls in interrupts.cpp ([445798a](https://github.com/Barbatos6669/PiPinPP/commit/445798a85af7a25efb8834444d040850c45ef4b5))
* Remove std library conflicts, keep only sq() function ([61f59da](https://github.com/Barbatos6669/PiPinPP/commit/61f59da64f3c518b27f0983d68fd3b920ad09468))

## [0.3.0](https://github.com/Barbatos6669/PiPinPP/compare/v0.2.0...v0.3.0) (2025-11-03)


### Features

* Add logging system and enhanced build options ([537b6e0](https://github.com/Barbatos6669/PiPinPP/commit/537b6e0a4842ce2fb0038877bb3aff9a325694d8))
* Add thread-safety to ArduinoCompat functions ([7856211](https://github.com/Barbatos6669/PiPinPP/commit/7856211f19df00bc086504a07c1ef1510c652a99))
* Add three comprehensive examples demonstrating v0.3.x features ([9725dbe](https://github.com/Barbatos6669/PiPinPP/commit/9725dbe17e067fd9062db8b09d09929c434594b0))
* **cmake:** Add modern CMake packaging support ([f2d8288](https://github.com/Barbatos6669/PiPinPP/commit/f2d828837c8f2632744c2b4ed308931b486e6aa0))
* **interrupts:** Implement GPIO interrupt support with Arduino-compatible API ([5ec4be1](https://github.com/Barbatos6669/PiPinPP/commit/5ec4be1471de1fd9ba0aaabde158daa0c8bd318c))
* **pwm:** Implement software PWM with Arduino-compatible analogWrite() ([493a70f](https://github.com/Barbatos6669/PiPinPP/commit/493a70f9283527643ce37f510f9b33754c8f7251))


### Bug Fixes

* Update badges in README ([20f9308](https://github.com/Barbatos6669/PiPinPP/commit/20f93089339bf39092d6a90aabc6e5b0df1aaf00))

## [0.2.0](https://github.com/Barbatos6669/PiPinPP/compare/v0.1.0...v0.2.0) (2025-11-01)


### Features

* Add Arduino-style API compatibility layer ([d732e62](https://github.com/Barbatos6669/PiPinPP/commit/d732e62bef025069601944c9bc1ef5060f052ef7))
* Add comprehensive project roadmap and contributor infrastructure ([130d555](https://github.com/Barbatos6669/PiPinPP/commit/130d55594940ebba117abb5cc4aaee747e770a3d))
* Add delay() function for Arduino compatibility ([f622274](https://github.com/Barbatos6669/PiPinPP/commit/f622274ffc4663215bcea17f97bc507cd3f0b7f0))

## [0.1.0](https://github.com/Barbatos6669/PiPinPP/compare/d9ad6591b9a722917c9d2f8a3e83d04d58da07c9...v0.1.0) (2025-10-30)


### Features

* created the base class for assigining a pin, and setting it to input output. ([d9ad659](https://github.com/Barbatos6669/PiPinPP/commit/d9ad6591b9a722917c9d2f8a3e83d04d58da07c9))

