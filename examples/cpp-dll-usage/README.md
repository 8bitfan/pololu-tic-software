# C++ DLL Usage Example

This example demonstrates how to use the Pololu Tic library as a C++ DLL/shared library.

## What This Example Shows

- Enumerating connected Tic devices
- Opening a connection to a device
- Reading device information and firmware version
- Getting current status variables (position, velocity, voltage, etc.)
- Controlling the motor (setting target position)
- Using both the C++ wrapper API and the C API
- Proper error handling with exceptions

## Building the Example

### Prerequisites

1. Install the libpololu-tic library (see [../../BUILDING.md](../../BUILDING.md))
2. Make sure the library is installed to a system location or that `PKG_CONFIG_PATH` is set correctly

### Build with CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Build Manually

**Linux/macOS:**
```bash
g++ -std=c++11 example.cpp -lpololu-tic-1 -o example
```

**Windows (MinGW):**
```bash
g++ -std=c++11 example.cpp -lpololu-tic-1 -o example.exe
```

**Windows (Visual Studio):**
```bash
cl /std:c++11 /EHsc example.cpp pololu-tic-1.lib
```

## Running the Example

### Linux/macOS

```bash
./example
```

If you get a library loading error, make sure the library is in your library path:

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH  # Linux
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH  # macOS
./example
```

### Windows

Make sure `pololu-tic-1.dll` is in the same directory as the executable or in your PATH.

```cmd
example.exe
```

## Expected Output

If no Tic devices are connected:
```
Pololu Tic C++ DLL/Shared Library Example
==========================================

=== Example: Enumerate Devices and Read Status ===
No Tic devices found.
Please connect a Tic device and try again.
```

If a Tic device is connected:
```
Pololu Tic C++ DLL/Shared Library Example
==========================================

=== Example: Enumerate Devices and Read Status ===
Found 1 Tic device(s).

Device Information:
  Name: Pololu Tic T825 Stepper Motor Controller
  Short Name: T825
  Serial Number: 00123456
  Firmware Version: 1.06

Current Status:
  Operation State: 2
  Energized: Yes
  Position Uncertain: No
  Current Position: 0
  Target Position: 0
  Current Velocity: 0
  Target Velocity: 0
  VIN Voltage: 12.5 V
  Uptime: 12345 ms
  No Errors

...
```

## Code Structure

The example is organized into several functions:

1. **display_device_info()** - Shows device name, serial number, and firmware version
2. **display_status()** - Shows current motor position, velocity, voltage, and errors
3. **example_enumerate_and_read_status()** - Basic device discovery and status reading
4. **example_motor_control()** - Demonstrates motor movement (requires user confirmation)
5. **example_c_api()** - Shows how to use the C API directly from C++

## Important Notes

- The motor control example requires user confirmation to prevent unexpected movement
- Make sure the motor can safely move before running the motor control example
- The library uses RAII, so resources are automatically cleaned up
- Errors are reported through exceptions when using the C++ API
- You can also use the C API directly if you prefer manual error handling

## Further Reading

- [DLL_USAGE.md](../../DLL_USAGE.md) - Complete DLL usage guide
- [include/tic.hpp](../../include/tic.hpp) - C++ API reference
- [include/tic.h](../../include/tic.h) - C API reference
- [Tic User's Guide](https://www.pololu.com/docs/0J71) - Official documentation
