# DMA-based GPIO Design Document

**Status:** Research Phase  
**Priority:** Phase 1, Item #2 (Performance Foundation)  
**Target:** Q1 2026  
**Date:** November 16, 2025

---

## Executive Summary

Implement optional DMA-based GPIO backend for hardware-accurate waveform generation with **zero CPU overhead**. This is the final piece of Phase 1 (Performance Foundation), completing the PWM performance story:

- **Software PWM (analogWrite)**: 10-30% CPU, <5µs jitter
- **EventPWM**: <5% CPU, <10µs jitter (✅ COMPLETED v0.4.0)
- **HardwarePWM**: 0% CPU, 0µs jitter (limited to 4 pins)
- **DMA GPIO** (NEW): 0% CPU, <1µs jitter (any GPIO, advanced users)

---

## Problem Statement

### Current Limitations

1. **Software PWM** - High CPU usage limits multi-channel applications
2. **EventPWM** - Good efficiency but still has 10µs jitter
3. **HardwarePWM** - Perfect but only 4 pins available (GPIO 12,13,18,19)

### Use Cases Requiring DMA

- **Servo arrays** (8+ servos, need <1µs jitter)
- **LED matrix control** (precise timing for WS2812B/NeoPixels)
- **Software UART** (high-speed serial without hardware UART)
- **Stepper motor control** (microsecond-accurate step pulses)
- **Logic analyzer** (record/replay GPIO patterns)
- **Protocol emulation** (I2C, SPI, custom protocols)

---

## pigpio Analysis

### How pigpio Achieves Zero-CPU PWM

**Key Insight:** pigpio uses Raspberry Pi's DMA controller to write GPIO registers directly from memory, bypassing CPU entirely.

#### Architecture

```
Memory Buffer          DMA Controller         GPIO Registers
┌────────────┐        ┌──────────────┐       ┌──────────┐
│ GPIO SET   │───────>│              │──────>│ GPSETx   │
│ GPIO CLR   │        │   DMA CBs    │       │ GPCLRx   │
│ DELAY      │        │ (Control     │       │          │
│ GPIO SET   │        │  Blocks)     │       │          │
│ ...        │        │              │       │          │
└────────────┘        └──────────────┘       └──────────┘
      ↑                      ↑
      └──────────────────────┘
        Circular buffer (no CPU!)
```

#### DMA Control Blocks (CBs)

pigpio creates circular linked list of DMA Control Blocks:
```c
struct dma_cb_t {
    uint32_t info;      // Transfer info
    uint32_t src;       // Source address (memory buffer)
    uint32_t dst;       // Destination (GPIO registers)
    uint32_t length;    // Transfer length
    uint32_t stride;    // 2D stride (unused)
    uint32_t next;      // Next CB address (circular!)
    uint32_t pad[2];    // Reserved
};
```

**PWM Generation:**
1. CB1: Write GPIO SET register (LED on)
2. CB2: DMA delay (busy-wait in DMA controller)
3. CB3: Write GPIO CLR register (LED off)
4. CB4: DMA delay (off period)
5. CB5: Jump to CB1 (circular, infinite loop)

**Result:** CPU sets up once, DMA runs forever with zero CPU overhead!

#### Timing Precision

pigpio uses PWM clock as DMA pacing:
- PWM hardware generates timing ticks
- DMA waits for PWM tick before next transfer
- Achieves sub-microsecond accuracy

---

## Implementation Strategy for PiPinPP

### Design Principles

1. **Optional Backend** - DMA is advanced feature, not default
2. **Fallback Required** - Must work on platforms without DMA
3. **libgpiod First** - Use libgpiod where possible, DMA for special cases
4. **Safety Critical** - Direct hardware access requires root, validation
5. **Modern C++** - RAII, smart pointers, no raw memory manipulation

### Proposed Architecture

```cpp
// Abstract interface
class PWMBackend {
public:
    virtual bool begin(int pin, double freq, double duty) = 0;
    virtual void setDutyCycle(double duty) = 0;
    virtual void end() = 0;
    virtual ~PWMBackend() = default;
};

// Existing implementations
class SoftwarePWM : public PWMBackend { /* busy-loop */ };
class EventPWM : public PWMBackend { /* hybrid timing */ };
class HardwarePWM : public PWMBackend { /* sysfs */ };

// NEW: DMA implementation
class DMAPWM : public PWMBackend {
private:
    int dmaChannel_;
    void* dmaMemory_;       // Shared memory for DMA
    dma_cb_t* controlBlocks_;
    volatile uint32_t* gpioRegisters_;
    
public:
    bool begin(int pin, double freq, double duty) override;
    void setDutyCycle(double duty) override;
    void end() override;
};

// Factory pattern
std::unique_ptr<PWMBackend> PWMFactory::create(PWMType type, int pin) {
    switch (type) {
        case PWMType::Software: return std::make_unique<SoftwarePWM>(pin);
        case PWMType::Event: return std::make_unique<EventPWM>(pin);
        case PWMType::Hardware: return std::make_unique<HardwarePWM>(pin);
        case PWMType::DMA: return std::make_unique<DMAPWM>(pin);
    }
}
```

### Technical Challenges

#### 1. Memory Mapping

**Problem:** DMA requires physical memory addresses, not virtual.

**Solution:** Use `/dev/mem` to map GPIO and DMA registers:
```cpp
int memFd = open("/dev/mem", O_RDWR | O_SYNC);
void* gpioMap = mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, 
                     MAP_SHARED, memFd, GPIO_BASE);
```

**Safety:** Requires `root` or `CAP_SYS_RAWIO` capability.

#### 2. DMA Channel Allocation

**Problem:** Raspberry Pi has 16 DMA channels, some reserved by OS.

**Solution:** Query `/sys/kernel/debug/dma` or use safe channels (5-7):
```cpp
int allocateDMAChannel() {
    // Try channels 5, 6, 7 (typically safe)
    for (int ch : {5, 6, 7}) {
        if (tryAllocateChannel(ch)) return ch;
    }
    throw std::runtime_error("No DMA channels available");
}
```

#### 3. Platform Detection

**Problem:** DMA register addresses differ per Raspberry Pi model.

**Solution:** Runtime detection via `/proc/cpuinfo` or device tree:
```cpp
enum class RPiModel { PI3, PI4, PI5, CM4, UNKNOWN };

RPiModel detectModel() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("Raspberry Pi 5") != std::string::npos) 
            return RPiModel::PI5;
        // ... check other models
    }
    return RPiModel::UNKNOWN;
}

uint32_t getGPIOBase(RPiModel model) {
    switch (model) {
        case RPiModel::PI3: return 0x3F200000;
        case RPiModel::PI4: return 0xFE200000;
        case RPiModel::PI5: return 0x1F00000000; // 64-bit address!
        default: throw std::runtime_error("Unknown Pi model");
    }
}
```

#### 4. Circular Buffer Management

**Problem:** DMA needs contiguous physical memory.

**Solution:** Use `mmap` with `MAP_LOCKED` or DMA-able memory allocator:
```cpp
void* allocateDMAMemory(size_t size) {
    // Option 1: /dev/mem + mmap
    void* mem = mmap(NULL, size, PROT_READ|PROT_WRITE,
                     MAP_SHARED|MAP_LOCKED, memFd, DMA_MEM_BASE);
    
    // Option 2: CMA (Contiguous Memory Allocator)
    // Requires kernel support
    
    return mem;
}
```

#### 5. Duty Cycle Updates

**Problem:** How to change duty cycle without stopping DMA?

**Solution:** Double-buffering or atomic register writes:
```cpp
void DMAPWM::setDutyCycle(double duty) {
    // Calculate new ON/OFF times
    uint32_t onTicks = calculateTicks(duty);
    uint32_t offTicks = calculateTicks(100.0 - duty);
    
    // Atomically update control block delays
    // DMA will use new values on next cycle
    controlBlocks_[1].length = onTicks;
    controlBlocks_[3].length = offTicks;
    
    // Memory barrier to ensure writes visible to DMA
    __sync_synchronize();
}
```

---

## Implementation Phases

### Phase 1: Research & Validation (CURRENT)
- ✅ Document pigpio architecture
- ✅ Identify technical challenges
- 🔄 Create proof-of-concept (single GPIO toggle)
- 🔄 Validate on Raspberry Pi 4

### Phase 2: Core DMA Engine
- Implement memory mapping wrapper (`DMAMemory` class)
- Create DMA control block builder
- Platform detection (Pi 3, 4, 5, CM4)
- Safe channel allocation

### Phase 3: PWM Implementation
- `DMAPWM` class implementing `PWMBackend` interface
- Circular buffer for continuous PWM
- Duty cycle updates
- Frequency changes
- Multi-pin coordination

### Phase 4: Testing & Safety
- Root permission checks
- Graceful fallback if DMA unavailable
- Memory leak prevention (RAII)
- Kernel panic prevention (validate addresses)
- Unit tests (require hardware)

### Phase 5: Documentation & Examples
- API documentation
- Safety warnings
- Example: NeoPixel WS2812B control
- Example: Servo array (16+ servos)
- Example: Custom waveform generator

---

## API Design

### User-Facing API

```cpp
#include "dma_pwm.hpp"

int main() {
    try {
        // Check if DMA is available
        if (!pipinpp::DMA::isAvailable()) {
            std::cerr << "DMA not available, falling back to EventPWM\n";
            // Use EventPWM instead
            return 1;
        }
        
        // Create DMA-backed PWM
        pipinpp::DMAPWM servo1(18);  // GPIO 18
        pipinpp::DMAPWM servo2(19);  // GPIO 19
        
        // Initialize at 50Hz (servo frequency)
        if (!servo1.begin(50, 7.5)) {  // 7.5% = center position
            throw std::runtime_error("Failed to initialize DMA PWM");
        }
        servo2.begin(50, 5.0);  // 5% = min position
        
        // Smooth servo sweep
        for (double pos = 5.0; pos <= 10.0; pos += 0.1) {
            servo1.setDutyCycle(pos);
            delay(20);
        }
        
        // Clean shutdown (stops DMA, unmaps memory)
        servo1.end();
        servo2.end();
        
    } catch (const pipinpp::DMAError& e) {
        std::cerr << "DMA Error: " << e.what() << std::endl;
        std::cerr << "Tip: Run with sudo or add CAP_SYS_RAWIO\n";
        return 1;
    }
    
    return 0;
}
```

### Advanced API (Waveform Generation)

```cpp
// For advanced users: arbitrary waveform generation
pipinpp::DMAWaveform waveform;

// Define custom GPIO pattern
waveform.addGPIOSet({17, 18, 19});   // Set pins HIGH
waveform.addDelay(1000);             // 1µs delay
waveform.addGPIOClear({17});         // Clear pin 17
waveform.addDelay(500);              // 0.5µs
waveform.addGPIOClear({18, 19});     // Clear remaining
waveform.addDelay(2000);             // 2µs

// Repeat forever
waveform.setRepeat(true);

// Execute via DMA
waveform.start();  // Runs until stop() called
```

---

## Safety Considerations

### Permission Model

**Requirement:** DMA requires direct hardware access.

**Options:**
1. **Run as root** - Simple but dangerous
2. **Capabilities** - `CAP_SYS_RAWIO` capability
3. **udev rules** - Allow specific users to access `/dev/mem`
4. **Kernel module** - Custom driver (overkill for PiPinPP)

**Recommendation:** Option 2 (capabilities) with clear documentation.

### Error Handling

```cpp
class DMAError : public std::runtime_error {
public:
    enum class Type {
        NO_PERMISSION,      // Can't open /dev/mem
        NO_CHANNELS,        // All DMA channels busy
        INVALID_ADDRESS,    // Bad memory mapping
        HARDWARE_FAULT      // DMA controller error
    };
    
    DMAError(Type type, const std::string& msg);
    Type type() const { return type_; }
};
```

### Resource Cleanup

**Critical:** DMA must be stopped cleanly to prevent GPIO stuck states or kernel panics.

```cpp
class DMAPWM {
private:
    struct DMAGuard {
        DMAPWM* pwm;
        ~DMAGuard() {
            if (pwm && pwm->isRunning()) {
                pwm->emergencyStop();  // Ensures cleanup
            }
        }
    };
    DMAGuard guard_{this};
    
public:
    ~DMAPWM() {
        end();  // Normal cleanup
    }
    
    void emergencyStop() {
        // Stop DMA immediately
        // Reset GPIO to safe state
        // Unmap memory
    }
};
```

---

## Performance Expectations

### Benchmarks (Estimated)

| Metric | SoftwarePWM | EventPWM | HardwarePWM | DMAPWM |
|--------|-------------|----------|-------------|---------|
| **CPU Usage** | 10-30% | <5% | 0% | **0%** |
| **Jitter** | <5µs | <10µs | 0µs | **<1µs** |
| **Max Pins** | 2-3 | 10+ | 4 | **28** |
| **Max Frequency** | 10kHz | 10kHz | 25MHz | **1MHz** |
| **Servo Control** | ❌ | ❌ | ✅ | **✅** |
| **NeoPixels** | ❌ | ❌ | ❌ | **✅** |
| **Root Required** | ❌ | ❌ | ❌ | **⚠️ Yes** |

### Memory Footprint

- Control blocks: ~256 bytes per pin
- DMA buffers: ~4KB per pin
- Total: ~5KB per active DMA channel

**Advantage over pigpio:** PiPinPP uses modern C++ RAII, no global daemon process required.

---

## Alternatives Considered

### 1. Fork pigpio

**Pros:** Proven, mature codebase  
**Cons:** C codebase, daemon architecture, hard to integrate with libgpiod

**Decision:** Build from scratch with modern C++, learn from pigpio's approach

### 2. Use WiringPi DMA

**Pros:** Simpler than pigpio  
**Cons:** WiringPi deprecated, less feature-complete

**Decision:** Rejected

### 3. Kernel Module

**Pros:** Safest, no /dev/mem required  
**Cons:** Complex, requires kernel compilation, distribution headaches

**Decision:** Too complex for library users

### 4. Skip DMA Entirely

**Pros:** Simpler, safer  
**Cons:** Missing key use case (NeoPixels, servo arrays)

**Decision:** Implement as optional feature with clear warnings

---

## Success Criteria

### Minimum Viable Product (MVP)

- [ ] Single GPIO DMA toggle working
- [ ] PWM generation via DMA (50Hz-1kHz)
- [ ] Duty cycle updates without stopping
- [ ] Clean shutdown (no kernel panics)
- [ ] Works on Raspberry Pi 4

### Full Feature Set

- [ ] Multi-pin DMA coordination
- [ ] Support Pi 3, 4, 5, CM4
- [ ] NeoPixel example (WS2812B timing)
- [ ] Servo array example (16 servos)
- [ ] Performance benchmarks vs EventPWM
- [ ] Comprehensive documentation with safety warnings

### Production Ready

- [ ] Zero kernel panics after 1000 hours uptime
- [ ] Graceful fallback if DMA unavailable
- [ ] Memory leak free (valgrind clean)
- [ ] Thread-safe
- [ ] Integration tests

---

## Timeline

**Target:** Q1 2026 (3 months)

- **Week 1-2:** Proof-of-concept (single GPIO toggle)
- **Week 3-4:** Core DMA engine (memory mapping, CB builder)
- **Week 5-6:** PWM implementation
- **Week 7-8:** Multi-pin support
- **Week 9-10:** Testing & safety hardening
- **Week 11-12:** Documentation, examples, polish

**Deliverable:** PiPinPP v0.5.0 with optional DMA backend

---

## Open Questions

1. **Should DMA be default for advanced users?**
   - Proposal: Auto-detect and use if available + root permissions
   - Fallback to EventPWM otherwise

2. **How to handle Pi 5's 64-bit address space?**
   - Pi 5 uses different memory layout
   - May need platform-specific code paths

3. **License implications of studying pigpio?**
   - pigpio is Unlicense (public domain)
   - Safe to learn from, implement independently

4. **Integration with existing PWM API?**
   - Option A: New class `DMAPWM` (separate)
   - Option B: Backend of existing `analogWrite()` (transparent)
   - Recommendation: Option A for v0.5.0, Option B for v1.0.0

---

## References

- [pigpio C library](http://abyz.me.uk/rpi/pigpio/) - DMA implementation reference
- [BCM2835 ARM Peripherals](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf) - Hardware datasheet
- [Raspberry Pi DMA Programming](https://github.com/Wallacoloo/Raspberry-Pi-DMA-Example) - Example code
- [Linux DMA Engine API](https://www.kernel.org/doc/html/latest/driver-api/dmaengine/index.html) - Kernel DMA docs

---

**Next Step:** Create proof-of-concept to validate DMA GPIO toggle on Raspberry Pi 4.
