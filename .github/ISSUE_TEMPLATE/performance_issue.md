---
name: Performance Issue
about: Report performance problems or suggest optimizations
title: '[PERF] '
labels: performance
assignees: ''
---

## ⚡ Performance Issue

**What operation is slow?**
- [ ] GPIO toggling (digitalWrite/digitalRead)
- [ ] PWM operations (analogWrite)
- [ ] Interrupt handling
- [ ] I2C/Wire communication
- [ ] SPI communication
- [ ] Serial communication
- [ ] Library initialization
- [ ] Other (specify):

---

## 📊 Performance Details

**How slow is it?**
<!-- Example: GPIO toggle takes 50µs, expected ~10µs -->
- **Observed performance:** 
- **Expected performance:** 
- **Benchmark method:** <!-- How did you measure? -->

**Frequency of operation:**
<!-- How often does this slow operation occur? -->
- [ ] Continuous (e.g., PWM, polling)
- [ ] Frequent (multiple times per second)
- [ ] Occasional (a few times per second)
- [ ] Rare (initialization, setup)

**Impact on your application:**
- [ ] Critical - blocks real-time requirements
- [ ] High - causes noticeable delays
- [ ] Medium - acceptable but could be better
- [ ] Low - minor optimization opportunity

---

## 🖥️ Environment

**Hardware:**
- Raspberry Pi Model: <!-- e.g., Pi 4 Model B, Pi 5 -->
- RAM: <!-- e.g., 2GB, 4GB, 8GB -->
- CPU Load: <!-- e.g., ~50%, ~90% -->
- Other processes running: <!-- Any CPU-intensive tasks? -->

**Software:**
- PiPinPP Version: <!-- e.g., v0.4.0 -->
- Operating System: <!-- e.g., Raspberry Pi OS Bookworm 64-bit -->
- Compiler: <!-- e.g., g++ 11.4.0 -->
- Build type: <!-- Release or Debug? -->
- Compiler flags: <!-- e.g., -O3, -march=native -->

---

## 🔬 Reproduction

**Minimal code example:**
```cpp
// Include setup and measurement code
#include <ArduinoCompat.hpp>
#include <chrono>

int main() {
    pinMode(17, OUTPUT);
    
    auto start = std::chrono::high_resolution_clock::now();
    // Your slow operation here
    for (int i = 0; i < 1000; i++) {
        digitalWrite(17, HIGH);
        digitalWrite(17, LOW);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time: " << duration.count() << "µs\n";
}
```

**Benchmark results:**
<!-- Paste your actual measurements here -->

---

## 💡 Optimization Ideas

**Have you identified the bottleneck?**
<!-- e.g., mutex locks, system calls, memory allocations -->



**Suggested optimizations:**
<!-- If you have ideas, share them! -->



**Comparison with other libraries:**
<!-- Have you tried WiringPi, pigpio, or RPi.GPIO? How do they compare? -->

---

## 📈 Additional Context

**Performance requirements:**
<!-- What performance do you need for your application? -->



**Related issues:**
<!-- Link to related performance issues or discussions -->



**Are you willing to:**
- [ ] Test proposed optimizations
- [ ] Submit a PR with performance improvements
- [ ] Provide benchmark results
- [ ] Help with profiling

