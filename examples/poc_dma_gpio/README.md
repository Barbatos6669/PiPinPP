# DMA GPIO Proof-of-Concept

**Status:** Experimental - Research Phase  
**Purpose:** Validate DMA-based GPIO control for future PiPinPP implementation  
**Date:** November 16, 2025

---

## ⚠️ WARNING: EXPERIMENTAL CODE

This is a **proof-of-concept** to validate DMA GPIO control. It is:

- **NOT production code** - Minimal error handling
- **Requires root** - Direct hardware access via `/dev/mem`
- **Platform-specific** - Raspberry Pi 4 only (BCM2711 addresses)
- **Educational** - Learn DMA concepts before full implementation

**DO NOT** use in production applications!

---

## What This PoC Does

Demonstrates zero-CPU GPIO toggle using Raspberry Pi's DMA controller:

1. Maps GPIO and DMA registers from `/dev/mem`
2. Configures GPIO 17 as output
3. Creates DMA control blocks for GPIO SET/CLEAR
4. Starts DMA in circular mode (infinite loop)
5. GPIO 17 toggles continuously with **0% CPU usage**

### Expected Behavior

- GPIO 17 toggles as fast as DMA allows (microseconds)
- CPU usage: 0% (toggle runs in DMA hardware)
- Console shows DMA status every second
- Ctrl+C stops cleanly

---

## Hardware Setup

### Required

- Raspberry Pi 4 (or Pi 3 with address changes)
- LED + 220Ω resistor connected to GPIO 17
- Or logic analyzer connected to GPIO 17

### Wiring

```
GPIO 17 ──[220Ω]──┤LED├──┴ GND
```

Or connect logic analyzer probe to GPIO 17 and ground.

---

## Building

### Standalone Build

```bash
cd examples/poc_dma_gpio
g++ -o poc_dma_gpio main.cpp -std=c++17 -pthread
```

### CMake Build (Full Project)

```bash
cd /path/to/PiPinPP
./scripts/build.sh
```

The PoC will be at: `build/examples/example_poc_dma_gpio`

---

## Running

### Prerequisites

1. **Root access** required for `/dev/mem`
2. **Raspberry Pi 4** (addresses hardcoded for BCM2711)
3. **No other DMA users** on channel 5

### Execute

```bash
sudo ./poc_dma_gpio
```

### Expected Output

```
╔═══════════════════════════════════════════════════════════════╗
║      PiPin++ DMA GPIO Proof-of-Concept (v0.4.0)              ║
╚═══════════════════════════════════════════════════════════════╝

⚠️  EXPERIMENTAL CODE - REQUIRES ROOT
This PoC validates DMA-based GPIO control.
Expected: GPIO 17 toggles at 1Hz with 0% CPU usage

✓ Opened /dev/mem
✓ Mapped GPIO registers at 0xfe200000
✓ Mapped DMA channel 5 registers
✓ GPIO 17 configured as output
✓ DMA control blocks configured
  CB0 (SET): src=0xc1234000 dst=0xfe20001c
  CB1 (CLR): src=0xc1234004 dst=0xfe200028
✓ DMA started on channel 5
  CS register: 0x1

🚀 DMA running! GPIO 17 toggling...
   Monitor with: watch -n 0.1 'gpio readall | grep GPIO.17'
   Or connect LED/logic analyzer to GPIO 17

Press Ctrl+C to stop.

[1s] DMA Status: ✓ ACTIVE (CS=0x1)
[2s] DMA Status: ✓ ACTIVE (CS=0x1)
[3s] DMA Status: ✓ ACTIVE (CS=0x1)
^C

Shutting down DMA...
Cleaning up...
✓ DMA stopped
✓ GPIO 17 set LOW
✓ Unmapped DMA control blocks
✓ Unmapped DMA data buffer
✓ Unmapped registers, closed /dev/mem

✅ PoC completed successfully!
Next: Add delay mechanism for actual PWM timing
```

### Verification

Check GPIO state with:
```bash
# In another terminal (while PoC running)
watch -n 0.1 'gpio readall | grep GPIO.17'

# Or use logic analyzer
# Should see rapid toggling (DMA limited, not CPU limited)
```

---

## How It Works

### 1. Memory Mapping

```cpp
// Map GPIO registers
void* gpioMap = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, memFd, GPIO_BASE);

// Map DMA channel 5 registers
void* dmaMap = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE,
                    MAP_SHARED, memFd, DMA_BASE + (5 * 0x100));
```

### 2. DMA Control Blocks

Two control blocks in circular loop:

```
CB0: Write to GPSET0 (set GPIO 17 HIGH)
  ↓
CB1: Write to GPCLR0 (set GPIO 17 LOW)
  ↓
(loop back to CB0)
```

Each CB structure:
```cpp
struct dma_cb_t {
    uint32_t info;    // Transfer flags
    uint32_t src;     // Source: memory buffer with bit mask
    uint32_t dst;     // Destination: GPIO register
    uint32_t length;  // 4 bytes (one 32-bit word)
    uint32_t next;    // Next CB address (circular)
};
```

### 3. Bus Addresses

DMA uses **bus addresses**, not virtual addresses:
```cpp
uint32_t memVirtToBus(void* virt) {
    // Pi 4: SDRAM is at 0xC0000000 in bus address space
    return 0xC0000000 | ((uint32_t)(uintptr_t)virt & 0x3FFFFFFF);
}
```

**Important:** Full implementation needs proper MMU translation!

### 4. Circular Execution

DMA runs forever because:
- CB0.next points to CB1
- CB1.next points back to CB0
- ACTIVE bit set once, DMA self-sustains

**Result:** Zero CPU, GPIO toggles in hardware!

---

## Limitations of This PoC

### 1. No Timing Control

Currently toggles as fast as DMA allows. For PWM, need:
- PWM clock for precise delays
- Or DMA pacing via peripheral DREQ
- Or software delays between control blocks

### 2. No Error Recovery

Minimal error handling. Production code needs:
- Validate memory mappings
- Check DMA errors
- Handle channel conflicts
- Timeout mechanisms

### 3. Platform-Specific

Hardcoded for Pi 4 (BCM2711):
```cpp
constexpr uint32_t BCM2711_PERI_BASE = 0xFE000000;  // Pi 4
// Pi 3 uses: 0x3F000000
// Pi 5 uses: 0x1F00000000 (64-bit!)
```

Production needs runtime detection.

### 4. Single Pin Only

Only GPIO 17 supported. Full implementation needs:
- Multi-pin coordination
- Pin validation
- Conflict detection

### 5. Bus Address Simplification

```cpp
uint32_t memVirtToBus(void* virt) {
    return 0xC0000000 | ((uint32_t)(uintptr_t)virt & 0x3FFFFFFF);
}
```

**Assumption:** Small allocations stay contiguous in lower 1GB.  
**Production needs:** Proper physical memory allocator or CMA (Contiguous Memory Allocator).

---

## Next Steps

### Phase 2: Add PWM Timing

Current: Rapid toggle (DMA speed limited)  
Goal: Controlled frequency PWM

**Options:**
1. **PWM Clock Pacing** - Use PWM peripheral as DMA pacer
2. **Delay Loops** - Add more CBs for software delays
3. **Timer-based** - Sync DMA to system timer

### Phase 3: Multi-Pin Support

- Array of control blocks per pin
- Coordinate multiple DMA channels
- Pin conflict detection

### Phase 4: Platform Abstraction

- Runtime detection (Pi 3, 4, 5, CM4)
- Bus address translation via `/proc/device-tree`
- Graceful fallback if DMA unavailable

### Phase 5: Production Hardening

- RAII wrappers for memory management
- Exception safety
- Thread synchronization
- Comprehensive error handling

---

## Troubleshooting

### "Failed to open /dev/mem"

**Cause:** Not running as root  
**Solution:** Use `sudo ./poc_dma_gpio`

### "Failed to map GPIO/DMA registers"

**Cause:** Wrong peripheral base address  
**Solution:** Verify Raspberry Pi model:
```bash
cat /proc/cpuinfo | grep "Model"
```

- Pi 3: Change `BCM2711_PERI_BASE` to `0x3F000000`
- Pi 4: Use `0xFE000000` (current)
- Pi 5: Needs 64-bit address space (`0x1F00000000`)

### "DMA error detected"

**Cause:** DMA channel 5 in use, or bad control blocks  
**Solution:**
1. Check DMA usage: `cat /sys/kernel/debug/dma`
2. Try different channel (6 or 7)
3. Validate control block addresses

### GPIO 17 doesn't toggle

**Cause:** DMA not actually accessing GPIO  
**Debug:**
```bash
# Check DMA status
sudo cat /sys/kernel/debug/dma | grep "DMA channel 5"

# Check GPIO state
gpio readall | grep GPIO.17
```

**Solution:** Verify bus address calculation is correct for your Pi model.

---

## Learn More

### DMA Resources

- [BCM2835 ARM Peripherals](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf) - Chapter 4: DMA
- [pigpio source code](https://github.com/joan2937/pigpio) - `pigpio.c` (DMA implementation)
- [Raspberry Pi DMA Tutorial](https://github.com/Wallacoloo/Raspberry-Pi-DMA-Example)

### PiPinPP Documentation

- [DMA Design Document](../../docs/planning/DMA_GPIO_DESIGN.md)
- [Roadmap Phase 1](../../docs/ROADMAP.md#phase-1-performance-foundation)
- [EventPWM (software alternative)](../../docs/API_REFERENCE.md#event-driven-pwm)

---

## Safety Notes

### Before Running

1. **Backup your Pi** - Direct hardware access can brick systems
2. **Test on spare Pi** - Don't test on production systems
3. **Know your model** - Wrong addresses can corrupt hardware
4. **Have serial console** - If SSH fails, you need recovery access

### During Development

- Start with GPIO read tests (safer than write)
- Validate all addresses before accessing
- Always implement signal handlers (Ctrl+C)
- Clean up on ALL exit paths

### Production Considerations

- Never use `/dev/mem` in libraries (security risk)
- Consider kernel module for production DMA
- Implement capability-based permissions
- Validate all user inputs

---

## License

Part of PiPinPP project - MIT License

**Disclaimer:** This code is for educational purposes. Direct hardware access can damage your Raspberry Pi if used incorrectly. No warranty provided.

---

**Status:** ✅ Research phase complete  
**Next:** Implement PWM timing control (Phase 2)
