# Robot Stress Test

Comprehensive stress test that simulates a complete robot system using all PiPinPP API features.

## 🤖 Simulated Robot Features

### Hardware Components
- **Differential Drive Motors** (2x PWM + 2x direction pins)
- **Bumper Sensors** (2x digital inputs with pull-ups)
- **Line Following Sensors** (2x digital inputs)
- **Ultrasonic Distance Sensor** (trigger + echo pins)
- **Status LEDs** (4x: power, status, error, heartbeat)
- **Emergency Stop Button** (interrupt-driven)

### Software Features
- **Multi-threaded Operation** (4 concurrent threads)
- **Thread-safe GPIO Access** (mutex-protected)
- **Real-time Interrupt Handling** (emergency stop)
- **PWM Motor Control** (variable speed)
- **Sensor Fusion** (multiple inputs)
- **State Machine Navigation** (autonomous behavior)
- **Performance Monitoring** (operations per second)

## 📋 API Coverage

This stress test exercises:

### Core Functions ✓
- `pinMode()` - 16 pins configured
- `digitalWrite()` - Motor control, LEDs, ultrasonic trigger
- `digitalRead()` - All sensors
- `analogWrite()` - 2 motor PWM channels
- `digitalToggle()` - Efficient LED blinking

### Pin Query Functions ✓
- `isOutput()` - Motor and LED validation
- `isInput()` - Sensor validation
- `getMode()` - Configuration verification

### Timing Functions ✓
- `millis()` - State machine timing, performance tracking
- `delay()` - Thread synchronization
- `delayMicroseconds()` - Ultrasonic sensor timing

### Interrupts ✓
- `attachInterrupt()` - Emergency stop button
- `detachInterrupt()` - Clean shutdown

### Math Functions ✓
- `map()` - Speed ramping
- `constrain()` - Safe motor speed limits
- `abs()` - Bidirectional motor control

### Exception Handling ✓
- `PinError` - Invalid pin operations
- `GpioAccessError` - Hardware access failures
- Recovery and error counting

## 🔌 Pin Connections

### Motors
- GPIO 18 - Left motor PWM
- GPIO 19 - Right motor PWM
- GPIO 23 - Left motor direction
- GPIO 24 - Right motor direction

### Sensors
- GPIO 17 - Front bumper (INPUT_PULLUP)
- GPIO 27 - Back bumper (INPUT_PULLUP)
- GPIO 22 - Left line sensor (INPUT_PULLUP)
- GPIO 10 - Right line sensor (INPUT_PULLUP)
- GPIO 16 - Ultrasonic trigger (OUTPUT)
- GPIO 12 - Ultrasonic echo (INPUT)

### LEDs
- GPIO 5 - Power LED (green)
- GPIO 6 - Status LED (yellow)
- GPIO 13 - Error LED (red)
- GPIO 26 - Heartbeat LED (blue)

### Control
- GPIO 4 - Emergency stop button (INPUT_PULLUP, interrupt)

### Stress Test
- GPIO 20 - Rapid toggle test #1
- GPIO 21 - Rapid toggle test #2

**Total: 18 GPIO pins used**

## 🏗️ Build Instructions

### From project root:
```bash
cd examples/robot_stress_test
mkdir build && cd build
cmake ..
make
```

### Or use main project build:
```bash
cd PiPinPP
mkdir build && cd build
cmake ..
make
./examples/robot_stress_test/robot_stress_test
```

## 🚀 Running the Test

### Standard run (30 seconds):
```bash
sudo ./robot_stress_test
```

### With GPIO hardware:
Connect LEDs and buttons according to pin mapping above, then run.

### Without hardware:
The test will run but may throw `GpioAccessError` exceptions when accessing unavailable pins. Error counter will track these.

## 📊 Performance Metrics

The test reports:
- **Total operations** - All GPIO and system calls
- **Motor updates** - PWM write operations
- **Sensor reads** - Digital read operations
- **LED toggles** - Digital toggle operations
- **Errors encountered** - Exception count
- **Operations/second** - Throughput metric

### Expected Performance (Raspberry Pi 4/5):
- **Operations/sec**: 500-1000+ ops/sec
- **Errors**: 0 (with proper hardware), >0 (simulated)
- **Thread safety**: 100% (no race conditions)

## 🧵 Thread Architecture

### Thread 1: Motor Control (50ms cycle)
- State machine navigation
- Obstacle avoidance
- Speed ramping
- Bumper monitoring

### Thread 2: Sensor Monitoring (30ms cycle)
- Line sensor reading
- Distance measurement
- LED status updates
- Collision detection

### Thread 3: System Monitor (100ms cycle)
- Heartbeat LED (500ms toggle)
- Performance reporting (5s interval)
- Pin configuration validation
- Health checks

### Thread 4: Stress Test (rapid)
- Continuous rapid GPIO toggling
- Thread-safety verification
- Maximum throughput test
- Concurrency stress

## 🛑 Emergency Stop

Press E-Stop button (GPIO 4 pulled LOW) or Ctrl+C to:
1. Trigger interrupt handler
2. Set emergency stop flag
3. Stop all motors immediately
4. Halt all threads
5. Generate final report

## ✅ Success Criteria

**PASS** if:
- ✓ All threads start successfully
- ✓ No deadlocks or race conditions
- ✓ No unhandled exceptions
- ✓ Clean shutdown sequence
- ✓ Error count = 0 (with hardware)
- ✓ Operations > 10,000

**PARTIAL PASS** if:
- ⚠️ Some GpioAccessError (no hardware)
- ⚠️ Error count > 0 but < 10
- ⚠️ Threads complete normally

**FAIL** if:
- ❌ Segmentation fault
- ❌ Deadlock (threads hang)
- ❌ Unhandled exception crash
- ❌ Memory corruption

## 🔍 What This Tests

### Functional
- All v0.3.6 API functions work correctly
- Pin state queries return accurate results
- Interrupts trigger reliably
- PWM generates proper signals
- Timing functions maintain accuracy

### Concurrency
- Multiple threads access GPIO safely
- Mutex locks prevent race conditions
- No deadlocks under load
- Thread-local state is isolated

### Robustness
- Exceptions are caught and handled
- System recovers from errors
- Invalid operations are rejected
- Clean shutdown is reliable

### Performance
- High-frequency GPIO operations
- Minimal overhead from library
- Efficient toggle implementation
- Acceptable throughput

## 📝 Notes

- **Simulated components**: Ultrasonic uses random distance, bumpers need physical buttons
- **Safety**: All motors stop on error or emergency stop
- **Adjustable duration**: Change countdown in main() for longer tests
- **Pin flexibility**: Modify pin constants for different hardware
- **Production ready**: This pattern can be used in real robot code

## 🐛 Known Limitations

- Ultrasonic distance is simulated (no `pulseIn()` yet - planned v0.4.0)
- Analog sensors not tested (no ADC yet - planned v0.5.0)
- Serial communication not tested (planned v0.4.0)

## 🎯 Future Enhancements

When v0.4.0 APIs are available:
- Add I2C IMU sensor (accelerometer/gyro)
- Add SPI motor controller communication
- Add UART GPS or Bluetooth module
- Add real ultrasonic distance measurement
- Add servo control for robot arm

---

**This stress test demonstrates that PiPinPP v0.3.6 is production-ready for real-world robotics applications!** 🚀
