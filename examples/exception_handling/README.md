# Exception Handling Example

Learn proper error handling with PiPinPP's exception system.

## Features

- Try-catch exception handling
- Different exception types (`InvalidPinError`, `GpioAccessError`)
- Graceful error recovery
- Helpful error messages

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_exception_handling
./examples/example_exception_handling
```

## What You'll Learn

1. **How to catch GPIO errors**:
```cpp
try {
    pinMode(999, OUTPUT);  // Invalid pin
} catch (const InvalidPinError& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

2. **Different exception types**:
   - `InvalidPinError` - Bad pin numbers
   - `GpioAccessError` - Permission/hardware issues
   - `PinError` - Base class (catches all)

3. **Safe resource cleanup**:
```cpp
Pin led(18, OUTPUT);
try {
    led.write(HIGH);
    // ... operations ...
} catch (...) {
    // Pin automatically cleaned up by destructor
}
```

## Common Exceptions

| Exception | Cause | Solution |
|-----------|-------|----------|
| `InvalidPinError` | Pin < 0 or > 27 | Use valid BCM GPIO number |
| `GpioAccessError` | Permission denied | Add user to `gpio` group |
| `GpioAccessError` | Pin in use | Check for conflicting processes |
| `PinError` | General GPIO error | Check wiring, permissions |

## Best Practices

✅ **DO:**
- Always use try-catch for GPIO operations
- Catch specific exception types when possible
- Log errors for debugging
- Provide helpful error messages to users

❌ **DON'T:**
- Ignore exceptions (`catch (...) { }` with no handling)
- Catch exceptions you can't handle
- Let GPIO resources leak on errors

## References

- [PiPinPP Exception Hierarchy](../../docs/API_REFERENCE.md#exceptions)
- [C++ Exception Handling](https://en.cppreference.com/w/cpp/language/exceptions)

## License

Part of the PiPinPP project.
