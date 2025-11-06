# Thread Safety Example

Demonstrates safe multi-threaded GPIO access with PiPinPP.

## Features

- Multiple threads accessing GPIO simultaneously
- Thread-safe pin registry
- Concurrent `digitalWrite()` operations
- No race conditions or conflicts

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_thread_safety
./examples/example_thread_safety
```

## What You'll See

Multiple threads blinking different LEDs concurrently without interference:

```
Thread 1: Blinking GPIO 18
Thread 2: Blinking GPIO 23  
Thread 3: Blinking GPIO 24
All threads running concurrently...
```

## Hardware Setup

- **GPIO 18** → LED → 220Ω → GND
- **GPIO 23** → LED → 220Ω → GND
- **GPIO 24** → LED → 220Ω → GND

## Key Concepts

### Thread-Safe Pin Access
```cpp
// Safe: Multiple threads can call pinMode() on same pin
std::thread t1([]{ pinMode(18, OUTPUT); });
std::thread t2([]{ pinMode(18, OUTPUT); });
// Both threads get the same Pin object from registry
```

### Internal Mutex Protection
PiPinPP uses mutexes internally to protect:
- Pin registry access
- GPIO hardware operations
- Configuration changes

### What's Safe
✅ Multiple threads reading/writing different pins
✅ Multiple threads configuring the same pin
✅ Concurrent interrupts and GPIO operations

### What to Avoid
❌ Relying on exact timing with multiple threads
❌ Assuming atomic operations across multiple calls
❌ Not synchronizing your own application logic

## Performance Notes

- Mutex locking adds ~1-2µs overhead per operation
- Acceptable for most applications
- For ultra-high-speed (>100kHz toggle rates), use single thread

## References

- [C++ std::thread](https://en.cppreference.com/w/cpp/thread/thread)
- [PiPinPP Architecture](../../docs/API_REFERENCE.md#thread-safety)

## License

Part of the PiPinPP project.
