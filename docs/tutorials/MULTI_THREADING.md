# Tutorial: Multi-Threading with PiPin++

Coordinate multiple GPIO tasks at once using C++ threads without stepping on each other.

## Overview
- **Project:** Two threads blink separate LEDs while a third thread prints timing info
- **Skills:** `std::thread`, mutex-safe ArduinoCompat API usage, graceful shutdown
- **Difficulty:** ⭐⭐⭐ Hard
- **Time:** ~30 minutes

## Hardware
- Raspberry Pi
- Two LEDs + 220 Ω resistors (GPIO17 and GPIO27 suggested)
- Optional: button to exit

## Steps

1. **Build the example:**
   ```bash
   ./build.sh --debug
   cmake --build build --target example_thread_safety
   ```

2. **Run it:**
   ```bash
   ./build/examples/example_thread_safety
   ```
   You’ll see two blink rates plus periodic status output; press Ctrl+C to exit.

3. **Explore the code:**
   - Each thread calls Arduino-style APIs; the internal pin registry handles locking.
   - `std::atomic<bool>` coordinates shutdown on SIGINT.
   - Try adding a third thread that reads a button and toggles both LEDs.

## Tips
- Keep callbacks short—long blocking calls reduce responsiveness across threads.
- Prefer `delay()`/`delayMicroseconds()` to `std::this_thread::sleep_for` for consistency.
- For CPU-intensive work, offload to worker threads but funnel GPIO writes through the Arduino layer.

## Troubleshooting
- **Racey output:** Ensure each thread uses its own pin or share via dedicated functions with locks.
- **Permission issues:** Same as other tutorials—ensure access to `/dev/gpiochip*`.
- **Unclean exit:** Use `signal(SIGINT, handler)` to flip an atomic flag and join threads.

## Next Steps
- Combine with `INTERRUPTS_101` to handle asynchronous button presses.
- Move heavy logic to `std::async` tasks and keep GPIO operations tight.
- Experiment with `pipinpp` CLI running in parallel to observe real-time pin state changes.
