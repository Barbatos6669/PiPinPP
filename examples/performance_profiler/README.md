# Performance Profiler Example

Analyze PiPin++ timing primitives under real Linux load. This example captures delay drift,
scheduling jitter, and GPIO toggle throughput, then writes the raw data to `performance_profile.csv`
so you can graph it in Python, Excel, or Grafana.

## What It Measures

- **Delay drift** – `delay()` accuracy over multiple samples (ms error relative to target)
- **Loop jitter** – `delayMicroseconds()` deviation while maintaining a 1 kHz control loop
- **GPIO throughput** – Average cost of paired `digitalWrite()` calls on GPIO 17
- **CSV logging** – Automatically records both sample series plus summary metrics

## Hardware

- Optional LED + 220Ω resistor on **GPIO 17** to visualize the toggle test
- No external gear is required for timing measurements

## Build & Run

```bash
cd /path/to/PiPinPP/build
cmake .. -DBUILD_EXAMPLES=ON # only needed the first time
make example_performance_profiler
./examples/example_performance_profiler   # use sudo if you are not in the gpio group
```

Sample output:

```
=== Delay Drift (target 25 ms) ===
Avg drift : +0.12 ms
Min / Max : -1 ms / +2 ms

=== Loop Jitter (1000 µs interval) ===
Avg jitter: +0.7 µs
Min / Max : -3 µs / +5 µs

=== GPIO Toggle Throughput (pin 17) ===
Cycles executed : 5000 high/low pairs
Total duration : 8421 µs
Cycle rate     : 593.68 Hz
Write cost     : 0.842 µs per digitalWrite
```

## CSV Logging

Running the example creates `performance_profile.csv` in the current working directory. The file
contains two sections:

1. **Sample data** (`sample,delay_error_ms,loop_jitter_us`) for quick plotting or histogramming
2. **Summary metrics** (cycle duration, rate, write cost) for dashboards or regression tracking

Open the CSV in your favorite plotting tool or import it into a Python notebook:

```python
import pandas as pd
samples = pd.read_csv("performance_profile.csv", comment="#", nrows=1500)
samples.plot(y="loop_jitter_us", title="PiPin++ loop jitter")
```

## Tips

- Close other CPU-intensive workloads to measure the best-case jitter
- Start multiple background `stress --cpu` jobs to observe how Linux scheduling impacts latency
- Change `PROFILER_PIN`, `LOOP_INTERVAL_US`, or `TOGGLE_CYCLES` in `main.cpp` to profile different scenarios

## Related Examples

- [`examples/timing_benchmark/`](../timing_benchmark/) – Friendly tour of `millis()`/`micros()`
- [`examples/benchmark_cpu/`](../benchmark_cpu/) – Compare busy-loop vs event-driven PWM CPU usage
- [`examples/thread_safety/`](../thread_safety/) – Exercises PiPin++ mutex-protected pin registry
